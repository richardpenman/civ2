/* The game, cities: the tax rate dialog and the trade, city status and wonders reports: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0040bdac auto */
void __cdecl
taxrate_balance(int *rate_adjust,int *rate_first,int *rate_second,int *rate_first_cap,int *lock_first,int *lock_second)

{
  int max_rate;
  
  max_rate = (int)taxrate_wnd.dlg->max_rate_value;
  while (*rate_second + *rate_first + *rate_adjust < 10) {
    if (((*rate_first < max_rate) && (*rate_first < *rate_first_cap)) && (*lock_first == 0)
       ) {
      *rate_first = *rate_first + 1;
    }
    else if ((*rate_second < max_rate) && (*lock_second == 0)) {
      *rate_second = *rate_second + 1;
    }
    else if ((*rate_first < max_rate) && (*lock_first == 0)) {
      *rate_first = *rate_first + 1;
    }
    else {
      *rate_adjust = *rate_adjust + 1;
    }
  }
  while (10 < *rate_second + *rate_first + *rate_adjust) {
    if ((*rate_first == 0) || (*lock_first != 0)) {
      if ((*rate_second == 0) || (*lock_second != 0)) {
        *rate_adjust = *rate_adjust - 1;
      }
      else {
        *rate_second = *rate_second - 1;
      }
    }
    else {
      *rate_first = *rate_first - 1;
    }
  }
  return;
}

/* 0040bed1 auto */
void __cdecl taxrate_dialog_redraw(void)

{
  taxrate_wnd.dlg->paint();
  return;
}

/* 0040beec auto */
void __cdecl taxrate_on_tax_scroll(int pos)

{
  int lux_rate;
  int science_rate2;
  int requested_rate;
  int tax_rate2;
  
  if ((int)taxrate_wnd.dlg->max_rate_value < pos) {
    taxrate_wnd.dlg->rates_clamped = 1;
  }
  tax_rate2 = pos;
  requested_rate = pos;
  tax_rate2 = int_clamp(pos,0,(i32)taxrate_wnd.dlg->max_rate_value);
  science_rate2 = (int)taxrate_wnd.dlg->sci_rate_sel;
  lux_rate = (int)taxrate_wnd.dlg->lux_rate_sel;
  taxrate_balance(&tax_rate2,&science_rate2,&lux_rate,(int *)&taxrate_wnd.dlg->init_sci_rate,
                     &taxrate_wnd.dlg->rate_locked[1],&taxrate_wnd.dlg->rate_locked[2]);
  taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
  if (requested_rate != tax_rate2) {
    taxrate_wnd.dlg->tax_scrollbar_obj.set_pos(tax_rate2);
    taxrate_wnd.dlg->tax_scrollbar_obj.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->sci_rate_sel != science_rate2) {
    taxrate_wnd.dlg->sci_rate_sel = science_rate2;
    taxrate_wnd.dlg->sci_scrollbar.set_pos(science_rate2);
    taxrate_wnd.dlg->sci_scrollbar.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->lux_rate_sel != lux_rate) {
    taxrate_wnd.dlg->lux_rate_sel = lux_rate;
    taxrate_wnd.dlg->lux_scrollbar.set_pos(lux_rate);
    taxrate_wnd.dlg->lux_scrollbar.show_and_redraw();
  }
  taxrate_wnd.dlg->calc_totals();
  taxrate_dialog_redraw();
  return;
}

/* 0040c07f auto */
void __cdecl taxrate_on_science_scroll(int pos)

{
  int tax_rate2;
  int lux_rate;
  int requested_rate;
  int science_rate2;
  
  if ((int)taxrate_wnd.dlg->max_rate_value < pos) {
    taxrate_wnd.dlg->rates_clamped = 1;
  }
  science_rate2 = pos;
  requested_rate = pos;
  science_rate2 = int_clamp(pos,0,(i32)taxrate_wnd.dlg->max_rate_value);
  lux_rate = (int)taxrate_wnd.dlg->lux_rate_sel;
  tax_rate2 = (int)taxrate_wnd.dlg->tax_rate_sel;
  taxrate_balance(&science_rate2,&lux_rate,&tax_rate2,(int *)&taxrate_wnd.dlg->init_lux_rate,
                     &taxrate_wnd.dlg->rate_locked[2],&taxrate_wnd.dlg->rate_locked[0]);
  taxrate_wnd.dlg->sci_rate_sel = science_rate2;
  if (requested_rate != science_rate2) {
    taxrate_wnd.dlg->sci_scrollbar.set_pos(science_rate2);
    taxrate_wnd.dlg->sci_scrollbar.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->lux_rate_sel != lux_rate) {
    taxrate_wnd.dlg->lux_rate_sel = lux_rate;
    taxrate_wnd.dlg->lux_scrollbar.set_pos(lux_rate);
    taxrate_wnd.dlg->lux_scrollbar.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->tax_rate_sel != tax_rate2) {
    taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
    taxrate_wnd.dlg->tax_scrollbar_obj.set_pos(tax_rate2);
    taxrate_wnd.dlg->tax_scrollbar_obj.show_and_redraw();
  }
  taxrate_wnd.dlg->calc_totals();
  taxrate_dialog_redraw();
  return;
}

/* 0040c212 auto */
void __cdecl taxrate_on_lux_scroll(int pos)

{
  int science_rate2;
  int tax_rate2;
  int requested_rate;
  int lux_rate;
  
  if ((int)taxrate_wnd.dlg->max_rate_value < pos) {
    taxrate_wnd.dlg->rates_clamped = 1;
  }
  lux_rate = pos;
  requested_rate = pos;
  lux_rate = int_clamp(pos,0,(i32)taxrate_wnd.dlg->max_rate_value);
  tax_rate2 = (int)taxrate_wnd.dlg->tax_rate_sel;
  science_rate2 = (int)taxrate_wnd.dlg->sci_rate_sel;
  taxrate_balance(&lux_rate,&tax_rate2,&science_rate2,(int *)&taxrate_wnd.dlg->init_tax_rate,
                     &taxrate_wnd.dlg->rate_locked[0],&taxrate_wnd.dlg->rate_locked[1]);
  taxrate_wnd.dlg->lux_rate_sel = lux_rate;
  if (lux_rate != requested_rate) {
    taxrate_wnd.dlg->lux_scrollbar.set_pos(lux_rate);
    taxrate_wnd.dlg->lux_scrollbar.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->tax_rate_sel != tax_rate2) {
    taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
    taxrate_wnd.dlg->tax_scrollbar_obj.set_pos(tax_rate2);
    taxrate_wnd.dlg->tax_scrollbar_obj.show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->sci_rate_sel != science_rate2) {
    taxrate_wnd.dlg->sci_rate_sel = science_rate2;
    taxrate_wnd.dlg->sci_scrollbar.set_pos(science_rate2);
    taxrate_wnd.dlg->sci_scrollbar.show_and_redraw();
  }
  taxrate_wnd.dlg->calc_totals();
  taxrate_dialog_redraw();
  return;
}

/* 0040c3a5 auto */
void __cdecl taxrate_on_ok_clicked(void)

{
  taxrate_wnd.active = 0;
  taxrate_wnd.dlg->window.end_modal_loop();
  return;
}

/* 0040c3cd auto */
void __cdecl taxrate_on_lock_toggled(int control_id,int checked)

{
  int i;
  
  taxrate_wnd.dlg->rate_locked[control_id - 300] = checked;
  if (checked != 0) {
    for (i = 0; i < 3; i++) {
      if (control_id - 300 != i) {
        taxrate_wnd.dlg->rate_locked[i] = 0;
        taxrate_wnd.dlg->lock_checkboxes[i].set_checked(0);
        ((Control *)&taxrate_wnd.dlg->lock_checkboxes[i])->show_and_redraw()
        ;
      }
    }
  }
  return;
}

/* 0040c480 auto */
void TaxRateDialog::calc_totals()

{
  u8 saved_science_rate;
  u8 saved_tax_rate;
  int upkeep;
  uptr civ;
  iptr improvement;
  iptr city;
  int improvement_counts[39];
  
  total_tax = 0;
  total_upkeep_value = 0;
  total_science = 0;
  civ = this->civ;
  for (improvement = 0; improvement < 0x27; improvement++) {
    improvement_counts[improvement] = 0;
  }
  saved_science_rate = civs[civ].science_rate;
  saved_tax_rate = civs[civ].tax_rate;
  civs[civ].tax_rate = (u8)tax_rate_sel;
  civs[civ].science_rate = (u8)sci_rate_sel;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && ((int)cities[city].owner == civ)) {
      city_split_trade(city,(int)cities[city].trade,1,0);
      for (improvement = 0; improvement < 0x27; improvement++) {
        upkeep = city_has_improvement(city,improvement);
        if (upkeep != 0) {
          improvement_counts[improvement]++;
        }
      }
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        total_tax += (int)cities[city].tax;
        total_science += (int)cities[city].science;
      }
    }
  }
  for (improvement = 0; improvement < 0x27; improvement++) {
    if ((improvement_counts[improvement] != 0) && (upkeep = civ_improvement_upkeep(civ,improvement), upkeep != 0)) {
      total_upkeep_value += improvement_counts[improvement] * upkeep;
    }
  }
  civs[civ].tax_rate = saved_tax_rate;
  civs[civ].science_rate = saved_science_rate;
  return;
}

/* 0040c7d0 auto */
void TaxRateDialog::paint()

{
  int research_cost;
  
  draw_caption();
  if ((taxrate_state.bg_surface == 0) || (taxrate_wnd.active == 0)) {
    gfx_fill_rect(this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((DibSurface *)taxrate_state.bg_surface,this,0,0,client_x,client_y,dialog_w,
                       dialog_h);
  }
  text_set_surface(this);
  text_set_font((GameFont *)&ui_text_font);
  if (rates_clamped == 0) {
    text_set_colors(0x25,0x12,1,1);
  }
  else {
    text_set_colors(0x6a,10,1,1);
    rates_clamped = 0;
  }
  text_clear();
  text_append_label(0x98);
  text_append_colon();
  text_append_string_id(government_names[civs[civ].government]);
  text_append_space();
  text_append_space();
  text_append_space();
  text_append_label(0x100);
  text_append_colon();
  text_append_int(max_rate_value * 10);
  text_append_percent();
  text_draw_centered(this,textbuf.buf,label_x_value,title_y_value,bar_width_value);
  text_set_colors(0x25,0x12,1,1);
  text_clear();
  text_append_int(0);
  text_append_percent();
  text_draw_shadowed(this,textbuf.buf,label_x_value,tax_label_y);
  text_draw_shadowed(this,textbuf.buf,label_x_value,sci_label_y);
  text_draw_shadowed(this,textbuf.buf,label_x_value,lux_label_y);
  text_clear();
  text_append_int(100);
  text_append_percent();
  text_draw_right_aligned(this,textbuf.buf,label_x_value,tax_label_y,bar_width_value);
  text_draw_right_aligned(this,textbuf.buf,label_x_value,sci_label_y,bar_width_value);
  text_draw_right_aligned(this,textbuf.buf,label_x_value,lux_label_y,bar_width_value);
  text_clear();
  text_append_label(0x102);
  text_draw_shadowed(this,textbuf.buf,
                     bar_width_value + margin_value + margin_value + client_x,
                     tax_label_y);
  text_clear();
  text_append_label(0x59);
  text_append_colon();
  text_append_int(tax_rate_sel * 10);
  text_append_percent();
  text_draw_centered(this,textbuf.buf,label_x_value,tax_label_y,bar_width_value);
  text_clear();
  text_append_label(0x5b);
  text_append_colon();
  text_append_int(sci_rate_sel * 10);
  text_append_percent();
  text_draw_centered(this,textbuf.buf,label_x_value,sci_label_y,bar_width_value);
  text_clear();
  text_append_label(0x5a);
  text_append_colon();
  text_append_int(lux_rate_sel * 10);
  text_append_percent();
  text_draw_centered(this,textbuf.buf,label_x_value,lux_label_y,bar_width_value);
  text_clear();
  text_append_label(0x88);
  text_append_colon();
  text_append_int(total_tax);
  text_append_space();
  text_append_space();
  text_append_space();
  text_append_label(0x87);
  text_append_colon();
  text_append_int(total_upkeep_value);
  text_draw_centered(this,textbuf.buf,label_x_value,conceal_button_value,bar_width_value);
  text_clear();
  text_append_label(0x89);
  text_append_colon();
  research_cost = civ_research_cost(civ);
  if (research_cost < 1) {
    research_cost = 1;
  }
  if ((int)total_science < 1) {
    total_science = 1;
  }
  text_append_int((int)((total_science - 1) + research_cost) / (int)total_science);
  text_append_space();
  text_append_label(0x2c);
  text_draw_centered(this,textbuf.buf,label_x_value,reveal_button_value,bar_width_value);
  wnd_blit_surface();
  return;
}

/* 0040cd64 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void TaxRateDialog::run(uptr civ)

{
  uptr row_y;
  u32 button_height;
  u32 client_left;
  DibSurface *this_00;
  u32 value;
  iptr label_text;
  i32 title_text;
  int label_center_x;
  u32 left_x;
  int bar_y;
  i32 style;
  i32 arg_x;
  i32 arg_y;
  i32 arg_7;
  i32 arg_8;
  i32 arg_9;
  u8 *parent_wnd_lock;
  u8 *parent_wnd_ok;
  u8 *parent_wnd_lux;
  u8 *parent_wnd_sci;
  u8 *parent_wnd_tax;
  DibSurface *bg_surface2;
  iptr city;
  iptr gif_resource_id;
  int i;
  int rates_changed;
  int label_width;
  int checkbox_y;
  i8 rect[16];
  int label_x;
  GamePalette palette;
  
  rates_changed = 0;
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  taxrate_wnd.dlg = this;
  this->civ = civ;
  taxrate_wnd.active = 1;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  (*(DibSurface * *)&taxrate_state.bg_surface) = bg_surface2;
  value = civ_max_tax_rate(civ);
  max_rate_value = value;
  rates_clamped = 0;
  while ((int)max_rate_value < (int)(uint)civs[civ].tax_rate) {
    civs[civ].tax_rate += 0xff;
    if ((int)(uint)civs[civ].science_rate < (int)max_rate_value) {
      civs[civ].science_rate++;
    }
    rates_clamped = 1;
  }
  while ((int)max_rate_value < (int)(uint)civs[civ].science_rate) {
    civs[civ].science_rate += 0xff;
    if ((int)(uint)civs[civ].tax_rate < (int)max_rate_value) {
      civs[civ].tax_rate++;
    }
    rates_clamped = 1;
  }
  while ((int)max_rate_value <
         (int)(10 - ((uint)civs[civ].science_rate + (uint)civs[civ].tax_rate))) {
    if ((int)(uint)civs[civ].tax_rate < (int)max_rate_value) {
      civs[civ].tax_rate++;
    }
    else {
      civs[civ].science_rate++;
    }
    rates_clamped = 1;
  }
  init_tax_rate = (uint)civs[civ].tax_rate;
  init_sci_rate = (uint)civs[civ].science_rate;
  init_lux_rate = 10 - ((uint)civs[civ].science_rate + (uint)civs[civ].tax_rate);
  taxrate_balance((int *)&init_tax_rate,(int *)&init_sci_rate,(int *)&init_lux_rate,
                     (int *)&init_sci_rate,&rates_changed,&rates_changed);
  tax_rate_sel = init_tax_rate;
  sci_rate_sel = init_sci_rate;
  lux_rate_sel = init_lux_rate;
  dialog_w = 400;
  dialog_h = 0x100;
  margin_value = 10;
  bar_gap = 6;
  value = GetSystemMetrics(3);
  bar_height = value;
  gif_resource_id = 0x41;
  if (0 < ui_metrics.large_screen) {
    dialog_w = (int)(dialog_w * 3) / 2;
    dialog_h = (int)(dialog_h * 3) / 2;
    bar_height = (int)(bar_height * 3) / 2;
    margin_value = (int)(margin_value * 3) / 2;
    bar_gap = (int)(bar_gap * 3) / 2;
    gif_resource_id = 0x42;
  }
  (*(DibSurface * *)&taxrate_state.bg_surface)->load_gif_resource(gif_resource_id,10,0xc0,&palette);
  value = ((GameFont *)((i8 *)&ui_text_font))->font_cached_height();
  row_spacing_value = value;
  label_text = (iptr)label_string(label_table.string_ids[0x102]);
  label_width = ((GameFont *)((i8 *)&ui_text_font))->text_width((const char *)label_text);
  bar_width_value = (dialog_w + margin_value * -2) - (margin_value + label_width);
  arg_9 = 0;
  arg_8 = 0;
  arg_7 = 0;
  value = dialog_h;
  left_x = dialog_w;
  arg_y = 0;
  arg_x = 0;
  style = 0xd;
  title_text = (iptr)label_string(label_table.string_ids[0x101]);
  create((const char *)title_text,style,arg_x,arg_y,left_x,value,arg_7,arg_8,arg_9);
  label_x = bar_width_value + margin_value + margin_value + client_x;
  label_center_x = (label_width >> 1) + label_x;
  left_x = margin_value + client_x;
  label_x_value = left_x;
  value = bar_gap + client_y;
  header_y_value = value;
  title_y_value = value;
  row_y = value + row_spacing_value + row_spacing_value;
  conceal_button_value = row_y;
  row_y += row_spacing_value;
  reveal_button_value = row_y;
  value = row_y + row_spacing_value + row_spacing_value;
  tax_label_y = value;
  bar_y = value + row_spacing_value;
  checkbox_y = ((int)bar_height >> 1) + bar_y - 0x10;
  rect_set_xywh((RECT *)rect,left_x,bar_y,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_tax = (u8 *)0x0;
  }
  else {
    parent_wnd_tax = (u8 *)&window;
  }
  tax_scrollbar_obj.create((Wnd *)parent_wnd_tax,0x65,(RECT *)rect,0);
  tax_scrollbar_obj.set_range(0,10);
  tax_scrollbar_obj.set_pos(tax_rate_sel);
  tax_scrollbar_obj.set_callback(taxrate_on_tax_scroll);
  value = bar_y + bar_gap + bar_height;
  sci_label_y = value;
  bar_y = value + row_spacing_value;
  rect_set_xywh((RECT *)rect,left_x,bar_y,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_sci = (u8 *)0x0;
  }
  else {
    parent_wnd_sci = (u8 *)&window;
  }
  sci_scrollbar.create((Wnd *)parent_wnd_sci,0x66,(RECT *)rect,0);
  sci_scrollbar.set_range(0,10);
  sci_scrollbar.set_pos(sci_rate_sel);
  sci_scrollbar.set_callback(taxrate_on_science_scroll);
  value = bar_y + bar_gap + bar_height;
  lux_label_y = value;
  rect_set_xywh((RECT *)rect,left_x,value + row_spacing_value,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_lux = (u8 *)0x0;
  }
  else {
    parent_wnd_lux = (u8 *)&window;
  }
  lux_scrollbar.create((Wnd *)parent_wnd_lux,0x67,(RECT *)rect,0);
  lux_scrollbar.set_range(0,10);
  lux_scrollbar.set_pos(lux_rate_sel);
  lux_scrollbar.set_callback(taxrate_on_lux_scroll);
  bar_y = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_area_height = bar_y + 8;
  button_row_width = client_width - 4;
  value = client_y;
  left_x = client_height;
  button_height = button_area_height;
  client_left = client_x;
  window.set_keyboard_nav(0);
  rect_set_xywh((RECT *)rect,client_left + 2,(value + left_x) - (button_height + 2),button_row_width,
                     button_area_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_ok = (u8 *)0x0;
  }
  else {
    parent_wnd_ok = (u8 *)&window;
  }
  label_text = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)parent_wnd_ok,0xc9,(RECT *)rect,(const char *)label_text);
  ok_button.set_callback((CtrlNotifyFn)taxrate_on_ok_clicked);
  ok_button.set_cancel_button();
  ok_button.set_default_button();
  if (ui_metrics.large_screen < 1) {
    checkbox_y += 6;
  }
  for (i = 0; i < 3; i++) {
    rate_locked[i] =
         taxrate_state.lock_flags[i];
    if (this == (TaxRateDialog *)0x0) {
      parent_wnd_lock = (u8 *)0x0;
    }
    else {
      parent_wnd_lock = (u8 *)&window;
    }
    lock_checkboxes[i].create((Wnd *)parent_wnd_lock,
               i + 300,label_center_x - 0x10,checkbox_y,(const char *)&str_empty_9);
    lock_checkboxes[i].set_checked(
               rate_locked[i]);
    lock_checkboxes[i].set_callback(taxrate_on_lock_toggled)
    ;
    checkbox_y = checkbox_y + bar_gap + bar_height + row_spacing_value;
  }
  calc_totals();
  set_redraw_callback(taxrate_dialog_redraw);
  repaint();
  show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)&window)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)&window)->run_modal_loop();
  ((ModalMenuWindow *)&window)->set_modal_idle_callback(0);
  while (taxrate_wnd.active != 0) {
    win_pump_one_message();
  }
  if ((*(DibSurface * *)&taxrate_state.bg_surface) != (DibSurface *)0x0) {
    (*(DibSurface * *)&taxrate_state.bg_surface)->surface_delete(1);
  }
  (*(DibSurface * *)&taxrate_state.bg_surface) = (DibSurface *)0x0;
  for (i = 0; i < 3; i++) {
    taxrate_state.lock_flags[i] =
         rate_locked[i];
  }
  if (((uint)civs[civ].tax_rate != tax_rate_sel) ||
     ((uint)civs[civ].science_rate != sci_rate_sel)) {
    rates_changed = 1;
  }
  civs[civ].tax_rate = (u8)tax_rate_sel;
  civs[civ].science_rate = (u8)sci_rate_sel;
  if ((game.turn < 0xb) && (civs[civ].government < GOV_MONARCHY)) {
    prefs.science_rate = (ushort)civs[civ].science_rate;
    prefs.tax_rate = (ushort)civs[civ].tax_rate;
    prefs_save_civ2dat();
  }
  if (rates_changed != 0) {
    game.transient_flags = game.transient_flags & ~TRANSIENT_ATTITUDE_DIRTY;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && ((int)cities[city].owner == civ)) {
        city_recalc(city,1);
      }
    }
  }
  ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(0);
  status_redraw(1);
  advisor_refresh_if_open(4);
  advisor_refresh_if_open(5);
  advisor_refresh_if_open(6);
  return;
}

/* 0040ddc6 auto */
void __cdecl taxrate_dialog_show(int civ)

{
  TaxRateDialog dialog2;
  
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    if ((game.multiplayer < 3) || (human_civ[0] == civ)) {
      dialog2.run(civ);
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    else {
      net_send_message(NM_KINGDOM_RATES,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,
                         0,0,0,0,0);
    }
  }
  return;
}

/* 0040decc auto */
void __cdecl taxrate_enforce_max_rate(int civ)

{
  int max_rate;
  
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    if ((game.multiplayer < 3) || (human_civ[0] == civ)) {
      max_rate = civ_max_tax_rate(civ);
      if (max_rate < (int)(uint)civs[civ].tax_rate) {
        taxrate_dialog_show(civ);
      }
      if (max_rate < (int)(10 - ((uint)civs[civ].science_rate + (uint)civs[civ].tax_rate))) {
        taxrate_dialog_show(civ);
      }
      if (max_rate < (int)(uint)civs[civ].science_rate) {
        taxrate_dialog_show(civ);
      }
    }
    else {
      net_send_message(NM_TAX_CHECK,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,
                         0,0,0,0,0);
    }
  }
  return;
}

/* 0040f060 auto */
TaxRateDialog::TaxRateDialog()

{
  
  FrameWindow::construct();
  tax_scrollbar_obj.construct();
  lux_scrollbar.construct();
  sci_scrollbar.construct();
  crt_vector_ctor_iterator(rate_lock_checkboxes,0x3c,3,checkbox_elem_ctor,
               checkbox_dtor);
  button_ctor(&ok_button);
  vtable = (u32)&ptr_fun;
}

/* 0040f1e0 auto */
TaxRateDialog::~TaxRateDialog()

{
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0040f25f */ { ctrl_dtor(&ok_button); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0040f26e */ { crt_vector_dtor_iterator((void *)lock_checkboxes,0x3c,3,checkbox_dtor); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0040f286 */ { sci_scrollbar.destruct(); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0040f295 */ { lux_scrollbar.destruct(); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0040f2a4 */ { tax_scrollbar_obj.destruct(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0040f2b3 */ { FrameWindow::destruct(); }
  return;
}

/* 0042b824 auto */
i32 __cdecl trade_commodity_cities_dialog(int civ,int commodity)

{
  bool matches;
  int has_improvement;
  iptr adjective;
  i32 result;
  iptr city;
  int section_count;
  int total_count;
  Dialog dialog2;
  int pass;
  iptr slot;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  text_set_label_arg(0,commodity_names[commodity]);
  dialog2.load_game_section((const char *)&str_supplyshow,0x40001);
  total_count = 0;
  for (pass = 0; pass < 2; pass++) {
    section_count = 0;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) &&
         ((cities[city].owner == civ ||
          (((pass == 0 &&
            ((*(char *)&cities[city].revealed_size[civ] != '\0' ||
             (has_improvement = city_has_improvement(city,IMP_PALACE), has_improvement != 0)))) || (game.reveal_map != '\0')))
          ))) {
        matches = false;
        for (slot = 0; slot < 3; slot++) {
          if (pass == 0) {
            if (*(char *)&cities[city].demands[slot] == commodity) {
              matches = true;
            }
          }
          else if (*(char *)&cities[city].supplies[slot] == commodity) {
            matches = true;
          }
        }
        if (matches) {
          text_clear();
          text_append(cities[city].name);
          text_append_lparen();
          if (cities[city].owner == civ) {
            text_append_int(cities[city].size);
          }
          else {
            adjective = civ_get_adjective(cities[city].owner);
            text_append((const char *)adjective);
            text_append_comma();
            if (((game.reveal_map == '\0') && (has_improvement = city_has_improvement(city,IMP_PALACE), has_improvement == 0))
               && (*(char *)&cities[city].revealed_size[civ] != '\0')) {
              text_append_int(*(char *)&cities[city].revealed_size[civ]);
            }
            else {
              text_append_int(cities[city].size);
            }
          }
          text_append_rparen();
          text_append_space();
          text_append_label((pass == 0) + 0x56);
          text_append_space();
          text_append_string_id(commodity_names[commodity]);
          dialog2.add_option((const char *)&textbuf.buf,city,0);
          total_count++;
          section_count++;
        }
      }
    }
    if (section_count != 0) {
      dialog2.add_option((const char *)&str_empty_32,-1,0);
    }
  }
  if (total_count == 0) {
    result = popup_game_flags((const char *)&str_supplynone,1);
  }
  else {
    result = dialog2.show_modal(0);
  }
  return result;
}

/* 0042bc47 auto */
void __cdecl trade_supply_search_dialog(int civ)

{
  int choice;
  iptr label;
  i32 option_flags;
  iptr option_id;
  Dialog dialog2;
  iptr commodity;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  choice = civ_has_advance(civ,ADV_TRADE);
  if (choice != 0) {
    do {
      dialog2.load_game_section((const char *)&str_supplysearch,0x40001);
      for (commodity = 0; commodity < 0x10; commodity++) {
        option_flags = 0;
        option_id = commodity;
        label = (iptr)label_string(commodity_names[commodity]);
        dialog2.add_option((const char *)label,option_id,option_flags);
      }
      choice = dialog2.show_modal(0);
      if (choice < 0) {
        return;
      }
      choice = trade_commodity_cities_dialog(civ,choice);
    } while (-1 < choice);
  }
  return;
}

/* Part of trade_advisor_draw (0042bd8f): the left column of a row: a city
   with its tax and science, then the totals. */
static void trade_advisor_draw_left_cell(iptr civ,int row_y,int total_science2,int col_x,uint row,int text_y2,int city_count,int item,int total_tax2,int total_upkeep)
{
  i8 sprite_rect4[16];
  i8 sprite_rect3[16];
  i8 sprite_rect2[16];
  i8 sprite_rect1[16];
  i8 sprite_rect0[16];
  iptr city2;
  iptr city;
  int upkeep2;
  int draw_x;
  int n;

  if (item < city_count) {
    n = 0;
    city2 = -1;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        if (item == n) {
          city2 = city;
          break;
        }
        n++;
      }
    }
    if (-1 < city2) {
      city = city2;
      draw_x = (row & 1) * 0x40 + col_x;
      gfx_draw_city(&advisor_state.window,city2,0,draw_x,row_y,0);
      text_set_font(&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      col_x += 0x8a;
      gfx_draw_text((const char *)&cities[city].name,col_x,text_y2);
      col_x += 0x69;
      draw_x = col_x;
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        text_clear();
        text_append_int(cities[city].tax);
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)sprite_rect0,&advisor_state.window,draw_x,
                   text_y2 + 2);
        draw_x += 0x12;
        text_clear();
        text_append_int(cities[city].science);
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((RECT *)sprite_rect1,&advisor_state.window,draw_x,
                   text_y2 + 2);
        draw_x += 0x12;
      }
      else {
        text_clear();
        text_append_label(0x86);
        text_set_colors(0x6a,0x12,1,1);
        gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
      }
    }
  }
  else if (city_count + 1 == item) {
    text_set_font(&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    text_clear();
    text_append_label(0x87);
    text_append_colon();
    text_append_int(total_upkeep);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)sprite_rect2,&advisor_state.window,draw_x,text_y2 + 2);
  }
  else if (city_count + 2 == item) {
    text_clear();
    text_append_label(0x88);
    text_append_colon();
    text_append_long(total_tax2);
    text_set_font(&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)sprite_rect3,&advisor_state.window,draw_x,text_y2 + 2);
  }
  else if (city_count + 3 == item) {
    text_clear();
    text_append_label(0x16e);
    text_append_colon();
    text_append_int(total_science2);
    text_set_font(&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((RECT *)sprite_rect4,&advisor_state.window,draw_x,text_y2 + 2);
  }
  else if (city_count + 4 == item) {
    text_clear();
    text_append_label(0x89);
    text_append_colon();
    upkeep2 = civ_research_cost(civ);
    if (upkeep2 < 2) {
      upkeep2 = 1;
    }
    if (total_science2 < 2) {
      total_science2 = 1;
    }
    text_append_int((total_science2 - 1 + upkeep2) / total_science2);
    text_append_space();
    text_append_label(0x2c);
    text_set_font(&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
  }
}

/* Part of trade_advisor_draw (0042bd8f): the right column of a row: an
   improvement kind with its count and upkeep, then the total upkeep. */
static void trade_advisor_draw_right_cell(iptr civ,int text_y2,int improvement_kinds,int item,int right_col_x,int improvement_counts[39],int total_upkeep)
{
  int value;
  i8 sprite_rect6[16];
  i8 sprite_rect5[16];
  iptr city2;
  iptr improvement;
  int draw_x;
  int n;

  if (item < improvement_kinds) {
    city2 = -1;
    n = 0;
    for (improvement = 0; improvement < 0x27; improvement++) {
      if (improvement_counts[improvement] != 0) {
        if (item == n) {
          city2 = improvement;
          break;
        }
        n++;
      }
    }
    if (-1 < city2) {
      text_set_font(&advisor_bold_font);
      improvement = city2;
      text_clear();
      text_append_int(improvement_counts[improvement]);
      text_append_space();
      text_append_string_id(improvements[improvement].name);
      text_append_space();
      text_append_lparen();
      text_append_label(0x84);
      text_append_colon();
      value = civ_improvement_upkeep(civ,improvement);
      text_append_int(value * improvement_counts[improvement]);
      text_set_colors(0x7a,10,1,1);
      draw_x = gfx_draw_text((const char *)&textbuf.buf,right_col_x,text_y2);
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)sprite_rect5,&advisor_state.window,draw_x,text_y2 + 2
                );
      draw_x += 0xe;
      text_clear();
      text_append_rparen();
      draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
    }
  }
  else if (improvement_kinds + 1 == item) {
    text_set_font(&advisor_bold_font);
    text_set_colors(0x7a,10,1,1);
    text_clear();
    text_append_label(0x87);
    text_append_colon();
    text_append_int(total_upkeep);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,right_col_x,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)sprite_rect6,&advisor_state.window,draw_x,text_y2 + 2);
  }
}

/* 0042bd8f auto */
/* WARNING: Removing unreachable block (ram,0x0042cc50) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl trade_advisor_draw(void)

{
  char *text;
  int value;
  i32 max_row;
  iptr civ;
  int row_y;
  int total_science2;
  int list_bottom;
  int gov_index;
  uint first_row2;
  int col_x;
  iptr improvement;
  int right_rows;
  int top_y;
  uint row;
  iptr city;
  int upkeep2;
  int text_y2;
  int city_count;
  int improvement_kinds;
  int item;
  int right_col_x;
  int total_tax2;
  int improvement_counts[39];
  int left_rows;
  int total_rows;
  int line_height;
  int total_upkeep;
  i8 scroll_rect[16];
  int rows_per_page;
  
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  top_y = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface(&advisor_state.window);
  text_set_font(&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = label_string(label_table.string_ids[0x152]);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  gov_index = int_clamp(civs[civ].government - 1,0,bugfix_ui() ? 5 : 4);  /* B35 */
  text_append_label(gov_index + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = civ_name_plural(civ);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  text = (char *)civ_leader_title(civ);
  text_append(text);
  text_append_space();
  text = civ_leader_name(civ);
  text_append(text);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  advisor_state.list_top = top_y + line_height;
  top_y = advisor_state.list_top + 4;
  line_height = 0x18;
  advisor_state.row_height = 0x18;
  advisor_state.list_top += 0xc;
  advisor_state.list_height = list_bottom - top_y;
  advisor_state.rows_per_page = int_clamp((list_bottom - top_y) / 0x18,1,99);
  rows_per_page = advisor_state.rows_per_page;
  for (improvement = 0; improvement < 0x27; improvement++) {
    improvement_counts[improvement] = 0;
  }
  improvement_kinds = 0;
  total_upkeep = 0;
  total_science2 = 0;
  total_tax2 = 0;
  city_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_count++;
      for (improvement = 0; improvement < 0x27; improvement++) {
        upkeep2 = civ_improvement_upkeep(civ,improvement);
        /* X10: the buildings with no upkeep listed too */
        if ((upkeep2 != 0 || (extra_ui(X_ZERO_UPKEEP) && improvement != 0)) &&
            (value = city_has_improvement(city,improvement), value != 0)) {
          improvement_counts[improvement]++;
        }
      }
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        total_tax2 += cities[city].tax;
        total_science2 += cities[city].science;
      }
    }
  }
  for (improvement = 0; improvement < 0x27; improvement++) {
    if (improvement_counts[improvement] != 0) {
      value = civ_improvement_upkeep(civ,improvement);
      total_upkeep += value * improvement_counts[improvement];
      improvement_kinds++;
    }
  }
  left_rows = city_count + 6;
  right_rows = improvement_kinds + 3;
  total_rows = right_rows;
  if (right_rows <= left_rows) {
    total_rows = left_rows;
  }
  advisor_state.page_count = int_clamp((rows_per_page - 1 + total_rows) / rows_per_page,1,99);
  max_row = int_clamp(total_rows - 1,0,999);
  (*(uint *)&advisor_state.first_row) = int_clamp((*(uint *)&advisor_state.first_row),0,max_row);
  row_y = top_y;
  first_row2 = (*(uint *)&advisor_state.first_row);
  row = (*(uint *)&advisor_state.first_row);
  do {
    if ((int)(first_row2 + rows_per_page) <= (int)row) {
      if (advisor_state.controls_created == 0) {
        rect_set((RECT *)scroll_rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,
                           advisor_state.client_right - 2,list_bottom);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((Wnd *)&advisor_state.wnd,200,(RECT *)scroll_rect,1);
        max_row = int_clamp(total_rows - 1,0,999);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,max_row);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos((*(uint *)&advisor_state.first_row));
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(advisor_state.rows_per_page);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(trade_advisor_on_scroll);
        ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
        advisor_state.controls_created = 1;
      }
      ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
      return;
    }
    if ((int)row < total_rows) {
      col_x = advisor_state.client_x + 2;
      right_col_x = advisor_state.client_x + 0x14f;
      if (row == 0) {
        text_y2 = row_y + 10;
        text_clear();
        text = label_string(label_table.string_ids[0x16c]);
        text_append(text);
        text_set_font(&advisor_font_block.advisor_font);
        text_set_colors(0x29,10,1,1);
        gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
        text_clear();
        text = label_string(label_table.string_ids[0x16d]);
        text_append(text);
        gfx_draw_text((const char *)&textbuf.buf,right_col_x,text_y2);
        row_y += line_height;
      }
      else {
        text_y2 = row_y + 9;
        item = row - 1;
        trade_advisor_draw_left_cell(civ,row_y,total_science2,col_x,row,text_y2,city_count,item,total_tax2,total_upkeep);
        trade_advisor_draw_right_cell(civ,text_y2,improvement_kinds,item,right_col_x,improvement_counts,total_upkeep);
        row_y += line_height;
      }
    }
    row++;
  } while( true );
}

/* 0042ccf4 auto */
void __cdecl trade_advisor_on_scroll(i32 pos)

{
  advisor_state.first_row = pos;
  trade_advisor_draw();
  return;
}

/* 0042cd11 auto */
void __cdecl trade_advisor_on_supply_clicked(void)

{
  trade_supply_search_dialog(advisor_state.report_civ);
  return;
}

/* 0042cd2f auto */
/* WARNING: Removing unreachable block (ram,0x0042cddd) */
/* WARNING: Removing unreachable block (ram,0x0042ce68) */

void __cdecl trade_advisor_show(int civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 rect[16];
  
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(5,5,0,600,400,0,0);
  advisor_state.report_civ = civ;
  rect_set_xywh((RECT *)rect,2,0,0x129,0x18);
  client_y = advisor_state.client_y;
  rect_h = rect_height((RECT *)rect);
  rect_offset((RECT *)rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
  rect_offset((RECT *)rect,299,0);
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((i8 *)&advisor_state.close_button))->create((Wnd *)&advisor_state.wnd,100,(RECT *)rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
  ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
  rect_offset((RECT *)rect,0xfffffed5,0);
  label = (iptr)label_string(label_table.string_ids[0x16f]);
  ((Button *)((i8 *)&advisor_state.toggle_button))->create((Wnd *)&advisor_state.wnd,0x65,(RECT *)rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.toggle_button))->set_callback((CtrlNotifyFn)trade_advisor_on_supply_clicked);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(trade_advisor_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  return;
}

/* 0042ced6 auto */
/* WARNING: Removing unreachable block (ram,0x0042d56a) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_status_draw(void)

{
  uint next_row;
  char *text;
  i32 max_row;
  i8 sprite_rect[16];
  iptr civ;
  int row_y;
  int list_bottom;
  int gov_index;
  int first_row2;
  int col_x;
  iptr improvement;
  int top_y;
  uint row;
  iptr city;
  iptr unit_type;
  uint build_cost;
  int text_y2;
  int city_count;
  int draw_x;
  int resource;
  int line_height;
  i8 scroll_rect[16];
  int rows_per_page;
  
  if (extra_ui(X_CITY_STATUS)) {
    extras_city_status_draw();   /* X31 */
    return;
  }
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  top_y = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface(&advisor_state.window);
  text_set_font(&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = label_string(label_table.string_ids[0x14e]);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  gov_index = int_clamp(civs[civ].government - 1,0,bugfix_ui() ? 5 : 4);  /* B35 */
  text_append_label(gov_index + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = civ_name_plural(civ);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  text = (char *)civ_leader_title(civ);
  text_append(text);
  text_append_space();
  text = civ_leader_name(civ);
  text_append(text);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  advisor_state.list_top = top_y + line_height;
  top_y = advisor_state.list_top + 2;
  line_height = 0x18;
  advisor_state.row_height = 0x18;
  advisor_state.list_top += 10;
  advisor_state.list_height = list_bottom - top_y;
  advisor_state.rows_per_page = int_clamp((list_bottom - top_y) / 0x18,1,99);
  city_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_count++;
    }
  }
  rows_per_page = advisor_state.rows_per_page;
  advisor_state.page_count = int_clamp((advisor_state.rows_per_page - 1 + city_count) / advisor_state.rows_per_page,1,99);
  max_row = int_clamp(city_count - 1,0,999);
  advisor_state.first_row = int_clamp(advisor_state.first_row,0,max_row);
  row = 0;
  row_y = top_y;
  first_row2 = advisor_state.first_row;
  for (city = 0; city < game.num_cities; city++) {
    next_row = row;
    if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
        (next_row = row + 1, first_row2 <= (int)row)) && ((int)row < first_row2 + rows_per_page))
    {
      col_x = (next_row & 1) * 0x40 + advisor_state.client_x + 2;
      row = next_row;
      gfx_draw_city(&advisor_state.window,city,0,col_x,row_y,0);
      text_set_font(&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      text_y2 = row_y + 9;
      col_x = advisor_state.client_x + 0x8c;
      gfx_draw_text((const char *)&cities[city].name,col_x,text_y2);
      col_x += 0x69;
      draw_x = col_x;
      for (resource = 0; resource < 3; resource++) {
        text_clear();
        if (resource == 0) {
          text_append_int(cities[city].total_food);
        }
        else if (resource == 1) {
          text_append_int(cities[city].total_shields);
        }
        else {
          text_append_int(cities[city].trade);
        }
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + resource * 0x78))->draw((RECT *)sprite_rect,&advisor_state.window,
                   draw_x,text_y2);
        draw_x += 0x12;
      }
      col_x += 0x78;
      draw_x = col_x;
      text_clear();
      if (cities[city].building < '\0') {
        improvement = -(int)cities[city].building;
        text_append_string_id(improvements[-(int)cities[city].building].name);
        text_append_space();
        if (0x26 < improvement) {
          text_set_colors(0x5e,10,0xffffffff,0xffffffff);
        }
        build_cost = (uint)improvements[improvement].cost;
      }
      else {
        text_set_colors(0x7a,10,0xffffffff,0xffffffff);
        unit_type = (iptr)cities[city].building;
        text_append_string_id(unit_types[unit_type].name);
        text_append_space();
        build_cost = (uint)(char)unit_types[unit_type].cost;
      }
      draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
      draw_x += 4;
      text_clear();
      text_append_lparen();
      text_append_int(cities[city].shield_box);
      text_append((const char *)&str_empty_33);
      text_append_int(cosmic.shield_box_rows * build_cost);
      text_append_rparen();
      text_set_colors(0x21,0x12,0xffffffff,0xffffffff);
      gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
      row_y += line_height;
      next_row = row;
    }
    row = next_row;
  }
  if (advisor_state.controls_created == 0) {
    rect_set((RECT *)scroll_rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,advisor_state.client_right - 2,
                       list_bottom);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((Wnd *)&advisor_state.wnd,200,(RECT *)scroll_rect,1);
    max_row = int_clamp(city_count - 1,0,999);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,max_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(advisor_state.rows_per_page);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(city_status_on_scroll);
    ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
    ((WndHandlers *)((i8 *)&advisor_state.wnd_events))->set_lbuttonup_handler(city_status_on_click);
    advisor_state.controls_created = 1;
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 0042d618 auto */
void __cdecl city_status_on_scroll(i32 pos)

{
  advisor_state.first_row = pos;
  city_status_draw();
  return;
}

/* 0042d635 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_status_on_click(i32 mouse_x,int mouse_y)

{
  int clicked_row;
  bool is_match;
  iptr city;
  int row;
  
  if (extra_ui(X_CITY_STATUS)) {
    extras_city_status_click(mouse_x,mouse_y);   /* X31 */
    return;
  }
  if ((-1 < mouse_y - advisor_state.list_top) &&
     (clicked_row = (mouse_y - advisor_state.list_top) / advisor_state.row_height, clicked_row < advisor_state.rows_per_page)) {
    row = 0;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner == advisor_state.report_civ)) &&
         (is_match = row == clicked_row + advisor_state.first_row, row = row + 1, is_match)) {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
        return;
      }
    }
  }
  return;
}

/* 0042d71e auto */
void __cdecl city_status_show(int civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(1,1,0,600,400,0,0);
  advisor_state.report_civ = civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(city_status_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  return;
}

/* 00431573 auto */
/* WARNING: Removing unreachable block (ram,0x00431bb2) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl wonders_report_draw(void)

{
  char *text;
  i32 value;
  int result;
  i32 shadow_color;
  i32 shadow_dx;
  i32 shadow_dy;
  i32 label_id;
  i8 sprite_rect[16];
  iptr report_civ2;
  int row_y;
  int list_bottom;
  int first_row2;
  int draw_x;
  iptr improvement;
  int box_bottom;
  int top_y;
  int row;
  iptr city;
  int box_top;
  int text_width;
  int list_width2;
  int text_y2;
  iptr wonder;
  int civ;
  int total_rows;
  int line_height;
  iptr era;
  i8 rect[16];
  int rows_per_page;
  int era_counts[4];
  
  report_civ2 = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  top_y = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface(&advisor_state.window);
  text_set_font(&advisor_font_block.advisor_font);
  text_set_colors(0x29,0x12,2,1);
  for (era = 0; era < 4; era++) {
    era_counts[era] = 0;
  }
  total_rows = 0;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    if (game.wonder_city[wonder] != -1) {
      era = wonder / 7;
      result = total_rows + 1;
      if (era_counts[era] == 0) {
        result = total_rows + 2;
      }
      total_rows = result;
      era_counts[era]++;
    }
  }
  if (total_rows == 0) {
    text_clear();
    text = label_string(label_table.string_ids[0x154]);
    text_append(text);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
    top_y = top_y + line_height + 4;
    text_clear();
    text_append((const char *)&str_empty_38);
    text_append_label(0x125);
    text_append((const char *)&str_empty_39);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
    top_y += line_height;
  }
  line_height = 0x28;
  advisor_state.row_height = 0x28;
  advisor_state.list_top = top_y;
  advisor_state.list_height = list_bottom - top_y;
  advisor_state.rows_per_page = int_clamp((list_bottom - top_y) / 0x28,1,99);
  rows_per_page = advisor_state.rows_per_page;
  advisor_state.page_count = int_clamp((advisor_state.rows_per_page - 1 + total_rows) / advisor_state.rows_per_page,1,99);
  value = int_clamp(total_rows - 1,0,999);
  advisor_state.first_row = int_clamp(advisor_state.first_row,0,value);
  row_y = top_y;
  row = 0;
  for (era = 0; era < 4; era++) {
    era_counts[era] = 0;
  }
  first_row2 = advisor_state.first_row;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    if (game.wonder_city[wonder] != -1) {
      improvement = wonder + 0x27;
      era = wonder / 7;
      if (era_counts[era] == 0) {
        era_counts[era]++;
        if ((first_row2 <= row) && (row < first_row2 + rows_per_page)) {
          result = text_read_section_line((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_wonders,era);
          if (result == 0) {
            text_y2 = row_y + 0xf;
            text_set_colors(0x29,0x12,1,1);
            gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y2,advisor_state.client_width - (advisor_state.vscroll_width + 2));
          }
          row_y += line_height;
        }
        row++;
      }
      if ((first_row2 <= row) && (row < first_row2 + rows_per_page)) {
        box_top = row_y + 2;
        text_clear();
        if (((era == 0) || ((wonder == WONDER_STATUE_OF_LIBERTY || (wonder == WONDER_EIFFEL_TOWER)))) && (game_language == 0))
        {
          text_append_label(0xd7);
          text_append_space();
        }
        text_append_string_id(improvements[improvement].name);
        text_append_space();
        if (game.wonder_city[wonder] < 0) {
          text_append_lparen();
          text_append_label(0xac);
          text_append_rparen();
          civ = 0;
        }
        else {
          if ((era == 0) && (game_language == 0)) {
            label_id = 0xad;
          }
          else {
            label_id = 0xd8;
          }
          text_append_label(label_id);
          text_append_space();
          city = (iptr)game.wonder_city[wonder];
          text_append(cities[city].name);
          text_append_space();
          text_append_lparen();
          text = (char *)civ_get_adjective(cities[city].owner);
          text_append(text);
          text_append_rparen();
          civ = (int)cities[city].owner;
        }
        text_set_font(&advisor_font_block.advisor_font);
        shadow_dy = 1;
        shadow_dx = 1;
        shadow_color = 0x12;
        value = civ_color_secondary(civ);
        text_set_colors(value,shadow_color,shadow_dx,shadow_dy);
        text_width = text_style.font->text_width((const char *)&textbuf.buf);
        text_width += 0x43;
        list_width2 = advisor_state.client_width - (advisor_state.vscroll_width + 2);
        draw_x = ((list_width2 >> 1) + advisor_state.client_x) - (text_width >> 1);
        ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c))->draw((RECT *)sprite_rect,&advisor_state.window,
                   draw_x,row_y + 10);
        draw_x += 0x42;
        text_y2 = row_y + 8;
        gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        row_y += line_height;
        box_bottom = row_y - 2;
        rect_set((RECT *)rect,advisor_state.client_x + 2,box_top,
                           ((advisor_state.client_width + advisor_state.client_x) - advisor_state.vscroll_width) - 4,box_bottom);
        value = civ_color_primary(civ);
        gfx_draw_rect_frame(&advisor_state.window,(RECT *)rect,value);
      }
      row++;
    }
  }
  if (advisor_state.controls_created == 0) {
    rect_set((RECT *)rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,advisor_state.client_right - 2,
                       list_bottom);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((Wnd *)&advisor_state.wnd,200,(RECT *)rect,1);
    value = int_clamp(total_rows - 1,0,999);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,value);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(advisor_state.rows_per_page);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(wonders_report_on_scroll);
    ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
    advisor_state.controls_created = 1;
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00431c56 auto */
void __cdecl wonders_report_on_scroll(i32 pos)

{
  advisor_state.first_row = pos;
  wonders_report_draw();
  return;
}

/* 00431c73 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl wonders_report_show(int civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(7,7,1,600,400,0,0);
  advisor_state.report_civ = civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(wonders_report_draw);
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
