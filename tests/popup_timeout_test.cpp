// SOURCE src/auto/game.cpp play_loop_casualty_popup
// B40: at the start of each civ's turn the game arms the network games'
// 20-second popup timeout (turn_popup_timeout_idle) for the next dialog,
// meant for the casualties report, but arms it also when there are no
// casualties and no report is shown: the next popup of that turn, whatever
// it is (an advance from the Great Library, a Travellers Report, an
// emissary), then closes by itself after 20 seconds, also in a single-player
// game.  Fixed: armed only for the report, when it is shown.
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

static GameState game;
static iptr human_civ[2];
static struct {
    int active;
} autoplay;
static struct {
    i32 casualty_counts[8];
    i32 casualty_reported_counts[8];
} net;
static struct {
    WndNotifyFn pending_idle_callback;
} dlg_style;
static char str_game_7[8] = "GAME";
static char str_casualty[12] = "CASUALTY";
static char str_casualties[16] = "CASUALTIES";
static u8 popup_info_icon_sprite[64];

static int plat_fix_bugs;
static inline int bugfix_state(int n) { return n == 40 && plat_fix_bugs && game.multiplayer < 3; }

static void turn_popup_timeout_idle(void) {}
static i32 civ_leader_title(int) { return (i32)(iptr)"King"; }
static char *civ_leader_name(int) { return (char *)"Philip II"; }
static void text_set_string_arg(int, const char *) {}
static void text_set_number_arg(int, int) {}
static void input_flush_events(void) {}
static void snd_play_effect(int, int, int, int) {}
static void casualty_timeline_show(int) {}

// The report, when shown: whether the timeout was armed for it.
static int reports, armed_for_report;
void Dialog::load_text_noflags(const char *, const char *) {}
DialogSprite *Dialog::add_sprite(Sprite *, int, const char *) { return 0; }
void Dialog::set_image_zoom(u32) {}
i32 Dialog::show_modal(int) {
    reports++;
    armed_for_report = dlg_style.pending_idle_callback == turn_popup_timeout_idle;
    dlg_style.pending_idle_callback = 0;      // taken by the dialog's window (create_window)
    return 0;
}

#include "production.inc"

alignas(8) static unsigned char dialog_mem[sizeof(Dialog)];

static void turn_start(int casualties) {
    std::memset(dialog_mem, 0, sizeof dialog_mem);
    std::memset(&net, 0, sizeof net);
    dlg_style.pending_idle_callback = 0;
    reports = armed_for_report = 0;
    human_civ[0] = 4;
    net.casualty_counts[4] = casualties;
    play_loop_casualty_popup((Dialog *)dialog_mem);
}

int main() {
    // The original: no casualties, no report, but the timeout is left armed
    // for whatever dialog comes next.
    plat_fix_bugs = 0;
    turn_start(0);
    assert(reports == 0);
    assert(dlg_style.pending_idle_callback == turn_popup_timeout_idle);

    // Fixed: no casualties, nothing armed.
    plat_fix_bugs = 1;
    turn_start(0);
    std::printf("no casualties: %d reports, timeout %s\n", reports,
                dlg_style.pending_idle_callback ? "left armed" : "not armed");
    assert(reports == 0);
    assert(dlg_style.pending_idle_callback == 0);

    // Casualties: the report is shown with its timeout, as the original's
    // (both modes), and nothing is left armed after it.
    for (plat_fix_bugs = 0; plat_fix_bugs < 2; plat_fix_bugs++) {
        turn_start(2);
        assert(reports == 1 && armed_for_report);
        assert(dlg_style.pending_idle_callback == 0);
    }

    // Autoplay: no report, nothing armed (both modes).
    for (plat_fix_bugs = 0; plat_fix_bugs < 2; plat_fix_bugs++) {
        autoplay.active = 1;
        turn_start(2);
        assert(reports == 0 && dlg_style.pending_idle_callback == 0);
        autoplay.active = 0;
    }
    return 0;
}
