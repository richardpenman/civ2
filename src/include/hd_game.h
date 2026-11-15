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
/* The game's pixels of a sprite drawn without a picture (drawn: the
   rectangle its draw gave) shown, where they are the same as underneath. */
void hd_sprite_pixels_shown(Sprite *sprite, DibSurface *surface, int x, int y, const RECT *drawn);
/* A test: a trireme slid from the map's centre in the 8 directions, alone
   and stacked, must leave the plane as it was (harness/hd/hd-redraw.txt). */
void hd_map_redraw_probe(void);
/* A test: the city at (x, y) in civil disorder, the map centred on it at
   zoom and redrawn (harness/hd/hd-disorder.txt). */
void hd_test_city_disorder(int x, int y, int zoom);
/* The diplomacy room's flag in the civilization's colour. */
void hd_diplomacy_flag(DibSurface *room, int colour);

#endif
