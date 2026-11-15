/*
 * (port) HD graphics, the game's side (src/game/hd_graphics.cpp,
 * docs/HD-GRAPHICS.md): calls added at the game's drawing, which do nothing
 * unless HD graphics are on; they change only the planes, never the game's
 * pixels, layout or rand() calls.
 */
#ifndef CIV2_HD_GAME_H
#define CIV2_HD_GAME_H

/* An advance's picture (the pack's advance/N) where the game draws its
   category icon: between hd_advance_begin(advance) and hd_advance_end(),
   that icon's draws show it (the original's icon where the pack has none). */
void hd_advance_begin(int advance);
void hd_advance_end(void);
/* Dialogs draw their sprites later: the advance whose icon the next
   Dialog::add_sprite adds (-1: none), the entry added (window_dialog.cpp),
   and its draw (begin: then hd_advance_end). */
void hd_dialog_advance(int advance);
void hd_dialog_sprite_added(const void *entry, const void *sprite);
void hd_dialog_sprite_begin(const void *entry);

#endif
