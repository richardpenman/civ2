// SOURCE src/game/throneroom.cpp throneroom_show_upgrade
// SOURCE src/game/throneroom.cpp throneroom_show
// SOURCE src/auto/reports.cpp throneroom_on_key
// A throne room opened inside another (a replay's `call` from the outer
// room's modal loop or dissolve) must leave throneroom_wnd pointing at the
// outer room when it has gone: the window's destructor clears it, and the
// outer room's key and click handlers read through it (the original's crash,
// an access violation at 0044f4dd in throneroom_on_click; docs/BUGS.md,
// memory errors).
#include <cassert>
#include <cstdio>
#define __cdecl
enum { SND_FEEDBK04 = 4 };
static void snd_play_effect(int, int, int, int) {}

struct Civ { unsigned char throne_room[9]; };
static Civ civs[8];

struct ThroneRoomWindow;
static ThroneRoomWindow *throneroom_wnd;
static int depth, nests, ended[4];

// As the game's: the constructor makes the window the throne room, the
// destructor clears it (0044ce16, 0044d027); the modal loops call back.
struct ThroneRoomWindow {
    int id;
    int choosing_item = 0;
    struct Surface {
        struct Wnd {
            ThroneRoomWindow *owner;
            void end_modal_loop() { ended[owner->id]++; }
        } window;
    } throne_surface_wnd;
    explicit ThroneRoomWindow(int) : id(depth) { throne_surface_wnd.window.owner = this; throneroom_wnd = this; }
    ~ThroneRoomWindow() { throneroom_wnd = 0; }
    void loop();
    void run_view() { loop(); }
    void run_upgrade() { loop(); }
};

void throneroom_show(int civ);
void throneroom_show_upgrade(int civ);

#include "production.inc"

// The modal loop: a room opened inside it (nests deep: a view, an
// improvement, ... by turns), then the key that ends it (0xd0), which the
// game gives throneroom_on_key.
void ThroneRoomWindow::loop()
{
    if (depth < nests) {
        depth++;
        if (depth % 2)
            throneroom_show(1);
        else
            throneroom_show_upgrade(1);
        depth--;
    }
    assert(throneroom_wnd == this);
    throneroom_on_key(0xd0);
}

int main()
{
    // A room alone: as before, nothing showing when it has gone.
    throneroom_show(1);
    assert(throneroom_wnd == 0 && ended[0] == 1);

    // A view inside an improvement; a view inside a view, an improvement inside
    // that (the outer rooms: NULL before the fix, their key a crash).
    civs[1].throne_room[0] = 1;
    nests = 1;
    throneroom_show_upgrade(1);
    assert(throneroom_wnd == 0 && ended[0] == 2 && ended[1] == 1);
    nests = 2;
    throneroom_show(1);
    assert(throneroom_wnd == 0 && ended[0] == 3 && ended[1] == 2 && ended[2] == 1);
    std::puts("throneroom nested: ok");
    return 0;
}
