/* The game, graphics: sprites, surfaces, palettes, fonts: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00407f90 auto */
int __cdecl rect_width(RECT *rect)

{
  return rect->right - rect->left;
}

/* 00407fc0 auto */
int __cdecl rect_height(RECT *rect)

{
  return rect->bottom - rect->top;
}

/* 004083f0 auto */
void DibSurface::release()

{
  create(0,0);
  return;
}

/* 00408680 auto */
void __cdecl rect_set(RECT *rect,int left,int top,int right,int bottom)

{
  SetRect(rect,left,top,right,bottom);
  return;
}

/* 004086c0 auto */
void __cdecl rect_set_xywh(RECT *rect,int x,int y,int width,int height)

{
  SetRect(rect,x,y,width + x,height + y);
  return;
}

/* 00408700 auto */
void __cdecl gfx_target_frame_rect(RECT *rect,i32 color)

{
  gfx_draw_box_ltrb(gfx_draw_target,rect->left,rect->top,
                     rect->right - 1,rect->bottom - 1,color);
  return;
}

/* 00408750 auto */
void __cdecl gfx_fill_draw_target(i32 color)

{
  gfx_fill_surface(gfx_draw_target,color);
  return;
}

/* 00408780 auto */
void __cdecl
gfx_target_fill_rect(i32 x,i32 y,i32 width,int height,
            i32 color)

{
  gfx_fill_rect_xywh(gfx_draw_target,x,y,width,height,color);
  return;
}

/* 0040ef70 auto */
u32 GameFont::font_cached_height()

{
  return cached_height;
}

/* 0040efa0 auto */
/* Library Function - Single Match
    public: __thiscall _Timevec::~_Timevec(void)
   
   Library: Visual Studio 1998 Debug */

i32 GameFont::line_spacing()

{
  i32 spacing;
  
  spacing = font_handle_line_spacing(hfont);
  return spacing;
}

/* 0040efd0 auto */
i32 GameFont::text_width(const char *text)

{
  i32 width;
  
  width = font_measure_width(hfont,text);
  return width;
}

/* 0040f010 auto */
DibSurface *DibSurface::surface_delete(uint flags)

{
  surface_dtor(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 0040fdb0 auto */
void __cdecl gfx_fill_rect(DibSurface *surface,RECT *rect,i32 color)

{
  iptr height;
  i32 width;
  
  height = rect_height(rect);
  width = rect_width(rect);
  gfx_fill_rect_xywh(surface,rect->left,rect->top,width,height,color
                    );
  return;
}

/* 00417ef0 auto */
void GameFont::set(i32 face,int size)

{
  u32 handle;
  
  if (hfont != 0) {
    font_handle_destroy(hfont);
  }
  handle = font_handle_create(face,size,0);
  hfont = handle;
  handle = font_measure_height(hfont);
  cached_height = handle;
  return;
}

/* 00417f70 auto */
u8 Sprite::get_key_color()

{
  return key_color;
}

/* 00418cb0 auto */
u32 GameFont::get_hfont()

{
  return hfont;
}

/* 0043c460 auto */
GameFont *GameFont::init(i32 face,i32 size)

{
  u32 handle;
  
  handle = font_handle_create(face,size,0);
  hfont = handle;
  handle = font_measure_height(hfont);
  cached_height = handle;
  return this;
}

/* 0043c4c0 auto */
GameFont *GameFont::init_styled(i32 face,i32 size,i32 style)

{
  u32 handle;
  
  handle = font_handle_create(face,size,style);
  hfont = handle;
  handle = font_measure_height(hfont);
  cached_height = handle;
  return this;
}

/* 0043c520 auto */
GameFont *GameFont::destroy()

{
  GameFont *result;

  result = this;
  if (hfont != 0) {
    result = (GameFont *)font_handle_destroy(hfont);
  }
  return result;
}

/* 0043c560 auto */
/* Library Function - Single Match
    public: class CView * __thiscall COleClientItem::GetActiveView(void)const 
   
   Library: Visual Studio 1998 Debug */

u32 DibSurface::surface_height()

{
  return pixel_height;
}

/* 0043c590 auto */
/* Library Function - Single Match
    public: class CView * __thiscall COleClientItem::GetActiveView(void)const 
   
   Library: Visual Studio 1998 Debug */

u32 DibSurface::surface_width()

{
  return pixel_width;
}

/* 0043c690 auto */
GameFont *GameFont::init_empty()

{
  hfont = 0;
  return this;
}

/* 0043c6c0 auto */
void GameFont::set_styled(i32 face,i32 size,i32 style)

{
  u32 handle;
  
  if (hfont != 0) {
    font_handle_destroy(hfont);
  }
  handle = font_handle_create(face,size,style);
  hfont = handle;
  handle = font_measure_height(hfont);
  cached_height = handle;
  return;
}

/* 0043c740 auto */
GamePalette *GamePalette::scalar_destruct(uint flags)

{
  destroy();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 0043c790 auto */
void __cdecl rect_offset(RECT *rect,int dx,int dy)

{
  OffsetRect(rect,dx,dy);
  return;
}

/* 0043c7c0 auto */
void __cdecl gfx_draw_rect_frame(DibSurface *surface,RECT *rect,i32 color)

{
  gfx_draw_box_ltrb(surface,rect->left,rect->top,
                     rect->right - 1,rect->bottom - 1,color);
  return;
}

/* 0043c8d0 auto */
i32 __cdecl gfx_draw_text(const char *text,i32 x,int y)

{
  i32 end_x;
  
  end_x = text_draw_shadowed(text_style.surface,text,x,y);
  return end_x;
}

/* 0043c910 auto */
void __cdecl gfx_draw_text_centered(const char *text,i32 x,int y,i32 width)

{
  text_draw_centered(text_style.surface,text,x,y,width);
  return;
}

/* 0043c950 auto */
i32 __cdecl
gfx_draw_text_right(const char *text,i32 right_x,i32 y,i32 flags)

{
  i32 result;
  
  result = text_draw_right_aligned(text_style.surface,text,right_x,y,flags);
  return result;
}

/* 0043cbb0 auto */
Sprite *Sprite::scalar_destruct(uint flags)

{
  sprite_destroy(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 00447f40 auto */
void __cdecl sprite_editor_tool_sprites_global_init(void)

{
  sprite_editor_tool_sprites_static_init();
  sprite_editor_tool_sprites_reg_atexit();
  return;
}

/* 00447f5a auto */
void __cdecl sprite_editor_tool_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites),0x3c,0xf,sprite_init,sprite_destroy);
  return;
}

/* 00447f82 auto */
void __cdecl sprite_editor_tool_sprites_reg_atexit(void)

{
  _atexit(sprite_editor_tool_sprites_static_dtor);
  return;
}

/* 00447f9f auto */
void __cdecl sprite_editor_tool_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites),0x3c,0xf,sprite_destroy);
  return;
}

/* 004483f8 auto */
void __cdecl small_citizen_sprites_global_init(void)

{
  small_citizen_sprites_static_init();
  small_citizen_sprites_reg_atexit();
  return;
}

/* 00448412 auto */
void __cdecl small_citizen_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&government_sprites_block.small_citizen_sprites),0x3c,0xb,sprite_init,sprite_destroy);
  return;
}

/* 0044843a auto */
void __cdecl small_citizen_sprites_reg_atexit(void)

{
  _atexit(small_citizen_sprites_static_dtor);
  return;
}

/* 00448457 auto */
void __cdecl small_citizen_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&government_sprites_block.small_citizen_sprites),0x3c,0xb,sprite_destroy);
  return;
}

/* 0044857e auto */
void __cdecl small_resource_icon_sprites_global_init(void)

{
  small_resource_icon_sprites_static_init();
  small_resource_icon_sprites_reg_atexit();
  return;
}

/* 00448598 auto */
void __cdecl small_resource_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 004485c0 auto */
void __cdecl small_resource_icon_sprites_reg_atexit(void)

{
  _atexit(small_resource_icon_sprites_static_dtor);
  return;
}

/* 004485dd auto */
void __cdecl small_resource_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 00448682 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl small_unhappy_icon_sprite_global_init(void)

{
  small_unhappy_icon_sprite_static_init();
  small_unhappy_icon_sprite_reg_atexit();
  return;
}

/* 0044869c auto */
void __cdecl small_unhappy_icon_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&small_unhappy_icon_sprite));
  return;
}

/* 004486b6 auto */
void __cdecl small_unhappy_icon_sprite_reg_atexit(void)

{
  _atexit(small_unhappy_icon_sprite_static_dtor);
  return;
}

/* 004486d3 auto */
void __cdecl small_unhappy_icon_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&small_unhappy_icon_sprite));
  return;
}

/* 0044885c auto */
void __cdecl icon_misc_sprites_global_init(void)

{
  icon_misc_sprites_static_init();
  icon_misc_sprites_reg_atexit();
  return;
}

/* 00448876 auto */
void __cdecl icon_misc_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&icon_misc_sprites_block.icon_misc_sprites),0x3c,6,sprite_init,sprite_destroy);
  return;
}

/* 0044889e auto */
void __cdecl icon_misc_sprites_reg_atexit(void)

{
  _atexit(icon_misc_sprites_static_dtor);
  return;
}

/* 004488bb auto */
void __cdecl icon_misc_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&icon_misc_sprites_block.icon_misc_sprites),0x3c,6,sprite_destroy);
  return;
}

/* 004488de auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl icons_unused_tall_sprite_global_init(void)

{
  icons_unused_tall_sprite_static_init();
  icons_unused_tall_sprite_reg_atexit();
  return;
}

/* 004488f8 auto */
void __cdecl icons_unused_tall_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&icons_unused_tall_sprite));
  return;
}

/* 00448912 auto */
void __cdecl icons_unused_tall_sprite_reg_atexit(void)

{
  _atexit(icons_unused_tall_sprite_static_dtor);
  return;
}

/* 0044892f auto */
void __cdecl icons_unused_tall_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&icons_unused_tall_sprite));
  return;
}

/* 00448b3a auto */
void __cdecl icons_pair_sprites_global_init(void)

{
  icons_pair_sprites_static_init();
  icons_pair_sprites_reg_atexit();
  return;
}

/* 00448b54 auto */
void __cdecl icons_pair_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&icons_pair_sprites_block.icons_pair_sprites),0x3c,4,sprite_init,sprite_destroy);
  return;
}

/* 00448b7c auto */
void __cdecl icons_pair_sprites_reg_atexit(void)

{
  _atexit(icons_pair_sprites_static_dtor);
  return;
}

/* 00448b99 auto */
void __cdecl icons_pair_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&icons_pair_sprites_block.icons_pair_sprites),0x3c,4,sprite_destroy);
  return;
}

/* 00448f27 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl gfx_sheet_surface_global_init(void)

{
  gfx_sheet_surface_static_init();
  gfx_sheet_surface_reg_atexit();
  return;
}

/* 00448f41 auto */
void __cdecl gfx_sheet_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface));
  return;
}

/* 00448f5b auto */
void __cdecl gfx_sheet_surface_reg_atexit(void)

{
  _atexit(gfx_sheet_surface_static_dtor);
  return;
}

/* 00448f78 auto */
void __cdecl gfx_sheet_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface));
  return;
}

/* 00449030 auto */
void __cdecl gfx_load_terrain_sprites(void)

{
  int loaded;
  i8 grab_rect_d[16];
  i8 grab_rect_c[16];
  i8 grab_rect_b[16];
  i8 grab_rect_a[16];
  int coast_idx;
  Sprite blank_sprite;
  int base_y;
  int quarter;
  i32 res2_x;
  int y2;
  i32 res_x;
  int x2;
  int base_x;
  int terrain2;
  uint i;
  Sprite dither_mask;
  
  sprite_init(&dither_mask);
  ON_SCOPE_EXIT(sprite_destroy(&dither_mask));
  sprite_init(&blank_sprite);
  ON_SCOPE_EXIT(sprite_destroy(&blank_sprite));
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_terrain1_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
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
    ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->free();
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->free();
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->free();
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c)),x2,y2);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78)),res_x,y2);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78)),res2_x,y2);
    y2 += 0x21;
  }
  x2 = 0x1c8;
  y2 = 100;
  for (i = 0; (int)i < 3; i++) {
    ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c)),x2,y2);
    y2 += 0x21;
  }
  ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&pollution_sprite_block.pollution_sprite,x2,y2);
  y2 += 0x21;
  ((Sprite *)((i8 *)&grassland_shield_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&grassland_shield_sprite,x2,y2);
  y2 += 0x21;
  ((Sprite *)((i8 *)&hut_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&hut_sprite,x2,y2);
  y2 = 0x16c;
  x2 = 1;
  for (i = 0; (int)i < 9; i++) {
    ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c)),x2,y2);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c)),x2,y2 + 0x21);
    x2 += 0x41;
  }
  dither_mask.free();
  sprite_grab_tile_cell(&dither_mask,1,0x1bf);
  ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite,0x42,0x1bf);
  ((Sprite *)((i8 *)&unexplored_tile_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&unexplored_tile_sprite,0x83,0x1bf);
  ((Sprite *)((i8 *)&tile_direction_mask_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&tile_direction_mask_sprite,0x105,0x1bf);
  blank_sprite.free();
  sprite_grab_tile_cell(&blank_sprite,0xc4,0x1bf);
  for (terrain2 = 0; terrain2 < 0xd; terrain2++) {
    gfx_fill_rect_xywh(&gfx_sheet_surface_block.gfx_sheet_surface,0,0,0x40,0x20,7);
    if (terrain2 < 0xb) {
      ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->draw((RECT *)grab_rect_a,&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    else if (terrain2 == 0xb) {
      ((Sprite *)((i8 *)&unexplored_tile_sprite))->draw((RECT *)grab_rect_b,&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    else {
      blank_sprite.draw((RECT *)grab_rect_c,&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    dither_mask.draw_keyed((RECT *)grab_rect_d,&gfx_sheet_surface_block.gfx_sheet_surface,10,0,0);
    for (quarter = 0; quarter < 4; quarter++) {
      if (quarter < 2) {
        x2 = 0x20;
      }
      else {
        x2 = 0;
      }
      if ((quarter < 1) || (2 < quarter)) {
        y2 = 0;
      }
      else {
        y2 = 0x10;
      }
      ((Sprite *)(((i8 *)&map_cursor_sprite_block.terrain_dither_sprites) + quarter * 0x3c + terrain2 * 0xf0))->free();
      sprite_grab_from_sheet(((Sprite *)(((i8 *)&map_cursor_sprite_block.terrain_dither_sprites) + terrain2 * 0xf0 + quarter * 0x3c)),x2,y2,0x20,
                   0x10);
    }
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_terrain2_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
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
    x2 = (i & 7) * 0x41;
    y2 = ((int)i >> 3) * 0x21;
    ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c)),base_x + x2,base_y + y2);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c)),base_x + x2,
                 base_y + y2 + 0x42);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c)),base_x + x2,
                 base_y + y2 + 0x84);
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c)),base_x + x2,
                 base_y + y2 + 0xc6);
  }
  base_x = 1;
  base_y = 0x1ad;
  for (coast_idx = 0; coast_idx < 8; coast_idx++) {
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0)),base_x,base_y,0x20,0x10);
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0)),base_x + 0x21,base_y + 0x22,0x20,0x10);
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0)),base_x,base_y + 0x11,0x20,0x10);
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0)),base_x,base_y + 0x22,0x20,0x10);
    base_x += 0x42;
  }
  base_x = 1;
  base_y = 0x14b;
  for (i = 0; (int)i < 4; i++) {
    ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell(((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c)),base_x,base_y);
    base_x += 0x41;
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  return;
}

/* 0044999c auto */
void __cdecl sprite_grab_tile_cell(Sprite *sprite,i32 x,i32 y)

{
  (sprite)->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x40,0x20);
  (sprite)->replace_color(9,7);
  return;
}

/* 004499d3 auto */
void __cdecl
sprite_grab_from_sheet(Sprite *sprite,i32 x,i32 y,i32 width,
            i32 height)

{
  (sprite)->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,width,height);
  (sprite)->replace_color(9,7);
  return;
}

/* Part of gfx_load_icon_sprites (00449a0e): ICONS.BMP (or ICONS.GIF): map cursor
   and grid, unit and city icons, resource and tax icons, explosions, window
   textures, improvement, wonder and advance icons, caption buttons. */
static void gfx_load_icons_sheet(void)
{
  int loaded;
  int col;
  int k;
  int y2;
  int x2;
  int j;
  int i;

  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_icons_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_icons_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_icons_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->free();
  ((Sprite *)((i8 *)&map_grid_sprite))->free();
  ((Sprite *)((i8 *)&icons_pair_sprites_block.city_radius_grid_sprite))->free();
  sprite_grab_tile_cell((Sprite *)&map_cursor_sprite_block.map_cursor_sprite,199,0x100);
  sprite_grab_tile_cell((Sprite *)&map_grid_sprite,199,0x121);
  sprite_grab_tile_cell((Sprite *)&icons_pair_sprites_block.city_radius_grid_sprite,0xf8,0x1ae);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_alt_sprite))->free();
  gfx_grab_fortify_overlay(1,0x20,0xbb);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_6))->free();
  sprite_grab_icon_cell_keyed(&fortify_overlay_sprites_block.icon_sprite_6,0x22,0xdf);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.pollution_icon_sprite))->free();
  sprite_grab_from_sheet(&fortify_overlay_sprites_block.pollution_icon_sprite,0x43,0xdf,0x20,0x20);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_7))->free();
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_8))->free();
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.city_disorder_icon_sprite))->free();
  sprite_grab_icon_cell_keyed(&fortify_overlay_sprites_block.icon_sprite_7,100,0xdf);
  sprite_grab_icon_cell_keyed(&fortify_overlay_sprites_block.icon_sprite_8,0x85,0xdf);
  sprite_grab_from_sheet((Sprite *)&fortify_overlay_sprites_block.city_disorder_icon_sprite,0xa6,0xdf,0x1e,0x1e);
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_misc_sprites))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_1))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_4))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_5))->free();
  sprite_grab_icon_cell_keyed(&icon_misc_sprites_block.icon_misc_sprites,199,0x100);
  sprite_grab_icon_cell_keyed(&icon_misc_sprites_block.icon_sprite_1,0xe8,0x100);
  sprite_grab_icon_cell_keyed(&icon_misc_sprites_block.icon_sprite_4,0x109,0x100);
  sprite_grab_icon_cell_keyed(&icon_misc_sprites_block.icon_sprite_5,0x12a,0x100);
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_2))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_3))->free();
  ((Sprite *)((i8 *)&icons_unused_tall_sprite))->free();
  sprite_grab_from_sheet(&icon_misc_sprites_block.icon_sprite_2,0x137,1,0x1f,0x60);
  sprite_grab_from_sheet(&icon_misc_sprites_block.icon_sprite_3,0x137,0x62,0x1f,0x60);
  sprite_grab_from_sheet((Sprite *)&icons_unused_tall_sprite,0x109,0x121,0x38,0x83);
  x2 = 1;
  y2 = 0x122;
  for (i = 0; i < 3; i++) {
    for (k = 0; k < 2; k++) {
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + i * 0x78))->free();
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78))->free();
      sprite_grab_square(((Sprite *)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + i * 0x78)),x2,y2,0xe);
      sprite_grab_square(((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78)),x2,y2 + 0xf,0xe);
    }
    x2 += 0xf;
  }
  x2 = 1;
  y2 = 0x140;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square(((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites) + i * 0x3c)),x2,y2,0xe);
    x2 += 0xf;
  }
  x2 = 0x31;
  y2 = 0x14e;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square(((Sprite *)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + i * 0x3c)),x2,y2,10);
    x2 += 0xb;
  }
  x2 = 0x31;
  y2 = 0x159;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square(((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites) + i * 0x3c)),x2,y2,10);
    x2 += 0xb;
  }
  ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->free();
  ((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite))->free();
  sprite_grab_square((Sprite *)&small_unhappy_icon_sprite,0x52,0x14e,10);
  sprite_grab_square((Sprite *)&unhappy_suppressed_icon_sprite,0x52,0x159,10);
  x2 = 0x31;
  y2 = 0x122;
  for (i = 0; i < 4; i++) {
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + i * 0x3c))->free();
    sprite_grab_square(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + i * 0x3c)),x2,y2,0xe);
    sprite_grab_square(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + i * 0x3c)),x2,0x131,0xe);
    x2 += 0xf;
  }
  x2 = 1;
  y2 = 0x164;
  for (i = 0; i < 8; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c)),x2,y2,0x20,0x20);
    x2 += 0x21;
  }
  ((DibSurface *)((i8 *)&window_border_tile_surface))->DibSurface::create(0x40,0x20);
  gfx_copy_rect_xywh(&gfx_sheet_surface_block.gfx_sheet_surface,&window_border_tile_surface,199,0x142,0,0,0x40,0x20);
  dialog_set_frame_tile_surface(&window_border_tile_surface);
  frame_set_border_tile(&window_border_tile_surface);
  ((DibSurface *)((i8 *)&panel_texture_surface))->DibSurface::create(0x20,0x20);
  gfx_copy_rect_xywh(&gfx_sheet_surface_block.gfx_sheet_surface,&panel_texture_surface,0x12a,0xbe,0,0,0x20,0x20);
  (*(i8 * *)&dlg_style.client_tile_surface) = ((i8 *)&panel_texture_surface);
  x2 = 0x157;
  y2 = 1;
  col = 0;
  for (i = 1; i < 0x27; i++) {
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c)),x2,y2,0x24,0x14);
    x2 += 0x25;
    col++;
    if (7 < col) {
      col = 0;
      y2 += 0x15;
      x2 = 0x157;
    }
  }
  x2 = 0x157;
  y2 = 0x6a;
  col = 0;
  for (i = 0; i < 0x1c; i++) {
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c)),x2,y2,0x24,0x14);
    x2 += 0x25;
    col++;
    if (6 < col) {
      col = 0;
      y2 += 0x15;
      x2 = 0x157;
    }
  }
  y2 = 0xd3;
  for (i = 0; i < 4; i++) {
    x2 = 0x157;
    for (j = 0; j < 5; j++) {
      ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + j * 0xf0 + i * 0x3c))->free();
      sprite_grab_from_sheet(((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + i * 0x3c + j * 0xf0)),x2,y2,0x24,
                   0x14);
      x2 += 0x25;
    }
    y2 += 0x15;
  }
  ((Sprite *)((i8 *)&icons_pair_sprites_block.icons_pair_sprites))->free();
  ((Sprite *)((i8 *)&icons_pair_sprites_block.icon_sprite_9))->free();
  sprite_grab_from_sheet(&icons_pair_sprites_block.icons_pair_sprites,0x15e,0xff,0x40,0x20);
  sprite_grab_from_sheet(&icons_pair_sprites_block.icon_sprite_9,0x15e,0x120,0x40,0x20);
  ((Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite))->free();
  ((Sprite *)((i8 *)&caption_zoom_out_sprite))->free();
  ((Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite))->free();
  ((Sprite *)((i8 *)&caption_view_mode_button_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&caption_close_button_sprite_block.caption_close_button_sprite,1,0x185,0x10,0x10);
  sprite_grab_from_sheet((Sprite *)&caption_zoom_out_sprite,0x12,0x185,0x10,0x10);
  sprite_grab_from_sheet((Sprite *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,0x23,0x185,0x10,0x10);
  sprite_grab_from_sheet((Sprite *)&caption_view_mode_button_sprite,0x34,0x185,0x10,0x10);
  ((Sprite *)((i8 *)&scroll_up_button_sprite))->free();
  ((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&scroll_up_button_sprite,0xe3,0x185,0x12,0x18);
  sprite_grab_from_sheet((Sprite *)&city_down_arrow_sprite_block.city_down_arrow_sprite,0xf6,0x185,0x12,0x18);
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
}

/* Part of gfx_load_icon_sprites (00449a0e): EDITORPT.GIF: the sprite editor's
   tools and backdrops. */
static void gfx_load_editor_tool_sprites(void)
{
  int loaded;
  int y2;
  int x2;
  int i;

  __chdir(((i8 *)&game_dirs.home));
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_editorpt_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded != 0) {
    x2 = 1;
    y2 = 0x1bf;
    for (i = 0; i < 0xf; i++) {
      ((Sprite *)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + i * 0x3c))->free();
      sprite_grab_from_sheet(((Sprite *)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + i * 0x3c)),x2,y2,0x20,0x20);
      x2 += 0x21;
    }
  }
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->free();
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,0x23f,1,0x40,0x30);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->free();
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,0x23f,0x32,0x40,0x20);
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
}

/* Part of gfx_load_icon_sprites (00449a0e): PEOPLE.BMP (or PEOPLE.GIF): the
   citizens of the four eras and the small citizens. */
static void gfx_load_people_sheet(void)
{
  int loaded;
  int y2;
  int x2;
  int i;
  int row;

  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_people_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_people_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_people_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  y2 = 6;
  for (row = 0; row < 4; row++) {
    x2 = 2;
    for (i = 0; i < 0xb; i++) {
      ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + row * 0x294 + i * 0x3c))->free();
      sprite_grab_from_sheet(((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + i * 0x3c + row * 0x294)),x2,y2,0x1b
                   ,0x1e);
      x2 += 0x1c;
    }
    y2 += 0x1f;
  }
  x2 = 2;
  y2 = 0x89;
  for (i = 0; i < 0xb; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + i * 0x3c)),x2,y2,0xf,0x1a);
    x2 += 0x10;
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
}

/* Part of gfx_load_icon_sprites (00449a0e): the resource sheet 0x56:
   government pictures, treaty icons, popup icons and the civ portraits in
   each civ's colour. palette: the caller's (the sheet's palette). */
static void gfx_load_diplomacy_sheet(GamePalette &palette)
{
  int loaded;
  int y2;
  int x2;
  int civ;

  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x56,10,0xc0,&palette);
  if (loaded == 0) {
    __chdir(((i8 *)&game_dirs.home));
    ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x56,10,0xc0,&palette);
  }
  __chdir(((i8 *)&game_dirs.home));
  x2 = 2;
  y2 = 0x44;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprites))->free();
  ((Sprite *)((i8 *)&government_sprites_block.government_despotism_sprite))->free();
  sprite_grab_from_sheet(&government_sprites_block.government_sprites,x2,y2,0x22,0x22);
  sprite_grab_from_sheet(&government_sprites_block.government_despotism_sprite,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_communism))->free();
  sprite_grab_from_sheet(&government_sprites_block.government_sprite_communism,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_fundamentalism))->free();
  sprite_grab_from_sheet(&government_sprites_block.government_sprite_fundamentalism,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_monarchy_sprite))->free();
  sprite_grab_from_sheet(&government_sprites_block.government_monarchy_sprite,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_democracy))->free();
  sprite_grab_from_sheet((Sprite *)&government_sprites_block.government_sprite_democracy,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_republic))->free();
  sprite_grab_from_sheet(&government_sprites_block.government_sprite_republic,x2,y2,0x22,0x22);
  x2 = 2;
  y2 = 200;
  ((Sprite *)((i8 *)&ceasefire_icon_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&ceasefire_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&war_icon_sprite_block.war_icon_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&war_icon_sprite_block.war_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&signal_allied_icon_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&signal_allied_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&diplo_popup_portrait_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&diplo_popup_portrait_sprite,x2,y2,0x40,0x40);
  ((Sprite *)((i8 *)&popup_info_icon_sprite))->free();
  ((Sprite *)((i8 *)&tutorial_icon_sprite_block.tutorial_icon_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&popup_info_icon_sprite,0x18e,0x86,0x40,0x40);
  sprite_grab_from_sheet((Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0x86,0x18e,0x40,0x40);
  for (civ = 0; civ < 8; civ++) {
    ((Sprite *)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c)),2,0x18e,0x40,0x40);
    ((Sprite *)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c))->replace_color(8,
               civ_primary_colors_block.civ_colors[civ].primary);
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
}

/* 00449a0e auto */
void __cdecl gfx_load_icon_sprites(void)

{
  int loaded;
  int frame_count;
  GamePalette palette;
  int y2;
  int x2;
  int i;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x55,10,0xc0,&palette);
  if (loaded == 0) {
    __chdir(((i8 *)&game_dirs.home));
    ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x55,10,0xc0,&palette);
  }
  __chdir(((i8 *)&game_dirs.home));
  x2 = 1;
  y2 = 1;
  frame_count = 0;
  for (i = 0; i < 0xb; i++) {
    ((Sprite *)(((i8 *)&hill_sprites_block.nuke_anim_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&hill_sprites_block.nuke_anim_sprites) + i * 0x3c)),x2,y2,0x5b,0x48);
    x2 += 0x5c;
    frame_count++;
    if (frame_count == 6) {
      x2 = 1;
      y2 += 0x49;
    }
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  gfx_load_icons_sheet();
  gfx_load_editor_tool_sprites();
  gfx_load_people_sheet();
  gfx_load_diplomacy_sheet(palette);
  return;
}

/* 0044ab72 auto */
void __cdecl sprite_grab_icon_cell_keyed(Sprite *sprite,i32 x,i32 y)

{
  (sprite)->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x20,0x20);
  (sprite)->replace_color(9,7);
  return;
}

/* 0044aba9 auto */
void __cdecl sprite_grab_square(Sprite *sprite,i32 x,i32 y,i32 size)

{
  sprite_grab_from_sheet(sprite,x,y,size,size);
  return;
}

/* 0044abd5 auto */
void __cdecl gfx_grab_fortify_overlay(int index,i32 x,i32 y)

{
  sprite_grab_city_cell(((Sprite *)(((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites) + index * 0x3c)),x,y);
  return;
}

/* 0044ac07 auto */
void __cdecl sprite_grab_city_cell(Sprite *sprite,i32 x,i32 y)

{
  (sprite)->grab_from_surface(&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x40,0x30);
  (sprite)->replace_color(9,7);
  return;
}

/* 0044ae4c auto */
void __cdecl gfx_load_city_sprites(void)

{
  int cell_x;
  i32 color;
  int style2;
  int civ;
  int y2;
  int flag_x;
  int size_idx;
  
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  cell_x = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_cities_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (cell_x == 0) {
    cell_x = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_cities_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (cell_x == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_cities_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  y2 = 0x27;
  for (style2 = 0; style2 < 6; style2++) {
    for (size_idx = 0; size_idx < 4; size_idx++) {
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->free();
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->free();
      cell_x = size_idx * 0x41;
      sprite_grab_city_cell(((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + style2 * 0x1e0 + size_idx * 0x78)),cell_x + 1,y2);
      city_sprite_scan_flag_offset(style2,size_idx,0,cell_x + 1,y2);
      city_sprite_scan_size_box_offset(style2,size_idx,0,cell_x + 1,y2);
      sprite_grab_city_cell(((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + style2 * 0x1e0 + size_idx * 0x78)),cell_x + 0x14e,y2)
      ;
      city_sprite_scan_flag_offset(style2,size_idx,1,cell_x + 0x14e,y2);
      city_sprite_scan_size_box_offset(style2,size_idx,1,cell_x + 0x14e,y2);
    }
    y2 += 0x31;
  }
  flag_x = 1;
  for (civ = 0; civ < 8; civ++) {
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->free();
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->free();
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78)),flag_x,0x1a9,0xe,0x16);
    sprite_grab_from_sheet(((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78)),flag_x,0x1c0,0xe,0x16);
    color = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(flag_x + 3,0x1ad);
    civ_primary_colors_block.civ_colors[civ].primary = color;
    color = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(flag_x + 6,0x1ad);
    civ_primary_colors_block.civ_colors[civ].mid = color;
    color = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(flag_x,0x1a7);
    civ_primary_colors_block.civ_colors[civ].secondary = color;
    flag_x += 0xf;
  }
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->free();
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->free();
  sprite_grab_city_cell(&fortify_overlay_sprites_block.fortify_overlay_sprites,0x8f,0x1a7);
  sprite_grab_city_cell(&fortify_overlay_sprites_block.fortress_sprite,0xd0,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->free();
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->free();
  sprite_grab_city_cell(&airbase_sprites_block.airbase_sprites,0x111,0x1a7);
  sprite_grab_city_cell(&airbase_sprites_block.airbase_occupied_sprite,0x152,0x1a7);
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  return;
}

/* 0044b30e auto */
void __cdecl gfx_load_unit_sprites(void)

{
  int loaded;
  int col;
  int unit_type;
  int y2;
  int x2;
  
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480(&gfx_sheet_surface_block.gfx_sheet_surface,str_units_bmp,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_units_gif,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_units_gif_2,10,0xc0,(GamePalette *)&game_palette_block.game_palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  col = 0;
  y2 = 1;
  x2 = 1;
  for (unit_type = 0; unit_type < 0x3f; unit_type++) {
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->free();
    sprite_grab_city_cell(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),x2,y2);
    unit_sprite_scan_shield_offset(unit_type,x2,y2);
    col++;
    x2 += 0x41;
    if (8 < col) {
      col = 0;
      y2 += 0x31;
      x2 = 1;
    }
  }
  ((Sprite *)((i8 *)&unit_shield_sprite))->free();
  sprite_grab_from_sheet((Sprite *)&unit_shield_sprite,0x24a,1,0xc,0x14);
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  return;
}

/* 0044b49e auto */
void __cdecl gfx_load_game_sprites(void)

{
  SurfaceWindow surf_wnd2;
  
  surf_wnd2.SurfaceWindow::construct();
  ON_SCOPE_EXIT(surf_wnd2.SurfaceWindow::destruct());
  gfx_load_city_sprites();
  gfx_load_icon_sprites();
  gfx_load_terrain_sprites();
  gfx_load_unit_sprites();
  return;
}

/* 00451830 auto */
i32 Sprite::get_width()

{
  i32 width;
  
  width = rect_width((RECT *)&bounds_rect);
  return width;
}

/* 00451860 auto */
i32 Sprite::get_height()

{
  i32 height;
  
  height = rect_height((RECT *)&bounds_rect);
  return height;
}

/* 0046e9d0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl grey_palette_global_init(void)

{
  grey_palette_static_init();
  grey_palette_reg_atexit();
  return;
}

/* 0046e9ea auto */
void __cdecl grey_palette_static_init(void)

{
  ((GamePalette *)((i8 *)&grey_palette_block.grey_palette))->init();
  return;
}

/* 0046ea04 auto */
void __cdecl grey_palette_reg_atexit(void)

{
  _atexit(grey_palette_atexit_dtor);
  return;
}

/* 0046ea21 auto */
void __cdecl grey_palette_atexit_dtor(void)

{
  ((GamePalette *)((i8 *)&grey_palette_block.grey_palette))->destroy();
  return;
}

/* 0046ea3b auto */
void __cdecl gfx_build_grey_remap(void)

{
  int rgb_handle;
  iptr rgb_data;
  int handle;
  LOGPALETTE *plpal;
  HPALETTE h;
  iptr full_rgb;
  iptr remap_table;
  UINT nearest_index;
  iptr offset;
  
  rgb_handle = mem_handle_alloc(0x2c4);
  if (rgb_handle == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_handle_fullcolors,(const char*)&str_d_ss_franklinton_grey_cpp,0x22);
  }
  else {
    rgb_data = (iptr)mem_handle_lock(rgb_handle);
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb((u8 *)rgb_data,10,0x20);
    handle = mem_handle_alloc(0x88);
    if (handle == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_hlocpal,(const char*)&str_d_ss_franklinton_grey_cpp_2,0x2a);
      mem_handle_unlock(rgb_handle);
      mem_handle_free(rgb_handle);
    }
    else {
      plpal = (LOGPALETTE *)mem_handle_lock(handle);
      plpal->palVersion = 0x300;
      plpal->palNumEntries = 0x20;
      for (offset = 0; offset < 0x60; offset += 3) {
        plpal->palPalEntry[offset / 3].peRed = *(BYTE *)(offset + rgb_data);
        plpal->palPalEntry[offset / 3].peGreen = *(BYTE *)(offset + 1 + rgb_data);
        plpal->palPalEntry[offset / 3].peBlue = *(BYTE *)(offset + 2 + rgb_data);
        plpal->palPalEntry[offset / 3].peFlags = 4;
      }
      h = CreatePalette(plpal);
      if (h == (HPALETTE)0x0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_handle_palette,(const char*)&str_d_ss_franklinton_grey_cpp_3,0x3e);
        mem_handle_unlock(rgb_handle);
        mem_handle_free(rgb_handle);
      }
      else {
        mem_handle_unlock(handle);
        mem_handle_free(handle);
        handle = mem_handle_alloc(0x2c4);
        if (handle == 0) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_handle_colors,(const char*)&str_d_ss_franklinton_grey_cpp_4,0x48);
          mem_handle_unlock(rgb_handle);
          mem_handle_free(rgb_handle);
        }
        else {
          full_rgb = (iptr)mem_handle_lock(handle);
          grey_palette_block.grey_remap_handle = mem_handle_alloc(0xec);
          if (grey_palette_block.grey_remap_handle == 0) {
            ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_handle_grey,(const char*)&str_d_ss_franklinton_grey_cpp_5,0x51);
            mem_handle_unlock(rgb_handle);
            mem_handle_free(rgb_handle);
            mem_handle_unlock(handle);
            mem_handle_free(handle);
          }
          else {
            remap_table = (iptr)mem_handle_lock(grey_palette_block.grey_remap_handle);
            ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb((u8 *)full_rgb,10,0xec);
            for (offset = 0; offset < 0x2c4; offset += 3) {
              nearest_index = GetNearestPaletteIndex
                                (h,RGB(*(i8 *)(offset + full_rgb),*(i8 *)(offset + 1 + full_rgb),
                                     *(i8 *)(offset + 2 + full_rgb)));
              *(char *)(remap_table + offset / 3) = (char)nearest_index + '\n';
              ((GamePalette *)((i8 *)&grey_palette_block.grey_palette))->set_entry(offset / 3 + 10,
                         *(u8 *)(nearest_index * 3 + rgb_data),
                         *(u8 *)(nearest_index * 3 + 1 + rgb_data),
                         *(u8 *)(nearest_index * 3 + 2 + rgb_data));
            }
            DeleteObject(h);
            mem_handle_unlock(handle);
            mem_handle_free(handle);
            mem_handle_unlock(rgb_handle);
            mem_handle_free(rgb_handle);
            mem_handle_unlock(grey_palette_block.grey_remap_handle);
          }
        }
      }
    }
  }
  return;
}

/* 0046ee1e auto */
void __cdecl gfx_free_grey_remap(void)

{
  if (grey_palette_block.grey_remap_handle != 0) {
    grey_palette_block.grey_remap_handle = mem_handle_free(grey_palette_block.grey_remap_handle);
  }
  return;
}

/* 0046ee4e auto */
void __cdecl gfx_grey_out_windows(void)

{
  int remap_table;
  int view;
  
  remap_table = (iptr)mem_handle_lock(grey_palette_block.grey_remap_handle);
  if (remap_table != 0) {
    for (view = 0; view < 8; view++) {
      if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
        map_views_block.map_view_recs[view].remap_colors((u8 *)remap_table,10,0xec);
      }
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->remap_colors((u8 *)remap_table,10,0xec);
    ((DibSurface *)((i8 *)&minimap_state.window))->remap_colors((u8 *)remap_table,10,0xec);
    ((DibSurface *)((i8 *)&city_wnd.window))->remap_colors((u8 *)remap_table,10,0xec);
    mem_handle_unlock(grey_palette_block.grey_remap_handle);
    ui_blit_all_windows();
  }
  return;
}

/* 0046ef3f auto */
void __cdecl gfx_repaint_main_windows(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      ((SurfaceWindow *)&map_views_block.map_view_recs[view])->repaint();
    }
  }
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
  ((SurfaceWindow *)((i8 *)&city_wnd.window))->repaint();
  wnd_flush_paint();
  return;
}

/* 0046efd6 auto */
void __cdecl screen_fade_out(void)

{
  int step;
  
  blink.unit_active = 0;
  if (display_palette.palettized != 0) {
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_begin(10,0xec,10,(GamePalette *)&grey_palette_block.grey_palette);
    ui_blit_all_windows();
    step = 10;
    while (-1 < step) {
      ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_step(step);
      ui_delay_ticks(1);
      step--;
    }
    palette_set_nocollapse_all();
    gfx_grey_out_windows();
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_end();
    palette_set_nocollapse_all();
  }
  return;
}

/* 0046f06f auto */
void __cdecl screen_fade_in(void)

{
  int step;
  
  if (display_palette.palettized != 0) {
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_begin(10,0xec,10,(GamePalette *)&grey_palette_block.grey_palette);
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_step(0);
    gfx_repaint_main_windows();
    ui_blit_all_windows();
    step = 0;
    while (step < 0xb) {
      ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_step(step);
      ui_delay_ticks(1);
      step++;
    }
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_end();
    palette_set_nocollapse_all();
  }
  return;
}

/* 0046f108 auto */
void __cdecl palette_set_nocollapse_all(void)

{
  GameLogPalette *logpal;
  int i;
  
  logpal = (GameLogPalette *)((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_logpalette();
  if (logpal == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_p,(const char*)&str_d_ss_franklinton_grey_cpp_6,0xbc);
  }
  for (i = 0; i < 0x100; i++) {
    logpal->entries[i].flags = 4;
  }
  ((GamePalette *)((i8 *)&game_palette_block.game_palette))->commit_range(10,0xec);
  return;
}

/* 0046f440 auto */
GamePalette *GamePalette::get_logpalette()

{
  return this;
}

/* 0046f460 auto */
int __cdecl gfx_load_bmp_640x480(DibSurface *surface,const char *filename,int first_color,int num_colors,GamePalette *palette)

{
  char color_base;
  size_t nread;
  GamePalette *logpal;
  int tmp;
  i32 blue;
  i32 green;
  i32 red;
  char pixel;
  int info_size;
  /* (port) the BITMAPINFOHEADER after biSize, read at once: the
     original's locals lay in its order, ours need not */
  struct { i32 width2; i32 height2; i16 planes; i16 bit_count; i32 compression; i32 size_image;
           i32 x_ppm; i32 y_ppm; i32 colors_used; i32 colors_important; i8 v5_rest[84]; } info;
  FILE *file;
  int ok;
  int src_index;
  int i;
  int row;
  int col;
  uint run_value;
  char *dst;
  char *bmp_lo;
  char *bmp_hi;
  uint file_header[4];
  uint code;
  const char *error_msg;
  i8 bmp_palette[0x400];
  
  error_msg = ((i8 *)&gfx_load_bmp_no_error_str);
  ok = 0;
  file = __fsopen((char *)filename,((i8 *)&gfx_bmp_read_mode),0x40);
  if (file == (FILE *)0x0) {
    ok = 0;
  }
  else {
    nread = _fread(file_header,0xe,1,file);
    if (nread == 0) {
      error_msg = str_error_reading_file_header;
    }
    else if ((file_header[0] & 0xffff) == 0x4d42) {
      nread = _fread(&info_size,4,1,file);
      /* BF-097 (Peter Johnson's ledger): at least the 40 bytes of a
         BITMAPINFOHEADER, whose compression and colours used are read below */
      if ((nread == 0) || (nread = (info_size - 4 <= (int)sizeof info && 0x24 <= info_size - 4) && _fread(&info,info_size - 4,1,file), nread == 0)) {
        error_msg = str_error_reading_info_header;
      }
      else {
        nread = _fread(bmp_palette,0x400,1,file);
        if (nread == 0) {
          error_msg = str_error_reading_file_palette;
        }
        else if (((info.width2 == 0x280) && (info.height2 == 0x1e0)) && (info.bit_count == 8)) {
          ok = 1;
          if (palette != 0) {
            tmp = num_colors;
            if ((info.colors_used != 0) && (tmp = info.colors_used, num_colors <= info.colors_used)) {
              tmp = num_colors;
            }
            num_colors = tmp;
            if (0x100 < num_colors + first_color) {
              num_colors = 0x100 - first_color;
            }
            src_index = 0;
            for (i = first_color; i < num_colors + first_color; i++) {
              red = *(i32 *)(bmp_palette + src_index * 4);
              green = *(i32 *)(bmp_palette + src_index * 4 + 1);
              blue = *(i32 *)(bmp_palette + src_index * 4 + 2);
              tmp = i;
              logpal = (palette)->get_logpalette();
              palette_set_entry_rgb((GameLogPalette *)logpal,tmp,blue,green,red);
              src_index++;
            }
            (palette)->commit_range(first_color,num_colors);
          }
          (surface)->DibSurface::create(info.width2,info.height2);
          color_base = (char)first_color;
          if (info.compression == 0) {
            while( true ) {
              row = info.height2 - 1;
              if ((row < 0) || (ok == 0)) break;
              dst = (char *)(surface)->pixel_ptr(0,row);
              for (col = 0; info.height2 = row, col < info.width2;
                  col = col + 1) {
                code = CRT_GETC(file);
                if (code == 0xffffffff) {
                  error_msg = str_error_reading_bitmap_image;
                  ok = 0;
                  info.height2 = row;
                  break;
                }
                *dst = (char)code + color_base;
                dst++;
              }
            }
          }
          else if (info.compression == 1) {
            col = 0;  /* BF-073: the first RLE delta has no preceding X writer. */
            row = info.height2 - 1;
            /* BF-098 (Peter Johnson's ledger): the image's bytes, and rows
               inside its row table; a row outside it is no position (an end
               of line after the last row is), and a write there or past the
               image ends the load as a bad image */
            bmp_lo = (char *)(surface)->pixel_ptr(0,0);
            bmp_hi = (char *)(surface)->pixel_ptr(0,info.height2 - 1);
            if (bmp_hi < bmp_lo) {
              dst = bmp_lo;
              bmp_lo = bmp_hi;
              bmp_hi = dst;
            }
            bmp_hi += info.width2;
            dst = (char *)(surface)->pixel_ptr(0,row);
            while( true ) {
              code = CRT_GETC(file);
              if (code == 0xffffffff) break;
              if (code == 0) {
                code = CRT_GETC(file);
                if (code == 0) {
                  row--;
                  dst = (row < 0) ? (char *)0x0 : (char *)(surface)->pixel_ptr(0,row);
                }
                else {
                  if (code == 1) break;
                  if (code == 2) {
                    tmp = CRT_GETC(file);
                    row -= tmp;
                    tmp = CRT_GETC(file);
                    col += tmp;
                    dst = ((row < 0) || (info.height2 <= row)) ? (char *)0x0 :
                          (char *)(surface)->pixel_ptr(col,row);
                  }
                  else {
                    if ((dst == (char *)0x0) || (dst < bmp_lo) || (bmp_hi - dst < (int)code)) {
                      error_msg = str_error_reading_bitmap_image;
                      ok = 0;
                      break;
                    }
                    for (i = 0; i < (int)code; i++) {
                      tmp = CRT_GETC(file);
                      pixel = (char)tmp;
                      *dst = pixel + color_base;
                      dst++;
                    }
                    if ((code & 1) != 0) {
                      CRT_GETC(file);
                    }
                    col += code;
                  }
                }
              }
              else {
                run_value = CRT_GETC(file);
                if ((dst == (char *)0x0) || (dst < bmp_lo) || (bmp_hi - dst < (int)code)) {
                  error_msg = str_error_reading_bitmap_image;  /* BF-098 */
                  ok = 0;
                  break;
                }
                for (i = 0; i < (int)code; i++) {
                  *dst = color_base + (char)run_value;
                  dst++;
                }
                col += code;
              }
            }
          }
          else if (info.compression == 2) {
            ok = 0;
            error_msg = str_incompatable_compression_mode_bi_rle4;
          }
        }
        else {
          error_msg = str_image_not_640_x_480_x_256_color;
        }
      }
    }
    else {
      error_msg = str_invalid_bitmap_file;
    }
    _fclose(file);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,error_msg,(LPCSTR)0x0,0);
    }
  }
  return ok;
}

/* 0046fbf3 auto */
int __cdecl gfx_save_bmp_640x480(DibSurface *surface,const char *filename,int first_color,int num_colors,GamePalette *palette)

{
  GamePalette *logpal;
  size_t nwritten;
  int tmp;
  i8 *red_ptr;
  i8 *green_ptr;
  i8 *blue_ptr;
  i32 info_size;
  int width2;
  int height2;
  i16 planes;
  i16 bit_count;
  i32 compression;
  i32 image_size;
  i32 x_ppm;
  i32 y_ppm;
  i32 colors_used;
  i32 colors_important;
  FILE *file;
  int ok;
  int dst_index;
  int i;
  int row;
  int col;
  byte *src;
  i16 file_type;
  i32 file_size;
  i16 reserved1;
  i16 reserved2;
  i32 bits_offset;
  int pixel;
  const char *error_msg;
  i8 bmp_palette[1024];
  /* (port) the headers as the file has them: the original wrote its locals,
     which lay in this order on its stack, ours need not */
  u8 file_hdr[0xe];
  u8 info_hdr[0x28];
  
  ok = 0;
  error_msg = ((i8 *)&gfx_save_bmp_no_error_str);
  file_type = 0x4d42;
  file_size = 0x4b436;
  reserved1 = 0;
  reserved2 = 0;
  bits_offset = 0x436;
  info_size = 0x28;
  width2 = 0x280;
  height2 = 0x1e0;
  planes = 1;
  bit_count = 8;
  compression = 0;
  image_size = 0x4b000;
  x_ppm = 0;
  y_ppm = 0;
  colors_used = 0;
  colors_important = 0;
  for (i = 0; i < 0x100; i++) {
    bmp_palette[i * 4] = 0;
    bmp_palette[i * 4 + 1] = bmp_palette[i * 4];
    bmp_palette[i * 4 + 2] = bmp_palette[i * 4 + 1];
    bmp_palette[i * 4 + 3] = 0;  /* BF-011: reserved bytes must not expose stack contents. */
  }
  for (i = 0xec; i < 0xfa; i++) {
    bmp_palette[i * 4] = 0xff;
    bmp_palette[i * 4 + 1] = bmp_palette[i * 4];
  }
  bmp_palette[1000] = 0xff;
  bmp_palette[0x3ee] = 0x7f;
  bmp_palette[0x3f2] = 0xff;
  bmp_palette[0x3f4] = 0xff;
  bmp_palette[0x3f6] = 0xff;
  bmp_palette[0x3f9] = 0xff;
  bmp_palette[0x3fc] = 0x87;
  bmp_palette[0x3fe] = 0x87;
  bmp_palette[0x3fd] = 0x54;
  if (palette != 0) {
    dst_index = 0;
    for (i = first_color; i < num_colors + first_color; i++) {
      blue_ptr = bmp_palette + dst_index * 4;
      green_ptr = bmp_palette + dst_index * 4 + 1;
      red_ptr = bmp_palette + dst_index * 4 + 2;
      tmp = i;
      logpal = (palette)->get_logpalette();
      palette_get_entry_rgb((GameLogPalette *)logpal,tmp,red_ptr,green_ptr,blue_ptr);
      dst_index++;
    }
  }
  file = __fsopen((char *)filename,((i8 *)&gfx_bmp_write_mode),0x40);
  if (file == (FILE *)0x0) {
    ok = 0;
  }
  else {
    memcpy(file_hdr,&file_type,2);
    memcpy(file_hdr + 2,&file_size,4);
    memcpy(file_hdr + 6,&reserved1,2);
    memcpy(file_hdr + 8,&reserved2,2);
    memcpy(file_hdr + 10,&bits_offset,4);
    nwritten = _fwrite(file_hdr,0xe,1,file);
    if (nwritten == 0) {
      error_msg = str_error_writing_file_header;
    }
    else {
      memcpy(info_hdr,&info_size,4);
      memcpy(info_hdr + 4,&width2,4);
      memcpy(info_hdr + 8,&height2,4);
      memcpy(info_hdr + 12,&planes,2);
      memcpy(info_hdr + 14,&bit_count,2);
      memcpy(info_hdr + 16,&compression,4);
      memcpy(info_hdr + 20,&image_size,4);
      memcpy(info_hdr + 24,&x_ppm,4);
      memcpy(info_hdr + 28,&y_ppm,4);
      memcpy(info_hdr + 32,&colors_used,4);
      memcpy(info_hdr + 36,&colors_important,4);
      nwritten = _fwrite(info_hdr,0x28,1,file);
      if (nwritten == 0) {
        error_msg = str_error_writing_info_header;
      }
      else {
        nwritten = _fwrite(bmp_palette,0x400,1,file);
        if (nwritten == 0) {
          error_msg = str_error_writing_file_palette;
        }
        else {
          ok = 1;
          row = height2;
          while( true ) {
            row--;
            if ((row < 0) || (ok == 0)) break;
            src = (byte *)(surface)->pixel_ptr(0,row);
            for (col = 0; col < width2; col++) {
              pixel = (uint)*src - first_color;
              src++;
              tmp = _fputc(pixel,file);
              if (tmp == -1) {
                error_msg = str_error_writing_bitmap_image;
                ok = 0;
                break;
              }
            }
          }
        }
      }
    }
    _fclose(file);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,error_msg,(LPCSTR)0x0,0);
    }
  }
  return ok;
}

/* 00471070 auto */
/* Library Function - Single Match
    public: class CRichEditView * __thiscall CRichEditCntrItem::GetActiveView(void)
   
   Library: Visual Studio 1998 Debug */

i32 DibSurface::get_width()

{
  i32 width;
  
  width = surface_width();
  return width;
}

/* 004710a0 auto */
/* Library Function - Single Match
    public: class CRichEditView * __thiscall CRichEditCntrItem::GetActiveView(void)
   
   Library: Visual Studio 1998 Debug */

i32 DibSurface::get_height()

{
  i32 height;
  
  height = surface_height();
  return height;
}

/* 00472cf0 auto */
int __cdecl gfx_zoom_scale(int value,int zoom)

{
  int scaled;
  
  scaled = (zoom + 8) * value;
  return (int)(scaled + (scaled >> 0x1f & 7U)) >> 3;
}

/* 0047314e auto */
void __cdecl rect_pack_to_short(RECT *rect,ShortRect *short_rect)

{
  short_rect->left = (short)rect->left;
  short_rect->right = (short)rect->right;
  short_rect->top = (short)rect->top;
  short_rect->bottom = (short)rect->bottom;
  return;
}

/* 00473190 auto */
void __cdecl rect_unpack_from_short(ShortRect *short_rect,RECT *rect)

{
  rect->left = (int)short_rect->left;
  rect->right = (int)short_rect->right;
  rect->top = (int)short_rect->top;
  rect->bottom = (int)short_rect->bottom;
  return;
}

/* 0047df20 auto */
void __cdecl gfx_set_zoom_scale(int zoom)

{
  gfx_set_scale_ratio(zoom + 8,8);
  return;
}

/* 0047df50 auto */
void __cdecl gfx_reset_zoom_scale(void)

{
  gfx_set_scale_ratio(1,1);
  return;
}

/* 0047df80 auto */
void __cdecl rect_intersect_into(LPRECT dest,RECT *rect_a,RECT *rect_b)

{
  IntersectRect(dest,rect_a,rect_b);
  return;
}

/* 0049621d auto */
PortraitDialog::PortraitDialog()

{
  
  SurfaceWindow::construct();
  surface_ctor(&surface);
  button_ctor(&cancel_button);
  button_ctor(&ok_button);
  dll.init();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3);
  civ = 0;
  field_1e0 = 0;
}

/* 0049632a auto */
/* Library Function - Single Match
    public: virtual __thiscall CCommandLineInfo::~CCommandLineInfo(void)
   
   Library: Visual Studio 1998 Debug */

PortraitDialog::~PortraitDialog()

{
  int eh_state;
  
  *(i8 ***)&vtable = (i8 **)&leader_anim_frame_starts_block.ptr_fun_3;
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 004963a5 */ { dll.free(); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 004963b4 */ { ctrl_dtor(&ok_button); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 004963c3 */ { ctrl_dtor(&cancel_button); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 004963d2 */ { surface_dtor(&surface); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004963e1 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 00496402 auto */
i32 PortraitDialog::create()

{
  wnd_create_with_surface((const char *)&str_empty_109,0xc00,0,0,ui_metrics.border_width * 8 + 0x276,
             ui_metrics.border_width * 4 + ui_metrics.caption_height + 0x16e,(GamePalette *)&game_palette_block.game_palette,&main_frame_wnd_block.main_frame_wnd);
  create_buttons();
  ((WndBase *)&window)->set_caption_height(ui_metrics.caption_height);
  window.wnd_handlers.set_lbuttonup_handler(portrait_dlg_on_click);
  window.wnd_handlers.set_dblclick_handler(portrait_dlg_on_dblclick);
  paint();
  repaint();
  ((WndBase *)&window)->center();
  return 1;
}

/* 004964b3 auto */
void PortraitDialog::create_buttons()

{
  u8 *parent_b;
  u8 *parent_a;
  tagRECT button_rect;
  
  SetRect(&button_rect,ui_metrics.border_width + 2,ui_metrics.border_width * 3 + ui_metrics.caption_height + ui_metrics.bevel_width + 0x14a,
          (ui_metrics.border_width * 8 + 0x276 >> 1) - 2,
          ui_metrics.border_width * 3 + ui_metrics.caption_height + 0x16e + ui_metrics.bevel_width);
  if (this == (PortraitDialog *)0x0) {
    parent_a = (u8 *)0x0;
  }
  else {
    parent_a = (u8 *)this + 0x48;
  }
  ok_button.create_with_callback((Wnd *)parent_a,0x65,&button_rect,0x21,(const char *)&str_ok);
  OffsetRect(&button_rect,((ui_metrics.border_width * 8 + 0x276 >> 1) + 2) - ui_metrics.border_width,0);
  if (this == (PortraitDialog *)0x0) {
    parent_b = (u8 *)0x0;
  }
  else {
    parent_b = (u8 *)this + 0x48;
  }
  cancel_button.create_with_callback((Wnd *)parent_b,100,&button_rect,0x21,(const char *)&str_cancel);
  cancel_button.set_callback(portrait_dlg_on_button);
  ok_button.set_callback(portrait_dlg_on_button);
  return;
}

/* 004965ff auto */
void PortraitDialog::run()

{
  /* B26: Cancel keeps the portrait; the original stores its stack's stale value
     (every mode: ours would store another one; docs/BUGS.md) */
  result = civs[civ].leader_portrait;
  ((Control *)cancel_button_bytes)->show_and_redraw();
  ((Control *)ok_button_bytes)->show_and_redraw();
  show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)&window)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)&window)->run_modal_loop();
  ((ModalMenuWindow *)&window)->set_modal_idle_callback(0);
  ((WndBase *)&window)->hide();
  wnd_flush_paint();
  ((Control *)cancel_button_bytes)->hide();
  ((Control *)ok_button_bytes)->hide();
  civs[civ].leader_portrait = (u8)result;
  return;
}

/* 004966c4 auto */
void PortraitDialog::paint()

{
  int loaded;
  i32 height;
  int tmp_a;
  int tmp_b;
  int tmp_c;
  int tmp_d;
  RECT *lprcSrc;
  int col;
  int y2;
  int i;
  int x2;
  int b;
  tagRECT dst_rect;
  tagRECT src_rect;
  iptr title;
  
  title = (iptr)label_string(label_table.string_ids[0x12a]);
  dll.load((const char *)&str_mk_dll);
  loaded = surface.load_gif_resource(
                     0xd3 - (uint)(leaders[civs[civ].tribe].female == '\0'),10,0xec,
                     (GamePalette *)&game_palette_block.game_palette);
  if (loaded != 0) {
    dll.dll_handle_free();
    loaded = ui_metrics.caption_height + ui_metrics.bevel_width * -2;
    height = get_width();
    gfx_tile_fill(this,ui_metrics.border_tile,0,ui_metrics.bevel_width,height,loaded,0,ui_metrics.bevel_width);
    height = get_height();
    loaded = ui_metrics.border_width + ui_metrics.bevel_width * -2;
    x2 = ui_metrics.bevel_width;
    tmp_a = ui_metrics.caption_height - ui_metrics.bevel_width;
    for (i = 0; i < 8; i++) {
      gfx_tile_fill(this,ui_metrics.border_tile,x2,tmp_a,loaded,height,x2,tmp_a);
      x2 = x2 + ui_metrics.border_width + 0x5a;
    }
    tmp_b = ui_metrics.border_width + ui_metrics.bevel_width * -2;
    y2 = ui_metrics.bevel_width + ui_metrics.caption_height + 0x6e;
    tmp_c = get_width();
    tmp_a = ui_metrics.bevel_width;
    loaded = ui_metrics.bevel_width * -2;
    for (i = 0; i < 2; i++) {
      gfx_tile_fill(this,ui_metrics.border_tile,tmp_a,y2,tmp_c + loaded,tmp_b,tmp_a,y2);
      y2 = y2 + ui_metrics.border_width + 0x6e;
    }
    loaded = ui_metrics.border_width * 2;
    tmp_c = get_width();
    tmp_b = ui_metrics.bevel_width;
    tmp_a = ui_metrics.bevel_width * -2;
    tmp_d = get_height();
    tmp_d = tmp_d + ui_metrics.border_width * -2 - 0x24 + ui_metrics.bevel_width;
    gfx_tile_fill(this,ui_metrics.border_tile,tmp_b,tmp_d,tmp_c + tmp_a,loaded + 0x24,tmp_b,tmp_d);
    SetRect(&dst_rect,ui_metrics.border_width - ui_metrics.bevel_width,ui_metrics.caption_height - ui_metrics.bevel_width,
            ui_metrics.bevel_width * 2 + (ui_metrics.border_width - ui_metrics.bevel_width) + 0x5a,
            ui_metrics.bevel_width * 2 + (ui_metrics.caption_height - ui_metrics.bevel_width) + 0x6e);
    SetRect(&src_rect,1,1,0x5b,0x6f);
    for (i = 0; i < 3; i++) {
      for (col = 0; col < 7; col++) {
        for (b = 0; b < ui_metrics.bevel_width; b++) {
          gfx_draw_bevel_rect(this,&dst_rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
          InflateRect(&dst_rect,-1,-1);
        }
        surface.DibSurface::blit(this,&src_rect,&dst_rect);
        OffsetRect(&src_rect,0x5b,0);
        InflateRect(&dst_rect,ui_metrics.bevel_width,ui_metrics.bevel_width);
        OffsetRect(&dst_rect,ui_metrics.border_width + 0x5a,0);
      }
      OffsetRect(&src_rect,-0x27d,0x6f);
      OffsetRect(&dst_rect,(ui_metrics.border_width + 0x5a) * -7,ui_metrics.border_width + 0x6e);
    }
    lprcSrc = (RECT *)get_rect();
    CopyRect(&dst_rect,lprcSrc);
    for (b = 0; b < ui_metrics.bevel_width; b++) {
      gfx_draw_bevel_rect(this,&dst_rect,ui_metrics.highlight_color,ui_metrics.shadow_color);
      InflateRect(&dst_rect,-1,-1);
    }
    SetRect(&dst_rect,ui_metrics.border_width - ui_metrics.bevel_width,ui_metrics.caption_height - ui_metrics.bevel_width,
            ui_metrics.bevel_width * 2 + (ui_metrics.border_width - ui_metrics.bevel_width) + 0x5a,
            ui_metrics.bevel_width * 2 + (ui_metrics.caption_height - ui_metrics.bevel_width) + 0x6e);
    for (b = 0; b < ui_metrics.bevel_width; b++) {
      gfx_draw_bevel_rect(this,&dst_rect,0x5e,0x5e);
      InflateRect(&dst_rect,-1,-1);
    }
    tmp_a = get_width();
    tmp_b = ((GameFont *)((i8 *)&caption_font_block.caption_font))->text_width((const char *)title);
    loaded = ui_metrics.bevel_width;
    tmp_b = (tmp_a >> 1) - (tmp_b + 4 >> 1);
    tmp_a = ui_metrics.bevel_width + 1;
    gfx_set_text_color_index(0x12);
    draw_text(&caption_font_block.caption_font,(const char *)title,tmp_b + 3,loaded + 2,5);
    gfx_set_text_color_index(0x29);
    draw_text(&caption_font_block.caption_font,(const char *)title,tmp_b + 1,tmp_a,5);
    draw_text(&caption_font_block.caption_font,(const char *)title,tmp_b,tmp_a,5);
  }
  return;
}

/* 00496c3c auto */
void __cdecl portrait_dlg_on_click(int x,int y)

{
  int tmp;
  tagRECT old_rect;
  int old_y;
  tagRECT new_rect;
  int new_y;
  int old_x;
  u32 old_sel;
  int new_x;
  int b;
  DibSurface *dlg;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    dlg = (DibSurface *)0x0;
  }
  else {
    dlg = (DibSurface *)(tmp - 0x48);
  }
  old_sel = dlg[6].bounds_bottom;
  tmp = dlg->get_height();
  if (y <= tmp - 0x24 + ui_metrics.border_width * -2) {
    tmp = (y - ui_metrics.caption_height) / (ui_metrics.border_width + 0x6e);
    dlg[6].bounds_bottom =
         ((x - (ui_metrics.border_width >> 1)) / (ui_metrics.border_width + 0x5a) - tmp) + tmp * 8;
    new_x = (((int)dlg[6].bounds_bottom % 7) * (ui_metrics.border_width + 0x5a) + ui_metrics.border_width) -
               ui_metrics.bevel_width;
    new_y = (((int)dlg[6].bounds_bottom / 7) * (ui_metrics.border_width + 0x6e) + ui_metrics.caption_height) -
               ui_metrics.bevel_width;
    old_x = ((ui_metrics.border_width + 0x5a) * ((int)old_sel % 7) + ui_metrics.border_width) - ui_metrics.bevel_width;
    old_y = ((ui_metrics.border_width + 0x6e) * ((int)old_sel / 7) + ui_metrics.caption_height) - ui_metrics.bevel_width;
    SetRect(&new_rect,new_x,new_y,ui_metrics.bevel_width * 2 + new_x + 0x5a,
            ui_metrics.bevel_width * 2 + new_y + 0x6e);
    SetRect(&old_rect,old_x,old_y,ui_metrics.bevel_width * 2 + old_x + 0x5a,
            ui_metrics.bevel_width * 2 + old_y + 0x6e);
    for (b = 0; b < ui_metrics.bevel_width; b++) {
      gfx_draw_bevel_rect(dlg,&new_rect,0x5e,0x5e);
      InflateRect(&new_rect,-1,-1);
      gfx_draw_bevel_rect(dlg,&old_rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
      InflateRect(&old_rect,-1,-1);
    }
    dlg->wnd_blit_surface();
  }
  return;
}

/* 00496e5d auto */
void __cdecl portrait_dlg_on_dblclick(i32 x,int y)

{
  int tmp;
  iptr dlg;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    dlg = 0;
  }
  else {
    dlg = tmp - 0x48;
  }
  portrait_dlg_on_click(x,y);
  tmp = ((DibSurface *)dlg)->get_height();
  if (y < tmp - 0x24 + ui_metrics.border_width * -2) {
    portrait_dlg_on_button(100);
  }
  return;
}

/* 00496ecf auto */
void __cdecl portrait_dlg_on_button(int button_id)

{
  iptr child;
  PortraitDialog *dlg;
  
  child = wnd_get_active_child();
  if (child == 0) {
    dlg = 0;
  }
  else {
    /* the active window is the dialog's Wnd */
    dlg = (PortraitDialog *)((u8 *)child - offsetof(PortraitDialog, window));
  }
  if (((button_id != 100) && (button_id == 0x65)) &&
     (dlg->result = dlg->field_1e0,
     leaders[civs[(int)dlg->civ].tribe].female != '\0')) {
    dlg->result = dlg->result + 0x15;
  }
  dlg->window.end_modal_loop();
  return;
}

/* 00497c40 auto */
void GamePalette::get_entry(int index,i8 *red,i8 *green,i8 *blue)

{
  palette_get_entry_rgb((GameLogPalette *)this,index,red,green,blue);
  return;
}

/* 00497c90 auto */
u32 *DibSurface::get_rect()

{
  return &bounds_rect;
}

/* 004a6980 auto */
i32 Sprite::width()

{
  i32 width;
  
  width = rect_width((RECT *)this);
  return width;
}

/* 004b3ca0 auto */
void __cdecl font_size16_bold_global_init(void)

{
  font_size16_bold_static_init();
  font_size16_bold_reg_atexit();
  return;
}

/* 004b3cba auto */
void __cdecl font_size16_bold_static_init(void)

{
  ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->init_styled(0,0x10,1);
  return;
}

/* 004b3cda auto */
void __cdecl font_size16_bold_reg_atexit(void)

{
  _atexit(font_size16_bold_static_dtor);
  return;
}

/* 004b3cf7 auto */
void __cdecl font_size16_bold_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->destroy();
  return;
}

/* 004b3d11 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size20_global_init(void)

{
  font_size20_static_init();
  font_size20_reg_atexit();
  return;
}

/* 004b3d2b auto */
void __cdecl font_size20_static_init(void)

{
  ((GameFont *)((i8 *)&font_size20))->init(0,0x14);
  return;
}

/* 004b3d49 auto */
void __cdecl font_size20_reg_atexit(void)

{
  _atexit(font_size20_static_dtor);
  return;
}

/* 004b3d66 auto */
void __cdecl font_size20_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size20))->destroy();
  return;
}

/* 004b3d80 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size14_global_init(void)

{
  font_size14_static_init();
  font_size14_reg_atexit();
  return;
}

/* 004b3d9a auto */
void __cdecl font_size14_static_init(void)

{
  ((GameFont *)((i8 *)&font_size14))->init(0,0xe);
  return;
}

/* 004b3db8 auto */
void __cdecl font_size14_reg_atexit(void)

{
  _atexit(font_size14_static_dtor);
  return;
}

/* 004b3dd5 auto */
void __cdecl font_size14_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size14))->destroy();
  return;
}

/* 004b3def auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size16_global_init(void)

{
  font_size16_static_init();
  font_size16_reg_atexit();
  return;
}

/* 004b3e09 auto */
void __cdecl font_size16_static_init(void)

{
  ((GameFont *)((i8 *)&font_size16))->init(0,0x10);
  return;
}

/* 004b3e27 auto */
void __cdecl font_size16_reg_atexit(void)

{
  _atexit(font_size16_static_dtor);
  return;
}

/* 004b3e44 auto */
void __cdecl font_size16_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size16))->destroy();
  return;
}

/* 004b3e5e auto */
void __cdecl font_size24_bold_global_init(void)

{
  font_size24_bold_static_init();
  font_size24_bold_reg_atexit();
  return;
}

/* 004b3e78 auto */
void __cdecl font_size24_bold_static_init(void)

{
  ((GameFont *)((i8 *)&font_size24_bold))->init_styled(0,0x18,1);
  return;
}

/* 004b3e98 auto */
void __cdecl font_size24_bold_reg_atexit(void)

{
  _atexit(font_size24_bold_static_dtor);
  return;
}

/* 004b3eb5 auto */
void __cdecl font_size24_bold_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size24_bold))->destroy();
  return;
}

/* 004b3ecf auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size30_global_init(void)

{
  font_size30_static_init();
  font_size30_reg_atexit();
  return;
}

/* 004b3ee9 auto */
void __cdecl font_size30_static_init(void)

{
  ((GameFont *)((i8 *)&font_size30))->init(0,0x1e);
  return;
}

/* 004b3f07 auto */
void __cdecl font_size30_reg_atexit(void)

{
  _atexit(font_size30_static_dtor);
  return;
}

/* 004b3f24 auto */
void __cdecl font_size30_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size30))->destroy();
  return;
}

/* 004b3f3e auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size21_global_init(void)

{
  font_size21_static_init();
  font_size21_reg_atexit();
  return;
}

/* 004b3f58 auto */
void __cdecl font_size21_static_init(void)

{
  ((GameFont *)((i8 *)&font_size21))->init(0,0x15);
  return;
}

/* 004b3f76 auto */
void __cdecl font_size21_reg_atexit(void)

{
  _atexit(font_size21_static_dtor);
  return;
}

/* 004b3f93 auto */
void __cdecl font_size21_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size21))->destroy();
  return;
}

/* 004b3fad auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl font_size24_global_init(void)

{
  font_size24_static_init();
  font_size24_reg_atexit();
  return;
}

/* 004b3fc7 auto */
void __cdecl font_size24_static_init(void)

{
  ((GameFont *)((i8 *)&font_size24))->init(0,0x18);
  return;
}

/* 004b3fe5 auto */
void __cdecl font_size24_reg_atexit(void)

{
  _atexit(font_size24_static_dtor);
  return;
}

/* 004b4002 auto */
void __cdecl font_size24_static_dtor(void)

{
  ((GameFont *)((i8 *)&font_size24))->destroy();
  return;
}

/* 004bb540 auto */
i32 Sprite::height()

{
  i32 height;
  
  height = rect_height((RECT *)this);
  return height;
}

/* 004bb800 auto */
void __cdecl rect_deflate(RECT *rect,int dx,int dy)

{
  rect_inflate(rect,-dx,-dy);
  return;
}

/* 004bb840 auto */
void __cdecl rect_inflate(RECT *rect,int dx,int dy)

{
  InflateRect(rect,dx,dy);
  return;
}

/* 004ccb6a auto */
void __cdecl gfx_draw_tiled_frame(DibSurface *surface,int x,int y,int width,int height,int border)

{
  int left;
  int top;
  i32 extent;
  int offset;
  int i;
  RECT rect;
  
  left = x - border;
  top = y - border;
  rect_set_xywh(&rect,left,top,width + border * 2,height + border * 2);
  extent = rect_width((RECT *)(iptr)((i8 *)&rect.left));
  gfx_tile_fill(surface,&window_border_tile_surface,left,top + 2,extent,border - 4,0,2);
  offset = rect_height((RECT *)(iptr)((i8 *)&rect.left));
  offset -= (border - 2);
  gfx_tile_fill(surface,&window_border_tile_surface,left,offset + top,extent,border - 4,0,offset);
  extent = rect_height((RECT *)(iptr)((i8 *)&rect.left));
  gfx_tile_fill(surface,&window_border_tile_surface,left + 2,top,border - 4,extent,2,0);
  offset = rect_width((RECT *)(iptr)((i8 *)&rect.left));
  offset -= (border - 2);
  gfx_tile_fill(surface,&window_border_tile_surface,offset + left,top,border - 4,extent,offset,0);
  for (i = 0; i < 2; i++) {
    gfx_draw_bevel_rect(surface,(RECT *)(iptr)((i8 *)&rect.left),0x25,0x12);
    rect_deflate(&rect,1,1);
  }
  rect_deflate(&rect,border - 4,0);
  rect.top = rect.top + border - 4;
  rect.bottom -= (border - 4);
  for (i = 0; i < 2; i++) {
    gfx_draw_bevel_rect(surface,(RECT *)(iptr)((i8 *)&rect.left),0x12,0x25);
    rect_deflate(&rect,1,1);
  }
  return;
}

/* 004d8af0 auto */
u32 GameFont::get_handle()

{
  return hfont;
}

/* 00548c78 auto */
void __cdecl
sprite_draw_spaced_row(DibSurface *surface,Sprite *sprite,int x,i32 y,int count,int slots,
            int spacing,i32 item_width,int spread_extra)

{
  int limit;
  i8 clip_rect[16];
  int remainder;
  int error_acc;
  int i;
  int orig_slots;
  
  error_acc = 0;
  orig_slots = slots;
  spacing = ui_calc_item_spacing(slots,spacing,item_width,&slots,&remainder);
  if (((orig_slots != 0) && (slots != orig_slots)) && (count != orig_slots)) {
    count = (slots * count) / orig_slots;
  }
  i = 0;
  while( true ) {
    limit = slots;
    if (count <= slots) {
      limit = count;
    }
    if (limit <= i) break;
    (sprite)->draw((RECT *)clip_rect,surface,x,y);
    x += spacing;
    if ((spread_extra != 0) && (1 < slots)) {
      for (error_acc = error_acc + remainder; slots - 1 <= error_acc;
          error_acc = error_acc - (slots - 1)) {
        x++;
      }
    }
    i++;
  }
  return;
}

/* 0054a874 auto */
uint __cdecl bits_nth_set(int n,uint mask)

{
  bool more;
  i32 bit;
  
  bit = 0;
  do {
    if (bit == 0) {
      bit = 1;
    }
    else {
      bit = bit << 1;
    }
    for (; (mask & bit) == 0; bit = bit << 1) {
    }
    more = n != 0;
    n--;
  } while (more);
  return bit;
}

/* 0054a8d3 auto */
int __cdecl bits_index_of(uint bit,uint mask)

{
  i32 index;
  
  index = 0;
  while (bit = bit >> 1, bit != 0) {
    if ((mask & bit) != 0) {
      index++;
    }
  }
  return index;
}

/* 0055a930 auto */
void __cdecl
gfx_copy_rect_same_pos(DibSurface *src,DibSurface *dst,i32 x,i32 y,i32 width,
            i32 height)

{
  gfx_copy_rect_xywh(src,dst,x,y,x,y,width,height);
  return;
}

/* 005683c5 auto */
i32 __cdecl gfx_dissolve_blit(DibSurface *surface,RECT *rect,uint cell_size,int mode)

{
  int lo;
  int pick;
  uint cols;
  uint rows;
  HGLOBAL mem;
  i32 result;
  int cell_x;
  int cell_y;
  int cell_right;
  int right_edge;
  uint row;
  uint col;
  int remaining;
  tagRECT area2;
  tagRECT cell_rect;
  POINT *cells;
  
  if (rect == 0) {
    lo = (surface)->get_height();
    pick = (surface)->get_width();
    SetRect(&area2,0,0,pick,lo);
  }
  else {
    CopyRect(&area2,rect);
  }
  lo = rect_width(&area2);
  cols = lo / (int)cell_size;
  lo = rect_height(&area2);
  rows = lo / (int)cell_size;
  remaining = rows * cols;
  mem = GlobalAlloc(0x40,remaining * 8);
  cells = (POINT *)GlobalLock(mem);
  if (cells == (POINT *)0x0) {
    result = 0;
  }
  else {
    switch(mode) {
    case 1:
      for (col = 0; col < cols; col++) {
        for (row = 0; row < rows; row++) {
          win_flush_pending_msgs();
          cells[(col * rows + row)].x =
               ((cols - col) - 1) * cell_size;
          cells[(col * rows + row)].y = row * cell_size;
        }
      }
      break;
    case 2:
    case 5:
      for (col = 0; col < cols; col++) {
        for (row = 0; row < rows; row++) {
          win_flush_pending_msgs();
          cells[(col * rows + row)].x = col * cell_size;
          cells[(col * rows + row)].y = row * cell_size;
        }
      }
      break;
    case 3:
      for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
          win_flush_pending_msgs();
          cells[(row * cols + col)].x = col * cell_size;
          cells[(row * cols + col)].y = row * cell_size;
        }
      }
      break;
    case 4:
      for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
          win_flush_pending_msgs();
          cells[(row * cols + col)].x = col * cell_size;
          cells[(row * cols + col)].y =
               ((rows - row) - 1) * cell_size;
        }
      }
    }
    if (mode == 5) {
      while (remaining != 0) {
        win_flush_pending_msgs();
        pick = rand_range(0,remaining - 1);
        cell_x = cells[pick].x + area2.left;
        cell_y = cells[pick].y + area2.top;
        lo = cell_size + cell_y;
        if (area2.bottom - 1 <= lo) {
          lo = area2.bottom - 1;
        }
        right_edge = cell_size + cell_x;
        cell_right = area2.right - 1;
        if (right_edge <= area2.right - 1) {
          cell_right = right_edge;
        }
        SetRect(&cell_rect,cell_x,cell_y,cell_right,lo);
        (surface)->wnd_blit_surface_rect(&cell_rect);
        remaining--;
        cells[pick].x = cells[remaining].x;
        cells[pick].y =
             cells[remaining].y;
      }
    }
    else {
      while (remaining != 0) {
        win_flush_pending_msgs();
        lo = (remaining - 1) - (rows * 0x14) / cell_size;
        if (lo < 1) {
          lo = 0;
        }
        pick = rand_range(lo,remaining - 1);
        cell_x = cells[pick].x + area2.left;
        cell_y = cells[pick].y + area2.top;
        lo = cell_size + cell_y;
        if (area2.bottom - 1 <= lo) {
          lo = area2.bottom - 1;
        }
        right_edge = cell_size + cell_x;
        cell_right = area2.right - 1;
        if (right_edge <= area2.right - 1) {
          cell_right = right_edge;
        }
        SetRect(&cell_rect,cell_x,cell_y,cell_right,lo);
        (surface)->wnd_blit_surface_rect(&cell_rect);
        remaining--;
        cells[pick].x = cells[remaining].x;
        cells[pick].y =
             cells[remaining].y;
      }
    }
    mem = GlobalHandle(cells);
    GlobalUnlock(mem);
    mem = GlobalHandle(cells);
    GlobalFree(mem);
    result = 1;
  }
  return result;
}

/* 0056b90b auto */
void __cdecl gfx_select_zoom_font(i32 zoom)

{
  int font_size;
  
  font_size = gfx_zoom_scale(0x20,zoom);
  font_size = font_size / 3;
  if (font_size == 10) {
    font_size = 0xb;
  }
  if (font_size != map_label_font_size) {
    ((GameFont *)((i8 *)&status_window_block.map_label_font))->set(1,font_size);
    map_label_font_size = font_size;
  }
  return;
}

/* 0056baff auto */
void __cdecl
gfx_draw_unit(DibSurface *surface,int unit,uint draw_flags,int px,int py,int zoom,int no_fortress)

{
  int tmp;
  iptr scaled;
  int text_y;
  i32 value;
  uint abs_dx;
  i8 clip_fortress[16];
  i8 clip_fortify[16];
  i8 clip_unit[16];
  i8 clip_silhouette[16];
  i8 clip_shield[16];
  i8 clip_shield_shadow[16];
  i8 clip_stack[16];
  i8 clip_stack_shadow[16];
  uptr order;
  iptr civ;
  int shield_x;
  int stack_dy;
  iptr unit2;
  uint stack_dx;
  iptr shield_height;
  uint silhouette;
  uint dmg;
  RECT shield_rect;
  RECT band_rect;
  uint sprite_type;
  uint unit_type;
  iptr shield_width;
  int max_hp;
  int bar_width;
  i32 color_mid;
  i32 hp_color;
  i32 color_primary;
  int shield_y;
  u8 order_text[80];
  int hp_width;
  int side;
  u8 order_char;
  RECT hp_rect;
  
  order_char = '\0';
  gfx_set_zoom_scale(zoom);
  unit2 = unit;
  if (((draw_flags & 2) != 0) && (unit2 = unit_stack_pick_display_unit(unit), unit2 < 0)) {
    gfx_reset_zoom_scale();
    return;
  }
  civ = (iptr)units[unit2].owner;
  unit_type = (uint)units[unit2].type;
  sprite_type = unit_type;
  if ((unit_type == UNIT_DIPLOMAT) && (civ == 0)) {
    sprite_type = 0x3e;
  }
  color_primary = civ_color_primary(civ);
  color_mid = civ_color_mid(civ);
  shield_x = gfx_zoom_scale(road_sprites_block.unit_shield_x_offsets[sprite_type],zoom);
  shield_x = px + shield_x;
  shield_y = gfx_zoom_scale(road_sprites_block.unit_shield_y_offsets[sprite_type],zoom);
  shield_y = py + shield_y;
  shield_width = gfx_zoom_scale(0xc,zoom);
  shield_height = gfx_zoom_scale(0x14,zoom);
  side = 1;
  if (road_sprites_block.unit_shield_x_offsets[sprite_type] < 0x20) {
    side = -1;
  }
  if (((draw_flags & 1) != 0) &&
     ((-1 < units[unit2].prev_in_stack || (-1 < units[unit2].next_in_stack)))) {
    tmp = gfx_zoom_scale(4,zoom);
    stack_dx = tmp * side;
    stack_dy = 0;
    rect_set_xywh(&shield_rect,stack_dx + shield_x,shield_y,shield_width,shield_height);
    if ((int)stack_dx < 1) {
      abs_dx = ~stack_dx + 1;
    }
    else {
      abs_dx = stack_dx;
    }
    if (2 < (int)abs_dx) {
      ((Sprite *)((i8 *)&unit_shield_sprite))->draw((RECT *)clip_stack_shadow,surface,
                 side + stack_dx + shield_x,stack_dy + shield_y + 1);
      rect_offset(&shield_rect,side,1);
      (surface)->replace_color((RECT *)&shield_rect.left,0x106,0xf);
      (surface)->replace_color((RECT *)&shield_rect.left,0x105,0xf);
      rect_offset(&shield_rect,-side,0xffffffff);
    }
    ((Sprite *)((i8 *)&unit_shield_sprite))->draw((RECT *)clip_stack,surface,stack_dx + shield_x,
               stack_dy + shield_y);
    (surface)->replace_color((RECT *)&shield_rect.left,0x106,color_mid);
    (surface)->replace_color((RECT *)&shield_rect.left,0x105,color_mid);
  }
  rect_set_xywh(&shield_rect,shield_x,shield_y,shield_width,shield_height);
  ((Sprite *)((i8 *)&unit_shield_sprite))->draw((RECT *)clip_shield_shadow,surface,side + shield_x,shield_y + 1
            );
  rect_offset(&shield_rect,side,1);
  (surface)->replace_color((RECT *)&shield_rect.left,0x106,0x10);
  (surface)->replace_color((RECT *)&shield_rect.left,0x105,0x10);
  rect_offset(&shield_rect,-side,0xffffffff);
  ((Sprite *)((i8 *)&unit_shield_sprite))->draw((RECT *)clip_shield,surface,shield_x,shield_y);
  (surface)->replace_color((RECT *)&shield_rect.left,0x106,color_primary);
  (surface)->replace_color((RECT *)&shield_rect.left,0x105,color_mid);
  scaled = gfx_zoom_scale(2,zoom);
  rect_set_xywh(&band_rect,shield_x,shield_y,shield_width,scaled);
  gfx_fill_rect(surface,&band_rect,10);
  tmp = rect_height(&band_rect);
  shield_rect.top += tmp;
  scaled = gfx_zoom_scale(3,zoom);
  rect_set_xywh(&hp_rect,shield_x,shield_rect.top,shield_width,scaled);
  tmp = rect_height(&hp_rect);
  shield_rect.top += tmp;
  scaled = gfx_zoom_scale(2,zoom);
  rect_set_xywh(&band_rect,shield_x,shield_rect.top,shield_width,scaled);
  gfx_fill_rect(surface,&band_rect,10);
  tmp = rect_height(&band_rect);
  shield_rect.top += tmp;
  bar_width = rect_width(&hp_rect);
  max_hp = (int)(char)unit_types[units[unit2].type].hit_points;
  dmg = (uint)units[unit2].damage;
  if (max_hp == 0) {
    max_hp = 10;
  }
  if ((game.rules & RULE_NORMAL_COMBAT) == 0) {
    dmg = 0;
  }
  if ((draw_flags & 4) == 0) {
    dmg = 0;
  }
  if (zoom < -3) {
    dmg = 0;
  }
  hp_width = (int)((max_hp - dmg) * bar_width) / max_hp;
  if ((int)dmg < max_hp / 3) {
    hp_color = 0x2a;
  }
  else if ((int)dmg < (max_hp * 2) / 3) {
    hp_color = 0x7a;
  }
  else {
    hp_color = 0x6a;
  }
  if (bar_width != hp_width) {
    gfx_fill_rect(surface,&hp_rect,10);
  }
  if (hp_width != 0) {
    band_rect.left = hp_rect.left;
    band_rect.top = hp_rect.top;
    band_rect.bottom = hp_rect.bottom;
    band_rect.right = hp_rect.right - (bar_width - hp_width);
    gfx_fill_rect(surface,&band_rect,hp_color);
  }
  order = (int)units[unit2].orders & 0xf;
  if (units[unit2].orders == ORDER_AI_IDLE) {
    order = 0xffffffff;
  }
  if ((int)order < 1) {
    order_char = '-';
    if ((units[unit2].flags & UNITF_AUTOMATED) != 0) {
      order_char = '*';
    }
  }
  else if ((int)order < 0xb) {
    order_char = order_names[order].key;
  }
  else if (order == 0xb) {
    if ((game.reveal_map == '\0') ||
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
        order_char = '-';
      }
      else {
        order_char = order_names[0xb].key;
      }
    }
    else {
      order_char = units[unit2].ai_task;
    }
  }
  else {
    order_char = '-';
    if ((units[unit2].flags & UNITF_AUTOMATED) != 0) {
      order_char = '*';
    }
  }
  if (order_char != '\0') {
    order_text[0] = order_char;
    order_text[1] = 0;
    tmp = gfx_zoom_scale(3,zoom);
    shield_rect.bottom -= tmp;
    tmp = crt_toupper((int)(char)order_char);
    if (tmp == 0x4d) {
      tmp = gfx_zoom_scale(1,zoom);
      shield_rect.top -= tmp;
      tmp = gfx_zoom_scale(1,zoom);
      shield_rect.bottom -= tmp;
    }
    gfx_select_zoom_font(zoom);
    gfx_set_text_color_index(10);
    if ((units[unit2].flags & UNITF_AUTOMATED) != 0) {
      gfx_set_text_color_index(0x1d);
    }
    value = 0;
    tmp = rect_height(&shield_rect);
    text_y = shield_rect.top + tmp / 2;
    tmp = rect_width(&shield_rect);
    (surface)->draw_text((GameFont *)&status_window_block.map_label_font,(const char *)order_text,shield_rect.left + tmp / 2,text_y,value);
  }
  silhouette = (uint)(units[unit2].orders == ORDER_SLEEP);
  if ((((unit_types[units[unit2].type].domain == DOMAIN_AIR) &&
       (tmp = map_in_bounds(units[unit2].x,units[unit2].y), tmp != 0)) &&
      (tmp = tile_airbase_owner((int)units[unit2].x,(int)units[unit2].y), -1 < tmp)) &&
     (((game.active_unit != unit2 || (moving_units_mode != 1)) || (blink.unit_active == '\0')))) {
    silhouette = 1;
  }
  if (silhouette == 0) {
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c))->draw((RECT *)clip_unit,surface,px,
               py);
  }
  else if (extra_ui(X_SENTRY_GREY)) {
    /* X41: in grey halftones, not a grey shape */
    extras_draw_grey_sprite((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c),
                            (iptr)surface,px,py);
  }
  else {
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c))->draw_silhouette((RECT *)clip_silhouette,surface,px,
               py,0x1a);
  }
  if (units[unit2].orders == ORDER_FORTIFIED) {
    ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->draw((RECT *)clip_fortify,surface,px,py);
  }
  tmp = map_in_bounds(units[unit2].x,units[unit2].y);
  if (((tmp != 0) &&
      (value = tile_improvements(units[unit2].x,units[unit2].y),
      ((byte)value & TILE_AIRBASE) == 0x40)) && (no_fortress == 0)) {
    ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->draw((RECT *)clip_fortress,surface,px,py);
  }
  if (extra_ui(X_WORK_COUNTER)) {
    extras_draw_work_counter((iptr)surface,unit2,px,py,zoom);    /* X3 */
  }
  gfx_reset_zoom_scale();
  return;
}

/* 0056c5fc auto */
void __cdecl
gfx_copy_rect_clipped(DibSurface *src,DibSurface *dst,int src_x,int src_y,int src_origin_x,int src_origin_y,int dst_x,
            int dst_y,int dst_origin_x,int dst_origin_y,int width,int height)

{
  src_x -= src_origin_x;
  src_y -= src_origin_y;
  dst_x -= dst_origin_x;
  dst_y -= dst_origin_y;
  if (src_x < 0) {
    dst_x -= src_x;
    width += src_x;
    src_x = 0;
  }
  if (dst_x < 0) {
    src_x -= dst_x;
    width += dst_x;
    dst_x = 0;
  }
  if (src_y < 0) {
    dst_y -= src_y;
    height += src_y;
    src_y = 0;
  }
  if (dst_y < 0) {
    src_y -= dst_y;
    height += dst_y;
    dst_y = 0;
  }
  if ((0 < width) && (0 < height)) {
    gfx_copy_rect_xywh(src,dst,src_origin_x + src_x,src_origin_y + src_y,dst_origin_x + dst_x,
                       dst_origin_y + dst_y,width,height);
  }
  return;
}

/* 0056d289 auto */
void __cdecl
gfx_draw_city(DibSurface *surface,int city,uint draw_flags,int px,int py,int zoom)

{
  char shown_size;
  short style_id;
  int tmp;
  size_t text_len;
  int box_dx;
  i8 clip_disorder[16];
  i8 clip_flag[16];
  i8 clip_city[16];
  iptr civ;
  int style2;
  int height2;
  iptr width2;
  int size_idx;
  i32 color_mid;
  int flag_y;
  i32 color_primary;
  int scale_factor;
  int flag_x;
  int pad;
  i8 template_text[8];
  uint occupied;
  int walled;
  char size_text[8];
  struct { int left; iptr top; i32 right; i32 bottom; } label_rect;
  int city_size;
  
  gfx_set_zoom_scale(zoom);
  width2 = gfx_zoom_scale(0x40,zoom);
  height2 = gfx_zoom_scale(0x30,zoom);
  civ = (iptr)cities[city].owner;
  rect_set_xywh((RECT *)&label_rect.left,px,py,width2,height2);
  gfx_set_draw_target(surface);
  color_mid = civ_color_mid(civ);
  color_primary = civ_color_primary(civ);
  tmp = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
  occupied = (uint)(-1 < tmp);
  tmp = city_has_improvement(city,IMP_CITY_WALLS);
  if ((tmp == 0) && (tmp = civ_has_wonder(civ,WONDER_GREAT_WALL), tmp == 0)) {
    walled = 0;
  }
  else {
    walled = 1;
  }
  scale_factor = (-1 < zoom) + 1;
  style_id = leaders[civs[civ].tribe].city_style;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    style_id = civ_names[civ].city_style;
  }
  style2 = (int)style_id;
  tmp = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (tmp != 0) {
    style2 = 4;
  }
  tmp = civ_has_advance(civ,ADV_AUTOMOBILE);
  if ((tmp != 0) && (tmp = civ_has_advance(civ,ADV_ELECTRONICS), tmp != 0)) {
    style2 = 5;
  }
  if ((((human_civ[0] == civ) || (game.reveal_map != '\0')) ||
      (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) || ((draw_flags & 0x1000) != 0))
  {
    shown_size = cities[city].size;
  }
  else {
    shown_size = *(char *)&cities[city].revealed_size[human_civ[0]];
  }
  city_size = (int)shown_size;
  if (style2 < 4) {
    if (city_size < 4) {
      size_idx = 0;
    }
    else if (city_size < 6) {
      size_idx = 1;
    }
    else if (city_size < 8) {
      size_idx = 2;
    }
    else {
      size_idx = 3;
    }
  }
  else if (style2 == 4) {
    if (city_size < 5) {
      size_idx = 0;
    }
    else if (city_size < 8) {
      size_idx = 1;
    }
    else if (city_size < 0xb) {
      size_idx = 2;
    }
    else {
      size_idx = 3;
    }
  }
  else if (city_size < 5) {
    size_idx = 0;
  }
  else if (city_size < 0xb) {
    size_idx = 1;
  }
  else if (city_size < 0x13) {
    size_idx = 2;
  }
  else {
    size_idx = 3;
  }
  tmp = city_has_improvement(city,IMP_PALACE);
  if ((tmp != 0) &&
     (((human_civ[0] == civ ||
       ((game.difficulty < DIFF_KING && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0))))
      && (size_idx < 3)))) {
    size_idx++;
  }
  ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + walled * 0x3c + style2 * 0x1e0))->draw((RECT *)clip_city,surface,px,py);
  tmp = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
  if ((-1 < tmp) && ((draw_flags & 2) == 0)) {
    flag_x = gfx_zoom_scale(peace_treaty_icon_sprite_block.city_flag_x_offsets[style2 * 8 + walled + size_idx * 2],
                                  zoom);
    flag_x = px + flag_x;
    flag_y = gfx_zoom_scale(peace_treaty_icon_sprite_block.city_flag_y_offsets[style2 * 8 + walled + size_idx * 2],
                                  zoom);
    flag_y = py + flag_y;
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->draw((RECT *)clip_flag,surface,flag_x,
               flag_y);
  }
  if ((draw_flags & 1) == 0) {
    if ((cities[city].flags & CITYF_DISORDER) == 0) {
      gfx_select_zoom_font(zoom);
      text_set_font((GameFont *)&status_window_block.map_label_font);
      text_set_surface(surface);
      __itoa(city_size,size_text,10);
      width2 = ((GameFont *)((i8 *)&status_window_block.map_label_font))->text_width((const char *)size_text);
      height2 = ((GameFont *)((i8 *)&status_window_block.map_label_font))->font_cached_height();
      crt_strcpy(template_text,((i8 *)&city_size_digit_str));
      text_len = _strlen(size_text);
      if (1 < text_len) {
        crt_strcat(template_text,((i8 *)&city_size_digit2_str));
      }
      pad = ((GameFont *)((i8 *)&status_window_block.map_label_font))->text_width((const char *)template_text);
      pad -= width2;
      if (pad < 0) {
        pad = 0;
      }
      width2 = width2 + pad + 5;
      height2 += 2;
      rect_set_xywh((RECT *)&label_rect.left,0,0,width2,height2);
      tmp = gfx_zoom_scale(road_sprites_block.city_label_y_offsets[style2 * 8 + walled + size_idx * 2],
                                 zoom);
      tmp = py + tmp;
      box_dx = gfx_zoom_scale(road_sprites_block.city_size_box_x_offsets[style2 * 8 + walled + size_idx * 2],
                                 zoom);
      rect_offset((RECT *)&label_rect.left,px + box_dx,tmp);
      gfx_draw_rect_frame(surface,(RECT *)&label_rect.left,10);
      rect_deflate((RECT *)&label_rect.left,1,1);
      gfx_fill_rect(surface,(RECT *)&label_rect.left,color_primary);
      text_set_colors(10,10,1,0);
      gfx_draw_text((const char *)size_text,label_rect.left + (pad >> 1) + 1,label_rect.top);
    }
    else {
      ((Sprite *)((i8 *)&fortify_overlay_sprites_block.city_disorder_icon_sprite))->draw((RECT *)clip_disorder,surface,px,py);
      (surface)->replace_color((RECT *)&label_rect.left,0x106,color_primary);
    }
  }
  gfx_reset_zoom_scale();
  return;
}

/* 0056e1c0 auto */
int __cdecl gfx_zoom_scale_round(int value,int zoom)

{
  int scaled;
  
  scaled = (zoom + 8) * value + 4;
  return (int)(scaled + (scaled >> 0x1f & 7U)) >> 3;
}

/* 005a9780 auto */
void __cdecl gfx_set_draw_target(DibSurface *surface)

{
  gfx_draw_target = surface;
  return;
}

/* 005a9798 auto */
void __cdecl
gfx_draw_line_color(DibSurface *surface,i32 x1,i32 y1,i32 x2,
            i32 y2,i32 color)

{
  gfx_set_text_color_index(color);
  (surface)->draw_line(x1,y1,x2,y2);
  return;
}

/* 005a97cc auto */
void __cdecl
gfx_draw_hline(DibSurface *surface,i32 x1,int x2,int y,i32 color)

{
  i8 rect[16];
  
  rect_set((RECT *)rect,x1,y,x2 + 1,y + 1);
  gfx_fill_rect(surface,(RECT *)rect,color);
  return;
}

/* 005a9858 auto */
void __cdecl
gfx_draw_vline(DibSurface *surface,int x,i32 y1,int y2,i32 color)

{
  i8 rect[16];
  
  rect_set((RECT *)rect,x,y1,x + 1,y2 + 1);
  gfx_fill_rect(surface,(RECT *)rect,color);
  return;
}

/* 005a98e4 auto */
void __cdecl
gfx_draw_box_ltrb(DibSurface *surface,i32 left,i32 top,i32 right,
            i32 bottom,i32 color)

{
  gfx_draw_hline(surface,left,right,top,color);
  gfx_draw_hline(surface,left,right,bottom,color);
  gfx_draw_vline(surface,left,top,bottom,color);
  gfx_draw_vline(surface,right,top,bottom,color);
  return;
}

/* 005a9964 auto */
void __cdecl
gfx_draw_frame_rect(DibSurface *surface,int x,int y,int width,int height,i32 color)

{
  gfx_draw_hline(surface,x,width + x - 1,y,color);
  gfx_draw_hline(surface,x,width + x - 1,height + y - 1,color);
  gfx_draw_vline(surface,x,y,height + y - 1,color);
  gfx_draw_vline(surface,width + x - 1,y,height + y - 1,color);
  return;
}

/* 005a99fc auto */
void __cdecl gfx_draw_bevel_rect(DibSurface *surface,RECT *rect,i32 light_color,i32 dark_color)

{
  gfx_draw_hline(surface,rect->left,rect->right - 1,
                     rect->top,light_color);
  gfx_draw_vline(surface,rect->left,rect->top,
                     rect->bottom - 1,light_color);
  gfx_draw_hline(surface,rect->left,rect->right - 1,
                     rect->bottom - 1,dark_color);
  gfx_draw_vline(surface,rect->right - 1,rect->top,
                     rect->bottom - 1,dark_color);
  return;
}

/* 005a9aa3 auto */
void __cdecl gfx_fill_surface(DibSurface *surface,i32 color)

{
  (surface)->fill(color);
  return;
}

/* 005a9abf auto */
void __cdecl
gfx_fill_rect_xywh(DibSurface *surface,i32 x,i32 y,int width,int height,
            i32 color)

{
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,x,y,width,height);
  (surface)->fill_rect((RECT *)rect,color);
  return;
}

/* 005a9afe auto */
void __cdecl
gfx_copy_rect_xywh(DibSurface *src,DibSurface *dest,i32 src_x,i32 src_y,i32 dest_x,
            i32 dest_y,int width,int height)

{
  i8 dest_rect[16];
  i8 src_rect[16];
  
  rect_set_xywh((RECT *)src_rect,src_x,src_y,width,height);
  rect_set_xywh((RECT *)dest_rect,dest_x,dest_y,width,height);
  (src)->DibSurface::blit(dest,(RECT *)src_rect,(RECT *)dest_rect);
  return;
}

/* 005a9b5d auto */
int __cdecl
gfx_tile_fill(DibSurface *dest,DibSurface *tile,int x,int y,int width,int height,int origin_x,
            int origin_y)

{
  int tile_width;
  int tile_height;
  int right;
  int bottom;
  int end_x;
  int span;
  int dist_y;
  int dist_x;
  int cur_x;
  int src_y;
  int src_x;
  int cur_y;
  
  tile_width = (tile)->surface_width();
  tile_height = (tile)->surface_height();
  cur_y = tile_height;
  if ((tile_width != 0) && (tile_height != 0)) {
    if (x == origin_x || x - origin_x < 0) {
      dist_x = ~(x - origin_x) + 1;
    }
    else {
      dist_x = x - origin_x;
    }
    if (y == origin_y || y - origin_y < 0) {
      dist_y = ~(y - origin_y) + 1;
    }
    else {
      dist_y = y - origin_y;
    }
    src_y = dist_y % tile_height;
    right = width + x;
    bottom = height + y;
    for (cur_y = y; cur_y < bottom; cur_y += span) {
      src_x = dist_x % tile_width;
      for (cur_x = x; cur_x < right; cur_x += span) {
        end_x = (cur_x + tile_width) - src_x;
        span = (tile_height + cur_y) - src_y;
        if (right <= end_x) {
          end_x = right;
        }
        if (bottom <= span) {
          span = bottom;
        }
        gfx_copy_rect_xywh(tile,dest,src_x,src_y,cur_x,cur_y,end_x - cur_x,
                           span - cur_y);
        span = tile_width - src_x;
        src_x = 0;
      }
      span = tile_height - src_y;
      src_y = 0;
    }
  }
  return cur_y;
}

/* 005ae006 auto */
int __cdecl bits_count8(uint bits)

{
  int i;
  i32 count;
  
  count = 0;
  for (i = 0; i < 8; i++) {
    if ((bits & 1) != 0) {
      count++;
    }
    bits = (int)bits >> 1;
  }
  return count;
}

/* 005ae3bf auto */
void __cdecl bit_index_split(int bit_index,int *out_byte,int *out_mask)

{
  *out_byte = bit_index >> 3;
  *out_mask = 1 << ((byte)bit_index & 7);
  return;
}
