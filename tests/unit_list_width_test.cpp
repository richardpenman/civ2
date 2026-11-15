// SOURCE src/auto/window_dialog.cpp Dialog::set_width
// SOURCE src/auto/window_dialog.cpp Dialog::set_listbox
// SOURCE src/auto/unit.cpp unit_stack_select_dialog
// X2 (UnitListScroll, docs/EXTRAS.md): the Activate Unit list of a stack of
// more than 9 units.  In a large window the game scales dialogs by 3/2
// (dlg_set_scale(3,2)): set_width scales the dialog's width, but set_listbox
// takes the list's width as given, so the list must be scaled the same way
// or it fills only two thirds of the dialog.
#include <cassert>
#include <cstdio>
#include <cstring>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#include "civ2classes.h"
static int _fclose(FILE *f) { return std::fclose(f); }
static void *operator_new(size_t n) { return ::operator new(n); }
#include "scoped.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");

enum { X_UNIT_LIST = 2 };
static int extras_on = 1;
static int extra_ui(int) { return extras_on; }

static struct { char buf[1024]; } textbuf;
static u32 dlg_unit_sprite_zoom;
static u32 scale_num = 1, scale_den = 1;    // what dlg_set_scale left
static u32 shown_list_width, shown_content_width;

// The stack: 12 units, 0 to 11.
static int unit_stack_sum(int, int) { return 12; }
static int unit_stack_first(int) { return 0; }
static int unit_next_in_stack(int unit) { return unit < 11 ? unit + 1 : -1; }
static void unit_stack_select_text(int unit) { std::snprintf(textbuf.buf, sizeof textbuf.buf, "Unit %d", unit); }
static void sprite_init(Sprite *) {}
static void sprite_destroy(Sprite *) {}
i32 Sprite::create_solid(i32, i32, uint) { return 0; }
static void extras_unit_list_draw() {}
static void unit_dlg_wake_unit_cb() {}
static void unit_dlg_dblclick_accept_cb() {}
static void unit_dlg_draw_sprite_cb() {}

Dialog::Dialog(i32) {
    std::memset((void *)this, 0, sizeof *this);
    this->scale_num = ::scale_num;
    this->scale_den = ::scale_den;
}
Dialog::~Dialog() {}
void Dialog::reset(i32) {}
void Dialog::set_title(const char *) {}
void Dialog::set_option_draw_fn(DialogOptionDrawFn) {}
void Dialog::set_list_indent(uptr, int) {}
void Dialog::set_list_rows(u32) {}
void Dialog::set_sprite_click_fn(DialogSpriteFn) {}
void Dialog::set_sprite_dblclick_fn(DialogSpriteFn) {}
void Dialog::set_sprite_draw_fn(DialogSpriteDrawFn) {}
void Dialog::set_image_zoom(u32) {}
DialogSprite *Dialog::add_sprite(Sprite *, int, const char *) { return 0; }
DialogOption *Dialog::add_option(const char *, int, uint) { return 0; }
i32 Dialog::show_modal(int) {
    shown_list_width = list_width[0];
    shown_content_width = content_width;
    return 0;
}

#include "production.inc"

int main() {
    // 1:1 (a window up to 1000 pixels wide): 332 of 340, as before.
    unit_stack_select_dialog(0, "Select Unit To Activate", 1);
    assert(shown_content_width == 0x154);
    assert(shown_list_width == 0x14c);

    // 3/2 (a larger window): the dialog is 510 wide, the list scaled with it.
    scale_num = 3;
    scale_den = 2;
    unit_stack_select_dialog(0, "Select Unit To Activate", 1);
    assert(shown_content_width == 510);
    std::printf("3/2: list %u of %u\n", shown_list_width, shown_content_width);
    assert(shown_list_width == 498);
    return 0;
}
