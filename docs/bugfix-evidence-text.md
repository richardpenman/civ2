# Text and choice boundary fixes

Unconditional memory-safety corrections under [BUGS.md](BUGS.md) ("Memory errors of the
original"), in every mode, including classic mode, replays and network games. `BF-*` identifies
Peter Johnson's independent reconstruction's evidence ledger (commit 4cfdd2a); each finding was
checked in our code before it was fixed.

The rule: what the game's own files hold is read, shown and chosen exactly as before. The game's
own text (the 14 LABELS, 37 RULES, 19 GAME, 27 PEDIA, 25 EVENTS, DESCRIBE and DEBUG files of the
game and its scenarios) was checked against every bound below with a separate reader: all of it
fits. A modified file (a scenario's, a translation's) with a longer text gets the text cut where
the buffer ends, as a longer text is cut on screen, or a line the loader refuses; a choice past
the rows a section of the game's text has is asked again or is no choice.

The copies go through two helpers in `src/auto/text.cpp`: `str_copy_max` and `str_append_max`
(copy, append within a buffer of n bytes), and `text_expand_vars_max`, `text_expand_vars`
(`%STRINGn` and the rest) within its destination; `text_expand_vars` itself is that with no
bound, for the unit tests and the callers whose destination is the text buffer's 2472 bytes
(`TEXTBUF_SIZE`). Where a buffer is a local of a part split out of a function, or an `sprintf`
of names, it is larger or an `snprintf`.

## Text into fixed buffers

| Finding | Site | Correction |
| --- | --- | --- |
| BF-029 | `cosmic_editor_write_section` | At the end of the file no line (the original printed its unset buffer); a line with no `;` comment has none (it printed a null pointer). |
| BF-036 | `events_editor_edit_action` parts (MOVEUNIT, CREATEUNIT, CHANGETERRAIN) | The eight coordinates of the events file within the 64-byte default (`snprintf`), which also keeps it within the dialog's 80 (BF-186). |
| BF-038 | `save_file_filter`, `save_filter_add_entry`, `game_save_dialog`, `game_load_dialog` | Each filter's description cut at 48 characters (four entries in the 260 bytes cleared); the dialogs' filters in 0x120 bytes; the monthly save name's month label cut so the name fits 256 bytes. |
| BF-040, 046, 048 | password title, autosave backup path, `events_write_actions` | 0x120, 0x200 and 0x120 bytes (a 24-byte name and a label; a name read from a line). |
| BF-042, 049, 089 | `tech_editor_fill_usage_list`, `unit_editor_edit_sound`, `events_editor_fill_triggers`/`_fill_actions` | `snprintf` into the original's 80, 128 and 256 bytes. |
| BF-043, 044, 054 | terrain base name, the embassy report's two lines, `city_assign_next_name`'s section | 256 bytes (the original's 40, 80, 24). |
| BF-050, 064, 199 | `civ_rating_window_draw`, `hall_of_fame_draw` | The fame line in 256 bytes; its expansion within the text buffer and the Hall of Fame's 132-byte title. |
| BF-051 | `city_button_change_advice`, `text_open_file_section`, `load_text_section` | The advice section name within 80 bytes, the section tag within 80, the dialog's section name within 112 (a longer name is not found). |
| BF-052 | `Dialog::draw_wrapped_text` | A word within 256 bytes (the original's 80), the line within its 256. |
| BF-053 | `PediaWindow::append_unit_fact` | The unit's facts within `draw_unit_page`'s 1000 bytes. |
| BF-056 | `hall_of_fame_add_entry` | Leader and civ names within their 24 bytes. |
| BF-057 | `error_report` | The context (a RULES.TXT line) within its 80 bytes, not over the last open path. |
| BF-059, 186 | `Dialog::load_text_section`, `load_section_keyword` | Lines, titles and buttons expanded within the 256-byte `expanded`; `@DEFAULT=` and the caller's edit default within the 80-byte edit text. |
| BF-060, 062, 063 | the defeat, arrival and Centauri captions; `ThroneRoomWindow`, `DestroyedCivsWindow` | Caption lines within the 1024- and 256-byte captions, expanded within the text buffer. |
| BF-061 | `dialog_prepend_title_label` | The label and the dialog's title within 512 bytes (the original's 128). |
| BF-069 | `PediaWindow::load_improvement_text` | The text within the text buffer it is copied into. |
| BF-081 | `ScenarioEvents::action_play_wave` | The event's sound name within the path (a longer one is not found). |
| BF-082 | `ParleyWindow::build_description` and its parts (`parley_desc_append`) | The offer's description within the 2048 characters it is checked for. |
| BF-084 | `DiploLeaderWindow::science_draw_tech` | The advance's name within the first 50-byte word; words after the fifth left out (the original put them past the five words and read on past them: Verne's rules have names of six and seven words, of which the original shows five, as now); the two lines within 52 and 50 bytes. |
| BF-096 | `labels_load` | At most the 0x378 labels allocated (the game's LABELS.TXT has exactly 888). |
| BF-129 | `rules_load_advances` | An epoch past 3 or a category past 4 is a bad line: the loader's error, as for a bad prerequisite. |
| BF-140, 212, 214, 215, 217, 220 | `PediaWindow::load_*_index` | Each DESCRIBE.TXT index stops at its list (100 advances, improvements 1..38, wonders 39..66, 62 units, 7 governments, 33 terrains, 256 concepts); the concepts stop at the end of the file. |
| BF-150 | `button_underline_hotkey`, `checkbox_underline_hotkey` | The label within 256 bytes; a hotkey past the 255th character is not underlined. |
| BF-151, 152 | `radiogrp_create_items` | A radio label within its 128 bytes; the original's `~` walk (which takes the hotkey position for the row) kept, within the rows. |
| BF-184 | `Dialog::run_modal` (`dialog_list_text_fits`) | A list item longer than the 132-byte buffers does not match the selection. |
| BF-185, 187 | `Dialog::create_controls` | The labels of the first 40 plain options (with more, the original took 40 and walked every option into its 41 labels). |
| BF-190 | `SpaceshipWindow::build_stats_text` and its arrival line | The statistics composed apart and cut to their 512 bytes, the success line to 128. |
| BF-198 | `hall_of_fame_load` (`hall_of_fame_valid`) | A HALLFAME.DAT place with a title past 23, a difficulty past 5 or a name not ending within its 24 bytes is read as no file. |
| BF-200 | `menu_set_item_label`, the diplomacy exchange option, the parley's waiting text | Expansions within 60 characters (the room the menu item was made with), 80 and 80 bytes. |
| BF-204 | `FrameWindow::draw_caption` | The caption in 0x240 bytes (title, a player's name, a label). |
| BF-231 | `unit_spy_city` | A sabotage choice past the 38 improvements is no choice. |

## Choices past a section's rows

| Finding | Site | Correction |
| --- | --- | --- |
| BF-072, 116, 117, 118 | the terrain editor's river, misc, forest/hills/mountains and coast icons | Past 19, 23, 16 and 32 (the original's: the sprite after the coast quarters) is no choice. |
| BF-075, 079 | `setup_hotseat_game`, `setup_pbem_game` | A sixth option is asked again (the original went on with no map set). |
| BF-077 | `events_editor_fill_triggers`, `events_editor_on_help` | An event with no trigger (an empty `@IF`) is listed by its number and has no help (the original listed an unset line, and its search for the trigger's bit never ended). |
| BF-100, 103 | the Foreign Minister | A choice that is not a civ is asked again; an attitude past the nine is no choice. |
| BF-101 | `menu_find_city` | A choice past the 256 cities is no choice. |
| BF-102 | `setup_tribe_city_style`, `setup_new_game_dialogs` | A style past the four is asked again (the high word the original keeps is kept). |
| BF-110 | `Dialog::add_edit_field` | A length that wrapped negative is the longest (255). |
| BF-113 | the six gender prompts | A choice past the two is no choice (as cancel). The leader, government and gender of saves and peers: [bugfix-evidence-files.md](bugfix-evidence-files.md), [bugfix-evidence-network.md](bugfix-evidence-network.md). |
| BF-114, 115 | the improvements' and city graphics' misc icons | Past 8 and 19 is no choice. |

## Other

| Finding | Site | Correction |
| --- | --- | --- |
| BF-297 | `intro_play_opening_video` | The player's audio flags start clear (the original found 0 on its stack; `AviAudio::construct` sets only the stream, and a heap player keeps the debug heap's fill, as in the original). |
| BF-301 | `registry_write_value`, `registry_read_value` | No close of a key that did not open (the original closed its unset handle: harmless in the native builds, whose platform rejects unknown handles; a bad close in the Windows build). |

## Settled

- **BF-014** (the events editor's event moved by `events_compact`): not a defect in ours. The
  events' records always sit first in the pool in list order once compacted (loading a
  scenario, a save or a network game leaves them so), and every editor path that keeps an event
  pointer across `events_compact` (`events_editor_edit_action`, `editor_text_dlg_on_ok`) keeps
  one that is already in its place. Only an event just added sits after the strings, and the
  compaction that moves it ends `events_editor_edit_trigger`, which reads no event after it.
  Checked by logging each record's address before and after each of the 24 compactions of the
  events replay (adding, editing and deleting triggers and actions of each kind): the only
  records that moved were the just-added ones, at that point.
- **BF-164** (the hot-seat player restored with no save): real, B34 in [BUGS.md](BUGS.md).

## Checks

- `CXX='g++ -m32' python3 tests/source_probe.py tests/resource_bounds_test.cpp`: the actual
  bodies of the helpers, `labels_load`, `cosmic_editor_write_section`, `hall_of_fame_load`,
  `error_report`, `dialog_prepend_title_label`, `save_file_filter`, three terrain icon choosers,
  the Civilopedia's wonder index and unit facts and the parley description, with guard bytes
  after each buffer, the game's own values (888 labels, the filters, the original's sprite at
  coast 32) and longer ones; `tests/text_bounds_test.cpp` covers `text_expand_vars`.
- A copy of the game with long labels, Hall of Fame lines of 26 `%STRING0`, longer captions and
  unit facts, run with the AddressSanitizer build on replays that show them.
- The 41 replays, the unit tests and the source probes, extras, the port and Windows builds.

## Not done

- The other unbounded copies of the game's text that the ledger does not list (for example
  `GameMenu::set_item_text` into an item made for a shorter label from another caller, the
  Civilopedia's terrain lines formatted with a LABELS.TXT format string) are as before.
