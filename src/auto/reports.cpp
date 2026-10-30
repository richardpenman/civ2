/* The game, the advisors, the High Council and the reports: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00419ed3 auto */
void __cdecl leaders_fixup_names(void)

{
  int i;
  int j;
  iptr leader;
  
  for (leader = 0; leader < 0x15; leader++) {
    leaders[leader].leader_name =
         leaders[leader].names[((uint)leaders[leader].female)];
    if (leaders[leader].plural < 1) {
      leaders[leader].plural = ~leaders[leader].plural + 1;
    }
    if (leaders[leader].adjective < 1) {
      leaders[leader].adjective = ~leaders[leader].adjective + 1;
    }
    for (i = 0; i < 7; i++) {
      for (j = 0; j < 2; j++) {
        if (leaders[leader].titles[i][j] < 1) {
          *(ushort *)&leaders[leader].titles[i][j] =
               ~*(ushort *)&leaders[leader].titles[i][j] + 1;
        }
      }
    }
  }
  return;
}

/* 0042a380 auto */
void __cdecl credits_font_global_init(void)

{
  credits_font_static_init();
  credits_font_reg_atexit();
  return;
}

/* 0042a39a auto */
void __cdecl credits_font_static_init(void)

{
  ((GameFont *)((i8 *)&credits_font_block.credits_font))->init_styled(0,0x18,1);
  return;
}

/* 0042a3ba auto */
void __cdecl credits_font_reg_atexit(void)

{
  _atexit(credits_font_static_dtor);
  return;
}

/* 0042a3d7 auto */
void __cdecl credits_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&credits_font_block.credits_font))->destroy();
  return;
}

/* 0042a3f1 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl advisor_font_global_init(void)

{
  advisor_font_static_init();
  advisor_font_reg_atexit();
  return;
}

/* 0042a40b auto */
void __cdecl advisor_font_static_init(void)

{
  ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->init(0,0x12);
  return;
}

/* 0042a429 auto */
void __cdecl advisor_font_reg_atexit(void)

{
  _atexit(advisor_font_static_dtor);
  return;
}

/* 0042a446 auto */
void __cdecl advisor_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->destroy();
  return;
}

/* 0042a460 auto */
void __cdecl hall_of_fame_font_global_init(void)

{
  hall_of_fame_font_static_init();
  hall_of_fame_font_reg_atexit();
  return;
}

/* 0042a47a auto */
void __cdecl hall_of_fame_font_static_init(void)

{
  ((GameFont *)((i8 *)&advisor_window_block.hall_of_fame_font))->init_styled(0,0x10,1);
  return;
}

/* 0042a49a auto */
void __cdecl hall_of_fame_font_reg_atexit(void)

{
  _atexit(hall_of_fame_font_static_dtor);
  return;
}

/* 0042a4b7 auto */
void __cdecl hall_of_fame_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&advisor_window_block.hall_of_fame_font))->destroy();
  return;
}

/* 0042a4d1 auto */
void __cdecl advisor_bold_font_global_init(void)

{
  advisor_bold_font_static_init();
  advisor_bold_font_reg_atexit();
  return;
}

/* 0042a4eb auto */
void __cdecl advisor_bold_font_static_init(void)

{
  ((GameFont *)((i8 *)&advisor_bold_font))->init_styled(0,0xe,1);
  return;
}

/* 0042a50b auto */
void __cdecl advisor_bold_font_reg_atexit(void)

{
  _atexit(advisor_bold_font_static_dtor);
  return;
}

/* 0042a528 auto */
void __cdecl advisor_bold_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&advisor_bold_font))->destroy();
  return;
}

/* 0042a5b3 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl advisor_button_font_global_init(void)

{
  advisor_button_font_static_init();
  advisor_button_font_reg_atexit();
  return;
}

/* 0042a5cd auto */
void __cdecl advisor_button_font_static_init(void)

{
  ((GameFont *)((i8 *)&advisor_button_font_block.advisor_button_font))->init(0,0x10);
  return;
}

/* 0042a5eb auto */
void __cdecl advisor_button_font_reg_atexit(void)

{
  _atexit(advisor_button_font_static_dtor);
  return;
}

/* 0042a608 auto */
void __cdecl advisor_button_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&advisor_button_font_block.advisor_button_font))->destroy();
  return;
}

/* 0042a622 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl advisor_window_global_init(void)

{
  advisor_window_static_init();
  advisor_window_reg_atexit();
  return;
}

/* 0042a63c auto */
void __cdecl advisor_window_static_init(void)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->PictureWindow::construct();
  return;
}

/* 0042a656 auto */
void __cdecl advisor_window_reg_atexit(void)

{
  _atexit(advisor_window_static_dtor);
  return;
}

/* 0042a673 auto */
void __cdecl advisor_window_static_dtor(void)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->PictureWindow::destruct();
  return;
}

/* 0042a68d auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl casualty_window_global_init(void)

{
  casualty_window_static_init();
  casualty_window_reg_atexit();
  return;
}

/* 0042a6a7 auto */
void __cdecl casualty_window_static_init(void)

{
  ((PictureWindow *)((i8 *)&casualty.window))->PictureWindow::construct();
  return;
}

/* 0042a6c1 auto */
void __cdecl casualty_window_reg_atexit(void)

{
  _atexit(casualty_window_static_dtor);
  return;
}

/* 0042a6de auto */
void __cdecl casualty_window_static_dtor(void)

{
  ((PictureWindow *)((i8 *)&casualty.window))->PictureWindow::destruct();
  return;
}

/* 0042ac4e auto */
void __cdecl advisor_on_close_clicked(void)

{
  int active_child;
  PictureWindow *window;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    window = (PictureWindow *)0x0;
  }
  else {
    window = (PictureWindow *)(active_child - 0x48);
  }
  if (*(u32 *)((u8 *)window + 0x4a0) == 0) {
    window->close();
  }
  else {
    ((ModalMenuWindow *)&window->window)->end_modal_loop();
  }
  return;
}

/* 0042acb0 auto */
void PictureWindow::advisor_add_close_button()

{
  u32 top_y;
  int rect_h;
  iptr label;
  u8 *parent_wnd2;
  i8 button_rect[16];
  
  rect_set_xywh((iptr)button_rect,2,0,0x254,0x18);
  top_y = client_y;
  rect_h = rect_height((RECT *)button_rect);
  rect_offset((iptr)button_rect,client_x,(top_y - rect_h) + 0x18e);
  if (this == (PictureWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  close_button.create((iptr)parent_wnd2,100,(iptr)button_rect,(const char *)label);
  close_button.set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  close_button.set_default_button();
  close_button.set_cancel_button();
  return;
}

/* 0042d781 auto */
int __cdecl
advisor_draw_city_citizens(iptr city,int x,i32 y,i32 width,int happy_count,int unhappy_count,
            int angry_count)

{
  int era;
  int specialist;
  i8 *clip_rect;
  i8 *surface;
  int draw_x;
  i32 draw_y;
  i8 specialist_rect[16];
  i8 unhappy_rect[16];
  i8 content_rect[16];
  i8 happy_rect[16];
  int sprite_row;
  int spacing;
  int i;
  uint citizen_index;
  
  spacing = ui_calc_item_spacing((int)cities[city].size,0x1c,width,0,0);
  citizen_index = 0;
  (*(uint *)&city_calc.specialist_count) = cities[city].worked_tiles >> 0x1a;
  for (i = 0; i < happy_count; i++) {
    surface = ((i8 *)&advisor_state.window);
    clip_rect = happy_rect;
    draw_x = x;
    draw_y = y;
    era = civ_city_style_era((int)cities[city].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + (citizen_index & 1) * 0x3c))->draw((iptr)clip_rect,
               (iptr)surface,draw_x,draw_y);
    x += spacing;
    citizen_index++;
  }
  for (i = 0; i < (int)((int)cities[city].size - (unhappy_count + (*(uint *)&city_calc.specialist_count) + happy_count));
      i = i + 1) {
    surface = ((i8 *)&advisor_state.window);
    clip_rect = content_rect;
    draw_x = x;
    draw_y = y;
    era = civ_city_style_era((int)cities[city].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + ((citizen_index & 1) * 4 + 8) * 0xf))->draw(
               (iptr)clip_rect,(iptr)surface,draw_x,draw_y);
    x += spacing;
    citizen_index++;
  }
  for (i = 0; i < unhappy_count; i++) {
    if (i < unhappy_count - angry_count) {
      sprite_row = 4;
    }
    else {
      sprite_row = 6;
    }
    surface = ((i8 *)&advisor_state.window);
    clip_rect = unhappy_rect;
    draw_x = x;
    draw_y = y;
    era = civ_city_style_era((int)cities[city].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + ((citizen_index & 1) + sprite_row) * 0x3c))->draw(
               (iptr)clip_rect,(iptr)surface,draw_x,draw_y);
    x += spacing;
    citizen_index++;
  }
  for (i = 0; i < (int)(*(uint *)&city_calc.specialist_count); i++) {
    surface = ((i8 *)&advisor_state.window);
    clip_rect = specialist_rect;
    draw_x = x;
    draw_y = y;
    era = civ_city_style_era((int)cities[city].owner);
    specialist = city_specialist_type(city,i);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + (specialist * 4 + 0x1c) * 0xf))->draw(
               (iptr)clip_rect,(iptr)surface,draw_x,draw_y);
    x += spacing;
  }
  return spacing;
}

/* 0042da1d auto */
/* WARNING: Removing unreachable block (ram,0x0042dfd1) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl attitude_advisor_draw(void)

{
  uint next_row;
  ushort recalc_flag;
  int y;
  int bottom;
  int line_height;
  iptr text;
  int gov_index;
  i32 max_row;
  iptr civ;
  int row_y;
  uint row_index;
  iptr city;
  int city_count;
  i8 scroll_rect[16];
  int rows_per_page;
  
  civ = advisor_state.report_civ;
  recalc_flag = game.transient_flags & TRANSIENT_ATTITUDE_DIRTY;
  game.transient_flags = game.transient_flags & ~TRANSIENT_ATTITUDE_DIRTY;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y = advisor_state.client_y + 2;
  bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x151]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  text_clear();
  gov_index = int_clamp(civs[civ].government - 1,0,4);
  text_append_label(gov_index + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  text_clear();
  text = civ_leader_title(civ);
  text_append((const char *)text);
  text_append_space();
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y = y + line_height + 2;
  advisor_state.row_height = 0x20;
  advisor_state.list_height = bottom - y;
  advisor_state.list_top = y;
  advisor_state.rows_per_page = int_clamp((bottom - y) / 0x20,1,99);
  city_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_count++;
    }
  }
  rows_per_page = advisor_state.rows_per_page;
  advisor_state.page_count = int_clamp((advisor_state.rows_per_page - 1 + city_count) / advisor_state.rows_per_page,1,99);
  max_row = int_clamp(city_count - 1,0,999);
  line_height = int_clamp(advisor_state.first_row,0,max_row);
  row_index = 0;
  advisor_state.first_row = line_height;
  row_y = y;
  for (city = 0; city < game.num_cities; city++) {
    next_row = row_index;
    if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
        (next_row = row_index + 1, line_height <= (int)row_index)) && ((int)row_index < line_height + rows_per_page)) {
      if ((recalc_flag != 0) && (GOV_FUNDAMENTALISM < civs[civ].government)) {
        city_recalc(city,1);
      }
      gfx_draw_city((iptr)&advisor_state.window,city,0,(next_row & 1) * 0x40 + advisor_state.client_x + 2,row_y,0);
      text_set_font((iptr)&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      if ((cities[city].unhappy == '\0') &&
         ((int)cities[city].size - (int)(char)cities[city].happy <=
          (int)cities[city].size >> 1)) {
        text_set_colors(0x29,0x12,0xffffffff,0xffffffff);
      }
      if ((char)cities[city].happy < (char)cities[city].unhappy) {
        text_set_colors(0x6a,0x12,0xffffffff,0xffffffff);
      }
      if (extra_ui(X_CELEBRATING) && extras_attitude_color(city, 0) == 0x72) {
        text_set_colors(0x72,0x12,0xffffffff,0xffffffff);   /* X22: celebrating in yellow */
      }
      gov_index = advisor_state.client_x;
      if (extra_ui(X_CELEBRATING)) {
        /* X22: and the food surplus or shortage after the name */
        int name_end = gfx_draw_text((const char *)&cities[city].name,advisor_state.client_x + 0x8c,row_y + 9);
        extras_draw_food_trend((iptr)&advisor_state.window, city, name_end + 4, row_y + 9);
      }
      else
      gfx_draw_text((const char *)&cities[city].name,advisor_state.client_x + 0x8c,row_y + 9);
      advisor_draw_city_citizens(city,gov_index + 0xf5,row_y + 3,
                         ((advisor_state.client_right - advisor_state.vscroll_width) - 0x20) - (gov_index + 0xf5),
                         (int)(char)cities[city].happy,(int)(char)cities[city].unhappy,0);
      row_y += 0x20;
    }
    row_index = next_row;
  }
  if (advisor_state.controls_created == 0) {
    rect_set((iptr)scroll_rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,y,advisor_state.client_right - 2,
                       bottom);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)scroll_rect,1);
    max_row = int_clamp(city_count - 1,0,999);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,max_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(advisor_state.rows_per_page);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(attitude_advisor_on_scroll);
    ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
    ((WndHandlers *)((i8 *)&advisor_state.wnd_events))->set_lbuttonup_handler(attitude_advisor_on_click);
    advisor_state.controls_created = 1;
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 0042e07f auto */
void __cdecl attitude_advisor_on_scroll(i32 scroll_pos)

{
  advisor_state.first_row = scroll_pos;
  attitude_advisor_draw();
  return;
}

/* 0042e09c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl attitude_advisor_on_click(i32 click_x,int click_y)

{
  int row;
  bool is_match;
  iptr city;
  int row_index;
  
  if ((-1 < click_y - advisor_state.list_top) &&
     (row = (click_y - advisor_state.list_top) / advisor_state.row_height, row < advisor_state.rows_per_page)) {
    row_index = 0;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner == advisor_state.report_civ)) &&
         (is_match = row_index == row + advisor_state.first_row, row_index = row_index + 1, is_match)) {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
        return;
      }
    }
  }
  return;
}

/* 0042e185 auto */
void __cdecl attitude_advisor_show(iptr report_civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(4,4,0,600,400,0,0);
  advisor_state.report_civ = report_civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(attitude_advisor_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  return;
}

/* 0042e1e8 auto */
void __cdecl defense_append_ability_label(i32 label_id)

{
  if (tiles_dll_block.unit_ability_list_nonempty != 0) {
    text_append_comma();
  }
  tiles_dll_block.unit_ability_list_nonempty = 1;
  text_append_label(label_id);
  return;
}

/* Part of defense_minister_draw (0042e220): a unit type's row in the
   statistics view: attack/defense/moves, hit points/firepower, units
   active and in production, and its abilities. */
static void defense_draw_unit_stats(iptr civ,iptr unit_type,int row_x,int col_x,int text_y2)
{
  bool has_hold;

  text_clear();
  text_append_int((int)(char)unit_types[unit_type].attack);
  text_append((const char *)&str_empty_34);
  text_append_int((int)(char)unit_types[unit_type].defense);
  if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
    text_append((const char *)&str_empty_35);
  }
  text_append((const char *)&str_empty_36);
  text_append_int((int)(char)unit_types[unit_type].move /
                     (int)(uint)cosmic.road_move_mult);
  gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  col_x = row_x + 0x123;
  text_clear();
  text_append_int((int)(char)unit_types[unit_type].hit_points / 10);
  text_append((const char *)&str_empty_37);
  text_append_int((int)(char)unit_types[unit_type].firepower);
  gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  col_x = row_x + 0x143;
  if (*(char *)&civs[civ].units_active[unit_type] != '\0') {
    text_set_colors(0x7a,10,0xffffffff,0xffffffff);
    text_clear();
    text_append_int((uint)civs[civ].units_active[unit_type]);
    text_append_space();
    text_append_label(0x15a);
    gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  }
  col_x = row_x + 0x180;
  if (*(char *)&civs[civ].units_in_production[unit_type] != '\0') {
    text_set_colors(0x5e,10,0xffffffff,0xffffffff);
    text_clear();
    text_append_int((uint)civs[civ].units_in_production[unit_type]);
    text_append_space();
    text_append_label(0x15b);
    gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  }
  col_x = row_x + 0x1c1;
  text_clear();
  text_set_colors(0x7a,10,0xffffffff,0xffffffff);
  (*(uint *)&tiles_dll_block.unit_ability_list_nonempty) = 0;
  has_hold = unit_types[unit_type].hold != '\0';
  if (has_hold) {
    text_append_label(0x15c);
    text_append_space();
    text_append_int((int)(char)unit_types[unit_type].hold);
  }
  (*(uint *)&tiles_dll_block.unit_ability_list_nonempty) = (uint)has_hold;
  if ((unit_types[unit_type].flags & UTYPE_ATTACK_AIR) == 0) {
    if (unit_types[unit_type].domain == DOMAIN_AIR) {
      defense_append_ability_label(0x15d);
    }
  }
  else {
    defense_append_ability_label(0x15e);
  }
  if ((unit_types[unit_type].flags & UTYPE_SEE_TWO_SPACES) != 0) {
    defense_append_ability_label(0x15f);
  }
  if ((unit_types[unit_type].flags & UTYPE_IGNORE_ZOC) != 0) {
    defense_append_ability_label(0x160);
  }
  if ((unit_types[unit_type].flags & UTYPE_IGNORE_WALLS) != 0) {
    defense_append_ability_label(0x161);
  }
  if ((unit_types[unit_type].flags & UTYPE_AMPHIBIOUS) != 0) {
    defense_append_ability_label(0x162);
  }
  if ((unit_types[unit_type].flags & UTYPE_SUBMARINE) != 0) {
    defense_append_ability_label(0x163);
  }
  if ((unit_types[unit_type].flags & UTYPE_CARRY_AIR) != 0) {
    defense_append_ability_label(0x164);
  }
  if ((unit_types[unit_type].flags & UTYPE_PARADROP) != 0) {
    defense_append_ability_label(0x165);
  }
  if ((unit_types[unit_type].flags & UTYPE_ALPINE) != 0) {
    defense_append_ability_label(0x166);
  }
  if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
    defense_append_ability_label(0x167);
  }
  if ((unit_types[unit_type].flags & UTYPE_X2_VS_AIR) != 0) {
    defense_append_ability_label(0x168);
  }
  if ((unit_types[unit_type].flags & UTYPE_COAST_BOUND) != 0) {
    defense_append_ability_label(0x169);
  }
  if ((unit_types[unit_type].flags & UTYPE_FUNDAMENTALISM_FREE) != 0) {
    defense_append_ability_label(0x16a);
  }
  if ((unit_types[unit_type].flags & UTYPE_SPOT_SUBMARINES) != 0) {
    defense_append_ability_label(0x16b);
  }
  if ((*(uint *)&tiles_dll_block.unit_ability_list_nonempty) != 0) {
    gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  }
}

/* Part of defense_minister_draw (0042e220): a unit type's row in the
   casualties view: the units lost by the civ and by each civ whose losses
   are known. row_x: the caller's, moved to the losses column. */
static void defense_draw_unit_losses(iptr civ,iptr unit_type,int text_y2,int &row_x)
{
  i32 value;
  int name_y;
  i32 bg_color;
  i32 shadow_color;
  i32 shadow_color2;
  int col_x;
  int civ2;

  row_x += 0x100;
  col_x = row_x;
  if (*(char *)&civs[civ].units_lost[unit_type] != '\0') {
    shadow_color2 = 0xffffffff;
    shadow_color = 0xffffffff;
    bg_color = 0x12;
    value = civ_color_secondary(civ);
    text_set_colors(value,bg_color,shadow_color,shadow_color2);
    text_clear();
    text_append_int((uint)civs[civ].units_lost[unit_type]);
    gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
  }
  col_x += 0x1e;
  for (civ2 = 1; civ2 < 8; civ2++) {
    if ((civ2 != civ) &&
       ((((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
         (name_y = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), name_y != 0)) ||
        ((name_y = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), name_y != 0 ||
         ((name_y = civ_has_wonder(civ,WONDER_MARCO_POLO), name_y != 0 ||
          (name_y = civ_has_wonder(civ,WONDER_UNITED_NATIONS), name_y != 0)))))))) {
      if (*(char *)&civs[civ2].units_lost[unit_type] != '\0') {
        shadow_color2 = 0xffffffff;
        shadow_color = 0xffffffff;
        bg_color = 0x12;
        value = civ_color_secondary(civ2);
        text_set_colors(value,bg_color,shadow_color,shadow_color2);
        text_clear();
        text_append_int((uint)civs[civ2].units_lost[unit_type]);
        gfx_draw_text((const char *)&textbuf.buf,col_x,text_y2);
      }
      col_x += 0x1e;
    }
  }
}

/* 0042e220 auto */
/* WARNING: Removing unreachable block (ram,0x0042ef32) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl defense_minister_draw(void)

{
  uint next_row;
  int y;
  int bottom;
  iptr unit;
  int list_top;
  i32 value;
  int row_x;
  int name_y;
  int name_x;
  iptr civ;
  int row_y;
  uint row_index;
  iptr unit_type;
  char type_listed[64];
  int text_y2;
  int type_count;
  int col_x;
  int civ2;
  int lost_x;
  int line_height;
  i8 frame_rect2[16];
  int rows_per_page;
  
  if (bugfix_ui()) {
    lost_x = -0x8000;  /* B17: no casualty listed yet, no frame (docs/BUGS.md) */
  }
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y = advisor_state.client_y + 2;
  bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  unit = (iptr)label_string(label_table.string_ids[0x14f]);
  text_append((const char *)unit);
  text_append_colon();
  if (advisor_state.report_param == 0) {
    unit = (iptr)label_string(label_table.string_ids[0x158]);
    text_append((const char *)unit);
  }
  else {
    unit = (iptr)label_string(label_table.string_ids[0x159]);
    text_append((const char *)unit);
  }
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  text_clear();
  list_top = int_clamp(civs[civ].government - 1,0,4);
  text_append_label(list_top + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  unit = (iptr)(civ_name_plural(civ));
  text_append((const char *)unit);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  text_clear();
  unit = civ_leader_title(civ);
  text_append((const char *)unit);
  text_append_space();
  unit = (iptr)(civ_leader_name(civ));
  text_append((const char *)unit);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  list_top = y + 2;
  line_height = 0x18;
  advisor_state.row_height = 0x18;
  advisor_state.list_top = y + 0x1a;
  advisor_state.list_height = bottom - list_top;
  advisor_state.rows_per_page = int_clamp((bottom - list_top) / 0x18,1,99);
  rows_per_page = advisor_state.rows_per_page;
  type_count = 0;
  unit_type = 0;
  do {
    if (0x3d < unit_type) {
      advisor_state.page_count = int_clamp((rows_per_page - 1 + type_count) / rows_per_page,1,99);
      value = int_clamp(type_count - 1,0,999);
      y = int_clamp(advisor_state.first_row,0,value);
      row_index = 0;
      advisor_state.first_row = y;
      row_y = list_top;
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        next_row = row_index;
        if (((type_listed[unit_type] != '\0') && (next_row = row_index + 1, y <= (int)row_index)) &&
           ((int)row_index < y + rows_per_page)) {
          row_x = (next_row & 1) * 0x40 + advisor_state.client_x;
          unit = unit_make_dummy(unit_type,civ);
          gfx_draw_unit((iptr)&advisor_state.window,unit,0,row_x + 2,row_y,0,0);
          text_set_font((iptr)&advisor_bold_font);
          text_set_colors(0x25,0x12,1,1);
          row_x = advisor_state.client_x;
          name_y = row_y + 0x11;
          name_x = advisor_state.client_x + 0x8c;
          text_y2 = name_y;
          unit = (iptr)label_string(unit_types[unit_type].name);
          gfx_draw_text((const char *)unit,name_x,name_y);
          col_x = row_x + 0xec;
          if (advisor_state.report_param == 0) {
            defense_draw_unit_stats(civ,unit_type,row_x,col_x,text_y2);
          }
          else {
            defense_draw_unit_losses(civ,unit_type,text_y2,row_x);
            lost_x = row_x;
          }
          row_y += line_height;
        }
        row_index = next_row;
      }
      if ((advisor_state.report_param != 0) && (!bugfix_ui() || (lost_x != -0x8000))) {
        rect_set((iptr)frame_rect2,lost_x - 8,list_top,lost_x + 0x10,bottom);
        value = civ_color_primary(civ);
        gfx_draw_rect_frame((iptr)&advisor_state.window,(RECT *)frame_rect2,value);
      }
      if (advisor_state.controls_created == 0) {
        rect_set((iptr)frame_rect2,(advisor_state.client_right - advisor_state.vscroll_width) - 2,list_top,advisor_state.client_right - 2
                           ,bottom);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)frame_rect2,1);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(defense_minister_on_scroll);
      }
      value = int_clamp(type_count - 1,0,999);
      ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,value);
      ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
      ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(advisor_state.rows_per_page);
      if (advisor_state.controls_created == 0) {
        ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
        advisor_state.controls_created = 1;
      }
      ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
      return;
    }
    if (advisor_state.report_param == 0) {
      if ((*(char *)&civs[civ].units_active[unit_type] == '\0') &&
         (*(char *)&civs[civ].units_in_production[unit_type] == '\0')) {
        type_listed[unit_type] = '\0';
      }
      else {
        type_listed[unit_type] = 1;
      }
      if (type_listed[unit_type] != '\0') {
        type_count++;
      }
    }
    else {
      type_listed[unit_type] = '\0';
      for (civ2 = 1; civ2 < 8; civ2++) {
        if ((*(char *)&civs[civ2].units_lost[unit_type] != '\0') &&
           (((((civ2 == civ ||
               ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0)) ||
              (y = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), y != 0)) ||
             ((y = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), y != 0 ||
              (y = civ_has_wonder(civ,WONDER_MARCO_POLO), y != 0)))) ||
            (y = civ_has_wonder(civ,WONDER_UNITED_NATIONS), y != 0)))) {
          type_listed[unit_type] = 1;
          type_count++;
          break;
        }
      }
    }
    unit_type++;
  } while( true );
}

/* 0042efe3 auto */
void __cdecl defense_minister_on_scroll(i32 scroll_pos)

{
  advisor_state.first_row = scroll_pos;
  defense_minister_draw();
  return;
}

/* 0042f000 auto */
void __cdecl defense_minister_on_toggle_view(void)

{
  (*(uint *)&advisor_state.report_param) = (uint)((*(uint *)&advisor_state.report_param) == 0);
  if ((*(uint *)&advisor_state.report_param) == 0) {
    ((Control *)((i8 *)&advisor_state.alt_toggle_button))->hide();
    ((Control *)((i8 *)&advisor_state.toggle_button))->show_and_redraw();
  }
  else {
    ((Control *)((i8 *)&advisor_state.toggle_button))->hide();
    ((Control *)((i8 *)&advisor_state.alt_toggle_button))->show_and_redraw();
  }
  defense_minister_draw();
  return;
}

/* 0042f079 auto */
/* WARNING: Removing unreachable block (ram,0x0042f1bc) */
/* WARNING: Removing unreachable block (ram,0x0042f131) */
/* WARNING: Removing unreachable block (ram,0x0042f21a) */

void __cdecl defense_minister_show(iptr report_civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 button_rect[16];
  
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(2,2,0,600,400,0,0);
  advisor_state.report_civ = report_civ;
  advisor_state.report_param = 0;
  rect_set_xywh((iptr)button_rect,2,0,0x129,0x18);
  client_y = advisor_state.client_y;
  rect_h = rect_height((RECT *)button_rect);
  rect_offset((iptr)button_rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
  rect_offset((iptr)button_rect,299,0);
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((i8 *)&advisor_state.close_button))->create((iptr)&advisor_state.wnd,100,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
  ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
  rect_offset((iptr)button_rect,0xfffffed5,0);
  label = (iptr)label_string(label_table.string_ids[0x159]);
  ((Button *)((i8 *)&advisor_state.toggle_button))->create((iptr)&advisor_state.wnd,0x65,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.toggle_button))->set_callback((CtrlNotifyFn)defense_minister_on_toggle_view);
  label = (iptr)label_string(label_table.string_ids[0x158]);
  ((Button *)((i8 *)&advisor_state.alt_toggle_button))->create((iptr)&advisor_state.wnd,0x66,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.alt_toggle_button))->set_callback((CtrlNotifyFn)defense_minister_on_toggle_view);
  ((Control *)((i8 *)&advisor_state.alt_toggle_button))->hide();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(defense_minister_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  return;
}

/* Part of intel_report_draw (0042f293): the heading lines: the leader, its
   attitude towards civ3 and treaty, and its personality (aggressive,
   civilized, expansionist...). */
static void intel_draw_civ_heading(iptr civ3,iptr civ,int line_height,int &y2)
{
  iptr text;
  i32 color;
  i32 bg_color;
  i32 shadow_color;
  i32 shadow_color2;
  iptr attitude_level;
  iptr leader;

  text_clear();
  text = civ_leader_title(civ);
  text_append((const char *)text);
  text_append_space();
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  text_append_space();
  diplo_eval_ai_stance(civ3,civ);
  attitude_level = diplo_attitude_level(diplo_eval.cur_attitude);
  text_append_lparen();
  text_append_string_id(attitude_names[attitude_level]);
  if ((*(byte *)(civ * 4 + (iptr)((u8 *)&civs + 33) + civ3 * 0x594) & 0x20) == 0) {
    if ((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) == 0) {
      if ((*(byte *)&civs[civ3].treaties[civ] & TREATY_PEACE) == 0) {
        if ((*(byte *)&civs[civ3].treaties[civ] & TREATY_CEASEFIRE) != 0) {
          text_append_comma();
          text_append_label(0x8f);
        }
      }
      else {
        text_append_comma();
        text_append_label(0x8e);
      }
    }
    else {
      text_append_comma();
      text_append_label(0x8d);
    }
  }
  else {
    text_append_comma();
    text_append_label(0x90);
  }
  text_append_rparen();
  shadow_color2 = 0xffffffff;
  shadow_color = 0xffffffff;
  bg_color = 0x12;
  color = civ_color_secondary(civ);
  text_set_colors(color,bg_color,shadow_color,shadow_color2);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
  y2 += line_height;
  text_clear();
  leader = (iptr)civs[civ].tribe;
  if (leaders[leader].attack == 1) {
    text_append_label(0x92);
    text_append_space();
  }
  if (leaders[leader].attack == -1) {
    text_append_label(0x93);
    text_append_space();
  }
  if (leaders[leader].civilize == 1) {
    text_append_label(0x96);
    text_append_space();
  }
  if (leaders[leader].civilize == -1) {
    text_append_label(0x97);
    text_append_space();
  }
  if (leaders[leader].expand == 1) {
    text_append_label(0x94);
    text_append_space();
  }
  if (leaders[leader].expand == -1) {
    text_append_label(0x95);
    text_append_space();
  }
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
}

/* Part of intel_report_draw (0042f293): government, capital, treasury,
   military units (easy levels) and research. */
static void intel_draw_civ_facts(iptr civ,int left_x,int right_x,int line_height,int &y2)
{
  int value;
  iptr city;
  iptr city2;
  int text_x2;
  char buf[80];

  text_clear();
  text_append_label(0x98);
  text_append_colon();
  text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,left_x,y2);
  text_clear();
  text_append_string_id(government_names[civs[civ].government]);
  text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
  gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
       (value = city_has_improvement(city,IMP_PALACE), value != 0)) {
      city2 = city;
      break;
    }
  }
  text_clear();
  text_append_label(0x99);
  text_append_colon();
  text_append_city_name(city2);
  value = right_x;
  text_x2 = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf);
  text_x2 = value - text_x2;
  text_clear();
  text_append_label(0x99);
  text_append_colon();
  text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  text_clear();
  text_append_city_name(city2);
  text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
  gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  y2 += line_height;
  text_clear();
  text_append_label(0x9a);
  text_append_colon();
  text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,left_x,y2);
  text_clear();
  text_append_gold(civs[civ].gold);
  text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  if (game.difficulty < DIFF_PRINCE) {
    text_x2 += 6;
    text_clear();
    text_append_label(0x9b);
    text_append_colon();
    text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
    text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
    text_clear();
    text_append_int((int)civs[civ].num_military_units);
    text_append_space();
    text_append_label(0x19);
    text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
    text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  }
  text_clear();
  text_append_label(0xec);
  text_append_colon();
  str_clear((char *)buf);
  if (civs[civ].researching < 0) {
    str_append_label((iptr)buf,0xe);
  }
  else {
    str_append_string_id((iptr)buf,advances[civs[civ].researching].name);
  }
  text_append((const char *)buf);
  value = right_x;
  text_x2 = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf);
  text_x2 = value - text_x2;
  text_clear();
  text_append_label(0xec);
  text_append_colon();
  text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,y2);
  text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
  text_x2 = gfx_draw_text((const char *)buf,text_x2,y2);
}

/* Part of intel_report_draw (0042f293): the civs the civ has contact
   with, and its treaty with each. */
static void intel_draw_contacts(iptr civ,int line_height,int &y2)
{
  iptr text;
  i32 color;
  i32 bg_color;
  i32 shadow_color;
  i32 shadow_color2;
  int civ2;
  char buf[80];

  for (civ2 = 1; civ2 < 8; civ2++) {
    if (((civ2 != civ) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0)) {
      text_clear();
      str_clear((char *)buf);
      if ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 33) + civ2 * 4) & 0x20) == 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
            if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) == 0) {
              str_append_label((iptr)buf,0x9d);
            }
            else {
              str_append_label((iptr)buf,0x8f);
            }
          }
          else {
            text_append_label(0x9c);
            text_append_space();
            str_append_label((iptr)buf,0x8e);
          }
        }
        else {
          str_append_label((iptr)buf,0x8d);
        }
      }
      else {
        text_append_label(0x9c);
        text_append_space();
        str_append_label((iptr)buf,0x90);
      }
      __strupr(buf);
      text_append((const char *)buf);
      text_append_space();
      text_append_label(0x7b);
      text_append_space();
      text = civ_leader_title(civ2);
      text_append((const char *)text);
      text_append_space();
      text = (iptr)(civ_leader_name(civ2));
      text_append((const char *)text);
      text_append_space();
      text_append_label(0x8c);
      text_append_space();
      text = (iptr)(civ_name_plural(civ2));
      text_append((const char *)text);
      shadow_color2 = 0xffffffff;
      shadow_color = 0xffffffff;
      bg_color = 0x12;
      color = civ_color_secondary(civ2);
      text_set_colors(color,bg_color,shadow_color,shadow_color2);
      gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
      y2 += line_height;
    }
  }
}

/* Part of intel_report_draw (0042f293): the civ's advances in
   three columns, one page of them (scrollbar created the first time). */
static void intel_draw_advances(iptr civ,int y2,int list_bottom,int scroll_height,int line_height,
                                           int rows_per_page)
{
  iptr text;
  i32 color;
  int value;
  int has_adv;
  i32 bg_color;
  i32 shadow_color;
  i8 icon_rect[16];
  int item_y;
  iptr first_item;
  int item_x;
  int item_index;
  iptr advance;
  int right_x;
  int left_x;
  int advance_count;
  int text_x2;
  int column;
  int row;
  i8 scroll_rect[16];

  advisor_state.list_left = advisor_state.client_x + 2;
  right_x = advisor_state.client_width + advisor_state.client_x - 2;
  advisor_state.list_width = right_x - advisor_state.list_left;
  advisor_state.column_width = advisor_state.list_width / 3;
  advisor_state.items_per_page = advisor_state.rows_per_page * 3;
  advance_count = 0;
  left_x = advisor_state.list_left;
  for (advance = 0; advance < 100; advance++) {
    value = civ_has_advance(civ,advance);
    if (value != 0) {
      advance_count++;
    }
  }
  advisor_state.page_count = int_clamp((rows_per_page - 1 + advance_count) / rows_per_page,1,99);
  color = int_clamp(advance_count - 1,0,999);
  advisor_state.first_row = int_clamp(advisor_state.first_row,0,color);
  item_index = 0;
  column = 0;
  row = 0;
  item_y = y2;
  first_item = advisor_state.first_row;
  text_set_font((iptr)&advisor_bold_font);
  for (advance = 0; advance < 100; advance++) {
    has_adv = civ_has_advance(civ,advance);
    value = item_index;
    if (((has_adv != 0) && (value = item_index + 1, first_item <= item_index)) &&
       (item_index < advisor_state.items_per_page + first_item)) {
      item_y = line_height * row + y2;
      item_x = advisor_state.column_width * column + left_x;
      item_index = value;
      ((Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[advance].epoch * 0x3c + (char)advances[advance].category * 0xf0))->draw((iptr)icon_rect,(iptr)&advisor_state.window,item_x,item_y);
      text_x2 = item_x + 0x26;
      shadow_color = 1;
      bg_color = 1;
      color = 0x12;
      value = civ_has_advance(human_civ[0],advance);
      text_set_colors((-(uint)(value == 0) & 0xffffffcb) + 0x5e,color,bg_color,shadow_color);
      has_adv = item_y + 2;
      value = text_x2;
      text = (iptr)label_string(advances[advance].name);
      gfx_draw_text((const char *)text,value,has_adv);
      row++;
      value = item_index;
      if (rows_per_page <= row) {
        row = 0;
        column++;
        if (2 < column) break;
      }
    }
    item_index = value;
  }
  if (advisor_state.controls_created == 0) {
    rect_set((iptr)scroll_rect,left_x,list_bottom,right_x,list_bottom + scroll_height);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)scroll_rect,0);
    color = int_clamp((rows_per_page - 1 + advance_count) / rows_per_page - 1,0,999);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,color);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(1);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(intel_report_on_scroll);
    ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
    ((WndHandlers *)((i8 *)&advisor_state.wnd_events))->set_lbuttonup_handler(intel_report_on_click);
    advisor_state.controls_created = 1;
  }
}

/* 0042f293 auto */
/* WARNING: Removing unreachable block (ram,0x004301aa) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl intel_report_draw(void)

{
  iptr text;
  iptr civ3;
  int list_bottom;
  int y2;
  int scroll_height;
  int right_x;
  int left_x;
  iptr civ;
  int line_height;
  int rows_per_page;
  
  civ3 = advisor_state.report_civ;
  civ = advisor_state.report_param;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y2 = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x150]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
  y2 += line_height;
  intel_draw_civ_heading(civ3,civ,line_height,y2);
  y2 = y2 + line_height + 6;
  left_x = advisor_state.client_x + 2;
  right_x = advisor_state.client_width + advisor_state.client_x - 4;
  text_set_font((iptr)&advisor_bold_font);
  line_height = ((GameFont *)((i8 *)&advisor_bold_font))->line_spacing();
  intel_draw_civ_facts(civ,left_x,right_x,line_height,y2);
  y2 = y2 + line_height + 6;
  intel_draw_contacts(civ,line_height,y2);
  y2 += 6;
  scroll_height = GetSystemMetrics(3);
  list_bottom -= (scroll_height + 2);
  line_height = 0x16;
  advisor_state.row_height = 0x16;
  advisor_state.list_top = y2;
  advisor_state.list_height = list_bottom - y2;
  advisor_state.rows_per_page = int_clamp((list_bottom - y2) / 0x16,0,99);
  rows_per_page = advisor_state.rows_per_page;
  if (0 < advisor_state.rows_per_page) {
    intel_draw_advances(civ,y2,list_bottom,scroll_height,line_height,rows_per_page);
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00430267 auto */
void __cdecl intel_report_on_scroll(int scroll_pos)

{
  advisor_state.first_row = advisor_state.rows_per_page * scroll_pos;
  intel_report_draw();
  return;
}

/* 0043028a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl intel_report_on_click(int click_x,int click_y)

{
  iptr civ;
  int row;
  int column;
  int has_adv;
  bool is_match;
  int advance_id;
  int item_index;
  
  civ = advisor_state.report_param;
  if ((((-1 < click_y - advisor_state.list_top) &&
       (row = (click_y - advisor_state.list_top) / advisor_state.row_height, row < advisor_state.rows_per_page)) &&
      (-1 < click_x - 2)) && (column = (click_x - 2) / advisor_state.column_width, column < 3)) {
    column = advisor_state.rows_per_page * column + advisor_state.first_row;
    item_index = 0;
    for (advance_id = 0; advance_id < 100; advance_id++) {
      has_adv = civ_has_advance(civ,advance_id);
      if ((has_adv != 0) && (is_match = item_index == column + row, item_index = item_index + 1, is_match)) {
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance_id);
        ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->end_modal_loop();
        intel_report_reopen = 1;
        return;
      }
    }
  }
  return;
}

/* 0043039d auto */
void __cdecl intel_report_on_cities_clicked(void)

{
  iptr civ_name;
  int has_palace;
  iptr improvement;
  iptr city;
  Dialog dialog2;
  iptr civ;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  civ = advisor_state.report_param;
  civ_name = (iptr)(civ_name_plural(advisor_state.report_param));
  text_set_string_arg(0,(const char *)civ_name);
  dialog2.load_game_text((const char *)&str_intellcity);
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      text_clear();
      text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
      has_palace = city_has_improvement(city,IMP_PALACE);
      if (has_palace != 0) {
        text_append_space();
        text_append_lparen();
        text_append_label(0x99);
        text_append_rparen();
      }
      if (cities[city].building < -0x26) {
        if (cities[city].building < 1) {
          improvement = ~(int)cities[city].building + 1;
        }
        else {
          improvement = (iptr)cities[city].building;
        }
        text_append_space();
        text_append_lparen();
        text_append_label(0xf4);
        text_append_space();
        text_append_string_id(improvements[improvement].name);
        text_append_rparen();
      }
      dialog2.add_option((const char *)&textbuf.buf,city,0);
    }
  }
  ((WndBase *)((i8 *)&advisor_state.wnd))->disable();
  dialog2.show_modal(0);
  ((WndBase *)((i8 *)&advisor_state.wnd))->enable();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->end_modal_loop();
  intel_report_reopen = 1;
  return;
}

/* 0043060b auto */
/* WARNING: Removing unreachable block (ram,0x004306c1) */
/* WARNING: Removing unreachable block (ram,0x0043074c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl intel_report_show(iptr report_civ,iptr target_civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 button_rect[16];
  
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(3,3,1,600,400,0,0);
  advisor_state.report_civ = report_civ;
  advisor_state.report_param = target_civ;
  rect_set_xywh((iptr)button_rect,2,0,0x129,0x18);
  client_y = advisor_state.client_y;
  rect_h = rect_height((RECT *)button_rect);
  rect_offset((iptr)button_rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
  rect_offset((iptr)button_rect,299,0);
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((i8 *)&advisor_state.close_button))->create((iptr)&advisor_state.wnd,100,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
  ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
  rect_offset((iptr)button_rect,0xfffffed5,0);
  label = (iptr)label_string(label_table.string_ids[0xc5]);
  ((Button *)((i8 *)&advisor_state.toggle_button))->create((iptr)&advisor_state.wnd,0x65,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.toggle_button))->set_callback((CtrlNotifyFn)intel_report_on_cities_clicked);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(intel_report_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  do {
    intel_report_reopen = 0;
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
    }
    advisor_modal_start_ticks_i32 = timer_ticks();
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  } while (intel_report_reopen != 0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return;
}

/* 004308ae auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl foreign_minister_show(iptr civ)

{
  NativeWindow *parley_hwnd_obj;
  int value;
  iptr text;
  i32 option_flags;
  iptr option_id;
  iptr cur_level;
  byte attitude_values[12];
  int popup_result;
  int has_embassy_wonder;
  iptr choice;
  int contact_count;
  Dialog dialog2;
  int both_human;
  iptr civ2;
  int sees_all;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if ((game.multiplayer < 3) || (parley_wnd_block.closed != 0)) {
    while( true ) {
      while( true ) {
        value = int_clamp((uint)game.power_rank[civ],1,7);
        text_set_label_arg(0,label_table.string_ids[0xe3 + value]);
        value = int_clamp((uint)civs[civ].reputation,0,7);
        text_set_label_arg(1,label_table.string_ids[0xdc + value]);
        if (game.multiplayer == '\0') {
          dialog2.load_game_section((const char *)&str_reportforeign,0x2000001);
        }
        else {
          dialog2.load_game_section((const char *)&str_reportforeignmulti,0x2000001);
        }
        dialog2.set_option_draw_fn((DialogOptionDrawFn)foreign_minister_draw_civ_city);
        dialog2.set_list_indent(0x4b,0);
        contact_count = 0;
        value = civ_has_wonder(civ,WONDER_UNITED_NATIONS);
        if ((value == 0) && (value = civ_has_wonder(civ,WONDER_MARCO_POLO), value == 0)) {
          has_embassy_wonder = 0;
        }
        else {
          has_embassy_wonder = 1;
        }
        if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (value = civ_has_wonder(civ,WONDER_MARCO_POLO), value == 0))
           && (game.reveal_map == '\0')) {
          sees_all = 0;
        }
        else {
          sees_all = 1;
        }
        for (civ2 = 1; civ2 < 8; civ2++) {
          if ((civ2 != civ) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0)
             ) {
            if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
                ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) ||
               ((mp_options.show_human_starts == 0 && (game.difficulty != DIFF_CHIEFTAIN)))) {
              both_human = 0;
            }
            else {
              both_human = 1;
            }
            if (((sees_all != 0) || (both_human != 0)) ||
               ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0)) {
              contact_count++;
              text_clear();
              text = civ_leader_title(civ2);
              text_append((const char *)text);
              text_append_space();
              text = (iptr)(civ_leader_name(civ2));
              text_append((const char *)text);
              text_append_space();
              text_append_label(0x8c);
              text_append_space();
              text = (iptr)(civ_name_plural(civ2));
              text_append((const char *)text);
              text_append_space();
              diplo_eval_ai_stance(civ,civ2);
              choice = diplo_attitude_level(diplo_eval.cur_attitude);
              text_append_lparen();
              text_append_string_id(attitude_names[choice]);
              if ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 33) + civ2 * 4) & 0x20) == 0) {
                if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
                  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
                    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) != 0) {
                      text_append_comma();
                      text_append_label(0x8f);
                    }
                  }
                  else {
                    text_append_comma();
                    text_append_label(0x8e);
                  }
                }
                else {
                  text_append_comma();
                  text_append_label(0x8d);
                }
              }
              else {
                text_append_comma();
                text_append_label(0x90);
              }
              if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_EMBASSY) == 0) &&
                 (has_embassy_wonder == 0)) {
                text_append_comma();
                text_append_label(0x91);
              }
              else {
                text_append_comma();
                if (extra_ui(X_FOREIGN_GOLD)) {
                  /* X37: the gold with a coin */
                  text_append_int(civs[civ2].gold);
                  text_append(EXTRAS_ICON_S "g");
                }
                else
                text_append_gold(civs[civ2].gold);
                if (game.difficulty < DIFF_PRINCE) {
                  text_append_comma();
                  text_append_int((int)civs[civ2].num_military_units);
                  text_append_space();
                  text_append_label(0x19);
                }
              }
              text_append_rparen();
              dialog2.add_option((const char *)&textbuf.buf,civ2,0);
            }
          }
        }
        if (contact_count == 0) {
          popup_game((const char *)&str_noforeign);
          return;
        }
        civ2 = dialog2.show_modal(0);
        if (((*((char *)&dialog2.style_flags + 1)) & 0x20) != 0) {
          dialog2.result_extra = 2;
        }
        if (dialog2.result_extra == 0) {
          return;
        }
        if (dialog2.result_extra != 1) break;
        if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_EMBASSY) == 0) &&
            (has_embassy_wonder == 0)) && (game.reveal_map == '\0')) {
          popup_game_with_sprite((const char *)&str_nointel,(iptr)&popup_info_icon_sprite,0);
        }
        else {
          intel_report_show(civ,civ2);
        }
      }
      if (dialog2.result_extra == 2) break;
      if (dialog2.result_extra != 3) {
        return;
      }
      text_clear();
      text = civ_leader_title(civ2);
      text_append((const char *)text);
      text_append_space();
      text = (iptr)(civ_leader_name(civ2));
      text_append((const char *)text);
      text_append_space();
      text_append_label(0x8c);
      text_append_space();
      text = (iptr)(civ_name_plural(civ2));
      text_append((const char *)text);
      text_set_string_arg(0,(const char *)&textbuf.buf);
      choice = dialog2.load_text((const char *)&str_game_2,(const char *)&str_attitudemulti,1);
      if (choice == 0) {
        dialog2.set_columns(3);
        for (choice = 0; choice < 9; choice++) {
          option_flags = 0;
          option_id = choice;
          text = (iptr)label_string(attitude_names[choice]);
          dialog2.add_option((const char *)text,option_id,option_flags);
        }
        if (*(char *)&civs[civ].attitude[civ2] == '\0') {
          cur_level = 0;
        }
        else {
          cur_level = diplo_attitude_level((uint)civs[civ].attitude[civ2]);
        }
        dialog2.set_default_choice(cur_level);
        choice = dialog2.show_modal(0);
        if (-1 < choice) {
          attitude_values[0] = 0;
          attitude_values[1] = 5;
          attitude_values[2] = 0x11;
          attitude_values[3] = 0x1f;
          attitude_values[4] = 0x32;
          attitude_values[5] = 0x44;
          attitude_values[6] = 0x52;
          attitude_values[7] = 0x5f;
          attitude_values[8] = 100;
          civs[civ].attitude[civ2] = attitude_values[choice];
          net_send_message(NM_SET_ATTITUDE,0xff,civ,civ2,(uint)attitude_values[choice],0,0,0,0,0);
        }
      }
    }
    if ((scenario_events_loaded == 0) ||
       (value = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_negotiation(civ,civ2)
       , value != 0)) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
          ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
         (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 || (2 < game.multiplayer)))
         ) {
        if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
            ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
           (2 < game.multiplayer)) {
          diplo_set_treaty_flags(civ,civ2,0x401);
          advisor_window_block.mp_meeting_reply = -1;
          diplo_meeting.mp_pending = 1;
          if (bugfix_ui()) {
            net.nm_parley_cancel_count = 0;  /* B6: a cancel left from an earlier audience does not end this one (docs/BUGS.md) */
          }
          net_send_text_args(0x3d,net.net_slots[net.net_civ_slots[civ2]].id,0
                             ,0,civ,0);
          text = (iptr)(civ_leader_name(civ2));
          text_set_string_arg(1,(const char *)text);
          text = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(2,(const char *)text);
          dlg_style.pending_idle_callback = mp_meeting_wait_idle;
          civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
          popup_result = popup_game_sprite_flags((const char *)&str_parleywaiting,0x2000001,(iptr)&popup_info_icon_sprite,0);
          if (net.nm_parley_cancel_count == 0) {
            if (popup_result == -1) {
              net_send_message(NM_PARLEYREQUEST_CANCEL,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0,0,0,0,0,
                                 0);
              parley_wnd_block.other_civ = -1;
              diplo_meeting.mp_pending = 0;
            }
            else if (advisor_window_block.mp_meeting_reply < 1) {
              dlg_style.pending_idle_callback = turn_popup_timeout_idle;
              popup_game_with_sprite((const char *)&str_parleygoaway,(iptr)&popup_info_icon_sprite,0);
              diplo_meeting.mp_pending = 0;
            }
            else if (advisor_window_block.mp_meeting_reply == 1) {
              advisor_window_block.mp_meeting_reply = -1;
              parley_wnd_block.other_civ = civ2;
              dlg_style.pending_idle_callback = mp_meeting_wait_idle;
              civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
              text = (iptr)(civ_name_plural(civ));
              text_set_string_arg(0,(const char *)text);
              popup_game_with_sprite((const char *)&str_parleyok,(iptr)&popup_info_icon_sprite,0);
              if (net.host_handoff_stage == '\0') {
                if (net.nm_parley_cancel_count == 0) {
                  parley_wnd_block.open(civ2,3);
                }
                else {
                  parley_wnd_block.other_civ = -1;
                  net.nm_parley_cancel_count = 0;
                  text = (iptr)(civ_name_plural(civ2));
                  text_set_string_arg(0,(const char *)text);
                  dlg_style.pending_idle_callback = turn_popup_timeout_idle;
                  popup_game_with_sprite((const char *)&str_parleycancel_2,(iptr)&popup_info_icon_sprite,0);
                  diplo_meeting.mp_pending = 0;
                }
              }
              else {
                parley_wnd_block.other_civ = -1;
                dlg_style.pending_idle_callback = turn_popup_timeout_idle;
                popup_game_with_sprite((const char *)&str_parleybusy,(iptr)&popup_info_icon_sprite,0);
                diplo_meeting.mp_pending = 0;
              }
            }
            else {
              dlg_style.pending_idle_callback = turn_popup_timeout_idle;
              popup_game_with_sprite((const char *)&str_parleybusy_2,(iptr)&popup_info_icon_sprite,0);
              diplo_meeting.mp_pending = 0;
            }
          }
          else {
            net.nm_parley_cancel_count = 0;
            text = (iptr)(civ_name_plural(civ2));
            text_set_string_arg(0,(const char *)text);
            dlg_style.pending_idle_callback = turn_popup_timeout_idle;
            popup_game_with_sprite((const char *)&str_parleycancel,(iptr)&popup_info_icon_sprite,0);
            parley_wnd_block.other_civ = -1;
            diplo_meeting.mp_pending = 0;
          }
        }
      }
      else {
        diplo_set_treaty_flags(civ,civ2,0x401);
        diplo_ai_meet_player(civ,civ2,0xffffffff,0xffffffff,1);
      }
    }
    else {
      popup_game((const char *)&str_peacenobetray);
    }
  }
  else {
    parley_hwnd_obj = ((WndBase *)((i8 *)&parley_wnd_block.window))->get_window();
    BringWindowToTop(parley_hwnd_obj->hwnd);
  }
  return;
}

/* 00431d22 auto */
void __cdecl powergraph_show(void)

{
  int next_row;
  DibSurface *self;
  GamePalette *this_00;
  i32 title;
  int text_y;
  int text_x;
  iptr civ_name;
  i32 wnd_style;
  i32 wnd_x;
  i32 wnd_y;
  i32 wnd_width;
  i32 wnd_height;
  i32 wnd_arg6;
  i32 wnd_arg7;
  i32 wnd_arg8;
  GamePalette *palette;
  DibSurface *surface;
  iptr civ;
  int legend_row;
  GameFont legend_font;
  u32 origin_x2;
  int prev_y;
  int prev_x;
  int grid_x;
  i8 graph_wnd[736];
  Dialog dialog2;
  i32 year2;
  i32 civ_color;
  int point_y;
  int point_x;
  int sample;
  u32 origin_y2;
  int grid_step;
  iptr sample_count;
  struct { u32 left; int top; i32 right; i32 bottom; } rect2;
  uint max_power;
  DibSurface *bg_surface2;
  GamePalette *bg_palette;
  int seh_state;
  
  seh_state = 0xffffffff;
  ((FrameWindow *)graph_wnd)->FrameWindow::construct();
  seh_state = 0;
  ((GameFont *)(graph_wnd + 0x2d8))->init_empty();
  (*((char *)&seh_state + 0)) = 1;
  legend_font.init_empty();
  (*((char *)&seh_state + 0)) = 2;
  dialog2.Dialog::construct(0x4000);
  (*((char *)&seh_state + 0)) = 3;
  bg_surface2 = (DibSurface *)0x0;
  bg_palette = (GamePalette *)0x0;
  ((GameFont *)(graph_wnd + 0x2d8))->set_styled(0,0xc,1);
  legend_font.set_styled(0,0xe,1);
  self = (DibSurface *)operator_new(0x48);
  (*((char *)&seh_state + 0)) = 4;
  if (self == (DibSurface *)0x0) {
    surface = (DibSurface *)0x0;
  }
  else {
    surface = surface_ctor(self);
  }
  (*((char *)&seh_state + 0)) = 3;
  bg_surface2 = surface;
  this_00 = (GamePalette *)operator_new(0x434);
  (*((char *)&seh_state + 0)) = 5;
  if (this_00 == (GamePalette *)0x0) {
    palette = (GamePalette *)0x0;
  }
  else {
    palette = this_00->init();
  }
  seh_state = (seh_state & 0xffffff00 | (byte)(3));
  bg_palette = palette;
  bg_surface2->load_gif_resource(0x33,10,0xc0,(iptr)palette);
  wnd_arg8 = 0;
  wnd_arg7 = 0;
  wnd_arg6 = 0;
  wnd_height = 0x1a9;
  wnd_width = 600;
  wnd_y = 0x14;
  wnd_x = 0;
  wnd_style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0xd6]);
  ((FrameWindow *)graph_wnd)->FrameWindow::create((const char *)title,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,wnd_arg6,wnd_arg7,wnd_arg8);
  text_set_surface((iptr)graph_wnd);
  text_set_font((iptr)(graph_wnd + 0x2d8));
  origin_x2 = (*(i32 *)((char *)&graph_wnd + 292));
  origin_y2 = (*(i32 *)((char *)&graph_wnd + 296));
  ((FrameWindow *)graph_wnd)->layout();
  ((FrameWindow *)graph_wnd)->FrameWindow::draw_caption();
  gfx_copy_rect_xywh((iptr)bg_surface2,(iptr)graph_wnd,0,0,origin_x2,origin_y2,600,400);
  rect2.left = (*(i32 *)((char *)&graph_wnd + 700));
  ((u8 *)&rect2.right)[0] = graph_wnd[0x2c4];
  ((u8 *)&rect2.right)[1] = graph_wnd[0x2c5];
  ((u8 *)&rect2.right)[2] = graph_wnd[0x2c6];
  ((u8 *)&rect2.right)[3] = graph_wnd[0x2c7];
  ((u8 *)&rect2.bottom)[0] = graph_wnd[0x2c8];
  ((u8 *)&rect2.bottom)[1] = graph_wnd[0x2c9];
  ((u8 *)&rect2.bottom)[2] = graph_wnd[0x2ca];
  ((u8 *)&rect2.bottom)[3] = graph_wnd[0x2cb];
  rect2.top = (*(i32 *)((char *)&graph_wnd + 704)) + 400;
  gfx_fill_rect((iptr)graph_wnd,(RECT *)&rect2,0x14);
  rect_set_xywh((iptr)&rect2,origin_x2 + 8,origin_y2 + 2,0x248,0x16e);
  gfx_draw_rect_frame((iptr)graph_wnd,(RECT *)&rect2,10);
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
    sample_count = (int)((int)game.turn + ((int)game.turn >> 0x1f & 3U)) >> 2;
  }
  else {
    sample_count = (int)game.turn / 2;
  }
  sample_count = int_clamp(sample_count,0,0x96);
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
    grid_step = 0x32;
  }
  else {
    grid_step = 0x19;
  }
  max_power = 0x32;
  for (civ = 1; civ < 8; civ++) {
    for (sample = 0; sample < sample_count; sample++) {
      if (max_power < (byte)(((i8 *)&power_graph_history_block.power_graph_history))[sample * 8 + civ]) {
        max_power = (uint)(byte)(((i8 *)&power_graph_history_block.power_graph_history))[sample * 8 + civ];
      }
    }
  }
  for (sample = 0; (sample <= game.turn && (sample < 0x259)); sample += grid_step)
  {
    if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
      grid_x = sample;
    }
    else {
      grid_x = sample * 2;
    }
    gfx_draw_line_color((iptr)graph_wnd,grid_x + origin_x2 + 8,origin_y2 + 2,
                       grid_x + origin_x2 + 8,origin_y2 + 0x170,10);
    if (sample % (grid_step * 2) == 0) {
      year2 = game_turn_to_year(sample + 1);
      text_clear();
      text_append_year(year2);
      text_set_colors(0x25,10,2,1);
      gfx_draw_text((const char *)&textbuf.buf,grid_x + origin_x2,origin_y2 + 0x172);
    }
  }
  ((DibSurface *)graph_wnd)->set_clip((RECT *)&rect2);
  for (civ = 1; civ < 8; civ++) {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) ||
       ((1 << ((byte)civ & 0x1f) & (uint)game.civs_at_start) != 0)) {
      prev_x = 0;
      prev_y = 0;
      for (sample = 0; (sample < sample_count && (sample < 0x96)); sample++) {
        point_x = sample * 4;
        point_y = (int)((uint)(byte)(((i8 *)&power_graph_history_block.power_graph_history))[sample * 8 + civ] * 0x168) /
                   (int)max_power;
        gfx_draw_line_color((iptr)graph_wnd,prev_x + origin_x2 + 9,(origin_y2 - prev_y) + 0x171,
                           point_x + origin_x2 + 9,(origin_y2 - point_y) + 0x171,0x12);
        prev_x = point_x;
        prev_y = point_y;
      }
    }
  }
  ((DibSurface *)graph_wnd)->set_clip((RECT *)(iptr)(graph_wnd + 700));
  text_set_font((iptr)&legend_font);
  legend_row = 0;
  for (civ = 1; civ < 8; civ++) {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) ||
       ((1 << ((byte)civ & 0x1f) & (uint)game.civs_at_start) != 0)) {
      civ_color = civ_color_primary(civ);
      text_set_colors(civ_color,10,2,1);
      next_row = legend_row + 1;
      text_y = legend_row * 0xe + origin_y2 + 1;
      text_x = origin_x2 + 10;
      civ_name = (iptr)(civ_name_plural(civ));
      gfx_draw_text((const char *)civ_name,text_x,text_y);
      prev_x = 0;
      prev_y = 0;
      for (sample = 0; (legend_row = next_row, sample < sample_count && (sample < 0x96));
          sample = sample + 1) {
        point_x = sample * 4;
        point_y = (int)((uint)(byte)(((i8 *)&power_graph_history_block.power_graph_history))[sample * 8 + civ] * 0x168) /
                   (int)max_power;
        gfx_draw_line_color((iptr)graph_wnd,prev_x + origin_x2 + 8,(origin_y2 + 0x170) - prev_y,
                           point_x + origin_x2 + 8,(origin_y2 + 0x170) - point_y,civ_color);
        prev_x = point_x;
        prev_y = point_y;
      }
    }
  }
  ((SurfaceWindow *)graph_wnd)->SurfaceWindow::show();
  dialog2.init((iptr)graph_wnd,0,0,0x114000);
  dialog2.wnd_width = (*(i32 *)((char *)&graph_wnd + 300));
  dialog2.wnd_height = (*(i32 *)((char *)&graph_wnd + 304));
  dialog2.origin_x = (*(i32 *)((char *)&graph_wnd + 292));
  dialog2.origin_y = (*(i32 *)((char *)&graph_wnd + 296));
  dialog2.show_modal(0);
  if (bg_surface2 != (DibSurface *)0x0) {
    bg_surface2->surface_delete(1);
  }
  if (bg_palette != (GamePalette *)0x0) {
    bg_palette->scalar_destruct(1);
  }
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 004325c9 */ { dialog2.Dialog::destruct(); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 004325d5 */ { legend_font.destroy(); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 004325e1 */ { ((GameFont *)((u8 *)&graph_wnd + 0x2d8))->destroy(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004325ed */ { ((FrameWindow *)&graph_wnd)->FrameWindow::destruct(); }
  return;
}

/* 00432611 auto */
void __cdecl historian_report_show(void)

{
  int category;
  int has_adv;
  iptr civ_name;
  iptr civ;
  iptr civ2;
  int i;
  iptr city;
  int advance;
  Dialog dialog2;
  int quote_index;
  int rank;
  i8 title_buf[80];
  int best_score;
  int scores[8];
  
  dialog2.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  category = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_historians);
  if (category == 0) {
    text_read_line(1);
    category = _atoi(((i8 *)&textbuf.buf));
    if (category != 0) {
      quote_index = rand_range(0,category - 1);
      for (i = 0; i <= quote_index; i++) {
        text_read_line(1);
      }
      text_set_string_arg(1,(const char *)&textbuf.buf);
      category = rand_range(0,4);
      text_read_section_line((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_histories,category);
      text_set_string_arg(2,(const char *)&textbuf.buf);
      dialog2.load_game_section((const char *)&str_history,0x40000);
      dialog2.add_line((const char *)&str_empty_40,0xffffffff,0xffffffff,0xffffffff,0);
      for (civ = 0; civ < 8; civ++) {
        scores[civ] = 0;
      }
      for (city = 0; city < game.num_cities; city++) {
        if (cities[city].id != 0) {
          if (category == 3) {
            scores[cities[city].owner] =
                 scores[cities[city].owner] +
                 (((int)cities[city].size + (int)(char)cities[city].happy) -
                 (int)(char)cities[city].unhappy);
          }
          else if (category == 4) {
            scores[cities[city].owner] =
                 scores[cities[city].owner] + (int)cities[city].size;
          }
        }
      }
      for (civ = 0; civ < 8; civ++) {
        if (category == 0) {
          scores[civ] += civs[civ].gold;
        }
        else if (category == 1) {
          scores[civ] += (uint)civs[civ].military_strength;
        }
        else if (category == 2) {
          for (advance = 0; advance < 100; advance++) {
            has_adv = civ_has_advance(civ,advance);
            if (has_adv != 0) {
              scores[civ]++;
            }
          }
        }
      }
      category = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_historyrank);
      if (category == 0) {
        for (rank = 1; rank < 8; rank++) {
          text_read_line(1);
          crt_strcpy(title_buf,((i8 *)&textbuf.buf));
          civ2 = -1;
          best_score = 0;
          for (civ = 1; civ < 8; civ++) {
            if ((best_score <= scores[civ]) &&
               ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) {
              best_score = scores[civ];
              civ2 = civ;
            }
          }
          if (civ2 < 0) break;
          scores[civ2] = -1;
          text_clear();
          text_append((const char *)&str_empty_41);
          text_append_int(rank);
          text_append_period();
          text_append_label(0x174);
          text_append_space();
          text_append((const char *)title_buf);
          text_append_space();
          text_append_label(0xd9);
          if (game_language == 2) {
            text_append_colon();
          }
          else {
            text_append_space();
            text_append_label(0x8c);
            text_append_space();
          }
          civ_name = (iptr)(civ_name_plural(civ2));
          text_append((const char *)civ_name);
          if (((human_civ[0] == civ2) ||
              ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_CONTACT) != 0)) ||
             (game.difficulty == DIFF_CHIEFTAIN)) {
            dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
          }
        }
        dialog2.show_modal(0);
      }
    }
  }
  return;
}

/* 00432c1c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl top_cities_draw(void)

{
  iptr text;
  i32 color;
  i32 bg_color;
  i32 shadow_color;
  i32 shadow_color2;
  i8 icon_rect[16];
  iptr top_cities[5];
  int bottom;
  int city_x;
  int box_bottom;
  int y2;
  iptr city;
  int box_top;
  int mid_x;
  int span_width;
  int right_x;
  int citizens_x;
  int wonder_x;
  iptr shift;
  iptr wonder;
  iptr rank;
  int line_height;
  iptr top_scores[5];
  int score;
  i8 box_rect[16];
  
  for (rank = 0; rank < 5; rank++) {
    top_cities[rank] = -1;
    top_scores[rank] = top_cities[rank];
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      score = ((int)cities[city].size + (int)(char)cities[city].happy) -
                 (int)(char)cities[city].unhappy;
      for (wonder = 0; wonder < 0x1c; wonder++) {
        if (game.wonder_city[wonder] == city) {
          score += 10;
        }
      }
      for (rank = 0; rank < 5; rank++) {
        if (score > top_scores[rank]) {
          for (shift = 3; rank <= shift; shift--) {
            top_scores[shift + 1] = top_scores[shift];
            top_cities[shift + 1] = top_cities[shift];
          }
          top_scores[rank] = score;
          top_cities[rank] = city;
          break;
        }
      }
    }
  }
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y2 = advisor_state.client_y + 2;
  bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x155]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
  advisor_state.list_top = y2 + line_height;
  y2 = advisor_state.list_top + 10;
  line_height = 0x18;
  advisor_state.row_height = 0x18;
  advisor_state.list_top += 0x12;
  advisor_state.list_height = bottom - y2;
  for (rank = 0; rank < 5; rank++) {
    city = top_cities[rank];
    if (-1 < top_cities[rank]) {
      box_top = y2;
      text_clear();
      text_append_int(rank + 1);
      text_append_period();
      text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
      text_append_space();
      text_append_lparen();
      text = civ_get_adjective((int)cities[city].owner);
      text_append((const char *)text);
      text_append_rparen();
      text_set_font((iptr)&advisor_font_block.advisor_font);
      shadow_color2 = 1;
      shadow_color = 1;
      bg_color = 0x12;
      color = civ_color_secondary((int)cities[city].owner);
      text_set_colors(color,bg_color,shadow_color,shadow_color2);
      gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
      y2 += line_height;
      city_x = advisor_state.client_x + 5;
      gfx_draw_city((iptr)&advisor_state.window,city,0x1000,city_x,y2 - 0xf,0);
      citizens_x = city_x + 0x42;
      right_x = advisor_state.client_width + advisor_state.client_x - 5;
      span_width = right_x - citizens_x;
      mid_x = (span_width >> 1) + citizens_x;
      advisor_draw_city_citizens(city,citizens_x,y2,(mid_x - 2) - citizens_x,
                         (int)(char)cities[city].happy,(int)(char)cities[city].unhappy,0
                        );
      wonder_x = mid_x;
      for (wonder = 0; wonder < 0x1c; wonder++) {
        if ((game.wonder_city[wonder] == city) && (0x23 < right_x - wonder_x)) {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c))->draw((iptr)icon_rect,(iptr)&advisor_state.window,
                     wonder_x,y2 + 6);
          wonder_x += 0x26;
        }
      }
      box_bottom = y2 + 0x20;
      y2 = box_bottom;
      rect_set((iptr)box_rect,advisor_state.client_x + 3,box_top,
                         advisor_state.client_width + advisor_state.client_x - 3,box_bottom);
      color = civ_color_primary((int)cities[city].owner);
      gfx_draw_rect_frame((iptr)&advisor_state.window,(RECT *)box_rect,color);
      y2 += 6;
    }
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00433122 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl top_cities_show(iptr report_civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(8,8,1,600,400,0,0);
  advisor_state.report_civ = report_civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(top_cities_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return;
}

/* 004331d1 auto */
void __cdecl
demographics_draw_rank(i32 *values,iptr civ,uint y,iptr unit_suffix,int left_x,int right_x)

{
  int has_wonder;
  iptr civ_name;
  i32 color;
  i32 bg_color;
  i32 shadow_color;
  i32 shadow_color2;
  uint best_value;
  int rank;
  int rival;
  iptr civ2;
  
  rank = 1;
  best_value = 0xffffd8f1;
  civ2 = civ;
  if ((int)y < 0) {
    values[civ] = -values[civ];
  }
  for (rival = 1; rival < 8; rival++) {
    if ((1 << ((byte)rival & 0x1f) & (uint)game.civs_alive) != 0) {
      if (((int)y < 0) && (civ != rival)) {
        values[rival] = -values[rival];
      }
      if (values[civ] < values[rival]) {
        rank++;
      }
      if ((int)best_value < values[rival]) {
        best_value = (uint)values[rival];
        civ2 = rival;
      }
    }
  }
  if ((int)y < 1) {
    y = ~y + 1;
  }
  if ((int)best_value < 1) {
    best_value = ~best_value + 1;
  }
  text_clear();
  text_append_label(rank + 0x189);
  gfx_draw_text((const char *)&textbuf.buf,left_x,y);
  if ((civ != civ2) &&
     (((((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
        (has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), has_wonder != 0)) ||
       (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder != 0)) ||
      (((game.reveal_map != '\0' || (has_wonder = civ_has_wonder(civ,WONDER_MARCO_POLO), has_wonder != 0)) ||
       (has_wonder = civ_has_wonder(civ,WONDER_UNITED_NATIONS), has_wonder != 0)))))) {
    text_clear();
    text_append_lparen();
    civ_name = (iptr)(civ_name_plural(civ2));
    text_append((const char *)civ_name);
    text_append((const char *)&str_empty_42);
    text_append_long(best_value);
    text_append((const char *)unit_suffix);
    text_append_rparen();
    shadow_color2 = 1;
    shadow_color = 1;
    bg_color = 0x12;
    color = civ_color_secondary(civ2);
    text_set_colors(color,bg_color,shadow_color,shadow_color2);
    gfx_draw_text_right((const char *)&textbuf.buf,left_x,y,right_x - left_x);
  }
  return;
}

/* Part of demographics_draw (00433434): the statistics of each civ, summed over
   its land tiles and cities (the arrays of the caller, 8 civs each). */
static void demographics_gather(int *life_expect,int *pollution,int *approval,int *food_surplus,
                                           int *land_area,int *income,int *literacy,int *gnp,int *shields2,
                                           int *population)
{
  int row_y;
  iptr city;
  int city_pollution2;
  u8 *tile_owner2;
  uptr civ2;
  int tile;

  for (civ2 = 0; (int)civ2 < 8; civ2++) {
    life_expect[civ2] = 0;
    pollution[civ2] = life_expect[civ2];
    approval[civ2] = pollution[civ2];
    food_surplus[civ2] = 0;
    land_area[civ2] = food_surplus[civ2];
    income[civ2] = land_area[civ2];
    literacy[civ2] = income[civ2];
    gnp[civ2] = 0;
    shields2[civ2] = gnp[civ2];
    population[civ2] = 1;
  }
  tile_owner2 = &map_data.tiles->owner;
  for (tile = 0; tile < map.area; tile++) {
    civ2 = (int)(uint)*tile_owner2 >> 4;
    if (civ2 < 8) {
      land_area[civ2]++;
    }
    tile_owner2 += 6;
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      civ2 = (uptr)cities[city].owner;
      city_recalc(city,1);
      population[civ2] += (int)cities[city].size;
      approval[civ2] =
           approval[civ2] + ((cities[city].size + city_calc.happy_count) - city_calc.unhappy_count);
      income[civ2] = income[civ2] + city_calc.lux_total * 2 + city_calc.tax_total;
      food_surplus[civ2] = food_surplus[civ2] + city_calc.yield_totals[0] + cities[city].size * -2;
      gnp[civ2] = gnp[civ2] + city_calc.total_shields + city_calc.total_trade + city_calc.yield_totals[0];
      shields2[civ2] += city_calc.total_shields;
      if (city_calc.pollution_divisor < 2) {
        city_calc.pollution_divisor = 1;
      }
      row_y = cities[city].size * city_calc.pop_pollution_factor;
      city_pollution2 = city_calc.total_shields / city_calc.pollution_divisor - 0x14 + ((int)(row_y + (row_y >> 0x1f & 3U)) >> 2);
      if (0 < city_pollution2) {
        pollution[civ2] += city_pollution2;
      }
      row_y = city_has_improvement(city,IMP_GRANARY);
      if ((row_y != 0) || (row_y = civ_has_wonder(civ2,WONDER_PYRAMIDS), row_y != 0)) {
        life_expect[civ2] += (int)cities[city].size;
      }
      row_y = city_has_improvement(city,IMP_AQUEDUCT);
      if (row_y != 0) {
        life_expect[civ2] += (int)cities[city].size;
      }
      row_y = city_has_improvement(city,IMP_SEWER_SYSTEM);
      if (row_y != 0) {
        life_expect[civ2] += (int)cities[city].size;
      }
      row_y = city_has_improvement(city,IMP_LIBRARY);
      if (row_y != 0) {
        literacy[civ2] += (int)cities[city].size;
      }
      row_y = city_has_improvement(city,IMP_UNIVERSITY);
      if (row_y != 0) {
        literacy[civ2] += (int)cities[city].size;
      }
    }
  }
}

/* Part of demographics_draw (00433434): the label of a row (label_string
   id) in label_color. */
static void demographics_row_label(i32 label_color,int label_x,int row_y,int id)
{
  iptr label;
  int value;
  int text_y;

  text_set_colors(label_color,0x12,0xffffffff,0xffffffff);
  value = label_x;
  text_y = row_y;
  label = (iptr)label_string(label_table.string_ids[id]);
  gfx_draw_text((const char *)label,value,text_y);
}

/* Part of demographics_draw (00433434): the rule under a row, and the
   next row's y. */
static void demographics_next_row(int line_left,int line_right,int row_height,int &row_y)
{
  gfx_draw_hline((iptr)&advisor_state.window,line_left,line_right,row_y + row_height - 2,99);
  row_y += row_height;
}

/* 00433434 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl demographics_draw(void)

{
  int row_y;
  iptr label;
  int value;
  int text_y;
  iptr civ;
  int pollution[8];
  int land_area[8];
  int bottom;
  int right_x;
  int y2;
  int rank_x;
  int approval[8];
  int value_x;
  int label_x;
  int food_surplus[8];
  int shields2[8];
  int income[8];
  int gnp[8];
  i32 label_color;
  uptr civ2;
  int values[8];
  i8 suffix_buf[80];
  int row_height;
  int population[8];
  int life_expect[8];
  int line_left;
  int literacy[8];
  int line_right;
  
  civ = advisor_state.report_civ;
  demographics_gather(life_expect,pollution,approval,food_surplus,land_area,income,literacy,gnp,shields2,
                      population);
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y2 = advisor_state.client_y + 2;
  bottom = advisor_state.client_y + 0x174;
  row_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  if (game_language == 0) {
    label = civ_get_adjective(civ);
    text_append((const char *)label);
    text_append_space();
  }
  label = (iptr)label_string(label_table.string_ids[0x156]);
  text_append((const char *)label);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y2,advisor_state.client_width);
  row_y = y2 + row_height + 10;
  row_height = 0x19;
  advisor_state.row_height = 0x19;
  advisor_state.list_height = bottom - row_y;
  label_color = 0x25;
  label_x = advisor_state.client_x + 2;
  value_x = advisor_state.client_x + 0xb4;
  rank_x = advisor_state.client_x + 0x142;
  right_x = advisor_state.client_width + label_x - 4;
  line_left = advisor_state.client_x + 2;
  line_right = advisor_state.client_width + advisor_state.client_x - 2;
  advisor_state.list_top = row_y;
  y2 = row_y;
  text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
  value = label_x;
  text_y = row_y;
  label = (iptr)label_string(label_table.string_ids[0x175]);
  gfx_draw_text((const char *)label,value,text_y);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    value = population[civ2];
    if (value < 2) {
      value = 1;
    }
    values[civ2] = (approval[civ2] * 0x32) / value;
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_percent();
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_empty_43,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x176);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    value = civ_total_population(civ2);
    values[civ2] = value;
    if (civ2 == civ) {
      text_clear();
      text_append_population(civ2,0xffffffff);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_0000,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x177);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = income[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_space();
      text_append_label(0x188);
      text_append_space();
      text_append((const char *)&str_empty_44);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_m,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x178);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = shields2[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_space();
      text_append_label(0x182);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  str_clear((char *)suffix_buf);
  str_append_label((iptr)suffix_buf,0x182);
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)suffix_buf,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x179);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = land_area[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append((const char *)&str_000);
      text_append_label(0x183);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_000_2,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17a);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] =
         ((population[civ2] * 2 + literacy[civ2]) * 2) / population[civ2];
    value = civ_has_advance(civ2,ADV_ALPHABET);
    if (value != 0) {
      values[civ2] = values[civ2] << 1;
    }
    value = civ_has_advance(civ2,ADV_WRITING);
    if (value != 0) {
      values[civ2] = values[civ2] << 1;
    }
    value = civ_has_advance(civ2,ADV_LITERACY);
    if (value != 0) {
      values[civ2] = values[civ2] << 1;
    }
    value = civ_has_advance(civ2,ADV_UNIVERSITY);
    if (value != 0) {
      values[civ2] = values[civ2] << 1;
    }
    value = int_clamp(values[civ2],0,100);
    values[civ2] = value;
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_percent();
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_empty_45,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17b);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] =
         (population[civ2] * 0x32) / (life_expect[civ2] + population[civ2]);
    value = civ_has_advance(civ2,ADV_MEDICINE);
    if (value != 0) {
      values[civ2] = values[civ2] / 2;
    }
    value = civ_has_wonder(civ2,WONDER_CURE_FOR_CANCER);
    if (value != 0) {
      values[civ2] = values[civ2] / 2;
    }
    life_expect[civ2] = (int)(0x708 / (longlong)(values[civ2] + 0x14));
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_percent();
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,-row_y,(iptr)&str_empty_46,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17c);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = pollution[civ2];
    life_expect[civ2] =
         life_expect[civ2] - (pollution[civ2] * 10) / population[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append((const char *)&str_0);
      text_append_label(0x184);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  str_clear((char *)suffix_buf);
  str_append((char *)suffix_buf,(const char *)&str_0_2);
  str_append_label((iptr)suffix_buf,0x184);
  demographics_draw_rank((i32 *)values,civ,-row_y,(iptr)suffix_buf,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17d);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    value = int_clamp(life_expect[civ2],0x14,99);
    values[civ2] = value;
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_space();
      text_append_label(0x185);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  str_clear((char *)suffix_buf);
  str_append_label((iptr)suffix_buf,0x186);
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)suffix_buf,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17e);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = (food_surplus[civ2] * 0x28) / population[civ2] + 0x14;
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2] / 10);
      text_append((const char *)&str_empty_47);
      text_append_long(values[civ2] % 10);
      text_append_space();
      text_append_label(0x187);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
    values[civ2] = values[civ2] / 10;
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_empty_48,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x17f);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = (civs[civ2].num_military_units * 10) / population[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append_space();
      text_append_label(0x185);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)suffix_buf,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x180);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    if (income[civ2] <
        (int)(30000 / (ulonglong)(longlong)(civs[civ2].num_advances + 1))) {
      values[civ2] =
           (int)((uint)civs[civ2].num_advances * income[civ2]) / population[civ2];
    }
    else {
      values[civ2] =
           (income[civ2] / population[civ2]) * (uint)civs[civ2].num_advances;
    }
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      text_append((const char *)&str_empty_49);
      text_append_label(0x189);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_empty_50,rank_x,right_x);
  demographics_next_row(line_left,line_right,row_height,row_y);
  demographics_row_label(label_color,label_x,row_y,0x181);
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    values[civ2] = (gnp[civ2] * 10) / population[civ2];
    if (civ2 == civ) {
      text_clear();
      text_append_long(values[civ2]);
      gfx_draw_text((const char *)&textbuf.buf,value_x,row_y);
    }
  }
  demographics_draw_rank((i32 *)values,civ,row_y,(iptr)&str_empty_51,rank_x,right_x);
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00434d8a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl demographics_show(iptr report_civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(9,9,1,600,400,0,0);
  advisor_state.report_civ = report_civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(demographics_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return;
}

/* 00436287 auto */
void __cdecl advisor_refresh_if_open(iptr report_id)

{
  if (report_id == 0xc) {
    if (casualty.report_id == 0xc) {
      ((SurfaceWindow *)((i8 *)&casualty.window))->repaint();
    }
  }
  else if (advisor_state.report_id == report_id) {
    ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  }
  return;
}

/* 004362e2 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl hall_of_fame_draw(void)

{
  int highlight;
  int y;
  int bottom;
  iptr text;
  int font_height;
  uint year2;
  uint abs_year;
  uint abs_year2;
  iptr month_index;
  int month;
  int year_step;
  int entry_y;
  int entry;
  int line;
  i8 line_buf[80];
  int line_height;
  int open_err;
  i8 title_buf[132];
  
  highlight = advisor_state.report_param;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  y = advisor_state.client_y + 1;
  bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  line_height -= 2;
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x1ba]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,y,advisor_state.client_width);
  y += line_height;
  text_set_font((iptr)&advisor_window_block.hall_of_fame_font);
  font_height = ((GameFont *)((i8 *)&advisor_window_block.hall_of_fame_font))->line_spacing();
  advisor_state.row_height = font_height - 3;
  advisor_state.list_top = y + 6;
  advisor_state.list_height = bottom - advisor_state.list_top;
  entry_y = advisor_state.list_top;
  line_height = advisor_state.row_height;
  for (entry = 0; entry < 6; entry++) {
    /* B15: a score over 32767 is negative as a signed 16-bit number: the
       original hides the entry (docs/BUGS.md) */
    if (((entry != 5) || (highlight == 5)) &&
        (bugfix_ui() ? (advisor_window_block.hall_of_fame[entry].score != -1)
                     : (-1 < advisor_window_block.hall_of_fame[entry].score))) {
      if (advisor_window_block.hall_of_fame[entry].leader_female == 0) {
        crt_strcpy(line_buf,str_malefame_2);
      }
      else {
        crt_strcpy(line_buf,str_femalefame_2);
      }
      open_err = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)line_buf);
      text_set_string_arg(0,(const char *)advisor_window_block.hall_of_fame[entry].leader_name);
      for (line = 0; line <= advisor_window_block.hall_of_fame[entry].title_index;
          line = line + 1) {
        if (open_err == 0) {
          text_read_line(1);
          crt_strcpy(line_buf,((i8 *)&textbuf.buf));
        }
        else if (advisor_window_block.hall_of_fame[entry].title_index == line) {
          crt_strcpy(line_buf,str_string0_2);
        }
        else {
          crt_strcpy(line_buf,str_error_3);
        }
        text_expand_vars((iptr)line_buf,(iptr)title_buf);
      }
      text_clear();
      text_append_int(entry + 1);
      text_append((const char *)&str_empty_59);
      text_append((const char *)title_buf);
      text_append_space();
      text_append_label(0x8c);
      text_append_space();
      text_append((const char *)advisor_window_block.hall_of_fame[entry].civ_name);
      text_append_space();
      text_append_lparen();
      text_append_label(0x1bb);
      text_append_space();
      if ((advisor_window_block.hall_of_fame[entry].is_scenario == 0) ||
         (-1 < advisor_window_block.hall_of_fame[entry].year_increment)) {
        year2 = (uint)advisor_window_block.hall_of_fame[entry].year;
        if ((int)year2 < 0) {
          if ((int)year2 < 1) {
            year2 = ~year2 + 1;
          }
          text_append_int(year2);
          text_append_space();
          text_append_label(0);
        }
        else if (game_language == 0) {
          text_append_label(1);
          text_append_space();
          abs_year2 = year2;
          if ((int)year2 < 1) {
            abs_year2 = ~year2 + 1;
          }
          text_append_int(abs_year2);
        }
        else {
          abs_year = year2;
          if ((int)year2 < 1) {
            abs_year = ~year2 + 1;
          }
          text_append_int(abs_year);
          text_append_space();
          text_append_label(1);
        }
      }
      else {
        if (advisor_window_block.hall_of_fame[entry].year_increment < 1) {
          year_step = ~(int)advisor_window_block.hall_of_fame[entry].year_increment + 1;
        }
        else {
          year_step = (int)advisor_window_block.hall_of_fame[entry].year_increment;
        }
        month_index = advisor_window_block.hall_of_fame[entry].turn * year_step +
                    (int)advisor_window_block.hall_of_fame[entry].start_year - 1;
        if (month_index % 0xc < 1) {
          month = ~(month_index % 0xc) + 1;
        }
        else {
          month = month_index % 0xc;
        }
        text_append_label(month + 0x1a4);
        text_append_space();
        if (month_index / 0xc < 1) {
          month_index = ~(month_index / 0xc) + 1;
        }
        else {
          month_index = month_index / 0xc;
        }
        text_append_int(month_index);
      }
      text_append_rparen();
      if (highlight == entry) {
        text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
      }
      else {
        text_set_colors(0xaf,0x12,0xffffffff,0xffffffff);
      }
      gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,entry_y,advisor_state.client_width);
      entry_y += line_height;
      text_clear();
      if ((advisor_window_block.hall_of_fame[entry].is_scenario == 0) ||
         (advisor_window_block.hall_of_fame[entry].objective_score < 0)) {
        text_append_label(0x22);
        text_append_colon();
        str_append_population((iptr)&textbuf.buf,0,(int)advisor_window_block.hall_of_fame[entry].population);
      }
      else {
        text_append_label(0x1b2);
        text_append_colon();
        text_append_int((int)advisor_window_block.hall_of_fame[entry].objective_score);
      }
      text_append_space();
      text_append_space();
      text_append_label(0x1bc);
      text_append_colon();
      if (bugfix_ui()) {
        text_append_int((int)(u16)advisor_window_block.hall_of_fame[entry].score);  /* B15 */
      }
      else {
        text_append_int((int)advisor_window_block.hall_of_fame[entry].score);
      }
      text_append_space();
      text_append_lparen();
      text_append_string_id(difficulty_names[(int)(short)advisor_window_block.hall_of_fame[entry].difficulty & 0xf]);
      if (advisor_window_block.hall_of_fame[entry].is_scenario != 0) {
        text_append_colon();
        text_append_label(0x1a2);
      }
      if ((advisor_window_block.hall_of_fame[entry].difficulty & 0x80) != 0) {
        text_append_colon();
        text_append_label(0x19a);
      }
      text_append_rparen();
      gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,entry_y,advisor_state.client_width);
      entry_y += line_height;
      text_clear();
      text_append((const char *)&str_empty_60);
      text_append_label(0x1bd);
      text_append_colon();
      text_append_int((int)advisor_window_block.hall_of_fame[entry].rating);
      text_append((const char *)&str_empty_61);
      text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
      gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,entry_y,advisor_state.client_width);
      entry_y = entry_y + line_height + 6;
    }
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00436b92 auto */
void __cdecl hall_of_fame_on_clear_clicked(void)

{
  advisor_state.report_civ = 1;
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->end_modal_loop();
  return;
}

/* 00436bb7 auto */
/* WARNING: Removing unreachable block (ram,0x00436c7e) */
/* WARNING: Removing unreachable block (ram,0x00436d09) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

iptr __cdecl hall_of_fame_show(int highlight)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 button_rect[16];
  
  hall_of_fame_load();
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(8,8,1,600,400,0,0);
  advisor_state.report_param = highlight;
  advisor_state.report_civ = 0;
  if (highlight < 0) {
    ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  }
  else {
    rect_set_xywh((iptr)button_rect,2,0,0x129,0x18);
    client_y = advisor_state.client_y;
    rect_h = rect_height((RECT *)button_rect);
    rect_offset((iptr)button_rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
    rect_offset((iptr)button_rect,299,0);
    label = (iptr)label_string(label_table.string_ids[0x147]);
    ((Button *)((i8 *)&advisor_state.close_button))->create((iptr)&advisor_state.wnd,100,(iptr)button_rect,(const char *)label);
    ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
    ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
    ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
    rect_offset((iptr)button_rect,0xfffffed5,0);
    label = (iptr)label_string(label_table.string_ids[0x1be]);
    ((Button *)((i8 *)&advisor_state.toggle_button))->create((iptr)&advisor_state.wnd,0x65,(iptr)button_rect,(const char *)label);
    ((Button *)((i8 *)&advisor_state.toggle_button))->set_callback((CtrlNotifyFn)hall_of_fame_on_clear_clicked);
  }
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(hall_of_fame_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return advisor_state.report_civ;
}

/* 00436dd7 auto */
void __cdecl hall_of_fame_clear(void)

{
  int i;
  
  for (i = 0; i < 6; i++) {
    advisor_window_block.hall_of_fame[i].score = 0xffff;
    advisor_window_block.hall_of_fame[i].rating = 0xffff;
  }
  return;
}

/* 00436e28 auto */
void __cdecl hall_of_fame_load(void)

{
  bool read_failed;
  FILE *_File;
  size_t read_count;
  int i;
  
  hall_of_fame_clear();
  _File = _fopen(str_hallfame_dat,((i8 *)&hall_of_fame_read_mode));
  if (_File != (FILE *)0x0) {
    read_failed = false;
    for (i = 0; i < 6; i++) {
      read_count = _fread((i8 *)&advisor_window_block.hall_of_fame[i],0x48,1,_File);
      if (read_count == 0) {
        read_failed = true;
        break;
      }
    }
    if (read_failed) {
      hall_of_fame_clear();
    }
    _fclose(_File);
  }
  return;
}

/* 00436ed2 auto */
void __cdecl hall_of_fame_save(void)

{
  FILE *_File;
  size_t written;
  int i;
  
  _File = _fopen(str_hallfame_dat_2,((i8 *)&hall_of_fame_write_mode));
  if (_File != (FILE *)0x0) {
    i = 0;
    while ((i < 6 &&
           (written = _fwrite((i8 *)&advisor_window_block.hall_of_fame[i],0x48,1,_File), written != 0))) {
      i++;
    }
    _fclose(_File);
  }
  return;
}

/* 00436f5a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl hall_of_fame_add_entry(iptr civ)

{
  i32 value;
  int score;
  int slot;
  int new_rank;
  int insert_pos;
  
  new_rank = -1;
  score = civ_score.total;
  if (civ_score.total <= civ_score.conquest_bonus) {
    score = civ_score.conquest_bonus;
  }
  if (bugfix_ui() && (0xfffe < score)) {
    score = 0xfffe;  /* B15: the most a 16-bit score holds (0xffff: an empty place) */
  }
  advisor_font_block.hof_new_entry.score = (i16)score;
  advisor_font_block.hof_new_entry.difficulty = (ushort)game.difficulty;
  if ((game.game_flags & GAME_CHEATED) != 0) {
    advisor_font_block.hof_new_entry.difficulty = advisor_font_block.hof_new_entry.difficulty | 0x80;
  }
  advisor_font_block.hof_new_entry.turn = game.turn;
  advisor_font_block.hof_new_entry.year = game.year;
  value = civ_total_population(civ);
  if (extra_rule(X_POP_LIMIT) && (32000 < value)) {
    value = 32000;  /* X51: the Hall of Fame keeps 16 bits */
  }
  advisor_font_block.hof_new_entry.population = (i16)value;
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
    advisor_font_block.hof_new_entry.objective_score = 0xffff;
  }
  else {
    advisor_font_block.hof_new_entry.objective_score = (i16)civ_score.civ_objective_value;
  }
  advisor_font_block.hof_new_entry.year_increment = scenario.year_increment[0];
  advisor_font_block.hof_new_entry.start_year = scenario.start_year;
  advisor_font_block.hof_new_entry.is_scenario = game.game_flags & GAME_SCENARIO;
  advisor_font_block.hof_new_entry.rating = (short)casualty_window_block.civ_rating_percent;
  advisor_font_block.hof_new_entry.leader_female = (ushort)leaders[civs[civ].tribe].female;
  advisor_font_block.hof_new_entry.title_index = (i16)civ_rating_title_font_block.civ_rating_title_index;
  value = (i32)(civ_leader_name(civ));
  crt_strcpy(advisor_font_block.hof_new_entry.leader_name,(char *)value);
  value = (i32)(civ_name_plural(civ));
  crt_strcpy(advisor_font_block.hof_new_entry.civ_name,(char *)value);
  hall_of_fame_load();
  advisor_window_block.hall_of_fame[5].score = 0xffff;
  advisor_window_block.hall_of_fame[5].rating = 0xffff;
  do {
    for (insert_pos = 0; insert_pos <= 5; insert_pos++) {
      if (advisor_window_block.hall_of_fame[insert_pos].rating < advisor_font_block.hof_new_entry.rating) {
        for (slot = 5; insert_pos < slot; slot--) {
          crt_memcpy((i8 *)&advisor_window_block.hall_of_fame[slot],(i8 *)&advisor_window_block.hall_of_fame[slot - 1],0x48);
        }
        crt_memcpy((i8 *)&advisor_window_block.hall_of_fame[insert_pos],(i8 *)&advisor_font_block.hof_new_entry,0x48);
        new_rank = insert_pos;
        break;
      }
    }
    hall_of_fame_save();
    score = hall_of_fame_show(new_rank);
    if (score == 0) {
      return;
    }
    hall_of_fame_clear();
  } while( true );
}

/* 004371b3 auto */
void __cdecl credits_text_global_init(void)

{
  credits_text_static_init();
  return;
}

/* 004371c8 auto */
void __cdecl credits_text_static_init(void)

{
  ((MemBlock *)((i8 *)&advisor_window_block.credits_text_block))->construct();
  return;
}

/* 004371e2 auto */
void __cdecl credits_text_init(uint capacity)

{
  memblock_init((MemBlock *)&advisor_window_block.credits_text_block,2,capacity);
  credits.line_count = 0;
  return;
}

/* 0043720f auto */
void __cdecl credits_text_free(void)

{
  memblock_free((MemBlock *)&advisor_window_block.credits_text_block);
  return;
}

/* 0043722c auto */
int __cdecl credits_add_line(char *line)

{
  int line_index;
  size_t len;
  char *dest;
  
  len = _strlen(line);
  dest = (char *)memblock_alloc((MemBlock *)&advisor_window_block.credits_text_block,len + 1);
  crt_strcpy(dest,line);
  line_index = credits.line_count;
  credits.line_count++;
  return line_index;
}

/* 00437284 auto */
char * __cdecl credits_get_line(int line_index)

{
  char *cursor;
  
  cursor = advisor_window_block.credits_text_data;
  for (; line_index != 0; line_index--) {
    for (; *cursor != '\0'; cursor++) {
    }
    cursor++;
  }
  return cursor;
}

/* 004372cd auto */
i32 __cdecl credits_load_section(i32 section)

{
  int status;
  char *line;
  i32 result;
  
  result = 1;
  switch(section) {
  case 0:
    status = text_open_section((const char *)&str_credits_2,(const char *)&str_credits);
    break;
  case 1:
    status = text_open_section((const char *)&str_mpcredits,(const char *)&str_credits_3);
    break;
  case 2:
    status = text_open_section((const char *)&str_fcredits,(const char *)&str_credits_4);
    break;
  case 3:
    status = text_open_section((const char *)&str_scredits,(const char *)&str_credits_5);
    break;
  default:
    return result;
  }
  if (status == 0) {
    while ((line = text_read_line(1), line != 0 && (textbuf.buf[0] != '@'))) {
      credits_add_line(((i8 *)&textbuf.buf));
    }
    result = 0;
  }
  return result;
}

/* 0043742f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl credits_draw_scroll(int full_redraw)

{
  i32 rect_h;
  i32 rect_w;
  i8 clip_rect[16];
  DibSurface *dead_surface;
  DibSurface *dead_surface2;
  Sprite *new_sprite;
  Sprite *sprite_mem;
  DibSurface *new_surface;
  DibSurface *surface_mem;
  Sprite *old_sprite;
  Sprite *old_sprite2;
  int saved_scroll;
  int y2;
  int bottom;
  int left_x;
  int top;
  int line;
  int text_width;
  int center_x;
  int drawn_right;
  int drawn_left;
  int max_width2;
  char *line_text;
  int free_slot;
  DibSurface *line_surface;
  int slot;
  int line_height;
  RECT rect2;
  int found_slot;
  
  if (credits.partial_redraw_ok == 0) {
    full_redraw = 1;
  }
  if (full_redraw == 0) {
    rect_h = rect_height((RECT *)&advisor_button_font_block.credits_drawn_rect);
    rect_w = rect_width((RECT *)&advisor_button_font_block.credits_drawn_rect);
    ((PictureWindow *)((i8 *)&advisor_state.window))->blit_at(advisor_button_font_block.credits_drawn_rect - advisor_state.client_x,
               advisor_button_font_block.credits_drawn_rect_top - advisor_state.client_y,rect_w,rect_h);
  }
  else {
    ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
    ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  }
  top = advisor_state.client_y;
  bottom = advisor_state.client_y + 0x176;
  line_height = ((GameFont *)((i8 *)&credits_font_block.credits_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&credits_font_block.credits_font);
  text_set_colors(0x25,0x12,2,1);
  advisor_state.row_height = line_height;
  advisor_state.list_top = top;
  advisor_state.list_height = bottom - top;
  left_x = advisor_state.client_x + 2;
  y2 = top;
  saved_scroll = credits.scroll_pos;
  line = credits.scroll_pos / (line_height / 2);
  if (line < credits.line_count) {
    y2 = top + (credits.scroll_pos % (line_height / 2)) * -2;
    for (slot = 0; slot < 0x1e; slot++) {
      if ((-1 < credits_font_block.credits_slot_lines[slot]) &&
         (credits_font_block.credits_slot_lines[slot] < line)) {
        old_sprite = advisor_window_block.credits_slot_sprites[slot];
        old_sprite2 = old_sprite;
        if (old_sprite != (Sprite *)0x0) {
          old_sprite->scalar_destruct(1);
        }
        advisor_window_block.credits_slot_sprites[slot] = (Sprite *)0x0;
        credits_font_block.credits_slot_lines[slot] = 0xffffffff;
      }
    }
    max_width2 = 2;
    for (; (y2 < bottom && (line < credits.line_count)); line++) {
      line_text = credits_get_line(line);
      found_slot = -1;
      free_slot = -1;
      for (slot = 0; slot < 0x1e; slot++) {
        if (credits_font_block.credits_slot_lines[slot] == line) {
          found_slot = slot;
          break;
        }
        if ((credits_font_block.credits_slot_lines[slot] < 0) && (free_slot < 0)) {
          free_slot = slot;
        }
      }
      if (found_slot < 0) {
        if (free_slot < 0) return;
        if (*line_text == '^') {
          line_text++;
          text_clear();
          text_append((const char *)line_text);
          line_text = ((i8 *)&textbuf.buf);
          text_set_colors(0xba,0x12,0xffffffff,0xffffffff);
        }
        else {
          text_set_colors(0xbe,0x12,0xffffffff,0xffffffff);
        }
        text_width = ((GameFont *)((i8 *)&credits_font_block.credits_font))->text_width((const char *)line_text);
        surface_mem = (DibSurface *)operator_new(0x48);
        if (surface_mem == (DibSurface *)0x0) {
          new_surface = (DibSurface *)0x0;
        }
        else {
          new_surface = surface_ctor(surface_mem);
        }
        line_surface = new_surface;
        if (text_width != 0) {
          new_surface->DibSurface::create(text_width,line_height);
          line_surface->set_palette((iptr)&game_palette_block.game_palette);
          text_set_surface((iptr)line_surface);
          line_surface->fill(0xfa);
          gfx_draw_text((const char *)line_text,0,0);
        }
        sprite_mem = (Sprite *)operator_new(0x3c);
        if (sprite_mem == (Sprite *)0x0) {
          new_sprite = 0;
        }
        else {
          new_sprite = sprite_init(sprite_mem);
        }
        advisor_window_block.credits_slot_sprites[free_slot] = new_sprite;
        if (text_width != 0) {
          advisor_window_block.credits_slot_sprites[free_slot]->grab_from_surface((iptr)line_surface,0xfa,0,0,
                     text_width,line_height);
        }
        dead_surface = line_surface;
        dead_surface2 = line_surface;
        if (line_surface != (DibSurface *)0x0) {
          line_surface->surface_delete(1);
        }
        credits_font_block.credits_slot_lines[free_slot] = line;
        advisor_window_block.credits_slot_x[free_slot] =
             ((advisor_state.client_width >> 1) + advisor_state.client_x) - (text_width >> 1);
        casualty_window_block.credits_slot_widths[free_slot] = text_width;
        found_slot = free_slot;
      }
      text_width = casualty_window_block.credits_slot_widths[found_slot];
      if (text_width != 0) {
        advisor_window_block.credits_slot_sprites[found_slot]->draw((iptr)clip_rect,(iptr)&advisor_state.window,
                   advisor_window_block.credits_slot_x[found_slot],y2);
      }
      if (max_width2 <= text_width) {
        max_width2 = text_width;
      }
      y2 += line_height;
    }
    max_width2 += 2;
    center_x = (advisor_state.client_width >> 1) + advisor_state.client_x;
    drawn_left = center_x - (max_width2 >> 1);
    drawn_right = max_width2 + drawn_left;
    rect_set((iptr)&rect2,drawn_left,top,drawn_right,bottom);
    advisor_button_font_block.credits_drawn_rect = rect2.left;
    advisor_button_font_block.credits_drawn_rect_top = rect2.top;
    advisor_button_font_block.credits_drawn_rect_right = rect2.right;
    advisor_button_font_block.credits_drawn_rect_bottom = rect2.bottom;
    if (advisor_state.client_width <= max_width2) {
      full_redraw = 1;
    }
    credits.partial_redraw_ok = (uint)(advisor_state.client_width > max_width2);
    if (full_redraw == 0) {
      ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface_rect((iptr)&rect2);
    }
    else {
      ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
      ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
    }
  }
  else {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->end_modal_loop();
  }
  return;
}

/* 00437a10 auto */
void __cdecl credits_on_redraw(void)

{
  credits_draw_scroll(1);
  return;
}

/* 00437a2a auto */
void __cdecl credits_on_timer(void)

{
  credits.scroll_pos++;
  credits_draw_scroll(0);
  return;
}

/* 00437a4a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl credits_show(int section)

{
  int load_failed;
  int timer_id;
  int slot;
  
  timer_id = -1;
  credits_text_init(0x1000);
  for (slot = 0; slot < 0x1e; slot++) {
    advisor_window_block.credits_slot_sprites[slot] = (Sprite *)0x0;
    credits_font_block.credits_slot_lines[slot] = 0xffffffff;
  }
  load_failed = credits_load_section(section);
  if (load_failed == 0) {
    credits.scroll_pos = 0;
    (*(i32 *)&credits.partial_redraw_ok) = 0;
    advisor_state.report_civ = 0;
    if (section == 3) {
      ((PictureWindow *)((i8 *)&advisor_state.window))->open(10000,8,1,600,400,0,0);
    }
    else {
      ((PictureWindow *)((i8 *)&advisor_state.window))->open(0xffffffa1,8,1,600,400,0,0)
      ;
    }
    ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
    ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(credits_on_redraw);
    music_play_track(3,0);
    ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
    ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
    ui_flush_input();
    timer_id = game_timer_create((TimerFn)credits_on_timer,0x32,0xffffffff);
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
    }
    advisor_modal_start_ticks_i32 = timer_ticks();
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
    ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  }
  for (slot = 0; slot < 0x1e; slot++) {
    if (-1 < credits_font_block.credits_slot_lines[slot]) {
      if (advisor_window_block.credits_slot_sprites[slot] != (Sprite *)0x0) {
        advisor_window_block.credits_slot_sprites[slot]->scalar_destruct(1);
      }
      advisor_window_block.credits_slot_sprites[slot] = (Sprite *)0x0;
      credits_font_block.credits_slot_lines[slot] = 0xffffffff;
    }
  }
  if (-1 < timer_id) {
    game_timer_destroy(timer_id);
  }
  credits_text_free();
  return;
}

/* 00437c6f auto */
void __cdecl intel_report_end_modal(void)

{
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->end_modal_loop();
  return;
}

/* 00437c8a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl advisor_mp_idle(void)

{
  int now_ticks;
  
  net.process_messages(1,0);
  now_ticks = timer_ticks();
  if (0x4b0 < now_ticks - advisor_modal_start_ticks_i32) {
    intel_report_end_modal();
    advisor_modal_start_ticks_i32 = timer_ticks();
  }
  return;
}

/* 00437ccd auto */
void __cdecl casualty_scrollbar_on_scroll(i32 scroll_pos)

{
  casualty.scroll_pos = scroll_pos;
  casualty_timeline_draw();
  return;
}

/* 00437cea auto */
/* WARNING: Removing unreachable block (ram,0x004384ab) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl casualty_timeline_draw(void)

{
  int y;
  int bottom;
  int line_height;
  iptr unit;
  int row_x;
  int text_y;
  int name_x;
  int text_width;
  uptr len;
  uptr len2;
  size_t len3;
  i32 max_row;
  iptr civ;
  int row_y;
  int log_index;
  int entry;
  int entry_count2;
  i8 scroll_rect[16];
  int rows_per_page;
  
  civ = casualty.civ;
  ((FrameWindow *)((i8 *)&casualty.window))->layout();
  ((FrameWindow *)((i8 *)&casualty.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&casualty.window))->blit();
  y = casualty.client_y + 2;
  bottom = casualty.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&casualty.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  unit = (iptr)label_string(label_table.string_ids[0x14f]);
  text_append((const char *)unit);
  text_append_colon();
  unit = (iptr)label_string(label_table.string_ids[0x368]);
  text_append((const char *)unit);
  gfx_draw_text_centered((const char *)&textbuf.buf,casualty.client_x,y,casualty.client_width);
  y += line_height;
  text_clear();
  row_x = int_clamp(civs[civ].government - 1,0,4);
  text_append_label(row_x + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  unit = (iptr)(civ_name_plural(civ));
  text_append((const char *)unit);
  gfx_draw_text_centered((const char *)&textbuf.buf,casualty.client_x,y,casualty.client_width);
  y += line_height;
  text_clear();
  unit = civ_leader_title(civ);
  text_append((const char *)unit);
  text_append_space();
  unit = (iptr)(civ_leader_name(civ));
  text_append((const char *)unit);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,casualty.client_x,y,casualty.client_width);
  y += line_height;
  line_height = y + 2;
  casualty.row_height = 0x18;
  casualty.first_row_y = y + 0x1a;
  casualty.list_height = bottom - line_height;
  casualty.rows_per_page = int_clamp((bottom - line_height) / 0x18,1,99);
  entry_count2 = 0;
  if (net.casualty_log_first[civ] < net.casualty_log_next[civ]) {
    entry_count2 = net.casualty_log_next[civ] - net.casualty_log_first[civ];
  }
  else if (net.casualty_log_first[civ] < net.casualty_log_next[civ]) {
    entry_count2 = 300;
  }
  rows_per_page = casualty.rows_per_page;
  if (entry_count2 != 0) {
    casualty.page_count = int_clamp((casualty.rows_per_page - 1 + entry_count2) / casualty.rows_per_page,1,99);
    max_row = int_clamp(entry_count2 - 1,0,999);
    y = int_clamp(casualty.scroll_pos,0,max_row);
    entry = 1;
    log_index = (net.casualty_log_next[civ] - 1) - y;
    casualty.scroll_pos = y;
    row_y = line_height;
    if (log_index < 0) {
      log_index += 300;
    }
    while (((entry <= net.casualty_counts[civ] && (entry - 1 < y + rows_per_page))
           && ((net.casualty_log_first[civ] != 0 ||
               (log_index < net.casualty_log_next[civ]))))) {
      row_x = (entry + 1U & 1) * 0x40 + casualty.client_x;
      unit = unit_make_dummy((int)net.casualty_log[civ * 0x12c + log_index].unit_type,
                                 civ);
      gfx_draw_unit((iptr)&casualty.window,unit,0,row_x + 2,row_y,0,0);
      text_set_font((iptr)&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      row_x = casualty.client_x;
      text_y = row_y + 0x11;
      name_x = casualty.client_x + 0x8c;
      text_clear();
      unit = (iptr)label_string(unit_types
                                 [net.casualty_log[civ * 0x12c + log_index].unit_type].name
                                );
      text_append((const char *)unit);
      while (text_width = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf),
            0x60 < text_width) {
        len = _strlen(((i8 *)&textbuf.buf));
        (((i8 *)&textbuf.before))[len] = 0;
      }
      gfx_draw_text((const char *)&textbuf.buf,name_x,text_y);
      text_clear();
      text_append_year((int)net.casualty_log[civ * 0x12c + log_index].year);
      while (name_x = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf),
            0x4c < name_x) {
        len2 = _strlen(((i8 *)&textbuf.buf));
        (((i8 *)&textbuf.before))[len2] = 0;
      }
      gfx_draw_text((const char *)&textbuf.buf,row_x + 0xec,text_y);
      text_set_colors(0x5e,10,0xffffffff,0xffffffff);
      text_clear();
      text_append_int((int)net.casualty_log[civ * 0x12c + log_index].x);
      text_append((const char *)&str_empty_62);
      text_append_int((int)net.casualty_log[civ * 0x12c + log_index].y);
      name_x = city_find_nearest((int)net.casualty_log[civ * 0x12c + log_index].x,
                                 (int)net.casualty_log[civ * 0x12c + log_index].y,
                                 -1,0xffffffff,-1);
      if (-1 < name_x) {
        text_append((const char *)&str_empty_63);
        text_append((const char *)(name_x * 0x58 + (iptr)((u8 *)&cities + 32)));
        text_append((const char *)&str_empty_64);
      }
      while (name_x = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf),
            0xaa < name_x) {
        len3 = _strlen(((i8 *)&textbuf.buf));
        (((i8 *)&textbuf.before))[len3] = 0;
      }
      gfx_draw_text((const char *)&textbuf.buf,row_x + 0x138,text_y);
      text_set_colors((int)net.casualty_log[civ * 0x12c + log_index].killer_color,0x12,
                         0xffffffff,0xffffffff);
      text_clear();
      text_append(net.casualty_log[civ * 0x12c + log_index].killer_name);
      while (text_width = casualty.client_right - casualty.vscroll_width,
            name_x = ((GameFont *)((i8 *)&advisor_bold_font))->text_width((const char *)&textbuf.buf),
            (text_width - 2) - (row_x + 0x1e2) < name_x) {
        len3 = _strlen(((i8 *)&textbuf.buf));
        (((i8 *)&textbuf.before))[len3] = 0;
      }
      gfx_draw_text((const char *)&textbuf.buf,row_x + 0x1e2,text_y);
      entry++;
      log_index--;
      if (log_index < 0) {
        log_index = 299;
      }
      row_y += 0x18;
    }
  }
  if (casualty.controls_created == 0) {
    rect_set((iptr)scroll_rect,(casualty.client_right - casualty.vscroll_width) - 2,line_height,casualty.client_right - 2,
                       bottom);
    ((Scrollbar *)((i8 *)&casualty.scrollbar))->create((iptr)&casualty.parent_wnd,200,(iptr)scroll_rect,1);
    ((Scrollbar *)((i8 *)&casualty.scrollbar))->set_callback(casualty_scrollbar_on_scroll);
    ((WndHandlers *)((i8 *)&casualty.wnd_events))->set_dblclick_handler(casualty_timeline_on_dblclick);
  }
  max_row = int_clamp(entry_count2 - 1,0,999);
  ((Scrollbar *)((i8 *)&casualty.scrollbar))->set_range(0,max_row);
  ((Scrollbar *)((i8 *)&casualty.scrollbar))->set_pos(casualty.scroll_pos);
  ((Scrollbar *)((i8 *)&casualty.scrollbar))->set_page_size(casualty.rows_per_page);
  if (casualty.controls_created == 0) {
    ((Control *)((i8 *)&casualty.scrollbar))->show_and_redraw();
    casualty.controls_created = 1;
  }
  ((DibSurface *)((i8 *)&casualty.window))->wnd_blit_surface();
  return;
}

/* 0043856b auto */
/* WARNING: Removing unreachable block (ram,0x00438634) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl casualty_timeline_show(iptr civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 button_rect[16];
  
  if (casualty.report_id < 0) {
    ((PictureWindow *)((i8 *)&casualty.window))->open(2,0xc,0,600,400,0,0);
    casualty.civ = civ;
    casualty.report_param = 0;
    rect_set_xywh((iptr)button_rect,2,0,0x254,0x18);
    client_y = casualty.client_y;
    rect_h = rect_height((RECT *)button_rect);
    rect_offset((iptr)button_rect,casualty.client_x,(client_y - rect_h) + 0x18e);
    label = (iptr)label_string(label_table.string_ids[0x147]);
    ((Button *)((i8 *)&casualty.close_button))->create((iptr)&casualty.parent_wnd,100,(iptr)button_rect,(const char *)label);
    ((Button *)((i8 *)&casualty.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
    ((Button *)((i8 *)&casualty.close_button))->set_default_button();
    ((Button *)((i8 *)&casualty.close_button))->set_cancel_button();
    ((SurfaceWindow *)((i8 *)&casualty.window))->set_redraw_callback(casualty_timeline_draw);
    ((SurfaceWindow *)((i8 *)&casualty.window))->repaint();
    ((SurfaceWindow *)((i8 *)&casualty.window))->SurfaceWindow::show();
  }
  else {
    ((SurfaceWindow *)((i8 *)&casualty.window))->repaint();
    ((SurfaceWindow *)((i8 *)&casualty.window))->SurfaceWindow::show();
  }
  return;
}

/* 004386b8 auto */
void __cdecl casualty_timeline_on_dblclick(i32 click_x,int click_y)

{
  int log_index;
  
  click_y -= (casualty.first_row_y - casualty.row_height / 2);
  if ((-1 < click_y) && (click_y = click_y / casualty.row_height, click_y < casualty.rows_per_page)) {
    log_index = ((net.casualty_log_next[casualty.civ] - 1) - casualty.scroll_pos) - click_y;
    if (log_index < 0) {
      log_index += 300;
    }
    if ((net.casualty_log_next[casualty.civ] != log_index) &&
       (((net.casualty_log_first[casualty.civ] != 0 ||
         (0x12a < net.casualty_counts[casualty.civ])) ||
        (log_index < net.casualty_log_next[casualty.civ])))) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on(
                 (int)net.casualty_log[casualty.civ * 0x12c + log_index].x,
                 (int)net.casualty_log[casualty.civ * 0x12c + log_index].y);
    }
  }
  return;
}

/* 0044ce16 auto */
ThroneRoomWindow *ThroneRoomWindow::construct(iptr civ)

{
  iptr i;
  
  this->ModalMenuWindow::construct();
  ((GamePalette *)&control_list)->init();
  ((DllHandle *)throne_dll)->init();
  ((SurfaceWindow *)throne_surface_wnd)->SurfaceWindow::construct();
  surface_ctor((DibSurface *)throne_backbuffer);
  ((ClickMap *)choice_clickmap)->construct();
  ((Dialog *)choice_dialog_2)->Dialog::construct(0x4000);
  ((GameFont *)((u8 *)&click_prompt_font))->init_empty();
  gfx_get_screen_rect((iptr)(media_wnd));
  throne_civ = civ;
  throneroom_wnd = this;
  throne_complete = 1;
  for (i = 0; i < 8; i++) {
    if (civs[(throne_civ)].throne_room[i] < 4) {
      throne_complete = 0;
    }
  }
  SetRect((LPRECT)((u8 *)&click_prompt_rect),0,0x1c2,0x280,0x1e0);
  ((GameFont *)((u8 *)&click_prompt_font))->set_styled(0,0x12,3);
  return this;
}

/* 0044d027 auto */
void ThroneRoomWindow::destruct()

{
  int try_level;
  
  try_level = try_level & 0xff;
  (*((char *)&try_level + 0)) = 7;
  throneroom_wnd = 0;
  ((DllHandle *)throne_dll)->dll_handle_free();
  (*((char *)&try_level + 0)) = 6;
  /* inlined unwind funclet 0044d0d8 */ { ((GameFont *)(((int)this) + 0x1c34))->destroy(); }
  (*((char *)&try_level + 0)) = 5;
  /* inlined unwind funclet 0044d0e7 */ { ((Dialog *)(((int)this) + 0x1930))->Dialog::destruct(); }
  (*((char *)&try_level + 0)) = 4;
  /* inlined unwind funclet 0044d0f6 */ { ((ClickMap *)(((int)this) + 0x660))->destruct(); }
  (*((char *)&try_level + 0)) = 3;
  /* inlined unwind funclet 0044d105 */ { surface_dtor((DibSurface *)(((int)this) + 0x618)); }
  (*((char *)&try_level + 0)) = 2;
  /* inlined unwind funclet 0044d114 */ { ((SurfaceWindow *)(((int)this) + 0x504))->SurfaceWindow::destruct(); }
  (*((char *)&try_level + 0)) = 1;
  /* inlined unwind funclet 0044d123 */ { ((DllHandle *)(((int)this) + 0x4fc))->free(); }
  try_level = try_level & 0xffffff00;
  /* inlined unwind funclet 0044d132 */ { ((GamePalette *)(((int)this) + 0xb8))->destroy(); }
  try_level = 0xffffffff;
  /* inlined unwind funclet 0044d141 */ { ModalMenuWindow::destruct(); }
  return;
}

/* 0044d162 auto */
uint ThroneRoomWindow::create_windows()

{
  int found;
  i8 path[128];
  
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_pv_dll);
  found = file_resolve_path((const char *)path);
  if (found != 0) {
    ((DllHandle *)throne_dll)->load((const char *)path);
    create_with_handlers((const char *)&str_empty_67,0x800,0,0,throne_width,throne_height + 5,(iptr)&game_palette_block.game_palette
               ,(iptr)&main_frame_wnd_block.main_frame_wnd);
    set_bg_color(0);
    ((DibSurface *)(u8 *)&throne_surface_wnd)->wnd_create_with_surface((const char *)&str_empty_68,0x800,0,0,0x280,0x1e0,(iptr)&control_list,
               (iptr)this);
    ((WndBase *)throne_wnd_base)->center();
    ((WndHandlers *)throne_handlers)->set_lbuttonup_handler(throneroom_on_click);
    ((WndHandlers *)throne_handlers)->set_keydown_handler(throneroom_on_key);
  }
  return (uint)(found != 0);
}

/* 0044d296 auto */
void ThroneRoomWindow::run_upgrade()

{
  int result;
  char *line;
  GameFont font2;
  i8 text_buf[1024];
  tagRECT rect;
  
  result = create_windows();
  if ((result != 0) && (result = render(), result != 0)) {
    ((DibSurface *)throne_backbuffer)->DibSurface::create(0x280,0x1e0);
    SetRect(&rect,0,0,0x280,0x1e0);
    ((DibSurface *)throne_surface_wnd)->DibSurface::blit((DibSurface *)(iptr)(throne_backbuffer),&rect,
               &rect);
    font2.init(1,0x18);
    ON_SCOPE_EXIT(font2.destroy());
    result = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_throne);
    if (result == 0) {
      str_clear((char *)text_buf);
      while (line = text_read_line(1), *line != '@') {
        str_append((char *)text_buf,(const char *)text_parse.line_cursor);
        str_append((char *)text_buf,(const char *)&str_empty_69);
      }
      SetRect(&rect,100,0x78,0x21c,0x1c2);
      gfx_set_text_color_index(0);
      ((DibSurface *)throne_surface_wnd)->draw_text_rect((iptr)&font2,(const char *)text_buf,(iptr)&rect,0);
      OffsetRect(&rect,2,2);
      gfx_set_text_color_index(0xff);
      ((DibSurface *)throne_surface_wnd)->draw_text_rect((iptr)&font2,(const char *)text_buf,(iptr)&rect,0);
      text_close_file();
      SetRect(&rect,0,0,0x280,0x1e0);
    }
    draw_click_prompt();
    choosing_item = 0;
    handle_show_raise();
    wnd_flush_paint();
    set_palette((iptr)&control_list);
    ((SurfaceWindow *)throne_surface_wnd)->SurfaceWindow::show();
    wnd_flush_paint();
    input_flush_events();
    ((WndBase *)throne_wnd_base)->set_capture();
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
    }
    ((ModalMenuWindow *)throne_wnd_base)->run_modal_loop();
    ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(0);
    ((WndBase *)throne_wnd_base)->release_capture();
    result = render_choices();
    if (result != 0) {
      result = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_addtothrone);
      if (result == 0) {
        str_clear((char *)text_buf);
        while (line = text_read_line(1), *line != '@') {
          str_append((char *)text_buf,(const char *)text_parse.line_cursor);
          str_append((char *)text_buf,(const char *)&str_empty_70);
        }
        SetRect(&rect,100,0x78,0x21c,0x1c2);
        gfx_set_text_color_index(0);
        ((DibSurface *)throne_surface_wnd)->draw_text_rect((iptr)&font2,(const char *)text_buf,(iptr)&rect,0
                  );
        OffsetRect(&rect,2,2);
        gfx_set_text_color_index(0xff);
        ((DibSurface *)throne_surface_wnd)->draw_text_rect((iptr)&font2,(const char *)text_buf,(iptr)&rect,0
                  );
        text_close_file();
        SetRect(&rect,0,0,0x280,0x1e0);
      }
      draw_click_prompt();
      choosing_item = 1;
      SetRect(&rect,0,0,0x280,0x1e0);
      gfx_dissolve_blit((iptr)throne_surface_wnd,(iptr)&rect,8,4);
      ((DibSurface *)throne_surface_wnd)->wnd_blit_surface();
      input_flush_events();
      ((WndBase *)throne_wnd_base)->set_capture();
      if (2 < game.multiplayer) {
        ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
      }
      ((ModalMenuWindow *)throne_wnd_base)->run_modal_loop();
      ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(0);
      ((WndBase *)throne_wnd_base)->release_capture();
      snd_play_effect(SND_CHEERS_13,1,0,0);
      SetRect(&rect,0,0,0x280,0x1e0);
      ((DibSurface *)throne_backbuffer)->DibSurface::blit((DibSurface *)throne_surface_wnd,&rect,
                 &rect);
      win_flush_pending_msgs();
      gfx_dissolve_blit((iptr)throne_surface_wnd,(iptr)&rect,8,3);
      ((DibSurface *)throne_surface_wnd)->wnd_blit_surface();
      wnd_flush_paint();
      if (throne_complete == 0) {
        line = (char *)(chosen_item + (iptr)((u8 *)&civs + 1011) + throne_civ * 0x594);
        *line = *line + 1;
      }
      else {
        civs[throne_civ].throne_room[8] =
             civs[throne_civ].throne_room[8] | (byte)(1 << ((byte)chosen_item & 0x1f));
      }
      result = render();
      if (result != 0) {
        snd_play_effect(SND_CHEERS_13,1,0,0);
        SetRect(&rect,0,0,0x280,0x1e0);
        gfx_dissolve_blit((iptr)throne_surface_wnd,(iptr)&rect,4,1);
        draw_click_prompt();
        ((DibSurface *)throne_surface_wnd)->wnd_blit_surface();
        choosing_item = 0;
        ((WndBase *)throne_wnd_base)->set_capture();
        if (2 < game.multiplayer) {
          ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
        }
        ((ModalMenuWindow *)throne_wnd_base)->run_modal_loop();
        ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(0);
        ((WndBase *)throne_wnd_base)->release_capture();
        ((WndBase *)throne_wnd_base)->hide();
        wnd_flush_paint();
      }
      set_palette((iptr)&game_palette_block.game_palette);
      hide();
      wnd_flush_paint();
    }
  }
  return;
}

/* 0044d9c1 auto */
void ThroneRoomWindow::run_view()

{
  int ok;
  
  ok = create_windows();
  if ((ok != 0) && (ok = render(), ok != 0)) {
    draw_click_prompt();
    choosing_item = 0;
    handle_show_raise();
    wnd_flush_paint();
    set_palette((iptr)&control_list);
    ((SurfaceWindow *)throne_surface_wnd)->SurfaceWindow::show();
    wnd_flush_paint();
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
    }
    ((ModalMenuWindow *)throne_wnd_base)->run_modal_loop();
    ((ModalMenuWindow *)throne_wnd_base)->set_modal_idle_callback(0);
    ((WndBase *)throne_wnd_base)->hide();
    wnd_flush_paint();
    set_palette((iptr)&game_palette_block.game_palette);
    hide();
    wnd_flush_paint();
  }
  return;
}

/* 0044dab5 auto */
void ThroneRoomWindow::draw_click_prompt()

{
  iptr label;
  
  text_clear();
  text_append((const char *)&str_empty_71);
  label = (iptr)label_string(label_table.string_ids[0x1c8]);
  text_append((const char *)label);
  text_append((const char *)&str_empty_72);
  gfx_set_text_color_index(0);
  ((DibSurface *)&throne_surface_wnd)->draw_text_in_rect((iptr)&click_prompt_font,(const char *)&textbuf.buf,
             (RECT *)&click_prompt_rect,0);
  OffsetRect((LPRECT)&click_prompt_rect,-1,-1);
  gfx_set_text_color_index(0xff);
  ((DibSurface *)&throne_surface_wnd)->draw_text_in_rect((iptr)&click_prompt_font,(const char *)&textbuf.buf,
             (RECT *)&click_prompt_rect,0);
  OffsetRect((LPRECT)&click_prompt_rect,1,1);
  return;
}

/* 0044db92 auto */
i32 ThroneRoomWindow::render()

{
  int loaded;
  i8 draw_tmp9[16];
  i32 fail8;
  i8 draw_tmp8[16];
  i32 fail7;
  i8 draw_tmp7[16];
  i32 fail6;
  i8 draw_tmp6[16];
  i32 fail5;
  i8 draw_tmp5[16];
  i32 fail4;
  i8 draw_tmp4[16];
  i32 fail3;
  i8 draw_tmp3[16];
  i32 fail2;
  i8 draw_tmp2[16];
  i32 fail1;
  i8 draw_tmp1[16];
  i32 result;
  DibSurface surface;
  iptr slot;
  int level;
  Sprite sprite;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  for (slot = 0; slot < 3; slot++) {
    win_flush_pending_msgs();
    if (*(char *)&civs[(throne_civ)].throne_room[slot] != '\0') {
      level = civs[(throne_civ)].throne_room[slot] - 1;
      loaded = surface.load_gif_resource(slot * 5 + level + 0x69,10,0xec,(iptr)&control_list);
      if (loaded == 0) {
        result = 0;
        return result;
      }
      sprite.grab_from_surface((iptr)&surface,9,
                 throneroom_piece_rects[slot][level].x,
                 throneroom_piece_rects[slot][level].y,
                 throneroom_piece_rects[slot][level].w,
                 throneroom_piece_rects[slot][level].h);
      sprite.replace_color(7,9);
      sprite.draw((iptr)draw_tmp1,(iptr)throne_surface_wnd,
                 throneroom_piece_rects[slot][level].x - 1,
                 throneroom_piece_rects[slot][level].y - 1);
    }
  }
  win_flush_pending_msgs();
  if ((throne_complete != 0) && ((civs[throne_civ].throne_room[8] & 1) != 0)) {
    loaded = surface.load_gif_resource(0xa0,10,0xec,(iptr)&control_list);
    if (loaded == 0) {
      fail1 = 0;
      result = fail1;
      return result;
    }
    sprite.grab_from_surface((iptr)&surface,9,throneroom_piece4_rect_x_block.throneroom_bonus_rect_x[0],throneroom_piece4_rect_x_block.throneroom_bonus_rect_y[0],throneroom_piece4_rect_x_block.throneroom_bonus_rect_w[0],throneroom_piece4_rect_x_block.throneroom_bonus_rect_h[0]);
    sprite.replace_color(7,9);
    sprite.draw((iptr)draw_tmp2,(iptr)throne_surface_wnd,throneroom_piece4_rect_x_block.throneroom_bonus_rect_x[0] - 1,throneroom_piece4_rect_x_block.throneroom_bonus_rect_y[0] - 1);
  }
  win_flush_pending_msgs();
  if (civs[throne_civ].throne_room[3] != '\0') {
    level = civs[throne_civ].throne_room[3] - 1;
    loaded = surface.load_gif_resource(civs[throne_civ].throne_room[3] + 0x77,10,0xec,
                       (iptr)&control_list);
    if (loaded == 0) {
      fail2 = 0;
      result = fail2;
      return result;
    }
    sprite.grab_from_surface((iptr)&surface,9,
               throneroom_piece_rects[slot][level].x,
               throneroom_piece_rects[slot][level].y,
               throneroom_piece_rects[slot][level].w,
               throneroom_piece_rects[slot][level].h);
    sprite.replace_color(7,9);
    sprite.draw((iptr)draw_tmp3,(iptr)throne_surface_wnd,
               throneroom_piece_rects[3][level].x - 1,
               throneroom_piece_rects[3][level].y - 1);
  }
  win_flush_pending_msgs();
  if (throne_complete != 0) {
    for (slot = 1; slot < 3; slot++) {
      if ((1 << ((byte)slot & 0x1f) & (uint)civs[throne_civ].throne_room[8]) != 0) {
        loaded = surface.load_gif_resource(slot + 0xa0,10,0xec,(iptr)&control_list)
        ;
        if (loaded == 0) {
          fail3 = 0;
          result = fail3;
          return result;
        }
        sprite.grab_from_surface((iptr)&surface,9,throneroom_piece_rects[8][slot].x,
                   throneroom_piece_rects[8][slot].y,throneroom_piece_rects[8][slot].w,
                   throneroom_piece_rects[8][slot].h);
        sprite.replace_color(7,9);
        sprite.draw((iptr)draw_tmp4,(iptr)throne_surface_wnd,throneroom_piece_rects[8][slot].x - 1,
                   throneroom_piece_rects[8][slot].y - 1);
      }
    }
  }
  win_flush_pending_msgs();
  if (civs[throne_civ].throne_room[4] != '\0') {
    level = civs[throne_civ].throne_room[4] - 1;
    loaded = surface.load_gif_resource(civs[throne_civ].throne_room[4] + 0x7c,10,0xec,
                       (iptr)&control_list);
    if (loaded == 0) {
      fail4 = 0;
      result = fail4;
      return result;
    }
    sprite.grab_from_surface((iptr)&surface,9,throneroom_piece_rects[4][level].x,
               throneroom_piece_rects[4][level].y,
               throneroom_piece_rects[4][level].w,
               throneroom_piece_rects[4][level].h);
    sprite.replace_color(7,9);
    sprite.draw((iptr)draw_tmp5,(iptr)throne_surface_wnd,
               throneroom_piece_rects[4][level].x - 1,
               throneroom_piece_rects[4][level].y - 1);
  }
  win_flush_pending_msgs();
  if ((throne_complete != 0) && ((civs[throne_civ].throne_room[8] & 8) != 0)) {
    loaded = surface.load_gif_resource(0xa3,10,0xec,(iptr)&control_list);
    if (loaded == 0) {
      fail5 = 0;
      result = fail5;
      return result;
    }
    sprite.grab_from_surface((iptr)&surface,9,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_x,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_y,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_w,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_h);
    sprite.replace_color(7,9);
    sprite.draw((iptr)draw_tmp6,(iptr)throne_surface_wnd,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_x - 1,throneroom_piece4_rect_x_block.throneroom_bonus3_rect_y - 1);
  }
  win_flush_pending_msgs();
  for (slot = 5; slot < 7; slot++) {
    if (*(char *)&civs[(throne_civ)].throne_room[slot] != '\0') {
      level = civs[(throne_civ)].throne_room[slot] - 1;
      loaded = surface.load_gif_resource(slot * 5 + level + 0x69,10,0xec,(iptr)&control_list);
      if (loaded == 0) {
        fail6 = 0;
        result = fail6;
        return result;
      }
      sprite.grab_from_surface((iptr)&surface,9,
                 throneroom_piece_rects[slot][level].x,
                 throneroom_piece_rects[slot][level].y,
                 throneroom_piece_rects[slot][level].w,
                 throneroom_piece_rects[slot][level].h);
      sprite.replace_color(7,9);
      sprite.draw((iptr)draw_tmp7,(iptr)throne_surface_wnd,
                 throneroom_piece_rects[slot][level].x - 1,
                 throneroom_piece_rects[slot][level].y - 1);
    }
  }
  win_flush_pending_msgs();
  if (throne_complete != 0) {
    for (slot = 4; slot < 7; slot++) {
      if ((1 << ((byte)slot & 0x1f) & (uint)civs[throne_civ].throne_room[8]) != 0) {
        loaded = surface.load_gif_resource(slot + 0xa0,10,0xec,(iptr)&control_list)
        ;
        if (loaded == 0) {
          fail7 = 0;
          result = fail7;
          return result;
        }
        sprite.grab_from_surface((iptr)&surface,9,throneroom_piece_rects[8][slot].x,
                   throneroom_piece_rects[8][slot].y,throneroom_piece_rects[8][slot].w,
                   throneroom_piece_rects[8][slot].h);
        sprite.replace_color(7,9);
        sprite.draw((iptr)draw_tmp8,(iptr)throne_surface_wnd,throneroom_piece_rects[8][slot].x - 1,
                   throneroom_piece_rects[8][slot].y - 1);
      }
    }
  }
  win_flush_pending_msgs();
  if (civs[throne_civ].throne_room[7] != '\0') {
    level = civs[throne_civ].throne_room[7] - 1;
    loaded = surface.load_gif_resource(civs[throne_civ].throne_room[7] + 0x8b,10,0xec,
                       (iptr)&control_list);
    if (loaded == 0) {
      fail8 = 0;
      result = fail8;
      return result;
    }
    sprite.grab_from_surface((iptr)&surface,9,throneroom_piece_rects[7][level].x,
               throneroom_piece_rects[7][level].y,
               throneroom_piece_rects[7][level].w,
               throneroom_piece_rects[7][level].h);
    sprite.replace_color(7,9);
    sprite.draw((iptr)draw_tmp9,(iptr)throne_surface_wnd,
               throneroom_piece_rects[7][level].x - 1,
               throneroom_piece_rects[7][level].y - 1);
  }
  win_flush_pending_msgs();
  result = 1;
  return result;
}

/* Part of ThroneRoomWindow::render_choices (0044e790): the 34 clickable regions of the
   throne room (rectangles and the cursor of each) added to the click map. */
void ThroneRoomWindow::add_choice_regions()
{
  i32 region_cursors[34];
  iptr idx;
  i32 region_rects[136];

  region_rects[0] = 1;
  region_rects[1] = 2;
  region_rects[2] = 0x16;
  region_rects[3] = 0x19c;
  region_rects[4] = 2;
  region_rects[5] = 0x194;
  region_rects[6] = 0x36;
  region_rects[7] = 0x48;
  region_rects[8] = 0x25c;
  region_rects[9] = 1;
  region_rects[10] = 0x23;
  region_rects[0xb] = 0x197;
  region_rects[0xc] = 0x23c;
  region_rects[0xd] = 0x195;
  region_rects[0xe] = 0x44;
  region_rects[0xf] = 0x49;
  region_rects[0x10] = 0x21d;
  region_rects[0x11] = 0x16d;
  region_rects[0x12] = 0x41;
  region_rects[0x13] = 0x46;
  region_rects[0x14] = 0x1c;
  region_rects[0x15] = 0x16e;
  region_rects[0x16] = 0x47;
  region_rects[0x17] = 0x44;
  region_rects[0x18] = 0x47;
  region_rects[0x19] = 0x124;
  region_rects[0x1a] = 100;
  region_rects[0x1b] = 0x27;
  region_rects[0x1c] = 0x1ad;
  region_rects[0x1d] = 0x124;
  region_rects[0x1e] = 99;
  region_rects[0x1f] = 0x27;
  region_rects[0x20] = 0x47;
  region_rects[0x21] = 0x124;
  region_rects[0x22] = 100;
  region_rects[0x23] = 0x27;
  region_rects[0x24] = 0x1ad;
  region_rects[0x25] = 0x124;
  region_rects[0x26] = 99;
  region_rects[0x27] = 0x27;
  region_rects[0x28] = 0x11c;
  region_rects[0x29] = 0x102;
  region_rects[0x2a] = 0x27;
  region_rects[0x2b] = 0x39;
  region_rects[0x2c] = 0x25;
  region_rects[0x2d] = 0x12;
  region_rects[0x2e] = 0x2d;
  region_rects[0x2f] = 0x120;
  region_rects[0x30] = 0x93;
  region_rects[0x31] = 0x12;
  region_rects[0x32] = 0x2c;
  region_rects[0x33] = 0x124;
  region_rects[0x34] = 0x19d;
  region_rects[0x35] = 0x14;
  region_rects[0x36] = 0x31;
  region_rects[0x37] = 0x121;
  region_rects[0x38] = 0x20b;
  region_rects[0x39] = 0x14;
  region_rects[0x3a] = 0x2f;
  region_rects[0x3b] = 0x11e;
  region_rects[0x3c] = 0xe;
  region_rects[0x3d] = 3;
  region_rects[0x3e] = 0xcf;
  region_rects[0x3f] = 0xc;
  region_rects[0x40] = 0x17f;
  region_rects[0x41] = 1;
  region_rects[0x42] = 0xd3;
  region_rects[0x43] = 0xe;
  region_rects[0x44] = 0xe3;
  region_rects[0x45] = 0xd4;
  region_rects[0x46] = 0xe;
  region_rects[0x47] = 0x51;
  region_rects[0x48] = 0x16f;
  region_rects[0x49] = 0xd5;
  region_rects[0x4a] = 0xb;
  region_rects[0x4b] = 0x52;
  region_rects[0x4c] = 0xbe;
  region_rects[0x4d] = 0x11;
  region_rects[0x4e] = 0x15;
  region_rects[0x4f] = 0x11a;
  region_rects[0x50] = 0x185;
  region_rects[0x51] = 0x12;
  region_rects[0x52] = 0x1a;
  region_rects[0x53] = 0x119;
  region_rects[0x54] = 0xde;
  region_rects[0x55] = 0x24;
  region_rects[0x56] = 0x96;
  region_rects[0x57] = 0x55;
  region_rects[0x58] = 0x11;
  region_rects[0x59] = 0x114;
  region_rects[0x5a] = 0xc1;
  region_rects[0x5b] = 0x19;
  region_rects[0x5c] = 400;
  region_rects[0x5d] = 0x107;
  region_rects[0x5e] = 0xd0;
  region_rects[0x5f] = 0x26;
  region_rects[0x60] = 0x5e;
  region_rects[0x61] = 0x75;
  region_rects[0x62] = 0x36;
  region_rects[99] = 0x47;
  region_rects[100] = 0x10b;
  region_rects[0x65] = 0x6c;
  region_rects[0x66] = 0x46;
  region_rects[0x67] = 0x5e;
  region_rects[0x68] = 0x1cc;
  region_rects[0x69] = 0x75;
  region_rects[0x6a] = 0x33;
  region_rects[0x6b] = 0x49;
  region_rects[0x6c] = 0xb;
  region_rects[0x6d] = 2;
  region_rects[0x6e] = 0x93;
  region_rects[0x6f] = 0x11b;
  region_rects[0x70] = 0x1c2;
  region_rects[0x71] = 2;
  region_rects[0x72] = 0xa7;
  region_rects[0x73] = 0x11b;
  region_rects[0x74] = 0x14;
  region_rects[0x75] = 0x13c;
  region_rects[0x76] = 0xb8;
  region_rects[0x77] = 0x22;
  region_rects[0x78] = 0x193;
  region_rects[0x79] = 0x13d;
  region_rects[0x7a] = 0xcc;
  region_rects[0x7b] = 0x23;
  region_rects[0x7c] = 0xde;
  region_rects[0x7d] = 0x12f;
  region_rects[0x7e] = 0xa8;
  region_rects[0x7f] = 0xaf;
  region_rects[0x80] = 0x13;
  region_rects[0x81] = 0x123;
  region_rects[0x82] = 0x250;
  region_rects[0x83] = 0xbd;
  region_rects[0x84] = 0xd4;
  region_rects[0x85] = 1;
  region_rects[0x86] = 0xb3;
  region_rects[0x87] = 0x127;
  region_cursors[0] = 7;
  region_cursors[1] = 7;
  region_cursors[2] = 7;
  region_cursors[3] = 7;
  region_cursors[4] = 0x10;
  region_cursors[5] = 0x10;
  region_cursors[6] = 0xf;
  region_cursors[7] = 0xf;
  region_cursors[8] = 0xe;
  region_cursors[9] = 0xe;
  region_cursors[10] = 6;
  region_cursors[0xb] = 5;
  region_cursors[0xc] = 5;
  region_cursors[0xd] = 5;
  region_cursors[0xe] = 5;
  region_cursors[0xf] = 5;
  region_cursors[0x10] = 5;
  region_cursors[0x11] = 0xd;
  region_cursors[0x12] = 0xd;
  region_cursors[0x13] = 4;
  region_cursors[0x14] = 4;
  region_cursors[0x15] = 4;
  region_cursors[0x16] = 0xc;
  region_cursors[0x17] = 0xc;
  region_cursors[0x18] = 0xb;
  region_cursors[0x19] = 0xb;
  region_cursors[0x1a] = 0xb;
  region_cursors[0x1b] = 3;
  region_cursors[0x1c] = 3;
  region_cursors[0x1d] = 10;
  region_cursors[0x1e] = 10;
  region_cursors[0x1f] = 2;
  region_cursors[0x20] = 1;
  region_cursors[0x21] = 0;
  ((ClickMap *)choice_clickmap)->clear();
  for (idx = 0; idx < 0x22; idx++) {
    ((ClickMap *)choice_clickmap)->add(idx,region_cursors[idx],
               region_rects[idx * 4],region_rects[idx * 4 + 1],region_rects[idx * 4 + 2],
               region_rects[idx * 4 + 3]);
  }
}

/* 0044e790 auto */
i32 ThroneRoomWindow::render_choices()

{
  int loaded;
  i8 draw_tmp2[16];
  i32 fail2;
  i8 draw_tmp1[16];
  i32 fail1;
  i32 result;
  DibSurface surface;
  iptr idx;
  int piece_rects[60];
  Sprite sprite;
  byte bonus_bits;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  piece_rects[0] = 0xd4;
  piece_rects[1] = 1;
  piece_rects[2] = 0xb3;
  piece_rects[3] = 0x127;
  piece_rects[4] = 0x13;
  piece_rects[5] = 0x11b;
  piece_rects[6] = 0x24f;
  piece_rects[7] = 0xc6;
  piece_rects[8] = 0xc6;
  piece_rects[9] = 0x12f;
  piece_rects[10] = 0xd7;
  piece_rects[0xb] = 0xb1;
  piece_rects[0xc] = 0xb;
  piece_rects[0xd] = 1;
  piece_rects[0xe] = 0x25f;
  piece_rects[0xf] = 0x11e;
  piece_rects[0x10] = 0xbc;
  piece_rects[0x11] = 0x12;
  piece_rects[0x12] = 0xe6;
  piece_rects[0x13] = 0x11b;
  piece_rects[0x14] = 1;
  piece_rects[0x15] = 1;
  piece_rects[0x16] = 0x260;
  piece_rects[0x17] = 0x137;
  piece_rects[0x18] = 0x11c;
  piece_rects[0x19] = 0x102;
  piece_rects[0x1a] = 0x27;
  piece_rects[0x1b] = 0x39;
  piece_rects[0x1c] = 1;
  piece_rects[0x1d] = 1;
  piece_rects[0x1e] = 0x280;
  piece_rects[0x1f] = 0x1e0;
  piece_rects[0x20] = 0x14;
  piece_rects[0x21] = 0x13a;
  piece_rects[0x22] = 0x24f;
  piece_rects[0x23] = 0x28;
  piece_rects[0x24] = 0x5f;
  piece_rects[0x25] = 0x6d;
  piece_rects[0x26] = 0x1a3;
  piece_rects[0x27] = 0x5f;
  piece_rects[0x28] = 0x12;
  piece_rects[0x29] = 0x103;
  piece_rects[0x2a] = 0x252;
  piece_rects[0x2b] = 0x2d;
  piece_rects[0x2c] = 0xde;
  piece_rects[0x2d] = 0xd4;
  piece_rects[0x2e] = 0xa2;
  piece_rects[0x2f] = 0x54;
  piece_rects[0x30] = 0x7b;
  piece_rects[0x31] = 0x125;
  piece_rects[0x32] = 0x162;
  piece_rects[0x33] = 0x23;
  piece_rects[0x34] = 0x48;
  piece_rects[0x35] = 0x125;
  piece_rects[0x36] = 0x1c8;
  piece_rects[0x37] = 0x27;
  piece_rects[0x38] = 0x16;
  piece_rects[0x39] = 0x16e;
  piece_rects[0x3a] = 0x249;
  piece_rects[0x3b] = 0x47;
  add_choice_regions();
  loaded = ((DibSurface *)throne_surface_wnd)->load_gif_resource(100,10,0xec,(iptr)&control_list);
  if (loaded == 0) {
    result = 0;
  }
  else {
    if (throne_complete == 0) {
      for (idx = 0; idx < 8; idx++) {
        win_flush_pending_msgs();
        if (civs[(throne_civ)].throne_room[idx] < 4) {
          loaded = surface.load_gif_resource(idx + 0xaa,10,0xec,(iptr)&control_list);
          if (loaded == 0) {
            fail1 = 0;
            result = fail1;
            return result;
          }
          sprite.grab_from_surface((iptr)&surface,9,piece_rects[idx * 4],
                     piece_rects[idx * 4 + 1],piece_rects[idx * 4 + 2],
                     piece_rects[idx * 4 + 3]);
          sprite.replace_color(7,9);
          sprite.draw((iptr)draw_tmp1,(iptr)throne_surface_wnd,piece_rects[idx * 4] - 1,
                     piece_rects[idx * 4 + 1] - 1);
        }
      }
    }
    else {
      bonus_bits = civs[throne_civ].throne_room[8];
      for (idx = 0; idx < 7; idx++) {
        win_flush_pending_msgs();
        if ((1 << ((byte)idx & 0x1f) & (uint)bonus_bits) == 0) {
          loaded = surface.load_gif_resource(idx + 0xb2,10,0xec,(iptr)&control_list);
          if (loaded == 0) {
            fail2 = 0;
            result = fail2;
            return result;
          }
          sprite.grab_from_surface((iptr)&surface,9,piece_rects[idx * 4 + 0x20],
                     piece_rects[idx * 4 + 0x21],piece_rects[idx * 4 + 0x22],
                     piece_rects[idx * 4 + 0x23]);
          sprite.replace_color(7,9);
          sprite.draw((iptr)draw_tmp2,(iptr)throne_surface_wnd,
                     piece_rects[idx * 4 + 0x20] - 1,piece_rects[idx * 4 + 0x21] - 1);
        }
      }
    }
    result = 1;
  }
  return result;
}

/* 0044f4c9 auto */
void __cdecl throneroom_on_click(i32 x,i32 y)

{
  int hit;
  iptr civ;
  i8 hit_buf[4];
  byte bonus_bits;
  ThroneRoomWindow *wnd2;
  
  wnd2 = throneroom_wnd;
  civ = (iptr)throneroom_wnd->throne_civ;
  if (throneroom_wnd->choosing_item == 0) {
    ((ModalMenuWindow *)throneroom_wnd->throne_wnd_base)->end_modal_loop();
  }
  else {
    hit = ((ClickMap *)throneroom_wnd->choice_clickmap)->hit_test(x,y,(iptr)hit_buf,
                       (iptr)&throneroom_wnd->chosen_item);
    if (hit < 0) {
      wnd2->chosen_item = 0;
      snd_play_effect(SND_FEEDBK04,1,0,0);
    }
    else {
      if (wnd2->throne_complete == 0) {
        if (9 < (int)wnd2->chosen_item) {
          snd_play_effect(SND_FEEDBK04,1,0,0);
          return;
        }
        if (3 < civs[civ].throne_room[((int)wnd2->chosen_item)]) {
          snd_play_effect(SND_FEEDBK04,1,0,0);
          return;
        }
      }
      else {
        if ((int)wnd2->chosen_item < 10) {
          snd_play_effect(SND_FEEDBK04,1,0,0);
          return;
        }
        wnd2->chosen_item = (int)wnd2->chosen_item - 10;
        bonus_bits = civs[civ].throne_room[8];
        if (((int)wnd2->chosen_item == 4) || ((int)wnd2->chosen_item == 5)) {
          if (((bonus_bits & 0x10) != 0) && ((bonus_bits & 0x20) != 0)) {
            snd_play_effect(SND_FEEDBK04,1,0,0);
            return;
          }
          if ((int)wnd2->chosen_item == 4) {
            if ((bonus_bits & 0x10) != 0) {
              wnd2->chosen_item = 5;
            }
          }
          else if ((bonus_bits & 0x20) != 0) {
            wnd2->chosen_item = 4;
          }
        }
        if ((1 << ((byte)wnd2->chosen_item & 0x1f) & (uint)civs[civ].throne_room[8]) != 0) {
          snd_play_effect(SND_FEEDBK04,1,0,0);
          return;
        }
      }
      ((ModalMenuWindow *)wnd2->throne_wnd_base)->end_modal_loop();
    }
  }
  return;
}

/* 0044f717 auto */
void __cdecl throneroom_on_key(int key)

{
  if (throneroom_wnd->choosing_item == 0) {
    if ((0xcf < key) && (key < 0xd3)) {
      ((ModalMenuWindow *)throneroom_wnd->throne_wnd_base)->end_modal_loop();
    }
  }
  else {
    snd_play_effect(SND_FEEDBK04,1,0,0);
  }
  return;
}

/* 0044f799 auto */
void __cdecl throneroom_close(void)

{
  if ((throneroom_wnd != 0) && (throneroom_wnd->choosing_item == 0)) {
    ((ModalMenuWindow *)throneroom_wnd->throne_wnd_base)->end_modal_loop();
  }
  return;
}

/* 004702e0 auto */
void __cdecl endgame_defeat_movie(iptr civ)

{
  EndgameMovieWindow *self;
  int created;
  EndgameMovieWindow *window;
  
  self = (EndgameMovieWindow *)operator_new(0x137c);
  if (self == (EndgameMovieWindow *)0x0) {
    window = (EndgameMovieWindow *)0x0;
  }
  else {
    window = self->defeat_movie_wnd_construct();
  }
  endgame_movie_window = window;
  if (window != (EndgameMovieWindow *)0x0) {
    created = window->defeat_movie_wnd_create(civ);
    if (created != 0) {
      endgame_movie_window->defeat_movie_wnd_play();
    }
    if (endgame_movie_window != (EndgameMovieWindow *)0x0) {
      endgame_movie_window->defeat_movie_wnd_scalar_destruct(1);
    }
    endgame_movie_window = (EndgameMovieWindow *)0x0;
  }
  return;
}

/* 004703d4 auto */
EndgameMovieWindow *EndgameMovieWindow::defeat_movie_wnd_construct()

{
  
  this->ModalMenuWindow::construct();
  ((GamePalette *)&palette)->init();
  ((AviPlayer *)media_wnd)->construct();
  ((DllHandle *)movie_dll)->init();
  ((GameFont *)movie_caption_font)->init_empty();
  surface_ctor((DibSurface *)movie_frame_surface);
  advance_civ = 0;
  return this;
}

/* 004704ec auto */
void EndgameMovieWindow::defeat_movie_wnd_destruct()

{
  int try_level;
  
  try_level = try_level & 0xff;
  (*((char *)&try_level + 0)) = 4;
  /* inlined unwind funclet 0047056b */ { surface_dtor((DibSurface *)(((int)this) + 0x1324)); }
  (*((char *)&try_level + 0)) = 3;
  /* inlined unwind funclet 0047057a */ { ((GameFont *)(((int)this) + 0xf1c))->destroy(); }
  (*((char *)&try_level + 0)) = 2;
  /* inlined unwind funclet 00470589 */ { ((DllHandle *)(((int)this) + 0xf14))->free(); }
  (*((char *)&try_level + 0)) = 1;
  /* inlined unwind funclet 00470598 */ { ((AviPlayer *)(((int)this) + 0x4ec))->destruct(); }
  try_level = try_level & 0xffffff00;
  /* inlined unwind funclet 004705a7 */ { ((GamePalette *)(((int)this) + 0xb8))->destroy(); }
  try_level = 0xffffffff;
  /* inlined unwind funclet 004705b6 */ { destruct(); }
  return;
}

/* 004705d7 auto */
i32 EndgameMovieWindow::defeat_movie_wnd_create(uint civ)

{
  HFILE file_handle;
  i32 result;
  i32 width;
  CHAR dll_path[128];
  _OFSTRUCT of_struct;
  RECT screen_bounds;
  int open_err;
  CHAR avi_path[128];
  
  gfx_get_screen_rect((iptr)&screen_bounds);
  str_clear((char *)avi_path);
  str_append((char *)avi_path,(const char *)&game_dirs.home);
  str_append((char *)avi_path,(const char *)&str_loser_avi);
  file_handle = OpenFile(avi_path,&of_struct,0x4000);
  if (file_handle == -1) {
    result = 0;
  }
  else {
    str_clear((char *)dll_path);
    str_append((char *)dll_path,(const char *)&game_dirs.home);
    str_append((char *)dll_path,(const char *)&str_civ2art_dll);
    file_handle = OpenFile(dll_path,&of_struct,0x4000);
    if (file_handle == -1) {
      result = 0;
    }
    else {
      ((DllHandle *)movie_dll)->load((const char *)dll_path);
      if ((int)civ < 1) {
        advance_civ = ~civ + 1;
      }
      else {
        advance_civ = civ;
      }
      create_with_handlers((const char *)&str_empty_84,0x800,screen_bounds.left,screen_bounds.top,screen_bounds.right - screen_bounds.left,
                 (screen_bounds.bottom - screen_bounds.top) + 5,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
      set_bg_color(0);
      ((DibSurface *)media_wnd)->load_gif_resource(4000,10,0xec,(iptr)&palette);
      ((AviPlayer *)media_wnd)->create((iptr)&str_empty_85,0x800,0,0);
      ((AviPlayer *)media_wnd)->set_double_size(1);
      open_err = ((AviPlayer *)media_wnd)->open((const char *)avi_path);
      if (open_err == 0) {
        ((DibSurface *)media_wnd)->fill(0);
        ((WndBase *)movie_wnd_base)->center();
        ((AviPlayer *)media_wnd)->set_end_callback(defeat_movie_on_video_end);
        SetRect((LPRECT)(movie_caption_rect),0x28,100,600,0x1e0);
        result = ((DibSurface *)media_wnd)->get_height();
        width = ((DibSurface *)media_wnd)->get_width();
        ((DibSurface *)movie_frame_surface)->DibSurface::create(width,result);
        ((DibSurface *)media_wnd)->avi_player_draw_to((iptr)(movie_frame_surface),0,0);
        ((GameFont *)movie_caption_font)->set_styled(0,0x12,2);
        ((DllHandle *)movie_dll)->dll_handle_free();
        result = 1;
      }
      else {
        if (open_err == -0x7ffbfeac) {
          popup_game((const char *)&str_vfwnotregistered_2);
        }
        result = 0;
      }
    }
  }
  return result;
}

/* 004708db auto */
void EndgameMovieWindow::defeat_movie_wnd_play()

{
  int status;
  iptr str;
  char *line;
  iptr advance;
  int page;
  
  music_play_track(2,1);
  handle_show_raise();
  wnd_flush_paint();
  set_palette((iptr)&palette);
  ((SurfaceWindow *)media_wnd)->SurfaceWindow::show();
  wnd_flush_paint();
  ((WndBase *)movie_wnd_base)->set_capture();
  gfx_set_text_color_index(0xff);
  advance = 0;
  while ((advance < 100 && (status = civ_has_advance(advance_civ,advance), status == 0))) {
    advance++;
  }
  str = (iptr)label_string(advances[advance].name);
  text_set_string_arg(0,(const char *)str);
  CharLowerA(((i8 *)&text_args.strings));
  str = (iptr)(civ_leader_name(advance_civ));
  text_set_string_arg(1,(const char *)str);
  for (page = 0; page < 3; page++) {
    str_clear((char *)movie_caption_text);
    text_clear();
    text_append((const char *)&str_archaeologists);
    text_append_int(page);
    status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&textbuf.buf);
    if (status == 0) {
      while (line = text_read_line(1), *line != '@') {
        str_append((char *)movie_caption_text,(const char *)text_parse.line_cursor);
        str_append((char *)movie_caption_text,(const char *)&str_empty_86);
      }
      text_clear();
      text_expand_vars((iptr)(movie_caption_text),(iptr)&textbuf.buf);
      ((DibSurface *)movie_frame_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),
                 (RECT *)(iptr)(movie_caption_rect),(RECT *)(iptr)(movie_caption_rect));
      OffsetRect((LPRECT)(movie_caption_rect),1,1);
      gfx_set_text_color_index(0);
      ((DibSurface *)media_wnd)->draw_text_rect((iptr)movie_caption_font,(const char *)&textbuf.buf,
                 (iptr)(movie_caption_rect),0);
      OffsetRect((LPRECT)(movie_caption_rect),-1,-1);
      gfx_set_text_color_index(0xff);
      ((DibSurface *)media_wnd)->draw_text_rect((iptr)movie_caption_font,(const char *)&textbuf.buf,
                 (iptr)(movie_caption_rect),0);
      gfx_dissolve_blit((iptr)(media_wnd),(iptr)(movie_caption_rect),4,5);
      text_close_file();
      ui_delay_ticks(0x168);
    }
  }
  ((AviPlayer *)media_wnd)->play();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)movie_wnd_base)->run_modal_loop();
  ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(0);
  ((WndBase *)movie_wnd_base)->release_capture();
  ((WndBase *)movie_wnd_base)->hide();
  wnd_flush_paint();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
  hide();
  wnd_flush_paint();
  return;
}

/* 00470c1c auto */
void __cdecl defeat_movie_on_video_end(void)

{
  int status;
  char *line;
  
  str_clear((char *)((*(int *)&endgame_movie_window) + 0xf24));
  text_clear();
  status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_archaeologists3);
  if (status == 0) {
    while( true ) {
      line = text_read_line(1);
      if (*line == '@') break;
      str_append((char *)((*(int *)&endgame_movie_window) + 0xf24),(const char *)text_parse.line_cursor);
      str_append((char *)((*(int *)&endgame_movie_window) + 0xf24),(const char *)&str_empty_87);
    }
    text_expand_vars((*(int *)&endgame_movie_window) + 0xf24,(iptr)&textbuf.buf);
    CharUpperA(((i8 *)&textbuf.buf));
    rect_set_xywh((*(int *)&endgame_movie_window) + 0x136c,0xe5,0x4c,200,0x85);
    gfx_set_text_color_index(0);
    ((DibSurface *)((*(int *)&endgame_movie_window) + 0x4ec))->draw_text_rect((*(int *)&endgame_movie_window) + 0xf1c,(const char *)&textbuf.buf,
               (*(int *)&endgame_movie_window) + 0x136c,0);
    gfx_dissolve_blit((*(int *)&endgame_movie_window) + 0x4ec,(*(int *)&endgame_movie_window) + 0x136c,4,5);
    text_close_file();
  }
  ui_delay_ticks(0x168);
  ((ModalMenuWindow *)((*(int *)&endgame_movie_window) + 0x534))->end_modal_loop();
  return;
}

/* 00471020 auto */
EndgameMovieWindow *EndgameMovieWindow::defeat_movie_wnd_scalar_destruct(uint flags)

{
  defeat_movie_wnd_destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004710d0 auto */
void __cdecl endgame_spaceship_arrival(iptr civ)

{
  EndgameMovieWindow *self;
  int created;
  iptr str;
  EndgameMovieWindow *window;
  Dialog dialog2;
  
  if (civ < 0) {
    endgame_show_centauri_beaten(civ);
  }
  else {
    snd_cd_stop();
    self = (EndgameMovieWindow *)operator_new(0x137c);
    if (self == (EndgameMovieWindow *)0x0) {
      window = (EndgameMovieWindow *)0x0;
    }
    else {
      window = self->arrival_movie_wnd_construct();
    }
    spaceship_arrival_window = (iptr)window;
    if (window != (EndgameMovieWindow *)0x0) {
      created = window->arrival_movie_wnd_create(civ);
      if (created == 0) {
        text_clear();
        text_append_year((int)civs[civ].spaceship_launch_year);
        text_set_string_arg(0,(const char *)&textbuf.buf);
        text_clear();
        text_append_int((int)civs[civ].spaceship_population);
        text_append((const char *)&str_0_000_3);
        text_set_string_arg(1,(const char *)&textbuf.buf);
        str = civ_get_adjective(civ);
        text_set_string_arg(2,(const char *)str);
        text_clear();
        text_append_year((int)civs[civ].spaceship_arrival_year);
        text_set_number_arg(3,(int)civs[civ].spaceship_arrival_year);
        str = (iptr)(civ_leader_name(civ));
        text_set_string_arg(4,(const char *)str);
        dialog2.Dialog::construct(0x4000);
        ON_SCOPE_EXIT(dialog2.Dialog::destruct());
        dialog2.load_game_text((const char *)&str_centauri);
        dialog2.add_sprite((iptr)&popup_info_icon_sprite,0,0);
        dialog2.show_modal(0);
      }
      else {
        ((EndgameMovieWindow *)spaceship_arrival_window)->arrival_movie_wnd_play();
      }
      if (spaceship_arrival_window != 0) {
        ((EndgameMovieWindow *)spaceship_arrival_window)->arrival_movie_wnd_scalar_destruct(1);
      }
      spaceship_arrival_window = 0;
    }
  }
  return;
}

/* 00471362 auto */
EndgameMovieWindow *EndgameMovieWindow::arrival_movie_wnd_construct()

{
  
  this->ModalMenuWindow::construct();
  ((GamePalette *)&palette)->init();
  ((AviPlayer *)media_wnd)->construct();
  ((DllHandle *)movie_dll)->init();
  ((GameFont *)movie_caption_font)->init_empty();
  surface_ctor((DibSurface *)movie_frame_surface);
  advance_civ = 0;
  return this;
}

/* 0047147a auto */
void EndgameMovieWindow::arrival_movie_wnd_destruct()

{
  int try_level;
  
  try_level = try_level & 0xff;
  (*((char *)&try_level + 0)) = 4;
  /* inlined unwind funclet 004714f9 */ { surface_dtor((DibSurface *)(((int)this) + 0x1324)); }
  (*((char *)&try_level + 0)) = 3;
  /* inlined unwind funclet 00471508 */ { ((GameFont *)(((int)this) + 0xf1c))->destroy(); }
  (*((char *)&try_level + 0)) = 2;
  /* inlined unwind funclet 00471517 */ { ((DllHandle *)(((int)this) + 0xf14))->free(); }
  (*((char *)&try_level + 0)) = 1;
  /* inlined unwind funclet 00471526 */ { ((AviPlayer *)(((int)this) + 0x4ec))->destruct(); }
  try_level = try_level & 0xffffff00;
  /* inlined unwind funclet 00471535 */ { ((GamePalette *)(((int)this) + 0xb8))->destroy(); }
  try_level = 0xffffffff;
  /* inlined unwind funclet 00471544 */ { destruct(); }
  return;
}

/* 00471565 auto */
i32 EndgameMovieWindow::arrival_movie_wnd_create(iptr civ)

{
  int found;
  i32 result;
  i32 width;
  i8 dll_path[128];
  RECT screen_bounds;
  int open_err;
  i8 avi_path[128];
  
  if (civ < 1) {
    advance_civ = ~civ + 1;
  }
  else {
    advance_civ = civ;
  }
  gfx_get_screen_rect((iptr)&screen_bounds);
  file_require_cdrom((const char *)&str_civ2_civ2_exe);
  str_clear((char *)avi_path);
  str_append((char *)avi_path,(const char *)&str_civ2_video_winwin_avi);
  found = file_resolve_path((const char *)avi_path);
  if (found == 0) {
    result = 0;
  }
  else {
    str_clear((char *)dll_path);
    str_append((char *)dll_path,(const char *)&str_civ2_civ2art_dll_2);
    found = file_resolve_path((const char *)dll_path);
    if (found == 0) {
      result = 0;
    }
    else {
      ((DllHandle *)movie_dll)->load((const char *)dll_path);
      create_with_handlers((const char *)&str_empty_88,0x800,screen_bounds.left,screen_bounds.top,screen_bounds.right - screen_bounds.left,
                 (screen_bounds.bottom - screen_bounds.top) + 5,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
      set_bg_color(0);
      ((DibSurface *)media_wnd)->load_gif_resource(2000,10,0xec,(iptr)&palette);
      ((AviPlayer *)media_wnd)->create((iptr)&str_empty_89,0x800,0,0);
      ((AviPlayer *)media_wnd)->set_double_size(1);
      open_err = ((AviPlayer *)media_wnd)->open((const char *)avi_path);
      if (open_err == 0) {
        ((DibSurface *)media_wnd)->fill(0);
        ((WndBase *)movie_wnd_base)->center();
        ((AviPlayer *)media_wnd)->set_end_callback(arrival_movie_on_video_end);
        ((AviPlayer *)media_wnd)->set_frame_callback(arrival_movie_on_frame,0x2ab);
        SetRect((LPRECT)(movie_caption_rect),2,100,0x27e,0x1e0);
        result = ((DibSurface *)media_wnd)->get_height();
        width = ((DibSurface *)media_wnd)->get_width();
        ((DibSurface *)movie_frame_surface)->DibSurface::create(width,result);
        ((DibSurface *)media_wnd)->avi_player_draw_to((iptr)(movie_frame_surface),0,0);
        ((GameFont *)movie_caption_font)->set_styled(0,0x12,3);
        ((DllHandle *)movie_dll)->dll_handle_free();
        result = 1;
      }
      else {
        if (open_err == -0x7ffbfeac) {
          popup_game((const char *)&str_vfwnotregistered_3);
        }
        result = 0;
      }
    }
  }
  return result;
}

/* 00471856 auto */
void EndgameMovieWindow::arrival_movie_wnd_play()

{
  iptr str;
  int status;
  char *line;
  int page;
  
  handle_show_raise();
  wnd_flush_paint();
  set_palette((iptr)&palette);
  ((SurfaceWindow *)media_wnd)->SurfaceWindow::show();
  wnd_flush_paint();
  ((WndBase *)movie_wnd_base)->set_capture();
  gfx_set_text_color_index(0xff);
  str_clear((char *)movie_caption_text);
  text_clear();
  text_append_year((int)civs[advance_civ].spaceship_launch_year);
  text_set_string_arg(0,(const char *)&textbuf.buf);
  text_clear();
  text_append_int((int)civs[advance_civ].spaceship_population);
  text_append((const char *)&str_0_000_4);
  text_set_string_arg(1,(const char *)&textbuf.buf);
  str = civ_get_adjective(advance_civ);
  text_set_string_arg(2,(const char *)str);
  text_clear();
  text_append_year((int)civs[advance_civ].spaceship_arrival_year);
  text_set_string_arg(3,(const char *)&textbuf.buf);
  str = (iptr)(civ_leader_name(advance_civ));
  text_set_string_arg(4,(const char *)str);
  for (page = 0; page < 3; page++) {
    str_clear((char *)movie_caption_text);
    text_clear();
    text_append((const char *)&str_centauri_2);
    text_append_int(page);
    status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&textbuf.buf);
    if (status == 0) {
      while( true ) {
        line = text_read_line(1);
        if (*line == '@') break;
        str_append((char *)movie_caption_text,(const char *)text_parse.line_cursor);
        str_append((char *)movie_caption_text,(const char *)&str_empty_90);
      }
      text_clear();
      text_expand_vars((iptr)(movie_caption_text),(iptr)&textbuf.buf);
      ((DibSurface *)movie_frame_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),
                 (RECT *)(iptr)(movie_caption_rect),(RECT *)(iptr)(movie_caption_rect));
      OffsetRect((LPRECT)(movie_caption_rect),1,1);
      gfx_set_text_color_index(0);
      ((DibSurface *)media_wnd)->draw_text_rect((iptr)movie_caption_font,(const char *)&textbuf.buf,
                 (iptr)(movie_caption_rect),0);
      OffsetRect((LPRECT)(movie_caption_rect),-1,-1);
      gfx_set_text_color_index(0xf1);
      ((DibSurface *)media_wnd)->draw_text_rect((iptr)movie_caption_font,(const char *)&textbuf.buf,
                 (iptr)(movie_caption_rect),0);
      gfx_dissolve_blit((iptr)(media_wnd),(iptr)(movie_caption_rect),4,5);
      text_close_file();
      ui_delay_ticks(0x168);
    }
  }
  ((AviPlayer *)media_wnd)->play();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)movie_wnd_base)->run_modal_loop();
  ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(0);
  ((WndBase *)movie_wnd_base)->release_capture();
  ((WndBase *)movie_wnd_base)->hide();
  wnd_flush_paint();
  hide();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
  wnd_flush_paint();
  return;
}

/* 00471c2a auto */
void __cdecl arrival_movie_on_frame(void)

{
  int status;
  char *line;
  i32 width;
  int text_y2;
  RECT dirty_rect;
  
  *(i32 *)(spaceship_arrival_window + 0xa98) = 0;
  str_clear((char *)(spaceship_arrival_window + 0xf24));
  text_clear();
  status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_centauri3);
  if (status == 0) {
    text_y2 = 0xc;
    text_set_font(spaceship_arrival_window + 0xf1c);
    text_set_surface(spaceship_arrival_window + 0x4ec);
    text_set_colors(0xff,0,1,1);
    while( true ) {
      line = text_read_line(1);
      if (*line == '@') break;
      str_clear((char *)(spaceship_arrival_window + 0xf24));
      str_append((char *)(spaceship_arrival_window + 0xf24),(const char *)text_parse.line_cursor);
      text_expand_vars(spaceship_arrival_window + 0xf24,(iptr)&textbuf.buf);
      width = rect_width((RECT *)(spaceship_arrival_window + 0x136c));
      gfx_draw_text_centered((const char *)&textbuf.buf,*(i32 *)(spaceship_arrival_window + 0x136c),text_y2,width);
      status = ((GameFont *)(spaceship_arrival_window + 0xf1c))->line_spacing();
      text_y2 += status;
    }
    dirty_rect.left = *(i32 *)(spaceship_arrival_window + 0x136c);
    dirty_rect.right = *(i32 *)(spaceship_arrival_window + 0x1374);
    dirty_rect.top = 0xc;
    dirty_rect.bottom = text_y2;
    gfx_dissolve_blit(spaceship_arrival_window + 0x4ec,(iptr)&dirty_rect,4,5);
    text_close_file();
  }
  return;
}

/* 00471db7 auto */
void __cdecl arrival_movie_on_video_end(void)

{
  ((ModalMenuWindow *)(spaceship_arrival_window + 0x534))->end_modal_loop();
  return;
}

/* 00471dd8 auto */
void __cdecl endgame_show_centauri_beaten(uptr civ)

{
  int status;
  iptr str;
  char *line;
  i8 surface_wnd[276];
  ModalMenuWindow frame_wnd;
  tagRECT text_rect;
  int page;
  DibSurface backup_surface;
  GameFont font2;
  i8 text_buf[256];
  DllHandle dll;
  RECT screen_bounds;
  i8 dll_path[128];
  GamePalette palette;
  
  ((SurfaceWindow *)surface_wnd)->SurfaceWindow::construct();
  ON_SCOPE_EXIT(((SurfaceWindow *)&surface_wnd)->SurfaceWindow::destruct());
  dll.init();
  ON_SCOPE_EXIT(dll.free());
  frame_wnd.ModalMenuWindow::construct();
  ON_SCOPE_EXIT(frame_wnd.ModalMenuWindow::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  surface_ctor(&backup_surface);
  ON_SCOPE_EXIT(surface_dtor(&backup_surface));
  font2.init_empty();
  ON_SCOPE_EXIT(font2.destroy());
  if ((int)civ < 1) {
    civ = ~civ + 1;
  }
  font2.set_styled(0,0x12,3);
  str_clear((char *)dll_path);
  str_append((char *)dll_path,(const char *)&str_civ2_civ2art_dll_3);
  status = file_resolve_path((const char *)dll_path);
  if (status != 0) {
    dll.load((const char *)dll_path);
    gfx_get_screen_rect((iptr)&screen_bounds);
    SetRect(&text_rect,0,0,0x280,0xf0);
    InflateRect(&text_rect,-10,0);
    ((WndBase *)&frame_wnd)->create_with_handlers((const char *)&str_empty_91,0x800,screen_bounds.left,screen_bounds.top,screen_bounds.right - screen_bounds.left,
               (screen_bounds.bottom - screen_bounds.top) + 5,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
    ((WndBase *)&frame_wnd)->set_bg_color(0);
    ((DibSurface *)surface_wnd)->wnd_create((iptr)&str_empty_92,0x800,0,0,0x280,0xf0,(iptr)&palette);
    ((DibSurface *)surface_wnd)->load_gif_resource(30000,10,0xec,(iptr)&palette);
    ((WndBase *)(surface_wnd + 0x48))->center();
    backup_surface.DibSurface::create(0x280,0xf0);
    ((DibSurface *)surface_wnd)->DibSurface::blit(&backup_surface,&text_rect,&text_rect);
    backup_surface.set_palette((iptr)&palette);
    ((WndBase *)&frame_wnd)->handle_show_raise();
    wnd_flush_paint();
    ((WndBase *)&frame_wnd)->set_palette((iptr)&palette);
    ((SurfaceWindow *)surface_wnd)->SurfaceWindow::show();
    wnd_flush_paint();
    ((WndBase *)(surface_wnd + 0x48))->set_capture();
    gfx_set_text_color_index(0xff);
    text_clear();
    text_append_year((int)civs[civ].spaceship_launch_year);
    text_set_string_arg(0,(const char *)&textbuf.buf);
    text_clear();
    text_append_int((int)civs[civ].spaceship_population);
    text_append((const char *)&str_0_000_5);
    text_set_string_arg(1,(const char *)&textbuf.buf);
    str = civ_get_adjective(civ);
    text_set_string_arg(2,(const char *)str);
    text_clear();
    text_append_year((int)civs[civ].spaceship_arrival_year);
    text_set_string_arg(3,(const char *)&textbuf.buf);
    str = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(4,(const char *)str);
    status = font2.font_cached_height();
    text_rect.top = text_rect.bottom + status * -2 >> 1;
    for (page = 0; page < 5; page++) {
      str_clear((char *)text_buf);
      text_clear();
      text_append((const char *)&str_centauri_beaten);
      text_append_int(page);
      status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&textbuf.buf);
      if (status == 0) {
        while (line = text_read_line(1), *line != '@') {
          str_append((char *)text_buf,(const char *)text_parse.line_cursor);
          str_append((char *)text_buf,(const char *)&str_empty_93);
        }
        text_clear();
        text_expand_vars((iptr)text_buf,(iptr)&textbuf.buf);
        backup_surface.DibSurface::blit((DibSurface *)surface_wnd,&text_rect,&text_rect);
        OffsetRect(&text_rect,1,1);
        gfx_set_text_color_index(0);
        ((DibSurface *)surface_wnd)->draw_text_rect((iptr)&font2,(const char *)&textbuf.buf,(iptr)&text_rect,0);
        OffsetRect(&text_rect,-1,-1);
        gfx_set_text_color_index(0xff);
        ((DibSurface *)surface_wnd)->draw_text_rect((iptr)&font2,(const char *)&textbuf.buf,(iptr)&text_rect,0);
        gfx_dissolve_blit((iptr)surface_wnd,(iptr)&text_rect,4,5);
        text_close_file();
        ui_delay_ticks(0x168);
      }
    }
    ((WndBase *)(surface_wnd + 0x48))->release_capture();
    ((WndBase *)(surface_wnd + 0x48))->hide();
    wnd_flush_paint();
    ((WndBase *)&frame_wnd)->set_palette((iptr)&game_palette_block.game_palette);
    ((WndBase *)&frame_wnd)->hide();
    wnd_flush_paint();
    dll.dll_handle_free();
  }
  return;
}

/* 004728c0 auto */
void *EndgameMovieWindow::arrival_movie_wnd_scalar_destruct(uint flags)

{
  arrival_movie_wnd_destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004a76f5 auto */
void __cdecl destroyed_civs_clear(void)

{
  iptr i;
  
  destroyed_civs.count[0] = 0;
  for (i = 0; i < 0xc; i++) {
    destroyed_civs.turns[i] = 0;
    (((i8 *)&destroyed_civs.killers))[i] = 0;
    (((i8 *)&destroyed_civs.names))[i * 0x18] = 0;
  }
  return;
}

/* 004bc480 auto */
i32 __cdecl council_military_advice(iptr civ,i32 era)

{
  int has;
  i32 result;
  int barracks_count;
  iptr unit;
  int air_strength[8];
  int city_count;
  iptr city2;
  iptr city;
  int land_rivals;
  iptr unit_type;
  int unit_count;
  int war_count;
  iptr civ2;
  int air_rivals;
  int land_strength[8];
  
  barracks_count = 0;
  city_count = 0;
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      has = city_has_improvement(city,IMP_BARRACKS);
      if (has != 0) {
        barracks_count++;
      }
      has = city_has_improvement(city,IMP_PALACE);
      if (has != 0) {
        city2 = city;
      }
      city_count++;
    }
  }
  if (city_count < 2) {
    city_count = 1;
  }
  unit_count = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == civ)) {
      unit_count++;
    }
  }
  if ((city_count - 1 + unit_count) / city_count < ((civs[civ].government < GOV_REPUBLIC) + 2)) {
    result = 1;
  }
  else {
    for (civ2 = 0; civ2 < 8; civ2++) {
      air_strength[civ2] = 0;
      land_strength[civ2] = air_strength[civ2];
    }
    for (civ2 = 1; civ2 < 8; civ2++) {
      has = civ_has_advance(civ2,ADV_SEAFARING);
      if (has != 0) {
        air_strength[civ2]++;
      }
      has = civ_has_advance(civ2,ADV_NUCLEAR_POWER);
      if (has != 0) {
        air_strength[civ2]++;
      }
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        has = civ_has_advance(civ2,(int)unit_types[unit_type].prerequisite);
        if (has != 0) {
          if (unit_types[unit_type].domain == DOMAIN_SEA) {
            air_strength[civ2]++;
          }
          else if ((unit_types[unit_type].role == ROLE_ATTACK) || (unit_types[unit_type].role == ROLE_DEFEND)) {
            land_strength[civ2]++;
          }
        }
      }
    }
    land_rivals = 0;
    air_rivals = 0;
    war_count = 0;
    for (civ2 = 1; civ2 < 8; civ2++) {
      if (civ != civ2) {
        if ((*(byte *)(civ2 * 4 + (iptr)((u8 *)&civs + 33) + civ * 0x594) & 0x20) != 0) {
          war_count++;
        }
        if (air_strength[civ] < air_strength[civ2]) {
          air_rivals++;
        }
        if (land_strength[civ] < land_strength[civ2]) {
          land_rivals++;
        }
      }
    }
    has = bits_count8((uint)game.civs_alive);
    if ((has - 1) / 2 < land_rivals) {
      result = 2;
    }
    else {
      has = bits_count8((uint)game.civs_alive);
      if ((has - 1) / 2 < air_rivals) {
        result = 3;
      }
      else {
        has = civ_has_advance(civ,ADV_MASONRY);
        if ((((has == 0) || (city2 < 0)) ||
            (has = city_has_improvement(city2,IMP_CITY_WALLS), has != 0)) ||
           (has = civ_has_wonder(civ,WONDER_GREAT_WALL), has != 0)) {
          if (barracks_count == 0) {
            result = 4;
          }
          else if ((war_count == 0) && (4 < game.power_rank[civ])) {
            result = 7;
          }
          else {
            result = 6;
          }
        }
        else {
          result = 5;
        }
      }
    }
  }
  return result;
}

/* 004bc8aa auto */
i32 __cdecl council_science_advice(iptr civ,int era)

{
  i32 result;
  int has;
  int city_count;
  iptr improvement;
  iptr city;
  int built_count;
  int rivals_ahead;
  iptr civ2;
  int route_total;
  
  rivals_ahead = 0;
  for (civ2 = 1; civ2 < 8; civ2++) {
    if ((civ2 != civ) && (civs[civ].num_advances <= civs[civ2].num_advances)) {
      rivals_ahead++;
    }
  }
  if (rivals_ahead == 0) {
    result = 7;
  }
  else {
    if (civs[civ].government < GOV_REPUBLIC) {
      if ((civs[civ].government != GOV_FUNDAMENTALISM) && (civs[civ].science_rate < 6)) {
        return 2;
      }
    }
    else if (civs[civ].science_rate < 4) {
      return 2;
    }
    city_count = 0;
    built_count = 0;
    route_total = (uint)civs[civ].units_active[UNIT_CARAVAN] + (uint)civs[civ].units_active[UNIT_FREIGHT];
    if (era == 0) {
      improvement = 6;
    }
    else if (era == 1) {
      improvement = 0xc;
    }
    else {
      improvement = 0x1a;
    }
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        city_count++;
        has = city_has_improvement(city,improvement);
        if (has != 0) {
          built_count++;
        }
        route_total += (char)cities[city].trade_routes;
      }
    }
    has = civ_has_advance(civ,(int)improvements[improvement].preq);
    if ((has == 0) || (city_count / 2 <= built_count)) {
      has = civ_has_advance(civ,ADV_TRADE);
      if ((has == 0) || ((int)(city_count + (city_count >> 0x1f & 3U)) >> 2 <= route_total)) {
        has = bits_count8((uint)game.civs_alive);
        if (rivals_ahead < has / 2) {
          result = 5;
        }
        else {
          result = 1;
        }
      }
      else {
        result = 4;
      }
    }
    else {
      result = 3;
    }
  }
  return result;
}

/* 004bcb9b auto */
i32 __cdecl council_trade_advice(iptr civ,int era)

{
  int value;
  i32 result;
  int built_count;
  int city_count;
  iptr improvement;
  iptr city;
  iptr needed_advance;
  int upkeep_total;
  int tax_total;
  int improvement_counts[39];
  iptr target_improvement;
  int ratio_divisor;
  int route_total;
  
  tax_total = 0;
  upkeep_total = 0;
  built_count = 0;
  city_count = 0;
  route_total = (uint)civs[civ].units_active[UNIT_CARAVAN] + (uint)civs[civ].units_active[UNIT_FREIGHT];
  if (era == 0) {
    needed_advance = 0x14;
    target_improvement = 5;
    ratio_divisor = 2;
  }
  else if (era == 1) {
    needed_advance = 6;
    target_improvement = 10;
    ratio_divisor = 3;
  }
  else {
    needed_advance = 0x16;
    target_improvement = 0x16;
    ratio_divisor = 4;
  }
  for (improvement = 0; improvement < 0x27; improvement++) {
    improvement_counts[improvement] = 0;
  }
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_count++;
      route_total += (char)cities[city].trade_routes;
      city_split_trade(city,(int)cities[city].trade,1,0);
      for (improvement = 0; improvement < 0x27; improvement++) {
        value = city_has_improvement(city,improvement);
        if (value != 0) {
          improvement_counts[improvement]++;
        }
      }
      value = city_has_improvement(city,target_improvement);
      if (value != 0) {
        built_count++;
      }
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        tax_total += cities[city].tax;
      }
    }
  }
  for (improvement = 0; improvement < 0x27; improvement++) {
    if ((improvement_counts[improvement] != 0) && (value = civ_improvement_upkeep(civ,improvement), value != 0)) {
      upkeep_total += improvement_counts[improvement] * value;
    }
  }
  if ((tax_total < upkeep_total) && (civs[civ].gold < 100)) {
    result = 1;
  }
  else {
    value = civ_has_advance(civ,needed_advance);
    if (value == 0) {
      result = 2;
    }
    else if (built_count < city_count / ratio_divisor) {
      result = 3;
    }
    else {
      value = civ_has_advance(civ,ADV_TRADE);
      if (value == 0) {
        result = 4;
      }
      else if (route_total < (int)(city_count + (city_count >> 0x1f & 3U)) >> 2) {
        result = 5;
      }
      else if (tax_total - upkeep_total < 6) {
        result = 6;
      }
      else {
        result = 7;
      }
    }
  }
  return result;
}

/* 004bcfcf auto */
i32 __cdecl council_foreign_advice(iptr civ,int era)

{
  int has;
  i32 result;
  int embassy_count;
  iptr needed_advance;
  int war_count;
  int alliance_count;
  int other_civ;
  int contact_count;
  
  war_count = 0;
  embassy_count = 0;
  alliance_count = 0;
  contact_count = 0;
  for (other_civ = 1; other_civ < 8; other_civ++) {
    if (other_civ != civ) {
      if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_CONTACT) != 0) {
        contact_count++;
      }
      if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) {
        alliance_count++;
      }
      if ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 33) + other_civ * 4) & 0x20) != 0) {
        war_count++;
      }
      if ((((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_EMBASSY) != 0) ||
          (has = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), has != 0)) ||
         (has = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has != 0)) {
        embassy_count++;
      }
    }
  }
  if (contact_count == 0) {
    result = 1;
  }
  else {
    if (era == 2) {
      needed_advance = 0x1b;
    }
    else {
      needed_advance = 0x58;
    }
    has = civ_has_advance(civ,needed_advance);
    if (has == 0) {
      result = 3;
    }
    else if ((((contact_count < 2) || (1 < civs[civ].reputation)) || (alliance_count != 0)) ||
            (((civs[civ].flags & CIVF_HAS_ALLIED) != 0 || (6 < game.power_rank[civ])))) {
      if (civs[civ].reputation < 3) {
        if ((war_count == contact_count) && (1 < civs[civ].reputation)) {
          result = 4;
        }
        else if ((civs[civ].flags & CIVF_TRADED_TECH) == 0) {
          result = 6;
        }
        else if ((embassy_count == 0) || ((war_count == contact_count && (embassy_count < contact_count)))) {
          result = 5;
        }
        else if (war_count == contact_count) {
          result = 6;
        }
        else {
          result = 7;
        }
      }
      else {
        result = 4;
      }
    }
    else {
      result = 2;
    }
  }
  return result;
}

/* 004bd2a3 auto */
i32 __cdecl council_attitude_advice(iptr civ,i32 era)

{
  bool rates_off;
  ushort recalc_flag;
  i32 result;
  iptr city;
  int content_count;
  int celebrate_count;
  int unhappy_count;
  int disorder_count;
  
  recalc_flag = game.transient_flags & TRANSIENT_ATTITUDE_DIRTY;
  game.transient_flags = game.transient_flags & ~TRANSIENT_ATTITUDE_DIRTY;
  content_count = 0;
  disorder_count = 0;
  celebrate_count = 0;
  unhappy_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      if ((recalc_flag != 0) && (GOV_FUNDAMENTALISM < civs[civ].government)) {
        city_recalc(city,1);
      }
      if ((char)cities[city].happy < (char)cities[city].unhappy) {
        unhappy_count++;
        if ((cities[city].flags & CITYF_DISORDER) != 0) {
          disorder_count++;
        }
      }
      else if (cities[city].happy == cities[city].unhappy) {
        content_count++;
      }
      if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
        celebrate_count++;
      }
    }
  }
  if (civs[civ].government < GOV_REPUBLIC) {
    rates_off = true;
    if ((((unhappy_count == 0) && (content_count != 0)) && (celebrate_count == 0)) &&
       ((uint)civs[civ].science_rate + (uint)civs[civ].tax_rate == 10)) {
      rates_off = false;
    }
  }
  else {
    rates_off = (uint)civs[civ].science_rate + (uint)civs[civ].tax_rate < 9;
  }
  if (unhappy_count == 0) {
    if (rates_off) {
      if (celebrate_count == 0) {
        result = 5;
      }
      else {
        result = 6;
      }
    }
    else {
      result = 4;
    }
  }
  else if (rates_off) {
    if ((disorder_count == 0) || (civs[civ].government != GOV_DEMOCRACY)) {
      result = 3;
    }
    else {
      result = 1;
    }
  }
  else {
    result = 2;
  }
  return result;
}

/* 0050a2f7 auto */
i32 __cdecl
foreign_minister_draw_civ_city(i32 ctx,iptr surface,int civ,byte flags,int x,int y,
            int height)

{
  int value;
  iptr city2;
  iptr city;
  int draw_x;
  int best_score;
  int score;
  
  best_score = 0;
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      score = (int)cities[city].size;
      value = city_has_improvement(city,IMP_PALACE);
      if (value != 0) {
        score += 200;
      }
      if (cities[city].building == -IMP_PALACE) {
        score += 100;
      }
      if (best_score < score) {
        best_score = score;
        city2 = city;
      }
    }
  }
  if (-1 < city2) {
    draw_x = x;
    if ((flags & 1) != 0) {
      draw_x = x + 0x26;
    }
    value = gfx_zoom_scale(0x30,0xfffffffe);
    gfx_draw_city(surface,city2,0,draw_x,y - (value - height) / 2,0xfffffffe);
  }
  return 0;
}

/* 00514e10 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl destroyed_civs_text_bg_global_init(void)

{
  destroyed_civs_text_bg_static_init();
  destroyed_civs_text_bg_reg_atexit();
  return;
}

/* 00514e2a auto */
void __cdecl destroyed_civs_text_bg_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&destroyed_civs_text_bg));
  return;
}

/* 00514e44 auto */
void __cdecl destroyed_civs_text_bg_reg_atexit(void)

{
  _atexit(destroyed_civs_text_bg_atexit_dtor);
  return;
}

/* 00514e61 auto */
void __cdecl destroyed_civs_text_bg_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&destroyed_civs_text_bg));
  return;
}

/* 00514e7b auto */
void __cdecl destroyed_civs_show(iptr killer_civ)

{
  int created;
  i8 window[1808];
  
  ((DestroyedCivsWindow *)window)->construct();
  ON_SCOPE_EXIT(((DestroyedCivsWindow *)&window)->destruct());
  created = ((DestroyedCivsWindow *)window)->create(killer_civ);
  if (created != 0) {
    ((DestroyedCivsWindow *)window)->run();
  }
  return;
}

/* 00514f16 auto */
DestroyedCivsWindow *DestroyedCivsWindow::construct()

{
  
  this->ModalMenuWindow::construct();
  ((GamePalette *)&control_list)->init();
  ((SurfaceWindow *)media_wnd)->SurfaceWindow::construct();
  surface_ctor((DibSurface *)civs_art_surface);
  ((DllHandle *)improvement_art_surface)->init();
  sprite_init((Sprite *)panel_sprite);
  sprite_init((Sprite *)portrait_sprite);
  ((GameFont *)caption_font_2)->init_empty();
  killer_civ = 0;
  (*(DestroyedCivsWindow * *)&victory_screen_wnd) = this;
  slide_y = 0;
  SetRect((LPRECT)&panel_clip_rect,0xd0,0x39,0x1b1,0x14c);
  ((GameFont *)caption_font_2)->set_styled(0,0x18,1);
  return this;
}

/* 005150b9 auto */
void DestroyedCivsWindow::destruct()

{
  int try_level;
  
  try_level = try_level & 0xff;
  (*((char *)&try_level + 0)) = 7;
  ((DllHandle *)improvement_art_surface)->dll_handle_free();
  victory_screen_wnd = 0;
  (*((char *)&try_level + 0)) = 6;
  /* inlined unwind funclet 0051516a */ { ((GameFont *)(((int)this) + 0x700))->destroy(); }
  (*((char *)&try_level + 0)) = 5;
  /* inlined unwind funclet 00515179 */ { sprite_destroy((Sprite *)(((int)this) + 0x688)); }
  (*((char *)&try_level + 0)) = 4;
  /* inlined unwind funclet 00515188 */ { sprite_destroy((Sprite *)(((int)this) + 0x64c)); }
  (*((char *)&try_level + 0)) = 3;
  /* inlined unwind funclet 00515197 */ { ((DllHandle *)(((int)this) + 0x648))->free(); }
  (*((char *)&try_level + 0)) = 2;
  /* inlined unwind funclet 005151a6 */ { surface_dtor((DibSurface *)(((int)this) + 0x600)); }
  (*((char *)&try_level + 0)) = 1;
  /* inlined unwind funclet 005151b5 */ { ((SurfaceWindow *)(((int)this) + 0x4ec))->SurfaceWindow::destruct(); }
  try_level = try_level & 0xffffff00;
  /* inlined unwind funclet 005151c4 */ { ((GamePalette *)(((int)this) + 0xb8))->destroy(); }
  try_level = 0xffffffff;
  /* inlined unwind funclet 005151d3 */ { ModalMenuWindow::destruct(); }
  return;
}

/* 005151f4 auto */
i32 DestroyedCivsWindow::create(uint killer)

{
  int found;
  i32 result;
  i32 width;
  iptr entry;
  i8 dll_path[128];
  RECT screen_bounds;
  tagRECT rect;
  
  gfx_get_screen_rect((iptr)&screen_bounds);
  str_clear((char *)dll_path);
  str_append((char *)dll_path,(const char *)&str_civ2_mk_dll_2);
  found = file_resolve_path((const char *)dll_path);
  if (found == 0) {
    result = 0;
  }
  else {
    ((DllHandle *)improvement_art_surface)->load((const char *)dll_path);
    killer_civ = killer;
    create_with_handlers((const char *)&str_empty_156,0x800,screen_bounds.left,screen_bounds.top,screen_bounds.right - screen_bounds.left,
               (screen_bounds.bottom - screen_bounds.top) + 5,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
    set_bg_color(0);
    ((DibSurface *)media_wnd)->wnd_create_with_surface((const char *)&str_empty_157,0x800,0,0,0x280,0x1e0,
               (iptr)&control_list,(iptr)this);
    ((WndBase *)movie_wnd_base)->center();
    ((DibSurface *)civs_art_surface)->load_gif_resource(0x2711,10,0xec,(iptr)&control_list);
    ((Sprite *)panel_sprite)->grab_from_surface((iptr)(civs_art_surface),8,0,0,0x129,0xad
              );
    ((DibSurface *)civs_art_surface)->load_gif_resource(10000,10,0xec,(iptr)&control_list);
    SetRect(&rect,0,0,0x280,0x1e0);
    ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),&rect
               ,&rect);
    SetRect((LPRECT)slot_sprites,0xc1,0x173,0x1c0,0x1cd);
    result = ((DibSurface *)civs_art_surface)->surface_height();
    width = ((DibSurface *)civs_art_surface)->surface_width();
    ((DibSurface *)((i8 *)&destroyed_civs_text_bg))->DibSurface::create(width,result);
    ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)&destroyed_civs_text_bg,(RECT *)slot_sprites,
               (RECT *)slot_sprites);
    for (entry = 0;
        (entry < 0xc && ((uint)(byte)(((i8 *)&destroyed_civs.killers))[entry] != (killer & 0xff)));
        entry = entry + 1) {
    }
    destroyed_civs_show_date = 0;
    draw_entry(entry);
    slide_y = 0x133;
    SetRect((LPRECT)&slide_rect,0xab,slide_y,0x1d4,slide_y + 0xad);
    slide_step(0);
    result = 1;
  }
  return result;
}

/* 00515516 auto */
void DestroyedCivsWindow::run()

{
  DWORD start_tick;
  int status;
  char *line;
  i32 width;
  iptr entry;
  i8 text_buf[256];
  DWORD now;
  GameFont font2;
  
  font2.init_empty();
  ON_SCOPE_EXIT(font2.destroy());
  handle_show_raise();
  wnd_flush_paint();
  set_palette((iptr)&control_list);
  ((SurfaceWindow *)media_wnd)->SurfaceWindow::show();
  wnd_flush_paint();
  ((WndBase *)movie_wnd_base)->set_capture();
  for (entry = 0; entry < 0xc; entry++) {
    if ((((i8 *)&destroyed_civs.killers))[entry] == (char)killer_civ) {
      draw_entry(entry);
      destroyed_civs_show_date = 1;
      draw_portrait(entry);
      start_tick = timeGetTime();
      snd_play_effect(((uint)(byte)(((i8 *)&destroyed_civs.portraits))[entry] + (uint)map.seed & 7) + 0x53,1,0,0)
      ;
      slide_up();
      ui_delay_ticks(0x3c);
      do {
        win_flush_pending_msgs();
        now = timeGetTime();
        if (2 < game.multiplayer) {
          net.process_messages(1,0);
        }
      } while ((int)(now - start_tick) < 7000);
      draw_entry(entry);
      slide_down();
    }
  }
  ui_delay_ticks(0x3c);
  ((DibSurface *)media_wnd)->fill(0);
  SetRect((LPRECT)slot_sprites,0,0,0x280,0x1e0);
  gfx_dissolve_blit((iptr)(media_wnd),(iptr)slot_sprites,4,4);
  ((DibSurface *)media_wnd)->load_gif_resource(0x2712,10,0xec,(iptr)&control_list);
  text_set_font((iptr)caption_font_2);
  text_set_colors(0x7c,0x81,1,1);
  text_clear();
  str_clear((char *)text_buf);
  status = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_slam);
  if (status == 0) {
    while (line = text_read_line(1), *line != '@') {
      str_append((char *)text_buf,(const char *)text_parse.line_cursor);
      str_append((char *)text_buf,(const char *)&str_empty_158);
    }
    text_draw_centered((iptr)(media_wnd),(const char *)text_buf,0,0x48,0x280);
  }
  gfx_dissolve_blit((iptr)(media_wnd),(iptr)slot_sprites,4,3);
  text_clear();
  text_append_label(0x1c8);
  font2.set_styled(0,0x10,3);
  text_set_font((iptr)&font2);
  width = 0x280;
  status = font2.line_spacing();
  text_draw_centered((iptr)(media_wnd),(const char *)&textbuf.buf,0,0x1dc - status,width);
  ((DibSurface *)media_wnd)->wnd_blit_surface_rect((iptr)slot_sprites);
  ((WndHandlers *)destroyed_civs_handlers)->set_keydown_handler(victory_screen_on_key);
  ((WndHandlers *)destroyed_civs_handlers)->set_lbuttonup_handler((WndMouseFn)victory_screen_on_click);
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)movie_wnd_base)->run_modal_loop();
  ((ModalMenuWindow *)movie_wnd_base)->set_modal_idle_callback(0);
  ((WndBase *)movie_wnd_base)->release_capture();
  ((WndBase *)movie_wnd_base)->hide();
  wnd_flush_paint();
  set_palette((iptr)&game_palette_block.game_palette);
  hide();
  wnd_flush_paint();
  return;
}

/* 00515999 auto */
void DestroyedCivsWindow::draw_entry(iptr entry)

{
  iptr label;
  i32 year;
  
  if (destroyed_civs_show_date == 0) {
    ((DibSurface *)((i8 *)&destroyed_civs_text_bg))->DibSurface::blit((DibSurface *)(iptr)((u8 *)this + 0x600),(RECT *)&slot_sprites,
               (RECT *)&slot_sprites);
    text_clear();
    text_append((const char *)(((i8 *)&destroyed_civs.names) + entry * 0x18));
    gfx_set_text_color_index(0x21);
    OffsetRect((LPRECT)&slot_sprites,1,1);
    ((DibSurface *)((u8 *)this + 0x600))->draw_text_rect((iptr)((u8 *)this + 0x700),(const char *)&textbuf.buf,(iptr)&slot_sprites
               ,0);
    gfx_set_text_color_index(0x10);
    OffsetRect((LPRECT)&slot_sprites,-1,-1);
    ((DibSurface *)((u8 *)this + 0x600))->draw_text_rect((iptr)((u8 *)this + 0x700),(const char *)&textbuf.buf,(iptr)&slot_sprites
               ,0);
  }
  else {
    ((DibSurface *)((i8 *)&destroyed_civs_text_bg))->DibSurface::blit((DibSurface *)(iptr)((u8 *)this + 0x600),(RECT *)&slot_sprites,
               (RECT *)&slot_sprites);
    text_clear();
    text_append((const char *)(((i8 *)&destroyed_civs.names) + entry * 0x18));
    text_append_colon();
    text_append_space();
    label = (iptr)label_string(label_table.string_ids[0x1c7]);
    text_append((const char *)label);
    text_append_space();
    year = game_turn_to_year((int)destroyed_civs.turns[entry]);
    text_append_year(year);
    gfx_set_text_color_index(0x21);
    OffsetRect((LPRECT)&slot_sprites,1,1);
    ((DibSurface *)((u8 *)this + 0x600))->draw_text_rect((iptr)((u8 *)this + 0x700),(const char *)&textbuf.buf,(iptr)&slot_sprites
               ,0);
    gfx_set_text_color_index(0x10);
    OffsetRect((LPRECT)&slot_sprites,-1,-1);
    ((DibSurface *)((u8 *)this + 0x600))->draw_text_rect((iptr)((u8 *)this + 0x700),(const char *)&textbuf.buf,(iptr)&slot_sprites
               ,0);
    ((DibSurface *)((u8 *)this + 0x600))->DibSurface::blit((DibSurface *)&media_wnd,(RECT *)&slot_sprites,
               (RECT *)&slot_sprites);
    ((DibSurface *)&media_wnd)->wnd_blit_surface_rect((iptr)&slot_sprites);
    destroyed_civs_show_date = 0;
  }
  ((DibSurface *)((u8 *)this + 0x600))->DibSurface::blit((DibSurface *)&media_wnd,(RECT *)&slot_sprites,
             (RECT *)&slot_sprites);
  return;
}

/* 00515c15 auto */
void DestroyedCivsWindow::draw_portrait(iptr entry)

{
  i8 draw_tmp[16];
  DibSurface portrait_surface;
  i8 rgb_buf[192];
  GamePalette palette;
  
  surface_ctor(&portrait_surface);
  ON_SCOPE_EXIT(surface_dtor(&portrait_surface));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)(iptr)((u8 *)&media_wnd),
             (RECT *)&panel_clip_rect,(RECT *)&panel_clip_rect);
  portrait_surface.load_gif_resource((byte)(((i8 *)&destroyed_civs.portraits))[entry] + 0xdc,10,0xec,(iptr)&palette);
  palette.get_rgb((iptr)rgb_buf,0x2a,0x40);
  ((GamePalette *)&control_list)->set_rgb(0x2a,0x40,(iptr)rgb_buf);
  portrait_surface.set_palette((iptr)&control_list);
  ((Sprite *)portrait_sprite)->grab_from_surface((iptr)&portrait_surface,9,1,1,0xe1,0x113);
  ((Sprite *)portrait_sprite)->draw((iptr)draw_tmp,(iptr)((u8 *)&media_wnd),
             0xd0,0x39);
  SetRect((LPRECT)&panel_clip_rect,0xd0,0x39,0x1b1,0x14c);
  return;
}

/* 00515dc8 auto */
void DestroyedCivsWindow::slide_step(int delta)

{
  i8 draw_tmp2[16];
  i8 draw_tmp1[16];
  struct { u32 left; u32 top; u32 right; int bottom; } gap_rect;
  struct { u32 left; u32 top; u32 right; u32 bottom; } dirty_rect;
  
  dirty_rect.left = slide_rect;
  dirty_rect.top = slide_top;
  dirty_rect.right = slide_right;
  dirty_rect.bottom = resource_dll;
  if (delta < 0) {
    dirty_rect.top -= delta;
  }
  else {
    dirty_rect.bottom += delta;
  }
  ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),(RECT *)&dirty_rect,
             (RECT *)&dirty_rect);
  ((Sprite *)portrait_sprite)->draw((iptr)draw_tmp1,(iptr)(media_wnd),
             0xd0,0x39);
  if (0 < delta) {
    gap_rect.left = panel_clip_rect;
    gap_rect.right = panel_clip_right;
    gap_rect.top = slide_y;
    gap_rect.bottom = slide_y + delta;
    ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),(RECT *)&gap_rect
               ,(RECT *)&gap_rect);
  }
  ((Sprite *)panel_sprite)->draw((iptr)draw_tmp2,(iptr)(media_wnd),
             0xab,slide_y + delta);
  OffsetRect((LPRECT)&slide_rect,0,delta);
  if ((int)dirty_rect.top < 1) {
    dirty_rect.top = 0;
  }
  ((DibSurface *)media_wnd)->wnd_blit_surface_rect((iptr)&dirty_rect);
  slide_y += delta;
  return;
}

/* 00515f3c auto */
void DestroyedCivsWindow::slide_down()

{
  DWORD tick;
  DWORD start_time;
  
  ((DibSurface *)civs_art_surface)->DibSurface::blit((DibSurface *)(iptr)(media_wnd),
             (RECT *)&panel_clip_rect,(RECT *)&panel_clip_rect);
  snd_wave_stop_all();
  snd_play_effect(SND_GUILLOTN,1,0,0);
  tick = timeGetTime();
  while (start_time = tick, (int)slide_y < 0x133) {
    slide_step(0xf);
    do {
      win_flush_pending_msgs();
      tick = timeGetTime();
      if (2 < game.multiplayer) {
        net.process_messages(1,0);
      }
    } while ((int)(tick - start_time) < 0x16);
  }
  return;
}

/* 00516005 auto */
void DestroyedCivsWindow::slide_up()

{
  while (-0xad < (int)slide_y) {
    win_flush_pending_msgs();
    slide_step(0xfffffffc);
  }
  return;
}

/* 00516042 auto */
void __cdecl victory_screen_on_click(void)

{
  ((ModalMenuWindow *)(victory_screen_wnd + 0x534))->end_modal_loop();
  return;
}

/* 00516063 auto */
void __cdecl victory_screen_on_key(int key)

{
  if ((0xcf < key) && (key < 0xd3)) {
    ((ModalMenuWindow *)(victory_screen_wnd + 0x534))->end_modal_loop();
  }
  return;
}

/* 00516570 auto */
void __cdecl council_show(iptr civ,i32 cheat_mode)

{
  int ok;
  i32 ret_addr;
  CouncilWindow council;
  
  council_state.active = 1;
  council.construct();
  ON_SCOPE_EXIT(council.destruct());
  snd_cd_stop();
  ret_addr = 0x5165c6;
  ok = council.init(civ,cheat_mode);
  if (ok != 0) {
    council.run_modal();
  }
  snd_sync_music_option();
  council_state.active = 0;
  return;
}

/* 0051661a auto */
CouncilWindow *CouncilWindow::construct()

{
  
  this->SurfaceWindow::construct();
  video.construct();
  dialog.Dialog::construct(0x4000);
  dll.init();
  button_ctor((Button *)((u8 *)this + 0x11c0));
  button_ctor((Button *)&advisor_button_4);
  button_ctor((Button *)((u8 *)&advisor_button_3));
  button_ctor((Button *)((u8 *)&advisor_button_2));
  button_ctor((Button *)((u8 *)&advisor_button_0));
  button_ctor((Button *)&council_close_button);
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 6);
  speech_line_y = 0xb0;
  council_active_2 = 0;
  /* B29: the first advisor button clears the speech by this index, which the
     original never sets (every mode: ours would read another stale value;
     docs/BUGS.md) */
  speech_advisor_index = 0;
  council_state.wnd = this;
  return this;
}

/* 005167d9 auto */
void CouncilWindow::destruct()

{
  int try_level;
  
  *(pointer *)&vtable = ((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 6);
  try_level = try_level & 0xff;
  council_state.wnd = 0;
  (*((char *)&try_level + 0)) = 8;
  /* inlined unwind funclet 0051689f */ { ctrl_dtor((Button *)(((int)this) + 0x12ec)); }
  (*((char *)&try_level + 0)) = 7;
  /* inlined unwind funclet 005168ae */ { ctrl_dtor((Button *)(((int)this) + 0x12b0)); }
  (*((char *)&try_level + 0)) = 6;
  /* inlined unwind funclet 005168bd */ { ctrl_dtor((Button *)(((int)this) + 0x1274)); }
  (*((char *)&try_level + 0)) = 5;
  /* inlined unwind funclet 005168cc */ { ctrl_dtor((Button *)(((int)this) + 0x1238)); }
  (*((char *)&try_level + 0)) = 4;
  /* inlined unwind funclet 005168db */ { ctrl_dtor((Button *)(((int)this) + 0x11fc)); }
  (*((char *)&try_level + 0)) = 3;
  /* inlined unwind funclet 005168ea */ { ctrl_dtor((Button *)(((int)this) + 0x11c0)); }
  (*((char *)&try_level + 0)) = 2;
  /* inlined unwind funclet 005168f9 */ { ((DllHandle *)(((int)this) + 0xe30))->free(); }
  (*((char *)&try_level + 0)) = 1;
  /* inlined unwind funclet 00516908 */ { ((Dialog *)(((int)this) + 0xb3c))->Dialog::destruct(); }
  try_level = try_level & 0xffffff00;
  /* inlined unwind funclet 00516917 */ { ((AviPlayer *)(((int)this) + 0x114))->destruct(); }
  try_level = 0xffffffff;
  /* inlined unwind funclet 00516926 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 00516947 auto */
i32 CouncilWindow::init(u32 civ,int cheat_mode)

{
  bool valid;
  i32 result;
  int cancelled;
  int choice;
  int advisor;
  int number;
  i8 avi_path[128];
  int open_err;
  
  council_civ = civ;
  result = civ_city_art_era(council_civ);
  council_era = result;
  wnd_create_with_surface((const char *)&str_empty_159,0xc02,0,0,ui_metrics.border_width * 6 + 0x1c2,
             ui_metrics.caption_height * 2 + ui_metrics.bevel_width + 0xfc,(iptr)&art_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
  ((WndBase *)&window)->set_caption_height(ui_metrics.caption_height);
  ui_set_button_font((iptr)&dialog_plain_font_obj_block.dialog_plain_font_obj);
  create_buttons();
  draw_background();
  repaint();
  for (advisor = 0; advisor < 5; advisor++) {
    result = council_advisor_opinion(council_civ,advisor);
    council_settings[advisor + 1] = result;
  }
  if (cheat_mode != 0) {
    while( true ) {
      cancelled = popup_game((const char *)&str_councilcheat0);
      choice = dlg_input.help_pressed;
      if ((cancelled != 0) || (dlg_input.help_pressed == 0)) break;
      if (dlg_input.help_pressed == 6) {
        council_active_2 = 1;
        break;
      }
      valid = false;
      while (!valid) {
        cancelled = dlg_input_number_game((const char *)&str_councilcheat1,0,(iptr)&number);
        if (cancelled != 0) {
          return 0;
        }
        if (((0 < number) && (number < 8)) && ((choice != 5 || (number < 7)))) {
          council_settings[choice] = number;
          valid = true;
        }
      }
    }
    valid = false;
    while ((!valid && (choice = dlg_input_number_game((const char *)&str_councilcheat2,0,(iptr)&number), choice == 0))) {
      if ((-1 < number) && (number < 3)) {
        council_era = number;
        valid = true;
      }
    }
  }
  if (civs[council_civ].government == GOV_ANARCHY) {
    council_active_2 = 1;
  }
  else {
    for (advisor = 0; advisor < 5; advisor++) {
      str_clear((char *)(advisor_code_slots + advisor * 3 - 0x40));
    }
    str_append((char *)&advisor_codes,(const char *)&str_m_2);
    str_append((char *)((int)&advisor_codes + 3),(const char *)&str_s);
    str_append((char *)((int)&advisor_codes_part2 + 2),(const char *)&str_t);
    str_append((char *)((int)&advisor_codes_part3 + 1),(const char *)&str_f);
    str_append((char *)&advisor_codes_part4,(const char *)&str_a_2);
    for (advisor = 0; advisor < 5; advisor++) {
      str_append_int((iptr)(advisor_code_slots + advisor * 3 - 0x40),
                         council_settings[advisor + 1]);
    }
  }
  if (council_active_2 != 0) {
    ((Button *)((u8 *)this + 0x11c0))->disable();
    ((Button *)&advisor_button_4)->disable();
    ((Button *)((u8 *)&advisor_button_3))->disable();
    ((Button *)((u8 *)&advisor_button_2))->disable();
    ((Button *)((u8 *)&advisor_button_0))->disable();
  }
  str_clear((char *)council_text_file);
  text_clear();
  if (council_active_2 == 0) {
    text_append((const char *)&str_council);
  }
  else {
    text_append((const char *)&str_anarchy);
  }
  text_append_int(council_era);
  str_append((char *)council_text_file,(const char *)&textbuf.buf);
  if ((council_active_2 == 0) &&
     (choice = load_speech(0), choice == 0)) {
    result = 0;
  }
  else {
    dialog.init((iptr)this,(iptr)this,
               (RECT *)(iptr)((u8 *)&speech_rect),0x84008);
    video.create_child((iptr)&str_empty_160,0x200,0,0,(iptr)this);
    str_clear((char *)avi_path);
    str_append((char *)avi_path,(const char *)&str_civ2_video);
    str_append((char *)avi_path,(const char *)council_text_file);
    str_append((char *)avi_path,(const char *)&str_avi_3);
    choice = file_resolve_path((const char *)avi_path);
    if (choice == 0) {
      result = 0;
    }
    else {
      open_err = ((AviPlayer *)&video_head)->open((const char *)avi_path);
      if (open_err == 0) {
        ((DibSurface *)&video_head)->avi_player_set_palette((iptr)&art_palette);
        ((WndBase *)&video_window_head)->set_palette((iptr)&art_palette);
        ((AviPlayer *)&video_head)->set_end_callback(council_on_video_end);
        draw_advisor_frames();
        result = 1;
      }
      else {
        if (open_err == -0x7ffbfeac) {
          popup_game((const char *)&str_vfwnotregistered_7);
        }
        result = 0;
      }
    }
  }
  return result;
}

/* 00516fd4 auto */
void CouncilWindow::run_modal()

{
  dlg_set_palette((iptr)&art_palette);
  screen_fade_out();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&art_palette);
  ((WndBase *)&window)->center();
  ((Control *)&advisor_button_4)->show_and_redraw();
  ((Control *)((u8 *)&advisor_button_3))->show_and_redraw();
  ((Control *)((u8 *)&advisor_button_0))->show_and_redraw();
  ((Control *)((u8 *)&advisor_button_2))->show_and_redraw();
  ((Control *)((u8 *)this + 0x11c0))->show_and_redraw();
  ((Control *)&council_close_button)->show_and_redraw();
  dialog.show_modal(0);
  this->SurfaceWindow::show();
  wnd_flush_paint();
  if (council_active_2 != 0) {
    ((WndBase *)&video_window_head)->move_xy(ui_metrics.border_width + 1,ui_metrics.caption_height + 1);
    ((SurfaceWindow *)&video_head)->SurfaceWindow::show();
    wnd_flush_paint();
    ((AviPlayer *)&video_head)->play();
  }
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)&window)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)&window)->run_modal_loop();
  ((ModalMenuWindow *)&window)->set_modal_idle_callback(0);
  snd_wave_stop_all();
  draw_background();
  ((WndBase *)&window)->hide();
  wnd_flush_paint();
  dialog.close();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
  screen_fade_in();
  dlg_set_palette((iptr)&game_palette_block.game_palette);
  return;
}

/* 00517158 auto */
void CouncilWindow::draw_background()

{
  int extent;
  i32 size;
  int offset;
  int inner_top;
  RECT *lprcSrc;
  iptr str;
  int i;
  int panel_x;
  int j;
  tagRECT rect;
  
  extent = ui_metrics.caption_height + ui_metrics.bevel_width * -2;
  size = get_width();
  gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,0,ui_metrics.bevel_width,size,extent,0,ui_metrics.bevel_width);
  offset = ui_metrics.bevel_width + ui_metrics.caption_height + 0x7a;
  gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,0,offset,size,extent,0,offset);
  extent = get_height();
  extent -= (ui_metrics.bevel_width + 0x1e);
  gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,0,extent,size,0x1e,0,extent);
  size = get_height();
  offset = ui_metrics.border_width + ui_metrics.bevel_width * -2;
  gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,ui_metrics.bevel_width,0,offset,size,ui_metrics.bevel_width,0);
  extent = get_width();
  extent -= (ui_metrics.bevel_width + offset);
  gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,extent,0,offset,size,extent,0);
  extent = ui_metrics.bevel_width * 2;
  panel_x = ui_metrics.bevel_width + ui_metrics.border_width + 0x5a;
  inner_top = ui_metrics.caption_height - ui_metrics.bevel_width;
  for (i = 0; i < 4; i++) {
    gfx_tile_fill((iptr)this,(iptr)&frame_texture_surface_block.frame_texture_surface,panel_x,inner_top,offset,extent + 0x7a,panel_x,inner_top);
    panel_x = panel_x + ui_metrics.border_width + 0x5a;
  }
  SetRect(&rect,ui_metrics.border_width - ui_metrics.bevel_width,inner_top,
          ui_metrics.bevel_width * 2 + (ui_metrics.border_width - ui_metrics.bevel_width) + 0x5a,ui_metrics.bevel_width * 2 + inner_top + 0x7a);
  for (i = 0; i < 5; i++) {
    for (j = 0; j < ui_metrics.bevel_width; j++) {
      gfx_draw_bevel_rect((iptr)this,&rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
      InflateRect(&rect,-1,-1);
    }
    fill_rect((iptr)&rect,10);
    InflateRect(&rect,ui_metrics.bevel_width,ui_metrics.bevel_width);
    OffsetRect(&rect,ui_metrics.border_width + 0x5a,0);
  }
  extent = get_height();
  extent = (extent - 0x1e) - ui_metrics.bevel_width;
  offset = get_width();
  offset = (offset - ui_metrics.border_width) + ui_metrics.bevel_width;
  inner_top = get_height();
  SetRect((LPRECT)&speech_rect,ui_metrics.border_width - ui_metrics.bevel_width,inner_top - 0x82 + ui_metrics.bevel_width * -3
          ,offset,extent);
  for (j = 0; j < ui_metrics.bevel_width; j++) {
    gfx_draw_bevel_rect((iptr)this,(RECT *)&speech_rect,ui_metrics.shadow_color,ui_metrics.highlight_color);
    InflateRect((LPRECT)&speech_rect,-1,-1);
  }
  fill_rect((iptr)&speech_rect,0x1d);
  InflateRect((LPRECT)&speech_rect,-2,-2);
  lprcSrc = (RECT *)get_rect();
  CopyRect(&rect,lprcSrc);
  for (j = 0; j < ui_metrics.bevel_width; j++) {
    gfx_draw_bevel_rect((iptr)this,&rect,ui_metrics.highlight_color,ui_metrics.shadow_color);
    InflateRect(&rect,-1,-1);
  }
  text_clear();
  if (game_language == 1) {
    str = (iptr)label_string(label_table.string_ids[0x12b]);
    text_append((const char *)str);
    text_append_space();
    str = civ_get_adjective(council_civ);
    text_append((const char *)str);
  }
  else {
    str = civ_get_adjective(council_civ);
    text_append((const char *)str);
    if (game_language == 2) {
      text_append((const char *)&str_er_2);
    }
    text_append_space();
    str = (iptr)label_string(label_table.string_ids[0x12b]);
    text_append((const char *)str);
  }
  offset = get_width();
  inner_top = ((GameFont *)((i8 *)&caption_font_block.caption_font))->text_width((const char *)&textbuf.buf);
  extent = ui_metrics.bevel_width;
  offset = (offset - inner_top) / 2;
  inner_top = ui_metrics.bevel_width + 1;
  gfx_set_text_color_index(10);
  draw_text((iptr)&caption_font_block.caption_font,(const char *)&textbuf.buf,offset + 1,extent + 2,5);
  gfx_set_text_color_index(0x1a);
  draw_text((iptr)&caption_font_block.caption_font,(const char *)&textbuf.buf,offset,inner_top,5);
  return;
}

/* 00517673 auto */
void CouncilWindow::create_buttons()

{
  int y;
  iptr label;
  int right;
  int height;
  Wnd *parent6;
  Wnd *parent5;
  Wnd *parent4;
  Wnd *parent3;
  Wnd *parent2;
  Wnd *parent1;
  tagRECT rect;
  
  y = (ui_metrics.caption_height - 0x16 >> 1) + ui_metrics.caption_height;
  SetRect(&rect,ui_metrics.border_width + 7,y + 0x79,ui_metrics.border_width + 0x52,y + 0x8f);
  if (this == (CouncilWindow *)0x0) {
    parent1 = (Wnd *)0x0;
  }
  else {
    parent1 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0x9b]);
  ((Button *)&advisor_button_0)->create_with_callback((iptr)parent1,0,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&advisor_button_0)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  OffsetRect(&rect,ui_metrics.border_width + 0x5a,0);
  if (this == (CouncilWindow *)0x0) {
    parent2 = (Wnd *)0x0;
  }
  else {
    parent2 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0x5b]);
  ((Button *)&advisor_button_1)->create_with_callback((iptr)parent2,1,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&advisor_button_1)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  OffsetRect(&rect,ui_metrics.border_width + 0x5a,0);
  if (this == (CouncilWindow *)0x0) {
    parent3 = (Wnd *)0x0;
  }
  else {
    parent3 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0x45]);
  ((Button *)&advisor_button_2)->create_with_callback((iptr)parent3,2,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&advisor_button_2)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  OffsetRect(&rect,ui_metrics.border_width + 0x5a,0);
  if (this == (CouncilWindow *)0x0) {
    parent4 = (Wnd *)0x0;
  }
  else {
    parent4 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1b8]);
  ((Button *)&advisor_button_3)->create_with_callback((iptr)parent4,3,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&advisor_button_3)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  OffsetRect(&rect,ui_metrics.border_width + 0x5a,0);
  if (this == (CouncilWindow *)0x0) {
    parent5 = (Wnd *)0x0;
  }
  else {
    parent5 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1b9]);
  ((Button *)&advisor_button_4)->create_with_callback((iptr)parent5,4,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&advisor_button_4)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  y = get_height();
  y = (y - ui_metrics.bevel_width) - 1;
  right = get_width();
  right -= ui_metrics.border_width;
  height = get_height();
  SetRect(&rect,ui_metrics.border_width,height - 0x1d,right,y);
  if (this == (CouncilWindow *)0x0) {
    parent6 = (Wnd *)0x0;
  }
  else {
    parent6 = &window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  ((Button *)&council_close_button)->create_with_callback((iptr)parent6,0xca,(iptr)&rect,0x21,(const char *)label);
  ((Button *)&council_close_button)->set_callback((CtrlNotifyFn)council_on_advisor_button);
  return;
}

/* 005179a3 auto */
i32 CouncilWindow::load_speech(uptr advisor)

{
  bool skip_read;
  bool matched;
  int status;
  char *line;
  u32 value;
  i8 *token;
  int roll;
  iptr i;
  iptr line_idx;
  uint used_mask;
  i32 result;
  
  used_mask = 0;
  result = 0;
  if (4 < (int)advisor) {
    return 0;
  }
  status = text_open_section((const char *)((u8 *)this + 0xe34),(const char *)((int)&advisor_codes + advisor * 3));
  if (status != 0) {
    return 0;
  }
  line = text_read_line(1);
  if (line != 0) {
    for (i = 0; i < 5; i++) {
      ((u32 *)&speech_line_advisors)[i] = 0;
    }
    speech_line_advisors[0] = advisor;
    value = text_next_int();
    speech_line_values[0] = value;
    value = text_next_int();
    speech_line_params[0] = value;
    str_clear((char *)((u8 *)this + 0xea8));
    str_trim((*(char * *)&text_parse.line_cursor));
    str_append((char *)((u8 *)this + 0xea8),(const char *)(*(char * *)&text_parse.line_cursor));
    line_idx = 1;
    matched = true;
    skip_read = false;
    while( true ) {
      if ((2 < line_idx) || (((u32 *)&speech_line_advisors)[line_idx] == 0xffffffff)) {
        result = 1;
        break;
      }
      if ((!skip_read) && (line = text_read_line(1), line == 0)) break;
      skip_read = false;
      token = text_next_token();
      switch(*token) {
      case 0x23:
        skip_read = matched;
        continue;
      case 0x3b:
        continue;
      case 0x41:
      case 0x61:
        advisor = 4;
        break;
      case 0x45:
      case 0x65:
        ((u32 *)&speech_line_advisors)[line_idx] = 0xffffffff;
        continue;
      case 0x46:
      case 0x66:
        advisor = 3;
        break;
      case 0x4d:
      case 0x6d:
        advisor = 0;
        break;
      case 0x52:
      case 0x72:
        status = _atoi(token + 1);
        roll = rand_range(1,10);
        if (roll <= status) {
          matched = false;
        }
        skip_read = roll > status;
        continue;
      case 0x53:
      case 0x73:
        advisor = 1;
        break;
      case 0x54:
      case 0x74:
        advisor = 2;
      }
      if ((used_mask & 1 << ((byte)advisor & 0x1f)) == 0) {
        if ((token[1] == '\0') || (value = _atoi(token + 1), value == ((u32 *)&advisor_opinions)[advisor])
           ) {
          ((u32 *)&speech_line_advisors)[line_idx] = advisor;
          value = text_next_int();
          ((u32 *)&speech_line_values)[line_idx] = value;
          value = text_next_int();
          ((u32 *)&speech_line_params)[line_idx] = value;
          str_clear((char *)((int)this + line_idx * 0xff + 0xea8));
          str_trim((*(char * *)&text_parse.line_cursor));
          if (*(*(char * *)&text_parse.line_cursor) != '*') {
            str_append((char *)((int)this + line_idx * 0xff + 0xea8),(const char *)(*(char * *)&text_parse.line_cursor));
          }
          line_idx++;
          matched = true;
          used_mask = used_mask | 1 << ((byte)advisor & 0x1f);
        }
        else {
          matched = false;
        }
      }
      else {
        matched = false;
      }
    }
  }
  text_close_file();
  return result;
}

/* 00517dd3 auto */
void CouncilWindow::draw_speech(int mode)

{
  int spacing;
  GameFont font2;
  tagRECT rect;
  
  font2.init(0,0xe);
  ON_SCOPE_EXIT(font2.destroy());
  if (!((speech_advisor_index < 0) || (2 < speech_advisor_index))) {
    if (mode != 1) {
      fill_rect((iptr)((u8 *)&speech_rect),0x1d);
      speech_line_y = 2;
    }
    if (mode == -1) {
      repaint();
    }
    else {
      rect.bottom = speech_rect_bottom;
      rect.left = speech_rect + 2;
      rect.right = speech_rect_right - 2;
      rect.top = speech_rect_top + speech_line_y;
      ((Dialog *)((u8 *)&council_dialog))->set_font((iptr)&font2);
      if (council_speech_lines[speech_advisor_index * 0xff + 0x5c] != '\0') {
        OffsetRect(&rect,1,1);
        gfx_set_text_color_index(0x25);
        draw_text_rect((iptr)&font2,
                   (const char *)(council_speech_lines + speech_advisor_index * 0xff + 0x5c),
                   (iptr)&rect,1);
        OffsetRect(&rect,-1,-1);
        gfx_set_text_color_index(10);
        draw_text_rect((iptr)&font2,
                   (const char *)(council_speech_lines + speech_advisor_index * 0xff + 0x5c),
                   (iptr)&rect,1);
        spacing = font2.line_spacing();
        speech_line_y = speech_line_y + spacing;
        spacing = font2.text_width(
                           (const char *)(council_speech_lines +
                                 speech_advisor_index * 0xff + 0x5c));
        if (rect.right - rect.left < spacing) {
          spacing = font2.line_spacing();
          speech_line_y = speech_line_y + spacing;
        }
      }
      repaint();
      wnd_flush_paint();
    }
  }
  return;
}

/* 00518007 auto */
void CouncilWindow::draw_advisor_frames()

{
  int status;
  char *line;
  i32 frame_no;
  int advisor;
  DibSurface surface;
  Sprite sprite;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (council_active_2 == 0) {
    for (advisor = 0; advisor < 5; advisor++) {
      status = text_open_section((const char *)council_text_file,
                                 (const char *)(advisor_code_slots + advisor * 3 - 0x40));
      if (status != 0) {
        return;
      }
      line = text_read_line(1);
      if (line == 0) {
        return;
      }
      frame_no = text_next_int();
      video.show_frame(frame_no);
      ((DibSurface *)&video_head)->avi_player_draw_to((iptr)this,
                 (ui_metrics.border_width + 0x5a) * advisor + ui_metrics.border_width + 1,ui_metrics.caption_height + 1);
      text_close_file();
    }
  }
  else {
    video.show_frame(1);
    ((DibSurface *)&video_head)->avi_player_draw_to((iptr)this,ui_metrics.border_width + 2,ui_metrics.caption_height + 2);
  }
  return;
}

/* 005181d8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl council_on_advisor_button(uptr button_id)

{
  CouncilWindow *self;
  int loaded;
  
  self = council_state.wnd;
  council_state.video_busy = 1;
  snd_wave_stop_all();
  ((AviPlayer *)((iptr)self + 0x114))->pause();
  if (button_id == 0xca) {
    self->draw_speech(0xffffffff);
    wnd_flush_paint();
    ((ModalMenuWindow *)((iptr)self + 0x48))->end_modal_loop();
  }
  else if (self->council_active_2 == 0) {
    self->draw_speech(0xffffffff);
    self->speech_advisor_index = 0;
    loaded = self->load_speech(button_id);
    if (loaded == 0) {
      self->draw_speech(0xffffffff);
      wnd_flush_paint();
      ((ModalMenuWindow *)((iptr)self + 0x48))->end_modal_loop();
    }
    else {
      ((WndBase *)((iptr)self + 0x15c))->hide();
      self->move_video_to_slot(button_id);
      if ((int)button_id < 5) {
        self->draw_speech(0);
        ((AviPlayer *)((iptr)self + 0x114))->show_frame(
                     (i32)self->speech_line_values[self->speech_advisor_index]);
        ((SurfaceWindow *)((iptr)self + 0x114))->SurfaceWindow::show();
        ((AviPlayer *)((iptr)self + 0x114))->play_range(
                   (i32)self->speech_line_values[self->speech_advisor_index],
                   (i32)self->speech_line_params[self->speech_advisor_index]);
        self->speech_advisor_index = self->speech_advisor_index + 1;
      }
      council_state.video_busy = 0;
    }
  }
  return;
}

/* 00518342 auto */
void CouncilWindow::move_video_to_slot(iptr slot)

{
  if (slot < 5) {
    ((WndBase *)((int)this + 0x15c))->move_xy((ui_metrics.border_width + 0x5a) * slot + ui_metrics.border_width + 1,
               ui_metrics.caption_height + 1);
  }
  return;
}

/* 00518392 auto */
i32 CouncilWindow::advisor_from_letter(i8 *token)

{
  i32 advisor;
  
  switch(*token) {
  case 0x41:
  case 0x61:
    advisor = 4;
    break;
  default:
    advisor = 0xffffffff;
    break;
  case 0x46:
  case 0x66:
    advisor = 3;
    break;
  case 0x4d:
  case 0x6d:
    advisor = 0;
    break;
  case 0x53:
  case 0x73:
    advisor = 1;
    break;
  case 0x54:
  case 0x74:
    advisor = 2;
  }
  return advisor;
}

/* 00518471 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl council_on_video_end(void)

{
  CouncilWindow *self;
  
  self = council_state.wnd;
  council_state.video_busy = 1;
  if (council_state.wnd->council_active_2 == 0) {
    ((WndBase *)((iptr)council_state.wnd + 0x15c))->hide();
    if ((self->speech_advisor_index < 3) && (-1 < (int)self->speech_line_advisors[self->speech_advisor_index])
       ) {
      self->move_video_to_slot((iptr)self->speech_line_advisors[self->speech_advisor_index]);
      self->draw_speech(1);
      ((AviPlayer *)((iptr)self + 0x114))->show_frame(
                   (i32)self->speech_line_values[self->speech_advisor_index]);
      ((SurfaceWindow *)((iptr)self + 0x114))->SurfaceWindow::show();
      ((AviPlayer *)((iptr)self + 0x114))->play_range(
                 (i32)self->speech_line_values[self->speech_advisor_index],
                 (i32)self->speech_line_params[self->speech_advisor_index]);
      self->speech_advisor_index = self->speech_advisor_index + 1;
    }
  }
  wnd_flush_paint();
  council_state.video_busy = 0;
  return;
}

/* 00518582 auto */
i32 __cdecl council_advisor_opinion(iptr civ,i32 advisor)

{
  i32 opinion;
  i32 era;
  
  if (council_state.wnd != 0) {
    era = council_state.wnd->council_era;
  }
  switch(advisor) {
  case 0:
    opinion = council_military_advice(civ,era);
    break;
  case 1:
    opinion = council_science_advice(civ,era);
    break;
  case 2:
    opinion = council_trade_advice(civ,era);
    break;
  case 3:
    opinion = council_foreign_advice(civ,era);
    break;
  default:
    opinion = council_attitude_advice(civ,era);
  }
  return opinion;
}

/* 00522dfa auto */
void __cdecl leaders_reset_custom_names(void)

{
  iptr leader;
  int i;
  
  for (leader = 0; leader < 0x15; leader++) {
    if (leaders[leader].leader_name < 1) {
      leaders[leader].leader_name = ~leaders[leader].leader_name + 1;
    }
    if (leaders[leader].plural < 1) {
      leaders[leader].plural = ~leaders[leader].plural + 1;
    }
    if (leaders[leader].adjective < 1) {
      leaders[leader].adjective = ~leaders[leader].adjective + 1;
    }
    for (i = 0; i < 7; i++) {
      if (leaders[leader].titles[i][0] < 1) {
        *(ushort *)&leaders[leader].titles[i][0] =
             ~*(ushort *)&leaders[leader].titles[i][0] + 1;
      }
      if (leaders[leader].titles[i][1] < 1) {
        *(ushort *)&leaders[leader].titles[i][1] =
             ~*(ushort *)&leaders[leader].titles[i][1] + 1;
      }
    }
  }
  return;
}

/* 0059c575 auto */
void __cdecl casualty_log_add(iptr unit,iptr unit2,iptr year,iptr x,i32 y)

{
  i32 color;
  char *_Source;
  size_t _Count;
  
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].unit_type =
       (ushort)units[unit].type;
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].x = (short)x;
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].y =
       (i16)y;
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].year = (short)year;
  color = civ_color_secondary((int)units[unit2].owner);
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].killer_color = (short)color;
  _Count = 0x18;
  _Source = civ_name_plural((int)units[unit2].owner);
  _strncpy(net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].killer_name,_Source,_Count);
  net.casualty_log[units[unit].owner * 0x12c + net.casualty_log_next[units[unit].owner]].killer_name[23] = 0;
  net.casualty_log_next[units[unit].owner] =
       net.casualty_log_next[units[unit].owner] + 1;
  if (net.casualty_log_next[units[unit].owner] == 300) {
    net.casualty_log_next[units[unit].owner] = 0;
  }
  if (net.casualty_log_next[units[unit].owner] ==
      net.casualty_log_first[units[unit].owner]) {
    net.casualty_log_first[units[unit].owner] =
         net.casualty_log_first[units[unit].owner] + 1;
    if (net.casualty_log_first[units[unit].owner] == 300) {
      net.casualty_log_first[units[unit].owner] = 0;
    }
  }
  else {
    net.casualty_counts[units[unit].owner] =
         net.casualty_counts[units[unit].owner] + 1;
  }
  if (-1 < casualty.report_id) {
    ((SurfaceWindow *)((i8 *)&casualty.window))->repaint();
  }
  return;
}
