/* Controls: buttons, check boxes, radio buttons, lists, scroll bars, edit boxes.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_CONTROLS_H
#define CIV2_STATE_CONTROLS_H

extern i8 button_wnd_title[4];  /* only button_create_hwnd uses it */
extern i8 spritebtn_wnd_title[4];  /* only spritebtn_create_hwnd uses it */
/* Where drawing goes: the surface and font chosen. */
#pragma pack(push, 1)
typedef struct DrawTarget {
    DibSurface *surface;  /* 00637e58 */
    i8 *font;  /* 00637e5c */
    u8 font_rest[24];
} DrawTarget;
#pragma pack(pop)
extern DrawTarget draw_target;
/* Text colors (palette indexes) of the controls. */
#pragma pack(push, 1)
typedef struct ControlColors {
    byte gfx_text;  /* 00637e78 */
    u8 gfx_text_rest[3];
    byte radio_text;  /* 00637e7c */
    u8 radio_text_rest[3];
    byte radio_shadow;  /* 00637e80 */
    u8 radio_shadow_rest[3];
    byte radio_dx;  /* 00637e84 */
    u8 radio_dx_rest[3];
    byte radio_dy;  /* 00637e88 */
    u8 radio_dy_rest[3];
    byte radio_disabled;  /* 00637e8c */
    u8 radio_disabled_rest[3];
    byte text;  /* 00637e90 */
    u8 text_rest[3];
    byte shadow;  /* 00637e94 */
    u8 shadow_rest[3];
    byte shadow_dx;  /* 00637e98 */
    u8 shadow_dx_rest[3];
    byte shadow_dy;  /* 00637e9c */
    u8 shadow_dy_rest[3];
    byte disabled;  /* 00637ea0 */
    u8 disabled_rest[3];
} ControlColors;
#pragma pack(pop)
extern ControlColors ctrl_colors;
/* The controls' text colors as RGB. */
#pragma pack(push, 1)
typedef struct CtrlRgb {
    i8 text_rgb_r;  /* 00637f00 */
    i8 text_rgb_g;  /* 00637f01 */
    i8 text_rgb_b;  /* 00637f02 */
    char text_rgb_set;  /* 00637f03 */
    i8 disabled_rgb_r;  /* 00637f04 */
    i8 disabled_rgb_g;  /* 00637f05 */
    i8 disabled_rgb_b;  /* 00637f06 */
    char disabled_rgb_set;  /* 00637f07 */
} CtrlRgb;
#pragma pack(pop)
#pragma pack(push, 1)
struct wnd_modal_stack_below_block {
    u8 wnd_modal_stack_before[4];  /* 00637eac */
    i32 wnd_modal_stack[17];  /* 00637eb0: the modal windows by depth */
    TimerManager *game_timer_mgr;  /* 00637ef4 */
    int timer_alt_kind_request;  /* 00637ef8 */
    int avi_idle_update_enabled;  /* 00637efc */
    CtrlRgb ctrl_rgb;  /* 00637f00 */
    int checkbox_mouse_pressed;  /* 00637f08 */
    i8 checkbox_wnd_title[4];  /* 00637f0c */
};
#pragma pack(pop)
extern struct wnd_modal_stack_below_block wnd_modal_stack_below_block;
inline constexpr CtrlRgb &ctrl_rgb = wnd_modal_stack_below_block.ctrl_rgb;
extern int scrollbar_class_registered;  /* only scrollbar_create_hwnd uses it */
extern i8 scrollbar_probe_wnd_title[4];  /* only scrollbar_create_hwnd uses it */
extern i8 scrollbar_wnd_title[4];  /* only scrollbar_create_hwnd uses it */
extern int editbox_class_registered;  /* only edit_create_window uses it */
extern i8 editbox_probe_wnd_title[4];  /* only edit_create_window uses it */
extern RODATA i8 editbox_native_class_name[8];
extern i8 editbox_wnd_title[4];  /* only edit_create_window uses it */
extern int combo_class_registered;  /* only combo_create_window uses it */
extern i8 combo_probe_wnd_title[4];  /* only combo_create_window uses it */
extern i8 combo_wnd_title[4];  /* only combo_create_window uses it */
extern int listbox_class_registered;  /* only listbox_create_hwnd uses it */
extern i8 listbox_probe_wnd_title[4];  /* only listbox_create_hwnd uses it */
extern i8 listbox_wnd_title[4];  /* only listbox_create_hwnd uses it */
extern i8 listbox_no_sel_text[4];  /* only listbox_hwnd_get_sel_text uses it */
/* Radio buttons: their text colors and the mouse capture. */
#pragma pack(push, 1)
typedef struct RadioState {
    i8 text_r;  /* 006386f4 */
    i8 text_g;  /* 006386f5 */
    i8 text_b;  /* 006386f6 */
    char text_set;  /* 006386f7 */
    i8 disabled_r;  /* 006386f8 */
    i8 disabled_g;  /* 006386f9 */
    i8 disabled_b;  /* 006386fa */
    char disabled_set;  /* 006386fb */
    int lbutton_captured;  /* 006386fc */
    int rbutton_captured;  /* 00638700 */
} RadioState;
#pragma pack(pop)
extern RadioState radio_state;
extern i8 radiogrp_item_wnd_title[4];  /* only radiogrp_create_items uses it */
extern RODATA i32 input_vkey_map_table[256];
extern iptr ddctrl_current_target;  /* only ddctrl_set_msg_target uses it */
extern u8 static_scrollbar_1[64];
extern u8 static_scrollbar_2[64];

#endif
