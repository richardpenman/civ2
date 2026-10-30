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
void __cdecl rect_set(iptr rect,int left,int top,int right,int bottom)

{
  SetRect((LPRECT)rect,left,top,right,bottom);
  return;
}

/* 004086c0 auto */
void __cdecl rect_set_xywh(iptr rect,int x,int y,iptr width,iptr height)

{
  SetRect((LPRECT)rect,x,y,width + x,height + y);
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
gfx_target_fill_rect(i32 x,i32 y,i32 width,iptr height,
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
void __cdecl gfx_fill_rect(iptr surface,RECT *rect,i32 color)

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
void GameFont::set(i32 face,iptr size)

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
void __cdecl rect_offset(iptr rect,int dx,int dy)

{
  OffsetRect((LPRECT)rect,dx,dy);
  return;
}

/* 0043c7c0 auto */
void __cdecl gfx_draw_rect_frame(iptr surface,RECT *rect,i32 color)

{
  gfx_draw_box_ltrb(surface,rect->left,rect->top,
                     rect->right - 1,rect->bottom - 1,color);
  return;
}

/* 0043c8d0 auto */
i32 __cdecl gfx_draw_text(const char *text,i32 x,iptr y)

{
  i32 end_x;
  
  end_x = text_draw_shadowed(text_style.surface,text,x,y);
  return end_x;
}

/* 0043c910 auto */
void __cdecl gfx_draw_text_centered(const char *text,i32 x,iptr y,i32 width)

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
  loaded = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_terrain1_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
    sprite_grab_tile_cell((iptr)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c),x2,y2);
    sprite_grab_tile_cell((iptr)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78),res_x,y2);
    sprite_grab_tile_cell((iptr)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78),res2_x,y2);
    y2 += 0x21;
  }
  x2 = 0x1c8;
  y2 = 100;
  for (i = 0; (int)i < 3; i++) {
    ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell((iptr)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c),x2,y2);
    y2 += 0x21;
  }
  ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->free();
  sprite_grab_tile_cell((iptr)&pollution_sprite_block.pollution_sprite,x2,y2);
  y2 += 0x21;
  ((Sprite *)((i8 *)&grassland_shield_sprite))->free();
  sprite_grab_tile_cell((iptr)&grassland_shield_sprite,x2,y2);
  y2 += 0x21;
  ((Sprite *)((i8 *)&hut_sprite))->free();
  sprite_grab_tile_cell((iptr)&hut_sprite,x2,y2);
  y2 = 0x16c;
  x2 = 1;
  for (i = 0; (int)i < 9; i++) {
    ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell((iptr)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c),x2,y2);
    sprite_grab_tile_cell((iptr)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c),x2,y2 + 0x21);
    x2 += 0x41;
  }
  dither_mask.free();
  sprite_grab_tile_cell((iptr)&dither_mask,1,0x1bf);
  ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite))->free();
  sprite_grab_tile_cell((iptr)&tile_pick_mask_sprite_block.tile_pick_mask_sprite,0x42,0x1bf);
  ((Sprite *)((i8 *)&unexplored_tile_sprite))->free();
  sprite_grab_tile_cell((iptr)&unexplored_tile_sprite,0x83,0x1bf);
  ((Sprite *)((i8 *)&tile_direction_mask_sprite))->free();
  sprite_grab_tile_cell((iptr)&tile_direction_mask_sprite,0x105,0x1bf);
  blank_sprite.free();
  sprite_grab_tile_cell((iptr)&blank_sprite,0xc4,0x1bf);
  for (terrain2 = 0; terrain2 < 0xd; terrain2++) {
    gfx_fill_rect_xywh((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,0,0,0x40,0x20,7);
    if (terrain2 < 0xb) {
      ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->draw((iptr)grab_rect_a,(iptr)&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    else if (terrain2 == 0xb) {
      ((Sprite *)((i8 *)&unexplored_tile_sprite))->draw((iptr)grab_rect_b,(iptr)&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    else {
      blank_sprite.draw((iptr)grab_rect_c,(iptr)&gfx_sheet_surface_block.gfx_sheet_surface,0,0);
    }
    dither_mask.draw_keyed((iptr)grab_rect_d,(iptr)&gfx_sheet_surface_block.gfx_sheet_surface,10,0,0);
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
      sprite_grab_from_sheet((iptr)(((i8 *)&map_cursor_sprite_block.terrain_dither_sprites) + terrain2 * 0xf0 + quarter * 0x3c),x2,y2,0x20,
                   0x10);
    }
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  loaded = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_terrain2_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
    sprite_grab_tile_cell((iptr)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c),base_x + x2,base_y + y2);
    sprite_grab_tile_cell((iptr)(((i8 *)&forest_sprites) + i * 0x3c),base_x + x2,
                 base_y + y2 + 0x42);
    sprite_grab_tile_cell((iptr)(((i8 *)&mountain_sprites) + i * 0x3c),base_x + x2,
                 base_y + y2 + 0x84);
    sprite_grab_tile_cell((iptr)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c),base_x + x2,
                 base_y + y2 + 0xc6);
  }
  base_x = 1;
  base_y = 0x1ad;
  for (coast_idx = 0; coast_idx < 8; coast_idx++) {
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->free();
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0),base_x,base_y,0x20,0x10);
    sprite_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0),base_x + 0x21,base_y + 0x22,0x20,0x10);
    sprite_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0),base_x,base_y + 0x11,0x20,0x10);
    sprite_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0),base_x,base_y + 0x22,0x20,0x10);
    base_x += 0x42;
  }
  base_x = 1;
  base_y = 0x14b;
  for (i = 0; (int)i < 4; i++) {
    ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->free();
    sprite_grab_tile_cell((iptr)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c),base_x,base_y);
    base_x += 0x41;
  }
  ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->release();
  return;
}

/* 0044999c auto */
void __cdecl sprite_grab_tile_cell(iptr sprite,i32 x,i32 y)

{
  ((Sprite *)sprite)->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x40,0x20);
  ((Sprite *)sprite)->replace_color(9,7);
  return;
}

/* 004499d3 auto */
void __cdecl
sprite_grab_from_sheet(iptr sprite,i32 x,i32 y,i32 width,
            i32 height)

{
  ((Sprite *)sprite)->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,width,height);
  ((Sprite *)sprite)->replace_color(9,7);
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
  loaded = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_icons_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_icons_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_icons_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
  sprite_grab_tile_cell((iptr)&map_cursor_sprite_block.map_cursor_sprite,199,0x100);
  sprite_grab_tile_cell((iptr)&map_grid_sprite,199,0x121);
  sprite_grab_tile_cell((iptr)&icons_pair_sprites_block.city_radius_grid_sprite,0xf8,0x1ae);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_alt_sprite))->free();
  gfx_grab_fortify_overlay(1,0x20,0xbb);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_6))->free();
  sprite_grab_icon_cell_keyed((iptr)&fortify_overlay_sprites_block.icon_sprite_6,0x22,0xdf);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.pollution_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&fortify_overlay_sprites_block.pollution_icon_sprite,0x43,0xdf,0x20,0x20);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_7))->free();
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.icon_sprite_8))->free();
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.city_disorder_icon_sprite))->free();
  sprite_grab_icon_cell_keyed((iptr)&fortify_overlay_sprites_block.icon_sprite_7,100,0xdf);
  sprite_grab_icon_cell_keyed((iptr)&fortify_overlay_sprites_block.icon_sprite_8,0x85,0xdf);
  sprite_grab_from_sheet((iptr)&fortify_overlay_sprites_block.city_disorder_icon_sprite,0xa6,0xdf,0x1e,0x1e);
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_misc_sprites))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_1))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_4))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_5))->free();
  sprite_grab_icon_cell_keyed((iptr)&icon_misc_sprites_block.icon_misc_sprites,199,0x100);
  sprite_grab_icon_cell_keyed((iptr)&icon_misc_sprites_block.icon_sprite_1,0xe8,0x100);
  sprite_grab_icon_cell_keyed((iptr)&icon_misc_sprites_block.icon_sprite_4,0x109,0x100);
  sprite_grab_icon_cell_keyed((iptr)&icon_misc_sprites_block.icon_sprite_5,0x12a,0x100);
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_2))->free();
  ((Sprite *)((i8 *)&icon_misc_sprites_block.icon_sprite_3))->free();
  ((Sprite *)((i8 *)&icons_unused_tall_sprite))->free();
  sprite_grab_from_sheet((iptr)&icon_misc_sprites_block.icon_sprite_2,0x137,1,0x1f,0x60);
  sprite_grab_from_sheet((iptr)&icon_misc_sprites_block.icon_sprite_3,0x137,0x62,0x1f,0x60);
  sprite_grab_from_sheet((iptr)&icons_unused_tall_sprite,0x109,0x121,0x38,0x83);
  x2 = 1;
  y2 = 0x122;
  for (i = 0; i < 3; i++) {
    for (k = 0; k < 2; k++) {
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + i * 0x78))->free();
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78))->free();
      sprite_grab_square((iptr)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + i * 0x78),x2,y2,0xe);
      sprite_grab_square((iptr)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78),x2,y2 + 0xf,0xe);
    }
    x2 += 0xf;
  }
  x2 = 1;
  y2 = 0x140;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square((iptr)(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites) + i * 0x3c),x2,y2,0xe);
    x2 += 0xf;
  }
  x2 = 0x31;
  y2 = 0x14e;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square((iptr)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + i * 0x3c),x2,y2,10);
    x2 += 0xb;
  }
  x2 = 0x31;
  y2 = 0x159;
  for (i = 0; i < 3; i++) {
    ((Sprite *)(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites) + i * 0x3c))->free();
    sprite_grab_square((iptr)(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites) + i * 0x3c),x2,y2,10);
    x2 += 0xb;
  }
  ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->free();
  ((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite))->free();
  sprite_grab_square((iptr)&small_unhappy_icon_sprite,0x52,0x14e,10);
  sprite_grab_square((iptr)&unhappy_suppressed_icon_sprite,0x52,0x159,10);
  x2 = 0x31;
  y2 = 0x122;
  for (i = 0; i < 4; i++) {
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + i * 0x3c))->free();
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + i * 0x3c))->free();
    sprite_grab_square((iptr)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + i * 0x3c),x2,y2,0xe);
    sprite_grab_square((iptr)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + i * 0x3c),x2,0x131,0xe);
    x2 += 0xf;
  }
  x2 = 1;
  y2 = 0x164;
  for (i = 0; i < 8; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c),x2,y2,0x20,0x20);
    x2 += 0x21;
  }
  ((DibSurface *)((i8 *)&window_border_tile_surface))->DibSurface::create(0x40,0x20);
  gfx_copy_rect_xywh((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(iptr)&window_border_tile_surface,199,0x142,0,0,0x40,0x20);
  dialog_set_frame_tile_surface((iptr)&window_border_tile_surface);
  frame_set_border_tile((iptr)&window_border_tile_surface);
  ((DibSurface *)((i8 *)&panel_texture_surface))->DibSurface::create(0x20,0x20);
  gfx_copy_rect_xywh((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(iptr)&panel_texture_surface,0x12a,0xbe,0,0,0x20,0x20);
  (*(i8 * *)&dlg_style.client_tile_surface) = ((i8 *)&panel_texture_surface);
  x2 = 0x157;
  y2 = 1;
  col = 0;
  for (i = 1; i < 0x27; i++) {
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c),x2,y2,0x24,0x14);
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
    sprite_grab_from_sheet((iptr)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c),x2,y2,0x24,0x14);
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
      sprite_grab_from_sheet((iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + i * 0x3c + j * 0xf0),x2,y2,0x24,
                   0x14);
      x2 += 0x25;
    }
    y2 += 0x15;
  }
  ((Sprite *)((i8 *)&icons_pair_sprites_block.icons_pair_sprites))->free();
  ((Sprite *)((i8 *)&icons_pair_sprites_block.icon_sprite_9))->free();
  sprite_grab_from_sheet((iptr)&icons_pair_sprites_block.icons_pair_sprites,0x15e,0xff,0x40,0x20);
  sprite_grab_from_sheet((iptr)&icons_pair_sprites_block.icon_sprite_9,0x15e,0x120,0x40,0x20);
  ((Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite))->free();
  ((Sprite *)((i8 *)&caption_zoom_out_sprite))->free();
  ((Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite))->free();
  ((Sprite *)((i8 *)&caption_view_mode_button_sprite))->free();
  sprite_grab_from_sheet((iptr)&caption_close_button_sprite_block.caption_close_button_sprite,1,0x185,0x10,0x10);
  sprite_grab_from_sheet((iptr)&caption_zoom_out_sprite,0x12,0x185,0x10,0x10);
  sprite_grab_from_sheet((iptr)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,0x23,0x185,0x10,0x10);
  sprite_grab_from_sheet((iptr)&caption_view_mode_button_sprite,0x34,0x185,0x10,0x10);
  ((Sprite *)((i8 *)&scroll_up_button_sprite))->free();
  ((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite))->free();
  sprite_grab_from_sheet((iptr)&scroll_up_button_sprite,0xe3,0x185,0x12,0x18);
  sprite_grab_from_sheet((iptr)&city_down_arrow_sprite_block.city_down_arrow_sprite,0xf6,0x185,0x12,0x18);
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
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_editorpt_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded != 0) {
    x2 = 1;
    y2 = 0x1bf;
    for (i = 0; i < 0xf; i++) {
      ((Sprite *)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + i * 0x3c))->free();
      sprite_grab_from_sheet((iptr)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + i * 0x3c),x2,y2,0x20,0x20);
      x2 += 0x21;
    }
  }
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->free();
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,0x23f,1,0x40,0x30);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->free();
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,0x23f,0x32,0x40,0x20);
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
  loaded = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_people_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_people_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_people_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
      sprite_grab_from_sheet((iptr)(((i8 *)&road_sprites_block.citizen_sprites) + i * 0x3c + row * 0x294),x2,y2,0x1b
                   ,0x1e);
      x2 += 0x1c;
    }
    y2 += 0x1f;
  }
  x2 = 2;
  y2 = 0x89;
  for (i = 0; i < 0xb; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&government_sprites_block.small_citizen_sprites) + i * 0x3c),x2,y2,0xf,0x1a);
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
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x56,10,0xc0,(iptr)&palette);
  if (loaded == 0) {
    __chdir(((i8 *)&game_dirs.home));
    ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x56,10,0xc0,(iptr)&palette);
  }
  __chdir(((i8 *)&game_dirs.home));
  x2 = 2;
  y2 = 0x44;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprites))->free();
  ((Sprite *)((i8 *)&government_sprites_block.government_despotism_sprite))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_sprites,x2,y2,0x22,0x22);
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_despotism_sprite,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_communism))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_sprite_communism,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_fundamentalism))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_sprite_fundamentalism,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_monarchy_sprite))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_monarchy_sprite,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_democracy))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_sprite_democracy,x2,y2,0x22,0x22);
  x2 += 0x42;
  ((Sprite *)((i8 *)&government_sprites_block.government_sprite_republic))->free();
  sprite_grab_from_sheet((iptr)&government_sprites_block.government_sprite_republic,x2,y2,0x22,0x22);
  x2 = 2;
  y2 = 200;
  ((Sprite *)((i8 *)&ceasefire_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&ceasefire_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&war_icon_sprite_block.war_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&war_icon_sprite_block.war_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&signal_allied_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&signal_allied_icon_sprite,x2,y2,0x40,0x40);
  x2 += 0x42;
  ((Sprite *)((i8 *)&diplo_popup_portrait_sprite))->free();
  sprite_grab_from_sheet((iptr)&diplo_popup_portrait_sprite,x2,y2,0x40,0x40);
  ((Sprite *)((i8 *)&popup_info_icon_sprite))->free();
  ((Sprite *)((i8 *)&tutorial_icon_sprite_block.tutorial_icon_sprite))->free();
  sprite_grab_from_sheet((iptr)&popup_info_icon_sprite,0x18e,0x86,0x40,0x40);
  sprite_grab_from_sheet((iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0x86,0x18e,0x40,0x40);
  for (civ = 0; civ < 8; civ++) {
    ((Sprite *)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c),2,0x18e,0x40,0x40);
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
  loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x55,10,0xc0,(iptr)&palette);
  if (loaded == 0) {
    __chdir(((i8 *)&game_dirs.home));
    ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_resource(0x55,10,0xc0,(iptr)&palette);
  }
  __chdir(((i8 *)&game_dirs.home));
  x2 = 1;
  y2 = 1;
  frame_count = 0;
  for (i = 0; i < 0xb; i++) {
    ((Sprite *)(((i8 *)&hill_sprites_block.nuke_anim_sprites) + i * 0x3c))->free();
    sprite_grab_from_sheet((iptr)(((i8 *)&hill_sprites_block.nuke_anim_sprites) + i * 0x3c),x2,y2,0x5b,0x48);
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
void __cdecl sprite_grab_icon_cell_keyed(iptr sprite,i32 x,i32 y)

{
  ((Sprite *)sprite)->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x20,0x20);
  ((Sprite *)sprite)->replace_color(9,7);
  return;
}

/* 0044aba9 auto */
void __cdecl sprite_grab_square(iptr sprite,i32 x,i32 y,i32 size)

{
  sprite_grab_from_sheet(sprite,x,y,size,size);
  return;
}

/* 0044abd5 auto */
void __cdecl gfx_grab_fortify_overlay(int index,i32 x,i32 y)

{
  sprite_grab_city_cell((iptr)(((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites) + index * 0x3c),x,y);
  return;
}

/* 0044ac07 auto */
void __cdecl sprite_grab_city_cell(iptr sprite,i32 x,i32 y)

{
  ((Sprite *)sprite)->grab_from_surface((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,7,x,y,0x40,0x30);
  ((Sprite *)sprite)->replace_color(9,7);
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
  cell_x = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_cities_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (cell_x == 0) {
    cell_x = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_cities_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (cell_x == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_cities_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
      sprite_grab_city_cell((iptr)(((i8 *)&gfx_sheet_surface_block.city_sprites) + style2 * 0x1e0 + size_idx * 0x78),cell_x + 1,y2);
      city_sprite_scan_flag_offset(style2,size_idx,0,cell_x + 1,y2);
      city_sprite_scan_size_box_offset(style2,size_idx,0,cell_x + 1,y2);
      sprite_grab_city_cell((iptr)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + style2 * 0x1e0 + size_idx * 0x78),cell_x + 0x14e,y2)
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
    sprite_grab_from_sheet((iptr)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78),flag_x,0x1a9,0xe,0x16);
    sprite_grab_from_sheet((iptr)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78),flag_x,0x1c0,0xe,0x16);
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
  sprite_grab_city_cell((iptr)&fortify_overlay_sprites_block.fortify_overlay_sprites,0x8f,0x1a7);
  sprite_grab_city_cell((iptr)&fortify_overlay_sprites_block.fortress_sprite,0xd0,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->free();
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->free();
  sprite_grab_city_cell((iptr)&airbase_sprites_block.airbase_sprites,0x111,0x1a7);
  sprite_grab_city_cell((iptr)&airbase_sprites_block.airbase_occupied_sprite,0x152,0x1a7);
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
  loaded = gfx_load_bmp_640x480((iptr)&gfx_sheet_surface_block.gfx_sheet_surface,(const char *)&str_units_bmp,10,0xc0,(iptr)&game_palette_block.game_palette);
  if (loaded == 0) {
    loaded = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_units_gif,10,0xc0,(iptr)&game_palette_block.game_palette);
    if (loaded == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->load_gif_file((const char *)&str_units_gif_2,10,0xc0,(iptr)&game_palette_block.game_palette);
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
    sprite_grab_city_cell((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),x2,y2);
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
  sprite_grab_from_sheet((iptr)&unit_shield_sprite,0x24a,1,0xc,0x14);
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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_handle_fullcolors,(iptr)&str_d_ss_franklinton_grey_cpp,0x22);
  }
  else {
    rgb_data = (iptr)mem_handle_lock(rgb_handle);
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb(rgb_data,10,0x20);
    handle = mem_handle_alloc(0x88);
    if (handle == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_hlocpal,(iptr)&str_d_ss_franklinton_grey_cpp_2,0x2a);
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
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_handle_palette,(iptr)&str_d_ss_franklinton_grey_cpp_3,0x3e);
        mem_handle_unlock(rgb_handle);
        mem_handle_free(rgb_handle);
      }
      else {
        mem_handle_unlock(handle);
        mem_handle_free(handle);
        handle = mem_handle_alloc(0x2c4);
        if (handle == 0) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_handle_colors,(iptr)&str_d_ss_franklinton_grey_cpp_4,0x48);
          mem_handle_unlock(rgb_handle);
          mem_handle_free(rgb_handle);
        }
        else {
          full_rgb = (iptr)mem_handle_lock(handle);
          grey_palette_block.grey_remap_handle = mem_handle_alloc(0xec);
          if (grey_palette_block.grey_remap_handle == 0) {
            ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_handle_grey,(iptr)&str_d_ss_franklinton_grey_cpp_5,0x51);
            mem_handle_unlock(rgb_handle);
            mem_handle_free(rgb_handle);
            mem_handle_unlock(handle);
            mem_handle_free(handle);
          }
          else {
            remap_table = (iptr)mem_handle_lock(grey_palette_block.grey_remap_handle);
            ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb(full_rgb,10,0xec);
            for (offset = 0; offset < 0x2c4; offset += 3) {
              nearest_index = GetNearestPaletteIndex
                                (h,CONCAT12(*(i8 *)(offset + 2 + full_rgb),
                                                  CONCAT11(*(i8 *)(offset + 1 + full_rgb),
                                                           *(i8 *)(offset + full_rgb))));
              *(char *)(remap_table + offset / 3) = (char)nearest_index + '\n';
              ((GamePalette *)((i8 *)&grey_palette_block.grey_palette))->set_entry(offset / 3 + 10,
                         CONCAT31((int3)(nearest_index * 3 >> 8),*(i8 *)(nearest_index * 3 + rgb_data)),
                         CONCAT31((int3)(nearest_index * 3 >> 8),*(i8 *)(nearest_index * 3 + 1 + rgb_data)),
                         CONCAT31((int3)(nearest_index * 3 >> 8),*(i8 *)(nearest_index * 3 + 2 + rgb_data)));
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
        map_views_block.map_view_recs[view].remap_colors(remap_table,10,0xec);
      }
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->remap_colors(remap_table,10,0xec);
    ((DibSurface *)((i8 *)&minimap_state.window))->remap_colors(remap_table,10,0xec);
    ((DibSurface *)((i8 *)&city_wnd.window))->remap_colors(remap_table,10,0xec);
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
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_begin(10,0xec,10,(iptr)&grey_palette_block.grey_palette);
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
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->fade_begin(10,0xec,10,(iptr)&grey_palette_block.grey_palette);
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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_p,(iptr)&str_d_ss_franklinton_grey_cpp_6,0xbc);
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
int __cdecl gfx_load_bmp_640x480(iptr surface,const char *filename,int first_color,int num_colors,iptr palette)

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
              logpal = ((GamePalette *)palette)->get_logpalette();
              palette_set_entry_rgb((GameLogPalette *)logpal,tmp,blue,green,red);
              src_index++;
            }
            ((GamePalette *)palette)->commit_range(first_color,num_colors);
          }
          ((DibSurface *)surface)->DibSurface::create(info.width2,info.height2);
          color_base = (char)first_color;
          if (info.compression == 0) {
            while( true ) {
              row = info.height2 - 1;
              if ((row < 0) || (ok == 0)) break;
              dst = (char *)((DibSurface *)surface)->pixel_ptr(0,row);
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
            bmp_lo = (char *)((DibSurface *)surface)->pixel_ptr(0,0);
            bmp_hi = (char *)((DibSurface *)surface)->pixel_ptr(0,info.height2 - 1);
            if (bmp_hi < bmp_lo) {
              dst = bmp_lo;
              bmp_lo = bmp_hi;
              bmp_hi = dst;
            }
            bmp_hi += info.width2;
            dst = (char *)((DibSurface *)surface)->pixel_ptr(0,row);
            while( true ) {
              code = CRT_GETC(file);
              if (code == 0xffffffff) break;
              if (code == 0) {
                code = CRT_GETC(file);
                if (code == 0) {
                  row--;
                  dst = (row < 0) ? (char *)0x0 : (char *)((DibSurface *)surface)->pixel_ptr(0,row);
                }
                else {
                  if (code == 1) break;
                  if (code == 2) {
                    tmp = CRT_GETC(file);
                    row -= tmp;
                    tmp = CRT_GETC(file);
                    col += tmp;
                    dst = ((row < 0) || (info.height2 <= row)) ? (char *)0x0 :
                          (char *)((DibSurface *)surface)->pixel_ptr(col,row);
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
int __cdecl gfx_save_bmp_640x480(iptr surface,const char *filename,int first_color,int num_colors,iptr palette)

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
      logpal = ((GamePalette *)palette)->get_logpalette();
      palette_get_entry_rgb((GameLogPalette *)logpal,tmp,(iptr)red_ptr,(iptr)green_ptr,(iptr)blue_ptr);
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
            src = (byte *)((DibSurface *)surface)->pixel_ptr(0,row);
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
void __cdecl rect_intersect_into(LPRECT dest,iptr rect_a,iptr rect_b)

{
  IntersectRect(dest,(RECT *)rect_a,(RECT *)rect_b);
  return;
}

/* 0049621d auto */
PortraitDialog *PortraitDialog::construct()

{
  
  SurfaceWindow::construct();
  surface_ctor(&surface);
  button_ctor(&cancel_button);
  button_ctor(&ok_button);
  dll.init();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3);
  civ = 0;
  field_1e0 = 0;
  return this;
}

/* 0049632a auto */
/* Library Function - Single Match
    public: virtual __thiscall CCommandLineInfo::~CCommandLineInfo(void)
   
   Library: Visual Studio 1998 Debug */

void PortraitDialog::destruct()

{
  int eh_state;
  
  *(i8 ***)&vtable = (i8 **)&leader_anim_frame_starts_block.ptr_fun_3;
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 004963a5 */ { ((DllHandle *)(((int)this) + 0x1d4))->free(); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 004963b4 */ { ctrl_dtor((Button *)(((int)this) + 0x198)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 004963c3 */ { ctrl_dtor((Button *)(((int)this) + 0x15c)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 004963d2 */ { surface_dtor((DibSurface *)(((int)this) + 0x114)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004963e1 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 00496402 auto */
i32 PortraitDialog::create()

{
  wnd_create_with_surface((const char *)&str_empty_109,0xc00,0,0,ui_metrics.border_width * 8 + 0x276,
             ui_metrics.border_width * 4 + ui_metrics.caption_height + 0x16e,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
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
  ((Button *)((u8 *)this + 0x198))->create_with_callback((iptr)parent_a,0x65,(iptr)&button_rect,0x21,(const char *)&str_ok);
  OffsetRect(&button_rect,((ui_metrics.border_width * 8 + 0x276 >> 1) + 2) - ui_metrics.border_width,0);
  if (this == (PortraitDialog *)0x0) {
    parent_b = (u8 *)0x0;
  }
  else {
    parent_b = (u8 *)this + 0x48;
  }
  ((Button *)((u8 *)this + 0x15c))->create_with_callback((iptr)parent_b,100,(iptr)&button_rect,0x21,(const char *)&str_cancel);
  ((Button *)((u8 *)this + 0x15c))->set_callback(portrait_dlg_on_button);
  ((Button *)((u8 *)this + 0x198))->set_callback(portrait_dlg_on_button);
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
                     (iptr)&game_palette_block.game_palette);
  if (loaded != 0) {
    dll.dll_handle_free();
    loaded = ui_metrics.caption_height + ui_metrics.bevel_width * -2;
    height = get_width();
    gfx_tile_fill((iptr)this,ui_metrics.border_tile,0,ui_metrics.bevel_width,height,loaded,0,ui_metrics.bevel_width);
    height = get_height();
    loaded = ui_metrics.border_width + ui_metrics.bevel_width * -2;
    x2 = ui_metrics.bevel_width;
    tmp_a = ui_metrics.caption_height - ui_metrics.bevel_width;
    for (i = 0; i < 8; i++) {
      gfx_tile_fill((iptr)this,ui_metrics.border_tile,x2,tmp_a,loaded,height,x2,tmp_a);
      x2 = x2 + ui_metrics.border_width + 0x5a;
    }
    tmp_b = ui_metrics.border_width + ui_metrics.bevel_width * -2;
    y2 = ui_metrics.bevel_width + ui_metrics.caption_height + 0x6e;
    tmp_c = get_width();
    tmp_a = ui_metrics.bevel_width;
    loaded = ui_metrics.bevel_width * -2;
    for (i = 0; i < 2; i++) {
      gfx_tile_fill((iptr)this,ui_metrics.border_tile,tmp_a,y2,tmp_c + loaded,tmp_b,tmp_a,y2);
      y2 = y2 + ui_metrics.border_width + 0x6e;
    }
    loaded = ui_metrics.border_width * 2;
    tmp_c = get_width();
    tmp_b = ui_metrics.bevel_width;
    tmp_a = ui_metrics.bevel_width * -2;
    tmp_d = get_height();
    tmp_d = tmp_d + ui_metrics.border_width * -2 - 0x24 + ui_metrics.bevel_width;
    gfx_tile_fill((iptr)this,ui_metrics.border_tile,tmp_b,tmp_d,tmp_c + tmp_a,loaded + 0x24,tmp_b,tmp_d);
    SetRect(&dst_rect,ui_metrics.border_width - ui_metrics.bevel_width,ui_metrics.caption_height - ui_metrics.bevel_width,
            ui_metrics.bevel_width * 2 + (ui_metrics.border_width - ui_metrics.bevel_width) + 0x5a,
            ui_metrics.bevel_width * 2 + (ui_metrics.caption_height - ui_metrics.bevel_width) + 0x6e);
    SetRect(&src_rect,1,1,0x5b,0x6f);
    for (i = 0; i < 3; i++) {
      for (col = 0; col < 7; col++) {
        for (b = 0; b < ui_metrics.bevel_width; b++) {
          gfx_draw_bevel_rect((iptr)this,&dst_rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
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
      gfx_draw_bevel_rect((iptr)this,&dst_rect,ui_metrics.highlight_color,ui_metrics.shadow_color);
      InflateRect(&dst_rect,-1,-1);
    }
    SetRect(&dst_rect,ui_metrics.border_width - ui_metrics.bevel_width,ui_metrics.caption_height - ui_metrics.bevel_width,
            ui_metrics.bevel_width * 2 + (ui_metrics.border_width - ui_metrics.bevel_width) + 0x5a,
            ui_metrics.bevel_width * 2 + (ui_metrics.caption_height - ui_metrics.bevel_width) + 0x6e);
    for (b = 0; b < ui_metrics.bevel_width; b++) {
      gfx_draw_bevel_rect((iptr)this,&dst_rect,0x5e,0x5e);
      InflateRect(&dst_rect,-1,-1);
    }
    tmp_a = get_width();
    tmp_b = ((GameFont *)((i8 *)&caption_font_block.caption_font))->text_width((const char *)title);
    loaded = ui_metrics.bevel_width;
    tmp_b = (tmp_a >> 1) - (tmp_b + 4 >> 1);
    tmp_a = ui_metrics.bevel_width + 1;
    gfx_set_text_color_index(0x12);
    draw_text((iptr)&caption_font_block.caption_font,(const char *)title,tmp_b + 3,loaded + 2,5);
    gfx_set_text_color_index(0x29);
    draw_text((iptr)&caption_font_block.caption_font,(const char *)title,tmp_b + 1,tmp_a,5);
    draw_text((iptr)&caption_font_block.caption_font,(const char *)title,tmp_b,tmp_a,5);
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
      gfx_draw_bevel_rect((iptr)dlg,&new_rect,0x5e,0x5e);
      InflateRect(&new_rect,-1,-1);
      gfx_draw_bevel_rect((iptr)dlg,&old_rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
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
  iptr dlg;
  
  child = wnd_get_active_child();
  if (child == 0) {
    dlg = 0;
  }
  else {
    dlg = child - 0x48;
  }
  if (((button_id != 100) && (button_id == 0x65)) &&
     (*(i32 *)(dlg + 0x1e4) = *(i32 *)(dlg + 0x1e0),
     leaders[civs[*(int *)(dlg + 0x1dc)].tribe].female != '\0')) {
    *(int *)(dlg + 0x1e4) = *(int *)(dlg + 0x1e4) + 0x15;
  }
  ((ModalMenuWindow *)(dlg + 0x48))->end_modal_loop();
  return;
}

/* 00497c40 auto */
void GamePalette::get_entry(iptr index,iptr red,iptr green,iptr blue)

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
void __cdecl rect_deflate(iptr rect,int dx,int dy)

{
  rect_inflate(rect,-dx,-dy);
  return;
}

/* 004bb840 auto */
void __cdecl rect_inflate(iptr rect,int dx,int dy)

{
  InflateRect((LPRECT)rect,dx,dy);
  return;
}

/* 004ccb6a auto */
void __cdecl gfx_draw_tiled_frame(iptr surface,int x,int y,int width,int height,int border)

{
  int left;
  int top;
  i32 extent;
  int offset;
  int i;
  RECT rect;
  
  left = x - border;
  top = y - border;
  rect_set_xywh((iptr)((i8 *)&rect.left),left,top,width + border * 2,height + border * 2);
  extent = rect_width((RECT *)(iptr)((i8 *)&rect.left));
  gfx_tile_fill(surface,(iptr)&window_border_tile_surface,left,top + 2,extent,border - 4,0,2);
  offset = rect_height((RECT *)(iptr)((i8 *)&rect.left));
  offset -= (border - 2);
  gfx_tile_fill(surface,(iptr)&window_border_tile_surface,left,offset + top,extent,border - 4,0,offset);
  extent = rect_height((RECT *)(iptr)((i8 *)&rect.left));
  gfx_tile_fill(surface,(iptr)&window_border_tile_surface,left + 2,top,border - 4,extent,2,0);
  offset = rect_width((RECT *)(iptr)((i8 *)&rect.left));
  offset -= (border - 2);
  gfx_tile_fill(surface,(iptr)&window_border_tile_surface,offset + left,top,border - 4,extent,offset,0);
  for (i = 0; i < 2; i++) {
    gfx_draw_bevel_rect(surface,(RECT *)(iptr)((i8 *)&rect.left),0x25,0x12);
    rect_deflate((iptr)((i8 *)&rect.left),1,1);
  }
  rect_deflate((iptr)((i8 *)&rect.left),border - 4,0);
  rect.top = rect.top + border - 4;
  rect.bottom -= (border - 4);
  for (i = 0; i < 2; i++) {
    gfx_draw_bevel_rect(surface,(RECT *)(iptr)((i8 *)&rect.left),0x12,0x25);
    rect_deflate((iptr)((i8 *)&rect.left),1,1);
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
sprite_draw_spaced_row(iptr surface,iptr sprite,int x,i32 y,int count,int slots,
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
  spacing = ui_calc_item_spacing(slots,spacing,item_width,(iptr)&slots,(iptr)&remainder);
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
    ((Sprite *)sprite)->draw((iptr)clip_rect,surface,x,y);
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
gfx_copy_rect_same_pos(iptr src,iptr dst,i32 x,i32 y,i32 width,
            i32 height)

{
  gfx_copy_rect_xywh(src,dst,x,y,x,y,width,height);
  return;
}

/* 005683c5 auto */
i32 __cdecl gfx_dissolve_blit(iptr surface,iptr rect,uint cell_size,int mode)

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
    lo = ((DibSurface *)surface)->get_height();
    pick = ((DibSurface *)surface)->get_width();
    SetRect(&area2,0,0,pick,lo);
  }
  else {
    CopyRect(&area2,(RECT *)rect);
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
        ((DibSurface *)surface)->wnd_blit_surface_rect((iptr)&cell_rect);
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
        ((DibSurface *)surface)->wnd_blit_surface_rect((iptr)&cell_rect);
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
gfx_draw_unit(iptr surface,iptr unit,uint draw_flags,int px,int py,int zoom,int no_fortress)

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
    rect_set_xywh((iptr)&shield_rect.left,stack_dx + shield_x,shield_y,shield_width,shield_height);
    if ((int)stack_dx < 1) {
      abs_dx = ~stack_dx + 1;
    }
    else {
      abs_dx = stack_dx;
    }
    if (2 < (int)abs_dx) {
      ((Sprite *)((i8 *)&unit_shield_sprite))->draw((iptr)clip_stack_shadow,surface,
                 side + stack_dx + shield_x,stack_dy + shield_y + 1);
      rect_offset((iptr)&shield_rect.left,side,1);
      ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x106,0xf);
      ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x105,0xf);
      rect_offset((iptr)&shield_rect.left,-side,0xffffffff);
    }
    ((Sprite *)((i8 *)&unit_shield_sprite))->draw((iptr)clip_stack,surface,stack_dx + shield_x,
               stack_dy + shield_y);
    ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x106,color_mid);
    ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x105,color_mid);
  }
  rect_set_xywh((iptr)&shield_rect.left,shield_x,shield_y,shield_width,shield_height);
  ((Sprite *)((i8 *)&unit_shield_sprite))->draw((iptr)clip_shield_shadow,surface,side + shield_x,shield_y + 1
            );
  rect_offset((iptr)&shield_rect.left,side,1);
  ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x106,0x10);
  ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x105,0x10);
  rect_offset((iptr)&shield_rect.left,-side,0xffffffff);
  ((Sprite *)((i8 *)&unit_shield_sprite))->draw((iptr)clip_shield,surface,shield_x,shield_y);
  ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x106,color_primary);
  ((DibSurface *)surface)->replace_color((iptr)&shield_rect.left,0x105,color_mid);
  scaled = gfx_zoom_scale(2,zoom);
  rect_set_xywh((iptr)&band_rect.left,shield_x,shield_y,shield_width,scaled);
  gfx_fill_rect(surface,(RECT *)&band_rect.left,10);
  tmp = rect_height((RECT *)&band_rect.left);
  shield_rect.top += tmp;
  scaled = gfx_zoom_scale(3,zoom);
  rect_set_xywh((iptr)&hp_rect.left,shield_x,shield_rect.top,shield_width,scaled);
  tmp = rect_height((RECT *)&hp_rect.left);
  shield_rect.top += tmp;
  scaled = gfx_zoom_scale(2,zoom);
  rect_set_xywh((iptr)&band_rect.left,shield_x,shield_rect.top,shield_width,scaled);
  gfx_fill_rect(surface,(RECT *)&band_rect.left,10);
  tmp = rect_height((RECT *)&band_rect.left);
  shield_rect.top += tmp;
  bar_width = rect_width((RECT *)&hp_rect.left);
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
    gfx_fill_rect(surface,(RECT *)&hp_rect.left,10);
  }
  if (hp_width != 0) {
    band_rect.left = hp_rect.left;
    band_rect.top = hp_rect.top;
    band_rect.bottom = hp_rect.bottom;
    band_rect.right = hp_rect.right - (bar_width - hp_width);
    gfx_fill_rect(surface,(RECT *)&band_rect.left,hp_color);
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
    tmp = rect_height((RECT *)&shield_rect.left);
    text_y = shield_rect.top + tmp / 2;
    tmp = rect_width((RECT *)&shield_rect.left);
    ((DibSurface *)surface)->draw_text((iptr)&status_window_block.map_label_font,(const char *)order_text,shield_rect.left + tmp / 2,text_y,value);
  }
  silhouette = (uint)(units[unit2].orders == ORDER_SLEEP);
  if ((((unit_types[units[unit2].type].domain == DOMAIN_AIR) &&
       (tmp = map_in_bounds((int)units[unit2].x,(int)units[unit2].y), tmp != 0)) &&
      (tmp = tile_airbase_owner((int)units[unit2].x,(int)units[unit2].y), -1 < tmp)) &&
     (((game.active_unit != unit2 || (moving_units_mode != 1)) || (blink.unit_active == '\0')))) {
    silhouette = 1;
  }
  if (silhouette == 0) {
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c))->draw((iptr)clip_unit,surface,px,
               py);
  }
  else if (extra_ui(X_SENTRY_GREY)) {
    /* X41: in grey halftones, not a grey shape */
    extras_draw_grey_sprite((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c),
                            surface,px,py);
  }
  else {
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + sprite_type * 0x3c))->draw_silhouette((iptr)clip_silhouette,surface,px,
               py,0x1a);
  }
  if (units[unit2].orders == ORDER_FORTIFIED) {
    ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->draw((iptr)clip_fortify,surface,px,py);
  }
  tmp = map_in_bounds((int)units[unit2].x,(int)units[unit2].y);
  if (((tmp != 0) &&
      (value = tile_improvements((int)units[unit2].x,(int)units[unit2].y),
      ((byte)value & TILE_AIRBASE) == 0x40)) && (no_fortress == 0)) {
    ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->draw((iptr)clip_fortress,surface,px,py);
  }
  if (extra_ui(X_WORK_COUNTER)) {
    extras_draw_work_counter(surface,unit2,px,py,zoom);    /* X3 */
  }
  gfx_reset_zoom_scale();
  return;
}

/* 0056c5fc auto */
void __cdecl
gfx_copy_rect_clipped(iptr src,iptr dst,int src_x,int src_y,int src_origin_x,int src_origin_y,int dst_x,
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
gfx_draw_city(iptr surface,iptr city,uint draw_flags,int px,int py,int zoom)

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
  rect_set_xywh((iptr)&label_rect.left,px,py,width2,height2);
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
  ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + walled * 0x3c + style2 * 0x1e0))->draw((iptr)clip_city,surface,px,py);
  tmp = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
  if ((-1 < tmp) && ((draw_flags & 2) == 0)) {
    flag_x = gfx_zoom_scale(peace_treaty_icon_sprite_block.city_flag_x_offsets[style2 * 8 + walled + size_idx * 2],
                                  zoom);
    flag_x = px + flag_x;
    flag_y = gfx_zoom_scale(peace_treaty_icon_sprite_block.city_flag_y_offsets[style2 * 8 + walled + size_idx * 2],
                                  zoom);
    flag_y = py + flag_y;
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->draw((iptr)clip_flag,surface,flag_x,
               flag_y);
  }
  if ((draw_flags & 1) == 0) {
    if ((cities[city].flags & CITYF_DISORDER) == 0) {
      gfx_select_zoom_font(zoom);
      text_set_font((iptr)&status_window_block.map_label_font);
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
      rect_set_xywh((iptr)&label_rect.left,0,0,width2,height2);
      tmp = gfx_zoom_scale(road_sprites_block.city_label_y_offsets[style2 * 8 + walled + size_idx * 2],
                                 zoom);
      tmp = py + tmp;
      box_dx = gfx_zoom_scale(road_sprites_block.city_size_box_x_offsets[style2 * 8 + walled + size_idx * 2],
                                 zoom);
      rect_offset((iptr)&label_rect.left,px + box_dx,tmp);
      gfx_draw_rect_frame(surface,(RECT *)&label_rect.left,10);
      rect_deflate((iptr)&label_rect.left,1,1);
      gfx_fill_rect(surface,(RECT *)&label_rect.left,color_primary);
      text_set_colors(10,10,1,0);
      gfx_draw_text((const char *)size_text,label_rect.left + (pad >> 1) + 1,label_rect.top);
    }
    else {
      ((Sprite *)((i8 *)&fortify_overlay_sprites_block.city_disorder_icon_sprite))->draw((iptr)clip_disorder,surface,px,py);
      ((DibSurface *)surface)->replace_color((iptr)&label_rect.left,0x106,color_primary);
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
  ((SpritePicker *)((i8 *)&sprite_editor.sprite_picker_window))->construct();
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
  ((SpritePicker *)((i8 *)&sprite_editor.sprite_picker_window))->destruct();
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_units_bmp_2,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_units_gif_3,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_units_gif_4,10,0xc0,(iptr)&palette);
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_backdrop,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((iptr)clip_unit,(iptr)&sprite_editor.sprite_sheet_surface,x2
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
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_units_bmp_5,10,0xc0,(iptr)&palette);
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_cities_bmp_2,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_cities_gif_3,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_cities_gif_4,10,0xc0,(iptr)&palette);
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
      gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,cell_x + x2,y2,width,height,fill_color);
      ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_city_bg,(iptr)&sprite_editor.sprite_sheet_surface,cell_x + x2,
                 y2);
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->draw(
                 (iptr)clip_city,(iptr)&sprite_editor.sprite_sheet_surface,cell_x + x2,y2);
      key_color = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->get_key_color();
      fill_color = (uint)key_color;
      height = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->height();
      width = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->width();
      gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,cell_x + walled_x,y2,width,height,fill_color);
      ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_walled_bg,(iptr)&sprite_editor.sprite_sheet_surface,cell_x + walled_x,
                 y2);
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->draw(
                 (iptr)clip_walled,(iptr)&sprite_editor.sprite_sheet_surface,cell_x + walled_x,y2);
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,flag_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->draw((iptr)clip_flag,(iptr)&sprite_editor.sprite_sheet_surface,
               x2,flag_y);
    key_color = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->height();
    width = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,flag_dark_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->draw((iptr)clip_flag_dark,(iptr)&sprite_editor.sprite_sheet_surface,
               x2,flag_dark_y);
    x2 += 0xf;
  }
  key_color = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->height();
  width = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,0x8f,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_fortify_bg,(iptr)&sprite_editor.sprite_sheet_surface,0x8f,0x1a7);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->draw((iptr)clip_fortify,(iptr)&sprite_editor.sprite_sheet_surface,0x8f,0x1a7);
  key_color = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->height();
  width = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,0xd0,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_fortress_bg,(iptr)&sprite_editor.sprite_sheet_surface,0xd0,0x1a7);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->draw((iptr)clip_fortress,(iptr)&sprite_editor.sprite_sheet_surface,0xd0,0x1a7);
  key_color = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->height();
  width = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,0x111,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_airbase_bg,(iptr)&sprite_editor.sprite_sheet_surface,0x111,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->draw((iptr)clip_airbase,(iptr)&sprite_editor.sprite_sheet_surface,0x111,0x1a7);
  key_color = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->height();
  width = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,0x152,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)clip_airbase_occ_bg,(iptr)&sprite_editor.sprite_sheet_surface,0x152,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->draw((iptr)clip_airbase_occ,(iptr)&sprite_editor.sprite_sheet_surface,0x152,0x1a7);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_cities_bmp_3);
  if ((result == 0) || (result = crt_remove(str_cities_bmp_4), result == 0)) {
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_cities_bmp_5,10,0xc0,(iptr)&palette);
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_terrain1_bmp_2,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_3,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_4,10,0xc0,(iptr)&palette);
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)base_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->draw((iptr)base_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    key_color = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->height();
    width = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,res_x,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)res_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,res_x,y2);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->draw((iptr)res_rect,(iptr)&sprite_editor.sprite_sheet_surface,res_x
               ,y2);
    key_color = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->height();
    width = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,res2_x,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)res2_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,res2_x,y2);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->draw((iptr)res2_rect,(iptr)&sprite_editor.sprite_sheet_surface,res2_x
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)impr_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->draw((iptr)impr_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    y2 += 0x21;
  }
  key_color = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->height();
  width = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)pollution_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->draw((iptr)pollution_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 += 0x21;
  key_color = ((Sprite *)((i8 *)&grassland_shield_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&grassland_shield_sprite))->height();
  width = ((Sprite *)((i8 *)&grassland_shield_sprite))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)shield_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&grassland_shield_sprite))->draw((iptr)shield_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 += 0x21;
  key_color = ((Sprite *)((i8 *)&hut_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&hut_sprite))->height();
  width = ((Sprite *)((i8 *)&hut_sprite))->width();
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)hut_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&hut_sprite))->draw((iptr)hut_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 = 0x16c;
  x2 = 1;
  for (i = 0; i < 9; i++) {
    key_color = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)road_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->draw((iptr)road_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    key_color = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2 + 0x21,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)rail_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2 + 0x21);
    ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->draw((iptr)rail_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
               ,y2 + 0x21);
    x2 += 0x41;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_terrain1_bmp_3);
  if ((result == 0) || (result = crt_remove(str_terrain1_bmp_4), result == 0)) {
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_terrain1_bmp_5,10,0xc0,(iptr)&palette);
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_terrain2_bmp_2,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_3,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_4,10,0xc0,(iptr)&palette);
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)river_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y);
    ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->draw((iptr)river_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y);
    key_color = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0x42,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)forest_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0x42);
    ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->draw((iptr)forest_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0x42);
    key_color = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0x84,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)mountain_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0x84);
    ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->draw((iptr)mountain_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0x84);
    key_color = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0xc6,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)hill_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0xc6);
    ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->draw((iptr)hill_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0xc6);
  }
  base_x = 1;
  base_y = 0x14b;
  for (i = 0; (int)i < 4; i++) {
    key_color = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x,base_y,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)mouth_bg_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x,base_y);
    ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->draw((iptr)mouth_rect,(iptr)&sprite_editor.sprite_sheet_surface,base_x
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x,base_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->draw((iptr)coast_n_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x,base_y);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x + 0x21,base_y + 0x22,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->draw((iptr)coast_e_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x + 0x21,base_y + 0x22);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x,base_y + 0x11,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->draw((iptr)coast_s_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x,base_y + 0x11);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,base_x,base_y + 0x22,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->draw((iptr)coast_w_rect,(iptr)&sprite_editor.sprite_sheet_surface,
               base_x,base_y + 0x22);
    base_x += 0x42;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_terrain2_bmp_3);
  if ((result == 0) || (result = crt_remove(str_terrain2_bmp_4), result == 0)) {
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_terrain2_bmp_5,10,0xc0,(iptr)&palette);
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_icons_bmp_2,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_3,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_4,10,0xc0,(iptr)&palette);
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
      gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
      ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + category2 * 0xf0 + row * 0x3c))->draw(
                 (iptr)icon_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2,y2);
      x2 += 0x25;
    }
    y2 += 0x15;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_icons_bmp_3);
  if ((result == 0) || (result = crt_remove(str_icons_bmp_4), result == 0)) {
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_icons_bmp_5,10,0xc0,(iptr)&palette);
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
  result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_icons_bmp_6,10,0xc0,(iptr)&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_5,10,0xc0,(iptr)&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_6,10,0xc0,(iptr)&palette);
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->draw((iptr)improvement_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
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
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->draw((iptr)wonder_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
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
  gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,199,0x100,width,height,fill_color);
  ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->draw((iptr)cursor_rect,(iptr)&sprite_editor.sprite_sheet_surface,199,0x100);
  x2 = 1;
  y2 = 0x164;
  for (i = 0; i < 8; i++) {
    key_color = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh((iptr)&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->draw((iptr)explosion_rect,(iptr)&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    x2 += 0x21;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_icons_bmp_7);
  if ((result == 0) || (result = crt_remove(str_icons_bmp_8), result == 0)) {
    gfx_save_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)&str_icons_bmp_9,10,0xc0,(iptr)&palette);
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
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(iptr)&game_palette_block.game_palette,
             parent_hwnd);
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
  
  rect_set_xywh((iptr)rect,0,0,sprite_editor.width,sprite_editor.height);
  ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(0x109,0x107);
  sprite_editor.target_sprite->grab_surface_rect((iptr)&sprite_editor.surface,(iptr)rect);
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
  rect_set_xywh((iptr)rect,0,0,sprite_editor.width,sprite_editor.height);
  ((Sprite *)((i8 *)&sprite_editor.result_sprite))->grab_rect((iptr)&sprite_editor.surface,0xfe,(iptr)rect);
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
  gfx_fill_rect_xywh((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x142,sprite_editor.origin_y + 0xac,0x20,0x20,right_color);
  gfx_draw_frame_rect((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x142,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  if (sprite_editor_colors.left == 0) {
    left_color = (*(int *)&sprite_editor.key_color);
  }
  else {
    left_color = sprite_editor_colors.left;
  }
  gfx_fill_rect_xywh((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x14a,sprite_editor.origin_y + 0xb4,0x10,0x10,left_color);
  gfx_draw_frame_rect((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x14a,sprite_editor.origin_y + 0xb4,0x10,0x10,10);
  if (sprite_editor_colors.prev_left == 0) {
    prev_left_color = (*(int *)&sprite_editor.key_color);
  }
  else {
    prev_left_color = sprite_editor_colors.prev_left;
  }
  gfx_fill_rect_xywh((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x20,0x20,prev_left_color);
  gfx_draw_frame_rect((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  if (sprite_picker_sprite_block.sprite_sheet_mode != 0xb) {
    if (sprite_editor_colors.prev_right == 0) {
      prev_right_color = (*(int *)&sprite_editor.key_color);
    }
    else {
      prev_right_color = sprite_editor_colors.prev_right;
    }
    gfx_fill_rect_xywh((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x166,sprite_editor.origin_y + 0xac,0x20,0x20,prev_right_color);
    gfx_draw_frame_rect((iptr)&sprite_editor.window,sprite_editor.origin_x + 0x166,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  }
  rect_set_xywh((iptr)rect,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x68,0x20);
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect((iptr)rect);
  return;
}

/* 0057261a auto */
void __cdecl sprite_editor_draw_palette_cell(iptr surface,int origin_x,int origin_y,int color)

{
  int col;
  int row;
  uint sign;
  i8 rect[16];
  
  if ((9 < color) && (color < 0xca)) {
    sign = (int)(color - 10U) >> 0x1f;
    col = ((color - 10U ^ sign) - sign & 0x1f ^ sign) - sign;
    row = (int)(color - 10 + (color - 10 >> 0x1f & 0x1fU)) >> 5;
    rect_set_xywh((iptr)rect,col * 0xc + origin_x,row * 0xc + origin_y,10,10);
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)rect,color);
    gfx_draw_bevel_rect((iptr)&sprite_editor.window,(RECT *)rect,10,10);
    rect_set_xywh((iptr)rect,col * 0xc + origin_x - 1,row * 0xc + origin_y - 1,0xc,0xc)
    ;
    if ((color == sprite_editor_colors.right) || (color == sprite_editor_colors.left)) {
      gfx_draw_bevel_rect(surface,(RECT *)rect,0x29,0x29);
    }
    else {
      gfx_draw_bevel_rect(surface,(RECT *)rect,0x1d,0x1d);
    }
    ((DibSurface *)surface)->wnd_blit_surface_rect((iptr)rect);
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
    ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((iptr)unit_rect,(iptr)&sprite_editor.surface,0,0);
    break;
  case 5:
  case 7:
  case 8:
  case 10:
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((iptr)tile_rect,(iptr)&sprite_editor.surface,0,0);
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
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,sprite_editor.origin_y + py * 4 + 0x10,
                       4,4);
    if (sprite_editor.paint_color == 0) {
      screen_color = (*(int *)&sprite_editor.key_color);
    }
    else {
      screen_color = sprite_editor.paint_color;
    }
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,screen_color);
  }
  else if (sprite_editor.brush_size == 1) {
    ((DibSurface *)((i8 *)&sprite_editor.window))->get_clip((RECT *)saved_clip);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                       sprite_editor.height << 2);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.plot_rect);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px - 1,py,3,1);
    if (sprite_editor.paint_color == 0) {
      color1 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color1 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((iptr)&sprite_editor.plot_rect,color1);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,sprite_editor.origin_y + py * 4 + 0x10,
                       0xc,4);
    if (sprite_editor.paint_color == 0) {
      color2 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color2 = sprite_editor.paint_color;
    }
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color2);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px,py - 1,1,3);
    if (sprite_editor.paint_color == 0) {
      color3 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color3 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((iptr)&sprite_editor.plot_rect,color3);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,py * 4 + sprite_editor.origin_y + 0xc,4
                       ,0xc);
    if (sprite_editor.paint_color == 0) {
      color4 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color4 = sprite_editor.paint_color;
    }
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color4);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)saved_clip);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,py * 4 + sprite_editor.origin_y + 0xc,
                       0xc,0xc);
  }
  else if (sprite_editor.brush_size == 2) {
    ((DibSurface *)((i8 *)&sprite_editor.window))->get_clip((RECT *)saved_clip);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                       sprite_editor.height << 2);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.plot_rect);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px - 1,py,4,2);
    if (sprite_editor.paint_color == 0) {
      color5 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color5 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((iptr)&sprite_editor.plot_rect,color5);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,sprite_editor.origin_y + py * 4 + 0x10,
                       0x10,8);
    if (sprite_editor.paint_color == 0) {
      color6 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color6 = sprite_editor.paint_color;
    }
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color6);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px,py - 1,2,4);
    if (sprite_editor.paint_color == 0) {
      color7 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color7 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((iptr)&sprite_editor.plot_rect,color7);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,py * 4 + sprite_editor.origin_y + 0xc,8
                       ,0x10);
    if (sprite_editor.paint_color == 0) {
      color8 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color8 = sprite_editor.paint_color;
    }
    gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color8);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)saved_clip);
    rect_set_xywh((iptr)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,py * 4 + sprite_editor.origin_y + 0xc,
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
  rect_set_xywh((iptr)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                     sprite_editor.height << 2);
  ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.marker_rect);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x20,sprite_editor.origin_y + 0x10,4,0x3c);
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x20,sprite_editor.origin_y + 0x4c,0xb4,4);
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,sprite_editor.origin_x + 0xd0,sprite_editor.origin_y + 0x10,4,0x3c);
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 8,sprite_editor.origin_y + flag_y * 4 + 0x10,0x14,4)
  ;
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,10);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 0x10,flag_y * 4 + sprite_editor.origin_y + 8,4,0x14)
  ;
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,10);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 9,sprite_editor.origin_y + flag_y * 4 + 0x11,0x12,2)
  ;
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x29);
  rect_set_xywh((iptr)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 0x11,flag_y * 4 + sprite_editor.origin_y + 9,2,0x12)
  ;
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x29);
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
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,sprite_editor.origin_x + 10,sprite_editor.origin_y + 0xe0,old_color);
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
    rect_set_xywh((iptr)grab_rect,0,0,sprite_editor.width,sprite_editor.height);
    temp_sprite.grab_rect((iptr)&sprite_editor.surface,0xfe,(iptr)grab_rect);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->draw_keyed((iptr)draw_rect,(iptr)&sprite_editor.surface,0xfe,0,0);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->free();
    temp_sprite.merge_onto((iptr)&sprite_editor.result_sprite);
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
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,old_color);
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,new_color);
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
      sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,old_color);
      sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,new_color);
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
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,old_color);
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,new_color);
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
    ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect(dirty_rect);
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
  sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,old_color);
  sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,temp_x,temp_y,new_color);
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
          ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect(dirty_rect);
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

void __cdecl sprite_editor_run(iptr sprite,const char *title,i32 sheet_mode,SpriteSheetSaveFn ok_callback)

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
  sprite_editor.target_sprite = (Sprite *)sprite;
  sprite_editor.width = ((Sprite *)sprite)->width();
  sprite_editor.height = sprite_editor.target_sprite->height();
  key_color = sprite_editor.target_sprite->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  sprite_editor.ok_callback = ok_callback;
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
  sprite_editor.target_sprite->merge_onto((iptr)&sprite_editor.orig_sprite);
  ((DibSurface *)((i8 *)&sprite_editor.surface))->DibSurface::create(sprite_editor.width,sprite_editor.height);
  sprite_editor_draw_backdrop();
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw((iptr)draw_rect,(iptr)&sprite_editor.surface,0,0);
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
  rect_set_xywh((iptr)button_rect,button_x,button_y2,button_width2,button_height);
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)&sprite_editor.wnd,0x65,(iptr)button_rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)sprite_editor_ok_clicked);
  button_x = button_x + button_width2 + 2;
  rect_set_xywh((iptr)button_rect,button_x,button_y2,button_width2,button_height);
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)&sprite_editor.wnd,0x66,(iptr)button_rect,(const char *)label);
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
  gfx_fill_rect((iptr)&sprite_editor.window,(RECT *)&sprite_editor.fill_rect,0x1d);
  x2 = sprite_editor.origin_x + 0x10;
  y2 = sprite_editor.origin_y + 0x10;
  width2 = sprite_editor.width << 2;
  height2 = sprite_editor.height << 2;
  rect_set_xywh((iptr)grab_rect,0,0,sprite_editor.width,sprite_editor.height);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->grab_surface_rect((iptr)&sprite_editor.surface,(iptr)grab_rect);
  key_color = ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  gfx_fill_rect_xywh((iptr)&sprite_editor.window,x2,y2,width2,height2,sprite_editor.key_color);
  gfx_set_scale_ratio(4,1);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw_keyed((iptr)sprite_rect,(iptr)&sprite_editor.window,0,x2,y2);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame((iptr)&sprite_editor.window,x2,y2,width2,height2,6);
  x2 = sprite_editor.origin_x + 10;
  y2 = sprite_editor.origin_y + 0xe0;
  for (i = 0; i < 0xc0; i++) {
    sprite_editor_draw_palette_cell((iptr)&sprite_editor.window,x2,y2,i + 10);
  }
  x2 = sprite_editor.origin_x + 0x11e;
  y2 = sprite_editor.origin_y + 0x14;
  for (col = 0; col < 3; col++) {
    for (row = 0; row < 5; row++) {
      if (row == 0 && col == 0) {
        gfx_fill_rect_xywh((iptr)&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,
                           sprite_editor.key_color);
      }
      else {
        gfx_fill_rect_xywh((iptr)&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,
                           0x29);
        ((Sprite *)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + (row * 3 + col) * 0x3c))->draw(
                   (iptr)tool_rect,(iptr)&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2);
      }
      gfx_draw_frame_rect((iptr)&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,10
                        );
    }
  }
  sprite_editor_draw_swatches();
  text_set_surface((iptr)&sprite_editor.window);
  text_set_font((iptr)&editor_font_block.editor_font);
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
  gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  text_set_surface((iptr)this);
  text_set_font((iptr)&ui_text_font);
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
  gfx_fill_rect_xywh((iptr)this,x2,y2,width2,height2,sprite_editor.key_color);
  gfx_set_scale_ratio(2,1);
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->draw_keyed((iptr)draw_rect,(iptr)this,0,x2,y2);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame((iptr)this,x2,y2,width2,height2,6);
  wnd_blit_surface();
  return;
}

/* 00574686 auto */
void __cdecl sprite_picker_redraw_cb(void)

{
  (*(SpritePicker * *)&sprite_editor.sprite_picker_dialog)->draw();
  return;
}

/* 005746a1 auto */
i32 __cdecl sprite_editor_cell_rect(uint cell,iptr out_x,iptr out_y,iptr out_width,iptr out_height)

{
  uint sign;
  i32 cell_count;
  
  *(i32 *)out_height = 0;
  *(i32 *)out_width = *(i32 *)out_height;
  *(i32 *)out_y = *(i32 *)out_width;
  *(i32 *)out_x = *(i32 *)out_y;
  cell_count = *(i32 *)out_x;
  if ((*(uint *)&sprite_picker_sprite_block.sprite_sheet_mode) < 0xc) {
    sign = (int)cell >> 0x1f;
    switch((*(uint *)&sprite_picker_sprite_block.sprite_sheet_mode)) {
    case 0:
      cell_count = 0x14;
      *(int *)out_x = ((int)(cell + (sign & 3)) >> 2) * 0x25 + 0x157;
      *(uint *)out_y = (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x15 + 0xd3;
      *(i32 *)out_width = 0x24;
      *(i32 *)out_height = 0x14;
      break;
    case 1:
      if ((int)cell < 0x18) {
        *(i32 *)out_x = 1;
      }
      else {
        *(i32 *)out_x = 0x14e;
        cell -= 0x18;
      }
      sign = (int)cell >> 0x1f;
      *(uint *)out_x = *(int *)out_x + (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x41;
      *(int *)out_y = ((int)(cell + (sign & 3)) >> 2) * 0x31 + 0x27;
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x30;
      cell_count = 0x30;
      break;
    case 2:
      cell_count = 4;
      *(uint *)out_x = cell * 0x41 + 0x8f;
      *(i32 *)out_y = 0x1a7;
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x30;
      break;
    case 3:
      cell_count = 0x12;
      *(i32 *)out_width = 0xe;
      *(i32 *)out_height = 0x16;
      *(int *)out_x = (((int)cell >> 1) % 9) * (*(int *)out_width + 1) + 1;
      *(uint *)out_y = (*(int *)out_height + 1) * (cell & 1) + 0x1a9;
      break;
    case 4:
      cell_count = 0x42;
      *(i32 *)out_width = 0x24;
      *(i32 *)out_height = 0x14;
      if ((int)cell < 0x26) {
        *(uint *)out_x = (((cell ^ sign) - sign & 7 ^ sign) - sign) * 0x25 + 0x157;
        *(int *)out_y = ((int)(cell + (sign & 7)) >> 3) * 0x15 + 1;
      }
      else {
        *(int *)out_x = ((int)(cell - 0x26) % 7) * 0x25 + 0x157;
        *(int *)out_y = ((int)(cell - 0x26) / 7) * 0x15 + 0x6a;
      }
      break;
    case 5:
      cell_count = 1;
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x20;
      *(i32 *)out_x = 199;
      *(i32 *)out_y = 0x100;
      break;
    case 6:
      cell_count = 8;
      *(i32 *)out_width = 0x20;
      *(i32 *)out_height = 0x20;
      *(uint *)out_x = (*(int *)out_width + 1) * cell + 1;
      *(i32 *)out_y = 0x164;
      break;
    case 7:
      cell_count = 0x2c;
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x20;
      *(int *)out_x = ((int)cell / 0xb) * 0x41 + 1;
      *(int *)out_y = ((int)cell % 0xb) * 0x21 + 1;
      break;
    case 8:
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x20;
      if ((int)cell < 0x40) {
        *(uint *)out_x = (((cell ^ sign) - sign & 7 ^ sign) - sign) * 0x41 + 1;
        *(uint *)out_y =
             (((int)(((cell ^ sign) - sign & 0xf ^ sign) - sign) < 8) - 1 & 0x21) +
             ((int)(cell + (sign & 0xf)) >> 4) * 0x42 + 0x43;
      }
      else {
        *(uint *)out_x = (cell - 0x40) * 0x41 + 1;
        *(i32 *)out_y = 0x14b;
      }
      cell_count = 0x44;
      break;
    case 9:
      *(i32 *)out_width = 0x20;
      *(i32 *)out_height = 0x10;
      *(i32 *)out_x = 1;
      *(i32 *)out_y = 0x1ad;
      switch(((cell ^ sign) - sign & 3 ^ sign) - sign) {
      case 0:
        break;
      case 1:
        *(int *)out_y = *(int *)out_y + 0x22;
        *(int *)out_x = *(int *)out_x + 0x21;
        break;
      case 2:
        *(int *)out_y = *(int *)out_y + 0x11;
        break;
      case 3:
        *(int *)out_y = *(int *)out_y + 0x22;
      }
      *(int *)out_x = *(int *)out_x + ((int)(cell + (sign & 3)) >> 2) * 0x42;
      cell_count = 0x20;
      break;
    case 10:
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x20;
      *(i32 *)out_x = 0x1c8;
      if ((int)cell < 6) {
        *(uint *)out_y = cell * 0x21 + 100;
      }
      else {
        *(int *)out_x = ((int)(cell - 6) % 9) * 0x41 + 1;
        *(int *)out_y = ((int)(cell - 6) / 9) * 0x21 + 0x16c;
      }
      cell_count = 0x18;
      break;
    case 0xb:
      cell_count = 0x3f;
      *(int *)out_y = ((int)cell / 9) * 0x31 + 1;
      *(int *)out_x = ((int)cell % 9) * 0x41 + 1;
      *(i32 *)out_width = 0x40;
      *(i32 *)out_height = 0x30;
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
  
  sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2);
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->free();
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->grab_from_surface((iptr)&sprite_editor.sprite_sheet_surface,7,x2,y2,width2,height2);
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
  ((DibSurface *)((i8 *)&sprite_editor.sprite_picker_window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(iptr)&game_palette_block.game_palette,
             (iptr)&sprite_editor.wnd);
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
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->draw((iptr)draw_rect,(iptr)&sprite_editor.surface,0,0);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
  rect_set_xywh((iptr)grab_rect,0,0,width2,height2);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->grab_surface_rect((iptr)&sprite_editor.surface,(iptr)grab_rect);
  sprite_editor.sprite_picker_running = 0;
  ((ModalMenuWindow *)(sprite_editor.sprite_picker_dialog + 0x48))->end_modal_loop();
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
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        sign = (int)cell >> 0x1f;
        ((Sprite *)
                   (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                   (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x3c +
                   ((int)(cell + (sign & 3)) >> 2) * 0xf0))->free();
        sign = (int)cell >> 0x1f;
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           ((int)(cell + (sign & 3)) >> 2) * 0xf0 +
                           (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x3c),x2,
                     y2,width2,height2);
      }
      break;
    case 1:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        half_cell = (int)cell % 0x18;
        sign = (int)half_cell >> 0x1f;
        era_row = (int)(half_cell + (sign & 3)) >> 2;
        size_col = ((half_cell ^ sign) - sign & 3 ^ sign) - sign;
        sheet_half = (int)cell / 0x18;
        ((Sprite *)
                   (((i8 *)&gfx_sheet_surface_block.city_sprites) + size_col * 0x78 + era_row * 0x1e0 + sheet_half * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&gfx_sheet_surface_block.city_sprites) + era_row * 0x1e0 + sheet_half * 0x3c + size_col * 0x78),
                     x2,y2,width2,height2);
      }
      break;
    case 2:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        switch(cell) {
        case 0:
          ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->free();
          sprite_editor_grab_from_sheet((iptr)&fortify_overlay_sprites_block.fortify_overlay_sprites,x2,y2,width2,height2);
          break;
        case 1:
          ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->free();
          sprite_editor_grab_from_sheet((iptr)&fortify_overlay_sprites_block.fortress_sprite,x2,y2,width2,height2);
          break;
        case 2:
          ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->free();
          sprite_editor_grab_from_sheet((iptr)&airbase_sprites_block.airbase_sprites,x2,y2,width2,height2);
          break;
        case 3:
          ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->free();
          sprite_editor_grab_from_sheet((iptr)&airbase_sprites_block.airbase_occupied_sprite,x2,y2,width2,height2);
        }
      }
      break;
    case 3:
      for (cell = 0; (int)cell < 8; cell++) {
        sprite_editor_cell_rect(cell * 2,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + cell * 0x78),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 2 + 1,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + cell * 0x78),x2,y2,width2,height2);
      }
      break;
    case 4:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        if ((int)cell < 0x26) {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + (cell * 4 + 4) * 0xf))->free();
          sprite_editor_grab_from_sheet((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + (cell * 4 + 4) * 0xf),x2,y2,width2,
                       height2);
        }
        else {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (cell * 4 - 0x98) * 0xf))->free();
          sprite_editor_grab_from_sheet((iptr)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (cell * 4 - 0x98) * 0xf),x2,y2,
                       width2,height2);
        }
      }
      break;
    case 5:
      break;
    case 6:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&government_sprites_block.explosion_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
      break;
    case 7:
      for (cell = 0; (int)cell < 0xb; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + cell * 0x3c),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x16,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + cell * 0x78),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x21,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + cell * 0x78),x2,y2,width2,height2);
      }
      break;
    case 8:
      for (cell = 0; (int)cell < 0x10; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&icon_misc_sprites_block.river_sprites) + cell * 0x3c),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x10,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&forest_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&forest_sprites) + cell * 0x3c),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x20,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&mountain_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&mountain_sprites) + cell * 0x3c),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x30,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&hill_sprites_block.hill_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
      for (cell = 0; (int)cell < 4; cell++) {
        sprite_editor_cell_rect(cell + 0x40,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
      break;
    case 9:
      for (cell = 0; (int)cell < 8; cell++) {
        sprite_editor_cell_rect(cell << 2,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + cell * 0xf0),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 1,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + cell * 0xf0),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 2,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + cell * 0xf0),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 3,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + cell * 0xf0),x2,y2,width2,height2);
      }
      break;
    case 10:
      for (cell = 0; (int)cell < 3; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
      sprite_editor_cell_rect(3,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2);
      ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->free();
      sprite_editor_grab_from_sheet((iptr)&pollution_sprite_block.pollution_sprite,x2,y2,width2,height2);
      sprite_editor_cell_rect(4,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2);
      ((Sprite *)((i8 *)&grassland_shield_sprite))->free();
      sprite_editor_grab_from_sheet((iptr)&grassland_shield_sprite,x2,y2,width2,height2);
      sprite_editor_cell_rect(5,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2);
      ((Sprite *)((i8 *)&hut_sprite))->free();
      sprite_editor_grab_from_sheet((iptr)&hut_sprite,x2,y2,width2,height2);
      for (cell = 0; (int)cell < 9; cell++) {
        sprite_editor_cell_rect(cell + 6,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&road_sprites_block.road_sprites) + cell * 0x3c),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0xf,(iptr)&x2,(iptr)&y2,(iptr)&width2,
                           (iptr)&height2);
        ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&road_sprites_block.railroad_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
      break;
    case 0xb:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,(iptr)&x2,(iptr)&y2,(iptr)&width2,(iptr)&height2)
        ;
        ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + cell * 0x3c),x2,y2,width2,height2);
      }
    }
    ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
    sprite_editor.target_sprite->merge_onto((iptr)&sprite_editor.orig_sprite);
    ((DibSurface *)((i8 *)&sprite_editor.surface))->DibSurface::create(sprite_editor.width,sprite_editor.height);
    sprite_editor_draw_backdrop();
    ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw((iptr)draw_rect,(iptr)&sprite_editor.surface,0,0);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->free();
    sprite_editor.target_sprite->merge_onto((iptr)&sprite_editor.result_sprite);
    sprite_editor.sprite_picker_running = 0;
    ((ModalMenuWindow *)(sprite_editor.sprite_picker_dialog + 0x48))->end_modal_loop();
    ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  }
  return;
}

/* 00575d89 auto */
void __cdecl
sprite_editor_grab_from_sheet(iptr sprite,i32 x,i32 y,i32 width,
            i32 height)

{
  ((Sprite *)sprite)->grab_from_surface((iptr)&sprite_editor.sprite_sheet_surface,7,x,y,width,height);
  ((Sprite *)sprite)->replace_color(9,7);
  return;
}

/* 00575dc4 auto */
void __cdecl sprite_picker_cancel_clicked(void)

{
  sprite_editor.sprite_picker_running = 0;
  ((ModalMenuWindow *)(sprite_editor.sprite_picker_dialog + 0x48))->end_modal_loop();
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
  
  (*(SpritePicker * *)&sprite_editor.sprite_picker_dialog) = this;
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
  sprite_editor.sprite_sheet_cell_count = sprite_editor_cell_rect(0,(iptr)dummy_out,(iptr)dummy_out,(iptr)dummy_out,(iptr)dummy_out);
  rect_set_xywh((iptr)rect,x2,y2,sci_rate_sel,scroll_height);
  if (this == (SpritePicker *)0x0) {
    scroll_parent = (u8 *)0x0;
  }
  else {
    scroll_parent = (u8 *)&window;
  }
  scrollbar.create((iptr)scroll_parent,0x65,(iptr)rect,0);
  scrollbar.set_range(0,sprite_editor.sprite_sheet_cell_count - 1);
  scrollbar.set_pos(0);
  scrollbar.set_callback(sprite_picker_select_cell);
  sprite_picker_select_cell(0);
  ((Control *)&scrollbar_head)->show_and_redraw();
  init_lux_rate = (int)(client_width - 8) / 3;
  y2 = (client_y + client_height) - (init_sci_rate + 2);
  x2 = client_x + 2;
  rect_set_xywh((iptr)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  button_1.create((iptr)ok_parent,0xc9,(iptr)rect,(const char *)label);
  button_1.set_callback((CtrlNotifyFn)sprite_picker_ok_clicked);
  button_1.set_default_button();
  x2 = x2 + init_lux_rate + 2;
  rect_set_xywh((iptr)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    import_parent = (u8 *)0x0;
  }
  else {
    import_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x255]);
  button_2.create((iptr)import_parent,0xc9,(iptr)rect,(const char *)label);
  button_2.set_callback((CtrlNotifyFn)sprite_picker_import_all_clicked);
  button_2.set_default_button();
  if (sprite_picker_sprite_block.sprite_sheet_mode == 5) {
    button_2.disable();
  }
  x2 = x2 + init_lux_rate + 2;
  rect_set_xywh((iptr)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  button_3.create((iptr)cancel_parent,0xca,(iptr)rect,(const char *)label);
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
  Dialog dialog2;
  char *file_name;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
    ok = ui_file_name_dialog((iptr)&sprite_editor.wnd,(const char *)prompt,(iptr)file_path,(iptr)filter,(iptr)&str_bmp_gif,1,0
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
                               (iptr)&palette);
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
          result = gfx_load_bmp_640x480((iptr)&sprite_editor.sprite_sheet_surface,(const char *)file_path,10,0xc0,(iptr)&palette);
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

void SpritePicker::destruct()

{
  int seh_state;
  
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 3;
  /* inlined unwind funclet 00578402 */ { ctrl_dtor((Button *)(((int)this) + 0x3b0)); }
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 00578411 */ { ctrl_dtor((Button *)(((int)this) + 0x374)); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 00578420 */ { ctrl_dtor((Button *)(((int)this) + 0x338)); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 0057842f */ { ((Scrollbar *)(((int)this) + 0x2f8))->destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0057843e */ { FrameWindow::destruct(); }
  return;
}

/* 005784a0 auto */
SpritePicker *SpritePicker::construct()

{
  
  FrameWindow::construct();
  scrollbar.construct();
  button_ctor(&button_1);
  button_ctor(&button_2);
  button_ctor(&button_3);
  vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_4;
  return this;
}

/* 005a9780 auto */
void __cdecl gfx_set_draw_target(iptr surface)

{
  gfx_draw_target = surface;
  return;
}

/* 005a9798 auto */
void __cdecl
gfx_draw_line_color(iptr surface,i32 x1,i32 y1,i32 x2,
            i32 y2,i32 color)

{
  gfx_set_text_color_index(color);
  ((DibSurface *)surface)->draw_line(x1,y1,x2,y2);
  return;
}

/* 005a97cc auto */
void __cdecl
gfx_draw_hline(iptr surface,i32 x1,int x2,int y,i32 color)

{
  i8 rect[16];
  
  rect_set((iptr)rect,x1,y,x2 + 1,y + 1);
  gfx_fill_rect(surface,(RECT *)rect,color);
  return;
}

/* 005a9858 auto */
void __cdecl
gfx_draw_vline(iptr surface,int x,i32 y1,int y2,i32 color)

{
  i8 rect[16];
  
  rect_set((iptr)rect,x,y1,x + 1,y2 + 1);
  gfx_fill_rect(surface,(RECT *)rect,color);
  return;
}

/* 005a98e4 auto */
void __cdecl
gfx_draw_box_ltrb(iptr surface,i32 left,i32 top,i32 right,
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
gfx_draw_frame_rect(iptr surface,int x,int y,int width,iptr height,i32 color)

{
  gfx_draw_hline(surface,x,width + x - 1,y,color);
  gfx_draw_hline(surface,x,width + x - 1,height + y - 1,color);
  gfx_draw_vline(surface,x,y,height + y - 1,color);
  gfx_draw_vline(surface,width + x - 1,y,height + y - 1,color);
  return;
}

/* 005a99fc auto */
void __cdecl gfx_draw_bevel_rect(iptr surface,RECT *rect,i32 light_color,i32 dark_color)

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
void __cdecl gfx_fill_surface(iptr surface,i32 color)

{
  ((DibSurface *)surface)->fill(color);
  return;
}

/* 005a9abf auto */
void __cdecl
gfx_fill_rect_xywh(iptr surface,i32 x,i32 y,iptr width,iptr height,
            i32 color)

{
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,x,y,width,height);
  ((DibSurface *)surface)->fill_rect((iptr)rect,color);
  return;
}

/* 005a9afe auto */
void __cdecl
gfx_copy_rect_xywh(iptr src,iptr dest,i32 src_x,i32 src_y,i32 dest_x,
            i32 dest_y,iptr width,iptr height)

{
  i8 dest_rect[16];
  i8 src_rect[16];
  
  rect_set_xywh((iptr)src_rect,src_x,src_y,width,height);
  rect_set_xywh((iptr)dest_rect,dest_x,dest_y,width,height);
  ((DibSurface *)src)->DibSurface::blit((DibSurface *)dest,(RECT *)src_rect,(RECT *)dest_rect);
  return;
}

/* 005a9b5d auto */
int __cdecl
gfx_tile_fill(iptr dest,iptr tile,int x,int y,int width,int height,int origin_x,
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
  
  tile_width = ((DibSurface *)tile)->surface_width();
  tile_height = ((DibSurface *)tile)->surface_height();
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
void __cdecl bit_index_split(iptr bit_index,iptr out_byte,iptr out_mask)

{
  *(iptr *)out_byte = bit_index >> 3;
  *(int *)out_mask = 1 << ((byte)bit_index & 7);
  return;
}

/* 005bb3f0 auto */
void DibSurface::wnd_create(iptr parent,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette)

{
  NativeWindow *window;
  
  ((WndBase *)(this + 1))->create_framed((const char *)parent,style,x,y,width,height,palette);
  create(width,height);
  set_palette(palette);
  window = ((WndBase *)(this + 1))->get_window();
  wnd_install_paint_proc((iptr)this,window);
  return;
}

/* 005bcaa7 auto */
void __cdecl gfx_get_screen_rect(iptr rect)

{
  int yBottom;
  int xRight;
  
  yBottom = GetSystemMetrics(1);
  xRight = GetSystemMetrics(0);
  SetRect((LPRECT)rect,0,0,xRight,yBottom);
  return;
}

/* 005bcc11 auto */
void __cdecl
dib_blit_to_wnd_obj(GdiDib *src,int src_x,int src_y,int width,int height,NativeWindow *dest,int dest_x,
            int dest_y)

{
  if ((src != 0) && (dest != 0)) {
    if ((dest->palette != 0) && (display_palette.palettized == 1)) {
      RealizePalette(dest->hdc);
    }
    BitBlt(dest->hdc,dest_x,dest_y,width,height,src->hdc,src_x,
           src_y,0xcc0020);
  }
  return;
}

/* 005bd630 auto */
DibSurface * __thiscall surface_ctor(DibSurface *self)

{
  self->vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_5;
  self->set_dims(0);
  return self;
}

/* 005bd65c auto */
void DibSurface::create(int width,int height)

{
  tagRECT rect;
  
  SetRect(&rect,0,0,width,height);
  alloc_dib8((iptr)&rect);
  return;
}

/* 005bd696 auto */
i32 DibSurface::alloc_dib8(iptr rect)

{
  char locked;
  u32 new_dib;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    new_dib = dib_destroy(dib);
    dib = (GdiDib *)new_dib;
  }
  if (((rect != 0) && (dim = rect_width((RECT *)rect), dim != 0)) &&
     (dim = rect_height((RECT *)rect), dim != 0)) {
    set_dims((int *)rect);
    new_dib = (u32)dib_create((RECT *)&bounds_rect);
    dib = (GdiDib *)new_dib;
    if (dib != 0) {
      new_dib = dib_get_stride(dib);
      stride = new_dib;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(iptr)&str_err_portallocfailed,(iptr)&str_d_ss_smeds32_port_cpp,0x71);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005bd813 auto */
void DibSurface::set_dims(int *rect)

{
  bits = 0;
  dib = 0;
  palette_version = 0;
  stride = 0;
  signed_pitch = 0;
  bytes_per_pixel = 1;
  if (rect == (int *)0x0) {
    pixel_height = 0;
    pixel_width = 0;
    SetRect((LPRECT)&bounds_rect,0,0,0,0);
    SetRect((LPRECT)&clip_left,0,0,0,0);
  }
  else {
    pixel_height = rect[3] - rect[1];
    pixel_width = rect[2] - *rect;
    SetRect((LPRECT)&bounds_rect,0,0,pixel_width,pixel_height);
    SetRect((LPRECT)&clip_left,0,0,pixel_width,pixel_height);
  }
  return;
}

/* 005bd915 auto */
DibSurface * __thiscall surface_dtor(DibSurface *self)

{
  char locked;
  u32 new_dib;
  
  self->vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_5;
  locked = self->is_locked();
  if (locked != '\0') {
    self->unlock();
  }
  new_dib = dib_destroy(self->dib);
  self->dib = (GdiDib *)new_dib;
  self->set_dims(0);
  if (draw_target.surface == self) {
    draw_target.surface = (DibSurface *)0x0;
  }
  return self;
}

/* 005bf071 auto */
i32 DibSurface::load_gif_file(const char *filename,uint first_color,uint color_count,iptr palette)

{
  int result;
  i32 ret;
  uint table_size;
  tagRECT rect;
  int mem;
  iptr buffer;
  int height2;
  FileObject file;
  int width2;
  byte lzw_min_code;
  char *data;
  char *cursor;
  char *image_desc;
  char *image_data;
  
  file.init();
  ON_SCOPE_EXIT(file.close());
  result = file.open(filename);
  if (result == 0) {
    ret = 0;
  }
  else {
    ret = file.get_size();
    mem = mem_handle_alloc(ret);
    win_flush_pending_msgs();
    if (mem == 0) {
      ret = 0;
    }
    else {
      buffer = (iptr)mem_handle_lock(mem);
      ret = file.get_size();
      file.read((char *)buffer,ret);
      buffer = mem_handle_unlock(mem);
      win_flush_pending_msgs();
      if (mem == 0) {
        dbg_logf_str((const char *)&str_error_gif_resource_not_found,filename);
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(3,(iptr)&str_err_resourcenotfound,(iptr)&str_d_ss_smeds32_port_cpp_2,0x382);
        ret = 0;
      }
      else {
        data = (char *)mem_handle_lock(mem);
        result = _strncmp(data,((i8 *)&gif_sig_file),3);
        if (result == 0) {
          if ((data[10] & 0x80U) == 0) {
            dbg_logf_str((const char *)&str_error_gif_contains_no_global_color_map,filename);
            mem_handle_unlock(mem);
            mem_handle_free(mem);
            ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat_2,(iptr)&str_d_ss_smeds32_port_cpp_4,0x396);
            ret = 0;
          }
          else {
            table_size = 1 << (data[10] & 7U) + 1;
            cursor = data + 0xd;
            win_flush_pending_msgs();
            if (palette != 0) {
              win_flush_pending_msgs();
              if (0x100 < (int)(color_count + first_color)) {
                color_count = 0x100 - first_color;
              }
              if (table_size <= color_count) {
                color_count = table_size;
              }
              ((GamePalette *)palette)->set_rgb(first_color,color_count,(iptr)cursor)
              ;
              win_flush_pending_msgs();
            }
            for (cursor = data + table_size * 3 + 0xd; *cursor == '\0'; cursor++)
            {
            }
            if ((*cursor == ',') || (*cursor == '!')) {
              image_desc = cursor + 1;
              width2 = endian_from_le((uint)*(ushort *)(cursor + 5));
              height2 = endian_from_le((uint)*(ushort *)(image_desc + 6));
              if ((image_desc[8] & 0x80U) != 0) {
                dbg_log((const char *)&str_warning_skipping_local_color_table);
              }
              SetRect(&rect,0,0,width2,height2);
              result = resize((iptr)&rect);
              if (result == 0) {
                mem_handle_unlock(mem);
                mem_handle_free(mem);
                ret = 0;
              }
              else {
                lzw_min_code = image_desc[9];
                image_data = image_desc + 10;
                win_flush_pending_msgs();
                gif_lzw_decode((iptr)image_data,first_color,
                             CONCAT22((short)(stride >> 0x10),(ushort)lzw_min_code),stride
                             ,width2,height2,bits);
                win_flush_pending_msgs();
                set_palette(palette);
                result = dib_is_top_down(dib);
                if (result == 0) {
                  dib_flip_vertical(dib);
                }
                mem_handle_unlock(mem);
                mem_handle_free(mem);
                file.map_close();
                ret = 1;
              }
            }
            else {
              dbg_logf_str((const char *)&str_error_gif_image_block_not_found,filename);
              mem_handle_unlock(mem);
              mem_handle_free(mem);
              ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat_3,(iptr)&str_d_ss_smeds32_port_cpp_5,0x3b3);
              ret = 0;
            }
          }
        }
        else {
          dbg_logf_str((const char *)&str_error_resource_is_not_a_gif,filename);
          mem_handle_unlock(mem);
          mem_handle_free(mem);
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat,(iptr)&str_d_ss_smeds32_port_cpp_3,0x38d);
          ret = 0;
        }
      }
    }
  }
  return ret;
}

/* 005bf5e1 auto */
i32 DibSurface::load_gif_resource(iptr res_id,uint first_color,uint color_count,iptr palette)

{
  i32 ret;
  int result;
  uint table_size;
  tagRECT rect;
  int res;
  char res_type[4];
  int height2;
  int width2;
  byte lzw_min_code;
  char *data;
  char *cursor;
  char *image_desc;
  char *image_data;
  
  res_type[0] = 0x47;
  res_type[1] = 0x49;
  res_type[2] = 0x46;
  res_type[3] = 0x53;
  res = res_load((iptr)res_type,res_id);
  if (res == 0) {
    dbg_logf((const char *)&str_error_gif_resource_not_found_2,res_id);
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(3,(iptr)&str_err_resourcenotfound_2,(iptr)&str_d_ss_smeds32_port_cpp_6,0x3f4);
    ret = 0;
  }
  else {
    data = (char *)res_lock(res);
    result = _strcmp(data,((i8 *)&gif_sig_resource));
    if (result == 0) {
      dbg_logf((const char *)&str_error_resource_is_not_a_gif_2,res_id);
      res_unlock(res);
      res_free(res);
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat_4,(iptr)&str_d_ss_smeds32_port_cpp_7,0x3ff);
      ret = 0;
    }
    else if ((data[10] & 0x80U) == 0) {
      dbg_logf((const char *)&str_error_gif_contains_no_global_color_map_2,res_id);
      res_unlock(res);
      res_free(res);
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat_5,(iptr)&str_d_ss_smeds32_port_cpp_8,0x408);
      ret = 0;
    }
    else {
      table_size = 1 << (data[10] & 7U) + 1;
      cursor = data + 0xd;
      if (palette != 0) {
        if (0x100 < (int)(color_count + first_color)) {
          color_count = 0x100 - first_color;
        }
        if (table_size <= color_count) {
          color_count = table_size;
        }
        ((GamePalette *)palette)->set_rgb(first_color,color_count,(iptr)cursor);
      }
      win_flush_pending_msgs();
      for (cursor = data + table_size * 3 + 0xd; *cursor == '\0'; cursor++) {
      }
      if ((*cursor == ',') || (*cursor == '!')) {
        image_desc = cursor + 1;
        width2 = endian_from_le((uint)*(ushort *)(cursor + 5));
        height2 = endian_from_le((uint)*(ushort *)(image_desc + 6));
        if ((image_desc[8] & 0x80U) != 0) {
          dbg_log((const char *)&str_warning_skipping_local_color_table_2);
        }
        SetRect(&rect,0,0,width2,height2);
        resize((iptr)&rect);
        lzw_min_code = image_desc[9];
        image_data = image_desc + 10;
        win_flush_pending_msgs();
        gif_lzw_decode((iptr)image_data,first_color,CONCAT22((short)(stride >> 0x10),(ushort)lzw_min_code)
                     ,stride,width2,height2,bits);
        win_flush_pending_msgs();
        set_palette(palette);
        result = dib_is_top_down(dib);
        if (result == 0) {
          dib_flip_vertical(dib);
        }
        res_unlock(res);
        res_free(res);
        ret = 1;
      }
      else {
        dbg_logf((const char *)&str_error_gif_image_block_not_found_2,res_id);
        res_unlock(res);
        res_free(res);
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(iptr)&str_err_badpicformat_6,(iptr)&str_d_ss_smeds32_port_cpp_9,0x424);
        ret = 0;
      }
    }
  }
  return ret;
}

/* 005c0034 auto */
void DibSurface::reset_clip()

{
  SetRect((LPRECT)&clip_left,bounds_rect,bounds_top,bounds_right,bounds_bottom);
  return;
}

/* 005c0073 auto */
void DibSurface::set_clip(RECT *rect)

{
  SetRect((LPRECT)&clip_left,rect->left,rect->top,rect->right,
          rect->bottom);
  IntersectRect((LPRECT)&clip_left,(RECT *)&clip_left,(RECT *)&bounds_rect);
  return;
}

/* 005c00ce auto */
void DibSurface::get_clip(RECT *out_rect)

{
  out_rect->left = clip_left;
  out_rect->top = clip_top;
  out_rect->right = clip_right;
  out_rect->bottom = clip_bottom;
  return;
}

/* 005c0105 auto */
i32 __thiscall surface_alloc_dib_by_bpp(DibSurface *self,iptr rect)

{
  i32 result;
  
  switch(self->bytes_per_pixel) {
  case 1:
    result = self->alloc_dib8(rect);
    break;
  case 2:
    result = self->alloc_dib16(rect);
    break;
  case 3:
    result = self->alloc_dib24(rect);
    break;
  case 4:
    result = self->alloc_dib32(rect);
    break;
  default:
    result = 0;
  }
  return result;
}

/* 005c019d auto */
i32 DibSurface::resize(iptr rect)

{
  i32 result;
  
  result = alloc_dib8(rect);
  return result;
}

/* 005c01c1 auto */
void DibSurface::lock()

{
  uptr new_bits;
  void *offsets;
  int top_down;
  int row;
  
  if (bits == 0) {
    new_bits = dib_get_bits(dib);
    bits = new_bits;
    if (bits != 0) {
      offsets = operator_new(pixel_height << 2);
      row_offsets = (uptr)offsets;
      top_down = dib_is_top_down(dib);
      if (top_down == 0) {
        *(u32 *)row_offsets = (pixel_height - 1) * stride;
        for (row = 1; row < (int)pixel_height; row++) {
          *(u32 *)(row_offsets + row * 4) =
               *(int *)((row_offsets - 4) + row * 4) - stride;
        }
      }
      else {
        *(i32 *)row_offsets = 0;
        for (row = 1; row < (int)pixel_height; row++) {
          *(u32 *)(row_offsets + row * 4) =
               *(int *)((row_offsets - 4) + row * 4) + stride;
        }
      }
    }
  }
  return;
}

/* 005c02e0 auto */
void DibSurface::unlock()

{
  uptr new_bits;
  
  if (bits != 0) {
    new_bits = dib_unlock_bits(dib);
    bits = new_bits;
    operator_delete((void *)row_offsets);
  }
  return;
}

/* 005c0333 auto */
void DibSurface::fill_rect(iptr rect,i32 color)

{
  char locked;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  tagRECT clipped;
  int height2;
  int width2;
  i32 was_locked;
  
  was_locked = 1;
  locked = is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_in_fillcolor);
    was_locked = 0;
    lock();
  }
  IntersectRect(&clipped,(RECT *)rect,(RECT *)&clip_left);
  width2 = rect_width(&clipped);
  if ((width2 != 0) && (height2 = rect_height(&clipped), height2 != 0)) {
    surf_height = surface_height();
    dib = get_dib();
    top_down = dib_is_top_down(dib);
    pitch = get_pitch();
    dib_fill_rect8(bits,color,clipped.left,clipped.top,width2,height2,
                 ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch,surf_height);
  }
  return;
}

/* 005c041f auto */
void DibSurface::fill(i32 color)

{
  fill_rect((iptr)&clip_left,color);
  return;
}

/* 005c044a auto */
void DibSurface::replace_color_clip(i32 from_color,i32 to_color)

{
  replace_color((iptr)&clip_left,from_color,to_color);
  return;
}

/* 005c0479 auto */
void DibSurface::replace_color(iptr rect,i32 from_color,i32 to_color)

{
  char locked;
  iptr height;
  iptr row_width;
  int next_row;
  char *pixel;
  tagRECT clipped;
  int row;
  int col;
  iptr width2;
  int was_locked;
  
  was_locked = 1;
  locked = is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_in_changecolor);
    was_locked = 0;
    lock();
  }
  IntersectRect(&clipped,(RECT *)rect,(RECT *)&clip_left);
  width2 = rect_width(&clipped);
  pixel = (char *)pixel_ptr(clipped.left,clipped.top);
  row = 0;
  while( true ) {
    height = rect_height(&clipped);
    if (height <= row) break;
    col = 0;
    while( true ) {
      row_width = rect_width(&clipped);
      if (row_width <= col) break;
      if (*pixel == (char)from_color) {
        *pixel = (char)to_color;
      }
      pixel++;
      col++;
    }
    next_row = this->next_row((iptr)pixel);
    pixel = (char *)(next_row - width2);
    row++;
  }
  if (was_locked == 0) {
    unlock();
  }
  return;
}

/* 005c0593 auto */
void DibSurface::blit(DibSurface *src,RECT *dest_rect,RECT *src_rect)

{
  GdiDib *dib;
  int top_down;
  int src_pitch;
  int dest_pitch;
  i32 src_height;
  i32 dest_height;
  int row_bytes;
  int src_x_bytes;
  int dest_x_bytes;
  i32 src_bits;
  i32 dest_bits;
  bool was_unlocked;
  tagRECT src_clip;
  tagRECT dest_clip;
  int src_was_locked;
  int height2;
  int width2;
  
  src_was_locked = 1;
  was_unlocked = bits == 0;
  if (was_unlocked) {
    dbg_log((const char *)&str_warning_source_port_not_locked_in_copyto);
    lock();
  }
  if (src->bits == 0) {
    dbg_log((const char *)&str_warning_destination_port_not_locked_in_c);
    src->lock();
    src_was_locked = 0;
  }
  IntersectRect(&dest_clip,dest_rect,(RECT *)&clip_left);
  IntersectRect(&src_clip,src_rect,(RECT *)((iptr)src + 0x14));
  width2 = src_clip.right - src_clip.left;
  if (dest_clip.right - dest_clip.left <= src_clip.right - src_clip.left) {
    width2 = dest_clip.right - dest_clip.left;
  }
  height2 = src_clip.bottom - src_clip.top;
  if (dest_clip.bottom - dest_clip.top <= src_clip.bottom - src_clip.top) {
    height2 = dest_clip.bottom - dest_clip.top;
  }
  if ((width2 != 0) && (height2 != 0)) {
    dib = src->get_dib();
    top_down = dib_is_top_down(dib);
    src_pitch = src->get_pitch();
    src_pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * src_pitch;
    dib = get_dib();
    top_down = dib_is_top_down(dib);
    dest_pitch = get_pitch();
    dest_pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * dest_pitch;
    src_height = src->surface_height();
    dest_height = surface_height();
    row_bytes = bytes_per_pixel * width2;
    src_x_bytes = bytes_per_pixel * src_clip.left;
    dest_x_bytes = bytes_per_pixel * dest_clip.left;
    top_down = height2;
    src_bits = src->get_bits();
    dest_bits = get_bits();
    gfx_copy_pixels(dest_bits,src_bits,dest_x_bytes,dest_clip.top,src_x_bytes,src_clip.top,row_bytes,top_down,dest_height,src_height,dest_pitch,
                 src_pitch);
  }
  if (was_unlocked) {
    unlock();
  }
  if (src_was_locked == 0) {
    src->unlock();
  }
  return;
}

/* 005c0979 auto */
void DibSurface::blit_to_wnd(iptr wnd,RECT *src_rect,iptr dest_pt)

{
  char locked;
  i32 dest_y;
  NativeWindow *window;
  i32 height;
  i32 width;
  i32 dest_x;
  
  if (dib == 0) {
    dbg_log((const char *)&str_error_null_port_in_copytoscreen);
  }
  else {
    locked = is_locked();
    if (locked == '\0') {
      dbg_log((const char *)&str_warning_port_not_locked_in_copytoscreen);
      lock();
    }
    dest_y = ((WndBase *)wnd)->get_palette_obj();
    apply_palette((GamePalette *)dest_y);
    dest_y = *(i32 *)(dest_pt + 4);
    dest_x = *(i32 *)dest_pt;
    window = ((WndBase *)wnd)->get_window();
    height = rect_height(src_rect);
    width = rect_width(src_rect);
    dib_blit_to_wnd_obj(dib,src_rect->left,src_rect->top,width,height,
                 window,dest_x,dest_y);
    if (locked == '\0') {
      unlock();
    }
  }
  return;
}

/* 005c0bf2 auto */
uint DibSurface::get_pixel(i32 x,i32 y)

{
  char in_clip;
  uint color;
  byte *pixel;
  
  in_clip = this->in_clip(x,y);
  if (in_clip == '\0') {
    dbg_logf_int2((const char *)&str_warning_getpixel_out_of_clip_region,x,y);
    color = 0;
  }
  else {
    pixel = (byte *)pixel_ptr(x,y);
    color = (uint)*pixel;
  }
  return color;
}

/* 005c0c5d auto */
void DibSurface::set_pixel(i32 x,i32 y,i32 color)

{
  char in_clip;
  i8 *pixel;
  
  in_clip = this->in_clip(x,y);
  if (in_clip == '\0') {
    dbg_logf_int2((const char *)&str_warning_setpixel_out_of_clip_region,x,y);
  }
  else {
    pixel = (i8 *)pixel_ptr(x,y);
    *pixel = (i8)color;
  }
  return;
}

/* 005c0cc5 auto */
void DibSurface::set_palette(iptr palette)

{
  u32 version;
  
  version = ((GamePalette *)palette)->get_version();
  if (version != palette_version) {
    dib_set_palette(dib,palette);
    version = ((GamePalette *)palette)->get_version();
    palette_version = version;
  }
  return;
}

/* 005c0d12 auto */
void DibSurface::apply_palette(GamePalette *palette)

{
  u32 version;
  
  if (palette != (GamePalette *)0x0) {
    version = palette->get_version();
    if (version != palette_version) {
      dib_set_palette(dib,(iptr)palette);
      version = palette->get_version();
      palette_version = version;
    }
  }
  return;
}

/* 005c0d69 auto */
void DibSurface::draw_text_cur_font(const char *text,int x,int y,uint flags)

{
  i8 old_color;
  int font_height;
  int shadow_offset;
  i32 saved_color;
  
  if (draw_target.font == (i8 *)0x0) {
    dbg_log((const char *)&str_error_no_current_font_selected);
  }
  else if (dib != 0) {
    if ((flags & 0x10) != 0) {
      font_height = font_measure_height(*(i32 *)draw_target.font);
      font_height += (font_height >> 0x1f & 7U);
      shadow_offset = font_height >> 3;
      old_color = gfx_set_text_color_index(CONCAT31((font_height >> 0xb),display_palette.black_index));
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_at(dib,*(i32 *)draw_target.font,(char *)text,shadow_offset + x,
                   shadow_offset + y,(iptr)&clip_left,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_at(dib,*(i32 *)draw_target.font,(char *)text,x,y,
                 (iptr)&clip_left,flags);
  }
  return;
}

/* 005c0f57 auto */
void DibSurface::draw_text(iptr font,const char *text,iptr x,int y,uint flags)

{
  i8 old_color;
  int font_height;
  int shadow_offset;
  i32 saved_color;
  
  if (dib != 0) {
    if ((flags & 0x10) != 0) {
      font_height = font_measure_height(*(i32 *)font);
      font_height += (font_height >> 0x1f & 7U);
      shadow_offset = font_height >> 3;
      old_color = gfx_set_text_color_index(CONCAT31((font_height >> 0xb),display_palette.black_index));
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_at(dib,*(i32 *)font,(char *)text,x + shadow_offset,y + shadow_offset,
                   (iptr)&clip_left,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_at(dib,*(i32 *)font,(char *)text,x,y,(iptr)&clip_left
                 ,flags);
  }
  return;
}

/* 005c1020 auto */
void DibSurface::draw_text_in_rect(iptr font,const char *text,RECT *rect,uint flags)

{
  i8 old_color;
  int shadow_offset;
  BOOL offset_ok;
  i32 saved_color;
  tagRECT shadow_rect;
  
  if (dib != 0) {
    if ((flags & 0x10) != 0) {
      shadow_offset = font_measure_height(*(i32 *)font);
      shadow_offset = (int)(shadow_offset + (shadow_offset >> 0x1f & 7U)) >> 3;
      shadow_rect.left = rect->left;
      shadow_rect.top = rect->top;
      shadow_rect.right = rect->right;
      shadow_rect.bottom = rect->bottom;
      offset_ok = OffsetRect(&shadow_rect,shadow_offset,shadow_offset);
      old_color = gfx_set_text_color_index(CONCAT31((int3)((uint)offset_ok >> 8),display_palette.black_index));
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_rect(dib,*(i32 *)font,(char *)text,(iptr)&shadow_rect,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_rect(dib,*(i32 *)font,(char *)text,(iptr)rect,flags);
  }
  return;
}

/* 005c1167 auto */
void DibSurface::draw_text_rect(iptr font,const char *text,iptr rect,i32 flags)

{
  if (dib != 0) {
    dib_draw_text_wrap(dib,*(i32 *)font,(char *)text,(LPRECT)rect,flags);
  }
  return;
}

/* 005c11b2 auto */
void DibSurface::draw_line(u32 x1,u32 y1,u32 x2,u32 y2)

{
  iptr handle;
  int top_down;
  int pitch;
  i32 height;
  i32 color;
  u32 start;
  
  if (((((((int)clip_left <= (int)x1) || ((int)clip_left <= (int)x2)) &&
        (((int)x2 < (int)clip_right || ((int)x1 < (int)clip_right)))) &&
       (((int)clip_top <= (int)y1 || ((int)clip_top <= (int)y2)))) &&
      (((int)y1 < (int)clip_bottom || ((int)y2 < (int)clip_bottom)))) &&
     (dib != 0)) {
    if (y2 == y1) {
      if ((int)x1 < (int)clip_left) {
        x1 = clip_left;
      }
      if ((int)clip_right <= (int)x1) {
        x1 = clip_right - 1;
      }
      if ((int)x2 < (int)clip_left) {
        x2 = clip_left;
      }
      if ((int)clip_right <= (int)x2) {
        x2 = clip_right - 1;
      }
      if (x2 != x1) {
        handle = (iptr)get_dib();
        top_down = dib_is_top_down((GdiDib *)handle);
        pitch = get_pitch();
        pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
        height = surface_height();
        start = x2;
        if ((int)x1 <= (int)x2) {
          start = x1;
        }
        top_down = crt_abs(x2 - x1);
        top_down++;
        color = CONCAT31((int3)((uint)top_down >> 8),(*(i8 *)&ctrl_colors.gfx_text));
        handle = get_bits();
        dib_hline8(handle,color,top_down,start,y1,height,pitch);
      }
    }
    else if (x2 == x1) {
      if ((int)y1 < (int)clip_top) {
        y1 = clip_top;
      }
      if ((int)clip_bottom <= (int)y1) {
        y1 = clip_bottom - 1;
      }
      if ((int)y2 < (int)clip_top) {
        y2 = clip_top;
      }
      if ((int)clip_bottom <= (int)y2) {
        y2 = clip_bottom - 1;
      }
      if (y2 != y1) {
        handle = (iptr)get_dib();
        top_down = dib_is_top_down((GdiDib *)handle);
        pitch = get_pitch();
        pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
        height = surface_height();
        start = y2;
        if ((int)y1 <= (int)y2) {
          start = y1;
        }
        top_down = crt_abs(y2 - y1);
        top_down++;
        color = CONCAT31((int3)((uint)top_down >> 8),(*(i8 *)&ctrl_colors.gfx_text));
        handle = get_bits();
        dib_vline8(handle,color,top_down,x1,start,height,pitch);
      }
    }
    else {
      dib_draw_line(dib,x1,y1,x2,y2);
    }
  }
  return;
}

/* 005c19ad auto */
i8 __cdecl gfx_set_text_color_index(i32 color_index)

{
  i8 prev_index;
  
  prev_index = (*(i8 *)&ctrl_colors.gfx_text);
  (*(i8 *)&ctrl_colors.gfx_text) = (i8)color_index;
  return prev_index;
}

/* 005c19d3 auto */
int DibSurface::pixel_ptr(int x,int y)

{
  return *(int *)(row_offsets + y * 4) + bits + x;
}

/* 005c1a00 auto */
i8 DibSurface::in_clip(int x,int y)

{
  i8 inside;
  
  if ((((x < (int)clip_left) || ((int)clip_right <= x)) ||
      (y < (int)clip_top)) || ((int)clip_bottom <= y)) {
    inside = 0;
  }
  else {
    inside = 1;
  }
  return inside;
}

/* 005c1a62 auto */
void DibSurface::remap_colors(iptr remap_table,iptr base_index,i32 unused)

{
  byte *pixel;
  int col;
  int row;
  
  if (bits != 0) {
    pixel = (byte *)bits;
    for (row = 0; row < (int)pixel_height; row++) {
      for (col = 0; col < (int)pixel_width; col++) {
        if (-1 < (int)((uint)*pixel - base_index)) {
          *pixel = *(byte *)(((uint)*pixel - base_index) + remap_table);
        }
        pixel++;
      }
      pixel += (stride - pixel_width);
    }
  }
  return;
}

/* 005c1b47 auto */
i32 DibSurface::alloc_dib16(iptr rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width((RECT *)rect), dim != 0)) &&
     (dim = rect_height((RECT *)rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create16((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 2;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(iptr)&str_err_portallocfailed_2,(iptr)&str_d_ss_smeds32_port_cpp_10,0x86c);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c1cf7 auto */
i32 DibSurface::alloc_dib24(iptr rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width((RECT *)rect), dim != 0)) &&
     (dim = rect_height((RECT *)rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create24((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 3;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(iptr)&str_err_portallocfailed_3,(iptr)&str_d_ss_smeds32_port_cpp_11,0x8a1);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c1ea7 auto */
i32 DibSurface::alloc_dib32(iptr rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width((RECT *)rect), dim != 0)) &&
     (dim = rect_height((RECT *)rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create32((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 4;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(iptr)&str_err_portallocfailed_4,(iptr)&str_d_ss_smeds32_port_cpp_12,0x8d6);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c54f0 auto */
bool DibSurface::is_locked()

{
  return bits != 0;
}

/* 005c55a0 auto */
int __cdecl dib_row_align4(int width)

{
  return ((int)(width + 3 + (width + 3 >> 0x1f & 3U)) >> 2) << 2;
}

/* 005c55d0 auto */
u32 DibSurface::get_pitch()

{
  return stride;
}

/* 005c55f0 auto */
/* Library Function - Single Match
    public: unsigned int __thiscall CCheckListBox::GetCheckStyle(void)
   
   Library: Visual Studio 1998 Debug */

GdiDib *DibSurface::get_dib()

{
  return dib;
}

/* 005c5610 auto */
int DibSurface::next_row(iptr row_ptr)

{
  return signed_pitch + row_ptr;
}

/* 005c5640 auto */
uptr DibSurface::get_bits()

{
  return bits;
}

/* 005c56f0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 GamePalette::get_version()

{
  return pal_version;
}

/* 005c5710 auto */
int DibSurface::prev_row(iptr row_ptr)

{
  return row_ptr - signed_pitch;
}

/* 005c6480 auto */
void GamePalette::commit_range(int first,int count)

{
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  logpal_apply_range((GameLogPalette *)this,hpalette,first,count);
  new_version();
  return;
}

/* 005c64da auto */
GamePalette *GamePalette::init()

{
  u32 hpal;
  
  logpal_init_from_system((GameLogPalette *)this);
  hpal = logpal_create_hpalette((GameLogPalette *)this);
  hpalette = hpal;
  new_version();
  tint_red = '\0';
  tint_green = '\0';
  tint_blue = '\0';
  tint_saved_rgb = 0;
  fade_dst_rgb = tint_saved_rgb;
  fade_src_rgb = fade_dst_rgb;
  return this;
}

/* 005c656b auto */
GamePalette *GamePalette::destroy()

{
  GamePalette *result;

  result = this;
  logpal_delete_hpalette(hpalette);
  if (fade_src_rgb != 0) {
    mem_handle_free(fade_src_rgb);
  }
  if (fade_dst_rgb != 0) {
    mem_handle_free(fade_dst_rgb);
  }
  if (tint_saved_rgb != 0) {
    result = (GamePalette *)mem_handle_free(tint_saved_rgb);
  }
  return result;
}

/* 005c65f9 auto */
void GamePalette::load_ctab_res(i32 res_id)

{
  int res;
  i32 *data;
  i32 first;
  i32 count;
  char res_type[4];
  
  res_type[0] = 0x43;
  res_type[1] = 0x54;
  res_type[2] = 0x41;
  res_type[3] = 0x42;
  res = res_load((iptr)res_type,res_id);
  if (res == 0) {
    dbg_logf_noret((const char *)&str_error_color_resource_not_found,res_id);
  }
  else {
    data = (i32 *)res_lock(res);
    first = endian_from_le(*data);
    count = endian_from_le(data[1]);
    set_rgb(first,count,(iptr)(data + 2));
    res_unlock(res);
    res_free(res);
  }
  return;
}

/* 005c6a42 auto */
void GamePalette::new_version()

{
  uint rnd;
  
  pal_version = 0;
  while (pal_version == 0) {
    rnd = crt_rand_at("palette_new_version");
    pal_version = rnd & 0x7fff;
  }
  return;
}

/* 005c6a8d auto */
void GamePalette::reserve_range(i32 first,i32 count)

{
  logpal_reserve_range((GameLogPalette *)this,first,count);
  commit_range(first,count);
  return;
}

/* 005c6b63 auto */
void GamePalette::get_rgb(iptr dest,i32 first,i32 count)

{
  palette_copy_rgb_range((GameLogPalette *)this,dest,first,count);
  return;
}

/* 005c6b93 auto */
void GamePalette::set_entry(i32 index,i32 red,i32 green, i32 blue)

{
  palette_set_entry_rgb((GameLogPalette *)this,index,red,green,blue);
  commit_range(index,1);
  return;
}

/* 005c6da8 auto */
void GamePalette::set_rgb(int first,int count,iptr rgb)

{
  i32 hi_bits;
  i32 index;
  
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  for (index = first; index < count + first; index++) {
    hi_bits = (i32)((uint)rgb >> 8);
    palette_set_entry_rgb((GameLogPalette *)this,index,CONCAT31(hi_bits,*(i8 *)rgb),
                 CONCAT31(hi_bits,*(i8 *)(rgb + 1)),
                 CONCAT31(hi_bits,*(i8 *)(rgb + 2)));
    rgb += 3;
  }
  commit_range(first,count);
  return;
}

/* 005c71f3 auto */
void GamePalette::fade_begin(u32 first,u32 count,u32 steps,iptr target_pal)

{
  u32 handle;
  iptr dst_buf;
  GamePalette *target_logpal;
  u32 count_arg;
  
  fade_steps = steps;
  fade_first = first;
  fade_count = count;
  handle = mem_handle_alloc(count * 3);
  fade_src_rgb = handle;
  handle = mem_handle_alloc(count * 3);
  fade_dst_rgb = handle;
  dst_buf = (iptr)mem_handle_lock(fade_src_rgb);
  palette_copy_rgb_range((GameLogPalette *)this,dst_buf,first,count);
  mem_handle_unlock(fade_src_rgb);
  dst_buf = (iptr)mem_handle_lock(fade_dst_rgb);
  handle = first;
  count_arg = count;
  target_logpal = ((GamePalette *)target_pal)->get_logpalette();
  palette_copy_rgb_range((GameLogPalette *)target_logpal,dst_buf,handle,count_arg);
  mem_handle_unlock(fade_dst_rgb);
  reserve_range(first,count);
  return;
}

/* 005c72f8 auto */
void GamePalette::fade_end()

{
  iptr src_buf;
  u32 handle;
  
  src_buf = (iptr)mem_handle_lock(fade_src_rgb);
  set_rgb(fade_first,fade_count,src_buf);
  mem_handle_unlock(fade_src_rgb);
  handle = mem_handle_free(fade_src_rgb);
  fade_src_rgb = handle;
  handle = mem_handle_free(fade_dst_rgb);
  fade_dst_rgb = handle;
  return;
}

/* 005c738e auto */
void GamePalette::fade_step(int step)

{
  char dst_weight;
  char src_weight;
  iptr src_buf;
  iptr dst_buf;
  iptr offset;
  i32 green;
  i32 red;
  i32 blue;
  
  if ((step < 0) || ((int)fade_steps < step)) {
    dbg_logf((const char *)&str_color_scale_factor_out_of_range,step);
  }
  else {
    src_weight = (char)step;
    dst_weight = (char)fade_steps - src_weight;
    src_buf = (iptr)mem_handle_lock(fade_src_rgb);
    dst_buf = (iptr)mem_handle_lock(fade_dst_rgb);
    for (offset = 0; offset < (int)(fade_count * 3); offset += 3) {
      red = (red & 0xffffff00 | (byte)(
                          (char)((longlong)(ulonglong)*(byte *)(offset + dst_buf) /
                                (longlong)(int)fade_steps) * dst_weight +
                          (char)((longlong)(ulonglong)*(byte *)(offset + src_buf) /
                                (longlong)(int)fade_steps) * src_weight));
      green = (green & 0xffffff00 | (byte)(
                          (char)((longlong)(ulonglong)*(byte *)(offset + 1 + src_buf) /
                                (longlong)(int)fade_steps) * src_weight +
                          (char)((longlong)(ulonglong)*(byte *)(offset + 1 + dst_buf) /
                                (longlong)(int)fade_steps) * dst_weight));
      blue = (blue & 0xffffff00 | (byte)(
                         (char)((longlong)(ulonglong)*(byte *)(offset + 2 + dst_buf) /
                               (longlong)(int)fade_steps) * dst_weight +
                         (char)((longlong)(ulonglong)*(byte *)(offset + 2 + src_buf) /
                               (longlong)(int)fade_steps) * src_weight));
      logpal_set_entry((GameLogPalette *)this,fade_first + offset / 3,red,green,blue);
    }
    palette_animate((GameLogPalette *)this,hpalette,fade_first,fade_count);
    mem_handle_unlock(fade_src_rgb);
    mem_handle_unlock(fade_dst_rgb);
  }
  return;
}

/* 005c8200 auto */
i32 __cdecl font_handle_create(i32 face,int size,i32 style)

{
  LOGFONTA logfont;
  i32 handle;
  HFONT hfont2;
  i32 *data;
  
  logfont.lfHeight = -size;
  logfont.lfWidth = 0;
  logfont.lfEscapement = 0;
  logfont.lfOrientation = 0;
  logfont.lfCharSet = 1;
  logfont.lfOutPrecision = 4;
  logfont.lfClipPrecision = 1;
  logfont.lfQuality = '\0';
  logfont.lfPitchAndFamily = '\0';
  if (((byte)style & 1) == 0) {
    logfont.lfWeight = 400;
  }
  else {
    logfont.lfWeight = 700;
  }
  logfont.lfItalic = (byte)style & 2;
  logfont.lfUnderline = (byte)style & 4;
  logfont.lfStrikeOut = (byte)style & 8;
  switch(face) {
  case 0:
    logfont.lfPitchAndFamily = 16;
    crt_strcpy(logfont.lfFaceName,str_times_new_roman);
    break;
  case 1:
    logfont.lfPitchAndFamily = ' ';
    crt_strcpy(logfont.lfFaceName,str_arial);
    break;
  case 2:
    crt_strcpy(logfont.lfFaceName,str_system_2);
    break;
  case 3:
    logfont.lfPitchAndFamily = '0';
    crt_strcpy(logfont.lfFaceName,str_courier);
  }
  handle = 0;
  hfont2 = CreateFontIndirectA(&logfont);
  if (hfont2 != (HFONT)0x0) {
    handle = mem_handle_alloc(8);
    data = (i32 *)mem_handle_lock(handle);
    *data = hfont2;
    data[1] = 1;
    mem_handle_unlock(handle);
  }
  return handle;
}

/* 005c841d auto */
i32 __cdecl font_handle_destroy(i32 handle)

{
  i32 *data;
  i32 result;
  
  data = (i32 *)mem_handle_lock(handle);
  if (data == (i32 *)0x0) {
    result = 0;
  }
  else {
    if (data[1] == 1) {
      DeleteObject((HGDIOBJ)*data);
    }
    mem_handle_unlock(handle);
    result = mem_handle_free(handle);
  }
  return result;
}

/* 005c847f auto */
LONG __cdecl font_measure_height(i32 handle)

{
  int size;
  HDC hdc;
  HGDIOBJ h;
  tagTEXTMETRICA metrics;
  i32 *data;
  
  size = mem_handle_size(handle);
  if (size == 0) {
    metrics.tmHeight = 1;
  }
  else {
    data = (i32 *)mem_handle_lock(handle);
    hdc = GetDC((HWND)0x0);
    h = SelectObject(hdc,(HGDIOBJ)*data);
    GetTextMetricsA(hdc,&metrics);
    SelectObject(hdc,h);
    ReleaseDC((HWND)0x0,hdc);
    mem_handle_unlock(handle);
  }
  return metrics.tmHeight;
}

/* 005c8514 auto */
int __cdecl font_handle_line_spacing(i32 handle)

{
  HDC hdc;
  HGDIOBJ h;
  tagTEXTMETRICA metrics;
  i32 *data;
  
  data = (i32 *)mem_handle_lock(handle);
  hdc = GetDC((HWND)0x0);
  h = SelectObject(hdc,(HGDIOBJ)*data);
  GetTextMetricsA(hdc,&metrics);
  SelectObject(hdc,h);
  ReleaseDC((HWND)0x0,hdc);
  mem_handle_unlock(handle);
  return metrics.tmExternalLeading + metrics.tmHeight;
}

/* 005c858e auto */
LONG __cdecl font_measure_width(i32 handle,const char *text)

{
  HDC hdc;
  HGDIOBJ h;
  size_t c;
  tagSIZE *lpsz;
  tagSIZE extent;
  i32 *data;
  
  data = (i32 *)mem_handle_lock(handle);
  hdc = GetDC((HWND)0x0);
  h = SelectObject(hdc,(HGDIOBJ)*data);
  lpsz = &extent;
  c = _strlen((char *)text);
  GetTextExtentPointA(hdc,text,c,lpsz);
  SelectObject(hdc,h);
  ReleaseDC((HWND)0x0,hdc);
  mem_handle_unlock(handle);
  return extent.cx;
}

/* 005c8791: written by hand in src/game/fixups.cpp */

/* 005c8c83 auto */
void __cdecl region_dispose(int region)

{
  if (region == 0) {
    dbg_log((const char *)&str_error_tried_to_dispose_null_region_in_ms);
  }
  return;
}

/* 005c8caf: written by hand in src/game/fixups.cpp */

/* 005cbdb0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 GamePalette::get_hpalette()

{
  return hpalette;
}

/* 005cd6e0 auto */
void __cdecl gfx_scale_init(void)

{
  int i;
  
  for (i = 0; i < 0x10; i++) {
    default_draw_font_obj_block.gfx_scale_caches[i].last_used = 0;
    default_draw_font_obj_block.gfx_scale_caches[i].num = 0;
    default_draw_font_obj_block.gfx_scale_caches[i].den = 0;
  }
  gfx_set_scale_ratio(1,1);
  gfx_set_scale_tables(1,1,1,1);
  return;
}

/* 005cd775 auto */
void __cdecl gfx_set_scale_ratio(int num,int den)

{
  i32 now;
  int slot;
  int index;
  int oldest_tick;
  int accum;
  int value;
  
  if (num < 2) {
    num = 1;
  }
  if (den < 2) {
    den = 1;
  }
  gfx_scale.num = num;
  gfx_scale.den = den;
  oldest_tick = timer_ticks();
  slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      slot = index;
      break;
    }
    if (default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      slot = index;
    }
  }
  if (0xf < index) {
    default_draw_font_obj_block.gfx_scale_caches[slot].num = gfx_scale.num;
    default_draw_font_obj_block.gfx_scale_caches[slot].den = gfx_scale.den;
    now = timer_ticks();
    default_draw_font_obj_block.gfx_scale_caches[slot].last_used = now;
    value = 0;
    if (gfx_scale.num < gfx_scale.den) {
      accum = 0;
      index = 0;
      while (index < 0x400) {
        for (; (-1 < accum && (index < 0x400)); index++) {
          default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
          accum -= gfx_scale.den;
        }
        accum += gfx_scale.num;
        value++;
      }
    }
    else {
      accum = gfx_scale.num;
      index = 0;
      while (index < 0x400) {
        for (; (gfx_scale.den <= accum && (index < 0x400)); index++) {
          default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
          accum -= gfx_scale.den;
        }
        accum += gfx_scale.num;
        value++;
      }
      }
  }
  gfx_scale_tables.table = (i32 *)default_draw_font_obj_block.gfx_scale_caches[slot].table;
  return;
}

/* 005cda06 auto */
void __cdecl gfx_get_scale_ratio(iptr out_num,iptr out_den)

{
  *(i32 *)out_num = gfx_scale.num;
  *(i32 *)out_den = gfx_scale.den;
  return;
}

/* 005cda2a auto */
void __cdecl gfx_set_scale_tables(int x_num,int x_den,int y_num,int y_den)

{
  i32 now;
  int index;
  int oldest_tick;
  int y_slot;
  int x_slot;
  
  if (x_num < 2) {
    x_num = 1;
  }
  if (x_den < 2) {
    x_den = 1;
  }
  if (y_num < 2) {
    y_num = 1;
  }
  if (y_den < 2) {
    y_den = 1;
  }
  gfx_scale.x_num = x_num;
  gfx_scale.x_den = x_den;
  gfx_scale.y_num = y_num;
  gfx_scale.y_den = y_den;
  oldest_tick = timer_ticks();
  x_slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.x_num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.x_den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      x_slot = index;
      break;
    }
    if (default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      x_slot = index;
    }
  }
  if (0xf < index) {
    gfx_scale_build_table(x_slot,gfx_scale.x_num,gfx_scale.x_den);
  }
  gfx_scale_tables.x_table = (i8 *)default_draw_font_obj_block.gfx_scale_caches[x_slot].table;
  oldest_tick = timer_ticks();
  y_slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.y_num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.y_den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      break;
    }
    if ((default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) && (x_slot != index)) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      y_slot = index;
    }
  }
  if (0xf < index) {
    gfx_scale_build_table(y_slot,gfx_scale.y_num,gfx_scale.y_den);
    index = y_slot;
  }
  gfx_scale_tables.y_table = (i8 *)default_draw_font_obj_block.gfx_scale_caches[index].table;
  return;
}

/* 005cdcdb auto */
void __cdecl gfx_scale_build_table(int slot,int num,int den)

{
  i32 now;
  int index;
  int accum;
  int value;
  
  default_draw_font_obj_block.gfx_scale_caches[slot].num = num;
  default_draw_font_obj_block.gfx_scale_caches[slot].den = den;
  now = timer_ticks();
  default_draw_font_obj_block.gfx_scale_caches[slot].last_used = now;
  value = 0;
  if (num < den) {
    accum = 0;
    index = 0;
    while (index < 0x400) {
      for (; (-1 < accum && (index < 0x400)); index++) {
        default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
        accum -= den;
      }
      accum += num;
      value++;
    }
  }
  else {
    accum = num;
    index = 0;
    while (index < 0x400) {
      for (; (den <= accum && (index < 0x400)); index++) {
        default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
        accum -= den;
      }
      accum += num;
      value++;
    }
  }
  return;
}

/* 005cde2c auto */
/* Library Function - Single Match
    public: __thiscall CString::CString(void)
   
   Library: Visual Studio 1998 Debug */

Sprite * __thiscall sprite_init(Sprite *self)

{
  self->clear_fields();
  return self;
}

/* 005cde4d auto */
Sprite * __thiscall sprite_destroy(Sprite *self)

{
  uptr ptr;
  
  if (self->data_handle != 0) {
    if (self->data_ptr != 0) {
      ptr = mem_handle_unlock(self->data_handle);
      self->data_ptr = ptr;
    }
    self = (Sprite *)mem_handle_free(self->data_handle);
  }
  return self;
}

/* 005cdea1 auto */
i32 Sprite::create_solid(i32 width,i32 height,uint color)

{
  DibSurface surface;
  i32 result;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  surface.DibSurface::create(width,height);
  surface.fill(color & 0xff);
  encode_rect((iptr)&surface,0,0,width,height);
  SetRect((LPRECT)&bounds_rect,0,0,0,0);
  return result;
}

/* 005cdf50 auto */
void Sprite::free()

{
  uptr ptr;
  u32 handle;
  
  if (data_ptr != 0) {
    ptr = mem_handle_unlock(data_handle);
    data_ptr = ptr;
  }
  if (data_handle != 0) {
    handle = mem_handle_free(data_handle);
    data_handle = handle;
  }
  clear_fields();
  return;
}

/* 005cebb4 auto */
void Sprite::grab_surface_rect(iptr surface,iptr rect)

{
  u32 handle;
  
  handle = encode_from_surface(surface,0xffffffff,(RECT *)rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cebec auto */
void Sprite::encode_rect(iptr surface,int x,int y,int width,int height)

{
  u32 handle;
  tagRECT rect;
  
  SetRect(&rect,x,y,x + width,height + y);
  handle = encode_from_surface(surface,0xffffffff,&rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cec44 auto */
void Sprite::grab_rect(iptr surface,uint key_color,iptr rect)

{
  u32 handle;
  
  handle = encode_from_surface(surface,key_color & 0xff,(RECT *)rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cedad auto */
void Sprite::grab_from_surface(iptr surface,uint key_color,int x,int y,int width, int height)

{
  u32 handle;
  tagRECT rect;
  
  SetRect(&rect,x,y,width + x,height + y);
  handle = encode_from_surface(surface,key_color & 0xff,&rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cef31 auto */
iptr Sprite::draw(iptr dest,iptr surface,i32 x,i32 y)

{
  blit_to_surface((int *)dest,(DibSurface *)surface,0xffffffff,x,y);
  return dest;
}

/* 005cef66 auto */
iptr Sprite::draw_keyed(iptr dest,iptr surface,uint key_color,i32 x, i32 y)

{
  blit_to_surface((int *)dest,(DibSurface *)surface,key_color & 0xff,x,y);
  return dest;
}

/* 005cf126 auto */
iptr Sprite::draw_silhouette(iptr dest,iptr surface,i32 x,i32 y, i32 color)

{
  blit_silhouette_to_surface((int *)dest,(DibSurface *)surface,0xffffffff,x,y,color);
  return dest;
}

/* 005cf23f auto */
void Sprite::merge_onto(iptr dest)

{
  merge_at(this,(Sprite *)dest,0,0,0);
  return;
}

/* 005cf2ff auto */
void Sprite::lock()

{
  uptr ptr;
  
  if (data_ptr == 0) {
    ptr = (uptr)mem_handle_lock(data_handle);
    data_ptr = ptr;
  }
  return;
}

/* 005cf337 auto */
void Sprite::unlock()

{
  uptr ptr;
  
  if (data_ptr != 0) {
    ptr = mem_handle_unlock(data_handle);
    data_ptr = ptr;
  }
  return;
}

/* 005cf36f auto */
bool Sprite::has_data()

{
  return data_ptr != 0;
}

/* 005cf39b auto */
void Sprite::set_origin(u32 x,u32 y)

{
  offset_x = x;
  offset_y = y;
  return;
}

/* 005cf3c5 auto */
void Sprite::get_origin(iptr out_x,iptr out_y)

{
  *(u32 *)out_x = offset_x;
  *(u32 *)out_y = offset_y;
  return;
}

/* 005cf467 auto */
void Sprite::replace_color(i32 old_color,i32 new_color)

{
  int rows_left;
  int *len_ptr;
  char had_data;
  char *cursor;
  int run_len;
  int rows;
  
  had_data = has_data();
  if (had_data == '\0') {
    lock();
    dbg_log((const char *)&str_warning_sprite_not_locked_in_changecolor);
  }
  rows = rect_height((RECT *)&bounds_rect);
  cursor = (char *)data_ptr;
  while (rows_left = rows - 1, rows != 0) {
    len_ptr = (int *)(cursor + 4);
    cursor += 8;
    run_len = *len_ptr;
    while (rows = rows_left, run_len != 0) {
      if (*cursor == (char)old_color) {
        *cursor = (char)new_color;
      }
      cursor++;
      run_len--;
    }
  }
  if (had_data == '\0') {
    unlock();
  }
  return;
}

/* 005cf64c auto */
int Sprite::encode_from_surface(iptr surface,int key_mode,RECT *src_rect)

{
  char locked;
  int rect_h;
  int rect_w;
  u32 freed;
  tagRECT rect;
  u8 *scan;
  int height2;
  i32 *out;
  int all_empty;
  int row;
  u8 key2;
  int col;
  i32 *run_lens;
  int was_locked;
  int handle;
  u8 *top_row;
  u8 *bottom_row;
  iptr width2;
  i32 *run_starts;
  int min_left;
  int first_row2;
  int max_right;
  int last_row;
  int total_size;
  u8 **run_ptrs;
  
  was_locked = 1;
  all_empty = 1;
  SetRect(&rect,src_rect->left,src_rect->top,src_rect->right,
          src_rect->bottom);
  IntersectRect(&rect,&rect,(RECT *)(surface + 0x14));
  min_left = 10000;
  max_right = 0;
  total_size = 0;
  width2 = rect.right - rect.left;
  height2 = rect.bottom - rect.top;
  locked = ((DibSurface *)surface)->is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_on_sprite_extrac);
    was_locked = 0;
    ((DibSurface *)surface)->lock();
  }
  top_row = (u8 *)((DibSurface *)surface)->pixel_ptr(rect.left,rect.top);
  if (top_row == (u8 *)0x0) {
    dbg_logf_int2((const char *)&str_error_sprite_extract_illegal_pixel_mappi,rect.left,rect.top);
    if (was_locked == 0) {
      ((DibSurface *)surface)->unlock();
    }
    handle = 0;
  }
  else {
    if (key_mode == -1) {
      key2 = *top_row;
    }
    else if (key_mode != -2) {
      key2 = (u8)key_mode;
    }
    row = 0;
    while (row < height2) {
      scan = top_row;
      for (col = 0; col < width2; col++) {
        if ((*scan != key2) || (key_mode == -2)) {
          first_row2 = row;
          scan++;
          top_row = (u8 *)((DibSurface *)surface)->prev_row((iptr)top_row);
          row = height2;
          all_empty = 0;
          break;
        }
        scan++;
      }
      row++;
      top_row = (u8 *)((DibSurface *)surface)->next_row((iptr)top_row);
    }
    if (all_empty == 0) {
      bottom_row = (u8 *)((DibSurface *)surface)->pixel_ptr(rect.left,
                                  rect.top + height2 - 1);
      row = 0;
      while (row < height2) {
        scan = bottom_row;
        for (col = 0; col < width2; col++) {
          if ((*scan != key2) || (key_mode == -2)) {
            last_row = height2 - row;
            row = height2;
            scan++;
            break;
          }
          scan++;
        }
        row++;
        bottom_row = (u8 *)((DibSurface *)surface)->prev_row((iptr)bottom_row);
      }
      run_ptrs = (u8 **)operator_new(0xa00);
      run_starts = (i32 *)operator_new(0xa00);
      run_lens = (i32 *)operator_new(0xa00);
      if (((run_ptrs == (u8 **)0x0) || (run_starts == (i32 *)0x0)) || (run_lens == (i32 *)0x0)) {
        dbg_log((const char *)&str_error_unable_to_allocate_scan_line_table);
        handle = 0;
      }
      else {
        row = 0;
        while (row < last_row - first_row2) {
          total_size += 8;
          run_starts[row] = 0;
          run_lens[row] = 0;
          scan = top_row;
          for (col = 0; col < width2; col++) {
            if ((*scan != key2) || (key_mode == -2)) {
              run_ptrs[row] = scan;
              run_starts[row] = col;
              break;
            }
            scan++;
          }
          if ((width2 != col) && (run_starts[row] < min_left)) {
            min_left = run_starts[row];
          }
          scan = top_row + width2;
          for (col = 0; scan = scan - 1, col < width2; col++)
          {
            if ((*scan != key2) || (key_mode == -2)) {
              run_lens[row] =
                   (width2 - col) - run_starts[row];
              total_size += run_lens[row];
              break;
            }
          }
          if ((width2 != col) &&
             (max_right <
              run_lens[row] + run_starts[row])) {
            max_right = run_lens[row] +
                       run_starts[row];
          }
          row++;
          top_row = (u8 *)((DibSurface *)surface)->next_row((iptr)top_row);
        }
        if (data_ptr != 0) {
          unlock();
        }
        if (data_handle != 0) {
          freed = mem_handle_free(data_handle);
          data_handle = freed;
        }
        total_size += 8;
        handle = mem_handle_alloc(total_size);
        if (handle == 0) {
          dbg_logf((const char *)&str_error_couldn_t_allocate_memory_for_sprit,total_size);
          if (was_locked == 0) {
            ((DibSurface *)surface)->unlock();
          }
          operator_delete(run_ptrs);
          operator_delete(run_starts);
          operator_delete(run_lens);
        }
        else {
          SetRect((LPRECT)&bounds_rect,min_left,first_row2,max_right,last_row);
          rect_h = rect_height(&rect);
          rect_w = rect_width(&rect);
          SetRect((LPRECT)this,0,0,rect_w,rect_h);
          if (key_mode == -2) {
            key_color = '\0';
          }
          else {
            key_color = key2;
          }
          offset_x = 0;
          offset_y = 0;
          out = (i32 *)mem_handle_lock(handle);
          for (row = 0; row < last_row - first_row2; row++) {
            *out = run_starts[row];
            out[1] = run_lens[row];
            out += 2;
            mem_move_src_dst((iptr)run_ptrs[row],(iptr)out,
                         run_lens[row]);
            out = (i32 *)((int)out + run_lens[row]);
          }
          *out = 0;
          out = (i32 *)mem_handle_unlock(handle);
          if (was_locked == 0) {
            ((DibSurface *)surface)->unlock();
          }
          operator_delete(run_ptrs);
          operator_delete(run_starts);
          operator_delete(run_lens);
        }
      }
    }
    else {
      SetRect((LPRECT)&bounds_rect,0,0,0,0);
      rect_h = rect_height(&rect);
      rect_w = rect_width(&rect);
      SetRect((LPRECT)this,0,0,rect_w,rect_h);
      if (key_mode == -2) {
        key_color = '\0';
      }
      else {
        key_color = key2;
      }
      offset_x = 0;
      offset_y = 0;
      if (was_locked == 0) {
        ((DibSurface *)surface)->unlock();
      }
      data_ptr = 0;
      handle = 0;
    }
  }
  return handle;
}

/* 005d056c auto */
int *Sprite::blit_to_surface(int *out_rect,DibSurface *surface,int key_arg,int x, int y)

{
  int trim_y;
  int tmp;
  int scaled_trim_x;
  int scaled_trim_y;
  i32 clip_w;
  int scaled_h;
  BOOL intersects;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  i32 bits;
  iptr scale_tbl;
  i32 clip_h;
  int skip_x;
  int dst_y;
  int dst_x;
  i32 key2;
  tagRECT clip_rect;
  int skip_y;
  int trim_x;
  
  if (key_arg == -1) {
    (*((char *)&key_arg + 0)) = key_color;
  }
  key2 = (key2 & 0xffffff00 | ((u8)key_arg));
  trim_x = bounds_rect - frame_rect;
  trim_y = bounds_top - frame_top;
  tmp = width();
  if (((tmp * gfx_scale.num) / gfx_scale.den < 0x401) &&
     (tmp = height(), (tmp * gfx_scale.num) / gfx_scale.den < 0x401)) {
    scaled_trim_x = gfx_scale_length(trim_x,0,1);
    scaled_trim_y = gfx_scale_length(trim_y,0,1);
    tmp = gfx_scale_length(offset_x,0,0);
    dst_x = scaled_trim_x + tmp + x;
    tmp = gfx_scale_length(offset_y,0,0);
    dst_y = scaled_trim_y + tmp + y;
    clip_h = 0;
    tmp = scaled_trim_y;
    clip_w = get_height();
    scaled_h = gfx_scale_length(clip_w,tmp,clip_h);
    clip_h = 0;
    tmp = scaled_trim_x;
    clip_w = get_width();
    tmp = gfx_scale_length(clip_w,tmp,clip_h);
    SetRect(&clip_rect,0,0,tmp,scaled_h);
    OffsetRect(&clip_rect,dst_x,dst_y);
    intersects = IntersectRect(&clip_rect,&clip_rect,(RECT *)&surface->clip_left);
    if (intersects != 0) {
      skip_x = 0;
      clip_w = rect_width(&clip_rect);
      if (dst_x < clip_rect.left) {
        skip_x = clip_rect.left - dst_x;
        dst_x = clip_rect.left;
      }
      skip_y = 0;
      clip_h = rect_height(&clip_rect);
      if (dst_y < clip_rect.top) {
        skip_y = clip_rect.top - dst_y;
        dst_y = clip_rect.top;
      }
      tmp = skip_y;
      scaled_h = trim_x;
      surf_height = surface->surface_height();
      dib = surface->get_dib();
      top_down = dib_is_top_down(dib);
      pitch = surface->get_pitch();
      pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
      scale_tbl = (iptr)gfx_scale_tables.table;
      bits = surface->get_bits();
      sprite_blit_rows_keyed(data_ptr,bits,scale_tbl,key2,dst_x,dst_y,pitch,surf_height,clip_w,clip_h
                   ,skip_x,tmp,scaled_h,trim_y,scaled_trim_x,scaled_trim_y);
    }
    *out_rect = clip_rect.left;
    out_rect[1] = clip_rect.top;
    out_rect[2] = clip_rect.right;
    out_rect[3] = clip_rect.bottom;
    return out_rect;
  }
  dbg_log((const char *)&str_error_sprite_size_exceeds_scale_table_si);
  SetRect(&clip_rect,0,0,0,0);
  *out_rect = clip_rect.left;
  out_rect[1] = clip_rect.top;
  out_rect[2] = clip_rect.right;
  out_rect[3] = clip_rect.bottom;
  return out_rect;
}

/* 005d10cd auto */
int *Sprite::blit_silhouette_to_surface(int *out_rect,DibSurface *surface,int key_arg,int x, int y,i32 color)

{
  int trim_y;
  int tmp;
  int scaled_trim_x;
  int scaled_trim_y;
  i32 clip_w;
  int scaled_h;
  BOOL intersects;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  i32 bits;
  iptr scale_tbl;
  i32 clip_h;
  int skip_x;
  int dst_y;
  int dst_x;
  i32 key2;
  tagRECT clip_rect;
  int skip_y;
  int trim_x;
  
  if (key_arg == -1) {
    (*((char *)&key_arg + 0)) = key_color;
  }
  key2 = (key2 & 0xffffff00 | ((u8)key_arg));
  trim_x = bounds_rect - frame_rect;
  trim_y = bounds_top - frame_top;
  tmp = width();
  if (((tmp * gfx_scale.num) / gfx_scale.den < 0x401) &&
     (tmp = height(), (tmp * gfx_scale.num) / gfx_scale.den < 0x401)) {
    scaled_trim_x = gfx_scale_length(trim_x,0,1);
    scaled_trim_y = gfx_scale_length(trim_y,0,1);
    tmp = gfx_scale_length(offset_x,0,0);
    dst_x = x + tmp + scaled_trim_x;
    tmp = gfx_scale_length(offset_y,0,0);
    dst_y = y + tmp + scaled_trim_y;
    clip_h = 0;
    tmp = scaled_trim_y;
    clip_w = get_height();
    scaled_h = gfx_scale_length(clip_w,tmp,clip_h);
    clip_h = 0;
    tmp = scaled_trim_x;
    clip_w = get_width();
    tmp = gfx_scale_length(clip_w,tmp,clip_h);
    SetRect(&clip_rect,0,0,tmp,scaled_h);
    OffsetRect(&clip_rect,dst_x,dst_y);
    intersects = IntersectRect(&clip_rect,&clip_rect,(RECT *)&surface->clip_left);
    if (intersects != 0) {
      skip_x = 0;
      clip_w = rect_width(&clip_rect);
      if (dst_x < clip_rect.left) {
        skip_x = clip_rect.left - dst_x;
        dst_x = clip_rect.left;
      }
      skip_y = 0;
      clip_h = rect_height(&clip_rect);
      if (dst_y < clip_rect.top) {
        skip_y = clip_rect.top - dst_y;
        dst_y = clip_rect.top;
      }
      tmp = skip_y;
      scaled_h = trim_x;
      surf_height = surface->surface_height();
      dib = surface->get_dib();
      top_down = dib_is_top_down(dib);
      pitch = surface->get_pitch();
      pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
      scale_tbl = (iptr)gfx_scale_tables.table;
      bits = surface->get_bits();
      sprite_blit_rows_solid(data_ptr,bits,scale_tbl,key2,dst_x,dst_y,pitch,surf_height,clip_w,clip_h
                   ,skip_x,tmp,scaled_h,trim_y,scaled_trim_x,scaled_trim_y,color);
    }
    *out_rect = clip_rect.left;
    out_rect[1] = clip_rect.top;
    out_rect[2] = clip_rect.right;
    out_rect[3] = clip_rect.bottom;
    return out_rect;
  }
  dbg_log((const char *)&str_error_sprite_size_exceeds_scale_table_si_2);
  SetRect(&clip_rect,0,0,0,0);
  *out_rect = clip_rect.left;
  out_rect[1] = clip_rect.top;
  out_rect[2] = clip_rect.right;
  out_rect[3] = clip_rect.bottom;
  return out_rect;
}

/* 005d1612 auto */
void Sprite::merge_at(Sprite *src,Sprite *dst,i32 unused,int dx, int dy)

{
  byte row_mask;
  LONG width;
  LONG height;
  int src_top;
  iptr src_w;
  int src_h;
  int merged;
  int dst_h;
  iptr new_h;
  int value;
  u32 new_handle;
  int dst_end;
  int src_start;
  int src_end;
  u32 freed;
  int written;
  int *dst_cursor;
  int *src_cursor;
  int row;
  int col;
  int cols_left;
  int *out;
  tagRECT rect;
  
  if (src->data_handle != 0) {
    if (dst->data_handle == 0) {
      SetRect((LPRECT)dst,(int)src->frame_rect,(int)src->frame_top,*(int *)((iptr)src + 8) + dx,
              *(int *)((iptr)src + 0xc) + dy);
      dst->bounds_rect = src->bounds_rect;
      dst->bounds_top = src->bounds_top;
      *(i32 *)dst->bounds_right = *(i32 *)((iptr)src + 0x18);
      *(i32 *)dst->bounds_bottom = *(i32 *)((iptr)src + 0x1c);
      OffsetRect((LPRECT)&dst->bounds_rect,dx,dy);
      dst->offset_x = (int)src->offset_x + dx;
      dst->offset_y = (int)src->offset_y + dy;
      dst->load_scale_num = src->load_scale_num;
      dst->load_scale_den = src->load_scale_den;
      dst->key_color = src->key_color;
      dst->data_handle = src->data_handle;
      mem_handle_clone((iptr)&dst->data_handle);
      dst->lock();
    }
    else {
      src_top = ((int)src->bounds_top + dy) - dst->bounds_top;
      src_w = rect_width((RECT *)((iptr)src + 0x10));
      value = (int)src->bounds_rect;
      src_h = rect_height((RECT *)((iptr)src + 0x10));
      merged = rect_width((RECT *)&dst->bounds_rect);
      merged = dst->bounds_rect + merged;
      dst_h = rect_height((RECT *)&dst->bounds_rect);
      new_h = src_h + src_top;
      if (src_h + src_top <= dst_h) {
        new_h = dst_h;
      }
      value = value + src_w + dx;
      if (value <= merged) {
        value = merged;
      }
      SetRect(&rect,0,0,value,new_h);
      height = rect.bottom;
      width = rect.right;
      src->lock();
      dst->lock();
      src_cursor = (int *)src->data_ptr;
      dst_cursor = (int *)dst->data_ptr;
      new_handle = mem_handle_alloc(width * height + height * 8);
      out = (int *)mem_handle_lock(new_handle);
      for (row = 0; row < height; row++) {
        row_mask = row < dst_h;
        if ((src_top <= row) && (row < src_h + src_top)) {
          row_mask = row_mask | 2;
        }
        switch(row_mask) {
        case 0:
          *out = 0;
          out[1] = 0;
          out += 2;
          break;
        case 1:
          *out = *dst_cursor;
          out[1] = dst_cursor[1];
          value = out[1];
          mem_move_src_dst((iptr)(dst_cursor + 2),(iptr)(out + 2),value);
          dst_cursor = (int *)((int)(dst_cursor + 2) + value);
          out = (int *)((int)(out + 2) + value);
          break;
        case 2:
          *out = *src_cursor + dx;
          out[1] = src_cursor[1];
          value = out[1];
          mem_move_src_dst((iptr)(src_cursor + 2),(iptr)(out + 2),value);
          src_cursor = (int *)((int)(src_cursor + 2) + value);
          out = (int *)((int)(out + 2) + value);
          break;
        case 3:
          value = *dst_cursor;
          dst_end = dst_cursor[1] + value;
          dst_cursor += 2;
          src_start = *src_cursor + dx;
          src_end = src_cursor[1] + src_start;
          src_cursor += 2;
          merged = value;
          if (src_start <= value) {
            merged = src_start;
          }
          *out = merged;
          merged = dst_end;
          if (dst_end <= src_end) {
            merged = src_end;
          }
          out[1] = merged - *out;
          out += 2;
          cols_left = width;
          col = 0;
          while (cols_left != 0) {
            written = 0;
            if ((value <= col) && (col < dst_end)) {
              *(char *)out = (char)*dst_cursor;
              dst_cursor = (int *)((int)dst_cursor + 1);
              written = 1;
            }
            if ((src_start <= col) && (col < src_end)) {
              if ((uint)src->key_color == (int)(char)*src_cursor) {
                if (written == 0) {
                  *(u8 *)out = dst->key_color;
                }
              }
              else {
                *(char *)out = (char)*src_cursor;
              }
              src_cursor = (int *)((int)src_cursor + 1);
              written++;
            }
            if (written != 0) {
              out = (int *)((int)out + 1);
            }
            col++;
            cols_left--;
          }
        }
      }
      OffsetRect(&rect,dst->bounds_rect,dst->bounds_top);
      dst->bounds_rect = rect.left;
      dst->bounds_top = rect.top;
      *(LONG *)dst->bounds_right = rect.right;
      *(LONG *)dst->bounds_bottom = rect.bottom;
      dst->frame_rect = rect.left;
      dst->frame_top = rect.top;
      *(LONG *)dst->frame_right = rect.right;
      *(LONG *)dst->frame_bottom = rect.bottom;
      if (dst->data_ptr != 0) {
        dst->unlock();
      }
      if (dst->data_handle != 0) {
        freed = mem_handle_free(dst->data_handle);
        dst->data_handle = freed;
      }
      mem_handle_unlock(new_handle);
      dst->data_handle = new_handle;
      dst->lock();
    }
  }
  return;
}

/* 005d1b38 auto */
void Sprite::clear_fields()

{
  SetRect((LPRECT)this,0,0,0,0);
  SetRect((LPRECT)&bounds_rect,0,0,0,0);
  offset_x = 0;
  offset_y = 0;
  load_scale_num = 1;
  load_scale_den = 1;
  key_color = '\0';
  data_handle = 0;
  data_ptr = 0;
  return;
}

/* 005d1d00 auto */
int __cdecl gfx_scale_length(int length,int start,int round_up)

{
  int result;
  bool positive;
  int index;
  
  if (length == 0) {
    result = 0;
  }
  else {
    positive = -1 < length;
    if (!positive) {
      length = -length;
    }
    if (gfx_scale.num < gfx_scale.den) {
      index = start;
      result = gfx_scale_tables.table[start] + length;
      for (; gfx_scale_tables.table[index] < result; index++) {
      }
      if ((round_up == 0) && (gfx_scale_tables.table[index] != result)) {
        index--;
      }
    }
    else {
      index = start;
      for (; gfx_scale_tables.table[index] <
             gfx_scale_tables.table[start] + length; index++) {
      }
    }
    result = ((-(uint)positive & 2) - 1) * (index - start);
  }
  return result;
}

/* 005de780 auto */
void __cdecl logpal_init_from_system(GameLogPalette *logpal)

{
  HDC hdc;
  uint raster_caps;
  int num_reserved;
  i8 *sys_color;
  int i;
  
  logpal->version = 0x300;
  logpal->count = 0x100;
  hdc = GetDC((HWND)0x0);
  raster_caps = GetDeviceCaps(hdc,0x26);
  if ((raster_caps & 0x100) == 0) {
    err_report_ctx_block.palette_static_half_count = 0;
    display_palette.palettized = 0;
  }
  else {
    num_reserved = GetDeviceCaps(hdc,0x18);
    err_report_ctx_block.palette_static_half_count = num_reserved / 2;
    GetSystemPaletteEntries(hdc,0,0x100,(LPPALETTEENTRY)logpal->entries);
  }
  for (i = 0; i < err_report_ctx_block.palette_static_half_count; i++) {
    logpal->entries[i].flags = 0;
  }
  for (; i < 0x100 - err_report_ctx_block.palette_static_half_count; i++) {
    logpal->entries[i].red = 0;
    logpal->entries[i].green = 0;
    logpal->entries[i].blue = 0;
    logpal->entries[i].flags = 4;
  }
  for (; i < 0x100; i++) {
    logpal->entries[i].flags = 0;
  }
  if (display_palette.palettized == 0) {
    sys_color = ((i8 *)&logpal_static_system_colors);
    for (i = 0; i < 10; i++) {
      logpal->entries[i].red = *sys_color;
      logpal->entries[i].green = sys_color[1];
      logpal->entries[i].blue = sys_color[2];
      sys_color += 3;
    }
    for (i = 0xf6; i < 0x100; i++) {
      logpal->entries[i].red = *sys_color;
      logpal->entries[i].green = sys_color[1];
      logpal->entries[i].blue = sys_color[2];
      sys_color += 3;
    }
  }
  ReleaseDC((HWND)0x0,hdc);
  return;
}

/* 005de984 auto */
void __cdecl logpal_reserve_range(GameLogPalette *logpal,int first,int count)

{
  int i;
  
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  for (i = first; i < count + first; i++) {
    logpal->entries[i].flags = 1;
  }
  return;
}

/* 005dea62 auto */
void __cdecl palette_animate(GameLogPalette *logpal,HPALETTE hpal,UINT first,UINT count)

{
  if (display_palette.palettized == 1) {
    AnimatePalette(hpal,first,count,(PALETTEENTRY *)&logpal->entries[first]);
  }
  return;
}

/* 005dea9e auto */
void __cdecl palette_get_entry_rgb(GameLogPalette *logpal,int index,iptr out_red,iptr out_green,iptr out_blue)

{
  *(i8 *)out_red = logpal->entries[index].red;
  *(i8 *)out_green = logpal->entries[index].green;
  *(i8 *)out_blue = logpal->entries[index].blue;
  return;
}

/* 005deadb auto */
void __cdecl
logpal_set_entry(GameLogPalette *logpal,int index,i32 red,i32 green,i32 blue)

{
  logpal->entries[index].red = (i8)red;
  logpal->entries[index].green = (i8)green;
  logpal->entries[index].blue = (i8)blue;
  return;
}

/* 005deb12 auto */
void __cdecl
palette_set_entry_rgb(GameLogPalette *logpal,int index,i32 red,i32 green,i32 blue)

{
  int i;
  
  logpal->entries[index].red = (char)red;
  logpal->entries[index].green = (char)green;
  logpal->entries[index].blue = (char)blue;
  logpal->entries[index].flags = 4;
  for (i = 0; i < err_report_ctx_block.palette_static_half_count; i++) {
    if (((logpal->entries[i].red == (char)red) &&
        (logpal->entries[i].green == (char)green)) &&
       (logpal->entries[i].blue == (char)blue)) {
      logpal->entries[index].flags = 1;
    }
  }
  for (i = 0x100 - err_report_ctx_block.palette_static_half_count; i < 0x100; i++) {
    if (((logpal->entries[i].red == (char)red) &&
        (logpal->entries[i].green == (char)green)) &&
       (logpal->entries[i].blue == (char)blue)) {
      logpal->entries[index].flags = 1;
    }
  }
  return;
}

/* 005dec4e auto */
HPALETTE __cdecl logpal_create_hpalette(GameLogPalette *logpal)

{
  i32 hpal;
  
  if (display_palette.palettized == 1) {
    hpal = CreatePalette((LOGPALETTE *)logpal);
  }
  else {
    hpal = (HPALETTE)0x0;
  }
  return hpal;
}

/* 005dec8a auto */
void __cdecl logpal_delete_hpalette(HGDIOBJ hpal)

{
  if (display_palette.palettized == 1) {
    DeleteObject(hpal);
  }
  return;
}

/* 005decb1 auto */
void __cdecl logpal_apply_range(GameLogPalette *logpal,HPALETTE hpal,UINT first,UINT count)

{
  if (display_palette.palettized == 1) {
    SetPaletteEntries(hpal,first,count,(PALETTEENTRY *)&logpal->entries[first]);
  }
  return;
}

/* 005ded12 auto */
void __cdecl palette_copy_rgb_range(GameLogPalette *logpal,iptr dest,int first,int count)

{
  int index;
  iptr offset;
  
  offset = 0;
  for (index = first; index < count + first; index++) {
    *(i8 *)(offset + dest) = logpal->entries[index].red;
    *(i8 *)(offset + 1 + dest) = logpal->entries[index].green;
    *(i8 *)(offset + 2 + dest) = logpal->entries[index].blue;
    offset += 3;
  }
  return;
}

/* 005e35b0 auto */
GdiDib * __cdecl dib_create(RECT *rect)

{
  int width;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  struct { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[256]; } bmi;  /* 8-bit, 256 colours */
  int height2;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height2 = rect->bottom - rect->top;
  if ((width < 1) || (height2 < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf);
    }
    else {
      _memset(&bmi,0,0x28);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 8;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      if (0 < bmi.bmiHeader.biHeight) {
        if (dib_rows.bottom_up_logged == 0) {
          dbg_log((const char *)&str_bottom_up_orientation_recommended);
        }
        dib_rows.bottom_up_logged = 1;
      }
      if (dib_rows.order == -1) {
        bmi.bmiHeader.biHeight = height2;
      }
      else if (dib_rows.order == 1) {
        bmi.bmiHeader.biHeight = -height2;
      }
      else {
        bmi.bmiHeader.biHeight = bmi.bmiHeader.biHeight * height2;
      }
      _memset(bmi.bmiColors,0,0x400);
      h = CreateDIBSection(hdc,(BITMAPINFO *)&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth;
        dib2->width = width;
        dib2->height = height2;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e388f auto */
i32 __cdecl dib_destroy(GdiDib *dib)

{
  i32 handle;
  
  if (dib != 0) {
    if (dib->hdc != 0) {
      SelectObject(dib->hdc,dib->old_bitmap);
      DeleteObject(dib->bitmap);
      DeleteDC(dib->hdc);
    }
    if (dib->palette != 0) {
      DeleteObject(dib->palette);
    }
    handle = dib->mem_handle;
    mem_handle_unlock(handle);
    mem_handle_free(handle);
  }
  return 0;
}

/* 005e392a auto */
i32 __cdecl dib_get_stride(GdiDib *dib)

{
  i32 stride;
  
  if (dib == 0) {
    stride = 0;
  }
  else {
    stride = dib->stride;
  }
  return stride;
}

/* 005e395a auto */
uint __cdecl dib_is_top_down(GdiDib *dib)

{
  return (uint)(dib->top_down == 1);
}

/* 005e3988 auto */
void __cdecl dib_flip_vertical(GdiDib *dib)

{
  i32 tmp_handle;
  byte *tmp_row;
  int row;
  byte *bottom_row;
  byte *top_row;
  
  top_row = (byte *)dib_get_bits(dib);
  tmp_handle = mem_handle_alloc(dib->stride);
  tmp_row = (byte *)mem_handle_lock(tmp_handle);
  bottom_row = (dib->height - 1) * dib->stride + top_row;
  for (row = 0; row < dib->height / 2; row++) {
    crt_memmove(tmp_row,bottom_row,dib->stride);
    crt_memmove(bottom_row,top_row,dib->stride);
    crt_memmove(top_row,tmp_row,dib->stride);
    top_row += dib->stride;
    bottom_row -= dib->stride;
  }
  mem_handle_unlock(tmp_handle);
  mem_handle_free(tmp_handle);
  return;
}

/* 005e3a81 auto */
i32 __cdecl dib_get_bits(GdiDib *dib)

{
  i32 bits;
  
  if (dib == 0) {
    bits = 0;
  }
  else {
    bits = dib->bits;
  }
  return bits;
}

/* 005e3aa8 auto */
i32 __cdecl dib_unlock_bits(GdiDib *dib)

{
  return 0;
}

/* 005e3b4c auto */
void __cdecl dib_set_color_table_range(GdiDib *dib,GamePalette *palette,UINT first,UINT count)

{
  RGBQUAD colors[256];
  uptr index;
  
  for (index = first; (int)index < (int)(count + first); index++) {
    palette->get_entry(index,(iptr)&colors[index].rgbRed,(iptr)&colors[index].rgbGreen,
               (iptr)(colors + index));
    colors[index].rgbReserved = '\0';
  }
  SetDIBColorTable(dib->hdc,first,count,colors);
  return;
}

/* 005e3bdc auto */
void __cdecl dib_set_palette(GdiDib *dib,iptr palette)

{
  dib_set_color_table_range(dib,(GamePalette *)palette,0,0x100);
  return;
}

/* 005e3cb4 auto */
void __cdecl
dib_draw_text_at(GdiDib *dib,int font,char *text,int x,int y,iptr clip_rect,
            uint align)

{
  int half_h;
  int half_w;
  size_t cchText;
  tagRECT *lprc;
  UINT format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  UINT old_align;
  tagRECT text_rect;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawstring);
  }
  else {
    font_data = (i32 *)mem_handle_lock(font);
    half_h = font_measure_height(font);
    half_h = y + half_h;
    half_w = font_measure_width(font,(const char *)text);
    SetRect(&text_rect,x,y,x + half_w,half_h);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 CONCAT12(color2.rgbBlue,CONCAT11(color2.rgbGreen,color2.rgbRed)));
    half_h = rect_height(&text_rect);
    half_h = half_h / 2;
    half_w = rect_width(&text_rect);
    half_w = half_w / 2;
    OffsetRect(&text_rect,-half_w,-half_h);
    if ((align & 1) != 0) {
      OffsetRect(&text_rect,half_w,0);
    }
    if ((align & 4) != 0) {
      OffsetRect(&text_rect,0,half_h);
    }
    if ((align & 8) != 0) {
      OffsetRect(&text_rect,0,-half_h);
    }
    if ((align & 2) != 0) {
      OffsetRect(&text_rect,-half_w,0);
    }
    old_align = GetTextAlign(dib->hdc);
    SetTextAlign(dib->hdc,1);
    MoveToEx(dib->hdc,text_rect.left,text_rect.top,(LPPOINT)0x0);
    IntersectRect(&text_rect,&text_rect,(RECT *)clip_rect);
    format = 0;
    lprc = &text_rect;
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,lprc,format);
    SetTextAlign(dib->hdc,old_align);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e3eca auto */
void __cdecl dib_draw_text_rect(GdiDib *dib,int font,char *text,iptr rect,uint align)

{
  size_t cchText;
  uint format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawstring_2);
  }
  else if (dib != 0) {
    font_data = (i32 *)mem_handle_lock(font);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 CONCAT12(color2.rgbBlue,CONCAT11(color2.rgbGreen,color2.rgbRed)));
    format = 0;
    if (align == 0) {
      format = 0x25;
    }
    else {
      if ((align & 8) != 0) {
        format = 8;
      }
      if ((align & 2) != 0) {
        format = format | 2;
      }
    }
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,(LPRECT)rect,format);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e3feb auto */
void __cdecl dib_draw_text_wrap(GdiDib *dib,int font,char *text,LPRECT rect,uint align)

{
  size_t cchText;
  UINT format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawtext);
  }
  else if (dib != 0) {
    font_data = (i32 *)mem_handle_lock(font);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 CONCAT12(color2.rgbBlue,CONCAT11(color2.rgbGreen,color2.rgbRed)));
    format = 0x10;
    if (align == 0) {
      format = 0x11;
    }
    else if ((align & 2) != 0) {
      format = 0x12;
    }
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,rect,format);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e40fb auto */
void __cdecl dib_draw_line(GdiDib *dib,int x1,int y1,int x2,int y2)

{
  RGBQUAD color2;
  HPEN pen;
  HGDIOBJ old_pen;
  
  if (dib != 0) {
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    pen = CreatePen(0,1,CONCAT12(color2.rgbBlue,
                                           CONCAT11(color2.rgbGreen,color2.rgbRed)));
    old_pen = SelectObject(dib->hdc,pen);
    MoveToEx(dib->hdc,x1,y1,(LPPOINT)0x0);
    LineTo(dib->hdc,x2,y2);
    SelectObject(dib->hdc,old_pen);
    DeleteObject(pen);
  }
  return;
}

/* 005e41ba auto */
GdiDib * __cdecl dib_create16(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  struct { BITMAPINFOHEADER bmiHeader; DWORD masks[3]; } bmi;  /* 16-bit BI_BITFIELDS */
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_2);
    }
    else {
      _memset(&bmi,0,0x34);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x10;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biCompression = 3;
      bmi.masks[0] = 0x7c00;
      bmi.masks[1] = 0x3e0;
      bmi.masks[2] = 0x1f;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,(BITMAPINFO *)&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_2);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth * 2;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e43c5 auto */
GdiDib * __cdecl dib_create24(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  BITMAPINFO bmi;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_3);
    }
    else {
      _memset(&bmi,0,0x2c);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x18;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_3);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth * 3;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e45b5 auto */
GdiDib * __cdecl dib_create32(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  BITMAPINFO bmi;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_4);
    }
    else {
      _memset(&bmi,0,0x2c);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x20;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_4);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth << 2;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e4d60 auto */
i32 __cdecl
gif_lzw_decode(iptr src,uint code_base,i32 min_code_size,uint pitch,int width,uint height,
            iptr dest)

{
  int table_handle;
  i32 result;
  iptr table;
  i16 code;
  
  table_handle = mem_handle_alloc(0x6000);
  if (table_handle == 0) {
    dbg_log((const char *)&str_error_cannot_allocate_decompression_tabl);
    result = 0;
  }
  else {
    table = (iptr)mem_handle_lock(table_handle);
    for (code = 0; code < 0x1000; code++) {
      *(i16 *)(table + code * 2) = 0xffff;
      *(ushort *)(table + 0x2000 + code * 2) = (ushort)(byte)code_base + code;
      *(i16 *)(table + 0x4000 + code * 2) = 1;
    }
    gif_lzw_decode_stream((byte *)src,(i8 *)dest,table,width,height,pitch,min_code_size);
    mem_handle_unlock(table_handle);
    mem_handle_free(table_handle);
    result = 1;
  }
  return result;
}

/* 005e4e60 auto */
void __cdecl
dib_fill_rect8(iptr bits,uint color,int x,int y,uint width,int height,int pitch,
            int surf_height)

{
  uint color16;
  uint dwords;
  int abs_pitch;
  uint *dst;
  
  abs_pitch = pitch;
  if (pitch < 1) {
    y = (surf_height - 1) - y;
    abs_pitch = -pitch;
  }
  dst = (uint *)(bits + y * abs_pitch + x);
  color16 = color | color << 8;
  do {
    dwords = width >> 2;
    if (dwords != 0) {
      for (; dwords != 0; dwords--) {
        *dst = color16 | color << 0x10 | color << 0x18;
        dst++;
      }
    }
    if ((width & 2) != 0) {
      *(short *)dst = (short)color16;
      dst = (uint *)((int)dst + 2);
    }
    if ((width & 1) != 0) {
      *(char *)dst = (char)color;
      dst = (uint *)((int)dst + 1);
    }
    dst = (uint *)((int)dst + (pitch - width));
    height--;
  } while (height != 0);
  return;
}

/* 005e4f9b auto */
void __cdecl
gfx_copy_pixels(int src_bits,int dst_bits,int src_x,int src_y,int dst_x,int dst_y,uint width,
            int height,int src_height,int dst_height,int src_pitch,int dst_pitch)

{
  i8 pixel;
  uint count;
  int abs_pitch;
  i32 *src;
  i32 *dst;
  int rows_left;
  
  abs_pitch = src_pitch;
  if (src_pitch < 1) {
    src_y = (src_height - 1) - src_y;
    abs_pitch = -src_pitch;
  }
  src = (i32 *)(src_bits + src_y * abs_pitch + src_x);
  abs_pitch = dst_pitch;
  if (dst_pitch < 1) {
    dst_y = (dst_height - 1) - dst_y;
    abs_pitch = -dst_pitch;
  }
  dst = (i32 *)(dst_bits + dst_y * abs_pitch + dst_x);
  rows_left = height;
  do {
    count = width >> 2;
    if (count != 0) {
      for (; count != 0; count--) {
        *dst = *src;
        src++;
        dst++;
      }
    }
    for (count = width & 3; count != 0; count--) {
      pixel = *(i8 *)src;
      src = (i32 *)((int)src + 1);
      *(i8 *)dst = pixel;
      dst = (i32 *)((int)dst + 1);
    }
    src = (i32 *)((int)src + (src_pitch - width));
    dst = (i32 *)((int)dst + (dst_pitch - width));
    rows_left--;
  } while (rows_left != 0);
  return;
}

/* 005e518e auto */
/* WARNING: Unable to use type for symbol run_start */

void __cdecl
sprite_blit_rows_keyed(iptr sprite_data,int dst_bits,iptr scale_tbl,i32 key,int dst_x,int dst_y,
            int pitch,int surf_height,int width,int height,int skip_x,int skip_y,
            i32 trim_x,int src_row,int scaled_trim_x,int scaled_trim_y)

{
  int *col_map;
  int cols_left;
  int run_len;
  iptr offset;
  iptr row_data;
  char *dst_row;
  char *dst;
  int run_start;
  int *row_map;
  
  run_len = pitch;
  if (pitch < 1) {
    dst_y = (surf_height - 1) - dst_y;
    run_len = -pitch;
  }
  dst_row = (char *)(dst_bits + dst_y * run_len + dst_x);
  row_map = (int *)((skip_y + scaled_trim_y) * 4 + scale_tbl);
  row_data = sprite_data + 8;
  do {
    for (; src_row < *row_map; src_row++) {
      row_data = row_data + *(int *)(row_data - 4) + 8;
    }
    run_start = *(int *)(row_data - 8);
    run_len = *(int *)(row_data - 4);
    if (run_len != 0) {
      cols_left = width;
      dst = dst_row;
      for (col_map = (int *)((skip_x + scaled_trim_x) * 4 + scale_tbl); *col_map < run_start;
          col_map = col_map + 1) {
        dst++;
        cols_left--;
      }
      if (0 < cols_left) {
        do {
          offset = *col_map - run_start;
          if (run_len <= offset) break;
          if (*(char *)(offset + row_data) != (char)key) {
            *dst = *(char *)(offset + row_data);
          }
          col_map++;
          dst++;
          cols_left--;
        } while (cols_left != 0);
      }
    }
    dst_row += pitch;
    row_map++;
    height--;
    if (height < 1) {
      return;
    }
  } while( true );
}

/* 005e52bf auto */
/* WARNING: Unable to use type for symbol run_start */

void __cdecl
sprite_blit_rows_solid(iptr sprite_data,int dst_bits,iptr scale_tbl,i32 key,int dst_x,int dst_y,
            int pitch,int surf_height,int width,int height,int skip_x,int skip_y,
            i32 trim_x,int src_row,int scaled_trim_x,int scaled_trim_y,i32 color)

{
  int *col_map;
  int cols_left;
  int run_len;
  iptr offset;
  iptr row_data;
  i8 *dst_row;
  i8 *dst;
  int run_start;
  int *row_map;
  
  run_len = pitch;
  if (pitch < 1) {
    dst_y = (surf_height - 1) - dst_y;
    run_len = -pitch;
  }
  dst_row = (i8 *)(dst_bits + dst_y * run_len + dst_x);
  row_map = (int *)((skip_y + scaled_trim_y) * 4 + scale_tbl);
  row_data = sprite_data + 8;
  do {
    for (; src_row < *row_map; src_row++) {
      row_data = row_data + *(int *)(row_data - 4) + 8;
    }
    run_start = *(int *)(row_data - 8);
    run_len = *(int *)(row_data - 4);
    if (run_len != 0) {
      cols_left = width;
      dst = dst_row;
      for (col_map = (int *)((skip_x + scaled_trim_x) * 4 + scale_tbl); *col_map < run_start;
          col_map = col_map + 1) {
        dst++;
        cols_left--;
      }
      if (0 < cols_left) {
        do {
          offset = *col_map - run_start;
          if (run_len <= offset) break;
          if (*(char *)(offset + row_data) != (char)key) {
            *dst = (i8)color;
          }
          col_map++;
          dst++;
          cols_left--;
        } while (cols_left != 0);
      }
    }
    dst_row += pitch;
    row_map++;
    height--;
    if (height < 1) {
      return;
    }
  } while( true );
}

/* 005e53f3 auto */
void __cdecl
gif_lzw_decode_stream(byte *src,i8 *dest,iptr prefix_tbl,int pitch,uint width,uint height,
            i32 min_code_size)

{
  byte *next_src;
  ushort prefix;
  ushort clear_code;
  ushort init_bits;
  ushort init_mask;
  ushort code;
  i8 *dst_end;
  uptr n;
  iptr len;
  uptr slot;
  iptr suffix_tbl;
  iptr length_tbl;
  i8 *from;
  i8 *row_next;
  i8 *to;
  byte *src_ptr;
  i8 *out_ptr;
  ushort next_code;
  ushort code_bits;
  ushort cur_code;
  uint bit_buf;
  ushort row_pos;
  i8 *dst;
  ushort code_mask;
  ushort bit_count;
  ushort prev_code;
  byte *block_end;
  
  suffix_tbl = prefix_tbl + 0x2000;
  length_tbl = prefix_tbl + 0x4000;
  row_pos = 0;
  src_ptr = src;
  block_end = src;
  dst = dest;
  dst_end = dest + (width & 0xffff) * (height & 0xffff) + 1;
  clear_code = 2 << ((byte)min_code_size & 0x1f);
  next_code = clear_code + 4;
  init_bits = (short)min_code_size + 1;
  init_mask = (2 << ((byte)init_bits & 0x1f)) - 2;
  bit_buf = 0;
  bit_count = 0;
  code_bits = init_bits;
  code_mask = init_mask;
  prev_code = clear_code;
  do {
    while( true ) {
      if (((short)pitch <= (short)row_pos) &&
         (row_pos = row_pos - (short)pitch, (ushort)((short)height - (short)pitch) != 0)) {
        row_next = dst + (ushort)((short)height - (short)pitch);
        to = row_next;
        from = dst;
        for (n = (uptr)row_pos; dst = row_next, n != 0; n--) {
          from--;
          to--;
          *to = *from;
        }
      }
      next_src = src_ptr;
      for (; src_ptr = next_src, bit_count < code_bits; bit_count += 8) {
        if (block_end == next_src) {
          src_ptr = next_src + 1;
          block_end = next_src + *next_src + 1;
        }
        next_src = src_ptr + 1;
        bit_buf = (uint)*src_ptr << ((byte)bit_count & 0x1f) | bit_buf;
      }
      code = (short)bit_buf << 1 & code_mask;
      if (next_code < code) {
        return;
      }
      bit_buf = bit_buf >> ((byte)code_bits & 0x1f);
      bit_count -= code_bits;
      if (code == (ushort)(clear_code + 2)) {
        return;
      }
      if (code != clear_code) break;
      next_code = clear_code + 4;
      code_bits = init_bits;
      code_mask = init_mask;
      prev_code = clear_code;
    }
    cur_code = code;
    if (prev_code != clear_code) {
      slot = (uptr)next_code;
      *(ushort *)(prefix_tbl + slot) = prev_code;
      *(i16 *)(length_tbl + slot) = *(i16 *)(length_tbl + (uint)prev_code);
      *(short *)(length_tbl + slot) = *(short *)(length_tbl + slot) + 1;
      if (code < next_code) {
        dst += *(ushort *)(length_tbl + (uint)code);
        row_pos += *(ushort *)(length_tbl + (uint)code);
        out_ptr = dst;
        if ((int)dst_end <= (int)dst) {
          return;
        }
        do {
          to = out_ptr - 1;
          *to = *(i8 *)(suffix_tbl + (uint)cur_code);
          prefix = *(ushort *)(prefix_tbl + (uint)cur_code);
          if (prefix == 0xffff) break;
          to = out_ptr - 2;
          *to = *(i8 *)(suffix_tbl + (uint)prefix);
          cur_code = *(ushort *)(prefix_tbl + (uint)prefix);
          out_ptr = to;
        } while (cur_code != 0xffff);
      }
      else {
        len = *(ushort *)(length_tbl + (uint)prev_code) + 1;
        dst += len;
        out_ptr = dst - 1;
        row_pos += (short)len;
        if ((int)dst_end <= (int)dst) {
          return;
        }
        do {
          to = out_ptr - 1;
          *to = *(i8 *)(suffix_tbl + (uint)prev_code);
          prefix = *(ushort *)(prefix_tbl + (uint)prev_code);
          if (prefix == 0xffff) break;
          to = out_ptr - 2;
          *to = *(i8 *)(suffix_tbl + (uint)prefix);
          prev_code = *(ushort *)(prefix_tbl + (uint)prefix);
          out_ptr = to;
        } while (prev_code != 0xffff);
        dst[-1] = *to;
      }
      *(i8 *)(suffix_tbl + (uint)next_code) = *to;
      next_code += 2;
      prev_code = code;
      if ((code_mask < next_code) && (code_bits != 0xc)) {
        code_bits++;
        code_mask = code_mask * 2 + 2;
      }
      continue;
    }
    dst += *(ushort *)(length_tbl + (uint)code);
    row_pos += *(ushort *)(length_tbl + (uint)code);
    out_ptr = dst;
    if ((int)dst_end <= (int)dst) {
      return;
    }
    do {
      out_ptr[-1] = *(i8 *)(suffix_tbl + (uint)cur_code);
      prefix = *(ushort *)(prefix_tbl + (uint)cur_code);
      prev_code = code;
      if (prefix == 0xffff) break;
      out_ptr[-2] = *(i8 *)(suffix_tbl + (uint)prefix);
      cur_code = *(ushort *)(prefix_tbl + (uint)prefix);
      out_ptr -= 2;
    } while (cur_code != 0xffff);
  } while( true );
}

/* 005e5869 auto */
void __cdecl
dib_hline8(iptr bits,i32 color,uint length,int x,int y,int surf_height,
            int pitch)

{
  ushort dwords;
  int count;
  i32 *dst;
  
  if (pitch < 1) {
    pitch = -pitch;
    y = (surf_height - 1) - y;
  }
  dst = (i32 *)(bits + y * pitch + x);
  dwords = (ushort)length >> 2;
  count = CONCAT22((short)(length >> 0x10),dwords);
  if (dwords != 0) {
    for (; count != 0; count--) {
      *dst = CONCAT22(CONCAT11((i8)color,(i8)color),
                         CONCAT11((i8)color,(i8)color));
      dst++;
    }
  }
  if ((length & 2) != 0) {
    *(ushort *)dst = CONCAT11((i8)color,(i8)color);
    dst = (i32 *)((int)dst + 2);
  }
  if ((length & 1) != 0) {
    *(i8 *)dst = (i8)color;
  }
  return;
}

/* 005e58e7 auto */
void __cdecl
dib_vline8(iptr bits,i32 color,int length,iptr x,int y,int surf_height,
            iptr pitch)

{
  iptr abs_pitch;
  i8 *dst;
  
  abs_pitch = pitch;
  if (pitch < 1) {
    abs_pitch = -pitch;
    y = (surf_height - 1) - y;
  }
  dst = (i8 *)(bits + y * abs_pitch + x);
  do {
    *dst = (i8)color;
    dst += pitch;
    length--;
  } while (length != 0);
  return;
}

/* 005e5ee0 auto */
void DDSurface::destruct()

{
  return;
}

/* 005e89d0: written by hand in src/game/fixups.cpp */

/* 005ea578: written by hand in src/game/regargs.cpp */

/* 005ea677: written by hand in src/game/regargs.cpp */

/* 005ed710 auto */
void __cdecl
gdi_blit_transparent(HDC dst_dc,HGDIOBJ bitmap,int dst_x,int dst_y,int width,int height,int src_x
            ,int src_y)

{
  HDC hdc;
  HGDIOBJ h;
  HDC hdc_00;
  HDC hdc_01;
  HBITMAP h_00;
  HGDIOBJ h_01;
  HBITMAP h_02;
  HGDIOBJ h_03;
  
  hdc = CreateCompatibleDC(dst_dc);
  h = SelectObject(hdc,bitmap);
  hdc_00 = CreateCompatibleDC(dst_dc);
  hdc_01 = CreateCompatibleDC(dst_dc);
  h_00 = CreateCompatibleBitmap(dst_dc,width,height);
  h_01 = SelectObject(hdc_01,h_00);
  h_02 = CreateBitmap(width,height,1,1,(void *)0x0);
  h_03 = SelectObject(hdc_00,h_02);
  SetBkColor(hdc_01,0xffffff);
  SetTextColor(hdc_01,0);
  SetBkColor(hdc,0x808000);
  BitBlt(hdc_00,0,0,width,height,hdc,src_x,src_y,0xcc0020);
  BitBlt(hdc_01,0,0,width,height,dst_dc,dst_x,dst_y,0xcc0020);
  BitBlt(hdc_01,0,0,width,height,hdc,src_x,src_y,0x660046);
  BitBlt(hdc_01,0,0,width,height,hdc_00,0,0,0x8800c6);
  BitBlt(hdc_01,0,0,width,height,hdc,src_x,src_y,0x660046);
  BitBlt(dst_dc,dst_x,dst_y,width,height,hdc_01,0,0,0xcc0020);
  SelectObject(hdc_00,h_03);
  DeleteObject(h_02);
  SelectObject(hdc_01,h_01);
  DeleteObject(h_00);
  DeleteDC(hdc_00);
  DeleteDC(hdc_01);
  SelectObject(hdc,h);
  DeleteDC(hdc);
  return;
}
