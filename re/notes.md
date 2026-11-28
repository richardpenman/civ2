# Analysis notes

Facts found while naming functions that are not yet expressed as types.

## Window objects: event handler slots

The game's window wrapper keeps message handlers in a table at window+0x10
(dispatcher: the window procedure, `ddwin_native_wndproc` in src/auto/window.cpp).  Offsets from the
table start:

| offset | called on |
|--------|-----------|
| 0x00 | WM_MOUSEMOVE |
| 0x04 | WM_LBUTTONDOWN |
| 0x08 | every WM_LBUTTONUP |
| 0x0c | WM_LBUTTONUP after a matching down |
| 0x10 | WM_RBUTTONDOWN |
| 0x14 | every WM_RBUTTONUP |
| 0x18 | WM_RBUTTONUP after a matching down |
| 0x1c | WM_LBUTTONDBLCLK |
| 0x20, 0x28 | WM_KEYDOWN |
| 0x24 | WM_KEYUP |
| 0x2c | WM_CHAR |
| 0x30 | WM_CLOSE |
| 0x34 | SC_MINIMIZE |
| 0x38 | SC_RESTORE |
| 0x3c | WM_SETFOCUS |
| 0x40 | WM_SIZE |
| 0x44 | WM_MOVE |
| 0x48 | message 0x44 |
| 0x4c-0x5c | scrolling (WM_HSCROLL, WM_VSCROLL) |

## Labels

label_string(n) returns entry n of the string heap loaded from LABELS.TXT,
counted from the line after @LABELS and its count line ("888"): 0 = "B.C.",
1 = "A.D.".  (Counting from the count line puts every label one off.)  The
table at iptr_00628420 holds pointers to those strings: offset/4 is the
label number (+0x734 = 461, "Technological Advances Editor").

## Multiplayer

The network manager object lives at 0x6ad108.  Player slots: 7 records of
0x54 bytes from 0x6ad30c (+0 network id, +0x24 player name, +0x4d active,
+0x50 civ); 0x6ad558 maps each civ to its slot, 0x6ad304 is the local slot.
Protocol (0x6ad2f8): 0 TCP/IP, 1 IPX/SPX (labels 657, 658), 2 dial-up,
3 direct serial.  Message-type names are a string table at 0x628470
(NM_START_CIVILIZATION ...); 0x6c8fe0 counts received messages per type.
The net message queue object is at 0x6c31d8.

## Busy flags

0x6ad8bc-0x6ad904 are set to 1 for the length of one function each
(unit_create, city_delete, civ_eliminate ...), named <function>_busy; UI
handlers refuse input while any is set (ui_input_busy guards those).

## Game data blocks

- 0x655128 (0x152 bytes, saved with the game): destroyed civs - a count,
  then up to 12 records {turn, destroying civ, leader portrait, civ name};
  destroyed_civs_show (mk.dll window) lists them.
- 0x6af2a0: casualty log, 300 entries of 0x22 bytes per civ (0x27d8 per
  civ): unit type, x, y, year, killer's colour, killer's name; ring indices
  at 0x6af260 (oldest) and 0x6af280 (next).
- 0x63f0c8: Hall of Fame, 6 records of 0x48 bytes (HALLFAME.DAT), sorted by
  the Civilization Rating short at +0x12; built at 0x63eac8.
- 0x634f60: spaceship parts, 6 rows {label id 198..203, max count, ?}.
- 0x64ba01 is wonder_expires (0x64ba28) - 0x27: indexed by improvement id.
- 0x67a410: "Difference Engine" section records (multiplayer state sync),
  0x18 bytes: size, padded size, id, checksum, mirror offset, data pointer;
  section 1 is the game struct.

## Windows and views

- map_views (stride 0x3f0): +0x124/+0x128 client offset in the view
  surface, +0x2de track-moves mask (labels 300-303), +0x2e8/+0x2ec the top-
  left visible tile.  Caption buttons: 1 close, 2 zoom in, 3 zoom out,
  4 cycle view mode.
- picwnd (picwnd_open): +0x450 report id, +0x460 scroll position, +0x494
  SM_CXVSCROLL, +0x49c per-report parameter.  Advisor screen ids: 1 City
  Status, 2 Defense, 4 Attitude, 5 Trade, 6 Science, 7 Wonders, 8 Top 5
  Cities (menu commands 0x501-0x506, 0x601, 0x602).
- Dialog object fonts: field_8 body, field_c title, field_10 list rows
  (defaults 0x6359e4/e8/ec); 0x635a58 is a 16-deep dialog stack, depth at
  0x635a9c.
- Display scaling: 0x6d4700 caches 16 scale tables of 0x100c bytes
  {numerator, denominator, last-use tick, 1024 ints}, LRU-replaced.
- DirectDraw surface vtable slots used: 2 Release, 5 Blt, 7 BltFast,
  12 GetAttachedSurface, 17 GetDC, 24 IsLost, 26 ReleaseDC, 27 Restore.
- Multiplayer message names: fixed 32-byte strings at 0x628470 (slot 4
  NM_PLAYER_LIST, 38/39 NM_NATION_ACCEPT/REJECT, 44 NM_HEART_BEAT_ACK ...);
  stacked-draw ring of 100 x 0x40 at 0x6ad920 (indices 0x6ad910/0x6ad914).

## More structures (round six)

- Control callback slots: hotspot (wndproc 0x5c8caf) +0x2c enter, +0x30
  leave, +0x34 click, +0x38 press, +0x3c double-click; sprite button
  (0x5cb6eb) +0x2c click, +0x30 right click; scrollbar +0x2c change, +0x30
  thumb track, +0x34 page size, +0x3c position; listbox +0x30 double-click,
  +0x34 selection change; dialog +0x23c sprite click, +0x240 sprite
  double-click, +0x24c selection change; window +0x60/+0x64 drag begin/end.
- NM_PLAYER_INFO (message 0x2e) payload at 0x6ad110: civ slot +0x10, tribe
  +0x14, gender +0x18, names-set +0x1c, leader/tribe/adjective (24 chars
  each) +0x20/+0x38/+0x50, titles-set +0x68, 7 government titles +0x6c.
- Path search: circular queue of 0x900 entries, x at 0x673fc8, y at
  0x6763c8, tail/head 0x673fbc/0x673fc4 (map_path_next_dir).
- Difference Engine sections: 1 game, 2 civ_names, 3 civs, 7 leaders,
  9 scenario block, 11 per-civ block 0x654b40, 12 map header, 23 scenario
  events (50000 bytes at 0x64b98c).
- Diplomacy working state 0x64b0ec-0x64b148 (diplo_eval_ai_stance);
  0x64b118 is the gold the AI demands (999 = handled).
- The game's top level: win_main -> game_main_entry -> game_main ->
  game_play_loop (single player) or the host/client turn loops.
- Possible original bug: palette_reapply_range (0x5de9e0) indexes the RGB
  table with the count instead of the loop index.
