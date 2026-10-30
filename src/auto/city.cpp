/* The game, cities: growth, production, happiness, the city window and city view: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0040bdac auto */
void __cdecl
taxrate_balance(iptr rate_adjust,iptr rate_first,iptr rate_second,iptr rate_first_cap,iptr lock_first,iptr lock_second)

{
  int max_rate;
  
  max_rate = (int)taxrate_wnd.dlg->max_rate_value;
  while (*(int *)rate_second + *(int *)rate_first + *(int *)rate_adjust < 10) {
    if (((*(int *)rate_first < max_rate) && (*(int *)rate_first < *(int *)rate_first_cap)) && (*(int *)lock_first == 0)
       ) {
      *(int *)rate_first = *(int *)rate_first + 1;
    }
    else if ((*(int *)rate_second < max_rate) && (*(int *)lock_second == 0)) {
      *(int *)rate_second = *(int *)rate_second + 1;
    }
    else if ((*(int *)rate_first < max_rate) && (*(int *)lock_first == 0)) {
      *(int *)rate_first = *(int *)rate_first + 1;
    }
    else {
      *(int *)rate_adjust = *(int *)rate_adjust + 1;
    }
  }
  while (10 < *(int *)rate_second + *(int *)rate_first + *(int *)rate_adjust) {
    if ((*(int *)rate_first == 0) || (*(int *)lock_first != 0)) {
      if ((*(int *)rate_second == 0) || (*(int *)lock_second != 0)) {
        *(int *)rate_adjust = *(int *)rate_adjust - 1;
      }
      else {
        *(int *)rate_second = *(int *)rate_second - 1;
      }
    }
    else {
      *(int *)rate_first = *(int *)rate_first - 1;
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
  taxrate_balance((iptr)&tax_rate2,(iptr)&science_rate2,(iptr)&lux_rate,(iptr)taxrate_wnd.dlg + 0x2f4,
                     (iptr)taxrate_wnd.dlg + 0x350,(iptr)taxrate_wnd.dlg + 0x354);
  taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
  if (requested_rate != tax_rate2) {
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x358))->set_pos(tax_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x358))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->sci_rate_sel != science_rate2) {
    taxrate_wnd.dlg->sci_rate_sel = science_rate2;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x3d8))->set_pos(science_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x3d8))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->lux_rate_sel != lux_rate) {
    taxrate_wnd.dlg->lux_rate_sel = lux_rate;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x398))->set_pos(lux_rate);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x398))->show_and_redraw();
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
  taxrate_balance((iptr)&science_rate2,(iptr)&lux_rate,(iptr)&tax_rate2,(iptr)taxrate_wnd.dlg + 0x2f0,
                     (iptr)taxrate_wnd.dlg + 0x354,(iptr)taxrate_wnd.dlg + 0x34c);
  taxrate_wnd.dlg->sci_rate_sel = science_rate2;
  if (requested_rate != science_rate2) {
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x3d8))->set_pos(science_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x3d8))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->lux_rate_sel != lux_rate) {
    taxrate_wnd.dlg->lux_rate_sel = lux_rate;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x398))->set_pos(lux_rate);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x398))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->tax_rate_sel != tax_rate2) {
    taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x358))->set_pos(tax_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x358))->show_and_redraw();
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
  taxrate_balance((iptr)&lux_rate,(iptr)&tax_rate2,(iptr)&science_rate2,(iptr)taxrate_wnd.dlg + 0x2ec,
                     (iptr)taxrate_wnd.dlg + 0x34c,(iptr)taxrate_wnd.dlg + 0x350);
  taxrate_wnd.dlg->lux_rate_sel = lux_rate;
  if (lux_rate != requested_rate) {
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x398))->set_pos(lux_rate);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x398))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->tax_rate_sel != tax_rate2) {
    taxrate_wnd.dlg->tax_rate_sel = tax_rate2;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x358))->set_pos(tax_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x358))->show_and_redraw();
  }
  if ((int)taxrate_wnd.dlg->sci_rate_sel != science_rate2) {
    taxrate_wnd.dlg->sci_rate_sel = science_rate2;
    ((Scrollbar *)((iptr)taxrate_wnd.dlg + 0x3d8))->set_pos(science_rate2);
    ((Control *)((iptr)taxrate_wnd.dlg + 0x3d8))->show_and_redraw();
  }
  taxrate_wnd.dlg->calc_totals();
  taxrate_dialog_redraw();
  return;
}

/* 0040c3a5 auto */
void __cdecl taxrate_on_ok_clicked(void)

{
  taxrate_wnd.active = 0;
  ((ModalMenuWindow *)((iptr)taxrate_wnd.dlg + 0x48))->end_modal_loop();
  return;
}

/* 0040c3cd auto */
void __cdecl taxrate_on_lock_toggled(int control_id,int checked)

{
  int i;
  
  *(int *)((iptr)taxrate_wnd.dlg + 0x34c + (control_id - 300) * 4) = checked;
  if (checked != 0) {
    for (i = 0; i < 3; i++) {
      if (control_id - 300 != i) {
        *(i32 *)((iptr)taxrate_wnd.dlg + 0x34c + i * 4) = 0;
        ((Checkbox *)(i * 0x3c + 0x418 + (iptr)taxrate_wnd.dlg))->set_checked(0);
        ((Control *)(i * 0x3c + 0x418 + (iptr)taxrate_wnd.dlg))->show_and_redraw()
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
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh(taxrate_state.bg_surface,(iptr)this,0,0,client_x,client_y,dialog_w,
                       dialog_h);
  }
  text_set_surface((iptr)this);
  text_set_font((iptr)&ui_text_font);
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
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,title_y_value,bar_width_value);
  text_set_colors(0x25,0x12,1,1);
  text_clear();
  text_append_int(0);
  text_append_percent();
  text_draw_shadowed((iptr)this,(const char *)&textbuf.buf,label_x_value,tax_label_y);
  text_draw_shadowed((iptr)this,(const char *)&textbuf.buf,label_x_value,sci_label_y);
  text_draw_shadowed((iptr)this,(const char *)&textbuf.buf,label_x_value,lux_label_y);
  text_clear();
  text_append_int(100);
  text_append_percent();
  text_draw_right_aligned((iptr)this,(const char *)&textbuf.buf,label_x_value,tax_label_y,bar_width_value);
  text_draw_right_aligned((iptr)this,(const char *)&textbuf.buf,label_x_value,sci_label_y,bar_width_value);
  text_draw_right_aligned((iptr)this,(const char *)&textbuf.buf,label_x_value,lux_label_y,bar_width_value);
  text_clear();
  text_append_label(0x102);
  text_draw_shadowed((iptr)this,(const char *)&textbuf.buf,
                     bar_width_value + margin_value + margin_value + client_x,
                     tax_label_y);
  text_clear();
  text_append_label(0x59);
  text_append_colon();
  text_append_int(tax_rate_sel * 10);
  text_append_percent();
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,tax_label_y,bar_width_value);
  text_clear();
  text_append_label(0x5b);
  text_append_colon();
  text_append_int(sci_rate_sel * 10);
  text_append_percent();
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,sci_label_y,bar_width_value);
  text_clear();
  text_append_label(0x5a);
  text_append_colon();
  text_append_int(lux_rate_sel * 10);
  text_append_percent();
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,lux_label_y,bar_width_value);
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
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,conceal_button_value,bar_width_value);
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
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x_value,reveal_button_value,bar_width_value);
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
  taxrate_balance((iptr)&init_tax_rate,(iptr)&init_sci_rate,(iptr)&init_lux_rate,
                     (iptr)&init_sci_rate,(iptr)&rates_changed,(iptr)&rates_changed);
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
  (*(DibSurface * *)&taxrate_state.bg_surface)->load_gif_resource(gif_resource_id,10,0xc0,(iptr)&palette);
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
  rect_set_xywh((iptr)rect,left_x,bar_y,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_tax = (u8 *)0x0;
  }
  else {
    parent_wnd_tax = (u8 *)&window;
  }
  tax_scrollbar_obj.create((iptr)parent_wnd_tax,0x65,(iptr)rect,0);
  tax_scrollbar_obj.set_range(0,10);
  tax_scrollbar_obj.set_pos(tax_rate_sel);
  tax_scrollbar_obj.set_callback(taxrate_on_tax_scroll);
  value = bar_y + bar_gap + bar_height;
  sci_label_y = value;
  bar_y = value + row_spacing_value;
  rect_set_xywh((iptr)rect,left_x,bar_y,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_sci = (u8 *)0x0;
  }
  else {
    parent_wnd_sci = (u8 *)&window;
  }
  sci_scrollbar.create((iptr)parent_wnd_sci,0x66,(iptr)rect,0);
  sci_scrollbar.set_range(0,10);
  sci_scrollbar.set_pos(sci_rate_sel);
  sci_scrollbar.set_callback(taxrate_on_science_scroll);
  value = bar_y + bar_gap + bar_height;
  lux_label_y = value;
  rect_set_xywh((iptr)rect,left_x,value + row_spacing_value,bar_width_value,bar_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_lux = (u8 *)0x0;
  }
  else {
    parent_wnd_lux = (u8 *)&window;
  }
  lux_scrollbar.create((iptr)parent_wnd_lux,0x67,(iptr)rect,0);
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
  rect_set_xywh((iptr)rect,client_left + 2,(value + left_x) - (button_height + 2),button_row_width,
                     button_area_height);
  if (this == (TaxRateDialog *)0x0) {
    parent_wnd_ok = (u8 *)0x0;
  }
  else {
    parent_wnd_ok = (u8 *)&window;
  }
  label_text = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)parent_wnd_ok,0xc9,(iptr)rect,(const char *)label_text);
  ok_button.set_callback((CtrlNotifyFn)taxrate_on_ok_clicked);
  ok_button.set_cancel_button();
  ok_button.set_default_button();
  if (ui_metrics.large_screen < 1) {
    checkbox_y += 6;
  }
  for (i = 0; i < 3; i++) {
    *(i32 *)(rate_values + i * 4 - 0x18) =
         taxrate_state.lock_flags[i];
    if (this == (TaxRateDialog *)0x0) {
      parent_wnd_lock = (u8 *)0x0;
    }
    else {
      parent_wnd_lock = (u8 *)&window;
    }
    ((Checkbox *)((u8 *)&advance_pane_rects + i * 0x3c + 0x58))->create((iptr)parent_wnd_lock,
               i + 300,label_center_x - 0x10,checkbox_y,(const char *)&str_empty_9);
    ((Checkbox *)((u8 *)&advance_pane_rects + i * 0x3c + 0x58))->set_checked(
               *(i32 *)(rate_values + i * 4 - 0x18));
    ((Checkbox *)((u8 *)&advance_pane_rects + i * 0x3c + 0x58))->set_callback(taxrate_on_lock_toggled)
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
         *(i32 *)(rate_values + i * 4 - 0x18);
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
void __cdecl taxrate_dialog_show(iptr civ)

{
  TaxRateDialog dialog2;
  
  dialog2.construct();
  ON_SCOPE_EXIT(dialog2.destruct());
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
void __cdecl taxrate_enforce_max_rate(iptr civ)

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
TaxRateDialog *TaxRateDialog::construct()

{
  
  FrameWindow::construct();
  tax_scrollbar_obj.construct();
  lux_scrollbar.construct();
  sci_scrollbar.construct();
  crt_vector_ctor_iterator(rate_lock_checkboxes,0x3c,3,checkbox_elem_ctor,
               checkbox_dtor);
  button_ctor(&ok_button);
  vtable = (u32)&ptr_fun;
  return this;
}

/* 0040f1e0 auto */
void TaxRateDialog::destruct()

{
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0040f25f */ { ctrl_dtor((Button *)(((int)this) + 0x4cc)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0040f26e */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x418),0x3c,3,checkbox_dtor); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0040f286 */ { ((Scrollbar *)(((int)this) + 0x3d8))->destruct(); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0040f295 */ { ((Scrollbar *)(((int)this) + 0x398))->destruct(); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0040f2a4 */ { ((Scrollbar *)(((int)this) + 0x358))->destruct(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0040f2b3 */ { FrameWindow::destruct(); }
  return;
}

/* 004100cf auto */
void __cdecl city_info_popup(iptr city)

{
  iptr adjective;
  int objective_value;
  uptr commodity;
  iptr i;
  Dialog dialog2;
  Sprite sprite;
  uptr demand;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite.create_solid(0x40,0x30,0);
  text_set_string_arg(0,(const char *)&cities[city].name);
  text_clear();
  adjective = civ_get_adjective((int)cities[city].owner);
  text_append((const char *)adjective);
  if (((game.game_flags & GAME_SCENARIO) != 0) && ((scenario.flags[0] & 2) != 0)) {
    objective_value = city_objective_value(city);
    if (objective_value != 0) {
      text_append_space();
      text_append_label(0x1b0);
      if (1 < objective_value) {
        text_append((const char *)&str_x_2);
        text_append_int(objective_value);
      }
    }
  }
  text_set_string_arg(1,(const char *)&textbuf.buf);
  for (i = 0; i < 3; i++) {
    demand = (uptr)*(char *)&cities[city].demands[i];
    text_clear();
    if ((int)demand < 0) {
      text_append_lparen();
    }
    if ((int)demand < 1) {
      commodity = ~demand + 1;
    }
    else {
      commodity = demand;
    }
    text_append_string_id(commodity_names[commodity]);
    if ((int)demand < 0) {
      text_append_rparen();
    }
    text_set_string_arg(i + 2,(const char *)&textbuf.buf);
  }
  popup_game_with_city((const char *)&str_cityinfo,city);
  return;
}

/* 00414e00 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 CityWindow::city_window_is_closed()

{
  return ((u32)is_closed);
}

/* 00428d00 auto */
void __cdecl city_editor_request_redraw(void)

{
  editor_active.wnd->city_editor_draw();
  return;
}

/* 00428d1b auto */
void __cdecl city_editor_on_ok(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  menu_update_states();
  return;
}

/* 00428d48 auto */
void __cdecl city_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_2,(const char *)&str_cities);
  dlg_set_parent_window(0);
  return;
}

/* 00428da7 auto */
void __cdecl city_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 00428dcf auto */
void __cdecl city_editor_on_edit_icon(void)

{
  i32 title_text;
  i32 sprite_count;
  SpriteSheetSaveFn save_callback;
  
  save_callback = sprite_sheet_save_cities;
  sprite_count = 1;
  title_text = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run((iptr)(((i8 *)&gfx_sheet_surface_block.city_sprites) +
                           editor_active.wnd->item_3 * 0x3c +
                           editor_active.wnd->item * 0x1e0 +
                           editor_active.wnd->item_2 * 0x78),(const char *)title_text,sprite_count,save_callback);
  city_editor_request_redraw();
  return;
}

/* 00428e50 auto */
void __cdecl city_editor_on_edit_misc(void)

{
  int choice;
  i32 title_text;
  SpriteSheetSaveFn save_callback;
  iptr parent_wnd2;
  i8 *sprites;
  i32 sprite_count2;
  
  sprite_count2 = 2;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_citymisc,1);
  /* BF-115 (Peter Johnson's ledger): a choice past the 20 sprites (a row
     added to a scenario's DEBUG.TXT) is no choice */
  if ((choice != -1) && (-1 < choice) && (choice < 0x14)) {
    switch(choice) {
    case 0:
      sprites = ((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites);
      break;
    case 1:
      sprites = ((i8 *)&airbase_sprites_block.airbase_sprites);
      break;
    default:
      if (choice < 10) {
        sprites = ((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + (choice * 8 - 0x10) * 0xf;
      }
      else {
        sprites = ((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + (choice * 8 - 0x60) * 0xf;
      }
      sprite_count2 = 3;
      break;
    case 10:
      sprites = ((i8 *)&fortify_overlay_sprites_block.fortress_sprite);
      break;
    case 0xb:
      sprites = ((i8 *)&airbase_sprites_block.airbase_occupied_sprite);
    }
    if (sprites != (i8 *)0x0) {
      save_callback = sprite_sheet_save_cities;
      title_text = (iptr)label_string(label_table.string_ids[0x1f6]);
      sprite_editor_run((iptr)sprites,(const char *)title_text,sprite_count2,save_callback);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 00428fd2 auto */
void __cdecl city_editor_on_combo_change(i32 unused)

{
  i32 selection;
  
  selection = editor_active.wnd->combos[0].get_cur_sel();
  editor_active.wnd->item = selection;
  selection = editor_active.wnd->combos[1].get_cur_sel();
  editor_active.wnd->item_2 = selection;
  selection = editor_active.wnd->combos[2].get_cur_sel();
  editor_active.wnd->item_3 = selection;
  city_editor_request_redraw();
  return;
}

/* 0042903e auto */
void EditorWindow::city_editor_create_combo(int combo_index)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&city_editor_combo_x_table_block.city_editor_combo_x_table))[combo_index * 2] + client_x - 0x1e,
                     (((i32 *)&city_editor_combo_x_table_block.city_editor_combo_y_table))[combo_index * 2] + client_y,0xa0,font_height_uptr << 3);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_callback(city_editor_on_combo_change);
  if (combo_index == 0) {
    label = (iptr)label_string(label_table.string_ids[0x20d]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20e]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20f]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x210]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x211]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x212]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
  }
  else if (combo_index == 1) {
    label = (iptr)label_string(label_table.string_ids[0x213]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x214]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x215]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x216]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
  }
  else if (combo_index == 2) {
    label = (iptr)label_string(label_table.string_ids[0x217]);
    ((ComboBox *)combo_bytes_36c)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x218]);
    ((ComboBox *)combo_bytes_36c)->add_string((const char *)label);
  }
  return;
}

/* 004293a8 auto */
void EditorWindow::city_editor_draw()

{
  byte key_color;
  iptr label;
  i8 draw_rect[16];
  Sprite *sprite;
  int label_y;
  int label_x;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  sprite = (Sprite *)
             (((i8 *)&gfx_sheet_surface_block.city_sprites) +
             item_3_u * 0x3c + item_uptr * 0x1e0 + item_2_uptr * 0x78);
  key_color = sprite->get_key_color();
  gfx_fill_rect_xywh((iptr)this,icon_x,icon_y,0x40,0x40,(uint)key_color);
  sprite->draw_keyed((iptr)draw_rect,(iptr)this,0,icon_x,icon_y + 8);
  gfx_draw_tiled_frame((iptr)this,icon_x,icon_y,0x40,0x40,6);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  label_x = city_editor_combo_x_table_block.city_editor_combo_x_table[0] + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x219]);
  text_append_str((const char *)label);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = city_editor_combo_x_table_block.city_editor_combo2_x + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo2_y + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x21a]);
  text_append_str((const char *)label);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = city_editor_combo_x_table_block.city_editor_combo3_x + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo3_y + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x21b]);
  text_append_str((const char *)label);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  wnd_blit_surface();
  return;
}

/* 00429671 auto */
void EditorWindow::city_editor_open_window()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title_text;
  int button_x;
  int button_y;
  iptr label;
  i32 style;
  i32 arg_x;
  i32 arg_y;
  u32 width;
  u32 height;
  i32 arg_7;
  i32 arg_8;
  i32 arg_9;
  u8 *parent_wnd_misc;
  u8 *parent_wnd_icon;
  u8 *parent_wnd_cancel;
  u8 *parent_wnd_help;
  u8 *parent_wnd_ok;
  DibSurface *bg_surface2;
  int i;
  i8 rect[16];
  GamePalette palette;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  editor_ui.bg_surface = bg_surface2;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorsq_gif,10,0xc0,(iptr)&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  arg_9 = 0;
  arg_8 = 0;
  arg_7 = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  arg_y = 0;
  arg_x = 0;
  style = 0xd;
  title_text = (iptr)label_string(label_table.string_ids[0x1ce]);
  this->FrameWindow::create((const char *)title_text,style,arg_x,arg_y,width,height,arg_7,arg_8,arg_9);
  for (i = 0; i < 3; i++) {
    city_editor_create_combo(i);
  }
  button_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = button_x + 8;
  button_width_u = (int)(client_width - 8) / 3;
  button_y = (client_y + client_height) - (button_height_u + 2);
  button_x = client_x + 2;
  rect_set_xywh((iptr)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_ok = (u8 *)0x0;
  }
  else {
    parent_wnd_ok = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)parent_wnd_ok,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)city_editor_on_ok);
  button_x = button_x + button_width_u + 2;
  rect_set_xywh((iptr)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_help = (u8 *)0x0;
  }
  else {
    parent_wnd_help = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)parent_wnd_help,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)city_editor_on_help);
  rect_set_xywh((iptr)rect,button_x + button_width_u + 2,button_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_cancel = (u8 *)0x0;
  }
  else {
    parent_wnd_cancel = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)parent_wnd_cancel,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)city_editor_on_cancel);
  cancel_button.set_cancel_button();
  button_width_u = 0x4d;
  button_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = button_x + 8;
  button_x = client_x + 0x19;
  button_y = client_y + 0x68;
  rect_set_xywh((iptr)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_icon = (u8 *)0x0;
  }
  else {
    parent_wnd_icon = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((iptr)parent_wnd_icon,0x65,(iptr)rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)city_editor_on_edit_icon);
  rect_set_xywh((iptr)rect,button_x,button_y + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_misc = (u8 *)0x0;
  }
  else {
    parent_wnd_misc = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23c]);
  misc_button.create((iptr)parent_wnd_misc,0x65,(iptr)rect,(const char *)label);
  misc_button.set_callback((CtrlNotifyFn)city_editor_on_edit_misc);
  window.set_keyboard_nav(0);
  item_3_u = 0;
  item_2_uptr = item_3_u;
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  ((ComboBox *)&field_334)->set_cur_sel(item_2_uptr);
  ((ComboBox *)combo_bytes_36c)->set_cur_sel(item_3_u);
  combo_count_u = 3;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(city_editor_request_redraw);
  city_editor_on_combo_change(0xc9);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if (editor_ui.bg_surface != (DibSurface *)0x0) {
    editor_ui.bg_surface->surface_delete(1);
  }
  editor_ui.bg_surface = (DibSurface *)0x0;
  combo_count_u = 0;
  return;
}

/* 00429e77 auto */
void __cdecl city_editor_run(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.city_editor_open_window();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 0042b824 auto */
i32 __cdecl trade_commodity_cities_dialog(iptr civ,iptr commodity)

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
            text_append_int((int)cities[city].size);
          }
          else {
            adjective = civ_get_adjective((int)cities[city].owner);
            text_append((const char *)adjective);
            text_append_comma();
            if (((game.reveal_map == '\0') && (has_improvement = city_has_improvement(city,IMP_PALACE), has_improvement == 0))
               && (*(char *)&cities[city].revealed_size[civ] != '\0')) {
              text_append_int((int)*(char *)&cities[city].revealed_size[civ]);
            }
            else {
              text_append_int((int)cities[city].size);
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
void __cdecl trade_supply_search_dialog(iptr civ)

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
      gfx_draw_city((iptr)&advisor_state.window,city2,0,draw_x,row_y,0);
      text_set_font((iptr)&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      col_x += 0x8a;
      gfx_draw_text((const char *)&cities[city].name,col_x,text_y2);
      col_x += 0x69;
      draw_x = col_x;
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        text_clear();
        text_append_int((int)cities[city].tax);
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)sprite_rect0,(iptr)&advisor_state.window,draw_x,
                   text_y2 + 2);
        draw_x += 0x12;
        text_clear();
        text_append_int((int)cities[city].science);
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((iptr)sprite_rect1,(iptr)&advisor_state.window,draw_x,
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
    text_set_font((iptr)&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    text_clear();
    text_append_label(0x87);
    text_append_colon();
    text_append_int(total_upkeep);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)sprite_rect2,(iptr)&advisor_state.window,draw_x,text_y2 + 2);
  }
  else if (city_count + 2 == item) {
    text_clear();
    text_append_label(0x88);
    text_append_colon();
    text_append_long(total_tax2);
    text_set_font((iptr)&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)sprite_rect3,(iptr)&advisor_state.window,draw_x,text_y2 + 2);
  }
  else if (city_count + 3 == item) {
    text_clear();
    text_append_label(0x16e);
    text_append_colon();
    text_append_int(total_science2);
    text_set_font((iptr)&advisor_bold_font);
    text_set_colors(0x29,10,1,1);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,col_x + 0x8a,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((iptr)sprite_rect4,(iptr)&advisor_state.window,draw_x,text_y2 + 2);
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
    text_set_font((iptr)&advisor_bold_font);
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
      text_set_font((iptr)&advisor_bold_font);
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
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)sprite_rect5,(iptr)&advisor_state.window,draw_x,text_y2 + 2
                );
      draw_x += 0xe;
      text_clear();
      text_append_rparen();
      draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
    }
  }
  else if (improvement_kinds + 1 == item) {
    text_set_font((iptr)&advisor_bold_font);
    text_set_colors(0x7a,10,1,1);
    text_clear();
    text_append_label(0x87);
    text_append_colon();
    text_append_int(total_upkeep);
    draw_x = gfx_draw_text((const char *)&textbuf.buf,right_col_x,text_y2);
    ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)sprite_rect6,(iptr)&advisor_state.window,draw_x,text_y2 + 2);
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
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = label_string(label_table.string_ids[0x152]);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  gov_index = int_clamp(civs[civ].government - 1,0,4);
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
        rect_set((iptr)scroll_rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,
                           advisor_state.client_right - 2,list_bottom);
        ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)scroll_rect,1);
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
        text_set_font((iptr)&advisor_font_block.advisor_font);
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

void __cdecl trade_advisor_show(iptr civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 rect[16];
  
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(5,5,0,600,400,0,0);
  advisor_state.report_civ = civ;
  rect_set_xywh((iptr)rect,2,0,0x129,0x18);
  client_y = advisor_state.client_y;
  rect_h = rect_height((RECT *)rect);
  rect_offset((iptr)rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
  rect_offset((iptr)rect,299,0);
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((i8 *)&advisor_state.close_button))->create((iptr)&advisor_state.wnd,100,(iptr)rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
  ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
  rect_offset((iptr)rect,0xfffffed5,0);
  label = (iptr)label_string(label_table.string_ids[0x16f]);
  ((Button *)((i8 *)&advisor_state.toggle_button))->create((iptr)&advisor_state.wnd,0x65,(iptr)rect,(const char *)label);
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
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = label_string(label_table.string_ids[0x14e]);
  text_append(text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,top_y,advisor_state.client_width);
  top_y += line_height;
  text_clear();
  gov_index = int_clamp(civs[civ].government - 1,0,4);
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
      gfx_draw_city((iptr)&advisor_state.window,city,0,col_x,row_y,0);
      text_set_font((iptr)&advisor_bold_font);
      text_set_colors(0x25,0x12,1,1);
      text_y2 = row_y + 9;
      col_x = advisor_state.client_x + 0x8c;
      gfx_draw_text((const char *)&cities[city].name,col_x,text_y2);
      col_x += 0x69;
      draw_x = col_x;
      for (resource = 0; resource < 3; resource++) {
        text_clear();
        if (resource == 0) {
          text_append_int((uint)cities[city].total_food);
        }
        else if (resource == 1) {
          text_append_int((uint)cities[city].total_shields);
        }
        else {
          text_append_int((int)cities[city].trade);
        }
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        draw_x += 2;
        ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + resource * 0x78))->draw((iptr)sprite_rect,(iptr)&advisor_state.window,
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
      text_append_int((int)cities[city].shield_box);
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
    rect_set((iptr)scroll_rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,advisor_state.client_right - 2,
                       list_bottom);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)scroll_rect,1);
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
void __cdecl city_status_show(iptr civ)

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
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
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
          text = (char *)civ_get_adjective((int)cities[city].owner);
          text_append(text);
          text_append_rparen();
          civ = (int)cities[city].owner;
        }
        text_set_font((iptr)&advisor_font_block.advisor_font);
        shadow_dy = 1;
        shadow_dx = 1;
        shadow_color = 0x12;
        value = civ_color_secondary(civ);
        text_set_colors(value,shadow_color,shadow_dx,shadow_dy);
        text_width = text_style.font->text_width((const char *)&textbuf.buf);
        text_width += 0x43;
        list_width2 = advisor_state.client_width - (advisor_state.vscroll_width + 2);
        draw_x = ((list_width2 >> 1) + advisor_state.client_x) - (text_width >> 1);
        ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c))->draw((iptr)sprite_rect,(iptr)&advisor_state.window,
                   draw_x,row_y + 10);
        draw_x += 0x42;
        text_y2 = row_y + 8;
        gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y2);
        row_y += line_height;
        box_bottom = row_y - 2;
        rect_set((iptr)rect,advisor_state.client_x + 2,box_top,
                           ((advisor_state.client_width + advisor_state.client_x) - advisor_state.vscroll_width) - 4,box_bottom);
        value = civ_color_primary(civ);
        gfx_draw_rect_frame((iptr)&advisor_state.window,(RECT *)rect,value);
      }
      row++;
    }
  }
  if (advisor_state.controls_created == 0) {
    rect_set((iptr)rect,(advisor_state.client_right - advisor_state.vscroll_width) - 2,top_y,advisor_state.client_right - 2,
                       list_bottom);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)rect,1);
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

void __cdecl wonders_report_show(iptr civ)

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

/* 0043cc00 auto */
void __cdecl city_reveal_to_civ(iptr city,iptr civ)

{
  if (-1 < civ) {
    cities[city].known_to = cities[city].known_to | (byte)(1 << ((byte)civ & 0x1f));
    *(i8 *)&cities[city].revealed_size[civ] = cities[city].size;
  }
  return;
}

/* 0043cc7e auto */
int __cdecl city_population_units(iptr city)

{
  int total;
  int i;
  
  total = 0;
  for (i = 1; i <= cities[city].size; i++) {
    total += i;
  }
  if (total < 2) {
    total = 1;
  }
  return total;
}

/* 0043cef9 auto */
uint __cdecl city_objective_value(iptr city)

{
  uint value;
  iptr wonder;
  
  value = (uint)((cities[city].flags & CITYF_OBJECTIVE) != 0);
  if ((scenario.flags[0] & 4) != 0) {
    for (wonder = 0; wonder < 0x1c; wonder++) {
      if (game.wonder_city[wonder] == city) {
        value++;
      }
    }
  }
  return value;
}

/* 0043cf76 auto */
i32 __cdecl city_at(int x,iptr y)

{
  int result;
  iptr city;
  
  result = map_in_bounds(x,y);
  if ((result != 0) && (result = tile_city_owner(x,y), -1 < result)) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].x == x)) &&
         (cities[city].y == y)) {
        return city;
      }
    }
  }
  return 0xffffffff;
}

/* 0043d07a auto */
iptr __cdecl city_find_nearest(iptr x,iptr y,iptr owner_civ,int target_landmass,iptr civ)

{
  short x2;
  short y2;
  int dist;
  int landmass2;
  iptr nearest;
  iptr city;
  
  nearest = -1;
  advisor_window_block.nearest_city_dist = 9999;
  if ((-1 < target_landmass) && (landmass2 = tile_land_landmass(x,y), landmass2 < 0)) {
    target_landmass = -1;
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      x2 = cities[city].x;
      y2 = cities[city].y;
      if (((((target_landmass < 0) || (landmass2 == target_landmass)) &&
           (((owner_civ < 0 || ((int)cities[city].owner == (owner_civ & 0xffU))) ||
            ((-1 < civ && (cities[city].owner == civ)))))) &&
          ((target_landmass != -2 || (dist = city_has_improvement(city,IMP_PORT_FACILITY), dist != 0)))) &&
         (dist = map_distance_diag(x,y,(int)x2,(int)y2), dist <= advisor_window_block.nearest_city_dist))
      {
        nearest = city;
        advisor_window_block.nearest_city_dist = dist;
      }
    }
  }
  return nearest;
}

/* 0043d20a auto */
i32 __cdecl city_has_improvement(iptr city,iptr improvement_id)

{
  iptr improvement;
  byte bit_mask[4];
  i32 result;
  
  result = 0;
  if (((0 < improvement_id) && (improvement_id < 0x23)) &&
     (bit_index_split(improvement_id,(iptr)&improvement,(iptr)bit_mask),
     (bit_mask[0] & cities[city].improvements[improvement]) != 0)) {
    result = 1;
  }
  return result;
}

/* 0043d289: written by hand in src/game/regargs.cpp */

/* Part of city_calc_commodity_demand (0043d400): the terrain of the city radius:
   the count of each terrain, rivers and roads; the scores cleared. */
static void city_commodity_count_terrain(int &road_tiles,int terrain_counts[11],iptr supply_order[16],int &river_tiles,int y3,int x3,iptr demand_order[16])
{
  byte terrain_byte;
  iptr in_bounds;
  int city2;
  byte *tile;
  uint tile_flags;
  int y2;
  uptr terrain2;
  iptr x2;
  iptr i;

  river_tiles = 0;
  for (i = 0; i < 0xb; i++) {
    terrain_counts[i] = 0;
  }
  for (i = 0; i < 0x10; i++) {
    (((int *)&commodity_demand.by_commodity))[i] = 0;
    (((int *)&commodity_supply.by_commodity))[i] = (((int *)&commodity_demand.by_commodity))[i];
    demand_order[i] = i;
    supply_order[i] = demand_order[i];
  }
  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x3);
    y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y3;
    in_bounds = map_in_bounds(x2,y2);
    if (in_bounds != 0) {
      terrain_byte = tile_terrain(x2,y2);
      terrain2 = (uint)terrain_byte;
      terrain_counts[terrain_byte]++;
      city2 = tile_resource(x2,y2);
      if (city2 != 0) {
        terrain_counts[terrain2] += 3;
      }
      tile = (byte *)tile_at(x2,y2);
      if ((*tile & 0x80) != 0) {
        river_tiles++;
      }
      tile_flags = tile_improvements(x2,y2);
      if ((tile_flags & TILE_ROAD) != 0) {
        road_tiles++;
      }
    }
  }
  terrain_counts[6] += terrain_counts[7];
  terrain_counts[7] = 0;
}

/* Part of city_calc_commodity_demand (0043d400): the supply score of each
   commodity, from the terrain, the advances, the size and the place. */
static void city_commodity_supply_scores(uptr city,iptr civ,int terrain_counts[11],int river_tiles,int size_class,uint landmass2,uint south_dist,uint advance_count,uint center_dx,uint east_dist,uint center_dy)
{
  int city2;
  i32 shift;
  int cloth_base;
  int wool_factor;
  iptr wine_bonus;
  int wine_base;

  commodity_supply.by_commodity[0] = terrain_counts[9] * 3 + terrain_counts[6] * 6 + terrain_counts[3] * 4 + river_tiles * 3;
  if ((int)advance_count < 0x10) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] * 2;
  }
  if ((int)advance_count < 0x18) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] << 1;
  }
  if ((int)advance_count < 0x31) {
    if (cities[city].size < 3) {
      commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] << 1;
    }
  }
  else {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] / 2;
  }
  if (7 < cities[city].size) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] / 2;
  }
  wool_factor = terrain_counts[6] + 2;
  if ((int)map.height / 3 < (int)center_dy) {
    wool_factor = terrain_counts[6] + 3;
  }
  commodity_supply.wool[0] = (river_tiles / 2 + terrain_counts[4] * 2 + terrain_counts[2]) * wool_factor;
  commodity_supply.beads = terrain_counts[10] * 8 - center_dy;
  if ('\t' < cities[city].size) {
    commodity_supply.beads = commodity_supply.beads >> 1;
  }
  if (0x20 < (int)advance_count) {
    commodity_supply.beads = commodity_supply.beads >> 1;
  }
  cloth_base = (terrain_counts[1] * 3 + terrain_counts[0]) - river_tiles;
  city2 = int_clamp((int)advance_count / 10,1,2);
  commodity_supply.cloth = cloth_base * city2;
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_supply.cloth = commodity_supply.cloth * 3 >> 1;
  }
  if ((int)advance_count < 8) {
    commodity_supply.cloth = commodity_supply.cloth >> 1;
  }
  if ((int)advance_count < 0x10) {
    commodity_supply.cloth = commodity_supply.cloth >> 1;
  }
  commodity_supply.salt = (terrain_counts[10] * 3 + terrain_counts[0] * 4 + terrain_counts[8] * 2) - (int)advance_count / 6;
  city2 = civ_has_advance(civ,ADV_POTTERY);
  if (city2 == 0) {
    commodity_supply.salt = commodity_supply.salt / 3;
  }
  city2 = city_has_improvement(city,IMP_AQUEDUCT);
  if (city2 != 0) {
    commodity_supply.salt += (commodity_supply.salt >> 1);
  }
  if (((landmass2 & 1) != 0) && ((int)landmass2 < 6)) {
    commodity_supply.salt += (commodity_supply.salt >> 1);
  }
  commodity_supply.coal = (terrain_counts[9] + terrain_counts[1] + terrain_counts[3] + terrain_counts[8] + 1) * terrain_counts[4] *
                 5;
  city2 = size_class / 2;
  if (1 < city2) {
    city2 = 2;
  }
  commodity_supply.coal = int_shift_signed(commodity_supply.coal,city2 - 1);
  if (((landmass2 & 1) != 0) && (1 < (int)landmass2)) {
    commodity_supply.coal += (commodity_supply.coal >> 1);
  }
  if ((int)advance_count < 0x14) {
    commodity_supply.coal = commodity_supply.coal >> 1;
  }
  commodity_supply.copper = terrain_counts[5] * 5 + terrain_counts[4] * 5;
  if ((landmass2 != 0) && ((landmass2 & 1) == 0)) {
    commodity_supply.copper = commodity_supply.copper * 2;
  }
  city2 = (terrain_counts[2] * 5 - terrain_counts[1]) + river_tiles;
  commodity_supply.dye = city2 * 2;
  if ((landmass2 != 0) && ((landmass2 & 3) == 0)) {
    commodity_supply.dye = city2 * 4;
  }
  wine_base = river_tiles * 5 - terrain_counts[2];
  wine_bonus = terrain_counts[1] << 2;
  wine_base = int_clamp(wine_base,0,wine_bonus);
  wine_bonus = int_clamp(wine_bonus,0,wine_base);
  commodity_supply.wine = wine_bonus + (int)(((int)map.width >> 1) - center_dx) / 2 + wine_base;
  if (south_dist != 0) {
    commodity_supply.wine = commodity_supply.wine >> 1;
  }
  shift = int_clamp(size_class - 1,0xfffffffe,1);
  commodity_supply.wine = int_shift_signed(commodity_supply.wine,shift);
  if ((landmass2 - 2 & 3) == 0) {
    commodity_supply.wine += (commodity_supply.wine >> 1);
  }
  if (civs[civ].tribe == 9) {
    commodity_supply.wine = commodity_supply.wine << 1;
  }
  if ('\n' < cities[city].size) {
    commodity_supply.wine = commodity_supply.wine >> 1;
  }
  commodity_supply.silk = (terrain_counts[3] * 2 + terrain_counts[9] + 1) * (terrain_counts[4] + 1);
  if (((commodity_supply.silk != 0) && (commodity_supply.silk = commodity_supply.silk + east_dist * 2, landmass2 != 0)) &&
     ((int)landmass2 % 5 == 0)) {
    commodity_supply.silk = commodity_supply.silk * 2;
  }
  if (civs[civ].tribe == 0xb) {
    commodity_supply.silk = commodity_supply.silk << 1;
  }
  commodity_supply.silver = terrain_counts[5] * 8;
  if (commodity_supply.silver != 0) {
    commodity_supply.silver = commodity_supply.silver + terrain_counts[4] + center_dx;
    city2 = civ_has_advance(civ,ADV_IRON_WORKING);
    if (city2 == 0) {
      commodity_supply.silver = commodity_supply.silver >> 1;
    }
    if (8 < (int)landmass2) {
      commodity_supply.silver += (commodity_supply.silver >> 1);
    }
    if (cities[city].size < 5) {
      commodity_supply.silver = commodity_supply.silver >> 1;
    }
  }
  if (civs[civ].tribe == 10) {
    commodity_supply.silk = commodity_supply.silk << 1;
  }
  commodity_supply.spice = (terrain_counts[9] * 3 + terrain_counts[8] * 2 + terrain_counts[0] * 2) *
                 ((terrain_counts[10] + river_tiles) / 2);
  if (commodity_supply.spice != 0) {
    if ((int)center_dy < 10) {
      commodity_supply.spice = commodity_supply.spice * 2;
    }
    commodity_supply.spice -= center_dy;
    if (landmass_table_block.landmass_recs[landmass2].tiles < 0x1a) {
      commodity_supply.spice += (commodity_supply.spice >> 1);
    }
    if (300 < landmass_table_block.landmass_recs[landmass2].tiles) {
      commodity_supply.spice = commodity_supply.spice >> 1;
    }
    if (landmass2 == 1) {
      commodity_supply.spice = commodity_supply.spice >> 1;
    }
  }
  commodity_supply.gems = (terrain_counts[5] + 1) * (terrain_counts[8] + 1) * (terrain_counts[0] + 1) + terrain_counts[1];
  if (commodity_supply.gems != 0) {
    city2 = int_clamp(size_class,1,4);
    commodity_supply.gems = (city2 * commodity_supply.gems) / 2;
    if (landmass2 == 7) {
      commodity_supply.gems += (commodity_supply.gems >> 1);
    }
  }
  commodity_supply.gold = (terrain_counts[5] + terrain_counts[4] / 2 + 1) * (river_tiles + 2);
  if (2 < terrain_counts[5]) {
    commodity_supply.gold = commodity_supply.gold * 2;
  }
  if (4 < cities[city].size) {
    commodity_supply.gold = commodity_supply.gold << 1;
  }
  if ('\t' < cities[city].size) {
    commodity_supply.gold = commodity_supply.gold << 1;
  }
  commodity_supply.oil = terrain_counts[6] * 8 + terrain_counts[0] * 10 + terrain_counts[8] * 6 + terrain_counts[7] * 0xc;
  if (game.advance_known_by[ADV_COMBUSTION] == '\0') {
    commodity_supply.oil = commodity_supply.oil >> 3;
  }
  if (commodity_supply.oil == 0) {
    commodity_supply.oil = -1;
  }
  else {
    if (landmass2 == 0x11) {
      commodity_supply.oil = commodity_supply.oil * 3;
    }
    else if ((1 < (int)landmass2) && ((landmass2 - 1 & 7) == 0)) {
      commodity_supply.oil += (commodity_supply.oil >> 1);
    }
    city2 = int_clamp(size_class / 2 - 2,1,2);
    commodity_supply.oil = city2 * commodity_supply.oil;
  }
  commodity_supply.uranium = (terrain_counts[6] + terrain_counts[0] + 1) * (terrain_counts[4] + river_tiles + 1) *
                 (terrain_counts[5] + 1);
  city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
  if (city2 == 0) {
    commodity_supply.uranium = 0;
  }
  if (commodity_supply.uranium == 0) {
    commodity_supply.uranium = -1;
  }
  else {
    if ((landmass2 != 0) && ((int)landmass2 % 10 == 0)) {
      commodity_supply.uranium += (commodity_supply.uranium >> 1);
    }
    city2 = size_class;
    if (5 < size_class) {
      city2 = 6;
    }
    commodity_supply.uranium = (commodity_supply.uranium * city2) / 6;
  }
}

/* Part of city_calc_commodity_demand (0043d400): the demand score of each
   commodity. */
static void city_commodity_demand_scores(uptr city,iptr civ,int road_tiles,int terrain_counts[11],int river_tiles,int size_class,uint landmass2,uint advance_count,uint center_dx,uint center_dy)
{
  int city2;
  i32 shift;
  int x4;
  int polar_dist;
  int chunk;
  int remaining_size;
  iptr metal_commodity;
  int weight;

  commodity_demand.by_commodity[0] = (terrain_counts[6] + terrain_counts[7]) * 5 + (int)(center_dy * 3) / 2 + terrain_counts[5] * 2 +
                 terrain_counts[3];
  city2 = ((int)advance_count / 10) * commodity_demand.by_commodity[0];
  commodity_demand.cloth = terrain_counts[4] * 4 + terrain_counts[3] * 4 + ((int)(city2 + (city2 >> 0x1f & 7U)) >> 3)
  ;
  if (cities[city].size < 3) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] * 2;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] / 3;
  }
  city2 = civ_has_advance(civ,ADV_MASS_PRODUCTION);
  if (city2 != 0) {
    commodity_demand.by_commodity[0] = 1;
  }
  if ((int)advance_count < 10) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] << 1;
  }
  if ((int)advance_count < 0x14) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] << 1;
  }
  if (0x2f < (int)advance_count) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] / 2;
  }
  if ((int)map.height >> 2 == center_dy || (int)(((int)map.height >> 2) - center_dy) < 0) {
    polar_dist = ~(((int)map.height >> 2) - center_dy) + 1;
  }
  else {
    polar_dist = ((int)map.height >> 2) - center_dy;
  }
  commodity_demand.wool[0] = polar_dist * 2 + terrain_counts[1] * 2 + terrain_counts[3];
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.wool[0] = commodity_demand.wool[0] << 1;
  }
  commodity_demand.beads = center_dy + ((0x15 - terrain_counts[10]) * 3) / 2;
  if (cities[city].size < 4) {
    commodity_demand.beads += (commodity_demand.beads >> 1);
  }
  if (11 < cities[city].size) {
    commodity_demand.beads = commodity_demand.beads / 2;
  }
  if (0x2f < (int)advance_count) {
    commodity_demand.beads = commodity_demand.beads / 2;
  }
  weight = 8;
  for (remaining_size = (int)cities[city].size; (weight != 0 && (0 < remaining_size));
      remaining_size = remaining_size - chunk) {
    chunk = int_clamp(remaining_size,0,5);
    commodity_demand.salt += chunk * weight;
    weight = weight / 2;
  }
  commodity_demand.salt -= (int)advance_count / 2;
  commodity_demand.coal = (center_dy + 10) * size_class + advance_count;
  if (cities[city].size < 5) {
    commodity_demand.coal = 0;
  }
  if (cities[city].size < 8) {
    commodity_demand.coal = commodity_demand.coal >> 1;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = civ_has_advance(civ,ADV_ELECTRICITY);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = city_has_improvement(city,IMP_POWER_PLANT);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = city_has_improvement(city,IMP_HYDRO_PLANT);
  if (((city2 != 0) || (city2 = city_has_improvement(city,IMP_NUCLEAR_PLANT), city2 != 0)) ||
     (city2 = city_has_improvement(city,IMP_SOLAR_PLANT), city2 != 0)) {
    commodity_demand.coal = commodity_demand.coal >> 3;
  }
  commodity_demand.copper = (river_tiles + road_tiles + 1) * size_class;
  if (commodity_demand.copper <= commodity_supply.copper) {
    commodity_demand.copper = commodity_demand.copper >> 1;
  }
  city2 = city_has_improvement(city,IMP_MARKETPLACE);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = city_has_improvement(city,IMP_BANK);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = civ_has_advance(civ,ADV_ELECTRICITY);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = civ_has_advance(civ,ADV_COMPUTERS);
  if (city2 != 0) {
    commodity_demand.copper = (int)(commodity_demand.copper + (commodity_demand.copper >> 0x1f & 3U)) >> 2;
  }
  if (cities[city].size < 5) {
    commodity_demand.copper = commodity_demand.copper >> 1;
  }
  commodity_demand.dye = commodity_supply.cloth + road_tiles;
  city2 = civ_has_advance(civ,ADV_CHEMISTRY);
  if (city2 != 0) {
    commodity_demand.dye = commodity_demand.dye / 2;
  }
  city2 = civ_has_advance(civ,ADV_MASS_PRODUCTION);
  if (city2 != 0) {
    commodity_demand.dye = commodity_demand.dye / 2;
  }
  if ((int)cities[city].x == (int)cities[city].y ||
      (int)cities[city].x - (int)cities[city].y < 0) {
    x4 = ~((int)cities[city].x - (int)cities[city].y) + 1;
  }
  else {
    x4 = (int)cities[city].x - (int)cities[city].y;
  }
  commodity_demand.wine = size_class * 4 + 4 + x4;
  commodity_demand.silk = ((((int)map.height >> 1) - center_dy) * 2 - center_dx) + ((int)map.width >> 1) +
                 terrain_counts[9] * 4 + terrain_counts[1] / 2 + terrain_counts[8] * 2 + terrain_counts[0] * 4;
  shift = int_clamp(size_class - 1,0xffffffff,1);
  commodity_demand.silk = int_shift_signed(commodity_demand.silk,shift);
  if ((landmass2 == 1) && ((city & 2) != 0)) {
    commodity_demand.silk += (commodity_demand.silk >> 1);
  }
  if (cities[city].size < 7) {
    commodity_demand.silk = commodity_demand.silk >> 1;
  }
  commodity_demand.silver = (int)cities[city].size << 3;
  commodity_demand.gold = 0;
  commodity_demand.gems = 0;
  metal_commodity = 10;
  city2 = ((int)cities[city].y + (int)cities[city].x) % 3;
  if (city2 == 1) {
    metal_commodity = 0xc;
    commodity_demand.gems = commodity_demand.silver;
  }
  else if (city2 == 2) {
    metal_commodity = 0xd;
    commodity_demand.gold = commodity_demand.silver;
  }
  city2 = city_has_improvement(city,IMP_CATHEDRAL);
  if ((city2 != 0) || (city2 = civ_has_wonder(civ,WONDER_MICHELANGELO), city2 != 0)) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] * 3 >> 1;
  }
  city2 = city_has_improvement(city,IMP_BANK);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] * 3 >> 1;
  }
  city2 = civ_has_advance(civ,ADV_ECONOMICS);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] >> 1;
  }
  city2 = civ_has_advance(civ,ADV_COMPUTERS);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] >> 1;
  }
  if (civs[civ].tribe == 0x11) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] << 1;
  }
  city2 = advance_count - 0xc;
  if (city2 < 1) {
    city2 = 0;
  }
  commodity_demand.spice = (int)landmass_table_block.landmass_recs[landmass2].tiles / 10 - city2;
  if (cities[city].size < 4) {
    commodity_demand.spice = commodity_demand.spice / 2;
  }
  if (400 < landmass_table_block.landmass_recs[landmass2].tiles) {
    if (7 < cities[city].size) {
      commodity_demand.spice = commodity_demand.spice << 1;
    }
    if ((-(int)cities[city].x - (int)cities[city].y & 3U) == 0) {
      commodity_demand.spice = 0;
    }
  }
  city2 = civ_has_advance(civ,ADV_REFRIGERATION);
  if (city2 != 0) {
    commodity_demand.spice = commodity_demand.spice >> 1;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 == 0) {
    commodity_demand.oil = -1;
  }
  else {
    commodity_demand.oil = ((int)advance_count / 3) * (size_class + 2);
    if (commodity_demand.oil <= commodity_supply.oil) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
    city2 = city_has_improvement(city,IMP_FACTORY);
    if (city2 == 0) {
      if (cities[city].size < 5) {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
      if (cities[city].size < '\n') {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
      if (cities[city].size < 20) {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
    }
    else {
      commodity_demand.oil = (commodity_demand.oil * 3) / 2;
    }
    city2 = civ_has_advance(civ,ADV_AUTOMOBILE);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil * 3;
    }
    city2 = city_has_improvement(city,IMP_SUPERHIGHWAYS);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil << 1;
    }
    city2 = city_has_improvement(city,IMP_MASS_TRANSIT);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
    city2 = city_has_improvement(city,IMP_RECYCLING_CENTER);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
  }
  city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
  if (city2 == 0) {
    commodity_demand.uranium = -1;
  }
  else {
    commodity_demand.uranium = advance_count * advance_count;
    shift = int_clamp(size_class - 3,0xfffffffd,0);
    commodity_demand.uranium = int_shift_signed(commodity_demand.uranium,shift);
    city2 = city_has_improvement(city,IMP_NUCLEAR_PLANT);
    if ((city2 != 0) || (city2 = city_has_improvement(city,IMP_SDI_DEFENSE), city2 != 0)) {
      commodity_demand.uranium = commodity_demand.uranium << 1;
    }
  }
}

/* Part of city_calc_commodity_demand (0043d400): the three commodities the city
   supplies and demands, the extra ones, and the caravans and trade
   routes kept up to date. */
static void city_commodity_pick(uptr city,iptr civ,iptr supply_order[16],uint advance_count,iptr demand_order[16])
{
  iptr *entry;
  char route_good;
  iptr saved_demand;
  int city2;
  iptr saved_supply;
  iptr unit;
  iptr slot;
  uint special_flags;
  iptr j;
  iptr commodity;
  iptr i;
  uint extra_demand;
  uint extra_supply;

  for (i = 0; i < 0x10; i++) {
    if ((((int *)&commodity_supply.by_commodity))[i] < (((int *)&commodity_demand.by_commodity))[i]) {
      if (0 < (((int *)&commodity_supply.by_commodity))[i]) {
        (((int *)&commodity_supply.by_commodity))[i] = 0;
      }
    }
    else if (0 < (((int *)&commodity_demand.by_commodity))[i]) {
      (((int *)&commodity_demand.by_commodity))[i] = 0;
    }
  }
  int_pairs_bubble_sort(0x10,(i32 *)supply_order,(i32 *)&commodity_supply.by_commodity);
  int_pairs_bubble_sort(0x10,(i32 *)demand_order,(i32 *)&commodity_demand.by_commodity);
  slot = 0xf;
  for (i = 0; i < 3; i++) {
    do {
      entry = supply_order + slot;
      commodity = *entry;
      saved_supply = commodity;
      slot--;
    } while (*(int *)(((i8 *)&commodity_supply.wool) + slot * 4) < 0);
    (*((char *)&commodity + 0)) = (i8)*entry;
    *(i8 *)&cities[city].supplies[i] = (i8)commodity;
    commodity = saved_supply;
  }
  slot = 0xf;
  for (i = 0; i < 3; i++) {
    do {
      entry = demand_order + slot;
      commodity = *entry;
      saved_demand = commodity;
      slot--;
    } while ((((i32 *)&commodity_demand.wool))[slot] < 0);
    (*((char *)&commodity + 0)) = (i8)*entry;
    *(i8 *)&cities[city].demands[i] = (i8)commodity;
    commodity = saved_demand;
  }
  if ((int)advance_count < 0x20) {
    extra_demand = (cities[city].y * 5 + cities[city].x * 3) % 0xe;
    extra_supply = (cities[city].y * 7 + cities[city].x * 0xd) % 0xe;
  }
  else {
    extra_demand = (cities[city].y * 5 + cities[city].x * 3) % 9 + 5;
    extra_supply = (cities[city].y * 7 + cities[city].x * 0xd) % 9 + 5;
  }
  if (extra_supply == extra_demand) {
    extra_supply = 0xfffffffe;
  }
  special_flags = 0;
  for (i = 0; i < 3; i++) {
    if ('\r' < *(char *)&cities[city].demands[i]) {
      special_flags = special_flags | 1;
    }
    if ('\r' < *(char *)&cities[city].supplies[i]) {
      special_flags = special_flags | 2;
    }
    if (((int)*(char *)&cities[city].supplies[i] == extra_demand) ||
       ((int)*(char *)&cities[city].demands[i] == extra_demand)) {
      extra_demand = 0xfffffffe;
    }
    if (((int)*(char *)&cities[city].supplies[i] == extra_supply) ||
       ((int)*(char *)&cities[city].demands[i] == extra_supply)) {
      extra_supply = 0xfffffffe;
    }
  }
  if (((int)extra_demand < 0) && ((special_flags & 1) == 0)) {
    city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
    if ((city2 == 0) || ((extra_demand & 1) == 0)) {
      city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
      if ((city2 != 0) && ((extra_demand & 1) == 0)) {
        extra_demand = 0xf;
      }
    }
    else {
      extra_demand = 0xe;
    }
  }
  if (-1 < (int)extra_demand) {
    cities[city].demands[1] = (u8)extra_demand;
  }
  if (((int)extra_supply < 0) && ((special_flags & 2) == 0)) {
    extra_supply = (extra_supply & 1) + 0xe;
    if ((extra_supply == 0xe) && (city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION), city2 == 0)) {
      extra_supply = 0xffffffff;
    }
    if ((extra_supply == 0xf) && (city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION), city2 == 0)) {
      extra_supply = 0xffffffff;
    }
  }
  if (-1 < (int)extra_supply) {
    cities[city].supplies[1] = (u8)extra_supply;
  }
  for (i = 0; i < (char)cities[city].trade_routes; i++) {
    route_good = *(char *)&cities[city].route_goods[i];
    if (-1 < route_good) {
      for (j = 0; j < 3; j++) {
        if (*(char *)&cities[city].supplies[j] == route_good) {
          *(char *)&cities[city].supplies[j] =
               -*(char *)&cities[city].supplies[j];
        }
      }
      city2 = (int)cities[city].route_partner[i];
      commodity = -1;
      for (j = 0; j < (char)cities[city2].trade_routes; j++) {
        if ((int)cities[city2].route_partner[j] == city) {
          commodity = (iptr)*(char *)&cities[city2].route_goods[j];
        }
      }
      if (-1 < commodity) {
        for (j = 0; j < 3; j++) {
          if (*(char *)&cities[city].demands[j] == commodity) {
            *(char *)&cities[city].demands[j] =
                 -*(char *)&cities[city].demands[j];
          }
        }
      }
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if ((((units[unit].id != 0) && ((byte)units[unit].home_city == city)) &&
        (unit_types[units[unit].type].role == ROLE_TRADE)) && (-1 < (char)units[unit].counter)
       ) {
      for (i = 0; i < 3; i++) {
        if (cities[city].supplies[i] == units[unit].counter) {
          *(char *)&cities[city].supplies[i] =
               -*(char *)&cities[city].supplies[i];
        }
      }
    }
  }
}

/* 0043d400 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_calc_commodity_demand(uptr city)

{
  iptr civ;
  int road_tiles;
  int terrain_counts[11];
  iptr supply_order[16];
  int river_tiles;
  int size_class;
  int west_dist;
  uint landmass2;
  int north_dist;
  int y3;
  uint south_dist;
  int x3;
  uint advance_count;
  uint center_dx;
  uint east_dist;
  iptr demand_order[16];
  uint center_dy;
  /* B1 (docs/BUGS.md): the original never zeroes the road count; its stack
     held 0 there in 687 of 713 calls probed, a stale address in the rest.
     0 in every mode. */
  road_tiles = 0;  /* stale stack the original reads (FRAME_PRESETS) */
  
  if ((((cities[city].flags & CITYF_TRADE_DIRTY) != 0) || (game.reveal_map != '\0')) ||
     (((int)game.turn + city & 0xf) == 0)) {
    cities[city].flags = cities[city].flags & ~CITYF_TRADE_DIRTY;
    x3 = (int)cities[city].x;
    y3 = (int)cities[city].y;
    civ = (iptr)cities[city].owner;
    landmass2 = tile_landmass(x3,y3);
    size_class = (cities[city].size + 2) / 5;
    advance_count = (uint)civs[cities[city].owner].num_advances;
    center_dy = y3 - ((int)map.height >> 1);
    center_dx = x3 - ((int)map.width >> 1);
    west_dist = 0;
    east_dist = 0;
    south_dist = 0;
    north_dist = 0;
    if ((int)center_dy < 0) {
      north_dist = ~center_dy + 1;
    }
    if (0 < (int)center_dy) {
      south_dist = center_dy;
    }
    if (0 < (int)center_dx) {
      east_dist = center_dx;
    }
    if ((int)center_dx < 0) {
      west_dist = ~center_dx + 1;
    }
    if ((int)center_dy < 1) {
      center_dy = ~center_dy + 1;
    }
    if ((int)center_dx < 1) {
      center_dx = ~center_dx + 1;
    }
    city_commodity_count_terrain(road_tiles,terrain_counts,supply_order,river_tiles,y3,x3,demand_order);
    city_commodity_supply_scores(city,civ,terrain_counts,river_tiles,size_class,landmass2,south_dist,advance_count,center_dx,east_dist,center_dy);
    city_commodity_demand_scores(city,civ,road_tiles,terrain_counts,river_tiles,size_class,landmass2,advance_count,center_dx,center_dy);
    city_commodity_pick(city,civ,supply_order,advance_count,demand_order);
  }
  return;
}

/* 0043f493 auto */
void __cdecl city_assign_default_name(iptr city)

{
  char civ;
  int result;
  size_t len;
  int name_label;
  uint skip_count;
  int attempt;
  i8 section_name[0x100];  /* BF-054 (Peter Johnson's ledger): the original's
                             24, less than a civilization's name */
  
  attempt = 0;
  civ = cities[city].owner;
  str_clear((char *)section_name);
  if (leaders[civs[civ].tribe].plural < 1) {
    name_label = ~(int)leaders[civs[civ].tribe].plural + 1;
  }
  else {
    name_label = (int)leaders[civs[civ].tribe].plural;
  }
  str_append_max((char *)section_name,label_string(name_label),sizeof section_name);
  do {
    attempt++;
    if (2 < attempt) break;
    leaders[civs[civ].tribe].city_name_index = leaders[civs[civ].tribe].city_name_index + 1
    ;
    skip_count = (uint)leaders[civs[civ].tribe].city_name_index;
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    result = text_open_section((const char *)&str_city,(const char *)section_name);
    if (result != 0) {
      __chdir(((i8 *)&game_dirs.home));
      result = text_open_section((const char *)&str_city_2,(const char *)section_name);
      if (result != 0) break;
    }
    __chdir(((i8 *)&game_dirs.home));
    for (; 0 < (int)skip_count; skip_count--) {
      text_read_line(1);
      len = _strlen(((i8 *)&textbuf.buf));
      if ((len == 0) || (result = __strnicmp(((i8 *)&textbuf.buf),str_stop,5), result == 0)) break;
    }
    if (0 < (int)skip_count) {
      result = text_open_section(0,(const char *)&str_extra);
      if (result != 0) {
        leaders[civs[civ].tribe].city_name_index = '\0';
        continue;
      }
      for (; 0 < (int)skip_count; skip_count--) {
        text_read_line(1);
        len = _strlen(((i8 *)&textbuf.buf));
        if ((len == 0) || (result = __strnicmp(((i8 *)&textbuf.buf),str_stop_2,5), result == 0))
        break;
      }
    }
    if ((int)skip_count < 1) {
      _memset(cities[city].name,0,0x10);
      _strncpy(cities[city].name,((i8 *)&textbuf.buf),0xf);
      break;
    }
    leaders[civs[civ].tribe].city_name_index = '\0';
  } while( true );
  text_close_file();
  return;
}

/* 0043f7a7 auto */
void __cdecl city_refresh_area_tiles(iptr city)

{
  char civ;
  short x2;
  short y2;
  iptr x;
  int y;
  int in_bounds;
  uint fertility;
  iptr notify;
  iptr tile_idx;
  
  x2 = cities[city].x;
  y2 = cities[city].y;
  civ = cities[city].owner;
  net_batch_begin();
  for (tile_idx = 0; tile_idx < 0x2d; tile_idx++) {
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx] + (int)y2;
    in_bounds = map_in_bounds(x,y);
    if (in_bounds != 0) {
      notify = 1;
      fertility = tile_get_fertility(x,y);
      tile_set_fertility(x,y,fertility & 7,notify);
      if (tile_idx < 0x15) {
        tile_set_radius_civ(x,y,(int)civ,1);
      }
    }
  }
  net_batch_flush();
  return;
}

/* 0043f8b0 auto */
uptr __cdecl city_create(iptr x,iptr y,iptr civ)

{
  char *build_count;
  char is_host;
  byte terrain;
  iptr start_ticks;
  iptr now_ticks;
  iptr can_build;
  iptr x2;
  int y2;
  iptr in_bounds;
  int result;
  i32 start_size;
  byte *tile;
  Tile *map_tile;
  iptr other_civ;
  uptr city;
  iptr unit_type;
  iptr tile_idx;
  int best_score;
  int score;
  
  net.city_create_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    for (city = 0; ((int)city < (int)game.num_cities && (cities[city].id != 0));
        city = city + 1) {
    }
    if ((int)game.num_cities == city) {
      if (0xfe < game.num_cities) {
        if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           (game.multiplayer < 3)) {
          popup_debug_text((const char *)&str_toomanycities);
        }
        net.city_create_busy = 0;
        return 0xffffffff;
      }
      game.num_cities++;
    }
    civs[civ].num_cities++;
    civs[civ].last_city_turn = game.turn;
    tile_set_improvements(x,y,2,1,1);
    tile_set_improvements(x,y,0x40,0,1);
    cities[city].x = (i16)x;
    cities[city].y = (i16)y;
    cities[city].owner = (byte)civ;
    cities[city].id = next_ids.city;
    next_ids.city++;
    cities[city].founder = (byte)civ;
    cities[city].turns_captured = '\0';
    cities[city].flags = 0x20000;
    cities[city].size = 1;
    if (game.wonder_city[WONDER_APOLLO] < 0) {
      cities[city].known_to = '\0';
      for (other_civ = 0; other_civ < 8; other_civ++) {
        *(i8 *)&cities[city].revealed_size[other_civ] = 0;
      }
    }
    else {
      cities[city].known_to = 0xff;
      for (other_civ = 0; other_civ < 8; other_civ++) {
        *(i8 *)&cities[city].revealed_size[other_civ] = 1;
      }
      tile_set_visible(x,y,0xff,1,1);
      for (other_civ = 1; other_civ < 8; other_civ++) {
        tile_update_known(x,y,other_civ);
      }
    }
    cities[city].specialists = 0;
    cities[city].building = UNIT_SETTLERS;
    best_score = 0;
    for (unit_type = 2; unit_type < 0x3e; unit_type++) {
      if ((unit_types[unit_type].role == ROLE_DEFEND) &&
         (can_build = civ_can_build_unit(civ,city,unit_type), can_build != 0)) {
        score = ((int)(char)unit_types[unit_type].defense << 3) /
                  (int)(char)unit_types[unit_type].cost;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          score++;
        }
        if (best_score <= score) {
          best_score = score;
          cities[city].building = (i8)unit_type;
        }
      }
    }
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_count = *build_count + 1;
    cities[city].food_box = 0;
    cities[city].shield_box = 0;
    cities[city].base_trade = 0;
    cities[city].name[0] = '\0';
    city_assign_default_name(city);
    cities[city].worked_tiles = 0;
    _memset(cities[city].improvements,0,5);
    if (civs[civ].num_cities == 1) {
      city_set_improvement(city,1,1);
      civs[civ].capital_x = (i16)x;
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
        net_batch_begin();
        for (tile_idx = 0; tile_idx < 0x14; tile_idx++) {
          x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + x);
          y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx] + y;
          in_bounds = map_in_bounds(x2,y2);
          if ((((in_bounds != 0) &&
               (tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),1,1), 0x27 < game.turn)) &&
              (tile_idx < 8)) &&
             ((map_tile = tile_at(x2,y2), (map_tile->visible & game.human_players) == 0 &&
              (terrain = tile_terrain(x2,y2), terrain_types[terrain].irrigate_to == -2)))) {
            tile_set_improvements(x2,y2,4,1,1);
          }
        }
        net_batch_flush();
        if ((0x28 < game.turn) &&
           (map_tile = tile_at(x,y), (map_tile->visible & game.human_players) == 0)) {
          start_size = int_clamp((game.turn - 0x14) / 0x14,2,10);
          cities[city].size = (i8)start_size;
          city_set_improvement(city,4,1);
          city_set_improvement(city,5,1);
          city_set_improvement(city,6,1);
        }
        if ((0x28 < game.turn) && (cities[city].building != UNIT_SETTLERS)) {
          unit_create((int)cities[city].building,civ,x,y);
        }
      }
    }
    cities[city].trade_routes = '\0';
    _memset(cities[city].route_partner,0,3);
    _memset(cities[city].route_goods,0,3);
    for (tile_idx = 0; tile_idx < 9; tile_idx++) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[tile_idx] + x);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[tile_idx] + y;
      result = map_in_bounds(x2,y2);
      if (result != 0) {
        result = tile_is_ocean(x2,y2);
        if (result != 0) {
          result = tile_landmass(x2,y2);
          if ((1 < landmass_table_block.landmass_recs[result].water_body_size) &&
             ((result < 0x3f || ((cities[city].flags & CITYF_COASTAL) != 0)))) {
            cities[city].flags = cities[city].flags | CITYF_OCEAN_ACCESS;
          }
          cities[city].flags = cities[city].flags | CITYF_COASTAL;
        }
        tile = (byte *)tile_at(x2,y2);
        if (((*tile & 0x80) != 0) || (is_host = tile_terrain(x2,y2), is_host == 5)) {
          cities[city].flags = cities[city].flags | CITYF_HYDRO_POSSIBLE;
        }
      }
    }
    city_refresh_area_tiles(city);
    city_calc_commodity_demand(city);
    cities[city].science = 0;
    cities[city].tax = 0;
    cities[city].trade = 0;
    cities[city].total_food = '\0';
    cities[city].total_shields = '\0';
    cities[city].happy = '\0';
    cities[city].unhappy = '\0';
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.city_create_busy = 0;
  }
  else {
    net.city_create_busy = 0;
    net.nm_city_created_result = 0xfffffffe;
    net_send_message(NM_CREATE_CITY,0,x,y,civ,0,0,0,0,0);
    start_ticks = timer_ticks();
    while ((net.nm_city_created_result == 0xfffffffe && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_city_created_result == 0xfffffffe) {
      dbg_log((const char *)&str_create_city_connection_to_server_timed_o);
      popup_game_with_sprite((const char *)&str_serverconnecttime,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (city = net.nm_city_created_result, net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return city;
}

/* 00440325 auto */
void __cdecl city_remove_trade_route(iptr city,iptr route_index)

{
  iptr slot;
  
  for (slot = route_index; slot < (char)cities[city].trade_routes - 1; slot++)
  {
    cities[city].route_partner[slot] =
         cities[city].route_partner[slot + 1];
    *(i8 *)&cities[city].route_goods[slot] =
         *(i8 *)&cities[city].route_goods[slot + 1];
  }
  cities[city].trade_routes += 0xff;
  cities[city].flags = cities[city].flags | CITYF_TRADE_DIRTY;
  return;
}

/* 004403ec auto */
void __cdecl city_set_trade_route_slot(iptr city,iptr slot,iptr partner,iptr commodity)

{
  i32 goods;
  
  cities[city].route_partner[slot] = (short)partner;
  goods = int_clamp(commodity,0xffffffff,0xf);
  *(char *)&cities[city].route_goods[slot] = (char)goods;
  cities[city].flags = cities[city].flags | CITYF_TRADE_DIRTY;
  return;
}

/* 00440453 auto */
void __cdecl city_add_trade_route(uptr city,uptr city2,iptr commodity)

{
  u8 route_count;
  iptr city3;
  int has_improvement;
  iptr weakest_slot;
  int min_value;
  int road_bonus;
  iptr slot;
  int route_value;
  
  for (slot = 0; slot < (char)cities[city].trade_routes; slot++) {
    if (((int)cities[city].route_partner[slot] == city2) &&
       (*(char *)&cities[city].route_goods[slot] == commodity)) {
      return;
    }
  }
  if ((char)cities[city].trade_routes < 3) {
    route_count = cities[city].trade_routes;
    cities[city].trade_routes++;
    city_set_trade_route_slot(city,(int)(char)route_count,city2,commodity);
    city_calc_commodity_demand(city);
    city_calc_commodity_demand(city2);
  }
  else {
    min_value = 9999;
    weakest_slot = -1;
    for (slot = 0; slot < (char)cities[city].trade_routes; slot++) {
      if ((*(char *)&cities[city].route_goods[slot] < '\0') && (commodity < 0)) {
        return;
      }
      city3 = (iptr)cities[city].route_partner[slot];
      route_value = (int)cities[city3].base_trade;
      road_bonus = map_road_connection((int)cities[city].owner,(int)cities[city].x,
                                    (int)cities[city].y,(int)cities[city3].x,(int)cities[city3].y
                                   );
      has_improvement = city_has_improvement(city,IMP_AIRPORT);
      if (((has_improvement != 0) && (has_improvement = city_has_improvement(city3,IMP_AIRPORT), has_improvement != 0)) && (road_bonus < 2))
      {
        road_bonus = 1;
      }
      if (road_bonus != 0) {
        route_value += (road_bonus * route_value >> 1);
      }
      if (cities[city].owner == cities[city3].owner) {
        route_value = route_value >> 1;
      }
      if (route_value < min_value) {
        min_value = route_value;
        weakest_slot = slot;
      }
    }
    /* BF-026 (Peter Johnson's ledger): with every route worth 9999 or more
       (and a partner whose trade is as much) no slot was picked; the
       original wrote the route at an unset slot */
    if ((-1 < weakest_slot) && (min_value <= cities[city2].base_trade)) {
      city_set_trade_route_slot(city,weakest_slot,city2,commodity);
      city_calc_commodity_demand(city);
      city_calc_commodity_demand(city2);
    }
  }
  return;
}

/* 004413d1 auto */
void __cdecl city_delete(uptr city,i32 from_network)

{
  char is_host;
  int saved_refresh_lock;
  int x2;
  int y2;
  iptr civ;
  uptr city4;
  int y;
  iptr x;
  int result;
  uint fertility;
  iptr notify;
  iptr unit;
  uint nearby_civs;
  iptr city3;
  byte owner_civ;
  iptr city2;
  i8 new_home;
  
  if ((-1 < (int)city) && (cities[city].id != 0)) {
    net.city_delete_busy = 1;
    if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
      saved_refresh_lock = city_wnd.refresh_locked;
      city_wnd.refresh_locked = 1;
      x2 = (int)cities[city].x;
      y2 = (int)cities[city].y;
      civ = (iptr)cities[city].owner;
      civs[civ].num_cities--;
      do {
        unit = (iptr)game.num_units;
        while( true ) {
          while( true ) {
            do {
              unit--;
              if (unit < 0) {
                cities[city].id = 0;
                if ((int)game.num_cities - 1U == city) {
                  game.num_cities--;
                }
                for (city2 = 0; city2 < game.num_cities; city2++) {
                  if (cities[city2].id != 0) {
                    unit = (iptr)(char)cities[city2].trade_routes;
                    while (unit = unit - 1, -1 < unit) {
                      if ((int)cities[city2].route_partner[unit] == city) {
                        city_remove_trade_route(city2,unit);
                      }
                    }
                  }
                }
                for (city2 = 0; city2 < 0x1c; city2++) {
                  if ((int)game.wonder_city[city2] == city) {
                    game.wonder_city[city2] = -2;
                  }
                }
                nearby_civs = 0;
                net_batch_begin();
                tile_set_improvements(x2,y2,2,0,1);
                for (city2 = 0; city2 < 0x2d; city2++) {
                  x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[city2] + x2);
                  y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[city2] + y2;
                  result = map_in_bounds(x,y);
                  if ((result != 0) && (result = tile_is_ocean(x,y), result == 0)) {
                    notify = 1;
                    fertility = tile_get_fertility(x,y);
                    tile_set_fertility(x,y,fertility | 8,notify);
                    if (city2 < 0x15) {
                      result = tile_radius_civ(x,y);
                      if (result == civ) {
                        tile_set_radius_civ(x,y,0,1);
                      }
                      y = tile_city_or_unit_owner(x,y);
                      if ((0 < y) && (y != civ)) {
                        owner_civ = (byte)y;
                        nearby_civs = nearby_civs | 1 << (owner_civ & 0x1f);
                      }
                    }
                  }
                }
                net_batch_flush();
                if (nearby_civs != 0) {
                  for (unit = tile_first_unit(x2,y2); -1 < unit;
                      unit = unit_next_in_stack(unit)) {
                    units[unit].visibility = units[unit].visibility | (byte)nearby_civs;
                  }
                }
                for (city3 = 0; city3 < game.num_cities; city3++) {
                  if (cities[city3].id != 0) {
                    city_refresh_area_tiles(city3);
                  }
                }
                ((CityWindow *)((i8 *)&city_wnd.window))->close_if_city(city);
                city_wnd.refresh_locked = saved_refresh_lock;
                if (saved_refresh_lock == 0) {
                  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
                }
                if (((char)from_network == '\0') && (2 < game.multiplayer)) {
                  net_send_state_diff(0xff,2,0,0,0);
                  net_send_message(NM_CITYWIN_DELETED,0xff,city,0,0,0,0,0,0,0);
                  net_send_message(NM_CITYWIN_REDRAW,0xff,0,0,0,0,0,0,0,0);
                  XD_FlushSendBuffer(5000);
                }
                net.city_delete_busy = 0;
                return;
              }
            } while ((units[unit].id == 0) || ((byte)units[unit].home_city != city));
            if (units[unit].type != UNIT_PARTISANS) break;
            units[unit].home_city = -1;
          }
          if (((((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) ||
              (city4 = city_at((int)units[unit].x,(int)units[unit].y),
              (int)city4 < 0)) || (city == city4)) break;
          if ((unit_types[units[unit].type].role != ROLE_DEFEND) ||
             (y = unit_stack_count_role(unit,ROLE_DEFEND), y != 1)) {
            cities[city4].flags = cities[city4].flags | CITYF_DEFENDER_LOST;
            break;
          }
          new_home = (i8)city4;
          units[unit].home_city = new_home;
        }
        unit_destroy_redraw(unit,1);
      } while( true );
    }
    net.city_delete_busy = 0;
    net.city_deleted_ack = -2;
    net_send_message(NM_DELETE_CITY,0,city,0,0,0,0,0,0,0);
    saved_refresh_lock = timer_ticks();
    while ((net.city_deleted_ack == -2 && (x2 = timer_ticks(), x2 - saved_refresh_lock < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.city_deleted_ack == -2) {
      dbg_log((const char *)&str_delete_city_connection_to_server_timed_o);
      popup_game_with_sprite((const char *)&str_serverconnecttime_2,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 00441a79 auto */
i32 __cdecl city_needed_size_improvement(iptr city)

{
  int has_building;
  
  if (((int)(uint)cosmic.aqueduct_size <= (int)cities[city].size) &&
     (has_building = city_has_improvement(city,IMP_AQUEDUCT), has_building == 0)) {
    return 9;
  }
  if (((int)(uint)cosmic.sewer_size <= (int)cities[city].size) &&
     (has_building = city_has_improvement(city,IMP_SEWER_SYSTEM), has_building == 0)) {
    return 0x17;
  }
  return 0;
}

/* 00441b11 auto */
void __cdecl city_set_production(iptr city,int production)

{
  char *build_counter;
  char old_building;
  byte civ2;
  int civ;
  i32 new_shields;
  char *civ_name;
  int bonus_mult;
  iptr improvement2;
  int old_count;
  iptr improvement;
  iptr old_wonder;
  Dialog dialog2;
  int new_count;
  iptr wonder;
  int civ3;
  int all_humans_have;
  iptr city3;
  iptr wonder_group;
  iptr city2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  old_building = cities[city].building;
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  if ((-1 < cities[city].building) && (cities[city].building < '>')) {
    build_counter = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_counter = *build_counter - 1;
  }
  if (0x62 < production) {
    production = city_choose_production(city,0,0);
  }
  if (production < 99) {
    if (((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) && (turn_flags.civ_turn_in_progress == 0)) {
      new_shields = city_shields_after_change(city,production);
      cities[city].shield_box = (i16)new_shields;
    }
    cities[city].building = (i8)production;
  }
  if ((cities[city].building != old_building) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0)
     ) {
    if ((cities[city].building < -0x26) || (old_building < -0x26)) {
      old_count = 0;
      new_count = 0;
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if (((cities[city2].id != 0) && (cities[city].owner == cities[city2].owner)) &&
           (city2 != city)) {
          if (cities[city2].building == old_building) {
            old_count++;
          }
          if (cities[city].building == cities[city2].building) {
            new_count++;
          }
        }
      }
    }
    if (cities[city].building < -0x26) {
      if (-0x27 < old_building) {
        cities[city].shield_box = cities[city].shield_box / 2;
      }
      if (cities[city].building < 1) {
        improvement = ~(int)cities[city].building + 1;
      }
      else {
        improvement = (iptr)cities[city].building;
      }
      wonder = improvement - 0x27;
      if (old_building < -0x26) {
        old_wonder = -0x27 - old_building;
      }
      else {
        old_wonder = 0;
      }
      civ_name = civ_name_plural(civ);
      text_set_string_arg(1,civ_name);
      if (new_count == 0) {
        if ((old_building < -0x26) && ((old_count == 0 || (game.wonder_city[old_wonder] != -1)))) {
          if (old_count != 0) {
            for (city3 = 0; city3 < game.num_cities; city3++) {
              if (((cities[city3].id != 0) && (cities[city3].owner == civ)) &&
                 (cities[city3].building == old_building)) {
                cities[city3].building = cities[city].building;
              }
            }
          }
          if (autoplay.active == 0) {
            if (old_building < 1) {
              improvement2 = ~(int)old_building + 1;
            }
            else {
              improvement2 = (iptr)old_building;
            }
            text_set_label_arg(2,improvements[improvement2].name);
            text_set_label_arg(3,improvements[improvement].name);
            dialog2.load_game_text((const char *)&str_switchwonder);
            dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c),0,0);
            dialog2.set_image_zoom(8);
            if (2 < game.multiplayer) {
              net_send_text_args(0x13,0xff,4,0,improvement,8);
            }
            dialog2.show_modal(0);
          }
        }
        else if ((((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] & 1) == 0) &&
                ((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] =
                      (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] | 1, autoplay.active == 0)) {
          text_set_label_arg(2,improvements[improvement].name);
          dialog2.load_game_text((const char *)&str_startwonder);
          dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c),0,0);
          dialog2.set_image_zoom(8);
          if (2 < game.multiplayer) {
            net_send_text_args(0x12,0xff,3,0,improvement,8);
          }
          dialog2.show_modal(0);
        }
      }
      if ((cities[city].shield_box == 0) &&
         (wonder_group = (improvement - 0x27) / 7, *(char *)&civs[civ].wonders_by_group[wonder_group] == '\0'
         )) {
        all_humans_have = 1;
        for (civ3 = 1; civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if ((all_humans_have == 0) || (*(char *)&civs[civ3].wonders_by_group[wonder_group] == '\0')) {
              all_humans_have = 0;
            }
            else {
              all_humans_have = 1;
            }
          }
        }
        if (all_humans_have != 0) {
          bonus_mult = (uint)game.difficulty + (uint)civs[civ].wonders_by_group[wonder_group] * -2;
          if (bonus_mult < 1) {
            bonus_mult = 0;
          }
          cities[city].shield_box =
               (short)bonus_mult * (ushort)improvements[improvement].cost + cities[city].shield_box;
        }
      }
    }
    else if ((old_building < -0x26) && (old_count == 0)) {
      if (old_building < 1) {
        improvement = ~(int)old_building + 1;
      }
      else {
        improvement = (iptr)old_building;
      }
      wonder = improvement - 0x27;
      if (((game.wonder_city[improvement - 0x27] != city) &&
          (bonus_mult = civ_has_wonder(civ,improvement - 0x27), bonus_mult == 0)) &&
         ((((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] & 2) == 0 &&
          ((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] = (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] | 2,
          autoplay.active == 0)))) {
        civ_name = civ_name_plural(civ);
        text_set_string_arg(1,civ_name);
        text_set_label_arg(2,improvements[improvement].name);
        dialog2.load_game_text((const char *)&str_abandonwonder);
        dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c),0,0);
        dialog2.set_image_zoom(8);
        if (2 < game.multiplayer) {
          net_send_text_args(0x14,0xff,3,0,improvement,8);
        }
        dialog2.show_modal(0);
      }
    }
  }
  if ((-1 < cities[city].building) && (cities[city].building < '>')) {
    build_counter = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_counter = *build_counter + 1;
  }
  return;
}

/* 0044263f auto */
i32 __cdecl city_adjacent_to_body(iptr city,iptr landmass)

{
  iptr x;
  int y;
  int ok;
  iptr dir;
  
  if (landmass < 0x3f) {
    for (dir = 0; dir < 8; dir++) {
      x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir]);
      y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
      ok = map_in_bounds(x,y);
      if (((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) &&
         (y = tile_landmass(x,y), y == landmass)) {
        return 1;
      }
    }
  }
  return 0;
}

/* 0044272d auto */
int __cdecl city_adjacent_sea_info(iptr city,int *out_max_size,uptr *out_dir)

{
  short x2;
  short y2;
  iptr x;
  int y;
  int ok;
  int body_size;
  int landmass2;
  uptr dir;
  int max_size;
  
  max_size = 0;
  landmass2 = 0;
  x2 = cities[city].x;
  y2 = cities[city].y;
  for (dir = 0; (int)dir < 8; dir++) {
    x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
    ok = map_in_bounds(x,y);
    if ((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) {
      landmass2 = tile_landmass(x,y);
      body_size = (int)landmass_table_block.landmass_recs[landmass2].water_body_size;
      if (0x3e < landmass2) {
        body_size = 1;
      }
      if (max_size <= body_size) {
        max_size = body_size;
      }
      dir += (2 - (dir & 1));
    }
  }
  if (out_max_size != (int *)0x0) {
    *out_max_size = max_size;
  }
  if (out_dir != (uptr *)0x0) {
    *out_dir = dir;
  }
  return landmass2;
}

/* 004429af auto */
i32 __cdecl cities_share_ocean(iptr city,iptr city2)

{
  iptr x;
  int y;
  int ok;
  int last_landmass;
  iptr dir;
  
  last_landmass = -1;
  dir = 0;
  while( true ) {
    if (7 < dir) {
      return 0;
    }
    x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir]);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    ok = map_in_bounds(x,y);
    if ((((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) &&
        (y = tile_landmass(x,y), y != last_landmass)) &&
       (ok = city_adjacent_to_body(city2,y), last_landmass = y, ok != 0)) break;
    dir++;
  }
  return 1;
}

/* 00447e53 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_down_arrow_sprite_global_init(void)

{
  city_down_arrow_sprite_static_init();
  city_down_arrow_sprite_reg_atexit();
  return;
}

/* 00447e6d auto */
void __cdecl city_down_arrow_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite));
  return;
}

/* 00447e87 auto */
void __cdecl city_down_arrow_sprite_reg_atexit(void)

{
  _atexit(city_down_arrow_sprite_static_dtor);
  return;
}

/* 00447ea4 auto */
void __cdecl city_down_arrow_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite));
  return;
}

/* 0044816e auto */
void __cdecl city_sprites_global_init(void)

{
  city_sprites_static_init();
  city_sprites_reg_atexit();
  return;
}

/* 00448188 auto */
void __cdecl city_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&gfx_sheet_surface_block.city_sprites),0x3c,0x30,sprite_init,sprite_destroy);
  return;
}

/* 004481b0 auto */
void __cdecl city_sprites_reg_atexit(void)

{
  _atexit(city_sprites_static_dtor);
  return;
}

/* 004481cd auto */
void __cdecl city_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&gfx_sheet_surface_block.city_sprites),0x3c,0x30,sprite_destroy);
  return;
}

/* 00448376 auto */
void __cdecl citizen_sprites_global_init(void)

{
  citizen_sprites_static_init();
  citizen_sprites_reg_atexit();
  return;
}

/* 00448390 auto */
void __cdecl citizen_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&road_sprites_block.citizen_sprites),0x3c,0x2c,sprite_init,sprite_destroy);
  return;
}

/* 004483b8 auto */
void __cdecl citizen_sprites_reg_atexit(void)

{
  _atexit(citizen_sprites_static_dtor);
  return;
}

/* 004483d5 auto */
void __cdecl citizen_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&road_sprites_block.citizen_sprites),0x3c,0x2c,sprite_destroy);
  return;
}

/* 004484fc auto */
void __cdecl tax_rate_icon_sprites_global_init(void)

{
  tax_rate_icon_sprites_static_init();
  tax_rate_icon_sprites_reg_atexit();
  return;
}

/* 00448516 auto */
void __cdecl tax_rate_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 0044853e auto */
void __cdecl tax_rate_icon_sprites_reg_atexit(void)

{
  _atexit(tax_rate_icon_sprites_static_dtor);
  return;
}

/* 0044855b auto */
void __cdecl tax_rate_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 00448600 auto */
void __cdecl lux_tax_sci_icon_sprites_global_init(void)

{
  lux_tax_sci_icon_sprites_static_init();
  lux_tax_sci_icon_sprites_reg_atexit();
  return;
}

/* 0044861a auto */
void __cdecl lux_tax_sci_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 00448642 auto */
void __cdecl lux_tax_sci_icon_sprites_reg_atexit(void)

{
  _atexit(lux_tax_sci_icon_sprites_static_dtor);
  return;
}

/* 0044865f auto */
void __cdecl lux_tax_sci_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 004486ed auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unhappy_suppressed_icon_sprite_global_init(void)

{
  unhappy_suppressed_icon_sprite_static_init();
  unhappy_suppressed_icon_sprite_reg_atexit();
  return;
}

/* 00448707 auto */
void __cdecl unhappy_suppressed_icon_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite));
  return;
}

/* 00448721 auto */
void __cdecl unhappy_suppressed_icon_sprite_reg_atexit(void)

{
  _atexit(unhappy_suppressed_icon_sprite_static_dtor);
  return;
}

/* 0044873e auto */
void __cdecl unhappy_suppressed_icon_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite));
  return;
}

/* 00448949 auto */
void __cdecl improvement_icon_sprites_global_init(void)

{
  improvement_icon_sprites_static_init();
  improvement_icon_sprites_reg_atexit();
  return;
}

/* 00448963 auto */
void __cdecl improvement_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites),0x3c,0x43,sprite_init,sprite_destroy);
  return;
}

/* 0044898b auto */
void __cdecl improvement_icon_sprites_reg_atexit(void)

{
  _atexit(improvement_icon_sprites_static_dtor);
  return;
}

/* 004489a8 auto */
void __cdecl improvement_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites),0x3c,0x43,sprite_destroy);
  return;
}

/* 0044ac3e auto */
void __cdecl city_sprite_scan_flag_offset(int style,int row,int col,int sheet_x,int sheet_y)

{
  int pixel;
  int dy;
  int dx;
  
  peace_treaty_icon_sprite_block.city_flag_x_offsets[col + style * 8 + row * 2] = 0;
  peace_treaty_icon_sprite_block.city_flag_y_offsets[col + style * 8 + row * 2] = 0;
  for (dx = 0; dx < 0x40; dx++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(dx + sheet_x - 1 + 1,sheet_y - 1);
    if (pixel == 4) {
      peace_treaty_icon_sprite_block.city_flag_x_offsets[col + style * 8 + row * 2] = dx - 1;
    }
  }
  for (dy = 0; dy < 0x30; dy++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(sheet_x - 1,dy + sheet_y - 1 + 1);
    if (pixel == 4) {
      peace_treaty_icon_sprite_block.city_flag_y_offsets[col + style * 8 + row * 2] = dy - 0xf;
    }
  }
  return;
}

/* 0044ad47 auto */
void __cdecl city_sprite_scan_size_box_offset(int style,int row,int col,int sheet_x,int sheet_y)

{
  int pixel;
  int dy;
  int dx;
  
  road_sprites_block.city_size_box_x_offsets[row * 2 + style * 8 + col] = 0;
  road_sprites_block.city_label_y_offsets[row * 2 + style * 8 + col] = 0;
  for (dx = 0; dx < 0x40; dx++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(dx + sheet_x - 1 + 1,sheet_y - 1);
    if (pixel == 3) {
      road_sprites_block.city_size_box_x_offsets[row * 2 + style * 8 + col] = dx;
    }
  }
  for (dy = 0; dy < 0x30; dy++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(sheet_x - 1,dy + sheet_y - 1 + 1);
    if (pixel == 3) {
      road_sprites_block.city_label_y_offsets[row * 2 + style * 8 + col] = dy;
    }
  }
  return;
}

/* 00453da0 auto */
i32 __cdecl wonder_is_expired(iptr wonder)

{
  char expiry_advance;
  int has_advance;
  int civ;
  i32 expired;
  
  expired = 0;
  expiry_advance = wonder_expires[wonder];
  if (expiry_advance < 0) {
    expired = 0;
  }
  else {
    for (civ = 1; civ < 8; civ++) {
      has_advance = civ_has_advance(civ,(int)expiry_advance);
      if (has_advance != 0) {
        expired = 1;
      }
    }
  }
  return expired;
}

/* 00453e18 auto */
int __cdecl wonder_get_city(iptr wonder)

{
  int result;
  
  result = wonder_is_expired(wonder);
  if (result == 0) {
    result = (int)game.wonder_city[wonder];
  }
  else {
    result = -1;
  }
  return result;
}

/* 00453edf auto */
int __cdecl wonder_get_owner(iptr wonder)

{
  iptr city;
  int civ;
  
  city = wonder_get_city(wonder);
  if (city < 0) {
    civ = -1;
  }
  else {
    civ = (int)cities[city].owner;
  }
  return civ;
}

/* 00454260 auto */
void __cdecl cityview_open(uptr city)

{
  CityViewWindow *self;
  int loaded;
  CityViewWindow *this2;
  
  self = (CityViewWindow *)operator_new(0x1cb4);
  if (self == (CityViewWindow *)0x0) {
    this2 = (CityViewWindow *)0x0;
  }
  else {
    this2 = self->construct();
  }
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
CityViewWindow *CityViewWindow::construct()

{
  u8 *slot;
  int i;
  
  this->ModalMenuWindow::construct();
  ((SurfaceWindow *)&control_list)->SurfaceWindow::construct();
  surface_ctor((DibSurface *)&ship_civ);
  ((GamePalette *)((u8 *)&cityview_palette))->init();
  surface_ctor((DibSurface *)((u8 *)&improvement_art_surface));
  surface_ctor((DibSurface *)wonder_art_surface);
  ((DllHandle *)&resource_dll)->init();
  crt_vector_ctor_iterator((u8 *)&slot_sprites,0x3c,0x3c,sprite_init,sprite_destroy);
  crt_vector_ctor_iterator(small_building_sprites_a,0x3c,4,sprite_init,sprite_destroy)
  ;
  crt_vector_ctor_iterator(small_building_sprites_b,0x3c,4,sprite_init,sprite_destroy
              );
  crt_vector_ctor_iterator(large_building_sprites_a,0x3c,4,sprite_init,
               sprite_destroy);
  crt_vector_ctor_iterator(large_building_sprites_b,0x3c,4,sprite_init,
               sprite_destroy);
  surface_ctor((DibSurface *)((u8 *)&icon_surface));
  ((GameFont *)title_font)->init_empty();
  gfx_get_screen_rect((iptr)&tax_rate_icon_sprites_block.cityview_screen_rect);
  cityview.scroll_x = (0x500 - tax_rate_icon_sprites_block.screen_rect_right) / 2;
  mem_copy_src_dst((iptr)&cityview_slot_x_table_block.cityview_slot_tables_init,(iptr)(improvement_slot_tables),0x174);
  mem_copy_src_dst((iptr)&str_empty,(iptr)(wonder_slot_tables),0x120);
  cityview.wnd = this;
  for (i = 0; i < 0x3c; i++) {
    slot = slot_ptr_table_lead + i * 4 + 0xc;
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
  }
  ((GameFont *)title_font)->set_styled(0,0x24,1);
  SetRect((LPRECT)(title_rect),0,0,tax_rate_icon_sprites_block.screen_rect_right,0x28);
  return this;
}

/* 00454699 auto */
void CityViewWindow::destruct()

{
  int seh_state;
  
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 0xd;
  (*(i32 *)&cityview.wnd) = 0;
  ((DllHandle *)((int)this + 0x6d8))->dll_handle_free();
  (*((char *)&seh_state + 0)) = 0xc;
  /* inlined unwind funclet 00454798 */ { ((GameFont *)(((int)this) + 0x1ca8))->destroy(); }
  (*((char *)&seh_state + 0)) = 0xb;
  /* inlined unwind funclet 004547a7 */ { surface_dtor((DibSurface *)(((int)this) + 0x199c)); }
  (*((char *)&seh_state + 0)) = 10;
  /* inlined unwind funclet 004547b6 */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x18ac),0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 9;
  /* inlined unwind funclet 004547ce */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x17bc),0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 8;
  /* inlined unwind funclet 004547e6 */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x16cc),0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 7;
  /* inlined unwind funclet 004547fe */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x15dc),0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 6;
  /* inlined unwind funclet 00454816 */ { crt_vector_dtor_iterator((void *)(((int)this) + 0x6dc),0x3c,0x3c,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 5;
  /* inlined unwind funclet 0045482e */ { ((DllHandle *)(((int)this) + 0x6d8))->free(); }
  (*((char *)&seh_state + 0)) = 4;
  /* inlined unwind funclet 0045483d */ { surface_dtor((DibSurface *)(((int)this) + 0x690)); }
  (*((char *)&seh_state + 0)) = 3;
  /* inlined unwind funclet 0045484c */ { surface_dtor((DibSurface *)(((int)this) + 0x648)); }
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 0045485b */ { ((GamePalette *)(((int)this) + 0x214))->destroy(); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 0045486a */ { surface_dtor((DibSurface *)(((int)this) + 0x1cc)); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00454879 */ { ((SurfaceWindow *)(((int)this) + 0xb8))->SurfaceWindow::destruct(); }
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
    ((DllHandle *)&resource_dll)->load((const char *)path);
    classify_site();
    art_era = civ_city_art_era((int)cities[city].owner);
    this->art_era = art_era;
    if (tax_rate_icon_sprites_block.screen_rect_right < 0x500) {
      ((DibSurface *)&ship_civ)->DibSurface::create(tax_rate_icon_sprites_block.screen_rect_right,0x1e0);
    }
    else {
      ((DibSurface *)&ship_civ)->DibSurface::create(0x500,0x1e0);
    }
    ok = ((DibSurface *)improvement_art_surface)->load_gif_resource(300,10,0xec,
                       (iptr)(cityview_palette));
    if (ok == 0) {
      result = 0;
    }
    else {
      ok = ((DibSurface *)wonder_art_surface)->load_gif_resource(0x131,10,0xec,
                         (iptr)(cityview_palette));
      if (ok == 0) {
        result = 0;
      }
      else {
        ok = art_surface.load_gif_resource(0x136,10,0xec,(iptr)(cityview_palette));
        if (ok == 0) {
          result = 0;
        }
        else {
          src_x = 1;
          src_y = 1;
          for (i = 0; i < 4; i++) {
            ((Sprite *)(building_art_block + i * 0x3c + 0x733))->grab_from_surface((iptr)&art_surface,9
                       ,src_x,src_y,0x9e,0x72);
            ((Sprite *)(building_art_block + i * 0x3c + 0x733))->replace_color(7,9);
            src_x += 0x9f;
          }
          src_x = 1;
          src_y = 0x74;
          ok = civ_city_style_era((int)cities[city].owner);
          if (1 < ok) {
            src_y += 0x73;
          }
          for (i = 0; i < 4; i++) {
            ((Sprite *)(building_art_block + i * 0x3c + 0x823))->grab_from_surface((iptr)&art_surface,9
                       ,src_x,src_y,0x9e,0x72);
            ((Sprite *)(building_art_block + i * 0x3c + 0x823))->replace_color(7,9);
            src_x += 0x9f;
          }
          src_x = 1;
          src_y = 0x15a;
          for (i = 0; i < 4; i++) {
            ((Sprite *)(building_art_block + i * 0x3c + 0x553))->grab_from_surface((iptr)&art_surface,9
                       ,src_x,src_y,0x7b,0x52);
            ((Sprite *)(building_art_block + i * 0x3c + 0x553))->replace_color(7,9);
            src_x += 0x7c;
          }
          src_x = 1;
          src_y = 0x1ad;
          ok = civ_city_style_era((int)cities[city].owner);
          if (1 < ok) {
            src_y += 0x53;
          }
          for (i = 0; i < 4; i++) {
            ((Sprite *)(building_art_block + i * 0x3c + 0x643))->grab_from_surface((iptr)&art_surface,9
                       ,src_x,src_y,0x7b,0x52);
            ((Sprite *)(building_art_block + i * 0x3c + 0x643))->replace_color(7,9);
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
  set_palette((iptr)&cityview_palette);
  ((SurfaceWindow *)&control_list)->SurfaceWindow::show();
  wnd_flush_paint();
  ((WndBase *)frame_wnd_base)->set_capture();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(net_modal_idle_poll);
  }
  ((ModalMenuWindow *)frame_wnd_base)->run_modal_loop();
  ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(0);
  ((WndBase *)frame_wnd_base)->release_capture();
  ((WndBase *)frame_wnd_base)->hide();
  wnd_flush_paint();
  set_palette((iptr)&game_palette_block.game_palette);
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
  
  ((DibSurface *)&icon_surface)->DibSurface::create(0x20,0x20);
  loaded = ((DibSurface *)&icon_surface)->load_gif_resource(399,10,0x20,(iptr)&game_palette_block.game_palette);
  return (uint)(loaded != 0);
}

/* 00455183 auto */
uint CityViewWindow::create_windows()

{
  int ok;
  int art_era2;
  
  create_with_handlers((const char *)&str_empty_74,0x800,tax_rate_icon_sprites_block.cityview_screen_rect,tax_rate_icon_sprites_block.cityview_screen_rect_top,
             tax_rate_icon_sprites_block.screen_rect_right - tax_rate_icon_sprites_block.cityview_screen_rect,(tax_rate_icon_sprites_block.cityview_screen_rect_bottom - tax_rate_icon_sprites_block.cityview_screen_rect_top) + 5,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
  set_bg_color(0);
  ((DibSurface *)&control_list)->wnd_create_with_surface((const char *)&str_empty_75,0x800,0,tax_rate_icon_sprites_block.cityview_screen_rect_bottom - 0x1e0 >> 1,
             tax_rate_icon_sprites_block.screen_rect_right - tax_rate_icon_sprites_block.cityview_screen_rect,0x1e0,(iptr)(cityview_palette),(iptr)this);
  ok = city_has_improvement(city_id,IMP_SUPERHIGHWAYS);
  if (ok == 0) {
    art_era2 = civ_city_art_era((int)cities[city_id].owner);
  }
  else {
    art_era2 = 3;
  }
  ok = ((DibSurface *)&ship_civ)->load_gif_resource(terrain_kind * 5 + art_era2 + 0x154,10,
                     0xec,(iptr)(cityview_palette));
  if (ok != 0) {
    ((WndHandlers *)frame_handlers)->set_keydown_handler(cityview_on_key);
    ((WndHandlers *)frame_handlers)->set_lbuttonup_handler((WndMouseFn)cityview_on_lbuttonup);
    ((WndHandlers *)frame_handlers)->set_keydown_pre_handler(cityview_on_key_pre);
    ((WndHandlers *)frame_handlers)->set_mousemove_handler((WndMouseFn)cityview_on_mousemove);
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
  ((DibSurface *)wonder_art_surface)->release();
  ((DibSurface *)improvement_art_surface)->release();
  return 1;
}

/* 00455add auto */
void CityViewWindow::draw_buildings()

{
  i8 draw_rect[16];
  int i;
  
  for (i = 0; i < (int)slot_count; i++) {
    if (*(int *)(building_art_block + i * 4 + 0x463) != 0) {
      (*(Sprite **)(building_art_block + i * 4 + 0x463))->draw((iptr)draw_rect,
                 (iptr)&ship_civ,
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
void CityViewWindow::place_improvement(iptr slot,iptr improvement)

{
  if (-1 < slot) {
    ((Sprite *)(slot * 0x3c + 0x6dc + (int)this))->grab_from_surface((iptr)&improvement_art_surface,9,
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_x) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_y) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_w) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_h) + (improvement - 1) * 0x10));
    ((Sprite *)(slot * 0x3c + 0x6dc + (int)this))->replace_color(7,9);
    (&slot_sprite_ptrs)[slot] = (int)this + slot * 0x3c + 0x6dc;
  }
  return;
}

/* 00455dfd auto */
void CityViewWindow::place_wonder(iptr slot,iptr wonder)

{
  if (-1 < slot) {
    ((Sprite *)(slot * 0x3c + 0x6dc + (int)this))->grab_from_surface((iptr)((u8 *)this + 0x690),9,
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_x) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_y) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_w) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_h) + wonder * 0x10));
    ((Sprite *)(slot * 0x3c + 0x6dc + (int)this))->replace_color(7,9);
    (&slot_sprite_ptrs)[slot] = (int)this + slot * 0x3c + 0x6dc;
  }
  return;
}

/* 00455ebe auto */
void CityViewWindow::place_large_house(iptr slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (houses_left = houses_left - 1, slot != -1)) {
    *(u8 **)(building_art_block + slot * 4 + 0x463) = building_art_block + variant * 0x3c + 0x823;
  }
  return;
}

/* 00455f2e auto */
void CityViewWindow::place_small_house(iptr slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (houses_left = houses_left - 1, slot != -1)) {
    *(u8 **)(building_art_block + slot * 4 + 0x463) = building_art_block + variant * 0x3c + 0x643;
  }
  return;
}

/* 00455f9e auto */
void CityViewWindow::place_large_empty_lot(iptr slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (slot != -1)) {
    *(u8 **)(building_art_block + slot * 4 + 0x463) = building_art_block + variant * 0x3c + 0x733;
  }
  return;
}

/* 00456005 auto */
void CityViewWindow::place_empty_lot(iptr slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (slot != -1)) {
    *(u8 **)(building_art_block + slot * 4 + 0x463) = building_art_block + variant * 0x3c + 0x553;
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
  ((DibSurface *)&ship_civ)->DibSurface::blit((DibSurface *)&control_list,&src_rect,&dst_rect
            );
  draw_title();
  ((SurfaceWindow *)&control_list)->repaint();
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
  ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(title_font),(const char *)&textbuf.buf,
             (RECT *)title_rect,0);
  gfx_set_text_color_index(0x1a);
  OffsetRect((LPRECT)title_rect,-1,-1);
  ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(title_font),(const char *)&textbuf.buf,
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
      ((ModalMenuWindow *)cityview.wnd->frame_wnd_base)->end_modal_loop();
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
    ((DibSurface *)&ship_civ)->DibSurface::blit((DibSurface *)&control_list,&src_rect,
               &dst_rect);
    draw_title();
    ((DibSurface *)&control_list)->wnd_blit_surface();
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
    ((DibSurface *)&ship_civ)->DibSurface::blit((DibSurface *)&control_list,&src_rect,
               &dst_rect);
    draw_title();
    ((DibSurface *)&control_list)->wnd_blit_surface();
    cityview.scroll_x -= step;
    OffsetRect(&src_rect,-step,0);
    GetCursorPos(&cursor);
  }
  return;
}

/* 00456e90 auto */
void *CityViewWindow::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 00490560 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 CityWindow::get_city()

{
  return ((u32)city_index);
}

/* 00498d40 auto */
void __cdecl city_load_citypref(void)

{
  int result;
  char *line;
  char *_Str1;
  char *_Str2;
  iptr found;
  iptr improvement;
  
  citypref.count = 0;
  result = text_open_section((const char *)&str_citypref_txt,(const char *)&str_nodefend);
  if (result == 0) {
    citypref.no_defend = 1;
  }
  text_close_file();
  result = text_open_section((const char *)&str_citypref_txt_2,(const char *)&str_autobuild);
  if (result == 0) {
    do {
      line = text_read_line(1);
      if (line == 0) break;
      text_next_token();
      found = -1;
      for (improvement = 1; improvement < 0x27; improvement++) {
        _Str2 = ((i8 *)&text_parse.token_buf);
        _Str1 = label_string(improvements[improvement].name);
        result = __strcmpi(_Str1,_Str2);
        if (result == 0) {
          found = improvement;
          break;
        }
      }
      if (0 < found) {
        (((i32 *)&map_views_block.citypref_improvements))[citypref.count] = found;
        citypref.count++;
      }
    } while (citypref.count < 0x20 && 0 < found);
    text_close_file();
  }
  return;
}

/* The state city_choose_production (00498e8b) shares between its parts: the
   survey of the city, its civ and the other cities, and the best choices so
   far. */
struct ChooseProdState {
  byte civ5;
  uptr civ;
  iptr landmass2;
  uint policy;
  uint threatened;
  int space_race;
  int food_surplus;
  int ocean_tiles;
  int shield_surplus;
  int net_trade;
  int wonder_in_progress;
  int wonders_here;
  int wonders_owned;
  int unrest_state;
  int nearest_dist;
  int enemy_strength;
  int farmland_count;
  int wonders_building;
  int airport_count;
  int port_count;
  int unit_total;
  int barracks_count;
  int rivals_ahead;
  int same_build_count;
  int foreign_same_build;
  int space_builders;
  int human_space_builders;
  int palace_count;
  int palace_size;
  int flagged_settlers;
  int homeless_settlers;
  int expand_bonus;
  int settle_goals;
  int enemy_offshore;
  int defense_urgent;
  int defense_need;
  int naval_units;
  /* the original's city loop counter: the palace score reads the value the
     survey left in it (stale) */
  iptr city2;
  int best_build_score;
  int best_unit_score;
  int choice_score;
  iptr best_build;
  iptr best_unit;
  iptr choice;
  int best_raw_score;
  /* the main function's arrays */
  i8 *wonder_builders;
  int *units_in_build;
  int *unit_counts;
};

/* Part of city_choose_production (00498e8b): the tiles around the city:
   ocean tiles, enemy strength next to it, farmland, and for an AI city short
   of food a tile to irrigate, irrigated for free where no human sees it. */
static void city_choose_production_scan_tiles(ChooseProdState *s,iptr city)
{
  byte terrain_byte;
  int ret;
  i32 bits;
  uint adjusted;
  byte *tile;
  Tile *map_tile;
  iptr unit;
  int want_irrigation;
  int y2;
  iptr x2;
  uint tmp;
  uptr terrain2;
  uptr civ2;
  iptr irrigate_tile;
  iptr unit_type2;

  irrigate_tile = -1;
  want_irrigation = 0;
  if ((((s->food_surplus == 0) && ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
      ((cities[city].flags & CITYF_AI_SETTLER_NEAR) == 0)) && (city_calc.settlers_supported == 0)) {
    want_irrigation = 1;
  }
  s->enemy_strength = 0;
  s->farmland_count = 0;
  for (unit_type2 = 0; unit_type2 < 0x14; unit_type2++) {
    x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[unit_type2]);
    y2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[unit_type2];
    ret = map_in_bounds(x2,y2);
    if (ret != 0) {
      terrain_byte = tile_terrain(x2,y2);
      terrain2 = (uptr)terrain_byte;
      if (terrain2 == TERRAIN_OCEAN) {
        s->ocean_tiles++;
        ret = city_tile_worked(city,unit_type2);
        if (ret != 0) {
          s->ocean_tiles += 3;
        }
      }
      civ2 = tile_unit_owner(x2,y2);
      if (((-1 < (int)civ2) && (civ2 != s->civ)) &&
         ((unit = tile_first_unit(x2,y2), -1 < unit &&
          ((int)units[unit].owner != s->civ)))) {
        ret = unit_stack_sum(unit,2);
        s->enemy_strength += ret;
      }
      bits = tile_improvements(x2,y2);
      if (((byte)bits & TILE_FARMLAND) == 0xc) {
        s->farmland_count++;
      }
      if (((want_irrigation != 0) && (ret = ai_tile_improve_choice(s->civ,x2,y2), ret == 1)) &&
         ((adjusted = tile_improvements(x2,y2), (adjusted & TILE_IRRIGATION) == 0 &&
          (terrain_types[terrain2].irrigate_to < '\0')))) {
        irrigate_tile = unit_type2;
        ret = tile_resource(x2,y2);
        if ((ret != 0) && (terrain2 == TERRAIN_PLAINS)) {
          want_irrigation = 0;
        }
      }
    }
  }
  if (-1 < irrigate_tile) {
    x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[irrigate_tile]);
    y2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[irrigate_tile];
    map_tile = tile_at(x2,y2);
    if ((map_tile->visible & game.human_players) == 0) {
      tmp = 4;
      tile = (byte *)tile_at(x2,y2);
      if ((*tile & 0x80) == 0) {
        tmp = tmp | 0x10;
      }
      tile_set_improvements(x2,y2,tmp,1,1);
      s->food_surplus++;
    }
  }
}

/* Part of city_choose_production (00498e8b): the civ's other cities: palaces,
   the same build, spaceship parts, barracks, airports, ports and wonders being
   built on the landmass, the units being built; the wonders other civs build. */
static void city_choose_production_survey_cities(ChooseProdState *s,iptr city)
{
  int ret;

  s->wonders_building = 0;
  s->airport_count = 0;
  s->port_count = 0;
  s->unit_total = 0;
  ret = civ_has_wonder(s->civ,WONDER_SUN_TZU);
  if (ret == 0) {
    s->barracks_count = 0;
  }
  else {
    s->barracks_count = 4;
  }
  s->rivals_ahead = 0;
  s->same_build_count = 0;
  s->foreign_same_build = 0;
  s->space_builders = 0;
  s->human_space_builders = 0;
  s->palace_count = 0;
  s->palace_size = 0;
  for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
    if (cities[s->city2].id != 0) {
      if ((int)cities[s->city2].owner == s->civ) {
        ret = city_has_improvement(s->city2,IMP_PALACE);
        if (((ret != 0) || (cities[s->city2].building == -IMP_PALACE)) &&
           (s->palace_count = s->palace_count + 1, s->palace_size < cities[s->city2].size)) {
          s->palace_size = (int)cities[s->city2].size;
        }
        if (s->city2 != city) {
          if ((cities[city].building == cities[s->city2].building) &&
             (s->same_build_count = s->same_build_count + 1,
             cities[city].total_shields < cities[s->city2].total_shields)) {
            s->rivals_ahead++;
          }
          if ((-0x26 < cities[s->city2].building) && (cities[s->city2].building < -0x22)) {
            s->space_builders++;
          }
          ret = tile_landmass((int)cities[s->city2].x,(int)cities[s->city2].y);
          if (ret == s->landmass2) {
            ret = city_has_improvement(s->city2,IMP_BARRACKS);
            if (ret != 0) {
              s->barracks_count++;
            }
            ret = city_has_improvement(s->city2,IMP_AIRPORT);
            if (ret != 0) {
              s->airport_count++;
            }
            ret = city_has_improvement(s->city2,IMP_PORT_FACILITY);
            if (ret != 0) {
              s->port_count++;
            }
            if (cities[s->city2].building == -IMP_BARRACKS) {
              s->barracks_count++;
            }
            if (cities[s->city2].building < -0x26) {
              s->wonders_building++;
              if (cities[city].total_shields < cities[s->city2].total_shields) {
                *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
                     *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] |
                     (byte)(1 << (s->civ5 & 0x1f));
              }
              ret = (uint)improvements[-(int)cities[s->city2].building].cost * city_calc.shield_rows;
              if (ret - cities[s->city2].shield_box == 0 || ret < cities[s->city2].shield_box)
              {
                *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
                     *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] | 1;
              }
            }
            if (-1 < cities[s->city2].building) {
              s->units_in_build[cities[s->city2].building] = s->units_in_build[cities[s->city2].building] + 1;
            }
          }
        }
      }
      else {
        if (cities[city].building == cities[s->city2].building) {
          s->foreign_same_build++;
        }
        if (cities[s->city2].building < -0x26) {
          *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
               *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] |
               (byte)(1 << (cities[s->city2].owner & 0x1fU));
        }
        if (((((uint)game.human_players & 1 << (cities[s->city2].owner & 0x1fU)) != 0) &&
            (-0x26 < cities[s->city2].building)) && (cities[s->city2].building < -0x22)) {
          s->human_space_builders++;
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the civ's units: settlers
   flagged or without a home city, units of each type on the landmass.
   unit_type is the original's loop counter, left at 0x3e by the loop that
   clears the counts: the settler test reads it so. */
static void city_choose_production_count_units(ChooseProdState *s,iptr unit_type)
{
  int ret;
  iptr unit;

  s->flagged_settlers = 0;
  s->homeless_settlers = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && ((int)units[unit].owner == s->civ)) {
      s->unit_total++;
      if (unit_types[unit_type].role == ROLE_SETTLE) {
        if ((units[unit].flags & UNITF_AI_COLONIST) != 0) {
          s->flagged_settlers++;
        }
        if (units[unit].home_city == -1) {
          s->homeless_settlers++;
          continue;
        }
      }
      ret = tile_landmass((int)units[unit].x,(int)units[unit].y);
      if (ret == s->landmass2) {
        s->unit_counts[units[unit].type] = s->unit_counts[units[unit].type] + 1;
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the bonus for settlers when the
   city is far from the civ's other cities or big. */
static void city_choose_production_expand_bonus(ChooseProdState *s,iptr city)
{
  int dist;

  if ((((cities[city].flags & (CITYF_AI_SETTLER_NEAR | CITYF_AI_TERRAFORM_GOAL)) == 0x80000) && (s->enemy_strength == 0)) &&
     ((city_calc.settlers_supported == 0 && ((s->policy != 1 && (2 < cities[city].size)))))) {
    s->choice_score = 9999;
    for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
      if ((((cities[s->city2].id != 0) && (s->city2 != city)) &&
          ((int)cities[s->city2].owner == s->civ)) &&
         (dist = map_distance((int)cities[city].x,(int)cities[city].y,
                                        (int)cities[s->city2].x,(int)cities[s->city2].y),
         dist < s->choice_score)) {
        s->choice_score = dist;
      }
    }
    if (8 < s->choice_score) {
      s->expand_bonus = 1;
    }
    if (((6 < cities[city].size) && (GOV_FUNDAMENTALISM < civs[s->civ].government)) &&
       ((0 < s->food_surplus && (s->policy != 0)))) {
      s->expand_bonus++;
    }
  }
}

/* Part of city_choose_production (00498e8b): the barbarians' choice: keep the
   unit being built or its upgrade. */
static iptr city_choose_production_barbarian(iptr city)
{
  int ret;
  iptr obsolete_by2;
  iptr unit_type;
  iptr unit_type2;

  if (cities[city].building < 1) {
    if (game.advance_known_by[ADV_EXPLOSIVES] != '\0') {
      return 0x13;
    }
    return 5;
  }
  unit_type = (iptr)cities[city].building;
  obsolete_by2 = (iptr)unit_types[unit_type].obsolete_by;
  if (obsolete_by2 < 0) {
    return unit_type;
  }
  if (game.difficulty == DIFF_CHIEFTAIN) {
    return unit_type;
  }
  ret = bits_count8((int)(char)game.advance_known_by[obsolete_by2]);
  if (ret < 2) {
    return unit_type;
  }
  if (((game.human_players & game.advance_known_by[obsolete_by2]) == 0) && (game.difficulty < DIFF_EMPEROR)) {
    return unit_type;
  }
  unit_type2 = 0;
  while( true ) {
    if (0x3d < unit_type2) {
      return unit_type;
    }
    if ((unit_types[unit_type2].prerequisite == obsolete_by2) &&
       (unit_types[unit_type].role == unit_types[unit_type2].role)) break;
    unit_type2++;
  }
  return unit_type2;
}

/* Part of city_choose_production (00498e8b): the defenders the city needs
   (defense_need, defense_urgent) and its naval units. */
static void city_choose_production_defense_need(ChooseProdState *s,iptr city)
{
  int ret;
  uint adjusted;
  int extra_defender;
  iptr unit;
  int under_threat;
  uint defenders_wanted;
  uint tmp;
  int defenders;
  int size_per_defender;

  under_threat = 0;
  s->defense_urgent = 0;
  s->defense_need = 0;
  unit = tile_first_unit((int)cities[city].x,(int)cities[city].y);
  defenders = unit_stack_count_role(unit,ROLE_DEFEND);
  ret = unit_stack_count_role(unit,ROLE_SEA_TRANSPORT);
  s->naval_units = unit_stack_count_role(unit,ROLE_NAVAL_SUPERIORITY);
  s->naval_units = (ret >> 1) + s->naval_units;
  if (defenders < 1) {
    s->defense_urgent = 2;
    s->defense_need = 2;
  }
  else {
    if ((under_threat == 0) &&
       (ret = tile_zoc_unit_adjacent((int)cities[city].x,(int)cities[city].y,s->civ), ret == 0)
       ) {
      size_per_defender = 4;
    }
    else {
      size_per_defender = 3;
    }
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret == 0) {
      if ((s->policy == 5) && (under_threat == 0)) {
        size_per_defender++;
      }
    }
    else {
      size_per_defender--;
    }
    if ((s->threatened == 0) && (*(char *)&civs[s->civ].continent_flags[s->landmass2] == '\0')) {
      extra_defender = 0;
    }
    else {
      extra_defender = 1;
    }
    defenders_wanted = extra_defender + (int)cities[city].size / size_per_defender;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = defenders_wanted;
    if (ret != 0) {
      tmp = 1;
      if (3 < cities[city].size) {
        tmp = 2;
      }
      if (7 < civs[s->civ].num_cities) {
        tmp = 3;
      }
      if (0xb < civs[s->civ].num_cities) {
        tmp = 4;
      }
      adjusted = tmp;
      if ((int)tmp <= (int)defenders_wanted) {
        adjusted = defenders_wanted;
      }
    }
    defenders_wanted = adjusted;
    if ((defenders < (int)defenders_wanted) && (s->defense_need = 1, defenders + 1 < (int)defenders_wanted)) {
      s->defense_urgent = 1;
    }
  }
}

/* Part of city_choose_production (00498e8b): an AI city sells the
   improvements its civ's wonders make redundant. */
static void city_choose_production_sell_redundant(ChooseProdState *s,iptr city)
{
  int ret;

  ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_GRANARY), ret != 0)) {
    city_set_improvement(city,3,0);
    civs[s->civ].gold += (uint)improvements[IMP_GRANARY].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_SETI);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_RESEARCH_LAB), ret != 0)) {
    city_set_improvement(city,0x1a,0);
    civs[s->civ].gold += (uint)improvements[IMP_RESEARCH_LAB].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_WOMENS_SUFFRAGE);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_POLICE_STATION), ret != 0)) {
    city_set_improvement(city,0x21,0);
    civs[s->civ].gold += (uint)improvements[IMP_POLICE_STATION].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_MICHELANGELO);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_CATHEDRAL), ret != 0)) {
    city_set_improvement(city,0xb,0);
    civs[s->civ].gold += (uint)improvements[IMP_CATHEDRAL].cost * city_calc.shield_rows;
  }
}

/* Part of city_choose_production (00498e8b): an AI city with an airport
   airlifts a defender in from another (and a human's diplomat may see it). */
static void city_choose_production_airlift_defender(ChooseProdState *s,iptr city)
{
  char leader_trait;
  int ret;
  int count;
  iptr unit;
  int watchers;
  iptr unit3;

  ret = city_has_improvement(city,IMP_AIRPORT);
  if ((((ret != 0) && ((int)game.turn_civ == s->civ)) && ((cities[city].flags & CITYF_AIRLIFTED) == 0)
      ) && ((s->policy != 4 && (s->policy != 5)))) {
    for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
      if (((cities[s->city2].id != 0) && ((int)cities[s->city2].owner == s->civ)) &&
         ((s->city2 != city &&
          (((ret = city_has_improvement(s->city2,IMP_AIRPORT), ret != 0 &&
            ((cities[s->city2].flags & CITYF_AIRLIFTED) == 0)) &&
           (ret = tile_adjacent_nonallied_unit((int)cities[s->city2].x,(int)cities[s->city2].y,s->civ),
           ret == 0)))))) {
        ret = tile_landmass((int)cities[s->city2].x,(int)cities[s->city2].y);
        if (((ret == s->landmass2) && (1 < s->defense_need)) ||
            ((ret != s->landmass2) && (civs[s->civ].continent_policy[ret] != s->policy))) {
          unit = tile_first_unit((int)cities[s->city2].x,(int)cities[s->city2].y);
          if (((-1 < unit) &&
              ((s->policy != 1 || (count = unit_stack_count_role(unit,ROLE_DEFEND), 1 < count)))) &&
             (leader_trait = *(char *)&civs[s->civ].continent_policy[ret],
             ret = unit_stack_sum(unit,2),
             (int)(3 - (uint)(leader_trait == 4)) <= ret)) {
            while ((-1 < unit && ((int)(char)unit_types[units[unit].type].role != s->policy))) {
              unit = unit_next_in_stack(unit);
            }
            if (-1 < unit) {
              cities[city].flags = cities[city].flags | CITYF_AIRLIFTED;
              cities[s->city2].flags = cities[s->city2].flags | CITYF_AIRLIFTED;
              unit_exhaust_moves(unit);
              unit_relocate(unit,(int)cities[city].x,(int)cities[city].y,1);
              unit3 = unit;
              watchers = 0;
              for (unit = 0; unit < game.num_units; unit++) {
                if ((((units[unit].id != 0) && (unit_types[units[unit].type].role == ROLE_AIR_SUPERIORITY)) &&
                    (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) &&
                   (((byte)(civs[units[unit].owner].treaties[s->civ] >> 8) & (TREATY_WAR >> 8)) != 0)) {
                  ret = map_distance((int)cities[city].x,(int)cities[city].y,
                                             (int)units[unit].x,(int)units[unit].y);
                  count = unit_max_moves(unit);
                  if (count / (int)(uint)cosmic.road_move_mult < ret) {
                    ret = map_distance((int)cities[s->city2].x,(int)cities[s->city2].y,
                                               (int)units[unit].x,(int)units[unit].y);
                    count = unit_max_moves(unit);
                    if (count / (int)(uint)cosmic.road_move_mult < ret) continue;
                  }
                  ret = rand_range(0,(-(uint)((units[unit].flags & UNITF_VETERAN) == 0) & 2) + 3);
                  if (ret == 0) {
                    unit_delete(unit3,1);
                    unit3 = -1;
                    break;
                  }
                  watchers++;
                }
              }
              if (unit3 < 0) {
                text_set_string_arg(0,(const char *)&cities[city].name);
                popup_game((const char *)&str_weshot);
              }
              else if (watchers != 0) {
                text_set_string_arg(0,(const char *)&cities[city].name);
                popup_game((const char *)&str_wesaw);
              }
            }
            break;
          }
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the score of City Walls. */
static uint city_choose_production_walls_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  iptr civ3;

  ret = civ_has_wonder(s->civ,WONDER_GREAT_WALL);
  if (ret == 0) {
    score = 10 - ((int)cities[city].size >> 1);
    for (civ3 = 1; civ3 < 8; civ3++) {
      if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
        if ((civs[s->civ].treaties[civ3] & (TREATY_VENDETTA | TREATY_WAR)) != 0) {
          score -= 2;
        }
        if (*(char *)&civs[civ3].continent_cities[s->landmass2] != '\0') {
          score--;
        }
      }
    }
    if (game.advance_known_by[ADV_ROBOTICS] != '\0') {
      score += 2;
    }
    if (game.advance_known_by[ADV_ADVANCED_FLIGHT] != '\0') {
      score += 2;
    }
    if (game.advance_known_by[ADV_ROCKETRY] != '\0') {
      score += 2;
    }
    score = int_clamp(score,1,10);
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret != 0) {
      score -= 4;
    }
    *defensive = 1;
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a SAM Missile Battery. */
static uint city_choose_production_sam_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  iptr civ3;
  uint tmp;
  uptr civ2;
  int threat_level;

  threat_level = ((uint)civs[game.best_human_rank].units_active[UNIT_STEALTH_BOMBER] * 2 +
              (uint)civs[game.best_human_rank].units_active[UNIT_BOMBER] * 2 +
              (uint)civs[game.best_human_rank].units_active[UNIT_CRUISE_MISSILE] +
             (uint)civs[game.best_human_rank].units_active[UNIT_HELICOPTER]) * 2;
  if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
    threat_level = 0;
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (civ2 != s->civ) {
        tmp = (uint)civs[civ2].units_active[UNIT_CRUISE_MISSILE] +
                   (uint)civs[civ2].units_active[UNIT_BOMBER] +
                   (uint)civs[civ2].units_active[UNIT_HELICOPTER] +
                   (uint)civs[civ2].units_active[UNIT_STEALTH_BOMBER];
        if (((*((byte *)&civs[s->civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) == 0) &&
           ((*(byte *)&civs[civ2].treaties[s->civ] & TREATY_VENDETTA) == 0)) {
          tmp = tmp / 2;
        }
        threat_level += tmp;
      }
    }
  }
  score = 0xc - threat_level;
  score = int_clamp(score,1,0xc);
  if ((cities[city].flags & CITYF_SAM_UNUSED) != 0) {
    score -= 3;
  }
  *defensive = 1;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
       ((4 < civs[civ3].reputation ||
        (((byte)(civs[s->civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0)))) {
      *defensive = (uint)(s->threatened != 0);
      break;
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a Coastal Fortress. */
static uint city_choose_production_coastal_fortress_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  iptr civ3;
  uint tmp;
  uptr civ2;
  int threat_level;

  ret = city_adjacent_to_body(city,1);
  if (ret != 0) {
    threat_level = (uint)civs[game.best_human_rank].units_active[UNIT_CRUISER] * 3 +
               (uint)civs[game.best_human_rank].units_active[UNIT_AEGIS_CRUISER] * 3 +
               (uint)civs[game.best_human_rank].units_active[UNIT_BATTLESHIP] * 5 +
               (uint)civs[game.best_human_rank].units_active[UNIT_IRONCLAD] +
               (uint)civs[game.best_human_rank].units_active[UNIT_DESTROYER];
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      threat_level = 0;
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        if (civ2 != s->civ) {
          tmp = ((int)(uint)civs[civ2].units_active[UNIT_IRONCLAD] >> 1) +
                     ((int)(uint)civs[civ2].units_active[UNIT_DESTROYER] >> 1) +
                     (uint)civs[civ2].units_active[UNIT_BATTLESHIP] * 2 +
                     (uint)civs[civ2].units_active[UNIT_CRUISER] +
                     (uint)civs[civ2].units_active[UNIT_AEGIS_CRUISER];
          if (((*((byte *)&civs[s->civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) == 0) &&
             ((*(byte *)&civs[civ2].treaties[s->civ] & TREATY_VENDETTA) == 0)) {
            tmp = tmp / 2;
          }
          threat_level += tmp;
        }
      }
    }
    score = 0xc - threat_level;
    score = int_clamp(score,1,0xc);
    *defensive = 1;
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         ((4 < civs[civ3].reputation ||
          (((byte)(civs[s->civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0)))) {
        *defensive = (uint)(s->threatened != 0);
        break;
      }
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a spaceship
   structural, and whether the AI holds back the other builds for it. */
static uint city_choose_production_spaceship_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  uint adjusted;
  int human_launched;
  iptr civ3;
  iptr part_group;
  int own_parts_cost;
  int part;
  int leader_parts_cost;

  ret = civ_spaceship_launched(s->civ);
  if (((ret == 0) && ((game.game_flags & GAME_WON) == 0)) &&
     ((ret = spaceship_choose_part_group(s->civ,0), -1 < ret &&
      (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0 ||
       (civs[s->civ].spaceship_parts[0] < 0x26)))))) {
    score = 4;
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         (ret = civ_spaceship_exists(civ3), ret != 0)) {
        score = 2;
        break;
      }
    }
    if ((s->policy != 0) && ((civs[s->civ].continent_flags[s->landmass2] & 0x10) == 0)) {
      *defensive = s->threatened;
    }
    human_launched = 0;
    if ((game.game_flags & GAME_WON) == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
           (ret = civ_spaceship_launched(civ3), ret != 0)) {
          human_launched = 1;
          break;
        }
      }
    }
    if ((1 < s->space_race) && (game.difficulty != DIFF_CHIEFTAIN)) {
      adjusted = score - 1;
      if (human_launched != 0) {
        adjusted = score - 2;
      }
      score = adjusted;
      if (s->defense_urgent == 0) {
        *defensive = s->threatened;
      }
    }
    leader_parts_cost = 0;
    own_parts_cost = 0;
    for (part = 0; part < 6; part++) {
      part_group = int_clamp((part + 1) / 2,0,2);
      own_parts_cost = own_parts_cost +
                  (int)civs[s->civ].spaceship_parts[part] *
                  (uint)improvements[part_group + 0x23].cost;
      leader_parts_cost = leader_parts_cost +
                 (int)civs[((uint)game.best_human_rank)].spaceship_parts[part] *
                 (uint)improvements[part_group + 0x23].cost;
    }
    if (((s->space_builders == 0) ||
        (((s->space_builders <= s->human_space_builders && (own_parts_cost <= leader_parts_cost)) || (human_launched != 0)))) &&
       ((((cities[city].flags & CITYF_DISORDER) == 0 && (game.difficulty != DIFF_CHIEFTAIN)) &&
        (((int)civs[s->civ].best_shield_surplus >> 1 <= s->shield_surplus || (0x13 < s->shield_surplus)))))) {
      s->choice_score = 999;
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the base score of an
   improvement, and whether it is defensive. */
static uint city_choose_production_improvement_score(ChooseProdState *s,iptr city,iptr improvement,uint *defensive)
{
  int ret;
  uint tmp;
  uint score;

  score = 999;
  *defensive = 0;
  if (improvement - 1U < 0x23) {
    ret = s->food_surplus >> 1;
    switch(improvement) {
    case IMP_PALACE:
      if ((((s->palace_count == 0) || ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
          ((s->palace_count == 0 ||
           ((s->palace_count == 1 && (s->palace_size * 2 < (int)cities[s->city2].size)))))) &&
         ((s->policy == 4 || (((s->policy == 5 || (s->policy == 0)) || (3 < city_calc.shield_waste)))))) {
        score = 2;
        if ('\t' < cities[s->city2].size) {
          score = 1;
        }
        if (14 < cities[s->city2].size) {
          score--;
        }
        if (3 < city_calc.shield_waste) {
          *defensive = s->threatened;
        }
      }
      break;
    case IMP_BARRACKS:
      score = (leaders[civs[s->civ].tribe].civilize + s->barracks_count + 4) -
                 (int)leaders[civs[s->civ].tribe].attack;
      if (((leaders[civs[s->civ].tribe].attack < '\0') ||
          ('\0' < leaders[civs[s->civ].tribe].civilize)) &&
         ((int)leaders[civs[s->civ].tribe].civilize + (int)leaders[civs[s->civ].tribe].attack != 0
         )) {
        score += s->barracks_count;
      }
      *defensive = 1;
      if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
        if (game.difficulty == DIFF_KING) {
          score = (int)(score * 3) / 2;
        }
        if (DIFF_KING < game.difficulty) {
          score = score << 1;
        }
      }
      break;
    case IMP_GRANARY:
      ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS);
      if (ret == 0) {
        if (city_calc.total_shields < 3) {
          score = 8;
        }
        else {
          score = 4;
        }
        if (city_calc.happy_count <= city_calc.unhappy_count) {
          score += 2;
        }
        if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) &&
           (tmp = (game.difficulty + 4) - (int)cities[s->city2].size, 0 < (int)tmp))
        {
          score += tmp;
        }
      }
      break;
    case IMP_TEMPLE:
      if (s->unrest_state == 1) {
        score = 9;
      }
      if (s->unrest_state == 2) {
        score = 0xfffffffb;
      }
      break;
    case IMP_MARKETPLACE:
      score = int_clamp(10 - (s->net_trade >> 1),1,10);
      if (s->unrest_state == 1) {
        score--;
      }
      if (s->unrest_state == 2) {
        score = 0xfffffffc;
      }
      break;
    case IMP_LIBRARY:
      score = int_clamp(10 - s->net_trade / 3,1,10);
      break;
    case IMP_COURTHOUSE:
      score = 0xe - (city_calc.shield_waste * 2 + city_calc.trade_corruption);
      if (civs[s->civ].government == GOV_DEMOCRACY) {
        if (s->unrest_state == 2) {
          score = 0xffffffff;
        }
      }
      else if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) {
        score -= (int)civs[game.best_human_rank].subversion_count / 2;
      }
      break;
    case IMP_CITY_WALLS:
      score = city_choose_production_walls_score(s,city,score,defensive);
      break;
    case IMP_AQUEDUCT:
      if ((int)((uint)cosmic.aqueduct_size - ret) <= (int)cities[city].size) {
        score = ((cosmic.aqueduct_size + 4) - (int)cities[city].size) - ret;
        score = int_clamp(score,1,0x14);
      }
      break;
    case IMP_BANK:
      score = int_clamp(10 - s->net_trade / 3,1,10);
      if (s->unrest_state == 1) {
        score--;
      }
      if (s->unrest_state == 2) {
        score = 0;
      }
      break;
    case IMP_CATHEDRAL:
      ret = civ_has_wonder(s->civ,WONDER_MICHELANGELO);
      if (ret == 0) {
        if (s->unrest_state == 1) {
          score = 8;
        }
        if (s->unrest_state == 2) {
          score = 0xfffffffd;
        }
      }
      break;
    case IMP_UNIVERSITY:
      if ((civs[s->civ].science_rate != '\0') &&
         ((s->space_race == 0 || (ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT), ret == 0)))) {
        score = int_clamp(10 - (s->net_trade >> 2),2,10);
      }
      break;
    case IMP_MASS_TRANSIT:
      break;
    case IMP_COLOSSEUM:
      if (s->unrest_state == 2) {
        score = 0xfffffffe;
      }
      break;
    case IMP_FACTORY:
    case IMP_HYDRO_PLANT:
      score = int_clamp(0xe - city_calc.total_shields,1,0xe);
      break;
    default:
      score = int_clamp(0xc - ((int)(city_calc.total_shields + (city_calc.total_shields >> 0x1f & 3U)) >>
                                          2),2,0xc);
      break;
    case IMP_SDI_DEFENSE:
      if (s->unrest_state < 2) {
        ret = int_clamp(0xf - cities[city].size,1,0xf);
        score = (ret + 1) / 2;
        if (game.wonder_city[WONDER_MANHATTAN] == -1) {
          score = score << 1;
        }
        if (civs[game.best_human_rank].units_active[UNIT_NUCLEAR_MISSILE] == '\0') {
          score = score << 1;
        }
        if ('\t' < cities[city].size) {
          score--;
        }
        ret = city_has_improvement(city,IMP_PALACE);
        if (ret != 0) {
          score = 0;
        }
      }
      *defensive = 1;
      break;
    case IMP_POWER_PLANT:
      score = int_clamp(0xc - city_calc.total_shields / 5,2,0xe);
      break;
    case IMP_STOCK_EXCHANGE:
      if ((s->unrest_state != 0) || (s->space_race < 2)) {
        score = int_clamp(0xb - (s->net_trade >> 2),2,0xb);
        if (s->unrest_state == 1) {
          score--;
        }
        if (s->unrest_state == 2) {
          score = 0;
        }
      }
      break;
    case IMP_SEWER_SYSTEM:
      if ((s->space_race < 2) &&
         ((int)((uint)cosmic.sewer_size - ret) <= (int)cities[city].size)) {
        score = ((cosmic.sewer_size + 4) - (int)cities[city].size) - ret;
        score = int_clamp(score,1,0x14);
      }
      break;
    case IMP_SUPERMARKET:
      score = (0xe - ((int)cities[city].size >> 1)) + s->farmland_count * -2 + s->space_race * 2 + ret
      ;
      score = int_clamp(score,2,0xe);
      break;
    case IMP_SUPERHIGHWAYS:
      score = (0xf - ((int)cities[city].size >> 1)) + s->space_race * 6;
      score = int_clamp(score,2,0xf);
      break;
    case IMP_RESEARCH_LAB:
      if (((s->space_race == 0) || (ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT), ret == 0)) &&
         (civs[s->civ].science_rate != '\0')) {
        score = int_clamp(0xb - (s->net_trade >> 2),2,10);
      }
      break;
    case IMP_SAM_BATTERY:
      score = city_choose_production_sam_score(s,city,score,defensive);
      break;
    case IMP_COASTAL_FORTRESS:
      score = city_choose_production_coastal_fortress_score(s,city,score,defensive);
      break;
    case IMP_SOLAR_PLANT:
      break;
    case IMP_HARBOR:
      if ((s->unrest_state < 2) &&
         (score = int_clamp(0x10 - s->ocean_tiles,2,0x10), s->food_surplus < 1)) {
        score = (int)score / 2;
      }
      break;
    case IMP_OFFSHORE_PLATFORM:
      if (s->unrest_state < 2) {
        score = 0x10 - s->ocean_tiles;
        score = int_clamp(score,2,0x10);
        if (0 < (int)score) {
          ret = city_has_improvement(city,IMP_FACTORY);
          if (ret != 0) {
            score = (int)score / 2;
          }
          ret = city_has_improvement(city,IMP_MFG_PLANT);
          if (ret != 0) {
            score = (int)score / 2;
          }
        }
        if (((city_calc.total_shields - city_calc.shield_support <= city_calc.total_shields / 3) ||
            (city_calc.total_shields < (int)cities[s->city2].size / 2)) &&
           ((int)cities[s->city2].size / 2 <= (int)(s->ocean_tiles + (s->ocean_tiles >> 0x1f & 3U)) >> 2))
        {
          score -= 2;
        }
      }
      break;
    case IMP_AIRPORT:
      score = (s->airport_count * 4 + 4) - (int)leaders[civs[s->civ].tribe].attack;
      if (*(char *)&civs[s->civ].continent_policy[s->landmass2] == 4) {
        score = (int)score / 2;
      }
      ret = civ_has_advance(s->civ,ADV_ADVANCED_FLIGHT);
      if (ret == 0) {
        score = score << 1;
      }
      if (((*(char *)&civs[s->civ].continent_cities[s->landmass2] == 1) ||
          (((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0 && (s->airport_count == 0))))
         && ((int)(uint)civs[s->civ].continent_cities[s->landmass2] < (int)score)) {
        score = (uint)civs[s->civ].continent_cities[s->landmass2];
      }
      ret = city_has_improvement(city,IMP_PALACE);
      if ((ret != 0) && (-1 < (int)score)) {
        score = 0;
      }
      *defensive = s->threatened;
      break;
    case IMP_POLICE_STATION:
      ret = civ_has_wonder(s->civ,WONDER_WOMENS_SUFFRAGE);
      if (ret == 0) {
        tmp = (*(uint *)&city_calc.aggressive_units);
        if (GOV_REPUBLIC < civs[s->civ].government) {
          tmp = (*(uint *)&city_calc.aggressive_units) << 1;
        }
        if (((GOV_FUNDAMENTALISM < civs[s->civ].government) && (tmp != 0)) &&
           (city_calc.happy_count - city_calc.unhappy_count < 2)) {
          tmp += 2;
        }
        score = 10 - tmp;
      }
      break;
    case IMP_PORT_FACILITY:
      ret = city_adjacent_to_body(city,1);
      if (ret != 0) {
        ret = s->nearest_dist;
        if (0x27 < s->nearest_dist) {
          ret = 0x28;
        }
        score = (0xb - (ret >> 2)) + s->naval_units * -5 +
                   (s->port_count * 2 - (int)leaders[civs[s->civ].tribe].attack) +
                   (int)leaders[civs[s->civ].tribe].civilize;
        score = int_clamp(score,2,0xf);
        if (leaders[civs[s->civ].tribe].attack < '\0') {
          score += s->port_count;
        }
      }
      break;
    case IMP_SS_STRUCTURAL:
      score = city_choose_production_spaceship_score(s,city,score,defensive);
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): an improvement: its score and
   whether it is the best so far. */
static void city_choose_production_try_improvement(ChooseProdState *s,iptr city,iptr improvement)
{
  int ret;
  uint defensive;
  uint score;

  ret = city_can_build_improvement(s->civ,city,improvement);
  if (ret != 0) {
    score = city_choose_production_improvement_score(s,city,improvement,&defensive);
    if ((int)score < 400) {
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
    }
    else {
      score = 0x3fff;
    }
    ret = city_has_improvement(city,IMP_BARRACKS);
    if (ret != 0) {
      score += (int)score / ((s->barracks_count >> 1) + 3);
    }
    ret = wonder_get_city(WONDER_SHAKESPEARE);
    if (ret == city) {
      score += (int)score / 3;
    }
    if ((int)score < s->choice_score) {
      s->choice_score = score;
      s->choice = -improvement;
    }
    if ((int)score < s->best_build_score) {
      s->best_build_score = score;
      s->best_build = -improvement;
    }
  }
}

/* Part of city_choose_production (00498e8b): the bias of each wonder for the
   civ's situation and its leader; 0 to leave the wonder out. */
static int city_choose_production_wonder_bias(ChooseProdState *s,iptr city,iptr wonder,uint score,uint *defensive,uint *out)
{
  int ret;
  uint adjusted;
  iptr civ3;
  iptr unit_type;
  uint tmp;
  uptr civ4;
  int ship_count;

  adjusted = score;
  switch(wonder) {
  case WONDER_PYRAMIDS:
    adjusted = score - (leaders[civs[s->civ].tribe].civilize * 3 + 2);
    break;
  case WONDER_HANGING_GARDENS:
    adjusted = score - ((int)civs[s->civ].num_cities / 3 + 1);
    break;
  case WONDER_COLOSSUS:
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret != 0) {
      score--;
    }
    ret = city_has_improvement(city,IMP_MARKETPLACE);
    if (ret != 0) {
      score--;
    }
    adjusted = score - (int)cities[city].size / 3;
    break;
  case WONDER_LIGHTHOUSE:
    adjusted = score -
            ((int)(civs[s->civ].units_active[UNIT_CARAVEL] + 2 + (uint)civs[s->civ].units_active[UNIT_TRIREME]
                  ) >> 2);
    break;
  case WONDER_GREAT_LIBRARY:
    for (civ4 = 1; adjusted = score, (int)civ4 < 8; civ4++) {
      if (civs[civ4].num_advances < civs[s->civ].num_advances) {
        if ((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) {
          adjusted = score + 1;
        }
      }
      else {
        adjusted = score - 1;
        if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
           (civs[s->civ].num_advances + 5 < (uint)civs[civ4].num_advances)) {
          adjusted = score - 3;
        }
      }
      score = adjusted;
    }
    break;
  case WONDER_ORACLE:
    adjusted = score -
            (((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U)
                   ) >> 2) - (int)game.turn / 0x19);
    break;
  case WONDER_GREAT_WALL:
    adjusted = score + leaders[civs[s->civ].tribe].civilize * -2;
    break;
  case WONDER_SUN_TZU:
    adjusted = score -
            (leaders[civs[s->civ].tribe].attack * 3 + (int)leaders[civs[s->civ].tribe].expand
             + leaders[civs[s->civ].tribe].civilize * -2 + 1);
    break;
  case WONDER_KING_RICHARDS:
    ret = int_clamp((city_calc.total_shields * 2) / 3,0,3);
    adjusted = score - ret;
    break;
  case WONDER_MARCO_POLO:
    for (civ3 = 1; adjusted = score, civ3 < 8; civ3++) {
      if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
        if ((*(byte *)&civs[s->civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
          score--;
        }
        if ((*(byte *)&civs[s->civ].treaties[civ3] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0) {
          score--;
        }
      }
    }
    break;
  case WONDER_MICHELANGELO:
    adjusted = score -
            ((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U))
            >> 2);
    break;
  case WONDER_COPERNICUS:
    score = (score - ((int)(s->net_trade + (s->net_trade >> 0x1f & 7U)) >> 3)) - 1;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_MAGELLAN:
    ship_count = 0;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if (unit_types[unit_type].domain == DOMAIN_SEA) {
        ship_count += (uint)civs[s->civ].units_active[unit_type];
      }
    }
    adjusted = score - ((int)(ship_count + 3 + (ship_count + 3 >> 0x1f & 3U)) >> 2);
    break;
  case WONDER_SHAKESPEARE:
    adjusted = score - (int)cities[city].size / 5;
    break;
  case WONDER_LEONARDO:
    *defensive = s->threatened;
    adjusted = score - 4;
    break;
  case WONDER_BACH:
    score -= ((int)(uint)civs[s->civ].continent_city_size[s->landmass2] >> 4);
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_NEWTON:
    score -= (int)cities[city].size / 6;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_ADAM_SMITH:
    adjusted = score -
            ((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U))
            >> 2);
    break;
  case WONDER_DARWIN:
    adjusted = score - 3;
    break;
  case WONDER_STATUE_OF_LIBERTY:
    ret = civ_has_advance(s->civ,ADV_COMMUNISM);
    if (ret == 0) {
      adjusted = score - 2;
    }
    else {
      adjusted = score - 1;
    }
    break;
  case WONDER_EIFFEL_TOWER:
    if (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) {
      adjusted = score - 1;
      if (3 < civs[game.civ_by_rank[7]].reputation) {
        adjusted = score - 2;
      }
      score = adjusted;
      adjusted = score;
      if ((7 < civs[game.civ_by_rank[7]].num_cities) &&
         (adjusted = score - 1, game.power_rank[s->civ] == 6)) {
        adjusted = score - 2;
      }
    }
    break;
  case WONDER_WOMENS_SUFFRAGE:
    adjusted = score - 1;
    if (((-1 < leaders[civs[s->civ].tribe].expand) &&
        (-1 < leaders[civs[s->civ].tribe].civilize)) &&
       (-1 < leaders[civs[s->civ].tribe].attack)) {
      adjusted = score - 2;
    }
    score = adjusted;
    if (0 < civs[s->civ].government_score[GOV_REPUBLIC]) {
      score--;
    }
    if (GOV_FUNDAMENTALISM < civs[s->civ].government) {
      score--;
    }
    adjusted = score;
    if (GOV_REPUBLIC < civs[s->civ].government) {
      adjusted = score - 1;
    }
    break;
  case WONDER_HOOVER_DAM:
    adjusted = score - civs[s->civ].continent_city_size[s->landmass2] / 0x14;
    break;
  case WONDER_MANHATTAN:
    ret = civ_has_advance(s->civ,ADV_ROCKETRY);
    if (ret != 0) {
      tmp = 0;
      for (civ3 = 1; civ3 < 8; civ3++) {
        if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
            (ret = civ_has_advance(civ3,ADV_ROCKETRY), ret != 0)) &&
           (civs[civ3].military_strength < civs[s->civ].military_strength)) {
          tmp = 1;
          break;
        }
      }
      if (tmp == 0) {
        if (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) {
          score--;
          ret = civ_has_advance((uint)game.civ_by_rank[7],ADV_ROCKETRY);
          if (ret == 0) {
            score--;
          }
        }
        adjusted = score +
                (leaders[civs[s->civ].tribe].civilize * 2 -
                (int)leaders[civs[s->civ].tribe].attack);
        break;
      }
    }
    return 0;
  case WONDER_UNITED_NATIONS:
    adjusted = score -
            ((int)leaders[civs[s->civ].tribe].civilize +
            leaders[civs[s->civ].tribe].attack * -2);
    if (game.power_rank[s->civ] == 7) {
      adjusted--;
    }
    break;
  case WONDER_APOLLO:
    adjusted = score -
            (leaders[civs[s->civ].tribe].civilize * 3 +
            leaders[civs[s->civ].tribe].attack * -2);
    break;
  case WONDER_SETI:
    adjusted = score - (leaders[civs[s->civ].tribe].civilize * 2 + 2);
    break;
  case WONDER_CURE_FOR_CANCER:
    adjusted = score - (leaders[civs[s->civ].tribe].attack + 2);
  }
  *out = adjusted;
  return 1;
}

/* Part of city_choose_production (00498e8b): the civs (a bit each) building
   the wonder the city builds, from wonder_builders.  B21: for a unit or an
   improvement the original reads that byte all the same, from its other
   locals below the array; none of them is a wonder's builders (every mode:
   ours would read past the array; docs/BUGS.md). */
static uint city_choose_production_rivals(ChooseProdState *s,iptr city)
{
  if (cities[city].building > -0x27) {
    return 0;
  }
  return (uint)*(byte *)&s->wonder_builders[-0x27 - cities[city].building];
}

/* Part of city_choose_production (00498e8b): a wonder: its score and whether
   it is the best so far. */
static void city_choose_production_try_wonder(ChooseProdState *s,iptr city,iptr wonder)
{
  int ret;
  uint adjusted;
  iptr improvement;
  int wonder_cost;
  iptr preq2;
  int expired;
  uint defensive;
  iptr wonder_group;
  uint score;
  int raw_score;

  improvement = wonder + 0x27;
  ret = city_can_build_improvement(s->civ,city,improvement);
  if (ret != 0) {
    defensive = 0;
    if (s->policy == 4) {
      defensive = s->threatened;
    }
    ret = (s->wonders_owned + s->wonders_here + 8) - civs[s->civ].gold / 200;
    if (ret < 2) {
      ret = 1;
    }
    wonder_group = wonder / 7;
    score = ret + (uint)civs[s->civ].wonders_by_group[wonder_group];
    if ((byte)(((i8 *)&map_views_block.human_wonders_by_group_min))[wonder_group] < civs[s->civ].wonders_by_group[wonder_group]) {
      score += 2;
    }
    if (civs[s->civ].wonders_by_group[wonder_group] <
        civs[((uint)game.worst_human_rank)].wonders_by_group[wonder_group]) {
      score--;
    }
    if ((((s->wonder_in_progress == 0) || ((int)cities[city].building == -improvement)) ||
        ((((game.human_players & *(byte *)&s->wonder_builders[-0x27 - cities[city].building]) ==
           0 || ((*(short *)((int)&game + cities[city].building * -2 + 0xb0) != -1 ||
                 ((int)cities[city].shield_box <
                  (int)((uint)improvements[-(int)cities[city].building].cost * city_calc.shield_rows)
                  / 2)))) &&
         ((*(byte *)&s->wonder_builders[-0x27 - cities[city].building] & 1) == 0)))) &&
       (((DIFF_WARLORD < game.difficulty || (preq2 = (iptr)improvements[improvement].preq, preq2 < 0))
        || ((game.human_players & game.advance_known_by[preq2]) != 0)))) {
      if (wonder != WONDER_MANHATTAN) {
        if (game.difficulty < DIFF_PRINCE) {
          for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
            if (((cities[s->city2].id != 0) &&
                (((uint)game.human_players & 1 << (cities[s->city2].owner & 0x1fU)) != 0)) &&
               ((int)cities[s->city2].building == -improvement)) {
              score = 999;
            }
          }
          if (score == 999) return;
        }
        ret = civ_has_advance(s->civ,ADV_ROCKETRY);
        if ((ret != 0) && ((game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0))
        return;
      }
      if (city_choose_production_wonder_bias(s,city,wonder,score,&defensive,&adjusted) == 0) return;
      score = adjusted;
      expired = wonder_is_expired(wonder);
      if (expired != 0) {
        score = score * 3 + (uint)game.difficulty * 0x32;
      }
      score += s->wonders_building * 5;
      if (((s->wonders_building == 0) || (s->wonder_in_progress != 0)) &&
         (*(char *)&civs[s->civ].wonders_by_group[wonder_group] == '\0')) {
        score -= 2;
      }
      if (s->policy == 1) {
        score = score + s->enemy_strength + 1;
      }
      else {
        score += s->enemy_strength / 2;
      }
      if ((int)cities[city].building == -improvement) {
        if (s->foreign_same_build != 0) {
          score = (int)score / 2;
        }
        score = (s->rivals_ahead + 1) * score + s->rivals_ahead;
      }
      else if ((1 << (s->civ5 & 0x1f) &
               (uint)city_choose_production_rivals(s,city)) != 0) {
        score = score * 2 + 1;
      }
      raw_score = score;
      wonder_cost = (uint)improvements[improvement].cost * city_calc.shield_rows + 1;
      if (((s->wonder_in_progress != 0) && (s->defense_urgent == 0)) && (expired == 0)) {
        if (s->wonders_building == 0) {
          if (s->threatened == defensive) {
            score = (int)score / 2;
          }
          else {
            defensive = s->threatened;
          }
        }
        if ((int)(wonder_cost * 3 + (wonder_cost * 3 >> 0x1f & 3U)) >> 2 <=
            (int)cities[city].shield_box) {
          defensive = s->threatened;
        }
        if (((s->threatened != defensive) && (wonder_cost / 2 <= (int)cities[city].shield_box)) &&
           (s->wonders_building < 3)) {
          defensive = s->threatened;
        }
      }
      if (s->wonder_in_progress == 0) {
        if ((int)civs[s->civ].best_shield_surplus >> 1 < s->shield_surplus) {
          if ((((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) &&
              (civs[s->civ].best_shield_surplus != s->shield_surplus)) && (s->shield_surplus < 6)) {
            score = score << 1;
          }
        }
        else {
          score = score << 1;
        }
      }
      if (civs[s->civ].best_shield_surplus == s->shield_surplus) {
        score -= (int)score / 3;
      }
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
      ret = city_has_improvement(city,IMP_BARRACKS);
      if (ret != 0) {
        score += (int)score / ((s->barracks_count >> 1) + 3);
      }
      ret = city_has_improvement(city,IMP_PALACE);
      if ((ret != 0) &&
         ((score = score - (int)score / 3, s->wonders_here == 0 ||
          (*(char *)&civs[s->civ].wonders_by_group[wonder_group] == '\0')))) {
        score = (int)score / 2;
      }
      if ((cities[city].flags & CITYF_WONDER_BUILT) == 0) {
        if ((s->wonder_in_progress != 0) && (expired == 0)) {
          score = (((wonder_cost - cities[city].shield_box) *
                      ((int)(score * 3 + ((int)(score * 3) >> 0x1f & 3U)) >> 2)) /
                      wonder_cost + ((int)(score + ((int)score >> 0x1f & 3U)) >> 2)) / 3;
        }
      }
      else {
        score = score * 3;
      }
      if (s->choice < -0x26) {
        if (raw_score < s->best_raw_score) {
          s->best_raw_score = raw_score;
          s->choice = -improvement;
          s->best_build = -improvement;
          if ((int)score < s->choice_score) {
            s->choice_score = score;
          }
          if ((int)score < s->best_build_score) {
            s->best_build_score = score;
          }
        }
      }
      else {
        if ((int)score < s->choice_score) {
          s->best_raw_score = raw_score;
          s->choice_score = score;
          s->choice = -improvement;
        }
        if ((int)score < s->best_build_score) {
          s->best_build_score = score;
          s->best_build = -improvement;
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the score of a diplomat or spy
   (to steal the spaceship advances when behind in the race); 0 to leave it
   out. */
static int city_choose_production_diplomat_score(ChooseProdState *s,iptr city,iptr unit_type,uint *score,uint *defensive)
{
  int ret;
  uint adjusted;
  iptr improvement;
  int race_behind;
  uptr civ2;
  uptr civ4;

  race_behind = ai_spaceship_race_behind(s->civ);
  if (race_behind != 0) {
    ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT);
    if ((((ret != 0) &&
         (ret = civ_has_advance(s->civ,(int)improvements[IMP_SS_COMPONENT].preq), ret != 0)) &&
        (ret = civ_has_advance(s->civ,(int)improvements[IMP_SS_MODULE].preq), ret != 0)) &&
       (ret = civ_has_advance(s->civ,ADV_FUSION_POWER), ret != 0)) {
      race_behind = 0;
    }
    if ((s->defense_urgent != 0) || ((cities[city].flags & CITYF_DISORDER) != 0)) {
      race_behind = 0;
    }
    if (game.difficulty < DIFF_PRINCE) {
      race_behind = 0;
    }
  }
  if (!(((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
       ((cities[city].flags & CITYF_AUTOBUILD) == 0))) return 0;
  if ((s->units_in_build[unit_type] <= race_behind) &&
     (s->unit_counts[unit_type] < (int)(uint)civs[s->civ].continent_cities[s->landmass2]))
  {
    *score = 0xb;
    civ4 = 0xffffffff;
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (((civ2 != s->civ) &&
          (((*(byte *)&civs[s->civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0 ||
           (((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0 &&
              ((game.power_rank[s->civ] <= game.power_rank[civ2] || (race_behind != 0))))
             && ((unit_type == UNIT_SPY ||
                 (((7 < (int)((uint)civs[civ2].num_advances -
                             (uint)civs[s->civ].num_advances) || (race_behind != 0)) ||
                  (game.power_rank[civ2] == 7)))))) &&
            ((((*(byte *)&civs[s->civ].treaties[civ2] & TREATY_ALLIANCE) != 0 ||
              (race_behind != 0)) ||
             ((civs[civ2].government < GOV_REPUBLIC ||
              ((civs[civ2].continent_attack[s->landmass2] <=
                civs[s->civ].continent_military[s->landmass2] ||
               (*(char *)&civs[s->civ].continent_cities[s->landmass2] == '\0')))))))))))) &&
         (*(char *)&civs[civ2].continent_cities[s->landmass2] != '\0')) {
        civ4 = civ2;
      }
    }
    if ((int)civ4 < 0) {
      civ4 = (uint)game.best_human_rank;
      if ((s->policy == 4) && (game.difficulty != DIFF_CHIEFTAIN)) {
        *defensive = s->threatened;
        *score = 8;
        ret = civs[s->civ].num_cities + 3;
        if ((int)((uint)civs[s->civ].units_active[unit_type] +
                 (uint)civs[s->civ].units_in_production[unit_type]) <
            (int)(ret + (ret >> 0x1f & 3U)) >> 2) {
          if (civs[game.best_human_rank].gold < civs[s->civ].gold) {
            *score = 7;
          }
          if (civs[game.best_human_rank].gold * 2 < civs[s->civ].gold) {
            (*score)--;
          }
          if (civs[s->civ].num_advances < civs[game.best_human_rank].num_advances) {
            if (s->unit_counts[unit_type] + s->units_in_build[unit_type] == 0) {
              *score -= 3;
            }
            *score = *score -
                       ((uint)civs[game.best_human_rank].num_advances -
                       (uint)civs[s->civ].num_advances);
          }
          *score = int_clamp(*score,civs[s->civ].units_active[unit_type] + 1,10);
        }
      }
      if (race_behind != 0) {
        *score = (int)*score / 2;
      }
      if (s->unit_counts[unit_type] != 0) {
        *score = 999;
      }
    }
    else {
      *score = int_clamp(10 - ((uint)civs[civ4].num_advances -
                                         (uint)civs[s->civ].num_advances),2,10);
      if ((((*(byte *)&civs[s->civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0) &&
          (*score = *score + 1, s->unit_counts[unit_type] != 0)) && (race_behind == 0)) {
        *score = 999;
      }
      if (civs[civ4].military_strength <= civs[s->civ].military_strength) {
        *defensive = s->threatened;
      }
    }
    if (civs[s->civ].government == GOV_COMMUNISM) {
      (*score)--;
    }
    if ((race_behind != 0) && ((int)*score < 0x14)) {
      for (improvement = 0x23; improvement < 0x26; improvement++) {
        ret = civ_has_advance(s->civ,(int)improvements[improvement].preq);
        if ((ret == 0) &&
           (ret = civ_has_advance(civ4,(int)improvements[improvement].preq),
           ret != 0)) {
          *defensive = s->threatened;
          adjusted = s->units_in_build[unit_type] * 2 + 1;
          if ((int)*score <= (int)adjusted) {
            adjusted = *score;
          }
          *score = adjusted;
          if ((((s->units_in_build[unit_type] == 0) &&
               (*(char *)&civs[s->civ].units_active[unit_type] == '\0')) &&
              (*score = adjusted - 1, -0x26 < s->choice)) && (s->choice < -0x22)) {
            s->choice = unit_type;
          }
        }
      }
    }
  }
  return 1;
}

/* Part of city_choose_production (00498e8b): a diplomat or a caravan: its
   score and whether it is the best so far. */
static void city_choose_production_try_special_unit(ChooseProdState *s,iptr city,iptr unit_type)
{
  int ret;
  uint defensive;
  uint score;

  if ((5 < (char)unit_types[unit_type].role) &&
     (ret = civ_can_build_unit(s->civ,city,unit_type), ret != 0)) {
    score = 999;
    defensive = 0;
    if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
      if (city_choose_production_diplomat_score(s,city,unit_type,&score,&defensive) == 0) return;
    }
    else if (unit_types[unit_type].role == ROLE_TRADE) {
      if ((cities[city].flags & CITYF_HAD_CARAVAN) != 0) return;
      score = 10;
      if ((char)cities[city].trade_routes < 3) {
        score = 10 - ((5 - leaders[civs[s->civ].tribe].expand) * city_calc.total_trade) / 10;
      }
      ret = score + (char)cities[city].trade_routes * 2;
      if (ret < 3) {
        ret = 2;
      }
      score = ret + (uint)civs[s->civ].continent_trade_units[s->landmass2];
      if (!(((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
           ((cities[city].flags & CITYF_AUTOBUILD) == 0))) return;
    }
    if ((int)score < 400) {
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
    }
    else {
      score = 0x3fff;
    }
    ret = city_has_improvement(city,IMP_BARRACKS);
    if (ret != 0) {
      score += ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
    }
    if ((int)score < s->choice_score) {
      s->choice_score = score;
      s->choice = unit_type;
    }
    if ((int)score < s->best_build_score) {
      s->best_build_score = score;
      s->best_build = unit_type;
    }
  }
}

/* Part of city_choose_production (00498e8b): whether a military or settler
   unit is one to consider (not obsolete, not too many). */
static int city_choose_production_unit_allowed(ChooseProdState *s,iptr city,iptr unit_type,uint role2)
{
  int ret;

  if (((((((int)role2 < 6) &&
         (ret = civ_can_build_unit(s->civ,city,unit_type), ret != 0)) &&
        (unit_type != UNIT_CARRIER)) &&
       ((role2 != 5 ||
        (((city_calc.settler_food_cost <= s->food_surplus ||
          (((city_calc.settlers_supported == 0 &&
            ((uint)civs[s->civ].units_active[UNIT_SETTLERS] + (uint)civs[s->civ].units_active[UNIT_ENGINEERS] ==
             s->flagged_settlers)) && (s->units_in_build[1] + s->units_in_build[0] < 2)))) &&
         (1 < cities[city].size)))))) &&
      (((unit_type != UNIT_WARRIORS || (civs[s->civ].government < GOV_MONARCHY)) &&
       (((char)unit_types[unit_type].attack < 'c' ||
        ((civs[s->civ].units_active[unit_type] < 4 &&
         (civs[s->civ].units_in_production[unit_type] < 2)))))))) &&
     ((role2 != 3 ||
      (((uint)civs[s->civ].units_active[UNIT_FIGHTER] + (uint)civs[s->civ].units_in_production[UNIT_FIGHTER] +
        (uint)civs[s->civ].units_active[UNIT_STEALTH_FIGHTER] + (uint)civs[s->civ].units_in_production[UNIT_STEALTH_FIGHTER] <
        (uint)civs[game.best_human_rank].units_active[UNIT_BOMBER] +
        (uint)civs[game.best_human_rank].units_active[UNIT_HELICOPTER] +
        (uint)civs[game.best_human_rank].units_active[UNIT_STEALTH_BOMBER] + 2 &&
       (((*(byte *)&civs[s->civ].treaties[((uint)game.best_human_rank)] & TREATY_PEACE) == 0 ||
        ((uint)civs[s->civ].units_active[unit_type] +
         (uint)civs[s->civ].units_in_production[unit_type] == 0)))))))) {
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      if (unit_type == UNIT_WARRIORS) {
        ret = civ_has_advance(s->civ,(int)unit_types[UNIT_PHALANX].prerequisite);
        if (ret != 0) return 0;
      }
      else if (unit_type == UNIT_ARCHERS) {
        ret = civ_has_advance(s->civ,(int)unit_types[UNIT_LEGION].prerequisite);
        if (ret != 0) return 0;
      }
      else if (unit_type == UNIT_HORSEMEN) {
        ret = civ_has_advance(s->civ,(int)unit_types[UNIT_CHARIOT].prerequisite);
        if (ret != 0) return 0;
        ret = civ_has_advance(s->civ,(int)unit_types[UNIT_ELEPHANT].prerequisite);
        if (ret != 0) return 0;
      }
    }
    if (unit_type == UNIT_CHARIOT) {
      ret = civ_has_advance(s->civ,(int)unit_types[UNIT_KNIGHTS].prerequisite);
      if (ret != 0) return 0;
      ret = civ_has_advance(s->civ,(int)unit_types[UNIT_CRUSADERS].prerequisite);
      if (ret != 0) return 0;
    }
    else if (unit_type == UNIT_ELEPHANT) {
      ret = civ_has_advance(s->civ,(int)unit_types[UNIT_KNIGHTS].prerequisite);
      if (ret != 0) return 0;
    }
    return 1;
  }
  return 0;
}

/* Part of city_choose_production (00498e8b): the base score of a unit from the
   units of its kind there are and being built; 0 to leave it out. */
static int city_choose_production_unit_base_score(ChooseProdState *s,iptr unit_type,uint *out)
{
  int ret;
  int explorer_cap;
  uint tmp;
  uint score;

  if (unit_types[unit_type].domain == DOMAIN_LAND) {
    score = s->units_in_build[unit_type] * 2 + s->unit_counts[unit_type];
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      score = (int)score / 2;
      if (((unit_type == UNIT_MARINES) || (unit_type == UNIT_ALPINE_TROOPS)) || (unit_type == UNIT_PARTISANS)) {
        score += 2;
      }
    }
    if (unit_type == UNIT_ARCHERS) {
      score = score + s->units_in_build[5] * 2 + s->unit_counts[5];
    }
    if (leaders[civs[s->civ].tribe].attack < '\0') {
      if (unit_type == UNIT_KNIGHTS) {
        score = score + s->units_in_build[0x12] * 2 + s->unit_counts[0x12];
      }
    }
    else if (unit_type == UNIT_CRUSADERS) {
      score = score + s->units_in_build[0x13] * 2 + s->unit_counts[0x13];
    }
    if ((unit_types[unit_type].role == ROLE_ATTACK) && (unit_types[unit_type].attack == '\0')) {
      if ((s->policy == 0) || (s->policy == 1)) return 0;
      if (s->policy == 4) {
        explorer_cap = 1;
      }
      else {
        explorer_cap = (landmass_table_block.landmass_recs[s->landmass2].tiles + 199) / 200;
      }
      if (!((s->units_in_build[unit_type] * 2 + s->unit_counts[unit_type] <= explorer_cap) &&
           (civs[s->civ].units_active[unit_type] < 2))) return 0;
    }
    if (s->policy == 4) {
      tmp = 1 - (int)leaders[civs[s->civ].tribe].expand;
      if (unit_types[unit_type].role != ROLE_SETTLE) {
        tmp = tmp * 2;
      }
      if (s->enemy_offshore != 0) {
        tmp = (int)tmp / 2;
      }
      score += tmp;
      if ((s->enemy_offshore != 0) && (unit_types[unit_type].role == ROLE_ATTACK)) {
        tmp = (int)leaders[civs[s->civ].tribe].attack + 1;
        ret = civ_has_advance(s->civ,(int)unit_types[UNIT_TRANSPORT].prerequisite);
        if (ret != 0) {
          tmp = tmp << 1;
        }
        score = int_clamp(score - tmp,0,99);
      }
    }
  }
  else {
    if (!((ushort)(((civs[s->civ].continent_cities[s->landmass2] < 2) - 1 & 4) + 4) <=
           civs[s->civ].continent_military[s->landmass2])) return 0;
    score = (int)(((uint)civs[s->civ].units_active[unit_type] +
                     (uint)civs[s->civ].units_in_production[unit_type] * 2) *
                    (uint)civs[s->civ].continent_military[s->landmass2]) /
               (s->unit_total + 1);
    if (((unit_types[unit_type].flags & UTYPE_SPOT_SUBMARINES) != 0) &&
       (score = score - civs[game.best_human_rank].units_active[UNIT_SUBMARINE], (int)score < 1))
    {
      score = 0;
    }
  }
  *out = score;
  return 1;
}

/* Part of city_choose_production (00498e8b): the score of settlers. */
static uint city_choose_production_settler_score(ChooseProdState *s,iptr city,iptr unit_type,uint score)
{
  char leader_trait;
  iptr saved_type;
  int ret;
  int settler_base;
  int support_penalty;
  iptr unit_type2;
  int threat_level;

  ret = city_has_improvement(city,IMP_GRANARY);
  if ((ret == 0) && (ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS), ret == 0)) {
    settler_base = 6;
  }
  else {
    settler_base = 4;
  }
  saved_type = unit_type;
  if (s->unit_counts[unit_type] < ((civs[s->civ].government + 1) / 2)) {
    leader_trait = leaders[civs[s->civ].tribe].civilize;
  }
  else {
    leader_trait = leaders[civs[s->civ].tribe].expand;
  }
  score = ((settler_base - leader_trait) - s->expand_bonus) * score;
  if (city_calc.settlers_supported != 0) {
    support_penalty = 8;
    if (cities[city].size < 4) {
      support_penalty = 0x10;
    }
    for (unit_type2 = 0; unit_type2 < city_calc.settlers_supported; unit_type2++) {
      score += (unit_type2 + 1) * support_penalty;
    }
  }
  if (cities[city].size < 2) {
    score = score << 1;
  }
  if ((s->unit_counts[unit_type] == 0) && (city_calc.settler_food_cost == s->food_surplus)) {
    score -= (int)score / 2;
  }
  if (s->defense_need < 2) {
    if (s->units_in_build[unit_type] == 0) {
      if (civs[s->civ].government < GOV_REPUBLIC) {
        if (GOV_DESPOTISM < civs[s->civ].government) {
          ret = int_clamp((int)(uint)civs[s->civ].continent_cities[s->landmass2] >> 1,1,3);
          if (s->unit_counts[unit_type] < ret) {
            score = (int)score / 2;
          }
          if (s->unit_counts[unit_type] <
              (civs[s->civ].continent_cities[s->landmass2] / 5)) {
            score = (int)score / 2;
          }
        }
      }
      else {
        ret = int_clamp(civs[s->civ].continent_cities[s->landmass2]
                                   - 1,1,6);
        if (s->unit_counts[saved_type] < ret) {
          score = (int)score / 2;
        }
        if (s->unit_counts[unit_type] <
            (int)(uint)civs[s->civ].continent_cities[s->landmass2] >> 2) {
          score = (int)score / 2;
        }
      }
    }
    if (((s->unit_counts[unit_type] == 0) && (s->units_in_build[unit_type] == 0)) &&
       (score = (int)score / 2, s->policy != 4)) {
      threat_level = leaders[civs[s->civ].tribe].expand + 3;
      if (s->policy == 1) {
        threat_level = leaders[civs[s->civ].tribe].expand + 2;
      }
      if (s->policy == 5) {
        threat_level++;
      }
      if ((int)(uint)civs[s->civ].continent_cities[s->landmass2] < threat_level) {
        score = 0;
      }
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): a military or settler unit: its
   score and whether it is the best so far. */
static void city_choose_production_try_military_unit(ChooseProdState *s,iptr city,iptr unit_type)
{
  int ret;
  uint adjusted;
  int count;
  int combat_value;
  int support_penalty;
  uint role2;
  int hp_factor;
  uint score;

  role2 = (uint)(char)unit_types[unit_type].role;
  if (city_choose_production_unit_allowed(s,city,unit_type,role2) != 0) {
    if (city_choose_production_unit_base_score(s,unit_type,&score) == 0) return;
    if ((unit_types[unit_type].flags & UTYPE_PARADROP) != 0) {
      role2 = 0;
    }
    if ((s->defense_need != 0) &&
       (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0 || (citypref.no_defend == 0))))
    {
      if (role2 == ROLE_DEFEND) {
        score = (uint)(2 / (longlong)s->defense_need);
        if ((s->defense_need == 2) && (s->unrest_state < 2)) {
          score = -1;
        }
      }
      else if (s->defense_need == 2) return;
    }
    adjusted = score;
    if (((role2 != ROLE_DEFEND) && (role2 != ROLE_SETTLE)) &&
       (adjusted = score << 1, s->policy != role2)) {
      adjusted = score << 2;
    }
    score = adjusted;
    if (((unit_type != UNIT_PARTISANS) ||
        ((s->defense_need == 0 &&
         ((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0)))) &&
       ((unit_types[unit_type].domain != DOMAIN_SEA ||
        ((((ret = city_adjacent_sea_info(city,0,0), ret != 0x3f &&
           ((cities[city].flags & CITYF_COASTAL) != 0)) && (ret != 0)) &&
         ((count = landmass_count_bordering(ret), 1 < count ||
          (((role2 != ROLE_SEA_TRANSPORT && (0x2f < landmass_table_block.landmass_recs[ret].water_body_size)) &&
           (ret < 0x3f)))))))))) {
      if (role2 == ROLE_SEA_TRANSPORT) {
        if (s->policy == 4) {
          score = (int)(score * 3) / 2;
        }
        else {
          if ((s->policy == 1) ||
             (*(char *)&civs[s->civ].units_active[unit_type] != '\0'))
          return;
          if (unit_type != UNIT_TRANSPORT) {
            score = score * 2 << ((byte)s->threatened & 0x1f);
          }
        }
      }
      if (role2 == ROLE_SETTLE) {
        score = city_choose_production_settler_score(s,city,unit_type,score);
      }
      else {
        score = score << 4;
      }
      score = ((char)unit_types[unit_type].cost + 2) * score;
      if ((unit_types[unit_type].attack == '\0') &&
         (unit_types[unit_type].defense == '\0')) {
        score = 0;
      }
      else {
        if ((game.rules & RULE_NORMAL_COMBAT) == 0) {
          hp_factor = 2;
        }
        else {
          hp_factor = int_clamp((int)(char)unit_types[unit_type].hit_points / 10
                                        + (int)(char)unit_types[unit_type].firepower,2,4)
          ;
        }
        combat_value = ((int)(char)unit_types[unit_type].attack +
                    (int)(char)unit_types[unit_type].defense) * hp_factor;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          combat_value++;
        }
        if (unit_type == UNIT_PARTISANS) {
          combat_value++;
        }
        score = (int)score /
                   ((((int)(char)unit_types[unit_type].move /
                      (int)(uint)cosmic.road_move_mult + 1) * combat_value) / 2);
      }
      if (role2 == ROLE_NAVAL_SUPERIORITY) {
        if ((game.game_flags & GAME_LARGE_MAP) != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
        if ((s->policy == 4) && (0 < (int)score)) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
        if (s->threatened == 0) {
          score = score << 1;
        }
        else {
          score = score << 2;
        }
        ret = city_has_improvement(city,IMP_PORT_FACILITY);
        if (ret != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
      }
      if (unit_types[unit_type].domain == DOMAIN_AIR) {
        score = score << 1;
        ret = city_has_improvement(city,IMP_AIRPORT);
        if (ret != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
      }
      if (role2 == ROLE_DEFEND) {
        score -= (s->settle_goals + 1 >> 1);
      }
      if (0x77 < s->unit_total) {
        score = score << 2;
      }
      if ((s->defense_need == 0) && ((int)role2 < 6)) {
        support_penalty = 0;
        if ((city_calc.shield_support != 0) && (city_calc.total_shields != 0)) {
          support_penalty = (int)((3 - s->threatened) * (city_calc.shield_support + 1) * score) /
                      city_calc.total_shields;
        }
        if (role2 == ROLE_SETTLE) {
          if ((((uint)civs[s->civ].units_active[unit_type] - s->homeless_settlers) +
               s->units_in_build[unit_type] == 0) &&
             (3 - leaders[civs[s->civ].tribe].expand < (int)cities[city].size)) {
            support_penalty = 0;
            if (city_calc.total_shields <=
                (int)((uint)(-1 < leaders[civs[s->civ].tribe].expand) + city_calc.shield_support * 2)
               ) {
              score -= (int)score / 3;
            }
          }
          else {
            support_penalty = support_penalty / 2;
          }
        }
        else if (((s->threatened == 0) && (city_calc.total_shields <= city_calc.support_unit_count)) ||
                ((civs[s->civ].flags & CIVF_AT_WAR) == 0)) {
          score = score << 1;
        }
        score += support_penalty;
      }
      if (((role2 == ROLE_DEFEND) && ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0))
         && (citypref.no_defend != 0)) {
        score = 999;
      }
      if (((int)score < s->choice_score) ||
         (((s->choice < 0 && ((cities[city].flags & CITYF_DISORDER) == 0)) &&
          ((1 < s->defense_need ||
           (((s->defense_urgent != 0 && (s->unrest_state < 2)) &&
            ((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0)))))))) {
        s->choice_score = score;
        s->choice = unit_type;
      }
      if ((int)score < s->best_unit_score) {
        s->best_unit_score = score;
        s->best_unit = unit_type;
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the final choice: an AI city
   keeps its wonder or palace, a human's city follows its governor's
   preference. */
static iptr city_choose_production_final_choice(ChooseProdState *s,iptr city)
{
  int ret;
  i32 bits;
  iptr unit2;
  iptr cur_improvement;
  iptr wonder;

  if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
     ((cities[city].flags & CITYF_AUTOBUILD) == 0)) {
    if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) && (cities[city].building == -IMP_PALACE))
    {
      s->choice = -1;
    }
    else if ((s->choice < -0x26) && (ret = city_has_improvement(city,IMP_TEMPLE), ret == 0)) {
      s->choice = -4;
    }
    else if ((cities[city].building < -0x26) &&
            (((s->same_build_count == 0 && (-0x27 < s->choice)) && ((cities[city].flags & CITYF_DISORDER_2ND_TURN) == 0)))) {
      if (cities[city].building < 1) {
        cur_improvement = ~(int)cities[city].building + 1;
      }
      else {
        cur_improvement = (iptr)cities[city].building;
      }
      wonder = cur_improvement - 0x27;
      if (((s->defense_need < 2) && (ret = wonder_is_expired(wonder), ret == 0)) &&
         ((game.wonder_city[wonder] == -1 &&
          ((s->defense_urgent == 0 || ((cities[city].flags & CITYF_DEFENDER_LOST) == 0)))))) {
        bits = 1;
        unit2 = tile_first_unit((int)cities[city].x,(int)cities[city].y);
        ret = unit_stack_count_role(unit2,bits);
        if (1 < ret) {
          s->choice = (iptr)cities[city].building;
        }
      }
    }
  }
  else if ((cities[city].flags & CITYF_AUTO_UNITS_ONLY) == 0) {
    if ((cities[city].flags & CITYF_AUTO_IMPROVEMENTS_ONLY) == 0) {
      if (500 < s->choice_score) {
        s->choice = 99;
      }
    }
    else if ((0x62 < s->best_build) || (s->choice = s->best_build, 500 < s->best_build_score)) {
      s->choice = 99;
    }
  }
  else if ((0x62 < s->best_unit) || (s->choice = s->best_unit, 500 < s->best_unit_score)) {
    s->choice = 99;
  }
  return s->choice;
}

/* 00498e8b auto */
/* WARNING: Type propagation algorithm not settling */

iptr __cdecl city_choose_production(iptr city,iptr out_best_unit,iptr out_best_build)

{
  iptr behind;
  iptr city_landmass;
  int ret;
  int unit_counts[62];
  iptr improvement;
  int units_in_build[62];
  int lost_cities;
  iptr unit_type;
  i8 wonder_builders[28];
  iptr wonder;
  uptr civ2;
  iptr unit_type2;
  int pref_chosen;
  ChooseProdState st;

  st.wonder_builders = wonder_builders;
  st.units_in_build = units_in_build;
  st.unit_counts = unit_counts;
  st.expand_bonus = 0;
  if (out_best_unit != 0) {
    *(i32 *)out_best_unit = 999;
  }
  if (out_best_build != 0) {
    *(i32 *)out_best_build = 999;
  }
  city_recalc(city,0);
  st.civ5 = cities[city].owner;
  st.civ = (uptr)(char)st.civ5;
  st.landmass2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
  st.policy = (uint)civs[st.civ].continent_policy[st.landmass2];
  if ((st.policy == 0) || (st.policy == 1)) {
    st.threatened = 1;
  }
  else {
    st.threatened = 0;
  }
  if (((game.game_flags & GAME_AI_GANG_UP) != 0) && (st.policy == 4)) {
    st.threatened = 1;
  }
  behind = ai_spaceship_race_behind(st.civ);
  st.space_race = spaceship_race_active();
  st.space_race = behind + st.space_race;
  lost_cities = 0;
  for (st.city2 = 0; st.city2 < game.num_cities; st.city2++) {
    if ((((cities[st.city2].id != 0) && ((int)cities[st.city2].founder == st.civ)) &&
        ((int)cities[st.city2].owner != st.civ)) &&
       ((city_landmass = tile_landmass((int)cities[st.city2].x,(int)cities[st.city2].y), city_landmass == st.landmass2
        && (((uint)game.human_players & 1 << (cities[st.city2].owner & 0x1fU)) != 0)))) {
      lost_cities++;
    }
  }
  if (leaders[civs[st.civ].tribe].attack + 1 < lost_cities) {
    st.threatened = 1;
  }
  st.food_surplus = (city_calc.yield_totals[0] - (int)cities[city].size * (uint)cosmic.food_per_citizen) -
              city_calc.settlers_supported * city_calc.settler_food_cost;
  st.ocean_tiles = 0;
  st.shield_surplus = city_calc.total_shields * 2 - city_calc.shield_support;
  st.net_trade = city_calc.total_trade - city_calc.trade_corruption;
  if ((cities[city].building < -0x26) && (0x27 < cities[city].shield_box)) {
    st.wonder_in_progress = 1;
  }
  else {
    st.wonder_in_progress = 0;
  }
  st.wonders_here = 0;
  st.wonders_owned = 0;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    st.wonder_builders[wonder] = 0;
    ret = wonder_get_city(wonder);
    if (ret == city) {
      st.wonders_here++;
    }
    ret = civ_has_wonder(st.civ,wonder);
    if (ret != 0) {
      st.wonders_owned++;
    }
  }
  if (((cities[city].flags & CITYF_DISORDER) == 0) && (((byte)(cities[city].flags >> 8) & ((CITYF_ON_EDGE | CITYF_AI_SPECIALIST_HELD) >> 8)) != 0xc0)) {
    if (city_calc.unhappy_count == city_calc.happy_count) {
      st.unrest_state = 1;
    }
    else {
      st.unrest_state = 0;
    }
  }
  else {
    st.unrest_state = 2;
  }
  city_find_nearest((int)cities[city].x,(int)cities[city].y,st.civ,0xfffffffe,-1);
  st.nearest_dist = advisor_window_block.nearest_city_dist;
  city_choose_production_scan_tiles(&st,city);
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    st.units_in_build[unit_type] = 0;
    st.unit_counts[unit_type] = 0;
  }
  city_choose_production_survey_cities(&st,city);
  city_choose_production_count_units(&st,unit_type);
  city_choose_production_expand_bonus(&st,city);
  st.settle_goals = 0;
  for (unit_type2 = 0; unit_type2 < 0x10; unit_type2++) {
    if ((civs[st.civ].ai_standing_goals[unit_type2].role == ROLE_DEFEND) &&
       (ret = tile_landmass((int)civs[st.civ].ai_standing_goals[unit_type2].x,
                              (int)civs[st.civ].ai_standing_goals[unit_type2].y),
       ret == st.landmass2)) {
      st.settle_goals++;
    }
  }
  if (st.threatened != 0) {
    st.settle_goals += (st.settle_goals >> 1);
  }
  if ((((st.unit_total == 0) && ((1 << (st.civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
      (civs[st.civ].government < GOV_MONARCHY)) && ((out_best_unit == 0 && (out_best_build == 0)))) {
    for (unit_type = 0x3d; -1 < unit_type; unit_type--) {
      if (((unit_types[unit_type].role == ROLE_DEFEND) && ((unit_types[unit_type].flags & UTYPE_PARADROP) == 0)) &&
         (ret = civ_can_build_unit(st.civ,city,unit_type), ret != 0)) {
        return unit_type;
      }
    }
  }
  if (st.civ == 0) {
    return city_choose_production_barbarian(city);
  }
  st.enemy_offshore = 0;
  if (st.policy == 4) {
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (((*((byte *)&civs[st.civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) != 0) &&
         (*(char *)&civs[civ2].continent_cities[st.landmass2] == '\0')) {
        st.enemy_offshore = 1;
      }
    }
  }
  city_choose_production_defense_need(&st,city);
  if ((1 << (st.civ5 & 0x1f) & (uint)game.human_players) == 0) {
    city_choose_production_sell_redundant(&st,city);
    city_choose_production_airlift_defender(&st,city);
  }
  st.best_build_score = 999;
  st.best_unit_score = 999;
  st.choice_score = 999;
  st.best_build = 999;
  st.best_unit = 999;
  st.choice = 999;
  for (improvement = 1; improvement < 0x27; improvement++) {
    city_choose_production_try_improvement(&st,city,improvement);
  }
  pref_chosen = 0;
  if (((1 << (st.civ5 & 0x1f) & (uint)game.human_players) != 0) && (citypref.count != 0)) {
    for (unit_type2 = 0; unit_type2 < citypref.count; unit_type2++) {
      improvement = (((i32 *)&map_views_block.citypref_improvements))[unit_type2];
      ret = city_has_improvement(city,improvement);
      if ((ret == 0) && (ret = city_can_build_improvement(st.civ,city,improvement), ret != 0)) {
        st.best_build = -improvement;
        st.choice = -improvement;
        pref_chosen = 1;
        break;
      }
    }
  }
  /* an improvement from the human player's CITYPREF.TXT list skips the wonders and special units */
  if (pref_chosen == 0) {
    if ((st.wonder_in_progress != 0) ||
       (((((civs[st.civ].continent_flags[st.landmass2] & 0x10) == 0 &&
          (2 < cities[city].size)) && (((byte)(cities[city].flags >> 8) & ((CITYF_ON_EDGE | CITYF_AI_SPECIALIST_HELD) >> 8)) != 0xc0))
        && ((4 - leaders[civs[st.civ].tribe].civilize <=
             (int)(uint)civs[st.civ].continent_cities[st.landmass2] ||
            ((*(char *)&civs[st.civ].continent_flags[st.landmass2] == '\0' &&
             (4 - leaders[civs[st.civ].tribe].civilize <= (int)civs[st.civ].num_cities)))))))) {
      for (wonder = 0; wonder < 0x1c; wonder++) {
        city_choose_production_try_wonder(&st,city,wonder);
      }
    }
    if (civs[st.civ].num_military_units < 0x200) {
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        city_choose_production_try_special_unit(&st,city,unit_type);
      }
    }
  }
  if (civs[st.civ].num_military_units < 0x200) {
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      city_choose_production_try_military_unit(&st,city,unit_type);
    }
  }
  else if (st.choice == 999) {
    st.choice = -0x26;
  }
  if (out_best_build != 0) {
    *(iptr *)out_best_build = st.best_build;
  }
  if (out_best_unit != 0) {
    *(iptr *)out_best_unit = st.best_unit;
  }
  return city_choose_production_final_choice(&st,city);
}

/* 004bb8e0 auto */
void __cdecl wonder_show_movie(iptr wonder)

{
  i32 ret_addr;
  WonderMovieWindow movie_wnd2;
  
  snd_wave_stop_all();
  movie_wnd2.construct();
  ON_SCOPE_EXIT(movie_wnd2.destruct());
  blink.unit_active = 0;
  snd_cd_stop();
  ret_addr = 0x4bb937;
  movie_wnd2.create(wonder);
  movie_wnd2.play();
  snd_wave_stop_all();
  snd_sync_music_option();
  if (game_language != 0) {
    ret_addr = 0x4bb96a;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_improvement_modal(wonder + 0x27);
  }
  return;
}

/* 004bb99f auto */
WonderMovieWindow *WonderMovieWindow::construct()

{
  
  this->FrameWindow::construct();
  player.construct();
  surface_ctor(&frame_surface);
  palette.init();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 2);
  (*(FrameWindow * *)&font_size16_bold_block.wonder_movie_wnd) = this;
  return this;
}

/* 004bba79 auto */
void WonderMovieWindow::destruct()

{
  int seh_state;
  
  *(pointer *)&vtable = ((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 2);
  seh_state = seh_state & 0xff;
  font_size16_bold_block.wonder_movie_wnd = 0;
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 004bbaf1 */ { ((GamePalette *)(((int)this) + 0xd4c))->destroy(); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 004bbb00 */ { surface_dtor((DibSurface *)(((int)this) + 0xd04)); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 004bbb0f */ { ((AviPlayer *)(((int)this) + 0x2d8))->destruct(); }
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
    art_dll.resource_dll_load((iptr)path);
    ON_SCOPE_EXIT(art_dll.free());
    ok = art_surface.load_gif_resource(wonder + 20000,10,0xec,(iptr)palette_bytes);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_failed_to_load_civ2art_gif,str_notice_9,0x40);
    }
    else {
      sprite.grab_from_surface((iptr)&art_surface,2,1,1,0x40,0x20);
      frame_surface.DibSurface::create(0x40,0x20);
      sprite.draw((iptr)draw_rect,(iptr)(frame_surface_bytes),0,0);
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
  title = (iptr)label_string(improvements[wonder_index + 0x27].name);
  this->FrameWindow::create((const char *)title,style,pos_x,pos_y,wnd_width,wnd_height,arg7,arg8,arg9);
  ((WndBase *)&window)->set_palette((iptr)palette_bytes);
  wnd_width = 0;
  rect = (iptr)get_rect();
  fill_rect(rect,wnd_width);
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
      str_append_int((iptr)path,0);
    }
    str_append_int((iptr)path,wonder_index);
    str_concat((iptr)path,(iptr)&str_avi_2);
    found = file_resolve_path((const char *)path);
    if (found != 0) {
      window.wnd_handlers.set_lbuttonup_handler((WndMouseFn)wonder_movie_on_lbuttonup);
      window.wnd_handlers.set_keydown_handler(wonder_movie_on_key);
      player.create_child((iptr)&str_empty_116,0x200,border_width,caption_height,
                 (iptr)this);
      open_result = ((AviPlayer *)&player_head)->open((const char *)path);
      if (open_result == 0) {
        ((AviPlayer *)&player_head)->set_end_callback(wonder_movie_on_video_end);
        ((WndHandlers *)&player_field_58)->set_lbuttonup_handler((WndMouseFn)wonder_movie_on_lbuttonup);
        ((DibSurface *)&player_head)->avi_player_set_palette((iptr)(palette_bytes))
        ;
        ((WndBase *)&player_field_48)->set_palette((iptr)(palette_bytes));
        screen_fade_out();
        main_frame_wnd_block.main_frame_wnd.set_palette((iptr)(palette_bytes));
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
        main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
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
  frame_set_border_tile((iptr)(frame_surface_bytes));
  this->FrameWindow::draw_caption();
  frame_set_border_tile((iptr)&window_border_tile_surface);
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
  fill_rect(rect,color);
  repaint();
  ((WndBase *)&window)->center();
  wnd_flush_paint();
  return;
}

/* 004bc193 auto */
void __cdecl wonder_movie_on_video_end(void)

{
  ((ModalMenuWindow *)(font_size16_bold_block.wonder_movie_wnd + 0x48))->end_modal_loop();
  return;
}

/* 004bc1b1 auto */
void __cdecl wonder_movie_on_lbuttonup(void)

{
  ((ModalMenuWindow *)(font_size16_bold_block.wonder_movie_wnd + 0x48))->end_modal_loop();
  return;
}

/* 004bc1cf auto */
void __cdecl wonder_movie_on_key(int key)

{
  if ((0xcf < key) && (key < 0xd3)) {
    ((ModalMenuWindow *)(font_size16_bold_block.wonder_movie_wnd + 0x48))->end_modal_loop();
  }
  return;
}

/* 004c03ae auto */
i32 __cdecl city_can_build_improvement(iptr civ,iptr city,iptr improvement)

{
  int ok;
  i32 result;
  
  result = 0;
  if (improvement < 0x27) {
    ok = civ_has_advance(civ,(int)improvements[improvement].preq);
    if (ok != 0) {
      if (city < 0) {
        result = 1;
      }
      else {
        ok = city_has_improvement(city,improvement);
        if (((((ok == 0) &&
              (((improvement != IMP_HARBOR && (improvement != IMP_OFFSHORE_PLATFORM)) || ((cities[city].flags & CITYF_COASTAL) != 0))))
             && (((improvement != IMP_PORT_FACILITY && (improvement != IMP_COASTAL_FORTRESS)) ||
                 (((cities[city].flags & CITYF_OCEAN_ACCESS) != 0 && ((cities[city].flags & CITYF_COASTAL) != 0))
                 )))) &&
            ((((improvement != IMP_GRANARY || (ok = civ_has_wonder(civ,WONDER_PYRAMIDS), ok == 0)) &&
              ((improvement != IMP_CATHEDRAL || (ok = civ_has_wonder(civ,WONDER_MICHELANGELO), ok == 0)))) &&
             ((improvement != IMP_COURTHOUSE || (ok = city_has_improvement(city,IMP_PALACE), ok == 0)))))) &&
           (((((improvement != IMP_BANK ||
               ((ok = city_has_improvement(city,IMP_MARKETPLACE), ok != 0 &&
                (ok = civ_has_advance(civ,(int)improvements[IMP_MARKETPLACE].preq), ok != 0)))) &&
              ((improvement != IMP_STOCK_EXCHANGE ||
               ((ok = city_has_improvement(city,IMP_BANK), ok != 0 &&
                (ok = civ_has_advance(civ,(int)improvements[IMP_BANK].preq), ok != 0)))))) &&
             (((improvement != IMP_UNIVERSITY ||
               ((ok = city_has_improvement(city,IMP_LIBRARY), ok != 0 &&
                (ok = civ_has_advance(civ,(int)improvements[IMP_LIBRARY].preq), ok != 0)))) &&
              (((improvement != IMP_RESEARCH_LAB ||
                (((ok = civ_has_wonder(civ,WONDER_SETI), ok == 0 &&
                  (ok = city_has_improvement(city,IMP_UNIVERSITY), ok != 0)) &&
                 (ok = civ_has_advance(civ,(int)improvements[IMP_UNIVERSITY].preq), ok != 0)))) &&
               ((improvement != IMP_SEWER_SYSTEM || (ok = city_has_improvement(city,IMP_AQUEDUCT), ok != 0)))))))) &&
            ((((improvement != IMP_POLICE_STATION || (ok = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE), ok == 0)) &&
              ((((improvement != IMP_MFG_PLANT ||
                 ((ok = city_has_improvement(city,IMP_FACTORY), ok != 0 &&
                  (ok = civ_has_advance(civ,(int)improvements[IMP_FACTORY].preq), ok != 0))))
                && ((improvement != IMP_HYDRO_PLANT || ((cities[city].flags & CITYF_HYDRO_POSSIBLE) != 0)))) &&
               (((((improvement != IMP_POWER_PLANT && (improvement != IMP_HYDRO_PLANT)) && (improvement != IMP_SOLAR_PLANT)) && (improvement != IMP_NUCLEAR_PLANT)
                 ) || (((ok = city_has_improvement(city,IMP_FACTORY), ok != 0 ||
                        (ok = city_has_improvement(city,IMP_MFG_PLANT), ok != 0)) &&
                       ((improvement == IMP_SOLAR_PLANT ||
                        ((((ok = civ_has_wonder(civ,WONDER_HOOVER_DAM), ok == 0 &&
                           (ok = city_has_improvement(city,IMP_HYDRO_PLANT), ok == 0)) &&
                          ((ok = city_has_improvement(city,IMP_NUCLEAR_PLANT), ok == 0 ||
                           (improvement == IMP_HYDRO_PLANT)))) &&
                         (ok = city_has_improvement(city,IMP_SOLAR_PLANT), ok == 0)))))))))))) &&
             (((improvement < 0x23 || (0x25 < improvement)) ||
              ((((game.rules & RULE_BLOODLUST) == 0 && (game.wonder_city[WONDER_APOLLO] != -1)) &&
               (((0 < game.year || ((game.game_flags & GAME_SCENARIO) != 0)) &&
                (ok = civ_spaceship_launched(civ), ok == 0)))))))))))) {
          result = 1;
        }
      }
    }
  }
  else {
    result = civ_can_build_wonder(civ,improvement - 0x27);
  }
  return result;
}

/* 004c66ba auto */
void __cdecl city_incite_transfer(iptr city,iptr civ,i32 reason)

{
  char *unit_counter;
  byte civ3;
  short x2;
  short y2;
  int civ2;
  iptr x;
  int y;
  int ok;
  iptr unit;
  iptr tile_index;
  
  civ3 = cities[city].owner;
  civ2 = (int)(char)civ3;
  x2 = cities[city].x;
  y2 = cities[city].y;
  if (-1 < civ) {
    net_batch_begin();
    for (tile_index = 0; tile_index < 0x15; tile_index++) {
      x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index] + (int)x2);
      y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index] + (int)y2;
      ok = map_in_bounds(x,y);
      if (ok != 0) {
        tile_set_visible(x,y,1 << ((byte)civ & 0x1f),1,1);
        tile_update_known(x,y,civ);
      }
    }
    net_batch_flush();
    if ((autoplay.active == 0) &&
       (((((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
            ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) ||
           ((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_EMBASSY) != 0)) ||
          ((y = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), y != 0 ||
           (y = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), y != 0)))) ||
         (((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ2] & TREATY_EMBASSY) != 0 ||
          ((y = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), y != 0 ||
           (y = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), y != 0)))))) ||
        (game.reveal_map != '\0')))) {
      if (((game.reveal_map != '\0') ||
          ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) != 0)) ||
         ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))) {
        mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,(int)cities[city].owner)
        ;
      }
      x = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(0,(const char *)x);
      text_set_string_arg(1,(const char *)&cities[city].name);
      x = civ_get_adjective(civ);
      text_set_string_arg(2,(const char *)x);
      popup_game((const char *)&str_civilwar);
      if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer)) {
        net_send_text_args(0x5e,net.net_slots[net.net_civ_slots[civ2]].id,3,0,0,
                           0);
      }
    }
    unit = (iptr)game.num_units;
    while (unit = unit - 1, -1 < unit) {
      if (((units[unit].id != 0) && (units[unit].owner == civ2)) &&
         ((y = map_distance_diag((int)units[unit].x,(int)units[unit].y,(int)x2,
                                      (int)y2), y < 2 &&
          ((y == 0 ||
           (ok = tile_city_owner((int)units[unit].x,(int)units[unit].y), ok < 0))))))
      {
        unit_counter = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *unit_counter = *unit_counter - 1;
        unit_counter = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *unit_counter = *unit_counter + 1;
        units[unit].owner = (byte)civ;
        units[unit].home_city = (i8)city;
        units[unit].moves_used = '\0';
        if ((units[unit].orders != ORDER_FORTIFYING) && (units[unit].orders != ORDER_FORTIFIED)) {
          units[unit].orders = ORDER_NONE;
        }
        tile_set_owner((int)units[unit].x,(int)units[unit].y,civ,1);
        if (y != 0) {
          map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
        }
      }
    }
    city_capture_aftermath(city,civ,reason);
  }
  return;
}

/* 004d8bc0 auto */
void __cdecl impr_editor_load_rules(void)

{
  char *_Source;
  size_t _Count;
  iptr improvement;
  
  for (improvement = 0; improvement < 0x43; improvement++) {
    _Count = 0x28;
    _Source = label_string(improvements[improvement].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[improvement],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[improvement][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[0] = (uint)improvements[improvement].cost;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[1] = (uint)improvements[improvement].upkeep;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[2] = (int)improvements[improvement].preq;
    if (0x27 < improvement) {
      editor_text_dlg_wnd_block.editor_item_recs[improvement].values[3] = (int)wonder_expires[improvement - 0x27];
    }
  }
  return;
}

/* 004d8caa auto */
void __cdecl impr_editor_store_rules(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr improvement;
  
  for (improvement = 0; improvement < 0x43; improvement++) {
    _Count = 0x19;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[improvement];
    _Dest = label_string(improvements[improvement].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    improvements[improvement].cost = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[0];
    improvements[improvement].upkeep = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[1];
    improvements[improvement].preq = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[2];
    if (0x27 < improvement) {
      wonder_expires[improvement - 0x27] = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[3];
    }
  }
  return;
}

/* 004d8d80 auto */
void __cdecl impr_editor_load_controls(void)

{
  int ctrl_id;
  int ctrl_index;
  char value_text[8];
  int sel_index;
  
  for (ctrl_index = 1; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      _sprintf(value_text,((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_value_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->set_text(
                 (const char *)value_text);
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      sel_index = *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58) + 2;
      editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot].set_cur_sel(sel_index);
    }
  }
  return;
}

/* 004d8ed6 auto */
int __cdecl impr_editor_save_controls(void)

{
  int field;
  i32 clamped;
  int ctrl_index;
  int clamp_count;
  char value_text[8];
  int value;
  
  clamp_count = 0;
  for (ctrl_index = 1; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->Control::get_text(
                 (char *)value_text);
      field = ((Control *)
                         &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      field -= 0xca;
      value = _atoi(value_text);
      clamped = int_clamp(value,*(i32 *)(((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_field_min_table) + field * 4),
                                 *(i32 *)(((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_field_max_table) + field * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] = clamped;
      if (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] != value) {
        clamp_count++;
      }
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      value = editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot].get_cur_sel();
      value -= 2;
      field = ((Control *)
                         &editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + field * 4) = value;
    }
  }
  return clamp_count;
}

/* 004d9095 auto */
void __cdecl impr_editor_redraw(void)

{
  editor_active.wnd->impr_editor_paint();
  return;
}

/* 004d90b0 auto */
i32 __cdecl impr_editor_write_section(FILE *file)

{
  char *name_str;
  size_t len;
  size_t name_len;
  iptr improvement;
  
  for (improvement = 0; improvement < 0x43; improvement++) {
    text_clear();
    text_append_string_id(improvements[improvement].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&impr_editor_ctrl_kinds_block.rules_improve_name_sep_str));
    name_str = label_string(improvements[improvement].name);
    len = _strlen(name_str);
    if (len < 0x19) {
      name_str = label_string(improvements[improvement].name);
      name_len = _strlen(name_str);
    }
    else {
      name_len = 0x19;
    }
    text_append_spaces(0x19 - name_len);
    text_append_int_2d((uint)improvements[improvement].cost);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&impr_editor_ctrl_kinds_block.rules_improve_cost_sep_str));
    text_append_int_2d((uint)improvements[improvement].upkeep);
    crt_strcat(((i8 *)&textbuf.buf),str_empty_137);
    text_append_advance_abbrev((int)improvements[improvement].preq,1);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_improve_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004d91f8 auto */
i32 __cdecl impr_editor_write_endwonder(FILE *file)

{
  iptr improvement;
  
  for (improvement = 0x27; improvement < 0x43; improvement++) {
    text_clear();
    text_append_advance_abbrev((int)wonder_expires[improvement - 0x27],1);
    text_append_spaces(8);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_endwonder_comment_sep_str));
    text_append_string_id(improvements[improvement].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_endwonder_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004d929a auto */
void __cdecl impr_editor_on_ok(void)

{
  iptr clamp_count;
  HWND hWnd;
  iptr write_ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg_text[32];
  
  clamp_count = impr_editor_save_controls();
  if (clamp_count == 0) {
    impr_editor_store_rules();
    rules_edit_set_section((iptr)&str_improve_2,(RulesSectionWriterFn)impr_editor_write_section);
    rules_edit_add_section((const char *)&str_endwonder_3,(RulesSectionWriterFn)impr_editor_write_endwonder);
    write_ok = rules_write_sections();
    if (write_ok == 0) {
      _sprintf(msg_text,str_error_updating_rules_s_3,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error_3;
      lpText = msg_text;
      wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    impr_editor_load_controls();
    impr_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_10);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 004d93b9 auto */
void __cdecl impr_editor_on_rename(void)

{
  int item_index;
  size_t len;
  iptr parent_wnd2;
  int item;
  char old_name[25];
  char new_name[260];
  iptr cur_sel;
  int dlg_result;
  
  item_index = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item_index],0x19);
  old_name[24] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    dlg_result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_imprname,0x18,(iptr)old_name,(iptr)new_name);
    dlg_pop_parent_window();
    if (dlg_result == -1) break;
    len = _strlen(new_name);
  } while (len == 0);
  if (-1 < dlg_result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item_index],new_name);
    cur_sel = editor_active.wnd->combos[0].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    for (item = 0; item < 0x43; item++) {
      editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
    }
    editor_active.wnd->combos[0].set_cur_sel(cur_sel);
    impr_editor_redraw();
  }
  return;
}

/* 004d953f auto */
void __cdecl impr_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_4,(const char *)&str_improvements);
  dlg_set_parent_window(0);
  return;
}

/* 004d959e auto */
void __cdecl impr_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 004d95c6 auto */
void __cdecl impr_editor_edit_icon(void)

{
  i32 title;
  i32 sprite_count;
  SpriteSheetSaveFn save_callback;
  
  save_callback = sprite_sheet_save_building_icons;
  sprite_count = 4;
  title = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + editor_active.wnd->item * 0x3c),(const char *)title,sprite_count,
                     save_callback);
  impr_editor_redraw();
  return;
}

/* 004d9619 auto */
void __cdecl impr_editor_edit_misc_icon(void)

{
  int choice;
  i32 title;
  SpriteSheetSaveFn save_callback;
  iptr parent_wnd2;
  i8 *sprites;
  i32 sprite_count2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_imprmisc,1);
  /* BF-114 (Peter Johnson's ledger): a choice past the cursor and the eight
     explosions (a row added to a scenario's DEBUG.TXT) is no choice */
  if ((choice != -1) && (-1 < choice) && (choice < 9)) {
    if (choice == 0) {
      sprites = ((i8 *)&map_cursor_sprite_block.map_cursor_sprite);
      sprite_count2 = 5;
    }
    else {
      sprites = ((i8 *)&government_sprites_block.explosion_sprites) + (choice * 4 - 4) * 0xf;
      sprite_count2 = 6;
    }
    if (sprites != (i8 *)0x0) {
      save_callback = sprite_sheet_save_building_icons;
      title = (iptr)label_string(label_table.string_ids[0x1f6]);
      sprite_editor_run((iptr)sprites,(const char *)title,sprite_count2,save_callback);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 004d9718 auto */
void __cdecl impr_editor_on_select(int ctrl_id)

{
  int clamp_count;
  HWND hWnd;
  i32 cur_sel;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    clamp_count = impr_editor_save_controls();
    if (clamp_count == 0) {
      cur_sel = editor_active.wnd->combos[0].get_cur_sel();
      editor_active.wnd->item = cur_sel;
      if (editor_active.wnd->item == 0) {
        editor_active.wnd->icon_button.disable();
      }
      else {
        editor_active.wnd->icon_button.enable();
      }
      if (editor_active.wnd->item < 0x27) {
        ((Control *)&editor_active.wnd->combos[2])->hide();
      }
      else {
        ((Control *)&editor_active.wnd->combos[2])->show_and_redraw();
      }
      impr_editor_load_controls();
      impr_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      impr_editor_load_controls();
      impr_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_11);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  return;
}

/* 004d986e auto */
void EditorWindow::impr_editor_create_combo(int combo_index)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  iptr improvement;
  iptr advance;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,impr_editor_combo_x_table_block.impr_editor_combo_pos[combo_index].x + client_x - 0x32,
                     impr_editor_combo_x_table_block.impr_editor_combo_pos[combo_index].y + client_y,200,
                     font_height_uptr << 3);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_callback(impr_editor_on_select);
  if (combo_index == 0) {
    for (improvement = 0; improvement < 0x43; improvement++) {
      label = (iptr)label_string(improvements[improvement].name);
      ((ComboBox *)&combos_u)->add_string((const char *)label);
    }
  }
  else if ((0 < combo_index) && (combo_index < 3)) {
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    }
  }
  return;
}

/* 004d9a9f auto */
void EditorWindow::impr_editor_create_edit(int edit_index)

{
  int ctrl_id;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&impr_editor_edit_x_table_block.impr_editor_edit_x_table))[edit_index * 2] + client_x,
                     (((i32 *)&impr_editor_edit_x_table_block.impr_editor_edit_y_table))[edit_index * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_138);
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->set_max_len(3);
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 004d9b93 auto */
void EditorWindow::impr_editor_paint()

{
  byte key_color;
  int open_err;
  char *percent_pos;
  size_t line_len;
  i8 sprite_rect[16];
  Sprite *icon_sprite;
  int text_y2;
  char *line;
  int text_x2;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  if ((int)item_uptr < 0x27) {
    icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + item_uptr * 0x3c);
  }
  else {
    icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (item_uptr * 4 - 0x9c) * 0xf);
  }
  if (item_uptr == 0) {
    gfx_fill_rect_xywh((iptr)this,icon_x,icon_y,0x48,0x28,10);
  }
  else {
    key_color = icon_sprite->get_key_color();
    gfx_fill_rect_xywh((iptr)this,icon_x,icon_y,0x48,0x28,(uint)key_color);
    gfx_set_scale_ratio(2,1);
    icon_sprite->draw((iptr)sprite_rect,(iptr)this,icon_x,icon_y);
    gfx_set_scale_ratio(1,1);
  }
  gfx_draw_tiled_frame((iptr)this,icon_x,icon_y,0x48,0x28,6);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(10,10,0,0);
  gfx_fill_rect_xywh((iptr)this,client_x + 0x32,client_y + 0xe8,0x1cc,0x6e,0x29);
  gfx_draw_frame_rect((iptr)this,client_x + 0x32,client_y + 0xe8,0x1cc,0x6e,10);
  text_x2 = client_x + 0x40;
  text_y2 = client_y + 0xee;
  text_clear();
  text_append((const char *)&str_pediaimprove);
  text_append_int(item_uptr);
  open_err = text_open_section((const char *)&str_pedia,(const char *)&textbuf.buf);
  if (open_err == 0) {
    do {
      line = text_read_line_or_end(1);
      if (*line == '^') {
        line++;
      }
      percent_pos = _strstr(line,((i8 *)&impr_editor_help_percent_str));
      if (percent_pos != (char *)0x0) {
        percent_pos = _strstr(line,((i8 *)&impr_editor_help_percent2_str));
        percent_pos[1] = ' ';
      }
      text_draw_shadowed((iptr)this,(const char *)line,text_x2,text_y2);
      text_y2 += font_height_uptr;
      line_len = _strlen(line);
    } while (1 < line_len);
  }
  text_set_colors(0x29,0x12,1,1);
  text_x2 = client_x + (int)wnd_width_u / 2;
  text_y2 = (client_y - font_height_uptr) + 0xe6;
  text_clear();
  text_append_label(0x1ea);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = client_x + (int)wnd_width_u / 2;
  text_y2 = impr_editor_combo_x_table_block.impr_editor_combo1_y + client_y + font_height_uptr * -2 - 2;
  text_clear();
  text_append_label(0x1e9);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_combo_x_table_block.impr_editor_combo1_x + client_x + 0x32;
  text_y2 = ((impr_editor_combo_x_table_block.impr_editor_combo1_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_combo_x_table_block.impr_editor_combo2_x + client_x + 0x32;
  text_y2 = ((impr_editor_combo_x_table_block.impr_editor_combo2_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1de);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_edit_x_table_block.impr_editor_edit_x_table[0] + client_x + 0x18;
  text_y2 = ((impr_editor_edit_x_table_block.impr_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e1);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_edit_x_table_block.impr_editor_edit1_x + client_x + 0x18;
  text_y2 = ((impr_editor_edit_x_table_block.impr_editor_edit1_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x9e);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  wnd_blit_surface();
  return;
}

/* 004da107 auto */
void EditorWindow::impr_editor_window_run()

{
  DibSurface *this_00;
  uptr line_height;
  i32 title;
  int btn_x;
  int btn_y;
  iptr label;
  i32 style;
  i32 wnd_x;
  i32 wnd_y;
  u32 wnd_width;
  u32 wnd_height;
  i32 arg7;
  i32 arg8;
  i32 arg9;
  u8 *misc_icon_parent;
  u8 *icon_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  DibSurface *bg_surface2;
  int ctrl_index;
  i8 button_rect[16];
  GamePalette palette;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  editor_ui.bg_surface = bg_surface2;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 1;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editoras_gif,10,0xc0,(iptr)&palette);
  line_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = line_height;
  arg9 = 0;
  arg8 = 0;
  arg7 = 0;
  wnd_height = wnd_height_u;
  wnd_width = wnd_width_u;
  wnd_y = 0;
  wnd_x = 0;
  style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x1cc]);
  this->FrameWindow::create((const char *)title,style,wnd_x,wnd_y,wnd_width,wnd_height,arg7,arg8,arg9);
  for (ctrl_index = 0; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      impr_editor_create_edit(impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot);
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      impr_editor_create_combo(impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot);
    }
  }
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_width - 10;
  button_width_u = (int)(btn_x + (btn_x >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  btn_x = client_x + 2;
  rect_set_xywh((iptr)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)ok_parent,0x65,(iptr)button_rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)impr_editor_on_ok);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((iptr)rename_parent,0x66,(iptr)button_rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)impr_editor_on_rename);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)help_parent,0x66,(iptr)button_rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)impr_editor_on_help);
  rect_set_xywh((iptr)button_rect,btn_x + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)cancel_parent,0x66,(iptr)button_rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)impr_editor_on_cancel);
  cancel_button.set_cancel_button();
  button_width_u = 0x55;
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_x + 0x19;
  btn_y = client_y + 0x50;
  rect_set_xywh((iptr)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    icon_parent = (u8 *)0x0;
  }
  else {
    icon_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((iptr)icon_parent,0x65,(iptr)button_rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)impr_editor_edit_icon);
  rect_set_xywh((iptr)button_rect,btn_x,btn_y + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    misc_icon_parent = (u8 *)0x0;
  }
  else {
    misc_icon_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23c]);
  misc_button.create((iptr)misc_icon_parent,0x65,(iptr)button_rect,(const char *)label);
  misc_button.set_callback((CtrlNotifyFn)impr_editor_edit_misc_icon);
  window.set_keyboard_nav(0);
  impr_editor_load_rules();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  impr_editor_load_controls();
  combo_count_u = 3;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(impr_editor_redraw);
  impr_editor_on_select(0xc9);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if (editor_ui.bg_surface != (DibSurface *)0x0) {
    editor_ui.bg_surface->surface_delete(1);
  }
  editor_ui.bg_surface = (DibSurface *)0x0;
  combo_count_u = 0;
  return;
}

/* 004da9e2 auto */
void __cdecl impr_editor_run(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.impr_editor_window_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 004e7270 auto */
int __cdecl city_acquire_wonder(iptr civ,iptr city,iptr wonder,iptr cost,iptr impr)

{
  char is_host;
  int start_time;
  int now;
  int acquired;
  
  acquired = 0;
  net.city_acquire_wonder_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    if (game.wonder_city[wonder] == -1) {
      acquired = 1;
      game.wonder_city[wonder] = (i16)city;
      cities[city].flags = cities[city].flags | CITYF_WONDER_BUILT;
      city_set_improvement(city,impr,1);
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) || (0x22 < impr)) {
        cities[city].shield_box = 0;
      }
    }
    if ((2 < game.multiplayer) && (acquired != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.city_acquire_wonder_busy = 0;
  }
  else {
    net.city_acquire_wonder_busy = 0;
    net.nm_wonder_acquired_result = -2;
    net_send_message(NM_ACQUIRE_WONDER,0,civ,city,wonder,cost,impr,0,0,0);
    start_time = timer_ticks();
    while ((net.nm_wonder_acquired_result == -2 && (now = timer_ticks(), now - start_time < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_wonder_acquired_result == -2) {
      dbg_log((const char *)&str_acquire_wonder_connection_to_server_time);
      acquired = 0;
    }
    else {
      while ((net.msgq_priority_count != 0 || (acquired = net.nm_wonder_acquired_result, net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return acquired;
}

/* 004e7458 auto */
i32 __cdecl city_production_category(iptr city,int build_item)

{
  i32 category;
  
  if (build_item < 0) {
    if (build_item < -0x22) {
      category = 2;
    }
    else {
      category = 1;
    }
  }
  else {
    category = 0;
  }
  return category;
}

/* 004e7492 auto */
void __cdecl city_save_build_state(iptr city)

{
  city_calc.saved_build_category = city_production_category(city,(int)cities[city].building);
  city_calc.saved_shield_box = (int)cities[city].shield_box;
  return;
}

/* 004e74df auto */
uint __cdecl city_shields_after_change(iptr city,iptr build_item)

{
  int category;
  uint shields;
  
  shields = (*(uint *)&city_calc.saved_shield_box);
  if ((game.difficulty != DIFF_CHIEFTAIN) &&
     (category = city_production_category(city,build_item), shields = (*(uint *)&city_calc.saved_shield_box), category != city_calc.saved_build_category)) {
    shields = (*(uint *)&city_calc.saved_shield_box) - ((uint)cosmic.change_penalty * ((*(uint *)&city_calc.saved_shield_box) & 0xffff)) / 100;
  }
  return shields;
}

/* 004e7549 auto */
void __cdecl city_set_specialist_type(iptr city,int slot,int type)

{
  byte shift;
  
  if (slot < 0x10) {
    shift = (char)slot * 2;
    cities[city].specialists = cities[city].specialists & ~(3 << (shift & 0x1f));
    cities[city].specialists = cities[city].specialists | type << (shift & 0x1f);
  }
  return;
}

/* 004e75a6 auto */
uint __cdecl city_specialist_type(iptr city,int slot)

{
  uint type;
  
  if (slot < 0x10) {
    type = (int)cities[city].specialists >> ((char)slot * 2 & 0x1fU) & 3;
  }
  else {
    type = 1;
  }
  return type;
}

/* 004e75ea auto */
int __cdecl city_count_specialists(iptr city,int type)

{
  int slot_type;
  i32 count;
  int slot;
  
  count = 0;
  for (slot = 0; slot < 0x10; slot++) {
    slot_type = city_specialist_type(city,slot);
    if (slot_type == type) {
      count++;
    }
  }
  return count;
}

/* 004e7641 auto */
void __cdecl city_calc_tile_flags(iptr city)

{
  byte civ2;
  int civ;
  iptr x;
  int y;
  byte *tile_flags;
  int result;
  uint tile_improv;
  iptr unit;
  iptr tile_index;
  
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  tile_index = 0;
  do {
    if (0x18 < tile_index) {
      return;
    }
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index] + (int)cities[city].x);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
    tile_flags = (byte *)(((i8 *)&city_calc.tile_flags) + tile_index);
    *tile_flags = 0;
    result = map_in_bounds(x,y);
    if ((result == 0) || (0x14 < tile_index)) {
      *tile_flags = *tile_flags | 1;
    }
    else {
      result = tile_visible_to(x,y,civ);
      if (result == 0) {
        *tile_flags = *tile_flags | 1;
      }
      else {
        if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (city_turn.processing != 0)) &&
           (tile_improv = tile_improvements(x,y), (tile_improv & TILE_POLLUTION) != 0)) {
          game.warming_pollution++;
        }
        unit = tile_first_unit(x,y);
        while (!(((unit < 0) || (units[unit].owner == civ)) ||
                ((result = tile_is_ocean(x,y), result != 0 ||
                 ((*(byte *)&civs[civ].treaties[units[unit].owner] & TREATY_ALLIANCE) != 0))))) {
          if (unit_types[units[unit].type].attack != '\0') {
            *tile_flags = *tile_flags | 4;
            if (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) {
              *tile_flags = *tile_flags | 0x20;
            }
            break;
          }
          unit = unit_next_in_stack(unit);
        }
        y = tile_city_owner(x,y);
        if (-1 < y) {
          *tile_flags = *tile_flags | 8;
        }
      }
    }
    tile_index++;
  } while( true );
}

/* 004e78ce auto */
uint __cdecl city_tile_worked(iptr city,iptr tile_index)

{
  return (uint)((cities[city].worked_tiles & 1 << ((byte)tile_index & 0x1f)) != 0);
}

/* 004e790c auto */
void __cdecl city_set_tile_worked(iptr city,iptr tile_index,int worked)

{
  uint mask;
  
  mask = 1 << ((byte)tile_index & 0x1f);
  if (worked == 0) {
    cities[city].worked_tiles = cities[city].worked_tiles & ~mask;
  }
  else {
    cities[city].worked_tiles = cities[city].worked_tiles | mask;
  }
  return;
}

/* 004e7967 auto */
void __cdecl city_calc_capital_link(iptr city)

{
  short x2;
  short y2;
  int x;
  int y;
  int x3;
  int y3;
  int dist;
  iptr city2;
  iptr near_tile;
  iptr tile_index;
  
  x = (int)cities[city].x;
  y = (int)cities[city].y;
  x3 = city_has_improvement(city,IMP_PALACE);
  if (x3 == 0) {
    city_calc.capital_distance = 0x20;
  }
  else {
    city_calc.capital_distance = 0;
  }
  city_calc.capital_nearest = -1;
  for (city2 = 0; city2 < game.num_cities; city2++) {
    if ((cities[city2].id != 0) && (city2 != city)) {
      x3 = map_distance_diag((int)cities[city].x,(int)cities[city].y,
                                 (int)cities[city2].x,(int)cities[city2].y);
      if ((cities[city2].owner == cities[city].owner) &&
         (y3 = city_has_improvement(city2,IMP_PALACE), y3 != 0)) {
        if ((x3 < city_calc.capital_distance) || (city_calc.capital_nearest < 0)) {
          city_calc.capital_nearest = city2;
        }
        if (x3 < city_calc.capital_distance) {
          city_calc.capital_distance = x3;
        }
      }
      if (x3 < 6) {
        for (tile_index = 0; tile_index < 0x15; tile_index++) {
          x3 = city_tile_worked(city2,tile_index);
          if ((x3 != 0) || (tile_index == 0x14)) {
            x3 = (int)cities[city2].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index];
            y3 = (int)cities[city2].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
            dist = map_distance_diag(x,y,x3,y3);
            if (dist < 3) {
              for (near_tile = 0; near_tile < 0x14; near_tile++) {
                if (((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[near_tile] == x3 - x) &&
                   ((int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[near_tile] == y3 - y)) {
                  (((i8 *)&city_calc.tile_flags))[near_tile] = (((i8 *)&city_calc.tile_flags))[near_tile] | 0x10;
                }
              }
            }
          }
        }
      }
    }
  }
  city_calc.capital_link = (uint)(city_calc.capital_distance == 0);
  if ((city_calc.capital_link != 0) &&
     (x3 = civ_has_advance((int)cities[city].owner,ADV_RAILROAD), x3 != 0)) {
    city_calc.capital_link++;
  }
  if ((-1 < city_calc.capital_nearest) && (city != city_calc.capital_nearest)) {
    x3 = city_has_improvement(city,IMP_AIRPORT);
    if ((x3 != 0) && (x3 = city_has_improvement(city_calc.capital_nearest,IMP_AIRPORT), x3 != 0)) {
      city_calc.capital_link = 1;
    }
    x3 = tile_landmass(x,y);
    x2 = cities[city_calc.capital_nearest].x;
    y2 = cities[city_calc.capital_nearest].y;
    y3 = tile_landmass((int)x2,(int)y2);
    if (y3 == x3) {
      city_calc.capital_link = map_road_connection((int)cities[city].owner,x,y,(int)x2,(int)y2
                                       );
    }
  }
  return;
}

/* 004e7d7f auto */
i32 __cdecl city_count_unit_support(iptr city,iptr unit,i32 government)

{
  city_calc.support_unit_count++;
  switch(government) {
  case GOV_ANARCHY:
  case GOV_DESPOTISM:
    if (cities[city].size < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_MONARCHY:
    if ((int)(uint)cosmic.monarchy_free_support < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_COMMUNISM:
    if ((int)(uint)cosmic.communism_free_support < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_FUNDAMENTALISM:
    if (((unit_types[units[unit].type].flags & UTYPE_FUNDAMENTALISM_FREE) == 0) &&
       ((int)(uint)cosmic.fundamentalism_free_support < city_calc.support_unit_count)) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  default:
    city_calc.shield_support++;
    return city_calc.shield_support;
  }
  return 0;
}

/* 004e7eb1 auto */
void __cdecl city_calc_food_box_rows(iptr city,iptr civ)

{
  byte food_cost;
  int adjusted;
  uint settler_food;
  int penalty;
  
  food_cost = cosmic.settler_food_low;
  if (GOV_MONARCHY < civs[civ].government) {
    food_cost = cosmic.settler_food_high;
  }
  settler_food = (uint)food_cost;
  (*(uint *)&city_calc.settler_food_cost) = settler_food;
  (*(uint *)&city_calc.food_box_rows) = (uint)cosmic.food_box_rows;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    (*(uint *)&city_calc.food_box_rows) = -(uint)game.difficulty + 0xd;
    if (game.difficulty < DIFF_KING) {
      (*(uint *)&city_calc.food_box_rows) = -(uint)game.difficulty + 0xe;
    }
    if (game.difficulty == DIFF_CHIEFTAIN) {
      (*(uint *)&city_calc.food_box_rows)++;
    }
    if (((200 < game.turn) && (DIFF_WARLORD < game.difficulty)) &&
       (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0)) {
      penalty = 2;
      if (((game.wonder_city[WONDER_MANHATTAN] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_ROCKETRY]) != 0)) &&
         ((penalty = 1, (game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0 ||
          ((game.human_players & game.advance_known_by[ADV_NUCLEAR_FISSION]) == 0)))) {
        penalty = 0;
      }
      adjusted = penalty;
      if (((game.wonder_city[WONDER_APOLLO] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_SPACE_FLIGHT]) != 0)) &&
         (adjusted = penalty - 1, (game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) == 0)) {
        adjusted = penalty - 2;
      }
      penalty = adjusted;
      adjusted = int_clamp(penalty,0,2);
      (*(uint *)&city_calc.food_box_rows) -= adjusted;
    }
    if ((cosmic.food_box_rows != '\n') &&
       ((*(uint *)&city_calc.food_box_rows) = (int)(cosmic.food_box_rows * (*(uint *)&city_calc.food_box_rows)) / 10, ((*(uint *)&city_calc.food_box_rows) & 1) != 0)) {
      (*(uint *)&city_calc.food_box_rows)++;
    }
  }
  return;
}

/* 004e80b1 auto */
void __cdecl city_calc_unit_support(uptr city)

{
  byte civ2;
  byte government;
  iptr civ;
  int result;
  int check;
  i32 tile_improv;
  iptr unit;
  iptr city2;
  int penalty;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  government = civs[civ].government;
  city_calc_food_box_rows(city,civ);
  (*(uint *)&city_calc.shield_rows) = (uint)cosmic.shield_box_rows;
  if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
    (*(uint *)&city_calc.shield_rows) = 0xd - game.difficulty;
    if (game.difficulty == DIFF_CHIEFTAIN) {
      (*(uint *)&city_calc.shield_rows) = 0xe;
    }
    if (game.difficulty < DIFF_KING) {
      (*(uint *)&city_calc.shield_rows)++;
    }
    if (((199 < game.turn) && (DIFF_WARLORD < game.difficulty)) &&
       (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0)) {
      penalty = 2;
      if (((game.wonder_city[WONDER_MANHATTAN] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_ROCKETRY]) != 0)) &&
         ((penalty = 1, (game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0 ||
          ((game.human_players & game.advance_known_by[ADV_NUCLEAR_FISSION]) == 0)))) {
        penalty = 0;
      }
      result = penalty;
      if (((game.wonder_city[WONDER_APOLLO] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_SPACE_FLIGHT]) != 0)) &&
         (result = penalty - 1, (game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) == 0)) {
        result = penalty - 2;
      }
      penalty = result;
      result = int_clamp(penalty,0,2);
      (*(uint *)&city_calc.shield_rows) -= result;
    }
    if ((cosmic.shield_box_rows != '\n') &&
       ((*(uint *)&city_calc.shield_rows) = (int)(cosmic.shield_box_rows * (*(uint *)&city_calc.shield_rows)) / 10, ((*(uint *)&city_calc.shield_rows) & 1) != 0))
    {
      (*(uint *)&city_calc.shield_rows)++;
    }
  }
  city_calc.shield_support = 0;
  city_calc.support_unit_count = 0;
  city_calc.settlers_supported = 0;
  city_calc.aggressive_units = 0;
  city_calc.home_attack_units = 0;
  unit = 0;
  do {
    if (game.num_units <= unit) {
      return;
    }
    if ((((units[unit].id != 0) && (units[unit].owner == civ)) &&
        ((byte)units[unit].home_city == city)) &&
       (units[unit].flags = units[unit].flags & ~(UNITF_AGGRESSIVE | UNITF_COSTS_SHIELD),
       (char)unit_types[units[unit].type].role < 6)) {
      result = city_count_unit_support(city,unit,(uint)government);
      if (result != 0) {
        units[unit].flags = units[unit].flags | UNITF_COSTS_SHIELD;
      }
      result = city_calc.aggressive_units;
      if (unit_types[units[unit].type].attack != '\0') {
        if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
          if (unit_types[units[unit].type].role != ROLE_AIR_SUPERIORITY) {
            city_calc.aggressive_units++;
          }
          if ((units[unit].x == cities[city].x) || (units[unit].y == cities[city].y))
          {
            city_calc.home_attack_units++;
          }
        }
        else if ((units[unit].x == cities[city].x) &&
                (units[unit].y == cities[city].y)) {
          city_calc.home_attack_units++;
        }
        else {
          check = tile_city_owner((int)units[unit].x,(int)units[unit].y);
          if (check < 0) {
            city_calc.aggressive_units++;
            tile_improv = tile_improvements((int)units[unit].x,(int)units[unit].y);
            if (((byte)tile_improv & TILE_AIRBASE) == 0x40) {
              for (city2 = 0; city2 < game.num_cities; city2++) {
                if (((cities[city2].id != 0) && (cities[city2].owner == civ)) &&
                   (check = map_distance((int)units[unit].x,(int)units[unit].y,
                                               (int)cities[city2].x,(int)cities[city2].y),
                   check < 4)) {
                  city_calc.aggressive_units--;
                  break;
                }
              }
            }
          }
        }
      }
      if (city_calc.aggressive_units != result) {
        units[unit].flags = units[unit].flags | UNITF_AGGRESSIVE;
      }
      if (unit_types[units[unit].type].role == ROLE_SETTLE) {
        city_calc.settlers_supported++;
      }
    }
    unit++;
  } while( true );
}

/* 004e868f auto */
int __cdecl city_tile_yield(iptr city,iptr tile_index,iptr yield_type)

{
  byte terrain_byte;
  iptr civ;
  iptr x;
  int y;
  int result;
  uint tile_improv;
  byte *tile;
  uptr terrain;
  int yield_value;
  byte improv_bits;
  
  civ = (iptr)cities[city].owner;
  x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index]);
  y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
  result = map_in_bounds(x,y);
  if (result == 0) {
    yield_value = 0;
  }
  else {
    if (city_turn.yield_tile_worked != 0) {
      tile_set_radius_civ(x,y,civ,1);
      result = tile_city_or_unit_owner(x,y);
      if (result < 0) {
        tile_set_owner(x,y,civ,1);
      }
    }
    terrain_byte = tile_terrain(x,y);
    terrain = (uptr)terrain_byte;
    result = tile_resource(x,y);
    yield_value = (int)*(char *)(yield_type + (iptr)((u8 *)&terrain_types + 10) + (result * 0xb + terrain) * 0x18);
    if ((game.multiplayer != '\0') && (mp_doubles.production != 0)) {
      yield_value = yield_value << 1;
    }
    tile_improv = tile_improvements(x,y);
    improv_bits = (byte)tile_improv;
    if (terrain == TERRAIN_OCEAN) {
      if ((yield_type == 0) && (result = city_has_improvement(city,IMP_HARBOR), result != 0)) {
        yield_value++;
      }
      if ((yield_type == 1) && (result = city_has_improvement(city,IMP_OFFSHORE_PLATFORM), result != 0)) {
        yield_value++;
      }
      if ((yield_type == 2) && (result = wonder_get_city(WONDER_COLOSSUS), result == city)) {
        yield_value++;
      }
    }
    else if (yield_type == 0) {
      if ((tile_improv & (TILE_CITY | TILE_IRRIGATION)) == 0) {
        city_note_terraform_job(city,civ,0,x,y);
      }
      else {
        yield_value += (char)terrain_types[terrain].irrigate_bonus;
        result = city_has_improvement(city,IMP_SUPERMARKET);
        if (result != 0) {
          if ((tile_improv & (TILE_CITY | TILE_MINE)) == 0) {
            city_note_terraform_job(city,civ,0,x,y);
          }
          else {
            yield_value += (yield_value >> 1);
          }
        }
      }
    }
    else if (yield_type == 1) {
      if (((improv_bits & 0xc) == 8) ||
         (((tile_improv & TILE_CITY) != 0 && (terrain_types[terrain].irrigate_bonus == '\0')))) {
        yield_value += (char)terrain_types[terrain].mine_bonus;
      }
      else if ((tile_improv & TILE_FARMLAND) == 0) {
        city_note_terraform_job(city,civ,1,x,y);
      }
      if ((terrain == TERRAIN_GRASSLAND) && (result = tile_grassland_has_shield(x,y), result == 0)) {
        yield_value = 0;
      }
      if ((tile_index == 0x14) && (yield_value == 0)) {
        yield_value = 1;
      }
    }
    else if (yield_type == 2) {
      tile = (byte *)tile_at(x,y);
      if ((*tile & 0x80) != 0) {
        yield_value++;
      }
      if ((tile_improv & (TILE_CITY | TILE_ROAD)) == 0) {
        if ((terrain < 3) || (yield_value != 0)) {
          city_note_road_job(city,civ,x,y);
        }
      }
      else if ((terrain < 3) || (yield_value != 0)) {
        yield_value++;
      }
      if ((yield_value != 0) && (result = wonder_get_city(WONDER_COLOSSUS), result == city)) {
        yield_value++;
      }
    }
    if ((yield_type == 1) && (result = wonder_get_city(WONDER_KING_RICHARDS), result == city)) {
      yield_value++;
    }
    if (yield_type == 1) {
      if (((tile_improv & TILE_RAILROAD) == 0) &&
         (((tile_improv & TILE_CITY) == 0 || (result = civ_has_advance(civ,ADV_RAILROAD), result == 0)))) {
        if ((((tile_improv & (TILE_CITY | TILE_RAILROAD)) == 0) && (result = civ_has_advance(civ,ADV_RAILROAD), result != 0)) &&
           (terrain != TERRAIN_OCEAN)) {
          city_note_road_job(city,civ,x,y);
        }
      }
      else {
        yield_value += (yield_value >> 1);
      }
    }
    if ((2 < yield_value) && ((cities[city].flags & CITYF_CELEBRATING) == 0)) {
      if (city_turn.processing != 0) {
        civs[civ].government_score[GOV_DESPOTISM]--;
      }
      if (civs[civ].government < GOV_MONARCHY) {
        yield_value--;
      }
    }
    if ((yield_value != 0) && (yield_type == 2)) {
      if (city_turn.processing != 0) {
        civs[civ].government_score[GOV_REPUBLIC]++;
        civs[civ].government_score[GOV_DEMOCRACY]++;
      }
      if ((byte)((-((cities[city].flags & CITYF_CELEBRATING) == 0) & 3U) + 2) <= civs[civ].government) {
        yield_value++;
      }
    }
    if (((yield_type == 2) && ((tile_improv & (TILE_CITY | TILE_ROAD)) != 0)) &&
       (result = city_has_improvement(city,IMP_SUPERHIGHWAYS), result != 0)) {
      yield_value += yield_value / 2;
    }
    if ((tile_improv & TILE_POLLUTION) != 0) {
      yield_value = yield_value + 1 >> 1;
      city_calc.tile_job_priority++;
      city_turn.tile_job_found = 1;
      city_calc.tile_job_x = x;
      city_calc.tile_job_y = y;
    }
    if (yield_value < 1) {
      yield_value = 0;
    }
  }
  return yield_value;
}

/* 004e8c8c auto */
void __cdecl
city_note_terraform_job(iptr city,iptr civ,iptr job_type,i32 x,i32 y)

{
  byte terrain_byte;
  int has_source;
  uint terrain;
  uint govt_level;
  
  if (((city_turn.yield_tile_worked != 0) && (city_turn.processing != 0)) && (city_turn.tile_job_found == 0)) {
    terrain_byte = tile_terrain(x,y);
    terrain = (uint)terrain_byte;
    if (((*(char *)(job_type + (iptr)((u8 *)&terrain_types + 14) + terrain * 0x18) != -1) &&
        (*(char *)(job_type + (iptr)((u8 *)&terrain_types + 20) + terrain * 0x18) != '\0')) &&
       ((job_type != 0 || (has_source = tile_has_irrigation_source(x,y), has_source != 0)))) {
      govt_level = (uint)civs[civ].government;
      if (civs[civ].government < GOV_MONARCHY) {
        govt_level = 1;
      }
      if ((int)*(char *)(job_type + (iptr)((u8 *)&terrain_types + 20) + terrain * 0x18) <= (int)govt_level) {
        city_calc.tile_job_priority++;
        city_turn.tile_job_found = 1;
        city_calc.tile_job_x = x;
        city_calc.tile_job_y = y;
      }
    }
  }
  return;
}

/* 004e8db5 auto */
void __cdecl city_note_road_job(iptr city,iptr civ,iptr x,iptr y)

{
  byte *tile;
  int has_adv;
  
  if ((((city_turn.yield_tile_worked != 0) && (city_turn.processing != 0)) && (city_turn.tile_job_found == 0)) &&
     ((tile = (byte *)tile_at(x,y), (*tile & 0x80) == 0 ||
      (has_adv = civ_has_advance(civ,ADV_BRIDGE_BUILDING), has_adv != 0)))) {
    city_turn.tile_job_found = 1;
    city_calc.tile_job_x = x;
    city_calc.tile_job_y = y;
  }
  return;
}

/* 004e8e4d auto */
void __cdecl city_calc_tile_yields(iptr city,iptr tile_index,int worked)

{
  i32 yield_value;
  iptr yield_type;
  
  city_turn.yield_tile_worked = worked;
  for (yield_type = 0; yield_type < 3; yield_type++) {
    yield_value = city_tile_yield(city,tile_index,yield_type);
    (((i32 *)&city_calc.tile_yields))[yield_type] = yield_value;
    if (city_turn.yield_tile_worked != 0) {
      (((int *)&city_calc.yield_totals))[yield_type] = (((int *)&city_calc.yield_totals))[yield_type] + (((i32 *)&city_calc.tile_yields))[yield_type];
    }
  }
  city_turn.yield_tile_worked = 0;
  return;
}

/* 004e8ecf auto */
uint __cdecl city_release_blocked_tiles(iptr city)

{
  uint status;
  i32 enemy_flags;
  int tile_index;
  
  enemy_flags = 0;
  for (tile_index = 0; tile_index < 0x14; tile_index++) {
    status = city_tile_status_flags(city,tile_index);
    enemy_flags = enemy_flags | status & 0x20;
    if (status != 0) {
      city_set_tile_worked(city,tile_index,0);
    }
  }
  return enemy_flags;
}

/* 004e8f42 auto */
void __cdecl city_assign_workers(iptr city)

{
  byte civ;
  char size;
  byte food_per_citizen;
  int score;
  int shield_score;
  int trade_score;
  iptr tile;
  uint worked_mask;
  int best_score;
  int best_food;
  iptr tile_index;
  uint specialists2;
  iptr best_tile;
  int tile_taken[21];
  int kept;
  
  worked_mask = 0;
  kept = 0;
  city_calc.workers_overflow = 0;
  civ = cities[city].owner;
  (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
  specialists2 = cities[city].worked_tiles >> 0x1a;
  if ((1 << (civ & 0x1f) & (uint)game.human_players) == 0) {
    if ((cities[city].flags & CITYF_DISORDER) == 0) {
      if ((city_turn.worker_realloc != 0) && (specialists2 != 0)) {
        if ((cities[city].flags & CITYF_ON_EDGE) == 0) {
          specialists2--;
        }
        else {
          cities[city].flags = cities[city].flags | CITYF_AI_SPECIALIST_HELD;
        }
      }
    }
    else {
      cities[city].worked_tiles = 0;
    }
  }
  if (specialists2 == 0) {
    cities[city].flags = cities[city].flags & ~CITYF_AI_SPECIALIST_HELD;
  }
  score = city_release_blocked_tiles(city);
  for (tile_index = 0; tile_index < 3; tile_index++) {
    (((int *)&city_calc.yield_totals))[tile_index] = 0;
  }
  for (tile_index = 0; tile_index < 0x14; tile_index++) {
    shield_score = city_tile_status_flags(city,tile_index);
    tile_taken[tile_index] = shield_score;
  }
  tile_taken[20] = 0;
  if (((((1 << (civ & 0x1f) & (uint)game.human_players) != 0) || ((game.turn + city & 3U) != 0)
       ) || ((cities[city].flags & CITYF_DISORDER) != 0)) || (score != 0)) {
    for (tile_index = 0; tile_index < 0x15; tile_index++) {
      score = city_tile_worked(city,tile_index);
      if (score != 0) {
        (*(uint *)&city_calc.unassigned_citizens)--;
      }
    }
    if ((int)(*(uint *)&city_calc.unassigned_citizens) < 0) {
      city_calc.workers_overflow = 1;
      (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
    }
    else {
      (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
      for (tile_index = 0; tile_index < 0x15; tile_index++) {
        if (tile_index == 0) {
          tile = 0x14;
        }
        else {
          tile = tile_index - 1;
        }
        score = city_tile_worked(city,tile);
        if (score != 0) {
          city_calc_tile_yields(city,tile,1);
          tile_taken[tile] = 1;
          worked_mask = worked_mask | 1 << ((byte)tile & 0x1f);
          (*(uint *)&city_calc.unassigned_citizens)--;
          if ((*(uint *)&city_calc.unassigned_citizens) == 0) break;
        }
      }
    }
    if (((*(uint *)&city_calc.unassigned_citizens) == specialists2) || ((*(uint *)&city_calc.unassigned_citizens) == 0)) {
      kept = 1;
    }
  }
  /* the citizens kept on their tiles: nothing more to place */
  if (kept == 0) {
    if ((0 < (int)(*(uint *)&city_calc.unassigned_citizens)) && (tile_taken[20] == 0)) {
      worked_mask = worked_mask | 0x100000;
      city_calc_tile_yields(city,0x14,1);
      tile_taken[20] = 1;
      (*(uint *)&city_calc.unassigned_citizens)--;
    }
    best_tile = 0;
    food_per_citizen = cosmic.food_per_citizen;
    if (cosmic.food_per_citizen < 2) {
      food_per_citizen = 1;
    }
    while (((city_calc.yield_totals[0] + (int)(*(uint *)&city_calc.unassigned_citizens) / (int)(uint)food_per_citizen <
             (int)((int)cities[city].size * (uint)cosmic.food_per_citizen +
                  city_calc.settlers_supported * city_calc.settler_food_cost) || (cities[city].size < 3)) &&
           (((*(uint *)&city_calc.unassigned_citizens) != specialists2 && -1 < (int)((*(uint *)&city_calc.unassigned_citizens) - specialists2) && (-1 < best_tile))))) {
      best_tile = -1;
      best_score = 0;
      best_food = 0;
      for (tile_index = 0; tile_index < 0x14; tile_index++) {
        if (tile_taken[tile_index] == 0) {
          score = city_tile_yield(city,tile_index,0);
          shield_score = city_tile_yield(city,tile_index,1);
          if ((((*(uint *)&city_calc.unassigned_citizens) != 1) || (city_calc.total_shields != 0)) || (shield_score * 2 != 0)) {
            trade_score = city_tile_yield(city,tile_index,2);
            trade_score = shield_score * 2 + trade_score;
            if ((best_food < score) || ((score == best_food && (best_score < trade_score)))) {
              best_tile = tile_index;
              best_score = trade_score;
              best_food = score;
            }
          }
        }
      }
      if (-1 < best_tile) {
        tile_taken[best_tile] = 1;
        city_calc_tile_yields(city,best_tile,1);
        worked_mask = worked_mask | 1 << ((byte)best_tile & 0x1f);
        (*(uint *)&city_calc.unassigned_citizens)--;
      }
    }
    if ((((1 << (civ & 0x1f) & (uint)game.human_players) == 0) && ((cities[city].flags & CITYF_DISORDER) != 0)
        ) && (city_turn.worker_realloc != 0)) {
      cities[city].flags = cities[city].flags | CITYF_AI_SPECIALIST_HELD;
      specialists2++;
    }
    for (; (*(uint *)&city_calc.unassigned_citizens) != specialists2 && -1 < (int)((*(uint *)&city_calc.unassigned_citizens) - specialists2);
        (*(uint *)&city_calc.unassigned_citizens) = (*(uint *)&city_calc.unassigned_citizens) - 1) {
      best_tile = -1;
      best_score = 0;
      for (tile_index = 0; tile_index < 0x14; tile_index++) {
        if (tile_taken[tile_index] == 0) {
          city_calc_tile_yields(city,tile_index,0);
          score = int_clamp((city_calc.yield_totals[0] -
                                     (int)cities[city].size * (uint)cosmic.food_per_citizen) -
                                     city_calc.settlers_supported * city_calc.settler_food_cost,1,99);
          score = (int)(0x10 / (longlong)score) * city_calc.tile_yields[0];
          size = cities[city].size;
          shield_score = int_clamp(city_calc.total_shields - city_calc.shield_support,1,99);
          shield_score = ((size * 3) / shield_score) * city_calc.tile_shields;
          size = cities[city].size;
          trade_score = int_clamp(city_calc.total_trade,1,99);
          score = score + shield_score + ((size * 2) / trade_score) * city_calc.tile_trade;
          if (best_score < score) {
            best_tile = tile_index;
            best_score = score;
          }
        }
      }
      if (best_tile < 0) break;
      tile_taken[best_tile] = 1;
      city_calc_tile_yields(city,best_tile,1);
      worked_mask = worked_mask | 1 << ((byte)best_tile & 0x1f);
    }
  }
  cities[city].worked_tiles = (*(uint *)&city_calc.unassigned_citizens) * 0x4000000 + worked_mask;
  if (city_calc.yield_totals[0] <
      (int)((int)cities[city].size * (uint)cosmic.food_per_citizen + city_calc.settlers_supported * city_calc.settler_food_cost)
     ) {
    if (cities[city].food_box == 0) {
      cities[city].food_box--;
    }
  }
  else if (cities[city].food_box < 0) {
    cities[city].food_box = 0;
  }
  return;
}

/* 004e9719 auto */
void __cdecl city_adjust_specialists(iptr city,int delta)

{
  if (delta < 0) {
    for (; delta != 0; delta++) {
      if (0x3ffffff < cities[city].worked_tiles) {
        cities[city].worked_tiles += 0xfc000000;
      }
    }
  }
  else {
    for (; delta != 0; delta--) {
      cities[city].worked_tiles += 0x4000000;
    }
  }
  return;
}

/* 004e97ae auto */
void __cdecl city_fill_specialists(iptr city)

{
  int type;
  int slot;
  
  city_calc.specialist_count = city_calc.unassigned_citizens;
  for (slot = 0; slot < city_calc.specialist_count; slot++) {
    type = city_specialist_type(city,slot);
    if (type == 0) {
      city_set_specialist_type(city,slot,1);
    }
  }
  for (slot = city_calc.specialist_count; slot < 0x10; slot++) {
    city_set_specialist_type(city,slot,0);
  }
  return;
}

/* 004e9849 auto */
int __cdecl city_corruption_divisor(iptr city,int government)

{
  i32 divisor;
  
  divisor = 4;
  if (GOV_ANARCHY < government) {
    divisor = 5;
  }
  if (GOV_DESPOTISM < government) {
    divisor++;
  }
  if (GOV_MONARCHY < government) {
    divisor++;
  }
  if (GOV_FUNDAMENTALISM < government) {
    divisor++;
  }
  return divisor;
}

/* 004e989a auto */
int __cdecl city_calc_corruption(iptr city,int amount,int clamp_distance,int update_scores)

{
  byte civ2;
  iptr civ;
  int divisor;
  uint palace_dist;
  bool is_communism;
  int govt;
  int communism_corruption;
  uint govt_level;
  int adj_distance;
  int distance;
  int base_corruption;
  int corruption;
  
  if (amount < 1) {
    corruption = 0;
  }
  else {
    civ2 = cities[city].owner;
    civ = (iptr)(char)civ2;
    govt_level = (uint)civs[civ].government;
    is_communism = govt_level == 3;
    if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
      govt_level++;
    }
    if (((city_turn.processing != 0) && (update_scores != 0)) || (!is_communism)) {
      distance = city_calc.capital_distance;
      if ((clamp_distance != 0) && (0x10 < city_calc.capital_distance)) {
        distance = 0x10;
      }
      adj_distance = distance;
      if (((govt_level < 2) && (distance != 0)) &&
         ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
        adj_distance = int_clamp((uint)game.difficulty + distance,0,0x20);
      }
      divisor = city_corruption_divisor(city,govt_level);
      base_corruption = (adj_distance * amount * 3) / (divisor * 0x14);
    }
    if (((city_turn.processing != 0) && (update_scores != 0)) || (is_communism)) {
      palace_dist = (uint)cosmic.communism_palace_dist;
      divisor = city_corruption_divisor(city,govt_level);
      communism_corruption = (int)(palace_dist * amount * 3) / (divisor * 0x14);
    }
    if (is_communism) {
      corruption = communism_corruption;
    }
    else {
      corruption = base_corruption;
    }
    corruption = int_clamp(corruption,0,amount);
    divisor = city_has_improvement(city,IMP_COURTHOUSE);
    if ((divisor != 0) || (divisor = city_has_improvement(city,IMP_PALACE), divisor != 0)) {
      corruption = corruption >> 1;
    }
    if ((city_turn.processing != 0) && (update_scores != 0)) {
      for (govt = 1; govt < GOV_DEMOCRACY; govt++) {
        govt_level = govt;
        if (govt == GOV_COMMUNISM) {
          civs[civ].government_score[GOV_COMMUNISM] -= (short)communism_corruption;
        }
        else if (govt != GOV_FUNDAMENTALISM) {
          if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
            govt_level = govt + 1;
          }
          adj_distance = distance;
          if ((((int)govt_level < 2) && (distance != 0)) &&
             ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
            adj_distance = int_clamp((uint)game.difficulty + distance,0,0x20);
          }
          divisor = city_corruption_divisor(city,govt_level);
          civs[civ].government_score[govt] =
               civs[civ].government_score[govt] -
               (short)((adj_distance * amount * 3) / (divisor * 0x14));
        }
      }
    }
  }
  return corruption;
}

/* 004e9c14 auto */
void __cdecl city_calc_shields_pollution(iptr city)

{
  iptr civ;
  int result;
  uint power_bonus;
  uint factory_bonus;
  
  factory_bonus = 0;
  power_bonus = 0;
  civ = (iptr)cities[city].owner;
  city_calc.pollution_divisor = 1;
  result = city_has_improvement(city,IMP_FACTORY);
  if (result != 0) {
    factory_bonus = 2;
  }
  result = city_has_improvement(city,IMP_MFG_PLANT);
  if (result != 0) {
    factory_bonus += 2;
  }
  result = city_has_improvement(city,IMP_POWER_PLANT);
  if (result != 0) {
    power_bonus = 2;
  }
  result = city_has_improvement(city,IMP_HYDRO_PLANT);
  if ((result != 0) || (result = city_has_improvement(city,IMP_NUCLEAR_PLANT), result != 0) ||
     (result = civ_has_wonder((int)cities[city].owner,WONDER_HOOVER_DAM), result != 0)) {
    power_bonus = 2;
    city_calc.pollution_divisor = 2;
  }
  result = city_has_improvement(city,IMP_RECYCLING_CENTER);
  if (result != 0) {
    city_calc.pollution_divisor = 3;
  }
  result = city_has_improvement(city,IMP_SOLAR_PLANT);
  if (result != 0) {
    power_bonus = 2;
    city_calc.pollution_divisor = 3;
  }
  if (factory_bonus <= power_bonus) {
    power_bonus = factory_bonus;
  }
  city_calc.total_shields = city_calc.total_shields +
                 ((int)(city_calc.total_shields * factory_bonus) >> 2) + ((int)(city_calc.total_shields * power_bonus) >> 2);
  city_calc.pop_pollution_factor = 0;
  result = city_has_improvement(city,IMP_MASS_TRANSIT);
  if (result == 0) {
    result = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_AUTOMOBILE);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_MASS_PRODUCTION);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_PLASTICS);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = civ_has_advance(civ,ADV_SANITATION);
      if (result == 0) {
        city_calc.pop_pollution_factor++;
      }
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = civ_has_advance(civ,ADV_ENVIRONMENTALISM);
      if (result != 0) {
        city_calc.pop_pollution_factor--;
      }
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = city_has_improvement(city,IMP_SOLAR_PLANT);
      if (result != 0) {
        city_calc.pop_pollution_factor--;
      }
    }
  }
  city_calc.pollution = city_calc.total_shields / city_calc.pollution_divisor - 0x14;
  result = city_has_improvement(city,IMP_SOLAR_PLANT);
  if (result != 0) {
    city_calc.pollution = 0;
  }
  city_calc.pollution += (cities[city].size * city_calc.pop_pollution_factor >> 2);
  result = city_calc_corruption(city,city_calc.total_shields - city_calc.shield_support,1,1);
  result = (((3 - (*(int *)&city_calc.capital_link)) * result) / 3) / (((int)(uint)civs[civ].government >> 1) + 1);
  city_calc.shield_waste = city_calc.total_shields - (city_calc.shield_support + 1);
  if (result <= city_calc.shield_waste) {
    city_calc.shield_waste = result;
  }
  if (city_calc.shield_waste < 1) {
    city_calc.shield_waste = 0;
  }
  if ((((civ == 0) || (civs[civ].government == GOV_DEMOCRACY)) || (civs[civ].government == GOV_FUNDAMENTALISM))
     || ((cities[city].building == -IMP_PALACE && (city_calc.capital_nearest < 0)))) {
    city_calc.shield_waste = 0;
  }
  city_calc.total_shields -= city_calc.shield_waste;
  return;
}

/* 004ea031 auto */
void __cdecl city_happy_clamp_stage(iptr city,iptr stage)

{
  int working;
  
  (*(i8 *)&city_calc.happy_count) = int_clamp((*(i8 *)&city_calc.happy_count),0,(int)cities[city].size);
  for (; ((*(i8 *)&city_calc.angry_count) != 0 && ((*(i8 *)&city_calc.unhappy_count) < (*(i8 *)&city_calc.angry_count))); (*(i8 *)&city_calc.unhappy_count)++) {
    (*(i8 *)&city_calc.angry_count)--;
  }
  (*(i8 *)&city_calc.unhappy_count) = int_clamp((*(i8 *)&city_calc.unhappy_count),0,(int)cities[city].size);
  while( true ) {
    working = int_clamp(cities[city].size - city_calc.unassigned_citizens,0,99);
    if ((*(i8 *)&city_calc.unhappy_count) + (*(i8 *)&city_calc.happy_count) <= working) break;
    if ((*(i8 *)&city_calc.angry_count) == 0) {
      (*(i8 *)&city_calc.happy_count)--;
      (*(i8 *)&city_calc.happy_count) = int_clamp((*(i8 *)&city_calc.happy_count),0,(int)cities[city].size);
    }
    else {
      (*(i8 *)&city_calc.angry_count)--;
    }
    (*(i8 *)&city_calc.unhappy_count)--;
    (*(i8 *)&city_calc.unhappy_count) = int_clamp((*(i8 *)&city_calc.unhappy_count),0,(int)cities[city].size);
  }
  for (; ((*(i8 *)&city_calc.angry_count) != 0 && ((*(i8 *)&city_calc.unhappy_count) + (*(i8 *)&city_calc.happy_count) < cities[city].size - city_calc.unassigned_citizens));
      (*(i8 *)&city_calc.unhappy_count) = (*(i8 *)&city_calc.unhappy_count) + 1) {
    (*(i8 *)&city_calc.angry_count)--;
  }
  if (-1 < stage) {
    (((i8 *)&city_calc.happy_by_stage))[stage] = *(i8 *)&city_calc.happy_count;
    (((i8 *)&city_calc.unhappy_by_stage))[stage] = *(i8 *)&city_calc.unhappy_count;
    (((i8 *)&city_calc.angry_by_stage))[stage] = *(i8 *)&city_calc.angry_count;
  }
  return;
}

/* 004ea1f6 auto */
void __cdecl city_split_trade(iptr city,int trade,int mode,int extra_tax)

{
  byte civ2;
  iptr civ;
  int result;
  int result2;
  uint science_rate2;
  int science_bonus;
  int orig_trade;
  uint bonus_level;
  
  if ((mode == 1) && (cities[city].building == -IMP_CAPITALIZATION)) {
    orig_trade = trade;
    trade = (int)cities[city].trade;
  }
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  science_rate2 = (uint)civs[civ].science_rate;
  if ((civs[civ].government == GOV_FUNDAMENTALISM) && (cosmic.fund_max_science < science_rate2)) {
    science_rate2 = (uint)cosmic.fund_max_science;
  }
  city_calc.lux_total = int_clamp((int)(trade * (10 - ((uint)civs[civ].science_rate +
                                                          (uint)civs[civ].tax_rate)) + 4) / 10,0,
                                    trade);
  city_calc.science_total = int_clamp((int)(trade * science_rate2 + 4) / 10,0,trade - city_calc.lux_total);
  city_calc.tax_total = trade - (city_calc.science_total + city_calc.lux_total);
  if (city_turn.processing != 0) {
    civs[civ].government_score[GOV_FUNDAMENTALISM] += (short)(city_calc.lux_total / 2);
    result = (leaders[civs[civ].tribe].civilize + 3) * city_calc.science_total;
    civs[civ].government_score[GOV_FUNDAMENTALISM] =
         civs[civ].government_score[GOV_FUNDAMENTALISM] - (short)((int)(result + (result >> 0x1f & 3U)) >> 2);
  }
  if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (civs[civ].government == GOV_FUNDAMENTALISM))
  {
    city_calc.science_total += city_calc.lux_total;
    city_calc.lux_total = 0;
  }
  result = city_count_specialists(city,1);
  city_calc.lux_total += result * 2;
  result = city_count_specialists(city,2);
  city_calc.tax_total += result * 3;
  result = city_count_specialists(city,3);
  city_calc.science_total += result * 3;
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.science_total -= (int)((uint)cosmic.fund_science_loss * city_calc.science_total) / 100;
  }
  city_calc.building_content = 0;
  result = city_has_improvement(city,IMP_TEMPLE);
  if (result != 0) {
    result = civ_has_advance(civ,ADV_MYSTICISM);
    if (result != 0) {
      city_calc.building_content++;
    }
    result = civ_has_advance(civ,ADV_CEREMONIAL_BURIAL);
    if (result != 0) {
      city_calc.building_content++;
    }
    result = civ_has_wonder(civ,WONDER_ORACLE);
    if (result != 0) {
      city_calc.building_content = city_calc.building_content << 1;
    }
  }
  result = city_has_improvement(city,IMP_COLOSSEUM);
  if (result != 0) {
    city_calc.building_content += 3;
    result = civ_has_advance(civ,ADV_ELECTRONICS);
    if (result != 0) {
      city_calc.building_content++;
    }
  }
  result = civ_has_advance(civ,ADV_MONOTHEISM);
  if ((result != 0) &&
     ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
      (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
    result = civ_has_advance(civ,ADV_COMMUNISM);
    result2 = civ_has_advance(civ,ADV_THEOLOGY);
    city_calc.building_content = city_calc.building_content + (uint)(result == 0) + (3 - (uint)(result2 == 0));
  }
  result = civ_has_wonder(civ,WONDER_BACH);
  if (result != 0) {
    city_calc.building_content += 2;
  }
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.tax_total += city_calc.building_content;
  }
  result = city_has_improvement(city,IMP_MARKETPLACE);
  bonus_level = (uint)(result != 0);
  result = city_has_improvement(city,IMP_BANK);
  if (result != 0) {
    bonus_level++;
  }
  result = city_has_improvement(city,IMP_STOCK_EXCHANGE);
  if (result != 0) {
    bonus_level++;
  }
  city_calc.lux_total += ((int)(city_calc.lux_total * bonus_level) >> 1);
  city_calc.tax_total += ((int)(city_calc.tax_total * bonus_level) >> 1);
  if (city_turn.processing != 0) {
    civs[civ].government_score[GOV_FUNDAMENTALISM] =
         civs[civ].government_score[GOV_FUNDAMENTALISM] + (short)((int)((bonus_level + 2) * city_calc.building_content) >> 1);
  }
  result = city_has_improvement(city,IMP_LIBRARY);
  bonus_level = (uint)(result != 0);
  result = city_has_improvement(city,IMP_UNIVERSITY);
  if (result != 0) {
    bonus_level++;
  }
  result = city_has_improvement(city,IMP_RESEARCH_LAB);
  if ((result != 0) || (result = civ_has_wonder(civ,WONDER_SETI), result != 0)) {
    bonus_level++;
  }
  science_bonus = city_calc.science_total * bonus_level;
  result = wonder_get_city(WONDER_NEWTON);
  if (result != city) {
    science_bonus = science_bonus >> 1;
  }
  city_calc.science_total += science_bonus;
  result = wonder_get_city(WONDER_COPERNICUS);
  if (result == city) {
    city_calc.science_total = city_calc.science_total << 1;
  }
  if (cities[city].building == -IMP_CAPITALIZATION) {
    if (mode == 0) {
      if (city_calc.shield_support < city_calc.total_shields) {
        city_calc.tax_total += (city_calc.total_shields - city_calc.shield_support);
      }
    }
    else {
      if (mode == 1) {
        city_split_trade(city,orig_trade,2,cities[city].tax - city_calc.tax_total);
        return;
      }
      if (mode == 2) {
        city_calc.tax_total += extra_tax;
      }
    }
  }
  cities[city].science = (i16)city_calc.science_total;
  cities[city].tax = (i16)city_calc.tax_total;
  cities[city].trade = (i16)trade;
  return;
}

/* 004ea8e4 auto */
int __cdecl city_calc_trade_happiness(uptr city)

{
  byte civ2;
  char goods;
  iptr civ;
  iptr city2;
  int net_trade;
  int result;
  int result2;
  uptr wonder_city;
  iptr unit;
  int content_limit;
  uint factor;
  int route_trade;
  int road_bonus;
  int unhappy_mult;
  iptr route;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  city_calc.trade_corruption = city_calc_corruption(city,city_calc.total_trade,0,0);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.trade_corruption = 0;
  }
  if (civs[civ].government == GOV_DEMOCRACY) {
    city_calc.trade_corruption = 0;
  }
  cities[city].base_trade = (short)city_calc.total_trade - (short)city_calc.trade_corruption;
  city_calc_commodity_demand(city);
  for (route = 0; route < (char)cities[city].trade_routes; route++) {
    city2 = (iptr)cities[city].route_partner[route];
    route_trade = (int)cities[city].base_trade + (int)cities[city2].base_trade + 4 >> 3;
    goods = *(char *)&cities[city].route_goods[route];
    road_bonus = map_road_connection((int)cities[city].owner,(int)cities[city].x,
                                  (int)cities[city].y,(int)cities[city2].x,(int)cities[city2].y);
    net_trade = city_has_improvement(city,IMP_AIRPORT);
    if (((net_trade != 0) && (net_trade = city_has_improvement(city2,IMP_AIRPORT), net_trade != 0)) && (road_bonus < 2)) {
      road_bonus = 1;
    }
    net_trade = city_has_improvement(city,IMP_SUPERHIGHWAYS);
    if (net_trade != 0) {
      road_bonus++;
    }
    if (road_bonus != 0) {
      route_trade += (road_bonus * route_trade >> 1);
    }
    if (goods < '\0') {
      route_trade = 0;
    }
    if (cities[city2].owner == civ) {
      route_trade = route_trade >> 1;
    }
    city_calc.route_trade_values[route] = route_trade;
    city_calc.total_trade += route_trade;
  }
  city_calc.trade_corruption = city_calc_corruption(city,city_calc.total_trade,0,1);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.trade_corruption = 0;
  }
  if (civs[civ].government == GOV_DEMOCRACY) {
    city_calc.trade_corruption = 0;
  }
  net_trade = city_calc.total_trade - city_calc.trade_corruption;
  city_split_trade(city,net_trade,0,0);
  (*(i8 *)&city_calc.happy_count) = 0;
  if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
    (*(i8 *)&city_calc.unhappy_count) = (cities[city].size - 1) - (cosmic.unhappy_size - 5);
  }
  else {
    factor = (uint)cosmic.riot_factor + (uint)game.difficulty * -2;
    if ((game.game_flags & GAME_LARGE_MAP) != 0) {
      factor += 2;
    }
    result = (int)((((int)(uint)civs[civ].government >> 1) + 2) * factor) / 2;
    if (result < 2) {
      result = 1;
    }
    content_limit = (uint)cosmic.unhappy_size - (uint)game.difficulty;
    if (((content_limit < 3) && (result2 = city_has_improvement(city,IMP_PALACE), result2 != 0)) &&
       ((city_calc.aggressive_units == 0 && (city_calc.home_attack_units == 0)))) {
      content_limit = 2;
    }
    (*(i8 *)&city_calc.unhappy_count) = (cities[city].size - 1) - (content_limit - 2);
    if (civs[civ].government != GOV_COMMUNISM) {
      (*(i8 *)&city_calc.unhappy_count) = (*(i8 *)&city_calc.unhappy_count) +
                     ((civs[civ].num_cities - result) + (int)city % result) / result;
    }
  }
  city_calc.angry_count = 0;
  if (cities[city].size < (*(i8 *)&city_calc.unhappy_count)) {
    city_calc.angry_count = (*(i8 *)&city_calc.unhappy_count) - cities[city].size;
    (*(i8 *)&city_calc.unhappy_count) = (int)cities[city].size;
  }
  city_happy_clamp_stage(city,0);
  (*(i8 *)&city_calc.happy_count) = city_calc.lux_total >> 1;
  city_happy_clamp_stage(city,1);
  result = city_has_improvement(city,IMP_COLOSSEUM);
  if (result != 0) {
    (*(i8 *)&city_calc.unhappy_count) -= 3;
    result = civ_has_advance(civ,ADV_ELECTRONICS);
    if (result != 0) {
      (*(i8 *)&city_calc.unhappy_count)--;
    }
  }
  result = civ_has_advance(civ,ADV_MONOTHEISM);
  if ((result != 0) &&
     ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
      (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
    result = civ_has_advance(civ,ADV_COMMUNISM);
    result2 = civ_has_advance(civ,ADV_THEOLOGY);
    (*(i8 *)&city_calc.unhappy_count) -= ((uint)(result == 0) + (3 - (uint)(result2 == 0)));
  }
  result = city_has_improvement(city,IMP_TEMPLE);
  if (result != 0) {
    result = civ_has_advance(civ,ADV_MYSTICISM);
    factor = (uint)(result != 0);
    result = civ_has_advance(civ,ADV_CEREMONIAL_BURIAL);
    if (result != 0) {
      factor++;
    }
    result = civ_has_wonder(civ,WONDER_ORACLE);
    if (result != 0) {
      factor = factor << 1;
    }
    (*(i8 *)&city_calc.unhappy_count) -= factor;
  }
  result = city_has_improvement(city,IMP_COURTHOUSE);
  if (((result != 0) || (result = city_has_improvement(city,IMP_PALACE), result != 0)) &&
     (civs[civ].government == GOV_DEMOCRACY)) {
    (*(i8 *)&city_calc.happy_count)++;
  }
  city_happy_clamp_stage(city,2);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.angry_count = 0;
    (*(i8 *)&city_calc.unhappy_count) = 0;
  }
  else if (civs[civ].government < GOV_REPUBLIC) {
    city_calc.martial_law_units = 0;
    for (unit = tile_first_unit((int)cities[city].x,(int)cities[city].y);
        result = city_calc.martial_law_units, -1 < unit; unit = unit_next_in_stack(unit)) {
      if ((unit_types[units[unit].type].attack != '\0') &&
         (city_calc.martial_law_units = city_calc.martial_law_units + 1, civs[civ].government == GOV_COMMUNISM)) {
        city_calc.martial_law_units = result + 2;
      }
    }
    factor = 3;
    if (civs[civ].government == GOV_COMMUNISM) {
      factor = 6;
    }
    if ((int)factor <= (int)city_calc.martial_law_units) {
      city_calc.martial_law_units = factor;
    }
    result = int_clamp(city_calc.martial_law_units,0,(*(i8 *)&city_calc.unhappy_count));
    (*(i8 *)&city_calc.unhappy_count) -= result;
  }
  else {
    result = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((result == 0) && (result = city_has_improvement(city,IMP_POLICE_STATION), result == 0)) {
      unhappy_mult = 1;
    }
    else {
      unhappy_mult = 0;
    }
    if (civs[civ].government == GOV_DEMOCRACY) {
      unhappy_mult++;
    }
    if (unhappy_mult != 0) {
      factor = city_calc.aggressive_units;
      if ((city_calc.aggressive_units != 0) && (civs[civ].government == GOV_REPUBLIC)) {
        factor = city_calc.aggressive_units - 1;
      }
      (*(i8 *)&city_calc.unhappy_count) += unhappy_mult * factor;
    }
  }
  city_happy_clamp_stage(city,3);
  result = civ_has_wonder(civ,WONDER_HANGING_GARDENS);
  if (result != 0) {
    (*(i8 *)&city_calc.happy_count)++;
    wonder_city = wonder_get_city(WONDER_HANGING_GARDENS);
    if (wonder_city == city) {
      (*(i8 *)&city_calc.happy_count) += 2;
    }
  }
  result = civ_has_wonder(civ,WONDER_CURE_FOR_CANCER);
  if (result != 0) {
    (*(i8 *)&city_calc.happy_count)++;
  }
  wonder_city = wonder_get_city(WONDER_SHAKESPEARE);
  if (wonder_city == city) {
    (*(i8 *)&city_calc.unhappy_count) = 0;
  }
  result = civ_has_wonder(civ,WONDER_BACH);
  if (result != 0) {
    (*(i8 *)&city_calc.unhappy_count) -= 2;
  }
  city_happy_clamp_stage(city,4);
  cities[city].science = (i16)city_calc.science_total;
  cities[city].tax = (i16)city_calc.tax_total;
  cities[city].trade = (i16)net_trade;
  cities[city].total_food = (u8)(*(i8 *)&city_calc.yield_totals);
  cities[city].total_shields = (u8)(*(i8 *)&city_calc.total_shields);
  cities[city].happy = (u8)(*(i8 *)&city_calc.happy_count);
  cities[city].unhappy = (u8)(*(i8 *)&city_calc.unhappy_count);
  return (*(i8 *)&city_calc.happy_count) - (*(i8 *)&city_calc.unhappy_count);
}

/* 004eb327 auto */
void __cdecl city_count_food_routes(iptr city)

{
  iptr city2;
  iptr route;
  
  city_calc.food_routes_out = 0;
  city_calc.food_routes_in = 0;
  city_calc.food_route_partner = -1;
  for (route = 0; route < (char)cities[city].trade_routes; route++) {
    if (*(char *)&cities[city].route_goods[route] < '\0') {
      city_calc.food_routes_out++;
      city_calc.food_route_partner = (int)cities[city].route_partner[route];
    }
  }
  city_calc.yield_totals[0] -= city_calc.food_routes_out;
  for (city2 = 0; city2 < game.num_cities; city2++) {
    if (cities[city2].id != 0) {
      for (route = 0; route < (char)cities[city2].trade_routes; route++) {
        if ((cities[city2].route_partner[route] == city) &&
           (*(char *)&cities[city2].route_goods[route] < '\0')) {
          city_calc.yield_totals[0]++;
          city_calc.food_routes_in++;
        }
      }
    }
  }
  return;
}

/* 004eb4a1 auto */
void __cdecl city_calc_all_stats(iptr city)

{
  city_assign_workers(city);
  city_count_food_routes(city);
  city_fill_specialists(city);
  city_calc_shields_pollution(city);
  city_calc_trade_happiness(city);
  return;
}

/* 004eb4ed: written by hand in src/game/retvals.cpp */

/* 004eb571 auto */
int __cdecl city_notify_dialog_ex(iptr msg_key,iptr city,int force,iptr sprite)

{
  int result;
  iptr label;
  iptr option_id;
  i32 option_flags;
  Dialog dialog2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (autoplay.active == 0) {
    if ((city_calc.notify_enabled == 0) && (force == 0)) {
      result = 0;
    }
    else {
      text_set_string_arg_b(0,(const char *)&cities[city].name);
      text_clear();
      text_append((const char *)msg_key);
      result = _strcmp((char *)msg_key,str_built);
      if ((result == 0) && ((cities[city].flags & CITYF_AUTOBUILD) != 0)) {
        text_append((const char *)&str_3);
      }
      dialog2.load_game_text((const char *)&textbuf.buf);
      mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,(int)cities[city].owner);
      if (city_calc.zoom_requested == 0) {
        option_flags = 0;
        option_id = 1;
        label = (iptr)label_string(label_table.string_ids[0x20]);
        dialog2.add_option((const char *)label,option_id,option_flags);
        option_flags = 0;
        option_id = 0;
        label = (iptr)label_string(label_table.string_ids[0x21]);
        dialog2.add_option((const char *)label,option_id,option_flags);
        if ((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0) {
          dialog2.set_default_choice(0);
        }
        else {
          dialog2.set_default_choice(1);
        }
      }
      if (sprite != 0) {
        dialog2.add_sprite(sprite,0,0);
        dialog2.set_image_zoom(8);
      }
      result = dialog2.show_modal(0);
      if (0 < result) {
        city_calc.zoom_requested = 1;
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004ebbde auto */
i32 __cdecl city_process_food(uptr city)

{
  char *goods_ptr;
  byte civ2;
  iptr civ;
  iptr box_limit;
  iptr improvement;
  iptr has_granary;
  int route_count;
  iptr unit2;
  iptr unit;
  int y2;
  int x2;
  /* "FAMINE0": the digit (msg_key[6]) picks the message; one array, as the
     original's two locals that the copy fills together */
  i8 msg_key[8];
  int route;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  city_calc_food_box_rows(city,civ);
  if (cities[city].food_box < 0) {
    unit2 = -1;
    for (unit = 0; unit < game.num_units; unit++) {
      if (((units[unit].id != 0) && (unit_types[units[unit].type].role == ROLE_SETTLE)) &&
         ((byte)units[unit].home_city == city)) {
        unit2 = unit;
        break;
      }
    }
    crt_strcpy(msg_key,str_famine0);
    /* B30: the famine's square, redrawn at the end; the original sets it only
       when the city shrinks (every mode: ours would redraw another stale square;
       docs/BUGS.md) */
    x2 = (int)cities[city].x;
    y2 = (int)cities[city].y;
    if (unit2 < 0) {
      msg_key[6] = 0x31;
    }
    else {
      text_set_label_arg(1,unit_types[units[unit2].type].name);
    }
    if (-1 < city_calc.food_route_partner) {
      msg_key[6] = 0x32;
      text_set_string_arg(1,(const char *)&cities[city_calc.food_route_partner].name);
    }
    if (autoplay.active == 0) {
      city_notify_dialog_ex((iptr)msg_key,city,0,0);
    }
    if (city_calc.food_routes_out < 1) {
      if (unit2 < 0) {
        cities[city].size--;
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        if (cities[city].size < 1) {
          city_delete(city,0);
          civ_check_destroyed(civ,0);
          return 1;
        }
      }
      else if (((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) ||
              ((game.human_players & cities[city].known_to) != 0)) {
        unit_delete_with_cargo(unit2,1);
      }
    }
    else {
      route_count = (int)(char)cities[city].trade_routes;
      while (route = route_count - 1, -1 < route) {
        goods_ptr = (char *)&cities[city].route_goods[route];
        route_count = route;
        if ((*goods_ptr < '\0') &&
           (cities[city].route_partner[route] == city_calc.food_route_partner)) {
          city_remove_trade_route(city,route);
          route_count = route;
        }
      }
    }
    cities[city].food_box = 0;
    map_redraw_area_all_views(x2,y2,0,human_civ[0],1);
    if (city_turn.window_shown != 0) {
      city_turn.window_dirty = 1;
    }
  }
  else {
    box_limit = (cities[city].size + 1) * city_calc.food_box_rows;
    if (bugfix_state() && (0x7e < cities[city].size)) {
      /* B16: a city stops at 127, the most its size (a signed byte) holds;
         the original grows on to -128 (docs/BUGS.md) */
      if (box_limit < cities[city].food_box) {
        cities[city].food_box = (short)box_limit;
      }
    }
    else if (box_limit - cities[city].food_box == 0 || box_limit < cities[city].food_box) {
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_food_box(1);
      }
      improvement = city_needed_size_improvement(city);
      cities[city].size++;
      has_granary = city_has_improvement(city,IMP_GRANARY);
      if ((has_granary == 0) && (route_count = civ_has_wonder(civ,WONDER_PYRAMIDS), route_count == 0)) {
        cities[city].food_box = 0;
      }
      else {
        cities[city].food_box =
             (cities[city].size + 1) * (short)((int)(uint)cosmic.food_box_rows >> 1);
      }
      if (improvement != IMP_NOTHING) {
        cities[city].size--;
        text_set_label_arg(1,improvements[improvement].name);
        if (((((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) ||
             (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) == 0)) || ((game.shown_event_flags & HINT_AQUEDUCT) != 0)) || (improvement != IMP_AQUEDUCT)) {
          if (((int)cities[city].building != -improvement) && ((game.message_options & MSGOFF_GROWTH_HALTED) == 0)) {
            city_notify_dialog((iptr)&str_furthergrowth,city,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c));
          }
        }
        else {
          text_set_number_arg_u8(0,(uint)CONCAT11((char)(game.shown_event_flags >> 8),cosmic.aqueduct_size));
          city_notify_dialog((iptr)&str_aqueduct,city,(iptr)(((u8 *)&caption_close_button_sprite_block.notify_sprites + 420)));
          game.shown_event_flags = game.shown_event_flags | HINT_AQUEDUCT;
        }
      }
      if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) &&
         (cities[city].size == 2)) {
        city_set_production(city,99);
      }
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_food_box(1);
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_citizens(1,0);
        city_turn.window_dirty = 1;
      }
    }
    map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
  }
  return 0;
}

/* Part of city_process_production (004ec3fe): the announcement of a completed
   improvement or wonder: sound, message or dialog (human), and the next
   build. */
static void city_process_production_announce(uptr city,uptr civ,iptr wonder,iptr improvement,int acquired)
{
  iptr str;
  i8 *built_sprite;
  i8 *notify_sprite;
  Dialog built_dialog;
  int announce;
  uptr city3;
  iptr sound;

  if ((human_civ[0] == civ) || (0x22 < improvement)) {
    announce = 1;
  }
  else {
    announce = 0;
  }
  if (autoplay.active != 0) {
    announce = 0;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    cities[city].known_to = 0xff;
  }
  text_set_label_arg(1,improvements[improvement].name);
  str = civ_get_adjective(civ);
  text_set_string_arg(2,(const char *)str);
  city3 = 0xbb;
  if (wonder == WONDER_CURE_FOR_CANCER) {
    city3 = 0xb8;
  }
  if ((((wonder == WONDER_MARCO_POLO) || (wonder == WONDER_SUN_TZU)) || (wonder == WONDER_KING_RICHARDS)) || (wonder == WONDER_LEONARDO)) {
    city3 = 0xba;
  }
  if ((wonder == WONDER_DARWIN) || (wonder == WONDER_MAGELLAN)) {
    city3 = 0xba;
  }
  if (wonder == WONDER_WOMENS_SUFFRAGE) {
    city3 = 0xb9;
  }
  text_set_label_arg(3,label_table.string_ids[city3]);
  if (announce != 0) {
    if (wonder < 0) {
      if ((improvement < 0x23) || (0x25 < improvement)) {
        switch(improvement) {
        case IMP_BARRACKS:
          sound = 5;
          break;
        default:
          sound = 2;
          break;
        case IMP_AQUEDUCT:
          sound = 4;
          break;
        case IMP_BANK:
          sound = 0x2f;
          break;
        case IMP_CATHEDRAL:
          sound = 0xb;
          break;
        case IMP_UNIVERSITY:
          sound = 0x37;
          break;
        case IMP_STOCK_EXCHANGE:
          sound = 0x45;
        }
        snd_play_effect(sound,1,0,0);
      }
      else {
        snd_play_effect(SND_BLDSPCSH,1,0,0);
      }
    }
    else {
      snd_play_effect(SND_NEWONDER,1,0,0);
    }
  }
  if ((human_civ[0] == civ) && (autoplay.active == 0)) {
    if ((2 < game.multiplayer) && (0x22 < improvement)) {
      text_set_string_arg(0,(const char *)&cities[city].name);
      net_send_text_args(0x47,0xff,4,0,wonder,improvement);
    }
    if ((game.message_options & MSGOFF_IMPROVEMENT_BUILT) == 0) {
      if (wonder < 0) {
        notify_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
      }
      else {
        notify_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
      }
      city_notify_dialog_ex((iptr)&str_built_4,city,1,(iptr)notify_sprite);
    }
  }
  else if (announce != 0) {
    text_set_string_arg(0,(const char *)&cities[city].name);
    if ((autoplay.active == 0) && (2 < game.multiplayer)) {
      net_send_text_args(0x47,0xff,4,0,wonder,improvement);
    }
    built_dialog.Dialog::construct(0x4000);
    ON_SCOPE_EXIT(built_dialog.Dialog::destruct());
    built_dialog.load_game_text((const char *)&str_built2);
    if (wonder < 0) {
      built_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
    }
    else {
      built_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
    }
    built_dialog.add_sprite((iptr)built_sprite,0,0)
    ;
    built_dialog.set_image_zoom(8);
    built_dialog.show_modal(0);
  }
  if ((((announce != 0) && (-1 < wonder)) && (human_civ[0] == civ)) &&
     ((autoplay.active == 0 && (((*((char *)&game.game_options + 2)) & (OPT_WONDER_MOVIES >> 16)) != 0)))) {
    wonder_show_movie(wonder);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    city_calc.completed_improvement = improvement;
  }
  if (acquired == 0) {
    cities[city].shield_box = 0;
  }
  if (((cities[city].flags & CITYF_AUTOBUILD) != 0) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
    city_set_production(city,99);
  }
}

/* Part of city_process_production (004ec3fe): a completed improvement or
   wonder: build it, announce it, and its effects (spaceship part, palace,
   wonders with an effect when built). */
static void city_process_production_complete_improvement(uptr city,uptr civ,int cost2,iptr wonder,iptr improvement,int acquired)
{
  int result;
  iptr str;
  i32 value;
  uptr city2;
  uptr city3;
  iptr part;
  uptr civ2;

  if (-1 < improvement) {
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
    }
    if (wonder == WONDER_MANHATTAN) {
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        value = int_clamp((civs[civ2].reputation + 1) / 2,0,6);
        civs[civ2].reputation = (u8)value;
      }
      if (autoplay.active == 0) {
        for (civ2 = 1; (int)civ2 < 8; civ2++) {
          if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) &&
              ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
             (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) != 0 ||
              ((int)cities[city].owner == (civ2 & 0xff))))) {
            if (human_civ[0] == civ2) {
              snd_play_effect(SND_LARGEXPL,1,0,0);
              city_notify_dialog((iptr)&str_manhattan,city,(iptr)&caption_close_button_sprite_block.city_notify_sprite);
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x46,net.net_slots[net.net_civ_slots[civ2]].id,1,0,
                                 city,0);
            }
          }
        }
      }
    }
    if (acquired == 0) {
      city_set_improvement(city,improvement,1);
      cities[city].shield_box -= (short)cost2;
    }
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       (improvement < 0x23)) {
      if (improvement == IMP_PALACE) {
        cities[city].building--;
      }
      city_set_production(city,99);
    }
    else {
      city_process_production_announce(city,civ,wonder,improvement,acquired);
    }
    if ((0x22 < improvement) && (improvement < 0x26)) {
      part = improvement - 0x23;
      part = spaceship_add_part(civ,part);
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         ((result = civ_spaceship_launched(civ), result == 0 && (part < 0)))) {
        text_set_label_arg(1,improvements[improvement].name);
        city_notify_dialog((iptr)&str_badspace_2,city,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c));
      }
    }
    if (improvement == IMP_PALACE) {
      for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
        if ((cities[city2].id != 0) && ((int)cities[city2].owner == civ)) {
          city_set_improvement(city2,1,0);
        }
      }
      city_set_improvement(city,1,1);
      civs[civ].capital_x = cities[city].x;
      if (autoplay.active == 0) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        str = civ_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        if ((autoplay.active == 0) && (2 < game.multiplayer)) {
          net_send_text_args(0x48,0xff,2,0,0,0);
        }
        popup_game((const char *)&str_movecapital);
      }
    }
    if (wonder == WONDER_DARWIN) {
      civ_research_complete(civ,0);
      civ_research_complete(civ,0);
    }
    if (wonder == WONDER_LEONARDO) {
      civ_leonardo_upgrade(civ);
    }
    if (wonder == WONDER_EIFFEL_TOWER) {
      civ_apply_eiffel_tower(civ);
    }
    if (wonder == WONDER_APOLLO) {
      wonder_apollo_reveal_map();
    }
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_improvements(1);
    }
    if (-1 < wonder) {
      city3 = city;
      for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
        if ((((cities[city2].id != 0) && (city2 != city3)) &&
            (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) == 0)) &&
           ((int)cities[city2].building == -improvement)) {
          city_set_production(city2,99);
        }
      }
      city_recalc(city3,0);
    }
  }
}

/* Part of city_process_production (004ec3fe): an AI diplomat: created next
   to the foreign city it is likely to reach first. */
static void city_process_production_ai_diplomat(uptr city,uptr civ,iptr &unit,iptr unit_type)
{
  int result;
  int landmass2;
  int roll;
  uptr target_city;
  uptr city2;
  int tie_count;
  iptr unit2;
  int best_dist;

  target_city = 0xffffffff;
  best_dist = 0x7fff;
  tie_count = 0;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if (((cities[city2].id != 0) && ((int)cities[city2].owner != civ)) &&
       ((*(byte *)&civs[civ].treaties[cities[city2].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) {
      unit = unit_find_nearest((int)cities[city2].x,(int)cities[city2].y,civ
                                    ,-1);
      if (dialog_text_font_obj_block.unit_nearest_dist == 3 || dialog_text_font_obj_block.unit_nearest_dist - 3 < 0) {
        dialog_text_font_obj_block.unit_nearest_dist = ~(dialog_text_font_obj_block.unit_nearest_dist - 3U) + 1;
      }
      else {
        dialog_text_font_obj_block.unit_nearest_dist -= 3;
      }
      if (-1 < unit) {
        if (((cities[city2].flags & CITYF_TECH_STOLEN) != 0) && (dialog_text_font_obj_block.unit_nearest_dist < 4)) {
          dialog_text_font_obj_block.unit_nearest_dist = 3;
        }
        if (dialog_text_font_obj_block.unit_nearest_dist <= best_dist) {
          if (best_dist == dialog_text_font_obj_block.unit_nearest_dist) {
            if (tie_count + 1 == 1 || tie_count < 0) {
              roll = 0;
              tie_count++;
            }
            else {
              tie_count += 2;
              roll = crt_rand_at("city_process_production");
              roll = roll % tie_count;
            }
            if (roll != 0) continue;
          }
          result = tile_is_ocean((int)units[unit].x,(int)units[unit].y);
          if (result == 0) {
            result = tile_landmass((int)cities[city2].x,(int)cities[city2].y);
            landmass2 = tile_landmass((int)units[unit].x,(int)units[unit].y);
            if (result == landmass2) {
              if (best_dist < dialog_text_font_obj_block.unit_nearest_dist) {
                tie_count = 1;
                best_dist = dialog_text_font_obj_block.unit_nearest_dist;
              }
              target_city = city2;
              unit2 = unit;
            }
          }
        }
      }
    }
  }
  if ((best_dist < 4) && (result = crt_rand_at("city_process_production"), result % 10 <= (int)(uint)game.difficulty)) {
    city2 = target_city;
    unit = unit2;
    unit = unit_create(unit_type,civ,(int)units[unit2].x,
                                  (int)units[unit2].y);
  }
  else {
    unit = unit_create(unit_type,civ,(int)cities[city].x,
                                  (int)cities[city].y);
  }
  if (civs[civ].government == GOV_COMMUNISM) {
    units[unit].flags = units[unit].flags | UNITF_VETERAN;
  }
}

/* Part of city_process_production (004ec3fe): a human's caravan: the dialog to
   choose its commodity. */
static void city_process_production_caravan_dialog(uptr city,uptr civ,iptr unit)
{
  int result;
  iptr str;
  iptr option_id;
  i32 value;
  iptr option_index;
  char goods_amount;
  Dialog caravan_dialog;
  iptr choice;
  iptr unit_type2;

  caravan_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(caravan_dialog.Dialog::destruct());
  mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,
                     (int)cities[city].owner);
  do {
    text_set_string_arg(0,(const char *)&cities[city].name);
    caravan_dialog.load_game_text((const char *)&str_caravanbuilt);
    for (unit_type2 = 0; unit_type2 < 3; unit_type2++) {
      if (-1 < *(char *)&cities[city].supplies[unit_type2]) {
        value = 0;
        option_index = unit_type2;
        str = (iptr)label_string(commodity_names
                                   [*(char *)&cities[city].supplies[unit_type2]]);
        caravan_dialog.add_option((const char *)str,option_index,value)
        ;
      }
    }
    value = 0;
    option_id = 3;
    str = (iptr)label_string(label_table.string_ids[0xc0]);
    caravan_dialog.add_option((const char *)str,option_id,value);
    choice = caravan_dialog.show_modal(0);
    if (caravan_dialog.result_extra == 0) {
      if (choice < 3) {
        text_set_label_arg(0,commodity_names
                             [*(char *)&cities[city].supplies[choice]]);
      }
      else {
        str = (iptr)label_string(label_table.string_ids[0xc0]);
        text_set_string_arg(0,(const char *)str);
      }
      result = popup_game((const char *)&str_caraconfirm);
      if (result != 0) {
        caravan_dialog.result_extra = 1;
      }
    }
    else if (choice < 3) {
      trade_commodity_cities_dialog(civ,(int)*(char *)&cities[city].supplies[choice]);
    }
    else {
      popup_game((const char *)&str_nofoodreport);
    }
  } while (caravan_dialog.result_extra != 0);
  if (dlg_input.help_pressed != 0) {
    city_calc.zoom_requested = 1;
  }
  if (choice < 3) {
    if (*(char *)&cities[city].supplies[choice] < 1) {
      units[unit].counter = ~cities[city].supplies[choice] + 1;
    }
    else {
      units[unit].counter = cities[city].supplies[choice];
    }
    if (*(char *)&cities[city].supplies[choice] < 1) {
      goods_amount = ~cities[city].supplies[choice] + 1;
    }
    else {
      goods_amount = *(char *)&cities[city].supplies[choice];
    }
    *(char *)&cities[city].supplies[choice] = -goods_amount;
  }
  else {
    units[unit].counter = 0xff;
  }
}

/* Part of city_process_production (004ec3fe): a completed unit: create it
   (settlers shrink the city), place an AI diplomat, a human's caravan
   dialog or notice.  1 when city_process_production returns at once (the
   city is gone or the unit was refused), 0 to go on. */
static int city_process_production_unit(uptr city,uptr civ)
{
  byte cost_byte;
  int result;
  iptr str;
  iptr unit;
  int civ3;
  iptr unit_type;
  int cost2;
  int y2;
  int x2;
  uptr city3;
  iptr unit_type2;

  unit_type = (iptr)cities[city].building;
  if ((-1 < unit_types[unit_type].obsolete_by) &&
     (result = civ_has_advance(civ,(int)unit_types[unit_type].obsolete_by), result != 0)) {
    for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
      if ((unit_types[unit_type2].prerequisite == unit_types[unit_type].obsolete_by) &&
         (unit_types[unit_type2].role == unit_types[unit_type].role)) {
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
          text_set_label_arg(1,unit_types[unit_type].name);
          text_set_label_arg(2,unit_types[unit_type2].name);
          city_notify_dialog((iptr)&str_upgraded,city,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type2 * 0x3c));
        }
        unit_type = unit_type2;
        str = unit_type;
        (*((char *)&unit_type + 0)) = (i8)unit_type2;
        cities[city].building = (i8)unit_type;
        unit_type = str;
        break;
      }
    }
  }
  cost2 = (char)unit_types[unit_type].cost * city_calc.shield_rows;
  if (cost2 <= cities[city].shield_box) {
    cities[city].flags = cities[city].flags & ~CITYF_WONDER_BUILT;
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
    }
    if (((unit_types[unit_type].role == ROLE_SETTLE) && (cities[city].size == 1)) &&
       (game.difficulty == DIFF_CHIEFTAIN)) return 1;
    cities[city].shield_box -= (short)cost2;
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
       (unit_types[unit_type].role != ROLE_DIPLOMACY)) {
      unit = unit_create(unit_type,civ,(int)cities[city].x,
                                    (int)cities[city].y);
      if (unit < 0) return 0;
      if ((unit_types[unit_type].role == ROLE_DIPLOMACY) && (civs[civ].government == GOV_COMMUNISM)) {
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      else if ((unit_types[unit_type].domain == DOMAIN_LAND) || ((game.rules & RULE_NORMAL_COMBAT) == 0)) {
        result = city_has_improvement(city,IMP_BARRACKS);
        if ((((result != 0) || (result = civ_has_wonder(civ,WONDER_SUN_TZU), result != 0)) &&
            ((char)unit_types[unit_type].role < 6)) &&
           ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      else if (unit_types[unit_type].domain == DOMAIN_SEA) {
        result = city_has_improvement(city,IMP_PORT_FACILITY);
        if (((result != 0) || (result = civ_has_wonder(civ,WONDER_LIGHTHOUSE), result != 0)) &&
           ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      else if (((unit_types[unit_type].domain == DOMAIN_AIR) &&
               (result = city_has_improvement(city,IMP_AIRPORT), result != 0)) &&
              ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      if (unit_types[unit_type].role == ROLE_SEA_TRANSPORT) {
        units[unit].counter = 0xff - ((byte)city & 0x3f);
      }
    }
    if (unit_types[unit_type].role == ROLE_SETTLE) {
      if ((cities[city].size == 1) && (civs[civ].num_cities == 1)) return 1;
      if ((cities[city].size == 1) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        text_set_label_arg(1,unit_types[unit_type].name);
        if ((autoplay.active != 0) || (result = popup_game((const char *)&str_ghosttown), result == 0)) {
          unit_delete(unit,1);
          return 1;
        }
      }
      cities[city].size--;
      if (cities[city].size < 1) {
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        text_set_string_arg(0,(const char *)&cities[city].name);
        text_set_label_arg(1,unit_types[unit_type].name);
        text_set_label_arg(3,label_table.string_ids[0xbb]);
        city_delete(city,0);
        result = civ_check_destroyed(civ,0);
        if (((result == 0) &&
            (unit_create(unit_type,civ,x2,y2),
            (game.message_options & MSGOFF_UNIT_BUILT) == 0)) && (city_calc.notify_enabled != 0)) {
          popup_game((const char *)&str_built_2);
        }
        city_turn.window_shown = 0;
        city_calc.notify_enabled = 0;
        map_redraw_tile_all_views(x2,y2);
        return 1;
      }
    }
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_supported_units(1);
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      if (unit_types[unit_type].role == ROLE_TRADE) {
        unit_pick_caravan_commodity(city,unit);
      }
      if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
        city_process_production_ai_diplomat(city,civ,unit,unit_type);
      }
      if (('b' < (char)unit_types[unit_type].attack) &&
         (*(char *)&civs[civ].units_active[unit_type] == 1)) {
        for (civ3 = 1; civ3 < 8; civ3++) {
          civs[civ3].last_contact_turn[civ] = 0xffff;
        }
      }
      city_set_production(city,99);
    }
    else {
      if ((unit_types[unit_type].role == ROLE_SETTLE) || (5 < (char)unit_types[unit_type].role)) {
        text_set_label_arg(1,unit_types[unit_type].name);
        if (unit_types[unit_type].role == ROLE_TRADE) {
          city_process_production_caravan_dialog(city,civ,unit);
        }
        else {
          if (unit_types[unit_type].role == ROLE_TRADE) {
            result = crt_rand_at("city_process_production");
            cost_byte = (byte)(result >> 0x1f);
            units[unit].counter = (((byte)result ^ cost_byte) - cost_byte & 0xf ^ cost_byte) - cost_byte;
          }
          if ((game.message_options & MSGOFF_UNIT_BUILT) == 0) {
            city3 = 0xbb;
            if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
              city3 = 0xb6;
            }
            if (unit_types[unit_type].role == ROLE_TRADE) {
              city3 = 0xbc;
            }
            text_set_label_arg(3,label_table.string_ids[city3]);
            city_notify_dialog((iptr)&str_built_3,city,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c));
          }
        }
      }
      cities[city].shield_box = 0;
      if ((cities[city].flags & CITYF_AUTOBUILD) != 0) {
        city_set_production(city,99);
      }
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
      }
    }
  }
  return 0;
}

/* Part of city_process_production (004ec3fe): the notice that an AI city will
   soon complete a wonder. */
static void city_process_production_almost_wonder(uptr city,uptr civ)
{
  iptr str;
  Dialog wonder_dialog;
  iptr improvement2;
  int cost2;

  if (cities[city].building < -0x26) {
    if (cities[city].building < 1) {
      improvement2 = ~(int)cities[city].building + 1;
    }
    else {
      improvement2 = (iptr)cities[city].building;
    }
    cost2 = (uint)improvements[improvement2].cost * city_calc.shield_rows;
    if (((cost2 <= (cities[city].shield_box + city_calc.total_shields) - city_calc.shield_support) &&
        (game.wonder_city[improvement2 - 0x27] == -1)) && (autoplay.active == 0)) {
      str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)str);
      text_set_label_arg(2,improvements[improvement2].name);
      if ((autoplay.active == 0) && (2 < game.multiplayer)) {
        net_send_text_args(0x4a,0xff,3,0,improvement2,0);
      }
      wonder_dialog.Dialog::construct(0x4000);
      ON_SCOPE_EXIT(wonder_dialog.Dialog::destruct());
      wonder_dialog.load_game_text((const char *)&str_almostwonder);
      wonder_dialog.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),0,0);
      wonder_dialog.set_image_zoom(8);
      wonder_dialog.show_modal(0);
    }
  }
}

/* Part of city_process_production (004ec3fe): an AI city buys part of its
   production. */
static void city_process_production_ai_buy(uptr city,uptr civ,int net_shields)
{
  byte cost_byte;
  int result;
  uint base_cost;
  int civ3;
  iptr landmass3;
  int cost2;
  int refund;
  uint policy;
  int gold_spent;
  iptr improvement;
  iptr group;

  if (((cities[city].flags & CITYF_DEFENDER_LOST) != 0) ||
     (result = tile_unit_owner((int)cities[city].x,(int)cities[city].y), result < 0)) {
    city_set_production(city,99);
  }
  gold_spent = 0;
  if (cities[city].building < '\0') {
    if (cities[city].building < 1) {
      improvement = ~(int)cities[city].building + 1;
    }
    else {
      improvement = (iptr)cities[city].building;
    }
  }
  else {
    improvement = 0;
  }
  if (improvement == IMP_NOTHING) {
    cost_byte = unit_types[cities[city].building].cost;
  }
  else {
    cost_byte = improvements[improvement].cost;
  }
  base_cost = (uint)cost_byte;
  cost2 = base_cost * city_calc.shield_rows;
  landmass3 = tile_land_landmass((int)cities[city].x,(int)cities[city].y);
  if (landmass3 < 0) {
    policy = 0;
  }
  else {
    policy = (uint)civs[civ].continent_policy[landmass3];
  }
  if (((((policy == 0) || (policy == 1)) || (policy == 4)) &&
      ((net_shields != 0 && (-1 < cities[city].building)))) &&
     ((int)(char)unit_types[cities[city].building].role == policy)) {
    gold_spent = civs[civ].gold >> 6;
  }
  if (0x26 < improvement) {
    group = (improvement - 0x27) / 7;
    if ((*(char *)&civs[civ].wonders_by_group[group] == '\0') ||
       (civs[civ].wonders_by_group[group] < (byte)(((i8 *)&map_views_block.human_wonders_by_group_min))[group])) {
      gold_spent = civs[civ].gold >> 6;
    }
    else if ((byte)(((i8 *)&map_views_block.human_wonders_by_group_max))[group] + 1 <
             (uint)civs[civ].wonders_by_group[group]) {
      gold_spent = civs[civ].gold >> 8;
    }
    else {
      gold_spent = civs[civ].gold >> 7;
    }
    if (0x9c3 < civs[civ].gold) {
      gold_spent = gold_spent << 1;
    }
  }
  if ((0x22 < improvement) && (improvement < 0x27)) {
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         (result = civ_spaceship_exists(civ3), result != 0)) {
        gold_spent = civs[civ].gold >> 7;
        break;
      }
    }
    result = ai_spaceship_race_behind(civ);
    if (result != 0) {
      gold_spent = gold_spent << 1;
    }
  }
  if ((((cities[city].flags & CITYF_DISORDER) != 0) && (improvement != IMP_NOTHING)) && (cities[city].shield_box != 0)
     ) {
    gold_spent = civs[civ].gold >> 3;
  }
  result = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
  if (((result < 0) || ((cities[city].flags & CITYF_DEFENDER_LOST) != 0)) &&
     ((-1 < cities[city].building && (cities[city].shield_box != 0)))) {
    gold_spent = civs[civ].gold >> 3;
  }
  if ((improvement == IMP_PALACE) && (cities[city].shield_box != 0)) {
    gold_spent = civs[civ].gold >> 3;
  }
  if (((cities[city].building == UNIT_NUCLEAR_MISSILE) && (civs[civ].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) &&
     (cities[city].shield_box != 0)) {
    gold_spent = civs[civ].gold >> 4;
  }
  if (2000 < civs[civ].gold) {
    gold_spent += (civs[civ].gold >> 9);
  }
  gold_spent = int_clamp(0,gold_spent,cost2 - cities[city].shield_box);
  cities[city].shield_box += (short)gold_spent;
  refund = ((int)((uint)game.difficulty * gold_spent) / 10) /
             (int)(8 - (uint)game.power_rank[game.best_human_rank]);
  if (civs[game.worst_human_rank].gold < civs[civ].gold) {
    if (999 < civs[civ].gold) {
      refund = refund / 2;
    }
    if (1999 < civs[civ].gold) {
      refund = 0;
    }
  }
  civs[civ].gold -= (gold_spent - refund);
  city_process_production_almost_wonder(city,civ);
}

/* 004ec3fe auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_process_production(uptr city)

{
  iptr unit_owner;
  int result;
  i8 *done_sprite;
  uptr civ;
  iptr improvement2;
  uptr city2;
  iptr landmass3;
  int cost2;
  iptr part;
  iptr wonder;
  int net_shields;
  iptr improvement;
  int acquired;
  
  improvement = -1;
  acquired = 0;
  city_calc.completed_improvement = -1;
  civ = (uptr)cities[city].owner;
  net_shields = city_calc.total_shields - city_calc.shield_support;
  if ((cities[city].flags & CITYF_DISORDER) != 0) {
    net_shields = 0;
  }
  if (net_shields < 1) {
    net_shields = 0;
  }
  if (cities[city].building != -IMP_CAPITALIZATION) {
    cities[city].shield_box += (short)net_shields;
  }
  if (((civ == 0) &&
      (landmass3 = tile_landmass((int)cities[city].x,(int)cities[city].y),
      0x10 < civs[civ].continent_military[landmass3])) &&
     (unit_owner = tile_unit_owner((int)cities[city].x,(int)cities[city].y), -1 < unit_owner)) {
    cities[city].shield_box = 0;
  }
  if (cities[city].building < '\0') {
    if (cities[city].building < 1) {
      improvement = ~(int)cities[city].building + 1;
    }
    else {
      improvement = (iptr)cities[city].building;
    }
    wonder = -1;
    cost2 = (uint)improvements[improvement].cost * city_calc.shield_rows;
    if (0x26 < improvement) {
      wonder = improvement - 0x27;
    }
    if ((autoplay.active == 0) &&
       ((((-1 < wonder && (game.wonder_city[wonder] != -1)) ||
         ((wonder < 0 && (result = city_has_improvement(city,improvement), result != 0)))) &&
        (text_set_label_arg(1,improvements[improvement].name), (game.message_options & MSGOFF_INVALID_BUILD) == 0)))) {
      if (wonder < 0) {
        done_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
      }
      else {
        done_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
      }
      city_notify_dialog((iptr)&str_badbuild,city,(iptr)done_sprite);
    }
    if (((improvement < 0x23) || (0x25 < improvement)) ||
       (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
        ((result = civ_spaceship_launched(civ), result == 0 &&
         (result = civ_spaceship_part_maxed(civ,improvement), result == 0)))))) {
      if ((cost2 <= cities[city].shield_box) && (improvement != IMP_CAPITALIZATION)) {
        cities[city].flags = cities[city].flags & ~CITYF_WONDER_BUILT;
        acquired = 0;
        if (wonder < 0) {
          result = city_has_improvement(city,improvement);
          if (result != 0) {
            improvement = -1;
          }
        }
        else {
          if ((game.wonder_city[wonder] == -1) &&
             ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
            for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
              if ((((cities[city2].id != 0) &&
                   (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) != 0)) &&
                  (cities[city2].building == cities[city].building)) &&
                 (result = (uint)improvements[improvement].cost * (uint)cosmic.shield_box_rows,
                 result - cities[city2].shield_box == 0 || result < cities[city2].shield_box))
              return;
            }
          }
          if (((game.multiplayer < 3) && (game.wonder_city[wonder] == -1)) ||
             ((2 < game.multiplayer &&
              (acquired = city_acquire_wonder(civ,city,wonder,cost2,improvement),
              acquired != 0)))) {
            if (game.multiplayer < 3) {
              game.wonder_city[wonder] = (i16)city;
              cities[city].flags = cities[city].flags | CITYF_WONDER_BUILT;
            }
          }
          else {
            wonder = -1;
            improvement = -1;
            if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
              city_set_production(city,99);
            }
          }
        }
        if (((improvement < 0x23) || (0x25 < improvement)) ||
           ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
          city_process_production_complete_improvement(city,civ,cost2,wonder,improvement,acquired);
        }
        else {
          result = civ_spaceship_launched(civ);
          if (result == 0) {
            part = spaceship_choose_part_group(civ,improvement - 0x23);
            if (part < 0) {
              city_set_production(city,99);
            }
            else {
              improvement2 = part + 0x23;
              cities[city].building = -(char)improvement2;
              cost2 = (uint)improvements[improvement2].cost * city_calc.shield_rows;
              improvement = improvement2;
              if (cost2 <= cities[city].shield_box) {
                city_process_production_complete_improvement(city,civ,cost2,wonder,improvement,acquired);
              }
            }
          }
          else {
            city_set_production(city,99);
          }
        }
      }
    }
    else {
      text_set_label_arg(1,improvements[improvement].name);
      city_notify_dialog((iptr)&str_badspace,city,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c));
    }
  }
  else {
    if (city_process_production_unit(city,civ) != 0) return;
  }
  if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
       ((cities[city].flags & CITYF_AUTOBUILD) != 0)) &&
      ((cities[city].shield_box == 0 || ((cities[city].building < '\0' && (improvement < 0))))))
     && (city_set_production(city,99), cities[city].building == 'c')) {
    cities[city].flags = cities[city].flags & ~(CITYF_AUTOBUILD | CITYF_AUTO_UNITS_ONLY | CITYF_AUTO_IMPROVEMENTS_ONLY);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    city_process_production_ai_buy(city,civ,net_shields);
  }
  cities[city].flags = cities[city].flags & ~CITYF_DEFENDER_LOST;
  return;
}

/* 004eef23 auto */
void __cdecl city_disband_unsupported_units(uptr city)

{
  byte civ2;
  byte government;
  bool within_support;
  bool near_city;
  iptr civ;
  int dist;
  i32 tile_improv;
  iptr city3;
  iptr cur_unit;
  iptr unit;
  iptr city2;
  int max_dist;
  iptr unit2;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  government = civs[civ].government;
  while( true ) {
    city_calc.support_unit_count = 0;
    city_calc.shield_support = 0;
    unit = 0;
    do {
      cur_unit = unit;
      if (game.num_units <= unit) {
        return;
      }
      if (((((units[unit].id != 0) && (units[unit].owner == civ)) &&
           ((byte)units[unit].home_city == city)) &&
          ((char)unit_types[units[unit].type].role < 6)) &&
         ((city_count_unit_support(city,unit,(uint)government), city_calc.total_shields < city_calc.shield_support ||
          ((((cities[city].flags & CITYF_DISORDER) != 0 && ((game.turn + unit & 7U) == 0)) &&
           (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0 && (GOV_FUNDAMENTALISM < government)))))))) {
        within_support = city_calc.shield_support <= city_calc.total_shields;
        max_dist = -1;
        unit2 = -1;
        for (unit = 0; unit < game.num_units; unit++) {
          if (((units[unit].id != 0) && ((byte)units[unit].home_city == city)) &&
             ((char)unit_types[units[unit].type].role < 6)) {
            if (within_support) {
              if ((4 < (char)unit_types[units[unit].type].role) ||
                 (dist = tile_city_owner((int)units[unit].x,(int)units[unit].y), -1 < dist))
              continue;
              tile_improv = tile_improvements((int)units[unit].x,(int)units[unit].y);
              if (((byte)tile_improv & TILE_AIRBASE) == 0x40) {
                near_city = false;
                for (city2 = 0; city2 < game.num_cities; city2++) {
                  if (((cities[city2].id != 0) && (cities[city2].owner == cities[city].owner)
                      ) && (dist = map_distance((int)units[unit].x,(int)units[unit].y,
                                                       (int)cities[city2].x,(int)cities[city2].y
                                                      ), dist < 4)) {
                    near_city = true;
                    break;
                  }
                }
                if (near_city) continue;
              }
            }
            dist = map_distance_diag((int)units[unit].x,(int)units[unit].y,
                                       (int)cities[city].x,(int)cities[city].y);
            if (max_dist < dist) {
              unit2 = unit;
              max_dist = dist;
            }
          }
        }
        if (city_calc.total_shields < city_calc.shield_support) {
          if (-1 < max_dist) {
            if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
              text_set_label_arg(1,unit_types[units[unit2].type].name);
              city_notify_dialog((iptr)&str_support,city,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit2].type * 0x3c));
            }
            unit_destroy_redraw(unit2,1);
            if (city_turn.window_shown != 0) {
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_supported_units(1);
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
            }
            break;
          }
        }
        else if (((game.game_flags & GAME_AI_GANG_UP) == 0) ||
                (civs[game.best_human_rank].num_advances <= civs[civ].num_advances)) {
          if ((0 < max_dist) && (unit_types[units[unit2].type].domain == DOMAIN_LAND)) {
            city3 = city_at((int)units[unit2].x,(int)units[unit2].y);
            if (-1 < city3) {
              cities[city3].shield_box =
                   (short)(((char)unit_types[units[unit2].type].cost * city_calc.shield_rows) / 2) +
                   cities[city3].shield_box;
            }
            unit_delete(unit2,1);
            cities[city].flags = cities[city].flags & ~(CITYF_DISORDER | CITYF_DISORDER_2ND_TURN | CITYF_DISORDER_LONG);
            break;
          }
        }
        else {
          civs[civ].government_score[GOV_DEMOCRACY] = -999;
          civs[civ].government_score[GOV_REPUBLIC] = -999;
        }
      }
      unit = cur_unit + 1;
    } while( true );
  }
}

/* 004ef578 auto */
void __cdecl city_process_disorder(iptr city)

{
  byte civ2;
  iptr civ;
  int happy_margin;
  iptr title;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  if (civ != 0) {
    happy_margin = city_calc.happy_count - city_calc.unhappy_count;
    if (happy_margin < 0) {
      if (((human_civ[0] == civ) && (autoplay.active == 0)) && ((game.message_options & MSGOFF_DISORDER) == 0))
      {
        snd_play_effect(SND_CIVDISOR,1,0,0);
        popup_city_picture_zoom((const char *)&str_disorder,city,0x48,0,0);
      }
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        cities[city].flags = cities[city].flags | (CITYF_DISORDER | CITYF_ON_EDGE);
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
      }
      else {
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        if ((cities[city].flags & CITYF_DISORDER_2ND_TURN) != 0) {
          cities[city].flags = cities[city].flags | CITYF_DISORDER_LONG;
        }
        cities[city].flags = cities[city].flags | CITYF_DISORDER_2ND_TURN;
        if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) &&
            (civs[civ].government == GOV_DEMOCRACY)) &&
           (((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 0x10) == 0)))) {
          city_notify_dialog((iptr)&str_revolt,city,(iptr)&government_sprites_block.government_sprite_democracy);
          civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
          civ_change_government(civ,0);
          civs[civ].flags = civs[civ].flags | CIVF_REVOLUTION;
        }
      }
    }
    else {
      if (((city_calc.happy_count == city_calc.unhappy_count) && (0 < city_calc.total_shields - city_calc.shield_support)) &&
         (-1 < city_calc.food_surplus)) {
        cities[city].flags = cities[city].flags | CITYF_ON_EDGE;
      }
      else {
        cities[city].flags = cities[city].flags & ~CITYF_ON_EDGE;
      }
      if ((-1 < happy_margin) && ((cities[city].flags & CITYF_DISORDER) != 0)) {
        cities[city].flags = cities[city].flags & ~(CITYF_DISORDER | CITYF_DISORDER_2ND_TURN | CITYF_DISORDER_LONG);
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        else if ((game.message_options & MSGOFF_ORDER_RESTORED) == 0) {
          city_notify_dialog_ex((iptr)&str_restored,city,0,0);
        }
        map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
      }
      if (civs[civ].government != GOV_ANARCHY) {
        civs[civ].gold += city_calc.tax_total;
      }
    }
    if ((((city_calc.unhappy_count == 0) && (2 < cities[city].size)) &&
        (cities[city].size + 1 >> 1 <= city_calc.happy_count)) && (civs[civ].government != GOV_ANARCHY)) {
      if ((cities[city].flags & CITYF_CELEBRATING) == 0) {
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
          title = civ_leader_title(civ);
          text_set_string_arg_b(1,(const char *)title);
          if ((game.message_options & MSGOFF_CELEBRATION) == 0) {
            if (city_calc.notify_enabled != 0) {
              music_play_track(3,0);
            }
            popup_city_picture_zoom((const char *)&str_weloveking,city,0x4a,0,0);
          }
        }
        cities[city].flags = cities[city].flags | CITYF_CELEBRATING;
      }
      else if (((GOV_FUNDAMENTALISM < civs[civ].government) &&
               ((int)((int)cities[city].size * (uint)cosmic.food_per_citizen +
                     city_calc.settlers_supported * city_calc.settler_food_cost) < city_calc.yield_totals[0])) &&
              (happy_margin = city_needed_size_improvement(city), happy_margin == 0) &&
              (!bugfix_state() || (cities[city].size < 0x7f))) {  /* B16 */
        cities[city].size++;
      }
    }
    else if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
      if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (civs[civ].government != GOV_ANARCHY)) && ((cities[city].flags & CITYF_DISORDER) == 0)) {
        title = civ_leader_title(civ);
        text_set_string_arg_b(1,(const char *)title);
        if ((game.message_options & MSGOFF_CELEBRATION) == 0) {
          city_notify_dialog_ex((iptr)&str_wedontloveking,city,0,0);
        }
      }
      cities[city].flags = cities[city].flags & ~CITYF_CELEBRATING;
    }
  }
  return;
}

/* 004efbc6 auto */
void __cdecl city_add_science(iptr city)

{
  byte civ2;
  uptr civ;
  int result;
  iptr improvement;
  
  civ2 = cities[city].owner;
  civ = (uptr)(char)civ2;
  if ((((-1 < civs[civ].researching) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0))
      && (game.difficulty == DIFF_CHIEFTAIN)) &&
     ((civs[civ].researching != 0x59 && (game.first_discoverer[civs[civ].researching] != '\0')))
     ) {
    city_calc.science_total = city_calc.science_total << 1;
  }
  if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (DIFF_WARLORD < game.difficulty)) {
    result = ai_spaceship_race_behind(civ);
    if (result != 0) {
      for (improvement = 0x23; improvement < 0x26; improvement++) {
        result = civ_has_advance(civ,(int)improvements[improvement].preq);
        if (result == 0) {
          city_calc.science_total = city_calc.science_total << 1;
          break;
        }
      }
    }
  }
  if (civs[civ].government != GOV_ANARCHY) {
    civ_add_research_points(civ,city_calc.science_total);
  }
  return;
}

/* 004efd44: written by hand in src/game/retvals.cpp */

/* 004f0221 auto */
void __cdecl city_pay_improvement_upkeep(iptr city)

{
  iptr civ;
  int upkeep;
  iptr improvement;
  
  civ = (iptr)cities[city].owner;
  if (civs[civ].government != GOV_ANARCHY) {
    for (improvement = 1; improvement < 0x27; improvement++) {
      upkeep = city_has_improvement(city,improvement);
      if (upkeep != 0) {
        upkeep = civ_improvement_upkeep(civ,improvement);
        civs[civ].gold -= upkeep;
        if (civs[civ].gold < 0) {
          civs[civ].gold = 0;
          city_set_improvement(city,improvement,0);
          if (autoplay.active == 0) {
            text_set_label_arg(1,improvements[improvement].name);
            text_set_number_arg(0,(uint)improvements[improvement].cost * city_calc.shield_rows);
            city_notify_dialog((iptr)&str_inhock,city,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c));
          }
          civs[civ].gold += (uint)improvements[improvement].cost * city_calc.shield_rows;
        }
      }
    }
  }
  return;
}

/* 004f080d auto */
void __cdecl city_add_terraform_goals(iptr city)

{
  byte civ2;
  bool added_goal;
  int civ;
  iptr x;
  int y;
  int in_bounds;
  uint improvements2;
  iptr tile_idx;
  
  added_goal = false;
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  for (tile_idx = 0; tile_idx < 0x14; tile_idx++) {
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + (int)cities[city].x);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx];
    in_bounds = map_in_bounds(x,y);
    if ((in_bounds != 0) && (improvements2 = tile_improvements(x,y), (improvements2 & TILE_POLLUTION) != 0)) {
      civ_add_ai_goal(civ,x,y,0x15,6);
      cities[city].flags = cities[city].flags | CITYF_AI_TERRAFORM_GOAL;
      added_goal = true;
    }
  }
  if (!added_goal) {
    y = civ_has_advance(civ,ADV_RAILROAD);
    if ((y != 0) &&
       (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0 || (city_turn.tile_job_found == 0)))) {
      if (city_calc.tile_job_priority < 4) {
        city_calc.tile_job_priority = 3;
      }
      ai_find_road_link_tile(city);
      if (city_turn.tile_job_found == 0) {
        return;
      }
    }
    city_calc.tile_job_priority += 2;
    if (city_turn.tile_job_found == 0) {
      city_calc.tile_job_priority = 2;
      ai_find_road_link_tile(city);
    }
    if (city_turn.tile_job_found == 0) {
      cities[city].flags = cities[city].flags & ~CITYF_AI_TERRAFORM_GOAL;
    }
    else {
      if (2 < city_calc.tile_job_priority) {
        if (4 < cities[city].size) {
          city_calc.tile_job_priority++;
        }
        y = city_has_improvement(city,IMP_PALACE);
        if (y != 0) {
          city_calc.tile_job_priority++;
        }
      }
      if (cities[city].size < 4) {
        city_calc.tile_job_priority--;
      }
      civ_add_ai_goal(civ,city_calc.tile_job_x,city_calc.tile_job_y,0x15,city_calc.tile_job_priority);
      cities[city].flags = cities[city].flags | CITYF_AI_TERRAFORM_GOAL;
    }
  }
  return;
}

/* 004f0a9c auto */
int __cdecl city_process_turn(uptr city)

{
  byte civ2;
  char civilize_level;
  short prev_food;
  iptr civ;
  uptr window_city;
  int result;
  i32 clamped;
  int gov_idx;
  uint free_support;
  short unrest_factor;
  int has_police;
  
  city_wnd.refresh_locked = 1;
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  if (((cities[city].turns_captured - 1 ^ (int)game.turn & 0x3fU) & 0x3f) == 0) {
    cities[city].founder = civ2;
  }
  if ((human_civ[0] == civ) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
    city_calc.notify_enabled = 1;
  }
  else {
    city_calc.notify_enabled = 0;
  }
  if (autoplay.active != 0) {
    city_calc.notify_enabled = 0;
  }
  city_calc.zoom_requested = 0;
  cities[city].flags = cities[city].flags & ~(CITYF_IMPROVEMENT_SOLD | CITYF_AI_SETTLER_NEAR | CITYF_INVESTIGATED);
  window_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  (*(uint *)&city_turn.window_shown) = (uint)(window_city == city);
  city_turn.window_dirty = 0;
  prev_food = cities[city].food_box;
  result = city_process_food(city);
  if (result == 0) {
    city_recalc(city,1);
    if (((*(uint *)&city_turn.window_shown) != 0) && (city_turn.window_dirty != 0)) {
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    if ((city_calc.workers_overflow != 0) && (autoplay.active == 0)) {
      city_notify_dialog_ex((iptr)&str_decrease,city,0,0);
    }
    city_calc.food_surplus = (city_calc.yield_totals[0] - (int)cities[city].size * (uint)cosmic.food_per_citizen) -
                   city_calc.settlers_supported * city_calc.settler_food_cost;
    cities[city].food_box += (short)city_calc.food_surplus;
    if ((((0 < prev_food) && (cities[city].food_box < prev_food)) &&
        ((int)cities[city].food_box + city_calc.food_surplus * 3 < 0)) &&
       (((game.message_options & MSGOFF_FOOD_LOW) == 0 && (autoplay.active == 0)))) {
      city_notify_dialog_ex((iptr)&str_foodshortage,city,0,0);
    }
    city_process_production(city);
    city_disband_unsupported_units(city);
    city_turn.worker_realloc = 1;
    city_turn.processing = 1;
    city_turn.tile_job_found = 0;
    city_calc.tile_job_priority = 0;
    city_recalc(city,1);
    city_turn.worker_realloc = 0;
    if (city_calc.total_shields < city_calc.shield_support) {
      civs[civ].government_score[GOV_ANARCHY] =
           civs[civ].government_score[GOV_ANARCHY] + ((short)city_calc.shield_support - (short)city_calc.total_shields) * 5;
    }
    clamped = int_clamp(city_calc.support_unit_count - (uint)cosmic.monarchy_free_support,0,
                               (int)cities[city].size);
    civs[civ].government_score[GOV_ANARCHY] = (short)clamped + civs[civ].government_score[GOV_ANARCHY];
    for (gov_idx = 1; gov_idx < 7; gov_idx++) {
      free_support = 0;
      switch(gov_idx) {
      case 1:
        free_support = (uint)cities[city].size;
        break;
      case 2:
        free_support = (uint)cosmic.monarchy_free_support;
        break;
      case 3:
        free_support = (uint)cosmic.communism_free_support;
        break;
      case 4:
        free_support = (uint)cosmic.fundamentalism_free_support;
      }
      if ((int)free_support < city_calc.support_unit_count) {
        civs[civ].government_score[gov_idx] =
             civs[civ].government_score[gov_idx] -
             ((short)city_calc.support_unit_count - (short)free_support);
      }
    }
    result = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((result == 0) && (result = city_has_improvement(city,IMP_POLICE_STATION), result == 0)) {
      has_police = 0;
    }
    else {
      has_police = 1;
    }
    civilize_level = leaders[civs[civ].tribe].civilize;
    unrest_factor = -(short)civilize_level + 7;
    result = city_has_improvement(city,IMP_MARKETPLACE);
    if (result != 0) {
      unrest_factor = -(short)civilize_level + 5;
    }
    result = city_has_improvement(city,IMP_BANK);
    if (result != 0) {
      unrest_factor--;
    }
    result = city_has_improvement(city,IMP_TEMPLE);
    if ((result != 0) &&
       ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
        (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
      unrest_factor--;
    }
    window_city = wonder_get_city(WONDER_SHAKESPEARE);
    if (window_city != city) {
      result = leaders[civs[civ].tribe].attack + city_calc.aggressive_units;
      prev_food = civs[civ].government_score[GOV_DEMOCRACY];
      clamped = int_clamp((2 - has_police) * (result + 1),0,99);
      civs[civ].government_score[GOV_DEMOCRACY] = prev_food - (short)clamped * unrest_factor;
      prev_food = civs[civ].government_score[GOV_REPUBLIC];
      clamped = int_clamp(result * (1 - has_police),0,99);
      civs[civ].government_score[GOV_REPUBLIC] = prev_food - (short)clamped * unrest_factor;
    }
    city_process_disorder(city);
    city_add_science(city);
    if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
      city_process_pollution(city);
      city_pay_improvement_upkeep(city);
    }
    city_add_terraform_goals(city);
    result = city_calc.total_shields * 2 - city_calc.shield_support;
    if (civs[civ].best_shield_surplus <= result) {
      civs[civ].best_shield_surplus = (i16)result;
    }
    city_wnd.refresh_locked = 0;
    city_turn.calc_cached_id = 0xffffffff;
    if ((*(uint *)&city_turn.window_shown) != 0) {
      city_save_build_state(city);
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    if (city_calc.zoom_requested != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    }
    city_turn.processing = 0;
    result = city_calc.happy_count - city_calc.unhappy_count;
  }
  else {
    result = -999;
  }
  return result;
}

/* 004f1220 auto */
void __cdecl wonder_apollo_reveal_map(void)

{
  Tile *tile;
  i32 arg_zero;
  i32 arg_one;
  iptr known_civ;
  uint y2;
  iptr civ_idx;
  iptr city;
  uint x2;
  int tile_idx;
  
  if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
    y2 = 0;
    x2 = 0;
    net_batch_begin();
    for (tile_idx = 0; tile_idx < map.area; tile_idx++) {
      tile_set_visible(x2,y2,0xff,1,1);
      for (civ_idx = 1; civ_idx < 8; civ_idx++) {
        arg_one = 1;
        arg_zero = 0;
        known_civ = civ_idx;
        tile = tile_at(x2,y2);
        known_set(x2,y2,CONCAT31((int3)((uint)(iptr)tile >> 8),(i8)tile->improvements),
                  known_civ,arg_zero,arg_one);
      }
      x2 += 2;
      if ((int)map.width <= (int)x2) {
        y2++;
        x2 = y2 & 1;
      }
    }
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        cities[city].known_to = 0xff;
        for (civ_idx = 0; civ_idx < 8; civ_idx++) {
          *(i8 *)&cities[city].revealed_size[civ_idx] = cities[city].size;
        }
        tile_set_visible((int)cities[city].x,(int)cities[city].y,0xff,1,1);
      }
    }
    net_batch_flush();
    if (2 < game.multiplayer) {
      net.tile_sync_enabled = 1;
      net_send_state_diff(0xff,2,0,0,0);
      net_send_message(NM_DRAW_MAP,0xff,0,0,0,0,0,0,0,0);
      if (autoplay.active == 0) {
        net_send_text_args(0x49,0xff,0,0,0,0);
      }
    }
    map_redraw_all_views(human_civ[0],1);
    if (autoplay.active == 0) {
      popup_game((const char *)&str_astronauts);
    }
  }
  else {
    net_send_message(NM_DO_APOLLO,0,0,0,0,0,0,0,0,0);
  }
  return;
}

/* 004f3d30 auto */
uint __cdecl city_tile_status_flags(iptr city,iptr tile_idx)

{
  return (uint)(byte)(((i8 *)&city_calc.tile_flags))[tile_idx];
}

/* 004f3d60 auto */
CityPicturePopup *CityPicturePopup::construct()

{
  
  PictureWindow::construct();
  dialog.Dialog::construct(0x4000);
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 3);
  return this;
}

/* 004f3e20 auto */
void *CityPicturePopup::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004f3e70: written by hand in src/game/unwind.cpp */

/* 004f3f30 auto */
void __cdecl city_notify_dialog(iptr message,iptr city,iptr extra)

{
  city_notify_dialog_ex(message,city,0,extra);
  return;
}

/* 00500e00 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_font_global_init(void)

{
  city_window_font_static_init();
  city_window_font_reg_atexit();
  return;
}

/* 00500e1a auto */
void __cdecl city_window_font_static_init(void)

{
  ((GameFont *)((i8 *)&city_window_font))->init(0,0x10);
  return;
}

/* 00500e38 auto */
void __cdecl city_window_font_reg_atexit(void)

{
  _atexit(city_window_font_atexit_dtor);
  return;
}

/* 00500e55 auto */
void __cdecl city_window_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&city_window_font))->destroy();
  return;
}

/* 00500e6f auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_small_font_global_init(void)

{
  city_window_small_font_static_init();
  city_window_small_font_reg_atexit();
  return;
}

/* 00500e89 auto */
void __cdecl city_window_small_font_static_init(void)

{
  ((GameFont *)((i8 *)&city_window_small_font))->init(0,10);
  return;
}

/* 00500ea7 auto */
void __cdecl city_window_small_font_reg_atexit(void)

{
  _atexit(city_window_small_font_atexit_dtor);
  return;
}

/* 00500ec4 auto */
void __cdecl city_window_small_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&city_window_small_font))->destroy();
  return;
}

/* 00500ede auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_global_init(void)

{
  city_window_static_init();
  city_window_reg_atexit();
  return;
}

/* 00500ef8 auto */
void __cdecl city_window_static_init(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->construct();
  return;
}

/* 00500f12 auto */
void __cdecl city_window_reg_atexit(void)

{
  _atexit(city_window_atexit_dtor);
  return;
}

/* 00500f2f auto */
void __cdecl city_window_atexit_dtor(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->destruct();
  return;
}

/* 00500f49 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_bg_surface_global_init(void)

{
  city_window_bg_surface_static_init();
  city_window_bg_surface_reg_atexit();
  return;
}

/* 00500f63 auto */
void __cdecl city_window_bg_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&city_window_bg_surface));
  return;
}

/* 00500f7d auto */
void __cdecl city_window_bg_surface_reg_atexit(void)

{
  _atexit(city_window_bg_surface_atexit_dtor);
  return;
}

/* 00500f9a auto */
void __cdecl city_window_bg_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&city_window_bg_surface));
  return;
}

/* 00500fb4 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_tile_pick_surface_global_init(void)

{
  city_tile_pick_surface_static_init();
  city_tile_pick_surface_reg_atexit();
  return;
}

/* 00500fce auto */
void __cdecl city_tile_pick_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&city_tile_pick_surface));
  return;
}

/* 00500fe8 auto */
void __cdecl city_tile_pick_surface_reg_atexit(void)

{
  _atexit(city_tile_pick_surface_atexit_dtor);
  return;
}

/* 00501005 auto */
void __cdecl city_tile_pick_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&city_tile_pick_surface));
  return;
}

/* 0050101f auto */
void __cdecl city_window_load_background(void)

{
  i32 height;
  i32 width;
  i8 dest_rect[16];
  DibSurface surface;
  GamePalette palette;
  Sprite sprite;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  if (city_wnd.size_level != city_wnd_state.bg_size_level) {
    city_wnd_state.bg_size_level = city_wnd.size_level;
    ((DibSurface *)((i8 *)&city_window_bg_surface))->release();
    surface.load_gif_file((const char *)&str_city_gif,10,0xc0,(iptr)&palette);
    sprite.grab_from_surface((iptr)&surface,7,0,0,0x27c,0x1a5);
    surface.release();
    height = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x1a5);
    width = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x27c);
    ((DibSurface *)((i8 *)&city_window_bg_surface))->DibSurface::create(width,height);
    gfx_set_zoom_scale(city_wnd_state.bg_size_level * 4 - 8);
    sprite.draw((iptr)dest_rect,(iptr)&city_window_bg_surface,0,0);
    gfx_reset_zoom_scale();
  }
  return;
}

/* 005013bc auto */
void __cdecl city_window_modal_notice(void)

{
  if (city_wnd_state.modal_notice_active == '\0') {
    city_wnd_state.modal_notice_active = 1;
    text_clear();
    text_append((const char *)&str_citymodal);
    text_append_int((city_wnd.open_mode == 2) + 1);
    popup_game((const char *)&textbuf.buf);
    if (dlg_input.help_pressed != 0) {
      city_window_exit_clicked(0);
    }
    city_wnd_state.modal_notice_active = '\0';
  }
  return;
}

/* 00501440 auto */
void CityWindow::reset_state()

{
  clickmap.clear();
  is_closed_b0[0] = 1;
  is_closed_b1[0] = '\0';
  is_closed_b2[0] = '\0';
  is_closed_b3[0] = '\0';
  redraw_suspended_b0[0] = '\0';
  redraw_suspended_b1[0] = '\0';
  redraw_suspended_b2[0] = '\0';
  redraw_suspended_b3[0] = '\0';
  redraw_deferred_b0[0] = '\0';
  redraw_deferred_b1[0] = '\0';
  redraw_deferred_b2[0] = '\0';
  redraw_deferred_b3[0] = '\0';
  city_index_b0[0] = 0xff;
  city_index_b1[0] = 0xff;
  city_index_b2[0] = 0xff;
  city_index_b3[0] = 0xff;
  improvement_scroll_pos_b0[0] = '\0';
  improvement_scroll_pos_b1[0] = '\0';
  improvement_scroll_pos_b2[0] = '\0';
  improvement_scroll_pos_b3[0] = '\0';
  panel_toggle_flag_b0[0] = 1;
  panel_toggle_flag_b1[0] = '\0';
  panel_toggle_flag_b2[0] = '\0';
  panel_toggle_flag_b3[0] = '\0';
  exit_button_b0[0] = '\0';
  exit_button_byte1[0] = '\0';
  exit_button_byte2[0] = '\0';
  exit_button_byte3[0] = '\0';
  return;
}

/* 005014bf auto */
/* Library Function - Single Match
    public: void __thiscall CMiniDockFrameWnd::OnClose(void)
   
   Library: Visual Studio 1998 Debug */

void CityWindow::remove_citizen_hotspots()

{
  clickmap.remove_by_id(2);
  return;
}

/* 005014e5 auto */
void CityWindow::remove_panel_hotspots()

{
  clickmap.remove_by_id(1);
  clickmap.remove_by_id(3);
  clickmap.remove_by_id(4);
  return;
}

/* 0050152b auto */
void CityWindow::remove_hotspots()

{
  remove_panel_hotspots();
  remove_citizen_hotspots();
  return;
}

/* 00501551 auto */
CityWindow *CityWindow::construct()

{
  
  this->FrameWindow::construct();
  clickmap.construct();
  ((GameFont *)&city_font)->init_empty();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 5);
  reset_state();
  return this;
}

/* 0050160a auto */
void CityWindow::destruct()

{
  int seh_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 5);
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 2;
  remove_hotspots();
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 00501673 */ { ((GameFont *)(((int)this) + 0x16ac))->destroy(); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00501682 */ { ((ClickMap *)(((int)this) + 0x2d8))->destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00501691 */ { (((FrameWindow *)this))->FrameWindow::destruct(); }
  return;
}

/* 005016b2 auto */
void CityWindow::blit_bg_rect(int *rect)

{
  i32 height;
  i32 width;
  
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  gfx_copy_rect_xywh((iptr)&city_window_bg_surface,(iptr)this,
                     (*rect - margin_x_2) - client_x,
                     (rect[1] - content_offset_y) - client_y,*rect,
                     rect[1],width,height);
  return;
}

/* 00501733 auto */
void CityWindow::begin_draw(iptr rect)

{
  gfx_set_draw_target((iptr)this);
  text_set_surface((iptr)this);
  text_set_font((iptr)((u8 *)&city_font));
  blit_bg_rect((int *)rect);
  return;
}

/* 00501780 auto */
void CityWindow::recalc_redraw(i32 flag)

{
  if ((((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) && (((u32)redraw_deferred) == 0)) &&
     (-1 < (int)((u32)city_index))) {
    city_recalc(((u32)city_index),1);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_citizens(1,flag);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_resource_map(1);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
  }
  return;
}

/* 00501819 auto */
void __cdecl city_window_cycle_specialist(int citizen_idx)

{
  iptr city3;
  iptr city;
  iptr city4;
  int city2;
  int spec_idx;
  short spec_slot;
  i32 recalc_flag;
  int spec_type;
  
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    recalc_flag = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_flag);
    ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if (cities[city4].size - city_calc.specialist_count <= (int)(short)citizen_idx) {
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      spec_slot = (short)citizen_idx - ((short)cities[city2].size - (short)city_calc.specialist_count);
      if (spec_slot < city_calc.specialist_count) {
        spec_idx = (int)spec_slot;
        city2 = spec_idx;
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        spec_type = city_specialist_type(city,city2);
        city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        if (cities[city2].size < 5) {
          if (spec_type == 1) {
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            popup_game_with_city((const char *)&str_elviserr,city);
            return;
          }
          spec_type = 1;
        }
        else {
          spec_type++;
          if (3 < spec_type) {
            spec_type = 1;
          }
        }
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        city_set_specialist_type(city,spec_idx,spec_type);
        ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(1);
      }
    }
  }
  return;
}

/* 005019c1 auto */
int CityWindow::draw_citizen_row(int x,int y,i32 width,int happy,int unhappy, int angry)

{
  int arg_y;
  int arg_x;
  int style_era;
  int spec_kind;
  i8 *rect_arg;
  CityWindow *view;
  iptr zero_a;
  iptr zero_b;
  i32 shadow_color;
  i8 spec_rect[16];
  i8 spec_shadow_rect[16];
  i8 unhappy_rect[16];
  i8 unhappy_shadow_rect[16];
  i8 content_rect[16];
  i8 content_shadow_rect[16];
  i8 happy_rect[16];
  i8 happy_shadow_rect[16];
  int mood_idx;
  int spacing;
  int i;
  uint citizen_num;
  
  gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
  zero_b = 0;
  zero_a = 0;
  arg_y = scale(0x1b);
  spacing = ui_calc_item_spacing((int)cities[((u32)city_index)].size,arg_y + 1,width,zero_a,zero_b)
  ;
  citizen_num = 0;
  for (i = 0; i < happy; i++) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = happy_shadow_rect;
    view = this;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (citizen_num & 1) * 0x3c))->draw_silhouette((iptr)rect_arg,
               (iptr)view,arg_x,arg_y,shadow_color);
    rect_arg = happy_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (citizen_num & 1) * 0x3c))->draw((iptr)rect_arg,
               (iptr)view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0;
      i < (int)cities[((u32)city_index)].size - (unhappy + happy + city_calc.specialist_count);
      i = i + 1) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = content_shadow_rect;
    view = this;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) * 4 + 8) * 0xf))->draw_silhouette(
               (iptr)rect_arg,(iptr)view,arg_x,arg_y,shadow_color);
    rect_arg = content_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) * 4 + 8) * 0xf))->draw(
               (iptr)rect_arg,(iptr)view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < unhappy; i++) {
    if (i < unhappy - angry) {
      mood_idx = 4;
    }
    else {
      mood_idx = 6;
    }
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = unhappy_shadow_rect;
    view = this;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) + mood_idx) * 0x3c))->draw_silhouette(
               (iptr)rect_arg,(iptr)view,arg_x,arg_y,shadow_color);
    rect_arg = unhappy_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) + mood_idx) * 0x3c))->draw(
               (iptr)rect_arg,(iptr)view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < city_calc.specialist_count; i++) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = spec_shadow_rect;
    view = this;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (spec_kind * 4 + 0x1c) * 0xf))->draw_silhouette(
               (iptr)rect_arg,(iptr)view,arg_x,arg_y,shadow_color);
    rect_arg = spec_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era((int)cities[((u32)city_index)].owner);
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (spec_kind * 4 + 0x1c) * 0xf))->draw(
               (iptr)rect_arg,(iptr)view,arg_y,arg_x);
    x += spacing;
  }
  gfx_reset_zoom_scale();
  return spacing;
}

/* 00501e63 auto */
int CityWindow::happy_draw_citizen_row(int x,i32 y,i32 width,int happy, int unhappy,int angry)

{
  int arg_x;
  int spec_kind;
  i8 *rect_arg;
  CityWindow *view;
  iptr zero_a;
  iptr zero_b;
  i32 arg_y;
  i8 spec_rect[16];
  i8 unhappy_rect[16];
  i8 content_rect[16];
  i8 happy_rect[16];
  int mood_idx;
  int spacing;
  int i;
  uint citizen_num;
  
  gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
  zero_b = 0;
  zero_a = 0;
  arg_x = scale(0xf);
  spacing = ui_calc_item_spacing((int)cities[((u32)city_index)].size,arg_x + 1,width,zero_a,zero_b)
  ;
  citizen_num = 0;
  for (i = 0; i < happy; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + (citizen_num & 1) * 0x3c))->draw((iptr)happy_rect,(iptr)this,
               x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0;
      i < (int)cities[((u32)city_index)].size - (unhappy + happy + city_calc.specialist_count);
      i = i + 1) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + ((citizen_num & 1) * 4 + 8) * 0xf))->draw((iptr)content_rect,
               (iptr)this,x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < unhappy; i++) {
    if (i < unhappy - angry) {
      mood_idx = 4;
    }
    else {
      mood_idx = 6;
    }
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + ((citizen_num & 1) + mood_idx) * 0x3c))->draw((iptr)unhappy_rect,
               (iptr)this,x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < city_calc.specialist_count; i++) {
    rect_arg = spec_rect;
    view = this;
    arg_x = x;
    arg_y = y;
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + (spec_kind * 4 + 0x1c) * 0xf))->draw((iptr)rect_arg,(iptr)view
               ,arg_x,arg_y);
    x += spacing;
  }
  gfx_reset_zoom_scale();
  return spacing;
}

/* 0050207f auto */
void CityWindow::draw_citizens(int blit,i32 flag)

{
  int top;
  i32 value;
  int pos;
  int offset;
  i32 row_width;
  iptr height;
  i32 unhappy;
  i32 angry;
  int x2;
  int i;
  i8 hotspot_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)(&citizens_rect));
    clickmap.remove_by_id(2);
    x2 = citizens_rect;
    top = citizens_rect_top;
    text_set_colors(0x7c,0x12,1,1);
    if (extra_ui(X_CITY_ATTITUDE)) {
      /* X23: coloured as in the Attitude Advisor: yellow celebrating, red
         in disorder */
      text_set_colors(extras_attitude_color(city_index, 0x7c),0x12,1,1);
    }
    text_clear();
    text_append_label(0x193);
    value = scale(0xc6);
    pos = scale(0x2e);
    pos = top + pos;
    offset = scale(2);
    gfx_draw_text_centered((const char *)&textbuf.buf,x2 + offset,pos,value);
    text_clear();
    text_append_label(0x3f);
    value = scale(0xee);
    pos = scale(0x2e);
    pos = top + pos;
    offset = scale(199);
    gfx_draw_text_centered((const char *)&textbuf.buf,x2 + offset,pos,value);
    value = city_calc.happy_count;
    unhappy = city_calc.unhappy_count;
    angry = city_calc.angry_count;
    row_width = scale(0x1a6);
    pos = scale(9);
    pos = top + pos;
    offset = scale(5);
    pos = draw_citizen_row(x2 + offset,pos,row_width,value,unhappy,angry);
    if (bugfix_ui()) {
      /* B10: each hotspot centred on its citizen, not at its sprite's left edge,
         which later sprites cover when they are drawn closer (docs/BUGS.md) */
      offset = scale(5) + scale(0xe) - pos / 2 - 1;
    }
    else {
      offset = scale(5);
    }
    x2 += offset;
    offset = scale(9);
    for (i = 0; i < cities[city_index].size;
        i = i + 1) {
      height = scale(0x1e);
      rect_set_xywh((iptr)hotspot_rect,x2,top + offset,pos,height);
      clickmap.hotspot_add_rect(i,2,(RECT *)hotspot_rect);
      x2 += pos;
    }
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)(&citizens_rect));
    }
  }
  return;
}

/* 005022c0 auto */
void __cdecl city_window_toggle_worked_tile(int mouse_x,int mouse_y)

{
  iptr city3;
  iptr city;
  iptr tile_w;
  int city2;
  int rel_x;
  int rel_y;
  iptr city5;
  iptr city4;
  i32 flag_arg;
  int dy;
  iptr tile_idx;
  int dx;
  iptr quadrant;
  iptr i;
  
  tile_idx = -1;
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    flag_arg = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,flag_arg);
    tile_w = gfx_zoom_scale(0x40,city_wnd.map_zoom);
    city2 = gfx_zoom_scale(0x20,city_wnd.map_zoom);
    rel_x = ((CityWindow *)((i8 *)&city_wnd.window))->scale(5);
    rel_x = city_wnd.map_rect + rel_x;
    rel_y = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0xb);
    rel_x = mouse_x - rel_x;
    rel_y = mouse_y - (city_wnd.map_rect_top + rel_y + (city2 >> 1));
    if ((-1 < rel_x) && (((rel_x < tile_w * 4 && (-1 < rel_y)) && (rel_y < city2 * 4)))) {
      dx = (rel_x / tile_w) * 2 - 3;
      dy = (rel_y / city2) * 2 - 3;
      if ((rel_x % tile_w < 0) || (rel_y % city2 < 0)) {
        quadrant = 0;
      }
      else {
        city2 = ((DibSurface *)((i8 *)&city_tile_pick_surface))->get_pixel(rel_x % tile_w,rel_y % city2);
        quadrant = city2 - 10 >> 4;
      }
      if (quadrant != 0) {
        dx += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dx_table))[quadrant];
        dy += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dy_table))[quadrant];
      }
      for (i = 0; i < 0x15; i++) {
        if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] == dx) &&
           ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] == dy)) {
          tile_idx = i;
          break;
        }
      }
      if (-1 < tile_idx) {
        if (tile_idx == 0x14) {
          city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          cities[city2].worked_tiles = 0;
        }
        else {
          city = tile_idx;
          city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          city2 = city_tile_worked(city5,city);
          if (city2 == 0) {
            if ((((i8 *)&city_calc.tile_flags))[tile_idx] != '\0') {
              return;
            }
            if (city_calc.specialist_count == 0) {
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].worked_tiles = 0;
            }
            else {
              flag_arg = 1;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_set_tile_worked(city,tile_idx,flag_arg);
              flag_arg = 0xffffffff;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_adjust_specialists(city,flag_arg);
            }
          }
          else {
            flag_arg = 0;
            city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_tile_worked(city4,tile_idx,flag_arg);
            flag_arg = 1;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_adjust_specialists(city,flag_arg);
          }
        }
        ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(0);
      }
    }
  }
  return;
}

/* Part of CityWindow::draw_resource_map (005025d5): the terrain, units, cities and huts
   of the 21 tiles of the city radius. */
void CityWindow::draw_resource_tiles(int half_tile_w,int half_tile_h,int origin_x2,int origin_y2)
{
  i8 cursor_rect[16];
  i8 hut_rect[16];
  int x2;
  int tile_y;
  iptr unit;
  int tile_x;
  int city;
  int unit_y;
  iptr tile_idx;
  iptr j;
  iptr y2;
  iptr i;
  i32 drew_city;

  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((int)cities[city_index].x +
                          (int)(((i8 *)&city_resmap_draw_dx_table))[i]);
    y2 = (int)cities[city_index].y + (int)(((i8 *)&city_resmap_draw_dy_table))[i]
    ;
    tile_x = (((i8 *)&city_resmap_draw_dx_table))[i] * half_tile_w + origin_x2;
    tile_y = (((i8 *)&city_resmap_draw_dy_table))[i] * half_tile_h + origin_y2;
    unit_y = tile_y - half_tile_h;
    tile_idx = 0;
    for (j = 0; j < 0x15; j++) {
      if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[j] == (((i8 *)&city_resmap_draw_dx_table))[i]) &&
         ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[j] == (((i8 *)&city_resmap_draw_dy_table))[i])) {
        tile_idx = j;
        break;
      }
    }
    ((MapView *)((i8 *)&map_views_block.map_views))->draw_tile_terrain((iptr)this,tile_x,tile_y,x2,y2,
               (int)cities[city_index].owner,
               map_zoom_level,tile_idx);
    if (i < 0x15) {
      if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 8) == 0) {
        if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 4) == 0) {
          if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 2) != 0) {
            gfx_set_zoom_scale(map_zoom_level);
            ((Sprite *)((i8 *)&hut_sprite))->draw((iptr)hut_rect,(iptr)this,tile_x,tile_y
                      );
            gfx_reset_zoom_scale();
          }
        }
        else {
          unit = tile_first_unit(x2,y2);
          while ((-1 < unit && (unit_types[units[unit].type].attack == '\0'))) {
            unit = unit_next_in_stack(unit);
          }
          if (-1 < unit) {
            gfx_draw_unit((iptr)this,unit,7,tile_x,unit_y,
                               map_zoom_level,0);
          }
        }
      }
      else {
        city = city_at(x2,y2);
        if (-1 < city) {
          drew_city = 1;
          gfx_draw_city((iptr)this,city,1,tile_x,unit_y,
                             map_zoom_level);
        }
      }
      if ((((((i8 *)&city_calc.tile_flags))[tile_idx] & 0x10) != 0) && (((((i8 *)&city_calc.tile_flags))[tile_idx] & 9) == 0)) {
        gfx_set_zoom_scale(map_zoom_level);
        ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->draw((iptr)cursor_rect,(iptr)this,tile_x,tile_y);
        gfx_reset_zoom_scale();
      }
    }
  }
}

/* Part of CityWindow::draw_resource_map (005025d5): the food, shield and trade icons
   on each worked tile. */
void CityWindow::draw_resource_yields(int tile_w,int half_tile_w,int half_tile_h,int origin_x2,int origin_y2)
{
  int scaled;
  int scaled2;
  int scaled3;
  iptr zero_a;
  iptr zero_b;
  i8 unhappy_icon_rect[16];
  i8 yield_icon_rect[16];
  int x2;
  int no_yield;
  int tile_y;
  int tile_x;
  iptr yield_kind;
  int unit_y;
  iptr tile_idx;
  int icon_y;
  int per_row;
  int icon_x;
  int min_per_row;
  int spacing;
  int row_count;
  iptr j;
  iptr y2;
  iptr i;

  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((int)cities[city_index].x +
                          (int)(((i8 *)&city_resmap_draw_dx_table))[i]);
    y2 = (int)cities[city_index].y + (int)(((i8 *)&city_resmap_draw_dy_table))[i]
    ;
    tile_x = (((i8 *)&city_resmap_draw_dx_table))[i] * half_tile_w + origin_x2;
    tile_y = (((i8 *)&city_resmap_draw_dy_table))[i] * half_tile_h + origin_y2;
    unit_y = tile_y - half_tile_h;
    tile_idx = 0;
    for (j = 0; j < 0x15; j++) {
      if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[j] == (((i8 *)&city_resmap_draw_dx_table))[i]) &&
         ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[j] == (((i8 *)&city_resmap_draw_dy_table))[i])) {
        tile_idx = j;
        break;
      }
    }
    scaled = city_tile_worked(city_index,tile_idx);
    if (scaled != 0) {
      gfx_set_zoom_scale(view_scale * 4 - 8);
      no_yield = 0;
      city_calc_tile_yields(city_index,tile_idx,0);
      scaled = tile_w;
      per_row = city_calc.tile_shields + city_calc.tile_trade + city_calc.tile_yields[0];
      if (per_row == 0) {
        per_row = 1;
        no_yield = 1;
      }
      zero_b = 0;
      zero_a = 0;
      scaled2 = scale(8);
      scaled += scaled2 * -2;
      scaled2 = scale(10);
      spacing = ui_calc_item_spacing(per_row,scaled2 + 1,scaled,zero_a,zero_b);
      scaled = tile_w;
      scaled2 = scale(8);
      scaled3 = scale(10);
      min_per_row = (scaled + scaled2 * -2) / (scaled3 + 1);
      if (per_row <= min_per_row) {
        per_row = min_per_row;
      }
      yield_kind = 0;
      row_count = 0;
      icon_x = scale(8);
      icon_x = tile_x + icon_x;
      scaled2 = half_tile_h + tile_y;
      scaled = scale(10);
      icon_y = scaled2 - (scaled >> 1);
      for (; yield_kind < 3; yield_kind++) {
        while ((((i32 *)&city_calc.tile_yields))[yield_kind] != 0) {
          ((Sprite *)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + yield_kind * 0x3c))->draw((iptr)yield_icon_rect,(iptr)this
                     ,icon_x,icon_y);
          row_count++;
          if (row_count < per_row) {
            icon_x += spacing;
          }
          else {
            row_count = 0;
            icon_x = scale(8);
            icon_x = tile_x + icon_x;
            scaled = scale(0xb);
            icon_y += scaled;
          }
          (((i32 *)&city_calc.tile_yields))[yield_kind] = (((i32 *)&city_calc.tile_yields))[yield_kind] - 1;
        }
      }
      if (no_yield != 0) {
        ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->draw((iptr)unhappy_icon_rect,(iptr)this,icon_x,icon_y);
      }
      gfx_reset_zoom_scale();
    }
  }
}

/* Part of CityWindow::draw_resource_map (005025d5): the food bar: eaten, surplus or
   shortage, and its labels. */
void CityWindow::draw_resource_food(int panel_left,int panel_top2,uint big_zoom)
{
  int scaled;
  int scaled2;
  i32 bar_height;
  i32 color;
  uint food_surplus_text;
  uint food_surplus_count;
  uint abs_food_surplus;
  i8 food_surplus_rect[16];
  i8 food_rect[16];
  int x2;
  int left_width;
  int food_needed;
  int icon_count;
  int icon_w;
  int right_x;
  int food_eaten;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  iptr y2;
  int produced;
  iptr i;
  int food_need_shown;
  int food_fit_count;
  int avail_width;
  int bar_x;
  uint surplus;

  food_needed = (int)cities[city_index].size * (uint)cosmic.food_per_citizen +
             city_calc.settlers_supported * city_calc.settler_food_cost;
  produced = city_calc.yield_totals[0];
  surplus = city_calc.yield_totals[0] - food_needed;
  icon_count = food_needed;
  if (food_needed <= city_calc.yield_totals[0]) {
    icon_count = city_calc.yield_totals[0];
  }
  food_eaten = food_needed;
  if ((int)surplus < 0) {
    food_eaten = city_calc.yield_totals[0];
  }
  food_need_shown = food_needed;
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((((int)surplus < 0) && (produced != 0)) || ((0 < (int)surplus && (food_needed != 0)))) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    icon_count--;
  }
  spacing = ui_calc_item_spacing(icon_count,icon_w,avail_width,(iptr)&food_fit_count,0);
  while (food_fit_count < icon_count) {
    icon_count--;
    if (produced < food_need_shown) {
      food_need_shown--;
    }
    else if (food_need_shown == produced) {
      produced--;
      food_need_shown--;
      food_eaten--;
    }
    else {
      produced--;
      food_eaten--;
    }
  }
  if ((surplus == 0) || (produced != 0)) {
    if (surplus == 0) {
      right_width = 0;
    }
    else {
      if ((int)surplus < 1) {
        abs_food_surplus = ~surplus + 1;
      }
      else {
        abs_food_surplus = surplus;
      }
      scaled = scale(2);
      scaled2 = scale(1);
      right_width = scaled + scaled2 + (abs_food_surplus - 1) * spacing + icon_w;
    }
  }
  else {
    right_width = scale(0xe2);
  }
  left_width = scale(0xe2);
  left_width -= right_width;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  right_x = left_width + bar_x;
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0xe);
  y2 = panel_top2 + y2;
  if (view_scale == 3) {
    y2--;
  }
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x2d);
  }
  if (right_width != 0) {
    color = 0x30;
    bar_height = resource_bar_height;
    scaled = scale(6);
    gfx_target_fill_rect(right_x,y2,right_width + scaled,bar_height,color);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < food_eaten) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < food_eaten; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->draw((iptr)food_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if (surplus != 0) {
    x2 = right_x;
    if (food_eaten < 1) {
      scaled = scale(1);
      x2 += scaled;
    }
    else {
      scaled = scale(4);
      x2 += (scaled >> 1);
    }
    i = 0;
    while( true ) {
      if ((int)surplus < 1) {
        food_surplus_count = ~surplus + 1;
      }
      else {
        food_surplus_count = surplus;
      }
      if ((int)food_surplus_count <= i) break;
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + (uint)(0 < (int)surplus) * 0x3c))->draw(
                 (iptr)food_surplus_rect,(iptr)this,x2,y2);
      x2 += spacing;
      i++;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0xe);
  scaled = panel_top2 + scaled;
  y2 = scale(0xe);
  y2 = scaled - y2;
  text_clear();
  text_append_label(0x40);
  text_set_colors(0x2a,10,1,1);
  text_append_colon();
  text_append_int(city_calc.food_routes_out + food_eaten);
  if (city_calc.food_routes_in != city_calc.food_routes_out) {
    text_append_space();
    text_append_lparen();
    text_append_int(city_calc.food_routes_in - city_calc.food_routes_out);
    text_append_space();
    text_append_label(0xa9);
    text_append_rparen();
  }
  gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  text_clear();
  if ((int)surplus < 0) {
    text_append_label(0x4a);
    text_set_colors(0xb,0x1d,1,1);
  }
  else {
    text_append_label(0x41);
    text_set_colors(0x2e,10,1,1);
  }
  text_append_colon();
  if ((int)surplus < 1) {
    food_surplus_text = ~surplus + 1;
  }
  else {
    food_surplus_text = surplus;
  }
  text_append_int(food_surplus_text);
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the shield bar: support, waste,
   surplus or shortage, and its labels. */
void CityWindow::draw_resource_shields(int panel_left,int panel_top2,uint big_zoom,uint &surplus)
{
  int scaled;
  int scaled2;
  int scaled3;
  i32 bar_height;
  int scaled4;
  i32 color;
  uint shield_surplus_text;
  uint shield_surplus_count;
  uint abs_shield_surplus;
  i8 shortage_rect[16];
  i8 coin_rect[16];
  i8 waste_rect[16];
  i8 shield_rect[16];
  int x2;
  int left_width;
  int mid_width;
  int fit_count;
  int icon_w;
  int mid_x;
  int support_shown;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  iptr i;
  int lux_x;
  int avail_width;
  int shields_shown;
  int bar_x;

  support_shown = city_calc.shield_support;
  produced = city_calc.total_shields + city_calc.shield_waste;
  surplus = city_calc.total_shields - city_calc.shield_support;
  total_icons = city_calc.shield_support;
  if (city_calc.shield_support <= produced) {
    total_icons = produced;
  }
  shields_shown = produced;
  if (-1 < (int)surplus) {
    shields_shown = city_calc.shield_support;
  }
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((((int)surplus < 0) && (produced != 0)) || ((0 < (int)surplus && (city_calc.shield_support != 0)))) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  if (city_calc.shield_waste != 0) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,(iptr)&fit_count,0);
  while (fit_count < total_icons) {
    total_icons--;
    if (produced - city_calc.shield_waste < support_shown) {
      support_shown--;
    }
    else {
      if (produced == city_calc.shield_waste) break;
      if (produced - city_calc.shield_waste == support_shown) {
        produced--;
        support_shown--;
        shields_shown--;
      }
      else {
        produced--;
        shields_shown--;
      }
    }
  }
  mid_width = 0;
  if (city_calc.shield_waste != 0) {
    mid_width = (city_calc.shield_waste - 1) * spacing + icon_w;
    if (surplus != 0) {
      scaled = scale(4);
      mid_width += (scaled >> 1);
    }
    if (produced != city_calc.shield_waste) {
      scaled = scale(4);
      mid_width += (scaled >> 1);
    }
  }
  if ((surplus == 0) || (produced != 0)) {
    if (surplus == 0) {
      right_width = 0;
    }
    else {
      if ((int)surplus < 1) {
        abs_shield_surplus = ~surplus + 1;
      }
      else {
        abs_shield_surplus = surplus;
      }
      scaled = scale(2);
      scaled2 = scale(1);
      right_width = scaled + scaled2 + (abs_shield_surplus - 1) * spacing + icon_w;
    }
  }
  else {
    right_width = scale(0xe2);
  }
  left_width = scale(0xe2);
  left_width -= (mid_width + right_width);
  if (mid_width != 0) {
    if (city_calc.shield_support == 0) {
      left_width = 0;
    }
    else {
      scaled = scale(2);
      scaled2 = scale(1);
      left_width = scaled + scaled2 + (city_calc.shield_support - 1) * spacing + icon_w;
    }
    mid_width = scale(0xe2);
    mid_width -= (right_width + left_width);
  }
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  lux_x = left_width + bar_x;
  right_x = mid_width + lux_x;
  /* B31: also when the waste's part of the bar came out 0 wide: the original
     then draws the waste at an x it never set (every mode: ours would use
     another stale value; docs/BUGS.md) */
  mid_x = ((mid_width >> 1) + lux_x) - ((city_calc.shield_waste - 1) * spacing + icon_w >> 1);
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x78);
  y2 = panel_top2 + y2;
  if (view_scale == 3) {
    y2--;
  }
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x54);
  }
  if (mid_width != 0) {
    gfx_target_fill_rect(lux_x,y2,mid_width,resource_bar_height,0xb);
  }
  if (right_width != 0) {
    color = 0x5c;
    bar_height = resource_bar_height;
    scaled = scale(6);
    gfx_target_fill_rect(right_x,y2,right_width + scaled,bar_height,color);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < shields_shown) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < shields_shown; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if (0 < city_calc.shield_waste) {
    x2 = scale(2);
    x2 = mid_x + x2;
    for (i = 0; i < city_calc.shield_waste; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.shield_shortage_icon_sprite))->draw((iptr)waste_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if (surplus != 0) {
    x2 = right_x;
    if (shields_shown < 1) {
      scaled = scale(1);
      x2 += scaled;
    }
    else {
      scaled = scale(4);
      x2 += (scaled >> 1);
    }
    i = 0;
    while( true ) {
      if ((int)surplus < 1) {
        shield_surplus_count = ~surplus + 1;
      }
      else {
        shield_surplus_count = surplus;
      }
      if ((int)shield_surplus_count <= i) break;
      if (cities[city_index].building == -IMP_CAPITALIZATION) {
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)coin_rect,(iptr)this,x2,y2);
      }
      else {
        ((Sprite *)(((i8 *)&war_icon_sprite_block.shield_shortage_icon_sprite) + (uint)(0 < (int)surplus) * 0x3c))->draw(
                   (iptr)shortage_rect,(iptr)this,x2,y2);
      }
      x2 += spacing;
      i++;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0xe);
  scaled2 = scale(2);
  scaled3 = scale(0x78);
  scaled4 = scale(0xffffffff);
  y2 = scaled + scaled2 + scaled3 + scaled4 + panel_top2;
  text_clear();
  text_append_label(0xcc);
  text_append_colon();
  scaled = city_calc.shield_support;
  if (produced <= city_calc.shield_support) {
    scaled = produced;
  }
  text_append_int(scaled);
  text_set_colors(0x54,10,1,1);
  gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  if (city_calc.shield_waste != 0) {
    text_clear();
    text_append_label(0x43);
    text_append_colon();
    text_append_int(city_calc.shield_waste);
    text_set_colors(0xb,0x1d,1,1);
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  text_clear();
  if ((int)surplus < 0) {
    text_append_label(0x4b);
    text_set_colors(0xb,0x1d,1,1);
  }
  else {
    text_append_label(0x44);
    text_set_colors(0x5c,10,1,1);
  }
  text_append_colon();
  if ((int)surplus < 1) {
    shield_surplus_text = ~surplus + 1;
  }
  else {
    shield_surplus_text = surplus;
  }
  text_append_int(shield_surplus_text);
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the trade bar: trade and
   corruption, and its labels; surplus is the shield surplus
   draw_resource_shields left (the original reads it here). */
void CityWindow::draw_resource_trade(int panel_left,int panel_top2,uint big_zoom,uint surplus)
{
  int scaled;
  int scaled2;
  uint abs_corruption;
  i8 corruption_rect[16];
  i8 trade_rect[16];
  int x2;
  int left_width;
  int fit_count;
  int text_end_x;
  int icon_w;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  iptr i;
  int avail_width;
  int bar_x;

  produced = city_calc.total_trade;
  total_icons = city_calc.total_trade;
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,(iptr)&fit_count,0);
  for (; fit_count < total_icons; total_icons--) {
    produced--;
  }
  right_width = 0;
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    if ((int)(*(uint *)&city_calc.trade_corruption) < 1) {
      abs_corruption = ~(*(uint *)&city_calc.trade_corruption) + 1;
    }
    else {
      abs_corruption = (*(uint *)&city_calc.trade_corruption);
    }
    scaled = scale(2);
    scaled2 = scale(1);
    right_width = scaled + scaled2 + (abs_corruption - 1) * spacing + icon_w;
  }
  left_width = scale(0xe2);
  left_width -= right_width;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  right_x = left_width + bar_x;
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x37);
  y2 = panel_top2 + y2;
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x76);
  }
  if (right_width != 0) {
    gfx_target_fill_rect(right_x,y2,right_width,resource_bar_height,0x79);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < (int)(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption))) {
    x2 = bar_x + 1;
    for (i = 0; i < (int)(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption)); i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->draw((iptr)trade_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    x2 = right_x;
    scaled = scale(4);
    x2 += (scaled >> 1);
    for (i = 0; i < (int)(*(uint *)&city_calc.trade_corruption); i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.trade_corruption_icon_sprite))->draw((iptr)corruption_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0x37);
  scaled = panel_top2 + scaled;
  y2 = scale(0xe);
  y2 = scaled - y2;
  text_clear();
  text_append_label(0x45);
  text_set_colors(0x76,10,1,1);
  text_append_colon();
  text_append_int(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption));
  text_end_x = gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  if ((civs[cities[city_index].owner].government == GOV_FUNDAMENTALISM) &&
     (city_calc.building_content != 0)) {
    scaled = scale(4);
    text_end_x += scaled;
    text_clear();
    text_append_label(0x5d);
    text_append_colon();
    text_append((const char *)&str_empty_144);
    text_append_int(city_calc.building_content);
    text_set_colors(0x79,10,1,1);
    gfx_draw_text((const char *)&textbuf.buf,text_end_x,y2);
  }
  text_clear();
  text_append_label(0x46);
  text_set_colors(0x79,10,1,1);
  text_append_colon();
  text_append_int((*(uint *)&city_calc.trade_corruption));
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the tax, luxury and science bar
   and its labels. */
void CityWindow::draw_resource_rates(int panel_left,int panel_top2,uint big_zoom)
{
  int scaled;
  int scaled2;
  int scaled3;
  i32 bar_height;
  int scaled4;
  i32 color;
  i32 fill_color;
  i8 science_rect[16];
  i8 lux_rect[16];
  i8 tax_rect[16];
  int x2;
  iptr civ;
  int left_width;
  int mid_width;
  int fit_count;
  int icon_w;
  int mid_x;
  int tax_icons;
  int lux_align;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int lux_icons;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  int sci_icons;
  iptr i;
  int lux_x;
  int avail_width;
  int nonzero_kinds;
  int bar_x;

  produced = city_calc.total_trade;
  tax_icons = city_calc.tax_total;
  lux_icons = city_calc.lux_total;
  sci_icons = city_calc.science_total;
  total_icons = city_calc.science_total + city_calc.lux_total + city_calc.tax_total;
  nonzero_kinds = (uint)(0 < city_calc.science_total) + (uint)(0 < city_calc.lux_total) + (uint)(0 < city_calc.tax_total);
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  for (; 1 < nonzero_kinds; nonzero_kinds--) {
    scaled = scale(4);
    avail_width -= (icon_w + scaled);
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,(iptr)&fit_count,0);
  for (; fit_count < total_icons; fit_count++) {
    if ((lux_icons < tax_icons) && (sci_icons < tax_icons)) {
      tax_icons--;
    }
    else if (lux_icons < sci_icons) {
      sci_icons--;
    }
    else {
      lux_icons--;
    }
  }
  left_width = 0;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  if (tax_icons != 0) {
    scaled = scale(1);
    left_width = scaled + (tax_icons - 1) * spacing + icon_w;
  }
  right_width = 0;
  if (sci_icons != 0) {
    scaled = scale(1);
    right_width = scaled + (sci_icons - 1) * spacing + icon_w;
  }
  scaled = scale(0xe2);
  scaled2 = scale(0xcd);
  right_x = (scaled + scaled2 + panel_left) - right_width;
  mid_width = scale(0xe2);
  mid_width -= (right_width + left_width);
  lux_x = scale(0xcd);
  lux_x = panel_left + lux_x;
  if (left_width != 0) {
    scaled = scale(4);
    mid_width -= scaled;
    scaled = scale(4);
    lux_x = lux_x + left_width + scaled;
  }
  if (right_width != 0) {
    scaled = scale(4);
    mid_width -= scaled;
  }
  mid_x = lux_x;
  lux_x += (mid_width >> 1);
  if (lux_icons != 0) {
    lux_x -= ((lux_icons - 1) * spacing + icon_w >> 1);
  }
  lux_align = 0;
  if ((lux_icons == 0) || (tax_icons != 0)) {
    if ((lux_icons != 0) && (sci_icons == 0)) {
      lux_align = 2;
      scaled = scale(1);
      nonzero_kinds = scaled + (lux_icons - 1) * spacing + icon_w;
      scaled = scale(0xe2);
      scaled2 = scale(0xcd);
      lux_x = (scaled + scaled2 + panel_left) - nonzero_kinds;
    }
  }
  else {
    lux_align = 1;
    lux_x = bar_x;
  }
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x4f);
  y2 = panel_top2 + y2;
  fill_color = 0x79;
  bar_height = resource_bar_height;
  color = scale(0xe2);
  gfx_target_fill_rect(bar_x,y2,color,bar_height,fill_color);
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < tax_icons) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < tax_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)tax_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if (lux_icons != 0) {
    x2 = lux_x;
    for (i = 0; i < lux_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites))->draw((iptr)lux_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  if (sci_icons != 0) {
    x2 = right_x;
    for (i = 0; i < sci_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((iptr)science_rect,(iptr)this,x2,y2);
      x2 += spacing;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0x4f);
  scaled2 = scale(0xe);
  scaled3 = scale(2);
  scaled4 = scale(0);
  y2 = scaled + scaled2 + scaled3 + scaled4 + panel_top2;
  civ = (iptr)cities[city_index].owner;
  text_clear();
  text_append_int((uint)civs[civ].tax_rate * 10);
  text_append_percent();
  text_append_space();
  text_append_label(0x47);
  text_append_colon();
  text_append_int(city_calc.tax_total);
  text_set_colors(0x76,10,1,1);
  if (lux_align == 1) {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  else {
    gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  }
  text_clear();
  text_append_int((10 - ((uint)civs[civ].tax_rate + (uint)civs[civ].science_rate)) *
                     10);
  text_append_percent();
  text_append_space();
  text_append_label(0x48);
  text_append_colon();
  text_append_int(city_calc.lux_total);
  text_set_colors(0x29,10,1,1);
  if (lux_align == 1) {
    gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  }
  else if (lux_align == 2) {
    gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
  }
  else {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  text_clear();
  text_append_int((uint)civs[civ].science_rate * 10);
  text_append_percent();
  text_append_space();
  text_append_label(0x49);
  text_append_colon();
  text_append_int(city_calc.science_total);
  text_set_colors(0x5e,10,1,1);
  if (lux_align == 2) {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  else {
    gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
  }
}

/* 005025d5 auto */
void CityWindow::draw_resource_map(int blit)

{
  u32 text_bottom;
  int scaled;
  i32 bar_height;
  int tile_h;
  int tile_w;
  int origin_x2;
  int map_top;
  int panel_top2;
  int panel_left;
  int origin_y2;
  int half_tile_w;
  int map_left;
  int half_tile_h;
  uint big_zoom;
  i8 map_rect[16];
  uint surplus;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)(&resource_map_rect));
    panel_left = resource_map_rect;
    panel_top2 = resource_map_rect_top;
    tile_w = gfx_zoom_scale(0x40,map_zoom_level);
    tile_h = gfx_zoom_scale(0x20,map_zoom_level);
    half_tile_w = tile_w >> 1;
    half_tile_h = tile_h >> 1;
    map_left = scale(5);
    map_left = panel_left + map_left;
    map_top = scale(0xb);
    map_top = panel_top2 + map_top;
    clickmap.remove_by_id(1);
    rect_set_xywh((iptr)map_rect,map_left,half_tile_h + map_top,tile_w << 2,tile_h << 2);
    clickmap.hotspot_add_rect(0,1,(RECT *)map_rect);
    origin_x2 = half_tile_w * 3 + map_left;
    origin_y2 = half_tile_h * 4 + map_top;
    draw_resource_tiles(half_tile_w,half_tile_h,origin_x2,origin_y2);
    draw_resource_yields(tile_w,half_tile_w,half_tile_h,origin_x2,origin_y2);
    text_set_font((iptr)&city_font);
    gfx_set_zoom_scale(view_scale * 4 - 8);
    text_clear();
    text_append_label(0x1c2);
    text_set_colors(0x7c,0x39,1,1);
    bar_height = rect_width((RECT *)&resource_label_rect);
    text_bottom = resource_label_rect_bottom;
    scaled = scale(0xe);
    gfx_draw_text_centered((const char *)&textbuf.buf,resource_label_rect,text_bottom - scaled,bar_height);
    text_set_colors(0x29,10,1,1);
    big_zoom = (uint)(view_scale == 3);
    draw_resource_food(panel_left,panel_top2,big_zoom);
    draw_resource_shields(panel_left,panel_top2,big_zoom,surplus);
    draw_resource_trade(panel_left,panel_top2,big_zoom,surplus);
    draw_resource_rates(panel_left,panel_top2,big_zoom);
    gfx_reset_zoom_scale();
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)(&resource_map_rect));
    }
  }
  return;
}

/* 00504c05 auto */
void CityWindow::draw_food_box(int blit)

{
  i32 width;
  iptr box_w;
  iptr box_h;
  i32 arg_y;
  i32 arg_x;
  int icon_x;
  int row_spacing;
  int extent;
  int row_y;
  int food_left;
  int row;
  RECT box_rect;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)(&food_box_rect));
    text_set_colors(0x2c,10,1,1);
    width = rect_width((RECT *)(iptr)(&food_box_rect));
    arg_y = food_box_rect_top;
    arg_x = food_box_rect;
    box_w = (iptr)label_string(label_table.string_ids[0x2b]);
    gfx_draw_text_centered((const char *)box_w,arg_x,arg_y,width);
    box_w = scale(0xb7);
    box_h = scale(0x93);
    arg_y = scale(0xf);
    arg_x = scale(6);
    rect_set_xywh((iptr)&box_rect.left,arg_x,arg_y,box_w,box_h);
    rect_offset((iptr)&box_rect.left,food_box_rect,
                       food_box_rect_top);
    arg_y = scale(3);
    arg_x = scale(3);
    rect_deflate((iptr)&box_rect.left,arg_x,arg_y);
    box_h = 0;
    box_w = 0;
    arg_y = rect_width((RECT *)&box_rect.left);
    arg_x = scale(0x11);
    icon_x = ui_calc_item_spacing(cities[city_index].size + 1,arg_x,arg_y,box_w
                               ,box_h);
    box_h = 0;
    box_w = 0;
    arg_y = rect_height((RECT *)&box_rect.left);
    arg_x = scale(0xe);
    row_spacing = ui_calc_item_spacing(city_calc.food_box_rows,arg_x,arg_y,box_w,box_h);
    icon_x = (cities[city_index].size + 1) * icon_x;
    extent = rect_width((RECT *)&box_rect.left);
    if (icon_x < extent) {
      arg_y = 0;
      extent = rect_width((RECT *)&box_rect.left);
      rect_deflate((iptr)&box_rect.left,extent - icon_x >> 1,arg_y);
    }
    icon_x = row_spacing * city_calc.food_box_rows;
    extent = rect_height((RECT *)&box_rect.left);
    if (icon_x < extent) {
      extent = rect_height((RECT *)&box_rect.left);
      box_rect.bottom -= (extent - icon_x);
    }
    rect_inflate((iptr)&box_rect.left,3,3);
    city_draw_bevel_rect((iptr)&box_rect.left,0x2c,0x39);
    rect_deflate((iptr)&box_rect.left,1,1);
    rect_deflate((iptr)&box_rect.left,2,2);
    icon_x = box_rect.left;
    row = 0;
    food_left = (int)cities[city_index].food_box;
    row_y = box_rect.top;
    gfx_set_zoom_scale(view_scale * 4 - 8);
    for (; (0 < food_left && (row < city_calc.food_box_rows)); row++) {
      extent = cities[city_index].size + 1;
      if (food_left <= extent) {
        extent = food_left;
      }
      width = 1;
      arg_y = rect_width((RECT *)&box_rect.left);
      arg_x = scale(0x11);
      city_window_draw_icon_row((iptr)&war_icon_sprite_block.resource_icon_sprites,icon_x,row_y,extent,
                         cities[city_index].size + 1,arg_x,arg_y,width);
      food_left -= extent;
      row_y += row_spacing;
    }
    gfx_reset_zoom_scale();
    icon_x = city_has_improvement(city_index,IMP_GRANARY);
    if ((icon_x != 0) ||
       (icon_x = civ_has_wonder((int)cities[city_index].owner,WONDER_PYRAMIDS),
       icon_x != 0)) {
      extent = city_calc.food_box_rows >> 1;
      icon_x = scale(1);
      row_spacing = box_rect.top + (extent * row_spacing - icon_x);
      arg_y = 0x2a;
      icon_x = scale(2);
      icon_x = (box_rect.right - 1) - icon_x;
      extent = scale(2);
      city_draw_hline(box_rect.left + extent,icon_x,row_spacing,arg_y);
    }
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)(&food_box_rect));
    }
  }
  return;
}

/* 0050503e auto */
void CityWindow::draw_production(int blit)

{
  int scaled;
  int scaled_x;
  i32 arg_y;
  iptr arg_w;
  int *fit_out;
  i32 arg_x;
  iptr arg_h;
  i32 width;
  i8 icon_rect[16];
  int row_spacing;
  int fit_count;
  int col_spacing;
  iptr box_h;
  iptr unit;
  i32 zoom2;
  iptr improvement;
  int pad_x2;
  iptr box_w;
  iptr unit_type;
  uint build_cost;
  int row_y;
  uint per_row;
  i32 row_x;
  uint shields_left;
  uint row_icons;
  int row;
  int row_width;
  struct { i32 left; int top; i32 right; u32 bottom; } box_rect;
  
  improvement = -1;
  unit_type = -1;
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)(&production_rect));
    box_w = scale(0xb7);
    box_h = scale(0x92);
    box_rect.left = production_rect;
    box_rect.top = production_rect_top;
    box_rect.right = production_rect_right;
    box_rect.bottom = production_rect_bottom;
    scaled = scale(0x28);
    box_rect.bottom = box_rect.top + scaled;
    clickmap.remove_by_id(5);
    clickmap.hotspot_add_rect(0,5,(RECT *)&box_rect.left);
    if (cities[city_index].building < '\0') {
      if (cities[city_index].building < 1) {
        improvement = ~(int)cities[city_index].building + 1;
      }
      else {
        improvement = (iptr)cities[city_index].building;
      }
      build_cost = (uint)improvements[improvement].cost;
      text_set_colors(0x54,10,1,1);
      width = rect_width((RECT *)(iptr)(&production_rect));
      arg_y = production_rect_top;
      arg_x = production_rect;
      arg_w = (iptr)label_string(improvements[improvement].name);
      gfx_draw_text_centered((const char *)arg_w,arg_x,arg_y,width);
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(0x10);
      scaled = production_rect_top + scaled;
      scaled_x = scale(0x50);
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)icon_rect,(iptr)&city_wnd.window,
                 production_rect + scaled_x,scaled);
      gfx_reset_zoom_scale();
    }
    else {
      unit_type = (iptr)cities[city_index].building;
      build_cost = (uint)(char)unit_types[unit_type].cost;
      unit = unit_make_dummy(unit_type,(int)cities[city_index].owner);
      zoom2 = 0xffffffff;
      if (view_scale == 1) {
        zoom2 = 0xfffffffb;
      }
      if (view_scale == 3) {
        zoom2 = 1;
      }
      arg_x = 0;
      arg_y = zoom2;
      scaled = scale(1);
      scaled = production_rect_top + scaled;
      scaled_x = scale(0x49);
      gfx_draw_unit((iptr)&city_wnd.window,unit,0,production_rect + scaled_x,scaled,arg_y,
                         arg_x);
    }
    if (cities[city_index].building != -IMP_CAPITALIZATION) {
      arg_w = box_w;
      arg_h = box_h;
      arg_y = scale(0x28);
      arg_x = scale(6);
      rect_set_xywh((iptr)&box_rect.left,arg_x,arg_y,arg_w,arg_h);
      rect_offset((iptr)&box_rect.left,production_rect,
                         production_rect_top);
      arg_y = scale(3);
      arg_x = scale(3);
      rect_deflate((iptr)&box_rect.left,arg_x,arg_y);
      arg_h = 0;
      arg_w = 0;
      arg_y = rect_height((RECT *)&box_rect.left);
      arg_x = scale(0xe);
      row_spacing = ui_calc_item_spacing(10,arg_x,arg_y,arg_w,arg_h);
      per_row = build_cost;
      if ((int)build_cost < 10) {
        per_row = (*(uint *)&city_calc.shield_rows);
        box_rect.bottom -= (10 - build_cost) * row_spacing;
      }
      arg_w = 0;
      fit_out = &fit_count;
      arg_y = rect_width((RECT *)&box_rect.left);
      arg_x = scale(0x11);
      col_spacing = ui_calc_item_spacing((*(uint *)&city_calc.shield_rows),arg_x,arg_y,(iptr)fit_out,arg_w);
      row_width = col_spacing * (*(uint *)&city_calc.shield_rows) + fit_count;
      pad_x2 = 0;
      scaled = rect_width((RECT *)&box_rect.left);
      if (row_width < scaled) {
        scaled = rect_width((RECT *)&box_rect.left);
        pad_x2 = scaled - row_width >> 1;
      }
      rect_inflate((iptr)&box_rect.left,3,3);
      city_draw_bevel_rect((iptr)&box_rect.left,0x51,0x5d);
      rect_deflate((iptr)&box_rect.left,1,1);
      rect_deflate((iptr)&box_rect.left,pad_x2 + 2,2);
      row = 0;
      shields_left = (uint)cities[city_index].shield_box;
      row_x = box_rect.left;
      row_y = box_rect.top;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      for (; (0 < (int)shields_left && (row < 10)); row++) {
        row_icons = shields_left;
        if ((int)per_row <= (int)shields_left) {
          row_icons = per_row;
        }
        width = 1;
        arg_y = rect_width((RECT *)&box_rect.left);
        arg_x = scale(0x11);
        city_window_draw_icon_row((iptr)&war_icon_sprite_block.shield_icon_sprite,row_x,row_y,row_icons,per_row,arg_x,arg_y,width);
        shields_left -= row_icons;
        row_y += row_spacing;
      }
      gfx_reset_zoom_scale();
    }
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)(&production_rect));
    }
  }
  return;
}

/* 005055dd auto */
void CityWindow::draw_button_area(int blit)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) &&
     ((-1 < city_index &&
      (begin_draw((iptr)button_area_rect), blit != 0)))) {
    wnd_blit_surface_rect((iptr)button_area_rect);
  }
  return;
}

/* 00505666 auto */
void CityWindow::draw_supported_units(int blit)

{
  int scaled;
  int used;
  int pad;
  i32 width;
  iptr size_arg;
  iptr zero_a;
  iptr zero_b;
  i8 suppressed_rect[16];
  i8 unhappy_rect[16];
  i8 shield_rect[16];
  i8 food_rect[16];
  int icon_x;
  iptr civ;
  int cell_h;
  int cell_y;
  iptr unit;
  int cell_x;
  int upkeep_icons;
  uint gov_type;
  int col;
  int margin_x;
  int margin_y;
  u32 area_top;
  int icon_spacing;
  u32 area_left;
  int cell_w;
  int unhappy_cost;
  int unit_count;
  int icon_offset_y;
  int icon_y;
  int row;
  int military_away;
  int unhappy_kind;
  int max_rows;
  int i;
  i32 zoom2;
  iptr unit_size;
  int max_cols;
  i8 hotspot_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index) &&
      (extra_ui(X_SUPPORT_LIST) || extra_ui(X_SUPPORT_SORT))) {
    extras_draw_supported_units(this,blit);   /* X11, X16 */
    return;
  }
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)&supported_units_rect);
    area_left = supported_units_rect;
    area_top = supported_units_rect_top;
    zoom2 = 0xfffffffd;
    if (view_scale == 1) {
      zoom2 = 0xfffffffa;
    }
    if (view_scale == 3) {
      zoom2 = 0;
    }
    cell_w = gfx_zoom_scale(0x45,zoom2);
    cell_h = gfx_zoom_scale(0x34,zoom2);
    max_cols = scale(0xc0);
    max_cols = max_cols / cell_w;
    max_rows = scale(0x4e);
    max_rows = max_rows / cell_h;
    scaled = scale(0xc0);
    used = cell_w * max_cols;
    pad = gfx_zoom_scale(5,zoom2);
    margin_x = (scaled - used) + pad >> 1;
    scaled = scale(0x4e);
    used = cell_h * max_rows;
    pad = gfx_zoom_scale(4,zoom2);
    margin_y = (scaled - used) + pad >> 1;
    icon_offset_y = gfx_zoom_scale(0x20,zoom2);
    unit_count = 0;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         ((uint)(byte)units[unit].home_city == (uint)city_index)) {
        unit_count++;
      }
    }
    if (unit_count <= max_cols) {
      scaled = scale(0x4e);
      used = gfx_zoom_scale(0x30,zoom2);
      margin_y = scaled - used >> 1;
      text_clear();
      text_append_label(0x1bf);
      text_set_colors(0x7c,0x12,1,1);
      width = rect_width((RECT *)(iptr)((u8 *)&supported_units_label_rect));
      scaled = scale(1);
      gfx_draw_text_centered((const char *)&textbuf.buf,supported_units_label_rect,
                         supported_units_label_rect_top + scaled,width);
    }
    unit_size = gfx_zoom_scale(0x40,zoom2);
    cell_x = margin_x + area_left;
    cell_y = margin_y + area_top;
    row = 0;
    col = 0;
    civ = (iptr)cities[city_index].owner;
    gov_type = (uint)civs[civ].government;
    military_away = 0;
    clickmap.remove_by_id(6);
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         ((uint)(byte)units[unit].home_city == (uint)city_index)) {
        icon_y = icon_offset_y + cell_y;
        gfx_draw_unit((iptr)this,unit,4,cell_x,cell_y,zoom2,0);
        upkeep_icons = 0;
        unhappy_kind = 0;
        if (unit_types[units[unit].type].role == ROLE_SETTLE) {
          upkeep_icons = city_calc.settler_food_cost;
        }
        if ((units[unit].flags & UNITF_COSTS_SHIELD) != 0) {
          upkeep_icons++;
        }
        if (((units[unit].flags & UNITF_AGGRESSIVE) != 0) && (GOV_FUNDAMENTALISM < (int)gov_type)) {
          scaled = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
          if ((scaled == 0) &&
             (scaled = city_has_improvement(city_index,IMP_POLICE_STATION), scaled == 0)) {
            unhappy_cost = 1;
          }
          else {
            unhappy_cost = 0;
          }
          if (gov_type == GOV_DEMOCRACY) {
            unhappy_cost++;
          }
          else if ((unhappy_cost != 0) && (military_away == 0)) {
            unhappy_cost = 0;
          }
          if (unhappy_cost == 0) {
            unhappy_cost = 1;
            unhappy_kind = 2;
          }
          else {
            unhappy_kind = 1;
          }
          upkeep_icons += unhappy_cost;
          military_away++;
        }
        gfx_set_zoom_scale(view_scale * 4 - 8);
        zero_b = 0;
        zero_a = 0;
        size_arg = unit_size;
        width = scale(10);
        icon_spacing = ui_calc_item_spacing(upkeep_icons,width,size_arg,zero_a,zero_b);
        icon_x = cell_x;
        if (unit_types[units[unit].type].role == ROLE_SETTLE) {
          for (i = 0; i < city_calc.settler_food_cost; i++) {
            ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites))->draw((iptr)food_rect,(iptr)this,icon_x,icon_y);
            icon_x += icon_spacing;
          }
        }
        if ((units[unit].flags & UNITF_COSTS_SHIELD) != 0) {
          ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_shield_icon_sprite))->draw((iptr)shield_rect,(iptr)this,icon_x,icon_y);
          icon_x += icon_spacing;
        }
        if (unhappy_kind != 0) {
          for (i = 0; i < unhappy_cost; i++) {
            if (unhappy_kind == 1) {
              ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->draw((iptr)unhappy_rect,(iptr)this,icon_x,icon_y)
              ;
            }
            else {
              ((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite))->draw((iptr)suppressed_rect,(iptr)this,icon_x,icon_y)
              ;
            }
            icon_x += icon_spacing;
          }
        }
        gfx_reset_zoom_scale();
        rect_set_xywh((iptr)hotspot_rect,cell_x,cell_y,unit_size,unit_size);
        clickmap.hotspot_add_rect(unit,6,(RECT *)hotspot_rect);
        col++;
        cell_x += cell_w;
        if (max_cols <= col) {
          col = 0;
          cell_x = margin_x + area_left;
          row++;
          cell_y += cell_h;
          if (max_rows <= row) break;
        }
      }
    }
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)&supported_units_rect);
    }
  }
  return;
}

/* 00505d3d auto */
void __cdecl city_window_sell_improvement(i32 list_row)

{
  short list_top;
  iptr city3;
  iptr city4;
  iptr city2;
  int city;
  int sale_price;
  i32 on;
  iptr imp_arg;
  iptr imp_idx;
  iptr improvement;
  int owned_count;
  
  owned_count = 0;
  improvement = -1;
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if (((cities[city4].flags & CITYF_IMPROVEMENT_SOLD) == 0) ||
       ((game.reveal_map != '\0' && ((game.game_flags & GAME_SCENARIO) != 0)))) {
      on = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,on);
      list_top = (short)city_wnd.improvement_list_top;
      for (imp_idx = 1; imp_idx < 0x27; imp_idx++) {
        imp_arg = imp_idx;
        city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        city = city_has_improvement(city2,imp_arg);
        if ((city != 0) && (owned_count = owned_count + 1, (short)(list_top + (short)list_row + 1) == owned_count))
        {
          improvement = imp_idx;
          break;
        }
      }
      if (-1 < improvement) {
        if ((imp_idx == 1) && ((game.reveal_map == '\0' || ((game.game_flags & GAME_SCENARIO) == 0)))) {
          snd_play_effect(SND_NEG1,0,0,0);
          popup_game_with_improvement((const char *)&str_canthockthis,IMP_PALACE,8);
        }
        else {
          sale_price = (uint)improvements[improvement].cost * (uint)cosmic.shield_box_rows;
          text_set_label_arg(0,improvements[improvement].name);
          text_set_number_arg(0,sale_price);
          city = popup_game_with_improvement((const char *)&str_hockthis,improvement,8);
          if (city == 0) {
            snd_play_effect(SND_SELL,0,0,0);
            on = 0;
            city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_improvement(city2,improvement,on);
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            civs[cities[city].owner].gold = civs[cities[city].owner].gold + sale_price;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            cities[city].flags = cities[city].flags | CITYF_IMPROVEMENT_SOLD;
            ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
            status_draw_info(1);
            advisor_refresh_if_open(5);
            advisor_refresh_if_open(4);
            advisor_refresh_if_open(6);
          }
        }
      }
    }
    else {
      snd_play_effect(SND_NEG1,0,0,0);
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      popup_game_with_city((const char *)&str_alreadysold,city2);
    }
  }
  return;
}

/* 00505ffa auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void CityWindow::draw_improvements(int blit)

{
  u32 list_right;
  i32 width;
  int scaled;
  int scaled2;
  iptr row_h;
  i8 coin_rect[16];
  i8 imp_rect[16];
  i8 wonder_rect[16];
  int has_it;
  int row_y;
  iptr improvement;
  int row_x;
  int shown_count;
  uptr list_top;
  u32 list_left;
  int name_y;
  int text_x2;
  int wonder_idx;
  int visible_rows;
  int row;
  int zoom2;
  i8 row_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)(u8 *)&improvements_rect);
    clickmap.remove_by_id(4);
    list_left = improvement_list_rect;
    list_top = improvement_list_rect_top;
    text_clear();
    text_append_label(0x1c0);
    text_set_colors(0x7c,0x12,1,1);
    width = rect_width((RECT *)(iptr)(u8 *)&improvements_rect);
    scaled2 = improvements_rect_top;
    scaled = scale(1);
    gfx_draw_text_centered((const char *)&textbuf.buf,*(i32 *)(u8 *)&improvements_rect,scaled2 - scaled,width);
    row_x = scale(2);
    row_x = list_left + row_x;
    row_y = scale(1);
    row_y = list_top + row_y;
    scaled2 = rect_height((RECT *)&improvement_list_rect);
    scaled = scale(1);
    visible_rows = scale(0xc);
    visible_rows = (scaled2 - (scaled >> 1)) / visible_rows;
    zoom2 = view_scale * 2 - 8;
    for (row = 0; row < visible_rows; row++) {
      row_h = scale(0xc);
      scaled2 = scale(0xc0);
      scaled = scale(4);
      rect_set_xywh((iptr)row_rect,row_x,row_y,scaled2 - scaled,row_h);
      clickmap.hotspot_add_rect(row,4,(RECT *)row_rect);
      scaled2 = scale(0xc);
      row_y += scaled2;
    }
    row_y = list_top + 1;
    shown_count = 0;
    for (improvement = 0; improvement < 0x43; improvement++) {
      if (improvement < 0x27) {
        has_it = city_has_improvement(city_index,improvement);
        wonder_idx = -1;
      }
      else {
        wonder_idx = improvement - 0x27;
        if ((int)game.wonder_city[improvement - 0x27] == city_index) {
          has_it = 1;
        }
        else {
          has_it = 0;
        }
      }
      if (((has_it != 0) &&
          (shown_count = shown_count + 1, improvement_scroll_pos + 1 <= shown_count)) &&
         (shown_count <= improvement_scroll_pos + visible_rows)) {
        gfx_set_zoom_scale(zoom2);
        if (wonder_idx < 0) {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)imp_rect,(iptr)this,
                     row_x,row_y);
        }
        else {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder_idx * 0x3c))->draw((iptr)wonder_rect,(iptr)this,
                     row_x,row_y);
        }
        gfx_reset_zoom_scale();
        text_x2 = scale(0x18);
        text_x2 = row_x + text_x2;
        name_y = row_y - 1;
        text_clear();
        text_append_string_id(improvements[improvement].name);
        text_set_colors(0x29,10,1,0);
        gfx_draw_text((const char *)&textbuf.buf,text_x2,name_y - 1);
        list_right = improvement_list_rect_right;
        scaled2 = scale(0xe);
        text_x2 = scale(4);
        text_x2 = (list_right - scaled2) - text_x2;
        if (wonder_idx < 0) {
          gfx_set_zoom_scale(view_scale * 3 - 7);
          ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)coin_rect,(iptr)this,text_x2,name_y);
          gfx_reset_zoom_scale();
        }
        scaled2 = scale(0xc);
        row_y += scaled2;
      }
    }
    city_wnd.improvement_list_count = shown_count;
    improvements_scrollbar->set_range(0,shown_count - 1);
    improvements_scrollbar->set_page_size(visible_rows - 1);
    improvements_scrollbar->set_pos(improvement_scroll_pos);
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)(u8 *)&improvements_rect);
    }
  }
  return;
}

/* 00506637 auto */
void __cdecl city_window_supported_unit_menu(iptr unit)

{
  iptr city3;
  iptr city2;
  iptr choice;
  int city;
  int window_city;
  iptr option_id;
  i32 arg;
  Dialog dialog2;
  Sprite sprite;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (city_wnd_state.unit_dialog_busy == 0) {
    city_wnd_state.unit_dialog_busy = 1;
    city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
      arg = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,arg);
      dlg_unit_sprite_zoom = 8;
      sprite.create_solid(0x40,0x20,0);
      text_set_unit_info_args(unit);
      dialog2.load_game_section((const char *)&str_childclick,1);
      dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)unit_dlg_draw_sprite_cb);
      dialog2.set_image_zoom(8);
      dialog2.add_sprite((iptr)&sprite,unit,0);
      arg = 0;
      option_id = 4;
      city2 = (iptr)label_string(label_table.string_ids[0x132]);
      dialog2.add_option((const char *)city2,option_id,arg);
      choice = dialog2.show_modal(0);
      if (0 < choice) {
        if (choice == 3) {
          units[unit].orders = ORDER_GOTO;
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          units[unit].goto_x = cities[city].x;
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          units[unit].goto_y = cities[city].y;
          if (bugfix_state()) {
            units[unit].aux_timer = '\0';  /* B9: the long move count starts again (docs/BUGS.md) */
          }
        }
        else if (choice == 4) {
          city = city_at((int)units[unit].x,(int)units[unit].y);
          if (-1 < city) {
            cities[city].shield_box =
                 (short)((int)((int)(char)unit_types[units[unit].type].cost *
                              (uint)cosmic.shield_box_rows) / 2) + cities[city].shield_box;
            window_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            if (window_city == city) {
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_save_build_state(city2);
            }
          }
          unit_destroy_redraw(unit,1);
          ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        }
        else {
          city = map_in_bounds((int)units[unit].x,(int)units[unit].y);
          if (city != 0) {
            city = unit_is_selectable(unit);
            if (city == 0) {
              ui_redraw_all_maps(0);
              map_set_cursor_pos((int)units[unit].x,(int)units[unit].y);
              ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y
                        );
            }
            else {
              game.active_unit = (i16)unit;
              units[unit].orders = ORDER_NONE;
              moving_units_mode = 0;
              ui_focus_active_unit(0);
            }
            if (choice == 2) {
              city_window_close();
            }
          }
        }
      }
    }
    city_wnd_state.unit_dialog_busy = 0;
  }
  return;
}

/* 00506a42 auto */
void __cdecl city_window_present_unit_menu(int stack_idx)

{
  iptr city3;
  iptr city;
  iptr nth;
  iptr city4;
  iptr y;
  iptr city5;
  int unit;
  uint city6;
  int city2;
  iptr option_id;
  i32 unit2;
  Dialog dialog2;
  Sprite sprite;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (city_wnd_state.unit_dialog_busy != 0) {
    return;
  }
  city_wnd_state.unit_dialog_busy = 1;
  ON_SCOPE_EXIT(city_wnd_state.unit_dialog_busy = 0);
  sprite.create_solid(0x40,0x20,0);
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner != human_civ[0]) && (game.reveal_map == '\0')) return;
  unit2 = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,unit2);
  if (city_wnd.info_panel_mode != 0) return;
  nth = (iptr)(short)stack_idx;
  city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  y = (iptr)cities[city4].y;
  city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  unit2 = tile_first_unit((int)cities[city5].x,y);
  unit = unit_stack_nth(unit2,nth);
  if (unit < 0) return;
  text_set_unit_info_args(unit);
  dialog2.load_game_section((const char *)&str_unitoptions,1);
  dlg_unit_sprite_zoom = 8;
  dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)unit_dlg_draw_sprite_cb);
  dialog2.set_image_zoom(8);
  dialog2.add_sprite((iptr)&sprite,unit,0);
  unit2 = 0;
  option_id = 0;
  city = (iptr)label_string(label_table.string_ids[0x51]);
  dialog2.add_option((const char *)city,option_id,unit2);
  if (units[unit].orders != ORDER_NONE) {
    unit2 = 0;
    option_id = 1;
    city = (iptr)label_string(label_table.string_ids[0x52]);
    dialog2.add_option((const char *)city,option_id,unit2);
    dialog2.set_default_choice(1);
  }
  if (units[unit].orders != ORDER_SLEEP) {
    unit2 = 0;
    option_id = 2;
    city = (iptr)label_string(label_table.string_ids[0x53]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  if ((units[unit].orders != ORDER_FORTIFYING) && (units[unit].orders != ORDER_FORTIFIED)) {
    unit2 = 0;
    option_id = 3;
    city = (iptr)label_string(label_table.string_ids[0x54]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (city6 != (byte)units[unit].home_city) {
    unit2 = 0;
    option_id = 5;
    city = (iptr)label_string(label_table.string_ids[0xb3]);
    dialog2.add_option((const char *)city,option_id,unit2);
    if (units[unit].home_city != -1) {
      unit2 = 0;
      option_id = 6;
      city = (iptr)label_string(label_table.string_ids[0xbd]);
      dialog2.add_option((const char *)city,option_id,unit2);
    }
  }
  unit2 = 0;
  option_id = 7;
  city = (iptr)label_string(label_table.string_ids[0xb4]);
  dialog2.add_option((const char *)city,option_id,unit2);
  if (((((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) &&
      (game.turn_civ != '\0')) && (city2 = unit_moves_left(unit), city2 != 0)) {
    unit2 = 0;
    option_id = 8;
    city = (iptr)label_string(label_table.string_ids[0xbe]);
    dialog2.add_option((const char *)city,option_id,unit2);
    unit2 = 0;
    option_id = 9;
    city = (iptr)label_string(label_table.string_ids[0xbf]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  unit2 = dialog2.show_modal(0);
  switch(unit2) {
  case 1:
    units[unit].orders = ORDER_NONE;
    break;
  case 2:
    unit_set_sleep(unit);
    break;
  case 3:
    units[unit].orders = ORDER_FORTIFYING;
    unit_exhaust_moves(unit);
    break;
  case 4:
    unit_relocate_in_place(unit,1);
    break;
  case 5:
    unit2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    units[unit].home_city = (i8)unit2;
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    return;
  case 6:
    ((CityWindow *)((i8 *)&city_wnd.window))->open((uint)(byte)units[unit].home_city);
    return;
  case 7:
    city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    cities[city2].shield_box =
         (short)((int)((int)(char)unit_types[units[unit].type].cost * (uint)cosmic.shield_box_rows)
                / 2) + cities[city2].shield_box;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_save_build_state(city);
    unit_destroy_redraw(unit,1);
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    return;
  case 8:
    game.active_unit = (i16)unit;
    units[unit].orders = ORDER_NONE;
    moving_units_mode = 0;
    ui_focus_active_unit(0);
    break;
  case 9:
    game.active_unit = (i16)unit;
    units[unit].orders = ORDER_NONE;
    moving_units_mode = 0;
    ui_focus_active_unit(0);
    city_window_close();
    return;
  }
  unit2 = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,unit2);
  ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
  return;
}

/* 005070e5 auto */
void CityWindow::draw_units_present()

{
  int scaled;
  i32 width;
  int row_width;
  int row_y;
  int pad_x;
  int max_icons;
  i32 flag;
  iptr demand_good;
  iptr supply_good;
  iptr cell_h;
  i8 food_icon_rect[16];
  i8 trade_icon_rect[16];
  iptr unit;
  int cell_y;
  int cell_x;
  iptr cell_w;
  int col;
  int row_h;
  uint is_negative;
  int stack_count;
  u32 area_top;
  u32 area_left;
  int per_row;
  int icon_x;
  int saved_per_row;
  int row_start_x;
  int slot;
  int slot_count2;
  int row;
  int label_y;
  iptr i;
  i32 zoom2;
  int unit_h;
  int line_h;
  int offset_y;
  i8 hotspot_rect[16];
  int col_w;
  int margin_x;
  
  area_left = info_panel_rect;
  area_top = info_panel_rect_top;
  zoom2 = map_zoom_level;
  cell_w = gfx_zoom_scale(0x40,zoom2);
  unit_h = gfx_zoom_scale(0x30,zoom2);
  col_w = gfx_zoom_scale(0x40,zoom2);
  row_h = gfx_zoom_scale(0x34,zoom2);
  per_row = scale(0xf4);
  per_row = per_row / col_w;
  scaled = scale(0xf4);
  margin_x = (scaled - col_w * per_row) / 2;
  offset_y = gfx_zoom_scale(4,zoom2);
  slot_count2 = per_row << 2;
  clickmap.remove_by_id(3);
  unit = tile_first_unit((int)cities[city_index].x,
                                (int)cities[city_index].y);
  col = 0;
  row = 0;
  row_start_x = area_left + margin_x;
  stack_count = unit_stack_sum(unit,2);
  if (stack_count <= per_row) {
    offset_y += (row_h >> 1);
    text_clear();
    text_append_label(0x1c3);
    text_set_colors(0x7c,0x12,1,1);
    width = rect_width((RECT *)(iptr)((u8 *)&info_title_rect));
    scaled = scale(1);
    gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + scaled,
                       width);
  }
  cell_x = row_start_x;
  cell_y = area_top + offset_y;
  text_set_surface((iptr)this);
  text_set_font((iptr)&city_font);
  saved_per_row = per_row;
  for (; -1 < unit; unit = unit_next_in_stack(unit)) {
    gfx_draw_unit((iptr)this,unit,4,cell_x,cell_y,zoom2,0);
    if (row < 2) {
      scaled = scale(0);
      label_y = cell_y + scaled + unit_h;
      text_clear();
      if (units[unit].home_city == -1) {
        text_append_label(0xe);
      }
      else {
        text_append((const char *)((uint)(byte)units[unit].home_city * 0x58 + (iptr)((u8 *)&cities + 32)));
      }
      textbuf.char_3 = 0;
      text_set_colors(10,0x1a,1,1);
      gfx_draw_text_centered((const char *)&textbuf.buf,cell_x,label_y,cell_w);
    }
    col++;
    cell_x += col_w;
    if (per_row <= col) {
      cell_x = row_start_x;
      cell_y += row_h;
      row++;
      col = 0;
      if (row == 2) {
        cell_x = (col_w >> 1) + area_left + margin_x;
        cell_y = (row_h >> 1) + area_top + offset_y;
        per_row--;
        row_start_x = cell_x;
      }
      else if (row == 4) break;
    }
  }
  col = 0;
  row = 0;
  per_row = saved_per_row;
  cell_x = area_left + margin_x;
  cell_y = area_top + offset_y;
  row_start_x = cell_x;
  for (slot = 0; slot < slot_count2; slot++) {
    if (row == 0) {
      cell_h = unit_h - 3;
    }
    else {
      cell_h = unit_h;
    }
    rect_set_xywh((iptr)hotspot_rect,cell_x,cell_y,cell_w,cell_h);
    clickmap.hotspot_add_rect(slot,3,(RECT *)hotspot_rect);
    col++;
    cell_x += col_w;
    if (per_row <= col) {
      col = 0;
      row++;
      cell_x = row_start_x;
      cell_y += row_h;
      if (row == 2) {
        cell_x = (col_w >> 1) + area_left + margin_x;
        cell_y = (row_h >> 1) + area_top + offset_y;
        per_row--;
        row_start_x = cell_x;
      }
      else if (row == 4) break;
    }
  }
  if (city_calc.pollution != 0) {
    gfx_set_zoom_scale(view_scale * 4 - 8);
    flag = 0;
    row_width = rect_width((RECT *)&info_panel_rect);
    scaled = scale(0xe);
    row_width -= scaled;
    width = scale(0x40);
    scaled = city_calc.pollution;
    max_icons = city_calc.pollution;
    row_y = scale(100);
    row_y = area_top + row_y;
    pad_x = scale(7);
    city_window_draw_icon_row((iptr)&fortify_overlay_sprites_block.pollution_icon_sprite,area_left + pad_x,row_y,scaled,max_icons,width,row_width,flag);
    gfx_reset_zoom_scale();
  }
  cell_x = scale(7);
  cell_x = area_left + cell_x;
  row_start_x = cell_x;
  cell_y = scale(0x85);
  cell_y = area_top + cell_y;
  line_h = ((GameFont *)&city_font)->line_spacing();
  line_h -= 2;
  text_set_colors(0x79,0x12,1,1);
  scaled = civ_has_advance((int)cities[city_index].owner,ADV_TRADE);
  if (scaled != 0) {
    text_clear();
    text_append_label(0x56);
    text_append_colon();
    for (i = 0; i < 3; i++) {
      is_negative = (uint)(*(char *)&cities[(city_index)].supplies[i] < '\0'
                       );
      if (is_negative != 0) {
        text_append_lparen();
      }
      if (*(char *)&cities[(city_index)].supplies[i] < 1) {
        supply_good = ~(int)*(char *)&cities[(city_index)].supplies[i] + 1;
      }
      else {
        supply_good = (iptr)*(char *)&cities[(city_index)].supplies[i];
      }
      text_append_string_id(commodity_names[supply_good]);
      if (is_negative != 0) {
        text_append_rparen();
      }
      if (i < 2) {
        text_append_comma();
      }
    }
    gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
    cell_y += line_h;
    text_clear();
    text_append_label(0x57);
    text_append_colon();
    for (i = 0; i < 3; i++) {
      is_negative = (uint)(*(char *)&cities[(city_index)].demands[i] < '\0'
                       );
      if (is_negative != 0) {
        text_append_lparen();
      }
      if (*(char *)&cities[(city_index)].demands[i] < 1) {
        demand_good = ~(int)*(char *)&cities[(city_index)].demands[i] + 1;
      }
      else {
        demand_good = (iptr)*(char *)&cities[(city_index)].demands[i];
      }
      text_append_string_id(commodity_names[demand_good]);
      if (is_negative != 0) {
        text_append_rparen();
      }
      if (i < 2) {
        text_append_comma();
      }
    }
    gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
    cell_y += line_h;
  }
  scaled = scale(2);
  cell_y += scaled;
  for (i = 0; i < (char)cities[city_index].trade_routes;
      i = i + 1) {
    text_clear();
    text_append((const char *)(cities[(city_index)].route_partner[i] *
                       0x58 + (iptr)((u8 *)&cities + 32)));
    text_append_space();
    if (*(char *)&cities[(city_index)].route_goods[i] < '\0') {
      text_append_label(0xc0);
    }
    else {
      text_append_string_id(commodity_names
                         [*(char *)&cities[(city_index)].route_goods[i]]);
    }
    text_append_colon();
    if (*(char *)&cities[(city_index)].route_goods[i] < '\0') {
      text_append((const char *)&str_1_5);
      scaled = gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
      icon_x = scale(2);
      icon_x = scaled + icon_x;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(3);
      ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites))->draw((iptr)food_icon_rect,(iptr)this,icon_x,cell_y + scaled)
      ;
      gfx_reset_zoom_scale();
    }
    else {
      text_append_plus();
      text_append_int(city_calc.route_trade_values[i]);
      scaled = gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
      icon_x = scale(2);
      icon_x = scaled + icon_x;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(3);
      ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_trade_icon_sprite))->draw((iptr)trade_icon_rect,(iptr)this,icon_x,cell_y + scaled)
      ;
      gfx_reset_zoom_scale();
    }
    cell_y += line_h;
  }
  return;
}

/* 00507b69 auto */
void CityWindow::draw_support_map()

{
  char civ;
  i32 title_width;
  int origin_x;
  int origin_y;
  int pixel_size;
  iptr x;
  int value;
  iptr unit;
  int y2;
  int map_x;
  int x_offset;
  
  civ = cities[((u32)city_index)].owner;
  text_clear();
  text_append_label(0x1c4);
  text_set_colors(0x7c,0x12,1,1);
  title_width = rect_width((RECT *)(iptr)((u8 *)this + 0x168c));
  origin_x = scale(1);
  gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + origin_x,title_width);
  origin_x = rect_width((RECT *)(iptr)((u8 *)this + 0x164c));
  origin_y = rect_height((RECT *)(iptr)((u8 *)this + 0x164c));
  pixel_size = int_clamp((origin_x - 4) / (int)map.width,1,(origin_y - 4) / (int)map.height);
  origin_x = ((origin_x - 4) - map.width * pixel_size >> 1) + 2 + info_panel_rect;
  origin_y = ((origin_y - 4) - map.height * pixel_size >> 1) + 2 + info_panel_rect_top;
  x_offset = (int)civs[civ].capital_x - ((int)map.width >> 1);
  if ((game.rules & RULE_FLAT_WORLD) != 0) {
    x_offset = 0;
  }
  for (y2 = 0; y2 < map.height; y2++) {
    for (map_x = 0; map_x < map.width; map_x++) {
      x = map_wrap_x(map_x + x_offset);
      value = map_in_bounds(x,y2);
      if ((value != 0) &&
         ((value = tile_visible_to(x,y2,(int)civ), value != 0 ||
          (game.reveal_map != '\0')))) {
        value = tile_is_ocean(x,y2);
        gfx_target_fill_rect(map_x * pixel_size + origin_x,y2 * pixel_size + origin_y,pixel_size,pixel_size,
                           (-(uint)(value == 0) & 0xffffffd3) + 0x5d);
      }
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && ((uint)(byte)units[unit].home_city == ((u32)city_index)))
    {
      value = map_wrap_x(units[unit].x - x_offset);
      gfx_target_fill_rect(value * pixel_size + origin_x,units[unit].y * pixel_size + origin_y,pixel_size,pixel_size,0x1d);
    }
  }
  value = map_wrap_x(cities[((u32)city_index)].x - x_offset);
  gfx_target_fill_rect(value * pixel_size + origin_x,cities[((u32)city_index)].y * pixel_size + origin_y,pixel_size,pixel_size
                     ,0x29);
  return;
}

/* 00507f31 auto */
void CityWindow::happy_draw_building_icons(iptr x_ptr,int row_top,int row_h)

{
  int scaled;
  i8 icon_rect[16];
  iptr improvement_id;
  int icon_y;
  int drawn_count;
  int slot;
  int forced;
  
  drawn_count = 0;
  scaled = scale(0x14);
  icon_y = ((row_h >> 1) + row_top) - (scaled >> 1);
  for (slot = 0; slot < 4; slot++) {
    forced = 0;
    switch(slot) {
    case 0:
      improvement_id = 0xe;
      break;
    case 1:
      improvement_id = 0xb;
      scaled = civ_has_advance((int)cities[((u32)city_index)].owner,ADV_MONOTHEISM);
      if (scaled == 0) continue;
      scaled = civ_has_wonder((int)cities[((u32)city_index)].owner,WONDER_MICHELANGELO);
      if (scaled != 0) {
        forced = 1;
        improvement_id = 0x31;
      }
      break;
    case 2:
      improvement_id = 4;
      break;
    case 3:
      scaled = city_has_improvement(((u32)city_index),IMP_PALACE);
      if (scaled == 0) {
        improvement_id = 7;
      }
      else {
        improvement_id = 1;
      }
      if (civs[cities[((u32)city_index)].owner].government != GOV_DEMOCRACY) continue;
    }
    scaled = city_has_improvement(((u32)city_index),improvement_id);
    if ((scaled != 0) || (forced != 0)) {
      if (2 < drawn_count) {
        return;
      }
      if (drawn_count != 0) {
        scaled = scale(2);
        *(int *)x_ptr = *(int *)x_ptr - scaled;
      }
      drawn_count++;
      scaled = scale(0x24);
      *(int *)x_ptr = *(int *)x_ptr - scaled;
      gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement_id * 0x3c))->draw((iptr)icon_rect,(iptr)this,
                 *(i32 *)x_ptr,icon_y);
      gfx_reset_zoom_scale();
    }
  }
  return;
}

/* 00508177 auto */
void CityWindow::happy_draw_unit_effects(iptr x_ptr,int row_top,int row_h,int max_width)

{
  byte gov_type;
  bool drew_any;
  iptr civ;
  int unit_w;
  iptr text;
  i32 text_x;
  int scaled;
  int half_h;
  int half_row_h;
  int *count_out;
  i32 flag;
  iptr unit;
  int unit_count;
  int aggressive;
  int unhappy_per_unit;
  int icon_count;
  int icons_width;
  u32 zoom2;
  int unit_h;
  
  drew_any = false;
  civ = (iptr)cities[((u32)city_index)].owner;
  gov_type = civs[civ].government;
  zoom2 = ((u32)map_zoom_level);
  unit_w = gfx_zoom_scale(0x40,zoom2);
  unit_h = gfx_zoom_scale(0x30,zoom2);
  half_row_h = row_h >> 1;
  if (gov_type == GOV_FUNDAMENTALISM) {
    unit_w = ((GameFont *)((u8 *)this + 0x16ac))->line_spacing();
    unit_w = (half_row_h + row_top) - (unit_w >> 1);
    text_set_colors(0x29,10,1,0);
    flag = 0;
    text_x = *(i32 *)x_ptr;
    text = (iptr)label_string(government_names[4]);
    text_x = gfx_draw_text_right((const char *)text,text_x,unit_w,flag);
    *(i32 *)x_ptr = text_x;
  }
  else if (gov_type < GOV_REPUBLIC) {
    unit_count = 0;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((((units[unit].id != 0) &&
           (half_h = unit_h >> 1, cities[((u32)city_index)].x == units[unit].x)) &&
          (cities[((u32)city_index)].y == units[unit].y)) &&
         (unit_types[units[unit].type].attack != '\0')) {
        unit_count++;
        if (3 < unit_count) {
          return;
        }
        *(int *)x_ptr = *(int *)x_ptr - unit_w;
        if (drew_any) {
          scaled = scale(2);
          *(int *)x_ptr = *(int *)x_ptr - scaled;
        }
        else {
          drew_any = true;
        }
        gfx_draw_unit((iptr)this,unit,4,*(i32 *)x_ptr,(half_row_h + row_top) - half_h,
                           zoom2,0);
      }
    }
  }
  else {
    unit_w = scale(10);
    half_h = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((half_h == 0) && (half_h = city_has_improvement(((u32)city_index),IMP_POLICE_STATION), half_h == 0)) {
      unhappy_per_unit = 1;
    }
    else {
      unhappy_per_unit = 0;
    }
    if (civs[civ].government == GOV_DEMOCRACY) {
      unhappy_per_unit++;
    }
    if (unhappy_per_unit != 0) {
      aggressive = city_calc.aggressive_units;
      if ((city_calc.aggressive_units != 0) && (civs[civ].government == GOV_REPUBLIC)) {
        aggressive = city_calc.aggressive_units - 1;
      }
      icon_count = aggressive * unhappy_per_unit;
      if (icon_count != 0) {
        half_h = scale(2);
        half_h = max_width - half_h;
        text = 0;
        count_out = &icon_count;
        icons_width = half_h;
        scaled = scale(10);
        half_h = ui_calc_item_spacing(icon_count,scaled + 1,half_h,(iptr)count_out,text);
        icons_width = scale(10);
        icons_width += (icon_count - 1) * half_h;
        *(int *)x_ptr = *(int *)x_ptr - icons_width;
        gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
        text_x = 0;
        half_h = icons_width;
        scaled = scale(10);
        city_window_draw_icon_row((iptr)&small_unhappy_icon_sprite,*(i32 *)x_ptr,(half_row_h + row_top) - (unit_w >> 1),icon_count
                           ,icon_count,scaled + 1,half_h,text_x);
        gfx_reset_zoom_scale();
      }
    }
  }
  return;
}

/* 00508552 auto */
void CityWindow::draw_happiness()

{
  i32 width;
  int scaled;
  iptr text;
  int scaled2;
  int mid_y;
  i8 coin_rect[16];
  i8 lux_rect[16];
  iptr civ;
  int row_y;
  int area_h;
  int area_x;
  int area_y;
  int area_w;
  int right_x;
  int avail_w;
  int row_offset;
  int area_left;
  int icon_y;
  iptr stage;
  iptr prev_stage;
  int row_h;
  int row_w;
  int icon_w;
  
  civ = (iptr)cities[((u32)city_index)].owner;
  text_clear();
  text_append_label(0x1c5);
  text_set_colors(0x7c,0x12,1,1);
  width = rect_width((RECT *)(iptr)((u8 *)this + 0x168c));
  scaled = scale(1);
  gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + scaled,width);
  area_left = info_title_rect + 2;
  scaled = scale(0xe);
  area_y = info_title_rect_top + scaled + 2;
  area_w = rect_width((RECT *)(iptr)((u8 *)this + 0x168c));
  area_w -= 4;
  area_h = rect_height((RECT *)(iptr)((u8 *)this + 0x168c));
  area_h -= 4;
  row_h = area_h / 5;
  area_x = area_left;
  row_y = area_y;
  mid_y = row_h >> 1;
  scaled = scale(0x1e);
  row_offset = mid_y - (scaled >> 1);
  prev_stage = 0;
  for (stage = 0; stage < 5; stage++) {
    if (stage != 0) {
      city_draw_hline(info_title_rect,info_title_rect_right - 1,row_y,0x7c);
    }
    row_w = area_w >> 1;
    right_x = area_x + area_w;
    switch(stage) {
    case 0:
      break;
    case 1:
      if ((uint)(byte)(((i8 *)&city_calc.happy_by_stage))[prev_stage] - (uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[prev_stage] ==
          (uint)city_calc.happy_after_lux - (uint)city_calc.unhappy_after_lux) continue;
      prev_stage = 1;
      icon_w = scale(0xe);
      right_x -= icon_w;
      mid_y = (row_h >> 1) + row_y;
      scaled = scale(0xe);
      icon_y = mid_y - (scaled >> 1);
      gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites))->draw((iptr)lux_rect,(iptr)this,right_x,icon_y);
      gfx_reset_zoom_scale();
      break;
    case 2:
      scaled = city_has_improvement(((u32)city_index),IMP_TEMPLE);
      if (((((scaled == 0) && (scaled = city_has_improvement(((u32)city_index),IMP_COLOSSEUM), scaled == 0)) &&
           (scaled = city_has_improvement(((u32)city_index),IMP_CATHEDRAL), scaled == 0)) &&
          (scaled = civ_has_wonder(civ,WONDER_MICHELANGELO), scaled == 0)) &&
         (((scaled = city_has_improvement(((u32)city_index),IMP_COURTHOUSE), scaled == 0 &&
           (scaled = city_has_improvement(((u32)city_index),IMP_PALACE), scaled == 0)) ||
          (civs[civ].government != GOV_DEMOCRACY)))) continue;
      prev_stage = 2;
      happy_draw_building_icons((iptr)&right_x,row_y,row_h);
      break;
    case 3:
      if ((uint)(byte)(((i8 *)&city_calc.happy_by_stage))[prev_stage] - (uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[prev_stage] ==
          (uint)city_calc.happy_after_units - (uint)city_calc.unhappy_after_units) continue;
      happy_draw_unit_effects((iptr)&right_x,row_y,row_h,area_w);
      break;
    case 4:
      if ((uint)city_calc.happy_after_units - (uint)city_calc.unhappy_after_units == (uint)city_calc.happy_after_wonders - (uint)city_calc.unhappy_after_wonders)
      continue;
      mid_y = (row_h >> 1) + row_y;
      scaled = ((GameFont *)((u8 *)this + 0x16ac))->line_spacing();
      icon_y = mid_y - (scaled >> 1);
      text_set_colors(0x29,10,1,0);
      width = 0;
      scaled = right_x;
      mid_y = icon_y;
      text = (iptr)label_string(label_table.string_ids[0x58]);
      right_x = gfx_draw_text_right((const char *)text,scaled,mid_y,width);
    }
    scaled = scale(2);
    right_x -= scaled;
    avail_w = right_x - area_x;
    scaled = avail_w;
    if (row_w <= avail_w) {
      scaled = row_w;
    }
    row_w = scaled;
    if ((stage == 2) && (civs[civ].government == GOV_FUNDAMENTALISM)) {
      mid_y = (row_h >> 1) + row_y;
      scaled = ((GameFont *)((u8 *)this + 0x16ac))->line_spacing();
      icon_y = mid_y - (scaled >> 1);
      text_set_colors(0x29,10,1,0);
      text_clear();
      text_append_label(0x5d);
      text_append_colon();
      text_append_int(city_calc.building_content);
      mid_y = (row_h >> 1) + row_y;
      scaled = scale(0xe);
      icon_y = mid_y - (scaled >> 1);
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)coin_rect,(iptr)this,area_x,icon_y);
      scaled = icon_y;
      mid_y = scale(0xe);
      scaled2 = scale(3);
      gfx_draw_text((const char *)&textbuf.buf,mid_y + scaled2 + area_x,scaled);
    }
    else {
      happy_draw_citizen_row(area_x,row_y + row_offset,scaled,
                         (uint)(byte)(((i8 *)&city_calc.happy_by_stage))[stage],(uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[stage]
                         ,(uint)(byte)(((i8 *)&city_calc.angry_by_stage))[stage]);
    }
    row_y += row_h;
  }
  return;
}

/* 00508adc auto */
void CityWindow::draw_info_panel(int blit)

{
  int panel_mode;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((iptr)&info_panel_rect);
    panel_mode = info_panel_mode;
    if (panel_mode == 0) {
      draw_units_present();
    }
    else if (panel_mode == 1) {
      draw_support_map();
    }
    else if (panel_mode == 2) {
      draw_happiness();
    }
    if (blit != 0) {
      wnd_blit_surface_rect((iptr)&info_panel_rect);
    }
  }
  return;
}

/* 00508bc5 auto */
void CityWindow::draw_all(i32 blit)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    draw_citizens(blit,0);
    draw_resource_map(blit);
    draw_food_box(blit);
    draw_production(blit);
    draw_button_area(blit);
    draw_supported_units(blit);
    draw_improvements(blit);
    draw_info_panel(blit);
  }
  return;
}

/* 00508c84 auto */
void CityWindow::set_scaled_rect(iptr rect,int x,int y,int width,int height)

{
  rect_set_xywh(rect,margin_x_2 +
                             (view_scale * x + 1) / 2 +
                             client_x,
                     client_y + (view_scale * y + 1) / 2 +
                     content_offset_y,
                     (view_scale * width + 1) / 2,
                     (view_scale * height + 1) / 2);
  return;
}

/* 00508d24 auto */
void CityWindow::layout_rects()

{
  set_scaled_rect((iptr)(&citizens_rect),0,0,0x1b4,0x3d);
  set_scaled_rect((iptr)(&resource_map_rect),0,0x3d,0x1b4,0x99);
  set_scaled_rect((iptr)&resource_label_rect,7,0x41,0xbc,0x89);
  set_scaled_rect((iptr)(&food_box_rect),0x1b4,0,200,0xa7);
  set_scaled_rect((iptr)(&production_rect),0x1b4,0xa7,200,0xbd);
  set_scaled_rect((iptr)button_area_rect,0x1b4,0x164,200,0x41);
  set_scaled_rect((iptr)&supported_units_rect,0,0xd4,0xc0,0x4e);
  set_scaled_rect((iptr)&supported_units_label_rect,7,0xd8,0xb5,0x45);
  set_scaled_rect((iptr)&improvements_rect,0,0x122,0xc0,0x83);
  set_scaled_rect((iptr)&improvement_list_rect,6,0x132,0xa6,0x6c);
  set_scaled_rect((iptr)&info_panel_rect,0xc0,0xd4,0xf4,0xd1);
  set_scaled_rect((iptr)(u8 *)&info_title_rect,0xc5,0xd8,0xe9,0xc6);
  return;
}

/* 00508ec6 auto */
void CityWindow::fill_margins()

{
  int margin_y;
  int scaled;
  int fill_size;
  int margin_x;
  u32 top;
  i32 extent;
  i32 color;
  
  margin_x = margin_x_2;
  margin_y = content_offset_y;
  if (0 < margin_x) {
    fill_size = layout_width;
    scaled = scale(0x27c);
    fill_size -= (margin_x + scaled);
    gfx_target_fill_rect(client_x,client_y,margin_x_2,
                       resource_bar_height,10);
    color = 10;
    extent = resource_bar_height;
    top = client_y;
    scaled = scale(0x27c);
    gfx_target_fill_rect(client_x + scaled + margin_x,top,fill_size,extent,color);
  }
  if (0 < margin_y) {
    margin_x = resource_bar_height;
    fill_size = scale(0x1a5);
    margin_x -= (margin_y + fill_size);
    gfx_target_fill_rect(client_x,client_y,layout_width,
                       content_offset_y,10);
    color = 10;
    extent = layout_width;
    fill_size = scale(0x1a5);
    gfx_target_fill_rect(client_x,client_y + fill_size + margin_y,extent,margin_x,color);
  }
  return;
}

/* 00509028 auto */
void CityWindow::apply_size()

{
  int height;
  int width;
  u32 border;
  i32 font_h;
  uint small_style;
  int font_size;
  
  border = border_width;
  height = get_height();
  width = get_width();
  view_scale_b0[0] = 2;
  view_scale_byte1[0] = '\0';
  view_scale_byte2[0] = '\0';
  view_scale_byte3[0] = '\0';
  if (((int)(border + 0x297) <= height) && ((int)(border * 2 + 0x3b6) <= width)) {
    view_scale_b0[0] = 3;
    view_scale_byte1[0] = '\0';
    view_scale_byte2[0] = '\0';
    view_scale_byte3[0] = '\0';
  }
  if ((height < (int)(border + 0x1b9)) || (width < (int)(border * 2 + 0x278))) {
    view_scale_b0[0] = 1;
    view_scale_byte1[0] = '\0';
    view_scale_byte2[0] = '\0';
    view_scale_byte3[0] = '\0';
  }
  if (view_scale < 2) {
    font_size = 0xc;
    caption_height = 0x12;
  }
  else {
    font_size = scale(0x10);
    border = scale(0x18);
    caption_height = border;
  }
  ((GameFont *)((i8 *)&city_window_font))->set(0,font_size);
  ((GameFont *)((i8 *)&city_window_small_font))->set(0,(font_size * 2) / 3);
  layout();
  layout_width = client_width;
  resource_bar_height = client_height;
  city_window_load_background();
  height = scale(0x27c);
  width = scale(0x1a5);
  if (height < layout_width) {
    margin_x_2 = layout_width - height >> 1;
  }
  else {
    margin_x_2_b0[0] = '\0';
    margin_x_b1[0] = '\0';
    content_offset_x_byte2[0] = '\0';
    content_offset_x_byte3[0] = '\0';
  }
  if (width < resource_bar_height) {
    content_offset_y = resource_bar_height - width >> 1;
  }
  else {
    content_offset_y_b0[0] = '\0';
    content_offset_y_byte1[0] = '\0';
    content_offset_y_byte2[0] = '\0';
    content_offset_y_byte3[0] = '\0';
  }
  small_style = (uint)(view_scale < 3);
  font_h = scale(0xc);
  ((GameFont *)&city_font)->set_styled(1,font_h,small_style);
  if ((margin_x_2 == prev_content_offset_x) &&
     (content_offset_y == prev_content_offset_y)) {
    if (prev_content_offset_x < 0) {
      create_controls();
    }
  }
  else {
    create_controls();
  }
  prev_content_offset_x = margin_x_2;
  prev_content_offset_y = content_offset_y;
  return;
}

/* 005092af auto */
void CityWindow::update_title()

{
  int objective_value;
  
  text_clear();
  text_append_label(0x1f);
  text_append_space();
  text_append_str((const char *)(city_index * 0x58 + (iptr)((u8 *)&cities + 32)));
  if (1 < city_wnd.size_level) {
    text_append_comma();
    text_append_year((int)game.year);
    if ((game.game_flags & GAME_SCENARIO) == 0) {
      text_append_comma();
      text_append_label(0x22);
      text_append_space();
      text_append_city_population(city_index);
    }
    else if (((scenario.flags[0] & 2) != 0) &&
            (objective_value = city_objective_value(city_index), objective_value != 0)) {
      text_append_comma();
      text_append_label(0x1b0);
      if (1 < objective_value) {
        text_append((const char *)&str_x_3);
        text_append_int(objective_value);
      }
    }
    text_append_space();
    text_append_lparen();
    text_append_label(0x9a);
    text_append_colon();
    text_append_gold(civs[cities[city_index].owner].gold);
    text_append_rparen();
  }
  set_caption((const char *)&textbuf.buf);
  return;
}

/* 00509429 auto */
void CityWindow::refresh()

{
  if ((((-1 < city_index) &&
       (city_index < (int)game.num_cities)) &&
      (cities[city_index].id != 0)) && (turn_loop.active != '\0')) {
    if ((redraw_suspended == 0) && (is_closed == 0)) {
      city_recalc(city_index,1);
      apply_size();
      update_title();
      gfx_set_draw_target((iptr)this);
      this->FrameWindow::draw_caption();
      fill_margins();
      layout_rects();
      draw_all(0);
      wnd_blit_surface();
    }
    else {
      update_title();
    }
  }
  return;
}

/* 00509545 auto */
void CityWindow::bring_to_top()

{
  NativeWindow *window;
  
  if ((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) {
    window = ((WndBase *)((u8 *)this + 0x48))->get_window();
    BringWindowToTop(window->hwnd);
  }
  return;
}

/* 00509590 auto */
void CityWindow::open(iptr city)

{
  int flushing;
  
  if ((city_turn.processing == 0) ||
     ((((((flushing = XD_InFlushSendBuffer(), flushing == 0 && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)
         ) && (((net.city_create_busy == 0 && (net.city_delete_busy == 0)) &&
               ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))))) &&
       ((net.unit_stack_link_busy == 0 && (((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0))))
       )) && (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
              ((((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))) &&
                (net.unit_delete_cargo_busy == 0)) && (net.unit_destroy_redraw_busy == 0)))))))) {
    game.transient_flags = game.transient_flags | TRANSIENT_CITY_WINDOW_OPENED;
    city_index = city;
    city_save_build_state(city_index);
    if (is_closed != 0) {
      info_panel_mode_b0[0] = '\0';
      info_panel_mode_b1[0] = '\0';
      info_panel_mode_b2[0] = '\0';
      info_panel_mode_b3[0] = '\0';
      improvement_scroll_pos_b0[0] = '\0';
      improvement_scroll_pos_b1[0] = '\0';
      improvement_scroll_pos_b2[0] = '\0';
      improvement_scroll_pos_b3[0] = '\0';
      panel_toggle_flag_b0[0] = 1;
      panel_toggle_flag_b1[0] = '\0';
      panel_toggle_flag_b2[0] = '\0';
      panel_toggle_flag_b3[0] = '\0';
      if (redraw_suspended != 0) {
        this->SurfaceWindow::show();
      }
    }
    is_closed_b0[0] = '\0';
    is_closed_b1[0] = '\0';
    is_closed_b2[0] = '\0';
    is_closed_b3[0] = '\0';
    redraw_deferred_b0[0] = '\0';
    redraw_deferred_b1[0] = '\0';
    redraw_deferred_b2[0] = '\0';
    redraw_deferred_b3[0] = '\0';
    if (redraw_suspended == 0) {
      repaint();
      this->SurfaceWindow::show();
    }
    else {
      ((WndBase *)&window)->show_restored();
    }
    if (bugfix_ui()) {
      ((WndBase *)&window)->set_focus();  /* B12: the keys go to the city window, not an advisor that opened it (docs/BUGS.md) */
    }
    if (city_wnd_state.exit_btn_pending != '\0') {
      create_exit_button(1);
      city_wnd_state.exit_btn_pending = '\0';
    }
    create_change_button(1);
    city_wnd_state.exit_requested = 0;
    if (city_turn.processing == 0) {
      if ((((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (3 < cities[city].size)) &&
          ((game.shown_event_flags & HINT_CITY_STUFF) == 0)) && (cities[city].owner != human_civ[0])) {
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_citystuff);
        game.shown_event_flags = game.shown_event_flags | HINT_CITY_STUFF;
      }
    }
    else {
      if (((((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && ((cities[city].flags & CITYF_DISORDER) != 0)) ||
           (((cities[city].flags & CITYF_DISORDER_LONG) != 0 && (game.difficulty < DIFF_KING)))) &&
          ((cities[city].owner == human_civ[0] && (autoplay.active == 0)))) &&
         (popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder_2), dlg_input.help_pressed != 0)) {
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder2);
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder3);
      }
      while (((redraw_suspended == 0 && (is_closed == 0))
             && (city_wnd_state.exit_requested == 0))) {
        win_pump_one_message();
      }
    }
  }
  return;
}

/* 00509935 auto */
void __cdecl city_window_on_redraw(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  return;
}

/* 0050994f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl city_window_close(void)

{
  city_wnd.closed = 1;
  ((WndBase *)((i8 *)&city_wnd.wnd))->hide();
  city_wnd.city = 0xffffffff;
  if (bugfix_ui() && (-1 < (int)advisor_state.report_id)) {
    advisor_state.wnd.focus_and_raise();  /* B12: back to the advisor that opened the city (docs/BUGS.md) */
  }
  else {
    main_frame_wnd_block.main_frame_wnd.focus_and_raise();
  }
  ui_flush_input();
  return 0;
}

/* 0050998f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_calc_rect(void)

{
  int width;
  int height;
  int screen_w;
  int screen_h;
  
  width = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x27c);
  height = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x1a5);
  screen_w = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x18);
  height = height + screen_w + 8;
  screen_w = main_frame_wnd_block.main_frame_wnd.client_width();
  screen_h = main_frame_wnd_block.main_frame_wnd.client_height();
  city_wnd.prev_offset_x = 0xffffffff;
  rect_set_xywh((iptr)&minimap_wnd_rect_block.city_wnd_rect,(screen_w >> 1) - (width + 0x10 >> 1),(screen_h >> 1) - (height >> 1),
                     width + 0x10 + ui_metrics.dlgframe_w2,height + ui_metrics.dlgframe_h2);
  return;
}

/* 00509a49 auto */
void __cdecl city_window_init_size(void)

{
  city_wnd.size_level = 2;
  if (999 < caption_font_block.screen_width) {
    city_wnd.size_level = 3;
  }
  city_window_calc_rect();
  return;
}

/* 00509a82 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_minimize(void)

{
  city_wnd.minimized = 1;
  return;
}

/* 00509a9c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_restore(void)

{
  city_wnd.minimized = 0;
  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  return;
}

/* 00509ac0 auto */
void __cdecl city_text_append_build_turns(int cost,int shields)

{
  int turns;
  int total_needed;
  i32 min_turns;
  i32 max_turns;
  
  max_turns = 999;
  min_turns = 1;
  total_needed = cost * city_calc.shield_rows;
  turns = int_clamp_c(city_calc.total_shields - city_calc.shield_support,1,99);
  turns = int_clamp_c(((total_needed - 1) - shields) / turns + 1,min_turns,max_turns);
  text_append_int(turns);
  text_append_space();
  if (turns == 1) {
    text_append_label(0x2d);
  }
  else {
    text_append_label(0x2c);
  }
  return;
}

/* 00509b48 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_buy(i32 control_id)

{
  bool can_afford;
  iptr cur_city;
  iptr flushing;
  int civ;
  iptr city2;
  iptr city3;
  iptr unit_type;
  iptr city4;
  int building;
  iptr city5;
  iptr city6;
  int city;
  uint shield_rows;
  i32 arg_lo;
  i32 clamp_max;
  uint cost2;
  int price;
  Dialog dialog2;
  i8 text_key[80];
  i32 eh_next;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         (((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))) &&
          (net.unit_stack_link_busy == 0)))) &&
        ((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))))) &&
       ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))) {
      arg_lo = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,arg_lo);
      civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = (int)cities[civ].owner;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      unit_type = (iptr)cities[city3].building;
      city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      building = (int)cities[city4].building;
      if ((human_civ[0] == civ) || (game.reveal_map != '\0')) {
        if (-building != 0x26) {
          city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city5].building < '\0') {
            cost2 = (uint)improvements[-building].cost;
            clamp_max = 999;
            arg_lo = 0;
            shield_rows = (uint)cosmic.shield_box_rows;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city = int_clamp(shield_rows * cost2 - (int)cities[city].shield_box,arg_lo,
                                        clamp_max);
            price = city * 2;
            if (0x22 < -building) {
              price = city << 2;
            }
            text_set_label_arg(0,improvements[-building].name);
          }
          else {
            cost2 = (uint)(char)unit_types[unit_type].cost;
            clamp_max = 999;
            arg_lo = 0;
            shield_rows = (uint)cosmic.shield_box_rows;
            city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city = int_clamp(shield_rows * cost2 - (int)cities[city6].shield_box,arg_lo,
                                        clamp_max);
            price = (city * city) / 0x14 + city * 2;
            text_set_label_arg(0,unit_types[unit_type].name);
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city].shield_box == 0) {
            price = price << 1;
          }
          text_set_number_arg(0,price);
          text_set_number_arg_v(1,civs[civ].gold);
          crt_strcpy(text_key,str_complete0);
          can_afford = price <= civs[civ].gold;
          if (can_afford) {
            text_key[8] = 0x31;
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if ((-1 < cities[city].building) &&
             (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
             (cities[city].flags & CITYF_DISORDER) != 0)) {
            text_key[8] = 0x32;
            can_afford = false;
          }
          dialog2.load_game_text((const char *)text_key);
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city].building < '\0') {
            dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + building * -0x3c),0,0);
          }
          else {
            dialog2.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),0,0);
          }
          dialog2.set_image_zoom(8);
          if (can_afford) {
            dialog2.set_default_choice(1);
          }
          building = dialog2.show_modal(0);
          if (can_afford) {
            if ((building == 0) && (city_wnd.closed != 1)) {
              civs[civ].gold -= price;
              civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[civ].shield_box = (ushort)cosmic.shield_box_rows * (short)cost2;
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_save_build_state(city2);
              snd_play_effect(SND_POS1,1,0,0);
              arg_lo = 1;
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_recalc(city2,arg_lo);
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
              ((CityWindow *)((i8 *)&city_wnd.window))->update_title();
              status_draw_info(1);
            }
          }
        }
      }
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_buy_blocked_by_xd_fl);
      net.blocked_input_kind = 7;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[civ].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050a1d6 auto */
i32 __cdecl
city_change_draw_option_icon(i32 dialog,iptr surface,int option,byte flags,int x,int y,
            int row_height)

{
  i8 improvement_rect[16];
  i8 unit_rect[16];
  int improvement;
  i32 zoom2;
  int unit_type;
  int draw_y;
  int draw_x;
  int y_offset;
  int sprite_height2;
  
  unit_type = -1;
  improvement = -1;
  zoom2 = 0xfffffffe;
  if (option < 0x3e) {
    unit_type = option;
  }
  else {
    improvement = option - 0x3e;
  }
  draw_x = x;
  if ((flags & 1) != 0) {
    draw_x = x + 0x26;
  }
  if (-1 < unit_type) {
    draw_y = y;
    sprite_height2 = gfx_zoom_scale(0x30,0xfffffffe);
    y_offset = (sprite_height2 - row_height) / 2;
    draw_y -= y_offset;
    gfx_set_zoom_scale(zoom2);
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((iptr)unit_rect,surface,draw_x,
               draw_y);
    gfx_reset_zoom_scale();
  }
  if (-1 < improvement) {
    y_offset = (0x14 - row_height) / 2;
    draw_y = y - y_offset;
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)improvement_rect,surface,draw_x,
               draw_y);
  }
  return 0;
}

/* Part of city_button_change (0050a473): with the advisors option, the
   build the advisors suggest, in two dialogs beside the list. */
static void city_button_change_advice(Dialog &unit_dialog,Dialog &improvement_dialog)
{
  int city;
  iptr city6;
  iptr *unit_out;
  iptr *improvement_out;
  iptr improvement;
  iptr unit_type;
  i8 caption[80];  /* BF-051 (Peter Johnson's ledger): a longer name is cut
                      (and its advice section not found) */

  if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
    improvement_out = &improvement;
    unit_out = &unit_type;
    city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_choose_production(city6,(iptr)unit_out,(iptr)improvement_out);
    if (unit_type != 999) {
      text_set_label_arg(0,unit_types[unit_type].name);
      unit_dialog.load_text((const char *)&str_advice_2,(const char *)&str_military,8);
      str_clear((char *)caption);
      str_append((char *)caption,(const char *)&str_empty_145);
      str_append_max((char *)caption,label_string(unit_types[unit_type].name),sizeof caption);
      unit_dialog.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),0,0);
      city = unit_dialog.load_text((const char *)&str_advice_3,(const char *)caption,0x8000)
      ;
      if (city != 0) {
        unit_dialog.load_text((const char *)&str_advice_4,(const char *)&str_default,0x8000);
      }
      unit_dialog.run_modal(0,300);
    }
    if (improvement != 999) {
      str_clear((char *)caption);
      str_append((char *)caption,(const char *)&str_empty_146);
      improvement_dialog.load_text((const char *)&str_advice_5,(const char *)&str_domestic,8);
      if (improvement < 0) {
        city = -improvement;
        improvement = -improvement;
        text_set_label_arg(0,improvements[city].name);
        if (improvement < 0x27) {
          str_append_max((char *)caption,label_string(improvements[improvement].name),sizeof caption);
          improvement_dialog.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c),
                     (ui_metrics.large_screen == 0) - 1 & 8,0);
        }
        else {
          str_append((char *)caption,(const char *)&str_wonder);
          improvement_dialog.add_sprite(
                     (iptr)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (improvement * 4 - 0x9c) * 0xf),
                     (ui_metrics.large_screen == 0) - 1 & 8,0);
        }
      }
      else {
        unit_type = improvement;
        text_set_label_arg(0,unit_types[improvement].name);
        str_append_max((char *)caption,label_string(unit_types[unit_type].name),sizeof caption);
        improvement_dialog.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),0,0
                  );
      }
      city = improvement_dialog.load_text((const char *)&str_advice_6,(const char *)caption,0x8000)
      ;
      if (city != 0) {
        improvement_dialog.load_text((const char *)&str_advice_7,(const char *)&str_default_2,0x8000);
      }
      improvement_dialog.run_modal(0,300);
    }
  }
}

/* Part of city_button_change (0050a473): the units, improvements and
   wonders the city can build, the options of the dialog. */
static void city_button_change_add_options(int city2,int &option_count2,iptr current_build,Dialog &change_dialog)
{
  iptr city3;
  int city;
  iptr city4;
  i32 on;
  iptr improvement;
  iptr unit_type;
  int wonder;

  option_count2 = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    city3 = unit_type;
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = civ_can_build_unit(city2,city4,city3);
    if (city != 0) {
      text_clear();
      text_append_string_id(unit_types[unit_type].name);
      text_append_space();
      text_append((const char *)&str_empty_147);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((int)(char)unit_types[unit_type].cost);   /* X30 */
      }
      text_append_lparen();
      city3 = unit_type;
      city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city4,city3);
      city_text_append_build_turns((int)(char)unit_types[unit_type].cost,on);
      text_append_comma();
      text_append_label(0xaa);
      text_append_colon();
      text_append_int((int)(char)unit_types[unit_type].attack);
      text_append((const char *)&str_empty_148);
      text_append_int((int)(char)unit_types[unit_type].defense);
      if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
        text_append((const char *)&str_empty_149);
      }
      text_append((const char *)&str_empty_150);
      text_append_int((int)(char)unit_types[unit_type].move /
                         (int)(uint)cosmic.road_move_mult);
      text_append_space();
      text_append_label(0xab);
      text_append_colon();
      text_append_int((int)(char)unit_types[unit_type].hit_points / 10);
      text_append((const char *)&str_empty_151);
      text_append_int((int)(char)unit_types[unit_type].firepower);
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,unit_type,0);
      if (current_build == unit_type) {
        change_dialog.set_default_choice(unit_type);
      }
      option_count2++;
    }
  }
  for (improvement = 1; improvement < 0x27; improvement++) {
    city3 = improvement;
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = city_can_build_improvement(city2,city4,city3);
    if (city != 0) {
      text_clear();
      text_append_string_id(improvements[improvement].name);
      text_append_space();
      text_append((const char *)&str_empty_152);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((uint)improvements[improvement].cost);   /* X30 */
      }
      text_append_lparen();
      city = -improvement;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city3,city);
      city_text_append_build_turns((uint)improvements[improvement].cost,on);
      if (extra_ui(X_PROD_COST)) {
        /* X30: and its upkeep */
        text_append_comma();
        text_append_int((int)civ_improvement_upkeep(city2,improvement));
        text_append(EXTRAS_ICON_S "g");
      }
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,improvement + 0x3e,0);
      if (-current_build == improvement) {
        change_dialog.set_default_choice(improvement + 0x3e);
      }
      option_count2++;
    }
  }
  for (wonder = 0; wonder < 0x1c; wonder++) {
    city = civ_can_build_wonder(city2,wonder);
    if (city != 0) {
      improvement = wonder + 0x27;
      text_clear();
      city = wonder_is_expired(wonder);
      if (city != 0) {
        text_append_str((const char *)&str_empty_153);
      }
      text_append_string_id(improvements[improvement].name);
      text_append_space();
      text_append((const char *)&str_empty_154);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((uint)improvements[improvement].cost);   /* X30 */
      }
      text_append_lparen();
      city = -improvement;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city3,city);
      city_text_append_build_turns((uint)improvements[improvement].cost,on);
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,wonder + 0x65,0);
      if (-current_build == improvement) {
        change_dialog.set_default_choice(improvement + 0x3e);
      }
      option_count2++;
    }
  }
}

/* 0050a473 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_change(i32 control_id)

{
  char *build_count;
  byte civ;
  iptr cur_city;
  iptr flushing;
  int city2;
  iptr city3;
  iptr city5;
  int city;
  iptr city4;
  iptr city7;
  iptr city8;
  i32 on;
  iptr unit_type3;
  Dialog unit_dialog;
  int option_count2;
  iptr current_build;
  Dialog change_dialog;
  Dialog improvement_dialog;
  int new_shields;
  iptr unit_type2;
  int confirmed;
  
  change_dialog.Dialog::construct(0x8000);
  ON_SCOPE_EXIT(change_dialog.Dialog::destruct());
  unit_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(unit_dialog.Dialog::destruct());
  improvement_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(improvement_dialog.Dialog::destruct());
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         (((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))) &&
          (net.unit_stack_link_busy == 0)))) &&
        ((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))))) &&
       ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))) {
      on = 0;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city3,on);
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = cities[city2].owner;
      city2 = (int)(char)civ;
      if ((human_civ[0] == city2) || (game.reveal_map != '\0')) {
        city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        current_build = (iptr)cities[city5].building;
        while (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
              city != -1) {
          city_button_change_advice(unit_dialog,improvement_dialog);
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          text_set_string_arg(0,(const char *)&cities[city].name);
          if (extra_ui(X_PROD_CANCEL)) {
            /* X29: with a Cancel button (Escape) */
            change_dialog.load_text((const char *)&str_game_7,(const char *)&str_production,1);
          }
          else
          change_dialog.load_game_text((const char *)&str_production);
          change_dialog.set_option_draw_fn((DialogOptionDrawFn)city_change_draw_option_icon);
          change_dialog.set_list_indent(0x4b,0);
          city_button_change_add_options(city2,option_count2,current_build,change_dialog);
          if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) != 0) && (game.multiplayer == '\0')) {
            city3 = (iptr)label_string(label_table.string_ids[0x4c]);
            change_dialog.add_button((const char *)city3);
          }
          if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
            if (0xb < option_count2) {
              option_count2 = 0xc;
            }
            change_dialog.set_position(0xfffffc19,0xfffffffb);
          }
          if (option_count2 < 0x10) {
            city = option_count2 + 1;
            if (city < 9) {
              city = 8;
            }
            change_dialog.set_list_rows(city);
          }
          city_wnd_state.dlg_close_pending = 0;
          dlg_style.pending_idle_callback = city_change_idle_autoclose;
          unit_type3 = change_dialog.run_modal(0,300);
          city_wnd_state.dlg_close_pending = 0;
          if ((unit_type3 < 0) ||
             (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), city == -1
             )) {
            ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
            return;
          }
          if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
            improvement_dialog.close();
            unit_dialog.close();
            ui_flush_input();
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          cities[city].flags = cities[city].flags & ~(CITYF_AUTOBUILD | CITYF_AUTO_UNITS_ONLY | CITYF_AUTO_IMPROVEMENTS_ONLY);
          if (change_dialog.result_extra == 0) {
            if (unit_type3 < 0x3e) {
              unit_type2 = unit_type3;
            }
            else {
              unit_type2 = -(unit_type3 - 0x3e);
            }
            city3 = unit_type2;
            city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            new_shields = city_shields_after_change(city4,city3);
            confirmed = 1;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            if ((new_shields < cities[city].shield_box) && ((game.message_options & MSGOFF_PRODUCTION_PENALTY) == 0)) {
              text_set_number_arg_u8(0,(uint)CONCAT11((char)(game.message_options >> 8),
                                                  cosmic.change_penalty));
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (cities[city].building < '\0') {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                text_set_label_arg(0,improvements[-(int)cities[city].building].name);
              }
              else {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                text_set_label_arg(0,unit_types[cities[city].building].name);
              }
              if (unit_type2 < 0) {
                text_set_label_arg(1,improvements[-unit_type2].name);
              }
              else {
                text_set_label_arg(1,unit_types[unit_type2].name);
              }
              city = popup_game((const char *)&str_prodchange);
              if (city == 0) {
                confirmed = 0;  /* the production kept */
              }
            }
            if (confirmed != 0) {
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (-1 < cities[city].building) {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + city2 * 0x594);
                *build_count = *build_count - 1;
              }
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city].building = (i8)unit_type2;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (-1 < cities[city].building) {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + city2 * 0x594);
                *build_count = *build_count + 1;
              }
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].shield_box = (i16)new_shields;
              advisor_refresh_if_open(1);
              advisor_refresh_if_open(2);
            }
          }
          else if (change_dialog.result_extra == 1) {
            if ((1 << (civ & 0x1f) & (uint)game.human_players) != 0) {
              city_load_citypref();
              on = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              unit_type3 = popup_game_with_city_flags((const char *)&str_automode,1,on);
              if (unit_type3 < 0) {
                ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
                return;
              }
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].flags = cities[city2].flags | CITYF_AUTOBUILD;
              if (unit_type3 == UNIT_ENGINEERS) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                cities[city2].flags = cities[city2].flags | CITYF_AUTO_UNITS_ONLY;
              }
              if (unit_type3 == UNIT_WARRIORS) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                cities[city2].flags = cities[city2].flags | CITYF_AUTO_IMPROVEMENTS_ONLY;
              }
            }
            on = 99;
            city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_production(city3,on);
            ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
            ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
            return;
          }
          else if (change_dialog.result_extra != 2) {
            if (change_dialog.result_extra == 3) {
              if (unit_type3 < 0x3e) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                city2 = (int)cities[city2].y;
                city7 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                city = (int)cities[city7].x;
                city8 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                unit_create(unit_type3,(int)cities[city8].owner,city,city2);
                city2 = unit_type3;
              }
              else {
                city2 = unit_type3 - 0x3e;
                if (city2 < 0x23) {
                  on = 1;
                  unit_type3 = city2;
                  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                  city_set_improvement(city3,city2,on);
                  city2 = unit_type3;
                }
                else if (city2 < 0x27) {
                  if ((0x22 < city2) && (city2 < 0x26)) {
                    city = unit_type3 - 0x61;
                    unit_type3 = city2;
                    city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                    spaceship_add_part((int)cities[city2].owner,city);
                    city2 = unit_type3;
                  }
                }
                else {
                  unit_type3 -= 0x65;
                  on = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                  game.wonder_city[unit_type3] = (i16)on;
                  city2 = unit_type3;
                }
              }
              unit_type3 = city2;
              ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
              popup_debug_text((const char *)&str_freebie);
            }
          }
          else {
            if (unit_type3 < 0x3e) {
              ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_unit_modal(unit_type3);
              current_build = unit_type3;
            }
            else {
              unit_type3 -= 0x3e;
              ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_improvement_modal(unit_type3);
              current_build = -unit_type3;
            }
            continue;
          }
          on = 1;
          city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          city_recalc(city3,on);
          ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
          ((CityWindow *)((i8 *)&city_wnd.window))->draw_resource_map(1);
          ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
          return;
        }
        change_dialog.close();
        unit_dialog.close();
        improvement_dialog.close();
      }
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_change_blocked_by_xd);
      net.blocked_input_kind = 8;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city2].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050b674 auto */
void __cdecl city_change_idle_autoclose(void)

{
  int result;
  
  if (((mp_timer.handle != 0) && (mp_timer.seconds_left < 1)) ||
     (result = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), result == -1)) {
    if (net.host_flag == '\0') {
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
    else if (city_wnd_state.dlg_close_pending == '\0') {
      city_wnd_state.dlg_close_pending = 1;
      city_dlg_close_start_ticks = timer_ticks();
    }
    else {
      result = timer_ticks();
      if (city_dlg_close_start_ticks + 0x4b0 < result) {
        city_wnd_state.dlg_close_pending = '\0';
        net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
        ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
      }
    }
  }
  net.process_messages(1,0);
  return;
}

/* 0050b74e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_rename(i32 control_id)

{
  iptr cur_city;
  iptr city2;
  int city;
  i8 *name_buf;
  i8 new_name[260];
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     (city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
     cities[city2].owner == human_civ[0])) {
    city = XD_InFlushSendBuffer();
    if (((((city == 0) &&
          ((((net.unit_create_busy == 0 && (net.unit_delete_busy == 0)) && (net.city_create_busy == 0)) &&
           ((net.city_delete_busy == 0 && (net.lock_map_busy == 0)))))) &&
         ((net.unlock_map_busy == 0 &&
          ((((net.unit_stack_unlink_busy == 0 && (net.unit_stack_link_busy == 0)) &&
            ((net.unit_stack_bottom_busy == 0 &&
             (((net.unit_relocate_busy == 0 && (net.city_acquire_wonder_busy == 0)) && (net.civ_eliminate_busy == 0)))))) &&
           ((net.civ_create_busy == 0 && (net.civ_check_destroyed_busy == 0)))))))) && (net.unit_gather_cargo_busy == 0)) &&
       (((net.unit_lift_off_map_busy == 0 && (net.unit_delete_cargo_busy == 0)) && (net.unit_destroy_redraw_busy == 0)))) {
      city_wnd_state.dlg_close_pending = 0;
      dlg_style.pending_idle_callback = city_change_idle_autoclose;
      name_buf = new_name;
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = dlg_game_text_input((const char *)&str_renamecity,0xf,(char *)&cities[city].name,(iptr)name_buf);
      if ((city == 0) &&
         (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), city != -1)) {
        snd_play_effect(SND_POS1,0,0,0);
        name_buf = new_name;
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        crt_strcpy((char *)(&cities[city].name),name_buf);
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        map_redraw_all_views(human_civ[0],1);
      }
      city_wnd_state.dlg_close_pending = 0;
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_rename_blocked_by_xd);
      net.blocked_input_kind = 10;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050b9a4 auto */
void __cdecl city_button_info(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 0;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050ba07 auto */
void __cdecl city_button_map(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 1;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050ba6a auto */
void __cdecl city_button_happy(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 2;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050bacd auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_view(i32 control_id)

{
  iptr cur_city;
  iptr flushing;
  int city;
  i32 city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))) &&
        (net.unit_stack_link_busy == 0)) &&
       (((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))) &&
        ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))) {
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      cityview_open(city2);
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_view_blocked_by_xd_f);
      net.blocked_input_kind = 9;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050bc4f auto */
void __cdecl city_window_exit_clicked(i32 control_id)

{
  int cur_city;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    snd_play_effect(SND_MOVPIECE,0,0,0);
    if (city_wnd.open_mode == 2) {
      city_wnd_state.exit_requested = 1;
    }
    else {
      city_window_close();
    }
    advisor_refresh_if_open(1);
    advisor_refresh_if_open(2);
    advisor_refresh_if_open(4);
    advisor_refresh_if_open(5);
    advisor_refresh_if_open(6);
  }
  return;
}

/* 0050bcda auto */
void __cdecl city_window_on_improvement_scroll(i32 scroll_pos)

{
  iptr city;
  i32 recalc_mode;
  
  city_wnd.improvement_list_top = scroll_pos;
  recalc_mode = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,recalc_mode);
  ((CityWindow *)((i8 *)&city_wnd.window))->draw_improvements(1);
  return;
}

/* 0050bd13 auto */
void __cdecl city_button_next_city(i32 control_id)

{
  iptr cur_city;
  iptr city3;
  int cmp;
  char first_name[80];
  iptr city;
  char next_name[80];
  iptr city4;
  iptr city5;
  iptr city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     ((city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
      cities[city3].owner == human_civ[0] || (game.reveal_map != '\0')))) {
    if (city_turn.processing == 0) {
      city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city4 = -1;
      city2 = city5;
      crt_strcpy(next_name,str_empty_155);
      crt_strcpy(first_name,(const char *)(&cities[city2].name));
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city2].owner == cities[city].owner)) {
          cmp = __strcmpi(cities[city].name,next_name);
          if ((0 < cmp) &&
             (cmp = __strcmpi(cities[city].name,cities[city2].name), cmp < 0)) {
            city4 = city;
            crt_strcpy(next_name,(const char *)(&cities[city].name));
          }
          cmp = __strcmpi(cities[city].name,first_name);
          if (0 < cmp) {
            city5 = city;
            crt_strcpy(first_name,(const char *)(&cities[city].name));
          }
        }
      }
      if (city4 < 0) {
        if (city2 != city5) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city5);
        }
      }
      else {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city4);
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 0050bf72 auto */
void __cdecl city_button_prev_city(i32 control_id)

{
  iptr cur_city;
  iptr city3;
  int cmp;
  char last_name[80];
  iptr city;
  char prev_name[80];
  iptr city4;
  iptr city5;
  iptr city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     ((city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
      cities[city3].owner == human_civ[0] || (game.reveal_map != '\0')))) {
    if (city_turn.processing == 0) {
      city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city4 = -1;
      city2 = city5;
      crt_strcpy(prev_name,str_zzzzzzzzzzzzzzzzzzzzzzzzz);
      crt_strcpy(last_name,(const char *)(&cities[city2].name));
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city2].owner == cities[city].owner)) {
          cmp = __strcmpi(cities[city].name,prev_name);
          if ((cmp < 0) &&
             (cmp = __strcmpi(cities[city].name,cities[city2].name), 0 < cmp)) {
            city4 = city;
            crt_strcpy(prev_name,(const char *)(&cities[city].name));
          }
          cmp = __strcmpi(cities[city].name,last_name);
          if (cmp < 0) {
            city5 = city;
            crt_strcpy(last_name,(const char *)(&cities[city].name));
          }
        }
      }
      if (city4 < 0) {
        if (city5 != city2) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city5);
        }
      }
      else {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city4);
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 0050c1d1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_mouse(i32 x,i32 y,i32 button)

{
  iptr flushing;
  iptr city;
  int unit;
  int hit;
  i32 hit_kind;
  
  flushing = XD_InFlushSendBuffer();
  if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
        ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
       ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))) && (net.unit_stack_link_busy == 0)
      ) && (((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
             (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
              ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))) &&
            ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))) {
    hit = city_wnd.clickmap.hit_test(x,y,(iptr)&unit,(iptr)&hit_kind)
    ;
    if (-1 < hit) {
      switch(hit_kind) {
      case 1:
        city_window_toggle_worked_tile(x,y);
        break;
      case 2:
        city_window_cycle_specialist(unit);
        break;
      case 3:
        city_window_present_unit_menu(unit);
        break;
      case 4:
        city_window_sell_improvement(unit);
        break;
      case 5:
        break;
      case 6:
        city_window_supported_unit_menu(unit);
      }
    }
  }
  else {
    dbg_log((const char *)&str_citywin_city_mouse_blocked_by_xd_flushse);
    net.blocked_input_kind = 6;
    net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    net.blocked_input_arg2 = (int)cities[city].owner;
    net.blocked_input_arg3 = x;
    net.blocked_input_arg4 = y;
    net.blocked_input_arg5 = button;
  }
  return;
}

/* 0050c405 auto */
void __cdecl city_window_on_lbuttonup(i32 x,i32 y)

{
  city_window_on_mouse(x,y,0);
  return;
}

/* 0050c427 auto */
void __cdecl city_window_on_rbuttonup(i32 x,i32 y)

{
  city_window_on_mouse(x,y,1);
  return;
}

/* 0050c449 auto */
void CityWindow::close_if_city(iptr city)

{
  if (((int)((u32)city_index) <= city) && (city <= (int)((u32)city_index))) {
    city_window_close();
  }
  return;
}

/* 0050c494 auto */
void CityWindow::on_unit_change(iptr unit,iptr x,iptr y)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    if (unit < 0) {
      if ((cities[city_index].x == x) &&
         (cities[city_index].y == y)) {
        city_recalc(city_index,0);
        draw_info_panel(1);
        draw_production(1);
      }
    }
    else if (((cities[city_index].x == units[unit].x) &&
             (cities[city_index].y == units[unit].y)) ||
            ((cities[city_index].x == x &&
             (cities[city_index].y == y)))) {
      city_recalc(city_index,0);
      draw_info_panel(1);
      draw_production(1);
      if ((uint)(byte)units[unit].home_city == (uint)city_index) {
        city_recalc(city_index,1);
        draw_supported_units(1);
        recalc_redraw(0);
      }
    }
  }
  return;
}

/* 0050c679 auto */
void CityWindow::refresh_if_city(iptr city)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) &&
     ((city_index == city && (-1 < city)))) {
    refresh();
  }
  return;
}

/* 0050c6ef auto */
void CityWindow::refresh_if_near(i32 x,i32 y)

{
  int dist;
  
  if ((((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) && (((u32)redraw_deferred) == 0)) &&
     ((-1 < (int)((u32)city_index) &&
      (dist = map_distance_diag(x,y,(int)cities[((u32)city_index)].x,
                                  (int)cities[((u32)city_index)].y), dist < 3)))) {
    recalc_redraw(0);
  }
  return;
}

/* 0050c7a3 auto */
void CityWindow::clear_controls()

{
  buy_button = 0;
  (change_button) = 0;
  (exit_button) = 0;
  view_button = ((u32)(iptr)exit_button);
  happy_button = view_button;
  rename_button = happy_button;
  map_button = rename_button;
  info_button = map_button;
  next_city_button = 0;
  prev_city_button = next_city_button;
  return;
}

/* 0050c859 auto */
void CityWindow::destroy_controls()

{
  extras_city_window_controls_destroyed();   /* X11 */
  if (buy_button != 0) {
    ((Wnd *)((u8 *)this + 0x48))->remove_all_controls();
    if ((Button *)buy_button != (Button *)0x0) {
      ((Button *)buy_button)->scalar_destruct(1);
    }
    if (change_button != (Button *)0x0) {
      change_button->scalar_destruct(1);
    }
    if ((Button *)info_button != (Button *)0x0) {
      ((Button *)info_button)->scalar_destruct(1);
    }
    if ((Button *)map_button != (Button *)0x0) {
      ((Button *)map_button)->scalar_destruct(1);
    }
    if ((Button *)rename_button != (Button *)0x0) {
      ((Button *)rename_button)->scalar_destruct(1);
    }
    if ((Button *)happy_button != (Button *)0x0) {
      ((Button *)happy_button)->scalar_destruct(1);
    }
    if ((Button *)view_button != (Button *)0x0) {
      ((Button *)view_button)->scalar_destruct(1);
    }
    if (((u32)(iptr)exit_button) != 0) {
      if (exit_button != (Button *)0x0) {
        exit_button->scalar_destruct(1);
      }
      (exit_button) = 0;
    }
    if ((SpriteButton *)prev_city_button != (SpriteButton *)0x0) {
      ((SpriteButton *)prev_city_button)->scalar_destruct(1);
    }
    if ((SpriteButton *)next_city_button != (SpriteButton *)0x0) {
      ((SpriteButton *)next_city_button)->scalar_destruct(1);
    }
    if (improvements_scrollbar != (Scrollbar *)0x0) {
      improvements_scrollbar->scalar_destruct(1);
    }
    clear_controls();
  }
  return;
}

/* 0050ca8d auto */
/* WARNING: Removing unreachable block (ram,0x0050cc02) */

void CityWindow::create_exit_button(int show)

{
  Button *this_00;
  iptr height;
  iptr width;
  int scaled;
  i32 dy;
  int top;
  Button *button;
  i8 rect[16];
  i32 eh_next;
  
  if (exit_button != 0) {
    if (exit_button != (Button *)0x0) {
      exit_button->scalar_destruct(1);
    }
    exit_button_b0[0] = '\0';
    exit_button_byte1[0] = '\0';
    exit_button_byte2[0] = '\0';
    exit_button_byte3[0] = '\0';
  }
  this_00 = (Button *)operator_new(0x3c);
  if (this_00 == (Button *)0x0) {
    button = 0;
  }
  else {
    button = button_ctor(this_00);
  }
  exit_button = button;
  ui_set_button_font((iptr)((u8 *)&city_font));
  height = scale(0x18);
  width = scale(0x39);
  scaled = scale(0x16c);
  top = content_offset_y + scaled + client_y;
  scaled = scale(0x1cb);
  rect_set_xywh((iptr)rect,client_x + scaled + margin_x_2,
                     top,width,height);
  dy = scale(0x19);
  scaled = scale(0x3a);
  top = scale(0x3a);
  rect_offset((iptr)rect,scaled + top,dy);
  height = (iptr)label_string(label_table.string_ids[0x25 + (-(uint)(notice_mode == 2) & 0xcc)]);
  exit_button->create((iptr)&city_wnd.wnd,8,(iptr)rect,(const char *)height);
  exit_button->set_cancel_button();
  exit_button->set_callback(city_window_exit_clicked);
  if (show != 0) {
    exit_button->show_and_redraw();
  }
  return;
}

/* 0050ccb3 auto */
/* WARNING: Removing unreachable block (ram,0x0050ce6a) */

void CityWindow::create_change_button(int show)

{
  int origin_x;
  u32 cx;
  u32 cy;
  Button *this_00;
  iptr height;
  iptr width;
  int top;
  int origin_y;
  i32 dx;
  i32 dy;
  Button *button;
  i8 rect[16];
  char auto_mode;
  i32 eh_next;
  
  auto_mode = (cities[city_index].flags & CITYF_AUTOBUILD) != 0;
  if (city_wnd_state.change_button_variant != auto_mode) {
    city_wnd_state.change_button_variant = auto_mode;
    if (change_button != 0) {
      if (change_button != (Button *)0x0) {
        change_button->scalar_destruct(1);
      }
      change_button_b0[0] = '\0';
      change_button_byte1[0] = '\0';
      change_button_byte2[0] = '\0';
      change_button_byte3[0] = '\0';
    }
    this_00 = (Button *)operator_new(0x3c);
    if (this_00 == (Button *)0x0) {
      button = 0;
    }
    else {
      button = button_ctor(this_00);
    }
    change_button = button;
    ui_set_button_font((iptr)((u8 *)&city_font));
    origin_x = margin_x_2;
    cx = client_x;
    cy = client_y;
    origin_y = content_offset_y;
    height = scale(0x18);
    width = scale(0x44);
    top = scale(0xb5);
    top = cy + origin_y + top;
    origin_y = scale(0x1ba);
    rect_set_xywh((iptr)rect,origin_x + cx + origin_y,top,width,height);
    dy = 0;
    dx = scale(0x73);
    rect_offset((iptr)rect,dx,dy);
    height = (iptr)label_string(label_table.string_ids[0x144 + (-(uint)(auto_mode == '\0') & 0xfffffee0)]);
    change_button->create((iptr)&city_wnd.wnd,2,(iptr)rect,(const char *)height);
    change_button->set_callback(city_button_change);
    if (show != 0) {
      change_button->show_and_redraw();
    }
  }
  return;
}

/* 0050cf06 auto */
/* WARNING: Removing unreachable block (ram,0x0050d785) */
/* WARNING: Removing unreachable block (ram,0x0050d64a) */
/* WARNING: Removing unreachable block (ram,0x0050d52c) */
/* WARNING: Removing unreachable block (ram,0x0050d3ea) */
/* WARNING: Removing unreachable block (ram,0x0050d307) */
/* WARNING: Removing unreachable block (ram,0x0050d471) */
/* WARNING: Removing unreachable block (ram,0x0050d5bb) */
/* WARNING: Removing unreachable block (ram,0x0050d6f6) */
/* WARNING: Removing unreachable block (ram,0x0050d871) */

void CityWindow::create_controls()

{
  iptr height;
  iptr width;
  int top;
  int left_offset;
  i32 offset;
  int scaled;
  int scaled2;
  int scroll_top;
  i32 offset2;
  i8 mask_rect[16];
  Scrollbar *scrollbar;
  Scrollbar *scrollbar_mem;
  SpriteButton *next_btn;
  SpriteButton *next_btn_mem;
  SpriteButton *prev_btn;
  SpriteButton *prev_btn_mem;
  Button *view_btn;
  Button *view_btn_mem;
  Button *happy_btn;
  Button *happy_btn_mem;
  Button *rename_btn;
  Button *rename_btn_mem;
  Button *map_btn;
  Button *map_btn_mem;
  Button *info_btn;
  Button *info_btn_mem;
  Button *change_btn;
  Button *change_btn_mem;
  Button *buy_btn;
  Button *buy_btn_mem;
  int vscroll_w;
  int origin_y2;
  int origin_x2;
  i8 icon_char;
  i8 icon_char_end;
  i8 rect[16];
  i32 eh_next;
  
  window.set_keyboard_nav(0);
  destroy_controls();
  buy_btn_mem = (Button *)operator_new(0x3c);
  if (buy_btn_mem == (Button *)0x0) {
    buy_btn = 0;
  }
  else {
    buy_btn = button_ctor(buy_btn_mem);
  }
  buy_button = (i32)buy_btn;
  change_btn_mem = (Button *)operator_new(0x3c);
  if (change_btn_mem == (Button *)0x0) {
    change_btn = 0;
  }
  else {
    change_btn = button_ctor(change_btn_mem);
  }
  change_button = change_btn;
  info_btn_mem = (Button *)operator_new(0x3c);
  if (info_btn_mem == (Button *)0x0) {
    info_btn = 0;
  }
  else {
    info_btn = button_ctor(info_btn_mem);
  }
  info_button = (i32)info_btn;
  map_btn_mem = (Button *)operator_new(0x3c);
  if (map_btn_mem == (Button *)0x0) {
    map_btn = 0;
  }
  else {
    map_btn = button_ctor(map_btn_mem);
  }
  map_button = (i32)map_btn;
  rename_btn_mem = (Button *)operator_new(0x3c);
  if (rename_btn_mem == (Button *)0x0) {
    rename_btn = 0;
  }
  else {
    rename_btn = button_ctor(rename_btn_mem);
  }
  rename_button = (i32)rename_btn;
  happy_btn_mem = (Button *)operator_new(0x3c);
  if (happy_btn_mem == (Button *)0x0) {
    happy_btn = 0;
  }
  else {
    happy_btn = button_ctor(happy_btn_mem);
  }
  happy_button = (i32)happy_btn;
  view_btn_mem = (Button *)operator_new(0x3c);
  if (view_btn_mem == (Button *)0x0) {
    view_btn = 0;
  }
  else {
    view_btn = button_ctor(view_btn_mem);
  }
  view_button = (i32)view_btn;
  prev_btn_mem = (SpriteButton *)operator_new(0xcc);
  if (prev_btn_mem == (SpriteButton *)0x0) {
    prev_btn = 0;
  }
  else {
    prev_btn = prev_btn_mem->construct();
  }
  prev_city_button = (i32)prev_btn;
  next_btn_mem = (SpriteButton *)operator_new(0xcc);
  if (next_btn_mem == (SpriteButton *)0x0) {
    next_btn = 0;
  }
  else {
    next_btn = next_btn_mem->construct();
  }
  next_city_button = (i32)next_btn;
  scrollbar_mem = (Scrollbar *)operator_new(0x40);
  if (scrollbar_mem == (Scrollbar *)0x0) {
    scrollbar = 0;
  }
  else {
    scrollbar = scrollbar_mem->construct();
  }
  improvements_scrollbar = scrollbar;
  origin_x2 = margin_x_2 + client_x;
  origin_y2 = client_y + content_offset_y;
  ui_set_button_font((iptr)((u8 *)&city_font));
  create_exit_button(0);
  height = scale(0x18);
  width = scale(0x44);
  top = scale(0xb5);
  top = origin_y2 + top;
  left_offset = scale(0x1ba);
  rect_set_xywh((iptr)rect,origin_x2 + left_offset,top,width,height);
  height = (iptr)label_string(label_table.string_ids[0x23]);
  ((Button *)buy_button)->create((iptr)&city_wnd.wnd,1,(iptr)rect,(const char *)height);
  ((Button *)buy_button)->set_callback(city_button_buy);
  city_wnd_state.change_button_variant = 0xff;
  create_change_button(0);
  height = scale(0x18);
  width = scale(0x15);
  top = scale(0x16c);
  top = origin_y2 + top;
  left_offset = scale(0x1b5);
  rect_set_xywh((iptr)rect,origin_x2 + left_offset,top,width,height);
  icon_char_end = 0;
  icon_char = 0x5e;
  spritebtn_create_icon_button((iptr)&city_wnd.wnd,9,(SpriteButton *)prev_city_button,(iptr)&scroll_up_button_sprite,(RECT *)rect);
  ((SpriteButton *)prev_city_button)->set_click_handler(city_button_prev_city);
  offset = scale(0x19);
  rect_offset((iptr)rect,0,offset);
  icon_char = 0x76;
  spritebtn_create_icon_button((iptr)&city_wnd.wnd,10,(SpriteButton *)next_city_button,(iptr)&city_down_arrow_sprite_block.city_down_arrow_sprite,(RECT *)rect);
  ((SpriteButton *)next_city_button)->set_click_handler(city_button_next_city);
  height = scale(0x18);
  width = scale(0x39);
  top = scale(0x16c);
  top = origin_y2 + top;
  left_offset = scale(0x1cb);
  rect_set_xywh((iptr)rect,origin_x2 + left_offset,top,width,height);
  height = (iptr)label_string(label_table.string_ids[0x27]);
  ((Button *)info_button)->create((iptr)&city_wnd.wnd,3,(iptr)rect,(const char *)height);
  ((Button *)info_button)->set_callback(city_button_info);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((iptr)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x29]);
  ((Button *)map_button)->create((iptr)&city_wnd.wnd,4,(iptr)rect,(const char *)height);
  ((Button *)map_button)->set_callback(city_button_map);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((iptr)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x2a]);
  ((Button *)rename_button)->create((iptr)&city_wnd.wnd,5,(iptr)rect,(const char *)height);
  ((Button *)rename_button)->set_callback(city_button_rename);
  offset = scale(0x19);
  top = scale(0xffffffc6);
  left_offset = scale(0xffffffc6);
  rect_offset((iptr)rect,top + left_offset,offset);
  height = (iptr)label_string(label_table.string_ids[0x28]);
  ((Button *)happy_button)->create((iptr)&city_wnd.wnd,6,(iptr)rect,(const char *)height);
  ((Button *)happy_button)->set_callback(city_button_happy);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((iptr)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x26]);
  ((Button *)view_button)->create((iptr)&city_wnd.wnd,7,(iptr)rect,(const char *)height);
  ((Button *)view_button)->set_callback(city_button_view);
  vscroll_w = GetSystemMetrics(2);
  left_offset = scale(0x83);
  left_offset -= 2;
  top = vscroll_w;
  scaled = scale(0x122);
  scroll_top = origin_y2 + scaled + 1;
  scaled = scale(0xc0);
  scaled2 = scale(0);
  rect_set_xywh((iptr)(improvements_scrollbar_rect),(scaled + scaled2 + origin_x2) - (vscroll_w + 1),
                     scroll_top,top,left_offset);
  improvements_scrollbar->create((iptr)&city_wnd.wnd,99,
             (iptr)(improvements_scrollbar_rect),1);
  top = rect_height((RECT *)&improvement_list_rect);
  left_offset = scale(1);
  scaled = scale(0xc);
  improvements_scrollbar->set_page_size((top - (left_offset >> 1)) / scaled);
  improvements_scrollbar->set_callback(city_window_on_improvement_scroll);
  improvements_scrollbar->set_track_callback(city_window_on_improvement_scroll);
  top = view_scale;
  if (top == 1) {
    map_zoom_level_b0[0] = 0xfb;
    map_zoom_level_byte1[0] = 0xff;
    map_zoom_level_byte2[0] = 0xff;
    map_zoom_level_byte3[0] = 0xff;
  }
  else if (top == 2) {
    map_zoom_level_b0[0] = 0xfe;
    map_zoom_level_byte1[0] = 0xff;
    map_zoom_level_byte2[0] = 0xff;
    map_zoom_level_byte3[0] = 0xff;
  }
  else if (top == 3) {
    map_zoom_level_b0[0] = 1;
    map_zoom_level_byte1[0] = '\0';
    map_zoom_level_byte2[0] = '\0';
    map_zoom_level_byte3[0] = '\0';
  }
  if (map_zoom_level != city_wnd_state.tile_pick_zoom_cached) {
    city_wnd_state.tile_pick_zoom_cached = map_zoom_level;
    offset = gfx_zoom_scale(0x20,map_zoom_level);
    offset2 = gfx_zoom_scale(0x40,map_zoom_level);
    ((DibSurface *)((i8 *)&city_tile_pick_surface))->DibSurface::create(offset2,offset);
    gfx_set_zoom_scale(map_zoom_level);
    ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite))->draw((iptr)mask_rect,(iptr)&city_tile_pick_surface,0,0);
    gfx_reset_zoom_scale();
  }
  return;
}

/* 0050dada auto */
void CityWindow::begin_notice()

{
  if (((u32)is_closed) == 0) {
    (notice_mode) = 2;
  }
  else {
    (notice_mode) = 1;
  }
  if (((u32)is_closed) == 0) {
    city_wnd_state.exit_btn_pending = 1;
  }
  return;
}

/* 0050db36 auto */
void CityWindow::end_notice()

{
  int notice_mode;
  
  notice_mode = this->notice_mode;
  notice_mode_b0[0] = '\0';
  notice_mode_b1[0] = '\0';
  notice_mode_b2[0] = '\0';
  notice_mode_b3[0] = '\0';
  if (notice_mode == 2) {
    create_exit_button(1);
  }
  city_wnd_state.exit_btn_pending = 0;
  return;
}

/* 0050db92 auto */
void __cdecl city_window_on_size(void)

{
  ((WndBase *)((i8 *)&city_wnd.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* 0050dbb8 auto */
void __cdecl city_window_on_move(void)

{
  ((WndBase *)((i8 *)&city_wnd.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* 0050dbde auto */
void __cdecl city_window_on_caption_button(int button)

{
  if (button == 1) {
    city_window_exit_clicked(0);
  }
  else if (button == 2) {
    if (1 < city_wnd.size_level) {
      city_wnd.size_level--;
      city_window_calc_rect();
      ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.city_wnd_rect);
    }
  }
  else if ((button == 3) && ((city_wnd.size_level < 2 || ((city_wnd.size_level < 3 && (999 < caption_font_block.screen_width)))))) {
    city_wnd.size_level++;
    city_window_calc_rect();
    ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.city_wnd_rect);
  }
  return;
}

/* 0050dcb6 auto */
void CityWindow::create()

{
  int height;
  int width;
  i32 title;
  i32 style;
  i32 left;
  i32 top;
  i32 extra_arg1;
  i32 extra_arg2;
  i32 extra_arg3;
  
  reset_state();
  notice_mode_b0[0] = '\0';
  notice_mode_b1[0] = '\0';
  notice_mode_b2[0] = '\0';
  notice_mode_b3[0] = '\0';
  content_offset_y_b0[0] = '\0';
  content_offset_y_byte1[0] = '\0';
  content_offset_y_byte2[0] = '\0';
  content_offset_y_byte3[0] = '\0';
  margin_x_2 = content_offset_y;
  prev_content_offset_y_b0[0] = 0xff;
  prev_content_offset_y_byte1[0] = 0xff;
  prev_content_offset_y_byte2[0] = 0xff;
  prev_content_offset_y_byte3[0] = 0xff;
  prev_content_offset_x = prev_content_offset_y;
  redraw_suspended_b0[0] = '\0';
  redraw_suspended_b1[0] = '\0';
  redraw_suspended_b2[0] = '\0';
  redraw_suspended_b3[0] = '\0';
  if ((turn_loop.resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    city_window_init_size();
  }
  extra_arg3 = 8;
  extra_arg2 = 0x18;
  extra_arg1 = 0;
  height = rect_height((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  height -= ui_metrics.dlgframe_h2;
  width = rect_width((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  width -= ui_metrics.dlgframe_w2;
  style = 2;
  left = minimap_wnd_rect_block.city_wnd_rect;
  top = minimap_wnd_rect_block.city_wnd_rect_top;
  title = (iptr)label_string(label_table.string_ids[0x7]);
  this->FrameWindow::create((const char *)title,style,left,top,width,height,extra_arg1,extra_arg2,extra_arg3);
  set_title_font((iptr)&city_window_font);
  set_title_small_font((iptr)&city_window_small_font);
  add_caption_button((iptr)&caption_close_button_sprite_block.caption_close_button_sprite,1);
  add_caption_button((iptr)&caption_zoom_out_sprite,2);
  add_caption_button((iptr)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,3);
  set_caption_button_handler(city_window_on_caption_button);
  clear_controls();
  window.wnd_handlers.set_close_handler(city_window_close);
  set_redraw_callback(city_window_on_redraw);
  window.wnd_handlers.set_minimize_handler(city_window_on_minimize);
  window.wnd_handlers.set_restore_handler(city_window_on_restore);
  window.wnd_handlers.set_lbuttonup_handler(city_window_on_lbuttonup);
  window.wnd_handlers.set_rbuttonup_handler(city_window_on_rbuttonup);
  window.wnd_handlers.set_keydown_handler(map_window_key);
  window.wnd_handlers.set_char_handler((WndKeyFn)map_window_ascii);
  window.wnd_handlers.set_size_handler((WndSizeFn)city_window_on_size);
  window.wnd_handlers.set_move_handler((WndMouseFn)city_window_on_move);
  return;
}

/* 0050dea8 auto */
void CityWindow::destroy()

{
  (redraw_suspended) = 1;
  destroy_controls();
  wnd_destroy_with_surface();
  return;
}

/* 005113b0 auto */
void __cdecl
city_draw_hline(i32 x,i32 y,i32 length,i32 color)

{
  gfx_draw_hline(gfx_draw_target,x,y,length,color);
  return;
}

/* 005113f0 auto */
void __cdecl city_draw_bevel_rect(iptr rect,i32 light_color,i32 dark_color)

{
  gfx_draw_bevel_rect(gfx_draw_target,(RECT *)rect,light_color,dark_color);
  return;
}

/* 005114d0 auto */
void __cdecl
city_window_draw_icon_row(iptr sprite,i32 count,i32 x,i32 y,
            i32 width,i32 spacing,i32 max_width,i32 flags)

{
  sprite_draw_spaced_row(gfx_draw_target,sprite,count,x,y,width,spacing,max_width,flags);
  return;
}

/* 00511690 auto */
int CityWindow::scale(int value)

{
  if (((u32)view_scale) != 2) {
    value = (int)(((u32)view_scale) * value) / 2;
  }
  return value;
}

/* 00579dbb auto */
int __cdecl city_treasury_share(iptr city)

{
  char civ;
  int share;
  
  civ = cities[city].owner;
  if (civs[civ].gold < (int)(32000 / (longlong)(int)cities[city].size)) {
    share = ((int)cities[city].size * civs[civ].gold) / (civs[civ].total_city_size + 1);
  }
  else {
    share = (civs[civ].gold / (civs[civ].total_city_size + 1)) * (int)cities[city].size;
  }
  if (share < 0) {
    share = 32000;
  }
  return share;
}

/* 0057a7e9 auto */
void __cdecl city_transfer_for_civil_war(iptr city,iptr civ2,iptr civ)

{
  civs[civ2].num_cities--;
  civs[civ].num_cities++;
  civs[civ2].total_city_size -= (short)cities[city].size;
  civs[civ].total_city_size = (short)cities[city].size + civs[civ].total_city_size;
  cities[city].owner = (i8)civ;
  tile_set_owner((int)cities[city].x,(int)cities[city].y,civ,1);
  return;
}

/* Part of city_capture_aftermath (0057b5df): the capital taken: the loser may
   buy its palace into its best other city (1000 gold), else loses its
   spaceship. */
static void city_capture_aftermath_palace_escape(uptr city,int x2,int y,iptr city4,iptr civ2)
{
  char size;
  int improvement2;
  int value;
  iptr x;
  iptr unit;
  uptr city3;
  int save_palace;
  uptr city2;
  int best_score;
  int score;

  best_score = -999;
  city3 = 0xffffffff;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if ((((cities[city2].id != 0) && (cities[city2].owner == civ2)) &&
        (city2 != city)) &&
       (((int)cities[city].size / 2 <= (int)cities[city2].size &&
        (7 < cities[city2].size)))) {
      size = cities[city2].size;
      score = map_distance_diag((int)cities[city2].x,(int)cities[city2].y,x2,y);
      score = size * 3 - score;
      unit = tile_first_unit((int)cities[city2].x,(int)cities[city2].y);
      improvement2 = unit_stack_sum(unit,2);
      score += improvement2 * 4;
      improvement2 = city_has_improvement(city2,IMP_SAM_BATTERY);
      if (improvement2 != IMP_NOTHING) {
        score = (score * 3) / 2;
      }
      improvement2 = city_has_improvement(city2,IMP_CITY_WALLS);
      if ((improvement2 != IMP_NOTHING) || (improvement2 = civ_has_wonder(civ2,WONDER_GREAT_WALL), improvement2 != IMP_NOTHING)) {
        score = score << 1;
      }
      improvement2 = city_has_improvement(city2,IMP_SDI_DEFENSE);
      if (improvement2 != IMP_NOTHING) {
        score = score * 3;
      }
      improvement2 = tile_landmass((int)cities[city2].x,(int)cities[city2].y);
      value = tile_landmass(x2,y);
      if (improvement2 != value) {
        score = score / 2;
      }
      if (best_score < score) {
        best_score = score;
        city3 = city2;
      }
    }
  }
  save_palace = 0;
  if (((999 < civs[civ2].gold) && (0 < (int)city3)) && (city4 < 0)) {
    city2 = city3;
    text_set_string_arg(1,(const char *)&cities[city3].name);
    x = civ_get_adjective(civ2);
    text_set_string_arg(2,(const char *)x);
    text_set_number_arg(0,1000);
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
        (human_civ[0] != civ2)) || (autoplay.active != 0)) {
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
          (game.multiplayer < 3)) || (autoplay.active != 0)) {
        if ((DIFF_PRINCE < game.difficulty) &&
           (((improvement2 = civ_spaceship_exists(civ2), improvement2 != IMP_NOTHING ||
             (0xb < civs[civ2].num_cities)) || (1999 < civs[civ2].gold)))) {
          save_palace = 1;
        }
      }
      else {
        sprite_picker_sprite_block.net_can_escape_reply = -1;
        net_send_text_args(0x17,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,1,human_civ[0],0);
        improvement2 = timer_ticks();
        while ((value = timer_ticks(), value - improvement2 < 0xe10 && (sprite_picker_sprite_block.net_can_escape_reply == -1))) {
          net.process_messages(1,0);
        }
        if (sprite_picker_sprite_block.net_can_escape_reply == -1) {
          save_palace = 0;
          improvement2 = civ_spaceship_exists(civ2);
          if (((improvement2 != IMP_NOTHING) || (0xb < civs[civ2].num_cities)) || (1999 < civs[civ2].gold)
             ) {
            save_palace = 1;
          }
        }
      }
    }
    else {
      save_palace = popup_game_with_sprite((const char *)&str_canescape_2,(iptr)&popup_info_icon_sprite,0);
    }
  }
  if (save_palace == 1) {
    civs[civ2].gold -= 1000;
    city_set_improvement(city2,1,1);
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
        (game.multiplayer < 3)) || (human_civ[0] == civ2)) {
      popup_game_with_city((const char *)&str_escape_2,city2);
    }
    else {
      net_send_text_args(0x18,net.net_slots[net.net_civ_slots[civ2]].id,3,0
                         ,city2,0);
    }
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
      city_set_production(city3,99);
    }
  }
  else {
    civ_spaceship_lost_notify(civ2);
    if (-1 < city4) {
      cities[city4].building = -IMP_PALACE;
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the wonder the city was building is
   abandoned (the dialog). */
static void city_capture_aftermath_abandon_wonder(uptr city,Dialog &dialog2,iptr civ2)
{
  int improvement2;
  iptr x;
  iptr improvement3;
  int same_build_count;
  uptr city2;

  if (cities[city].building < 1) {
    improvement3 = ~(int)cities[city].building + 1;
  }
  else {
    improvement3 = (iptr)cities[city].building;
  }
  same_build_count = 0;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if (((cities[city2].id != 0) && (city2 != city)) &&
       ((cities[city2].owner == civ2 &&
        (cities[city2].building == cities[city].building)))) {
      same_build_count++;
    }
  }
  x = (iptr)(civ_name_plural(civ2));
  text_set_string_arg(1,(const char *)x);
  text_set_label_arg(2,improvements[improvement3].name);
  text_set_number_arg(0,same_build_count);
  if (same_build_count == 0) {
    dialog2.load_game_text((const char *)&str_abandonwonder_3);
    if (2 < game.multiplayer) {
      net_send_text_args(0x14,0xff,3,0,improvement3,8);
    }
  }
  else {
    text_clear();
    text_append((const char *)&str_stillwonder);
    x = int_clamp(same_build_count,1,2);
    text_append_int(x);
    dialog2.load_game_text((const char *)&textbuf.buf);
    improvement2 = int_clamp(same_build_count,1,2);
    if (improvement2 == IMP_PALACE) {
      if (game.multiplayer < 3) {
        net_send_text_args(0x4c,0xff,3,0,improvement3,0);
      }
      else {
        net_send_text_args(0x4b,0xff,3,0,improvement3,0);
      }
    }
  }
  dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement3 * 0x3c),0,0);
  dialog2.set_image_zoom(8);
  dialog2.show_modal(0);
}

/* Part of city_capture_aftermath (0057b5df): the wonders of the city change hands
   (the dialogs). */
static void city_capture_aftermath_wonder_notices(uptr city,iptr civ,Dialog &dialog2,iptr civ2)
{
  int improvement2;
  uptr wonder_city;
  iptr wonder;

  for (wonder = 0; wonder < 0x1c; wonder++) {
    wonder_city = wonder_get_city(wonder);
    if (wonder_city == city) {
      improvement2 = wonder + 0x27;
      text_set_label_arg(1,improvements[improvement2].name);
      text_clear();
      if (((wonder < 7) || (wonder == WONDER_STATUE_OF_LIBERTY)) || ((wonder == WONDER_EIFFEL_TOWER || (0x16 < wonder)))) {
        text_append_label(0xf3);
        text_append_space();
      }
      text_set_string_arg(2,(const char *)&textbuf.buf);
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
        net_send_text_args(0x1a,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,0,improvement2,0);
      }
      if ((human_civ[0] == civ) && (autoplay.active == 0)) {
        dialog2.load_game_text((const char *)&str_capturewonder_2);
        dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),0,0);
        dialog2.set_image_zoom(8);
        dialog2.show_modal(0);
      }
      else if ((human_civ[0] == civ2) && (autoplay.active == 0)) {
        dialog2.load_game_text((const char *)&str_lostwonder_2);
        dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),0,0);
        dialog2.set_image_zoom(8);
        dialog2.show_modal(0);
      }
      if (wonder == WONDER_EIFFEL_TOWER) {
        civ_apply_eiffel_tower(civ);
      }
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the city shrinks (an AI may keep it
   whole), city = -1 when it is destroyed. */
static void city_capture_aftermath_shrink(uptr &city,iptr civ,int x2,int y,iptr civ2)
{
  int improvement2;
  uint rnd;
  uint sign;
  int civ4;
  int human_on_continent;

  human_on_continent = 0;
  if (civ == 0) {
    improvement2 = tile_landmass(x2,y);
    for (civ4 = 1; civ4 < 8; civ4++) {
      if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
         (*(char *)&civs[civ4].continent_cities[improvement2] != '\0')) {
        human_on_continent = 1;
      }
    }
  }
  if ((cities[city].size < 2) &&
     (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 &&
        ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) ||
       ((civ == 0 && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) &&
      ((((civ != 0 || (human_on_continent != 0)) &&
        ((game.human_players & cities[city].known_to) == 0)) &&
       (game.power_rank[civ] != 7)))))) {
    if (game.power_rank[game.worst_human_rank] < game.power_rank[civ]) {
      rnd = crt_rand_at("city_capture_aftermath");
      sign = (int)rnd >> 0x1f;
      if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
        cities[city].size--;
      }
    }
  }
  else {
    cities[city].size--;
  }
  if (cities[city].size == '\0') {
    city_delete(city,0);
    city = 0xffffffff;
  }
}

/* Part of city_capture_aftermath (0057b5df): the new owner takes the city radius
   and the city. */
static void city_capture_aftermath_take_tiles(uptr city,iptr civ,int x2,int y,iptr civ2)
{
  int improvement2;
  iptr x;
  iptr y2;
  iptr improvement;

  civs[civ].last_city_turn = game.turn;
  civ_clear_standing_goals_near(civ,ROLE_SETTLE,x2,y,4);
  net_batch_begin();
  for (improvement = 0; improvement < 0x15; improvement++) {
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[improvement] + x2);
    y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[improvement] + y;
    improvement2 = map_in_bounds(x,y2);
    if ((improvement2 != IMP_NOTHING) && (tile_set_radius_civ(x,y2,civ,1), civ == 0)) {
      tile_set_visible(x,y2,1,1,1);
    }
  }
  net_batch_flush();
  cities[city].owner = (byte)civ;
  city_reveal_to_civ(city,civ2);
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_city_taken((iptr)&cities[city].name,civ,civ2);
  }
}

/* Part of city_capture_aftermath (0057b5df): the news of the capture: plunder,
   messages to the humans and the multiplayer peers. */
static void city_capture_aftermath_announce(uptr city,iptr civ,int x2,int y,iptr civ2,iptr plunder)
{
  int improvement2;
  iptr x;
  int civ3;

  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) || (autoplay.active != 0)) {
    if (autoplay.active == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0) &&
           ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0)) {
          if (human_civ[0] == civ3) {
            map_redraw_tile_all_views(x2,y);
          }
          else if (2 < game.multiplayer) {
            net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id,x2,y,0,0,0,0,0,0);
          }
          if (((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) == 0) &&
             ((((byte)(civs[civ].treaties[civ3] >> 16) & (TREATY_MERCENARY >> 16)) == 0 ||
              (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) {
            if (((*(byte *)&civs[civ3].treaties[civ2] & TREATY_ALLIANCE) == 0) &&
               ((((byte)(civs[civ2].treaties[civ3] >> 16) & (TREATY_MERCENARY >> 16)) == 0 ||
                (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) {
              if (((((*(byte *)&civs[civ3].treaties[civ] & TREATY_EMBASSY) != 0) ||
                   (improvement2 = civ_has_wonder(civ3,WONDER_UNITED_NATIONS), improvement2 != IMP_NOTHING)) ||
                  (improvement2 = civ_has_wonder(civ3,WONDER_MARCO_POLO), improvement2 != IMP_NOTHING)) ||
                 ((((*(byte *)&civs[civ3].treaties[civ2] & TREATY_EMBASSY) != 0 ||
                   (improvement2 = civ_has_wonder(civ3,WONDER_UNITED_NATIONS), improvement2 != IMP_NOTHING)) ||
                  (improvement2 = civ_has_wonder(civ3,WONDER_MARCO_POLO), improvement2 != IMP_NOTHING)))) {
                x = (iptr)(civ_name_plural(civ));
                text_set_string_arg(1,(const char *)x);
                x = civ_get_adjective(civ2);
                text_set_string_arg(2,(const char *)x);
                if (human_civ[0] == civ3) {
                  popup_city_picture_zoom((const char *)&str_citycapture2_2,city,0x46,1,civ);
                }
                else if (2 < game.multiplayer) {
                  net_send_text_args(0x1e,net.net_slots[net.net_civ_slots[civ3]].id,4,1,
                                     civ,city);
                }
              }
            }
            else {
              x = (iptr)(civ_name_plural(civ));
              text_set_string_arg(1,(const char *)x);
              x = (iptr)(civ_name_plural(civ2));
              text_set_string_arg(2,(const char *)x);
              if (human_civ[0] == civ3) {
                popup_city_picture_zoom((const char *)&str_cityloseally_2,city,0x46,1,civ);
              }
              else if (2 < game.multiplayer) {
                net_send_text_args(0x1d,net.net_slots[net.net_civ_slots[civ3]].id,4,1,civ
                                   ,city);
              }
            }
          }
          else {
            x = (iptr)(civ_name_plural(civ));
            text_set_string_arg(1,(const char *)x);
            x = civ_get_adjective(civ2);
            text_set_string_arg(2,(const char *)x);
            if (human_civ[0] == civ3) {
              popup_city_picture_zoom((const char *)&str_citywinally_2,city,0x46,1,civ);
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x1c,net.net_slots[net.net_civ_slots[civ3]].id,5,1,civ,
                                 city);
            }
          }
        }
      }
    }
  }
  else {
    x = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)x);
    text_set_number_arg(0,plunder);
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      mapviews_ensure_visible(x2,y,civ);
      map_redraw_tile_all_views(x2,y);
      popup_city_picture_zoom((const char *)&str_citycapture_2,city,0x46,1,civ);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer))
       && (human_civ[0] != civ2)) {
      net_send_message(NM_CITYWIN_DELETED,net.net_slots[net.net_civ_slots[civ2]].id,
                         city,0,0,0,0,0,0,0);
      net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0
                         ,0,0,0,0,0);
      net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ2]].id,x2
                         ,y,civ,0,0,0,0,0);
      net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ2]].id,x2
                         ,y,0,0,0,0,0,0);
      net_send_text_args(0x1b,net.net_slots[net.net_civ_slots[civ2]].id,4,1,
                         civ,city);
    }
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer))
       && (human_civ[0] != civ)) {
      net_send_message(NM_CITYWIN_DELETED,net.net_slots[net.net_civ_slots[civ2]].id,
                         city,0,0,0,0,0,0,0);
      net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0
                         ,0,0,0,0,0);
      net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ]].id,x2,
                         y,civ,0,0,0,0,0);
      net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ]].id,x2,
                         y,0,0,0,0,0,0);
      net_send_text_args(0x1b,net.net_slots[net.net_civ_slots[civ]].id,4,1,
                         civ,city);
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the units the city supported:
   rehomed or disbanded. */
static void city_capture_aftermath_rehome_units(uptr city,iptr civ,int x2,int y,iptr civ2)
{
  char *build_count;
  int improvement2;
  Tile *map_tile;
  iptr x;
  iptr improvement4;
  iptr unit;
  iptr unit_type;
  iptr y2;
  iptr improvement;
  uptr city2;
  int best_score;
  int score;

  for (unit = (iptr)game.num_units - 1; -1 < unit; unit--) {
    if (((units[unit].id != 0) && (units[unit].owner == civ2)) &&
       ((byte)units[unit].home_city == city)) {
      if (units[unit].type != UNIT_PARTISANS) {
        if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
            (city2 = city_at((int)units[unit].x,(int)units[unit].y),
            -1 < (int)city2)) && (city2 != city)) {
          if ((unit_types[units[unit].type].role == ROLE_DEFEND) &&
             (improvement2 = unit_stack_count_role(unit,ROLE_DEFEND), improvement2 == IMP_PALACE)) {
            units[unit].home_city = (i8)city2;
            continue;
          }
          cities[city2].flags = cities[city2].flags | CITYF_DEFENDER_LOST;
        }
        unit_destroy_redraw(unit,1);
        /* rescan from the last unit */
        unit = (iptr)game.num_units;
        continue;
      }
      units[unit].home_city = -1;
    }
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
     (((GOV_FUNDAMENTALISM < civs[civ].government || (GOV_FUNDAMENTALISM < civs[civ2].government)) &&
      (game.difficulty != DIFF_CHIEFTAIN)))) {
    cities[city].flags = cities[city].flags | CITYF_DISORDER;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    cities[city].building = UNIT_SETTLERS;
    if (civ == 0) {
      cities[city].building = UNIT_LEGION;
      unit = tile_first_unit(x2,y);
      if (-1 < unit) {
        cities[city].building = units[unit].type;
      }
    }
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_count = *build_count + 1;
    city_set_production(city,99);
  }
  else {
    map_redraw_neighbors_all_views((int)cities[city].x,(int)cities[city].y);
    if ((cities[city].building < '\0') ||
       (improvement2 = civ_can_build_unit(civ,city,(int)cities[city].building), improvement2 != IMP_NOTHING)) {
      if (cities[city].building < '\0') {
        if (cities[city].building < 1) {
          improvement4 = ~(int)cities[city].building + 1;
        }
        else {
          improvement4 = (iptr)cities[city].building;
        }
        improvement2 = city_can_build_improvement(civ,city,improvement4);
      }
    }
    /* improvement2 == 0: the current build can't be built */
    if (improvement2 == IMP_NOTHING) {
      cities[city].building = UNIT_SETTLERS;
      best_score = 0;
      for (unit_type = 0x3e; -1 < unit_type; unit_type--) {
        if ((unit_types[unit_type].role == ROLE_DEFEND) &&
           (improvement2 = civ_can_build_unit(civ,city,unit_type), improvement2 != IMP_NOTHING)) {
          score = ((int)(char)unit_types[unit_type].defense << 3) /
                     (int)(char)unit_types[unit_type].cost;
          if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
            score++;
          }
          if (best_score < score) {
            best_score = score;
            cities[city].building = (i8)unit_type;
          }
          break;
        }
      }
    }
    net_batch_begin();
    for (improvement = 0; improvement < 8; improvement++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[improvement] + x2);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[improvement] + y;
      improvement2 = map_in_bounds(x,y2);
      if ((improvement2 != IMP_NOTHING) &&
         (map_tile = tile_at(x,y2),
         ((uint)map_tile->visible & 1 << ((byte)civ & 0x1f)) == 0)) {
        tile_set_visible(x,y2,1 << ((byte)civ & 0x1f),1,1);
        map_redraw_tile_all_views(x,y2);
      }
    }
    net_batch_flush();
    city_turn.processing = 1;
    ((CityWindow *)((i8 *)&city_wnd.window))->begin_notice();
    ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    ((CityWindow *)((i8 *)&city_wnd.window))->end_notice();
    city_turn.processing = 0;
  }
}

/* Part of city_capture_aftermath (0057b5df): partisans rise for the loser. */
static void city_capture_aftermath_partisans(uptr city,iptr civ,int capture_mode,int x2,int y,iptr civ2,i8 city_name[80])
{
  byte terrain;
  int improvement2;
  int value;
  iptr x;
  uint rnd;
  i32 tile_bits;
  int veteran_roll;
  int rank_diff;
  int gov_diff;
  iptr unit;
  int partisan_count;
  int partisans_made;
  iptr y3;
  iptr x3;
  int partisan_idx;
  iptr y2;
  iptr improvement;
  int best_score;
  int score;

  if ((uint)civs[civ2].government == (uint)civs[civ].government ||
      (int)((uint)civs[civ2].government - (uint)civs[civ].government) < 0) {
    gov_diff = ~((uint)civs[civ2].government - (uint)civs[civ].government) + 1;
  }
  else {
    gov_diff = (uint)civs[civ2].government - (uint)civs[civ].government;
  }
  if ((uint)game.power_rank[civ2] == (uint)game.power_rank[civ] ||
      (int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) < 0) {
    rank_diff = ~((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) + 1;
  }
  else {
    rank_diff = (uint)game.power_rank[civ2] - (uint)game.power_rank[civ];
  }
  improvement2 = cities[city].size + 5;
  partisan_count = (((int)(improvement2 + (improvement2 >> 0x1f & 7U)) >> 3) * (gov_diff + rank_diff + 1)) / 2;
  if (capture_mode != 0) {
    partisan_count = partisan_count / 2;
  }
  improvement2 = civ_has_advance(civ2,ADV_CONSCRIPTION);
  if (improvement2 == IMP_NOTHING) {
    partisan_count--;
  }
  improvement2 = civ_has_advance(civ2,ADV_GUERRILLA_WARFARE);
  if (improvement2 == IMP_NOTHING) {
    improvement2 = civ_has_advance(civ2,ADV_COMMUNISM);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = 0;
    }
    improvement2 = civ_has_advance(civ2,ADV_GUNPOWDER);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = 0;
    }
  }
  else {
    improvement2 = civ_has_advance(civ,ADV_GUERRILLA_WARFARE);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = partisan_count << 1;
    }
    else {
      partisan_count++;
    }
  }
  improvement2 = (((civs[civ2].treaties[civ] & TREATY_VENDETTA) == 0) - 1 & 2) +
           (civs[civ].reputation - 4);
  if (improvement2 < 1) {
    improvement2 = 0;
  }
  partisans_made = 0;
  for (partisan_idx = 0; partisan_idx < partisan_count; partisan_idx++) {
    best_score = 0;
    for (improvement = 0; improvement < 0x14; improvement++) {
      x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[improvement] + x2);
      y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[improvement] + y;
      value = map_in_bounds(x,y2);
      if (((value != 0) && (value = tile_is_ocean(x,y2), value == 0)) &&
         (value = tile_city_or_unit_owner(x,y2), value < 0)) {
        terrain = tile_terrain(x,y2);
        score = (char)terrain_types[terrain].defense * 2;
        rnd = tile_improvements(x,y2);
        if ((rnd & TILE_ROAD) != 0) {
          score++;
        }
        rnd = tile_improvements(x,y2);
        if ((rnd & TILE_RAILROAD) != 0) {
          score++;
        }
        tile_bits = tile_improvements(x,y2);
        if (((byte)tile_bits & TILE_AIRBASE) == 0x40) {
          score = score << 1;
        }
        if (best_score < score) {
          best_score = score;
          y3 = y2;
          x3 = x;
        }
      }
    }
    if ((best_score != 0) &&
       (unit = unit_create(UNIT_PARTISANS,civ2,x3,y3), -1 < unit)) {
      if (partisans_made == 0) {
        if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
          snd_play_effect(SND_INFANTRY,1,0,0);
        }
        if (((2 < game.multiplayer) &&
            ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
           (human_civ[0] != civ2)) {
          net_send_message(NM_SOUND_PLAY,net.net_slots[net.net_civ_slots[civ2]].id
                             ,0x22,1,0,0,0,0,0,0);
        }
      }
      partisans_made++;
      units[unit].orders = ORDER_FORTIFIED;
      if (improvement2 != IMP_NOTHING) {
        if (improvement2 < 1) {
          veteran_roll = 0;
        }
        else {
          veteran_roll = crt_rand_at("city_capture_aftermath");
          veteran_roll = veteran_roll % (improvement2 + 1);
        }
        if (veteran_roll != 0) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      unit_set_seen_by(unit,civ);
      map_redraw_tile_all_views(x3,y3);
      if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
        ui_delay_ticks(1);
      }
    }
  }
  if (((partisans_made != 0) &&
      (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
       ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) && (autoplay.active == 0))
  {
    x = civ_get_adjective(civ2);
    text_set_string_arg(0,(const char *)x);
    text_set_string_arg(1,(const char *)city_name);
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      dlg_game_txt_with_unit((const char *)&str_partisans_2,unit);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
      net_send_text_args(0x1f,net.net_slots[net.net_civ_slots[civ2]].id,2,0
                         ,unit,0);
    }
  }
}

/* 0057b5df auto */
void __cdecl city_capture_aftermath(uptr city,iptr civ,int capture_mode)

{
  uint *attitude_flags;
  char *build_count;
  bool is_weaker;
  byte terrain;
  int x2;
  int y;
  iptr loser_launched;
  iptr winner_launched;
  int improvement2;
  uptr wonder_city;
  iptr city4;
  Dialog dialog2;
  uint founded_count;
  iptr civ2;
  uint was_founder;
  iptr plunder;
  iptr improvement;
  i8 city_name[80];
  uptr city2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  civ2 = (iptr)cities[city].owner;
  x2 = (int)cities[city].x;
  y = (int)cities[city].y;
  if (capture_mode < 2) {
    diplo_force_war(civ,civ2);
  }
  was_founder = (uint)(cities[city].founder == civ);
  crt_strcpy(city_name,(const char *)(&cities[city].name));
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
     (((loser_launched = civ_spaceship_launched(civ2), loser_launched == 0 &&
       (winner_launched = civ_spaceship_launched(civ), winner_launched == 0)) || (game.difficulty < DIFF_PRINCE)))) {
    diplo_set_treaty_flags(civ,civ2,0x10000);
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
        (civs[civ].government != GOV_DEMOCRACY)) || (game.power_rank[civ] != 7)) {
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         (game.power_rank[civ2] < game.power_rank[civ])) {
        founded_count = 0;
        for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
          if (((cities[city2].id != 0) && (city2 != city)) &&
             ((cities[city2].founder == civ2 && (cities[city2].owner == civ)))) {
            founded_count++;
          }
        }
        if ((1 < (int)founded_count) && ((founded_count & 1) == 0)) {
          attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
          *attitude_flags = *attitude_flags | 0x400000;
        }
      }
    }
    else {
      attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
      *attitude_flags = *attitude_flags | 0x400000;
    }
  }
  diplo_clear_treaty_flags(civ,civ2,0x800);
  if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
      (14 < cities[city].size)) && (was_founder == 0)) {
    attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *attitude_flags = *attitude_flags | 0x10;
  }
  city4 = -1;
  if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
      (improvement2 = city_has_improvement(city,IMP_PALACE), improvement2 != IMP_NOTHING)) && (4 < civs[civ2].num_cities)) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      if (game.power_rank[game.worst_human_rank] < game.power_rank[civ2]) {
        is_weaker = true;
      }
      else {
        is_weaker = false;
      }
    }
    else {
      is_weaker = game.power_rank[civ] < game.power_rank[civ2];
    }
    if ((is_weaker) && (improvement2 = civ_civil_war(civ2), improvement2 == IMP_NOTHING)) {
      cities[city].owner = -1;
      city4 = civ_find_central_city(civ2);
      cities[city].owner = (byte)civ2;
    }
  }
  plunder = city_treasury_share(city);
  improvement2 = city_has_improvement(city,IMP_PALACE);
  if (improvement2 != IMP_NOTHING) {
    city_capture_aftermath_palace_escape(city,x2,y,city4,civ2);
  }
  if ((cities[city].building < -0x26) && (autoplay.active == 0)) {
    city_capture_aftermath_abandon_wonder(city,dialog2,civ2);
  }
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) && (autoplay.active == 0)) {
    city_capture_aftermath_wonder_notices(city,civ,dialog2,civ2);
  }
  plunder = int_clamp(plunder,0,civs[civ2].gold);
  civs[civ2].gold -= plunder;
  if (civ != 0) {
    civs[civ].gold += plunder;
  }
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    status_draw_info(1);
  }
  cities[city].shield_box = 0;
  cities[city].flags = cities[city].flags & 0xffffffc4;
  if (-1 < cities[city].building) {
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ2 * 0x594);
    *build_count = *build_count - 1;
  }
  if (was_founder == 0) {
    city_set_improvement(city,1,0);
    city_set_improvement(city,4,0);
    city_set_improvement(city,0xb,0);
    city_set_improvement(city,7,0);
  }
  if ((capture_mode == 0) && (was_founder == 0)) {
    improvement2 = _rand();
    terrain = (byte)(improvement2 >> 0x1f);
    for (improvement = 0; improvement < 5; improvement++) {
      cities[city].improvements[improvement] =
           cities[city].improvements[improvement] &
           (byte)(0xaa >> ((((byte)improvement2 ^ terrain) - terrain & 1 ^ terrain) - terrain & 0x1f));
    }
  }
  if (((capture_mode == 0) && (was_founder == 0)) || (1 < cities[city].size)) {
    city_capture_aftermath_shrink(city,civ,x2,y,civ2);
  }
  if (-1 < (int)city) {
    city_capture_aftermath_take_tiles(city,civ,x2,y,civ2);
  }
  tile_set_owner(x2,y,civ,1);
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    snd_play_effect(SND_BARRACKS,0,0,0);
  }
  text_set_string_arg(0,(const char *)city_name);
  text_set_label_arg(3,label_table.string_ids[175 - (uint)(was_founder == 0)]);
  text_set_label_arg(4,label_table.string_ids[0xb1 - (uint)(was_founder == 0)]);
  tile_update_known(x2,y,civ);
  tile_update_known(x2,y,civ2);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_INFO_STATUS,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  city_capture_aftermath_announce(city,civ,x2,y,civ2,plunder);
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x20) == 0)) {
    civ_steal_advance(civ,civ2);
  }
  if (-1 < (int)city) {
    city_capture_aftermath_rehome_units(city,civ,x2,y,civ2);
  }
  improvement2 = civ_check_destroyed(civ2,civ);
  if ((((improvement2 == IMP_NOTHING) && (-1 < (int)city)) && (was_founder == 0)) &&
     (((civs[civ2].government == GOV_COMMUNISM || (civs[civ2].government == GOV_DEMOCRACY)) ||
      ((game.first_discoverer[ADV_GUERRILLA_WARFARE] != '\0' ||
       (improvement2 = civ_has_advance(civ2,ADV_COMMUNISM), improvement2 != IMP_NOTHING)))))) {
    city_capture_aftermath_partisans(city,civ,capture_mode,x2,y,civ2,city_name);
  }
  if (-1 < (int)city) {
    cities[city].founder = (byte)civ2;
    cities[city].turns_captured = (u8)game.turn;
    wonder_city = wonder_get_city(WONDER_LEONARDO);
    if (wonder_city == city) {
      civ_leonardo_upgrade(civ);
    }
  }
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_INFO_STATUS,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}
