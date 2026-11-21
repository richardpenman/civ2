/*
 * The game set up from the user's copy of Civilization II Multiplayer Gold
 * Edition (src/host/setup.c).
 */
#ifndef CIV2_SETUP_H
#define CIV2_SETUP_H

#include <stddef.h>

/* Windows: the program's own files, as resources (src/host/setup.rc): deflated. */
#define SETUP_RES_FONTS_WIN 3       /* with Windows' widths (src/platform/text.c) */
#define SETUP_RES_FONTS_WIN_AA 4
/* the others (the font files of characters beyond Windows-1252, the
   translations, OpenCiv2's English of the game's texts: data/native's
   fonts/<file>, lang/<code>.txt, text/<NAME>.TXT), listed in SETUP_RES_FILES: lines "ID PATH"
   (tools/build_port.py) */
#define SETUP_RES_FILES 99

/* Where the game is, before it starts (port_main.c): with no arguments
   GameData (Windows: beside the program; macOS, Linux: in the user's data
   folder), set up first if need be; "--setup [FOLDER]": set up again (from
   FOLDER: an installed game or the CD); a folder: the game, played where
   it is.  macOS, Linux: started in the game's folder, the game there.
   Windows: the fonts beside the program (data), from its resources, where
   missing.  1: game set (a path of the system's); 0: quit. */
int setup_choose_game(int argc, char **argv, char *game, size_t size);

/* The original's files game (the game's folder) lacks, from the player's
   copy (found where it usually is, else asked: an installed game, its CD, a
   .zip of the installed game, a disc image), its own files kept: OpenCiv2
   Options' Classic graphics (host_add_original).  1 done, 0 cancelled or
   failed (said). */
int setup_add_original(const char *game);

#endif
