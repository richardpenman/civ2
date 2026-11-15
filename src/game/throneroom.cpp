/*
 * The throne room.  Both functions keep the throne-room window object
 * (0x1c44 bytes) on the stack, where the decompiler only saw it as an
 * unnamed stack address, so they are written out here.
 *
 * A throne room opened while another is showing (a replay's `call` from
 * the outer room's modal loop or dissolve: the game itself never does it,
 * the room covering the screen and taking the keys) leaves throneroom_wnd,
 * which the window's destructor clears, NULL for the outer one, whose next
 * click or key reads through it (throneroom_on_click, throneroom_on_key):
 * the original's crash (an access violation at 0044f4dd).  A memory error,
 * fixed in every mode (docs/BUGS.md): the outer room is the throne room
 * again when the inner one has gone.
 */
#include "game.h"

/* 0x0044cc80: after the civilization earns a throne-room improvement:
 * show the throne room to place it, unless every piece is complete
 * (pieces 0-6 at level 4, and all the bits of piece 8). */
void __cdecl throneroom_show_upgrade(int civ)
{
    ThroneRoomWindow *outer = throneroom_wnd;   /* (a room this one opens inside) */
    {
        ThroneRoomWindow room(civ);     /* the window object, as the original kept it: on the stack */
        int i, incomplete = 0;
        for (i = 0; i < 7; i++)
            if (civs[civ].throne_room[i] < 4)
                incomplete = 1;
        if ((~civs[civ].throne_room[8] & 0x7f) != 0)
            incomplete = 1;
        if (incomplete)
            room.run_upgrade();
    }
    throneroom_wnd = outer;
}

/* 0x0044cd9b: show the throne room (to look at it). */
void __cdecl throneroom_show(int civ)
{
    ThroneRoomWindow *outer = throneroom_wnd;   /* (a room this one opens inside) */
    {
        ThroneRoomWindow room(civ);
        room.run_view();
    }
    throneroom_wnd = outer;
}
