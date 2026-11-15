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
/* A unit's shield (each of its draws, in that palette colour), until the
   game has recoloured it in that rectangle. */
void hd_shield_begin(int colour);
void hd_shield_end(DibSurface *surface, const RECT *recoloured);
/* A sprite the game recolours in that area after drawing it, in that
   colour (flat: the picture tinted; else its pack's variant of that colour). */
void hd_recolour_begin(Sprite *sprite, DibSurface *surface, const RECT *area, int colour, int flat);
void hd_recolour_end(DibSurface *surface, const RECT *area);
/* The diplomacy room's flag in the civilization's colour. */
void hd_diplomacy_flag(DibSurface *room, int colour);

#endif
