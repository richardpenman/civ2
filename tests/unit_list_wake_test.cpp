// SOURCE src/auto/unit.cpp unit_dlg_wake_unit_cb
// SOURCE src/auto/window_dialog.cpp dialog_list_on_lbuttondown
// X2 (UnitListScroll, docs/EXTRAS.md): in the game's own Select Unit To
// Activate dialog (9 units or fewer) a click on a unit's picture wakes it
// (unit_dlg_wake_unit_cb: no orders, not automated), so a few clicks wake
// several sleeping units.  X2's list of more than 9 units must too: a click
// on a unit's row wakes it, in that list only, and not with the extra off,
// and the row is drawn again so its picture shows the unit awake.
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
#include "enums.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");

enum { X_UNIT_LIST = 2 };
static int extras_on = 1;
static int extra_ui(int) { return extras_on; }
static i32 extras_unit_list_draw(Dialog *, DibSurface *, i32, i32, i32, i32, i32) { return 0; }
static i32 other_draw(Dialog *, DibSurface *, i32, i32, i32, i32, i32) { return 0; }

static Unit units[2048];
static struct {
    i32 timeout_start;
    Dialog *active;
} dialog_state;

// The row under the mouse: index 3, unit 7's.
static DialogOption row = {};
void Dialog::pick_pane_at(int, int) {}
i32 Dialog::option_at_point(int, int) { return 3; }
DialogOption *Dialog::option_at(int index) { return index == 3 ? &row : 0; }
static bool awake();
static int drawn_awake;     // the row redrawn after the unit woke
void Dialog::select_option(DialogOption *option, int) { selected_option = option; }
uint Dialog::draw_option(DialogOption *option, int) {
    if (option == &row && awake())
        drawn_awake = 1;
    return 0;
}
void WndBase::set_capture() {}

#include "production.inc"

alignas(8) static unsigned char dialog_mem[sizeof(Dialog)];
alignas(8) static unsigned char surface_mem[sizeof(SurfaceWindow)];

static Dialog *list_dialog(DialogOptionDrawFn draw) {
    std::memset(dialog_mem, 0, sizeof dialog_mem);
    Dialog *d = (Dialog *)dialog_mem;
    d->surf_wnd = (SurfaceWindow *)surface_mem;
    d->style_flags = 0x1000;        // a list (set_listbox)
    d->option_count = 12;
    d->on_draw_option = draw;
    return d;
}

static void sleep_unit() {
    std::memset(units, 0, sizeof units);
    units[7].id = 1;
    units[7].orders = ORDER_SLEEP;
    units[7].flags = UNITF_AUTOMATED;
    row.id = 7;
    drawn_awake = 0;
}

static bool awake() { return units[7].orders == ORDER_NONE && (units[7].flags & UNITF_AUTOMATED) == 0; }

int main() {
    // X2's list: the click wakes the unit.
    sleep_unit();
    dialog_state.active = list_dialog((DialogOptionDrawFn)extras_unit_list_draw);
    dialog_list_on_lbuttondown(10, 10);
    assert(dialog_state.active->selected_option == &row);
    std::printf("X2 list: unit %s\n", awake() ? "woken" : "still asleep");
    assert(awake());
    // ...and its row is drawn again, awake: select_option drew it before the
    // unit woke, so its picture would stay greyed (asleep) until redrawn.
    std::printf("X2 list: row %s\n", drawn_awake ? "redrawn awake" : "not redrawn");
    assert(drawn_awake);

    // Another list: nothing woken.
    sleep_unit();
    dialog_state.active = list_dialog((DialogOptionDrawFn)other_draw);
    dialog_list_on_lbuttondown(10, 10);
    assert(units[7].orders == ORDER_SLEEP);
    sleep_unit();
    dialog_state.active = list_dialog(0);
    dialog_list_on_lbuttondown(10, 10);
    assert(units[7].orders == ORDER_SLEEP);

    // The extra off (classic, replays): nothing woken.
    extras_on = 0;
    sleep_unit();
    dialog_state.active = list_dialog((DialogOptionDrawFn)extras_unit_list_draw);
    dialog_list_on_lbuttondown(10, 10);
    assert(units[7].orders == ORDER_SLEEP);
    return 0;
}
