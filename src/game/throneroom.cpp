/*
 * The throne room.  Both functions keep the throne-room window object
 * (0x1c44 bytes) on the stack, where the decompiler only saw it as an
 * unnamed stack address, so they are written out here.
 */
#include "game.h"

/* 0x0044cc80: after the civilization earns a throne-room improvement:
 * show the throne room to place it, unless every piece is complete
 * (pieces 0-6 at level 4, and all the bits of piece 8). */
void __cdecl throneroom_show_upgrade(int civ)
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

/* 0x0044cd9b: show the throne room (to look at it). */
void __cdecl throneroom_show(int civ)
{
    ThroneRoomWindow room(civ);
    room.run_view();
}
