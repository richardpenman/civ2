# File-loading boundary fixes

These are unconditional memory-safety corrections under [BUGS.md](BUGS.md), including
classic mode, replays and network games. No valid record layout, game rule or packet format
changes. `BF-*` identifies Peter Johnson's independent reconstruction's evidence ledger.

| Finding | Original and current site | Evidence and port decision |
| --- | --- | --- |
| BF-091 | `004732a6`, `save_read_units_cities` | Serialized signed counts reach bulk reads (`count * 32` / `count * 88`) or legacy per-record loops without a bounds check. Negative counts become huge bulk sizes; excessive positive counts cross the backing. Reject before reads and ID fixup, using **this port's** `power_graph_history_block.units[2048]` and `cities[256]`. Do not import the other reconstruction's 2,050-unit extent or change creation limits. |
| BF-111 | `005d4c5f` / `005d4f6a`, both WAV loaders | A short `fmt ` chunk leaves sample-rate/bit-depth or other copied PCM fields unwritten. A complete read shorter than 16 bytes follows the existing unsupported-format result 10; a truncated read follows result 8. Both close the file and publish no sound record. |
| BF-305 | Same WAV loaders | The original reads the declared format size into a 16-byte local; OpenCiv2 uses an 18-byte `WAVEFORMATEX`, still too small for arbitrary extensions. Read the 16 retained bytes and consume extensions in bounded 256-byte pieces. No allocation proportional to untrusted size, signed-length conversion or overwrite of neighboring locals. |

## Second tranche: what a save, a scenario or the game's files index

The same rule, in every mode: a save, scenario or map is refused as one that does not read
(the load's existing failure: the game says it cannot load the file), a settings file falls
back to its defaults, a text section without its end stops where the file ends. The domains
are those the game writes; all 91 saves and scenarios at hand (the game's 44, including
version 0x27 scenarios, and the replays' autosaves, hot-seat and network saves) and the 11
maps pass them, checked with a separate reader of the format.

| Finding | Site | Correction |
| --- | --- | --- |
| BF-058, BF-296 | `file_open_path` (`src/auto/text.cpp`); `ScenarioEvents::action_play_wave`, the unit sound editor | The path is built in 0x208 bytes (the original's 136; a save loaded from outside the game's folder keeps its whole folder, and sound lookups go through it) and its first 79 characters go to `error_state.last_open_path` (80); a shorter path leaves the same bytes as the original's `strcpy`. The event sound's path (240) and the unit sound editor's (256) take the scenario folder (264) as well. |
| BF-134..136, 172, 176, 201, 230, 291 | `save_game_record_valid`, end of `save_load_gpk` (`src/auto/game.cpp`) | Difficulty 0..5, barbarians 0..3, the civ whose turn it is and the player's 0..7, the saved local civ 0..7 in a game on this computer (network games take theirs from the session), power ranks 0..7, AI opponents (counted from the civs alive) 0..6; each civ's tribe 0..20, research -1..99, government 0..6, leader/tribe/adjective/title names ending within their 24 bytes. |
| BF-094 | `map_header_valid`, `map_read` (`src/auto/map.cpp`) | Before the old map is freed: an even width of at least 2, a height of at least 1, `(width/2)*height` below 32768 (`map.area` is 16 bits). Saves, scenarios and `.MP` maps. |
| BF-236, 287, 288 | `map_tiles_valid`, `map_read` | Each square's terrain 0..10, region 0..63, the civ on it 0..7 or none (15). |
| BF-093, 123..128, 193, 232, 233 | `save_records_valid`, end of `save_read_units_cities` | Active units: type 0..61, owner 0..7, stack links -1..2047, home city none or an active city, a caravan's (role 7) commodity 0..15 or none, a unit with goto orders on the map. Active cities: owner 0..7, at most 3 trade routes with partners 0..255 and goods at most 15 (negative: food), commodities supplied and demanded -15..15 (negative: in use), production -66..61, name ending within its 16 bytes. |
| BF-224, 284, 282, 166, 281 | `save_read_game` (`src/game/retvals.cpp`) | The scenario name ends within its 80 bytes and its objective civ is 0..7; map window zoom -7..8; destroyed-civ history count 0..12; a multiplayer save's civ to resume with 0..7. |
| BF-234 | `save_event_trigger_valid` (saved events); `events_editor_on_add_trigger` | A saved event's trigger is none or one of the nine kinds; a ReceivedTechnology trigger with a civ (whose advance the editor names) waits for an advance 0..99. The editor adds no event for a choice past its nine triggers (the list comes from the game's text). |
| BF-226 | `prefs_load_civ2dat` | The four 32-byte network names in `CIV2.DAT` must end within their bytes, else the network defaults (as for a short file). |
| BF-121 | `text_read_line_or_end` | The readers that use a line without checking it (Civilopedia, credits and reports, the improvements' help, the cosmic editor; 25 sites) get "@" at the end of the file, the end of the section they read to, in place of a null line. |
| BF-197 | `text_expand_vars` | `%STRINGn` within the 24 string arguments, `%NUMBERn` and `%HEXn` within the 6 numbers (another is left out); `%HEX` pads to 4 digits only a shorter number (the original's unsigned `4 - len` padded a longer one without end). |
| BF-097, BF-098 | `gfx_load_bmp_640x480` (`src/auto/graphics.cpp`) | An info header of at least 40 bytes (the compression and colours used are read); an RLE row outside the row table is no position (an end of line after the last row, as encoders write, is still accepted) and a write there or outside the image fails the load with its existing "error reading bitmap image". |
| BF-013 | `sprite_sheet_save_terrain1` | The improvement's own sprite height, not sprite 11's, 660 bytes into a 184-byte block (the TERRAIN1 export). |
| BF-024 | `DiploLeaderWindow::init_science` | `line2_buf` starts empty: the first advance's trim length came from it unset and could put the NUL past the 53-byte line. |
| BF-026 | `city_add_trade_route` | With every route worth 9999 or more no slot is picked; none is replaced (the original wrote at an unset slot). |
| BF-028 | `civ_heal_units` | The facility starts at 0 (none) for a unit type of another domain than land, air or sea. |
| BF-030 | `ScenarioEvents::parse_file` | The parse state starts zeroed; an action after `@THEN` with no `@IF` before it is a bad events file (the original wrote through an unset event pointer). |
| BF-173 | `advance_has_prereq` | The search visits each advance once: a cycle of prerequisites (a scenario's rules; Bonaparte's have one, and the original takes them, BF-304) gives an answer instead of running out of stack; elsewhere the answer is the same. |
| (ours) | `game_load_dialog` scenario variants | The two `FindFirstFileA` searches for a scenario's `ALT*` files are closed (one handle leaked per scenario load). |

Run `CXX='g++ -m32' python3 tests/source_probe.py tests/file_bounds_test.cpp` (the save, map and
event checks, valid endpoints and each field past them, and through the reader) and
`tests/text_bounds_test.cpp` (paths, text arguments, unterminated sections, prerequisite cycles,
the trade slot, the events parser); the BMP cases are in `tests/bitmap_undefined_test.cpp`.

The Bonaparte scenario's `TORPEDOS.WAV` was the independent reconstruction's real-world
18-byte PCM witness. The original admitted and played it, despite writing past its local.
OpenCiv2 already accommodates 18 bytes; the fix preserves that case and additionally handles
larger extensions without overflowing `WAVEFORMATEX`. Existing rate/bit-depth checks and the
first 16 published format bytes remain unchanged; no new audio codec is inferred.

Base checked: `17842e4497c5a6d3d5be9245cf080ce0dde535fc`. Run:

```sh
CXX='g++ -m32' python3 tests/source_probe.py tests/file_bounds_test.cpp
# Or: set CXX=cl, in an x86 Visual Studio developer shell, then the Python command.
```

The probe compiles the actual three loader bodies and shared format reader with production
serialized/Win32 types and stub file/MMIO/allocation providers. Debug and optimized runs
cover negative/excess/zero/max counts in three save versions, short reads, both WAV entry
points, 0..15-byte formats, 16/18/1024-byte formats, a huge declaration with short backing,
unsupported bit depth, retained PCM fields, cleanup and absence of allocation on rejection.
It does not execute the physical audio device, full file-loading UI or replay suite.

The first tranche was limited to record **counts** and PCM format **extents**; the second
covers the fields above. A rejected load may already have imported its game header and
part of the rest (as a truncated file does); this does not claim an atomic whole-save
transaction.

## Remaining work

- The same domains in state images and difference records from a peer (`recv_state_diff`,
  `NM_JOIN_GAMESTATE`, the map and owner/region/terrain messages): a join's units and cities
  arrive in separate records, and a check after each record would refuse a valid join midway
  (Peter Johnson's reconstruction had to defer it to the join's ready marker), so one check
  function does not serve both; see [bugfix-evidence-network.md](bugfix-evidence-network.md).
- BF-014 and BF-164 are settled in [bugfix-evidence-text.md](bugfix-evidence-text.md).
- The rest of the multiplayer block (passwords, e-mail addresses) and the events' other
  indices (unit types, civs and terrain of their actions) are read as before.
- BF-024's trim takes the length of the wrong line (a display difference, only with an
  advance whose first word is wider than the column); only its memory error is fixed.
