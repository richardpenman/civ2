/*
 * The game set up from the user's copy of Civilization II Multiplayer Gold
 * Edition (src/host/setup.c).
 */
#ifndef CIV2_SETUP_H
#define CIV2_SETUP_H

#include <stddef.h>

/* Windows: the program's own files, as resources (src/host/setup.rc): deflated. */
#define SETUP_RES_FONTS 1
#define SETUP_RES_FONTS_AA 2
#define SETUP_RES_ATLAS 3

/* Where the game is, before it starts (port_main.c): with no arguments
   GameData (Windows: beside the program; macOS, Linux: in the user's data
   folder), set up first if need be; "--setup [FOLDER]": set up again (from
   FOLDER: an installed game or the CD); a folder: the game, played where
   it is.  macOS, Linux: started in the game's folder, the game there.
   Windows: the fonts beside the program (data), from its resources, where
   missing.  1: game set (a path of the system's); 0: quit. */
int setup_choose_game(int argc, char **argv, char *game, size_t size);

#endif
