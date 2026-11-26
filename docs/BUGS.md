# Bugs of the original game

This file lists the gameplay bugs of Civilization II Multiplayer Gold Edition (5.4.0f, patch 3)
that OpenCiv2 reproduces: what goes wrong, where the code does it, and how to fix it. It
covers the bugs found in the code during the reconstruction and the ones the community has
documented and that can be traced to our code.

OpenCiv2 fixes them by default. The player can switch the fixes off in OpenCiv2 Options (the
main menu's last choice: *Bug fixes*, "Fix the original game's bugs", `FixBugs=` in `CIV.INI`'s
`[OpenCiv2]` section), or one by one ([below](#switching-the-fixes)); the choice is apart from the
graphics, so the original's rules can be played with HD graphics too, and without the original's
files. `CIV2_CLASSIC=1` restores the original behaviour whatever is chosen (it also restores the
original look). Replays and unit tests always use the original behaviour: they are compared with
the original. A fix that changes the game's state (**state** below) is also off in network games,
where every player, the original game included, must compute the same game, whatever is chosen. A
fix that changes only what one player sees (**UI only** below) stays on there.

In the code: `plat_fixes` (a bit per fix, by its number) is set at start and again when OpenCiv2
Options changes the choice (`fixes_load`, `src/game/extras.cpp`, where `plat_fixes_allowed`:
not with `CIV2_CLASSIC=1`, in replays or unit tests; `src/platform/main.c`), and two helpers in
`src/include/auto_common.h` test it: `bugfix_state(n)` (off in network games too:
`game.multiplayer` > 2) and `bugfix_ui(n)`. Each fix reads `if (bugfix_state(n)) <fixed> else
<original>` (or the `bugfix_ui(n)` form), marked `B<n>` in a comment, the original's code kept as
it was. `CIV2_TEST_FIXES=1` turns the fixes on in a replay (tests only: such a replay no longer
matches the original's recording); `CIV2_TEST_SETTINGS=Fix.B16=0` (or `FixBugs=0`) switches
them there as `CIV.INI` would.

## Switching the fixes

`FixBugs=1` (the default) or `=0` in `CIV.INI`'s `[OpenCiv2]` section switches all the fixes
below at once; it is the tick of OpenCiv2 Options' *Bug fixes* page. `Fix.B<n>=1` or `=0` (the
number of a fix in this file) switches one of them, and wins over `FixBugs=` for it: with
`FixBugs=0` and `Fix.B16=1` the game has the original's bugs but a city stops at size 127; with
`Fix.B35=0` the advisors call a democracy a republic again, the other fixes on. The keys are
read when the game starts and when OpenCiv2 Options changes the choice; a change takes effect
from the game started or loaded next. The fixes not in this table (B1, B8, B21, B22 in part,
B26, B27, B29 to B31, B39, and the memory errors below) are in every mode and cannot be switched:
the original's behaviour there is a read of stale stack or memory that our build could not
match anyway, or a crash. A **state** fix stays off in network games whatever its key says.

| Key | Kind | Bug |
| --- | --- | --- |
| `Fix.B2` | state | [A new game in the same session keeps the human players' place in the city name list](#b2-a-new-game-in-the-same-session-keeps-the-human-players-place-in-the-city-name-list) |
| `Fix.B3` | state | [The AI's attitude change starts from an uninitialised local ("aggression bug")](#b3-the-ais-attitude-change-starts-from-an-uninitialised-local-aggression-bug) |
| `Fix.B4` | state | [Border intrusions: only the first intruded tile per city is examined](#b4-border-intrusions-only-the-first-intruded-tile-per-city-is-examined) |
| `Fix.B6` | UI only | [A stale parley cancel ends the next granted audience at once](#b6-a-stale-parley-cancel-ends-the-next-granted-audience-at-once) |
| `Fix.B7` | UI only | ["Opponent's Response" popups nest](#b7-opponents-response-popups-nest) |
| `Fix.B9` | state | [The "long move" warning comes too early](#b9-the-long-move-warning-comes-too-early) |
| `Fix.B10` | UI only | [Clicking a citizen in the city window changes the one to its right](#b10-clicking-a-citizen-in-the-city-window-changes-the-one-to-its-right) |
| `Fix.B11` | UI only | [Hot-seat: crash in the status panel between players](#b11-hot-seat-crash-in-the-status-panel-between-players) |
| `Fix.B12` | UI only | [A city opened from an advisor leaves the keyboard with the advisor](#b12-a-city-opened-from-an-advisor-leaves-the-keyboard-with-the-advisor) |
| `Fix.B13` | state | [A ship's unloading choice tests a flag that starts set](#b13-a-ships-unloading-choice-tests-a-flag-that-starts-set) |
| `Fix.B14` | UI only | [An enemy unit that fails to move pauses the game as if seen](#b14-an-enemy-unit-that-fails-to-move-pauses-the-game-as-if-seen) |
| `Fix.B15` | UI only | [Hall of Fame scores over 32767 are hidden](#b15-hall-of-fame-scores-over-32767-are-hidden) |
| `Fix.B16` | state | [A city grows past 127 to a negative size](#b16-a-city-grows-past-127-to-a-negative-size) |
| `Fix.B17` | UI only | [The Defense Minister's Casualties view frames a column at a stale position](#b17-the-defense-ministers-casualties-view-frames-a-column-at-a-stale-position) |
| `Fix.B18` | UI only | [Joining a network game sets "Open Game" from a stale value](#b18-joining-a-network-game-sets-open-game-from-a-stale-value) |
| `Fix.B19` | state | [A ship away from its own city keeps every defender aboard fortified](#b19-a-ship-away-from-its-own-city-keeps-every-defender-aboard-fortified) |
| `Fix.B20` | UI only | [Buttons wider than their dialog run past its left edge](#b20-buttons-wider-than-their-dialog-run-past-its-left-edge) |
| `Fix.B22` | state | [Hot-seat: an AI unit's move is shown, paused for and announced by stale values](#b22-hot-seat-an-ai-units-move-is-shown-paused-for-and-announced-by-stale-values) |
| `Fix.B23` | state | [A MOVEUNIT event for a named unit type moves nothing](#b23-a-moveunit-event-for-a-named-unit-type-moves-nothing) |
| `Fix.B24` | state | [A ship with no port to gather at makes its own city a ferry port](#b24-a-ship-with-no-port-to-gather-at-makes-its-own-city-a-ferry-port) |
| `Fix.B25` | state | [A human unit under autoplay meets a lone diplomat and does what its stack says](#b25-a-human-unit-under-autoplay-meets-a-lone-diplomat-and-does-what-its-stack-says) |
| `Fix.B28` | UI only | [A save whose end could not be written is reported saved](#b28-a-save-whose-end-could-not-be-written-is-reported-saved) |
| `Fix.B32` | state | [Renaming in a rules editor to a shorter name shifts every later name](#b32-renaming-in-a-rules-editor-to-a-shorter-name-shifts-every-later-name) |
| `Fix.B33` | UI only | [The events parser's debug output names a stale civilization](#b33-the-events-parsers-debug-output-names-a-stale-civilization) |
| `Fix.B34` | state | [Hot-seat: a player whose civilization is destroyed in its own turn becomes an earlier player](#b34-hot-seat-a-player-whose-civilization-is-destroyed-in-its-own-turn-becomes-an-earlier-player) |
| `Fix.B35` | UI only | [The advisors call a democracy a republic](#b35-the-advisors-call-a-democracy-a-republic) |
| `Fix.B36` | state | [A ship lost at sea takes every unit on its square with it](#b36-a-ship-lost-at-sea-takes-every-unit-on-its-square-with-it) |
| `Fix.B37` | state | [An AI breaks its treaty with the player on its own side only, and says nothing](#b37-an-ai-breaks-its-treaty-with-the-player-on-its-own-side-only-and-says-nothing) |
| `Fix.B38` | UI only | [A Civilopedia description does not take the keyboard](#b38-a-civilopedia-description-does-not-take-the-keyboard) |
| `Fix.B40` | state | [A popup closes by itself after 20 seconds, also in a single-player game](#b40-a-popup-closes-by-itself-after-20-seconds-also-in-a-single-player-game) |

An earlier version's `Mode=Classic` (OpenCiv2 Options' Mode page, now gone) becomes `FixBugs=0`
at the next start, with Classic graphics and the additions off ([EXTRAS.md](EXTRAS.md#what-classic-mode-was)).

Several bugs read a local the original never set (B1, B3, B13, B19). What such a local holds
is whatever an earlier call left on the stack, which depends on what ran before. With the fix off
the game gives it the value seen in our recordings of the original (under Wine), so it reproduces
those games exactly, not every possible run of the original.

Each entry gives an ID, where the code is (file, function, line at the time of writing), what
goes wrong, the source, and the proposed fix. **Saves** says whether the fix could change what
is saved or break loading the original's saves.

Status of the list: 40 bugs. Fixed unless switched off ([above](#switching-the-fixes)): B2, B3, B4, B6, B7, B9 to B20, B22 (in part),
B23 to B25, B28, B32 to B38 and B40. Fixed in every mode: B1 (our build reads the original's usual
value), B8 (a memory error), and B21, B22 (in part), B26, B27 and B29 to B31, where the original
reads its stack and our build would read another value, so the original's behaviour could not
be matched anyway, and B39 (the same result wherever the original's worked). Not fixed: B5 (dead code, no effect). Memory errors already fixed in every mode, and
stale reads of locals that are not bugs, are listed separately. Reports traced
to code that has no bug are under "Checked, not bugs in MGE". Hot-seat games are not network
games: the state fixes are on there (MGE has no play by e-mail: docs/STATUS.md).

## Game rules

### B1. Commodity demand counts roads on top of an uninitialised value
- **Where:** `src/auto/city.cpp`, `city_calc_commodity_demand` (0043d400; `int road_tiles;`
  line 2881) and its part `city_commodity_count_terrain` (line 2262).
  ```cpp
  if ((tile_flags & TILE_ROAD) != 0) {
    road_tiles++;
  }
  ```
  `river_tiles` and the terrain counts are set to 0 first; `road_tiles` is not.
- **What goes wrong:** the road count starts from whatever the stack holds. Copper demand
  (`(river_tiles + road_tiles + 1) * size_class`, line 2566) and dye demand
  (`commodity_supply.cloth + road_tiles`, line 2589) come out far too high, so caravans find
  dye and copper in demand almost everywhere.
- **Source:** our code. Community: the "dye-copper demand bug", fixed by the CIV2UIA launcher
  ([README, item 40](https://github.com/FoxAhead/Civ2-UI-Additions)), which sets the local to 0
  at 0043d61d.
- **Fix:** `road_tiles = 0` with the other counters. **State** (supply and demand lists are game
  data and drive the AI's caravans).
- **Saves:** no format change. The demand lists in a save are recomputed every 16 turns or when
  the city changes.
- **Status: 0 in every mode.** The reconstruction probed this local in the original
  (`CIV2_STACKPROBE`, tools/gen_c.py FRAME_PRESETS `local_12c`): under Wine its stack held 0 there
  in 687 of 713 calls over 50 turns, an address left by an earlier call in the rest. Our code
  has started it at 0 since then (the line was lost when the function was split into parts, and
  is back), which is the fix. With the fix off the game does not reproduce the original's occasional stale
  address (it depends on what ran before, and differs under Windows, where the community saw
  the bug often); no replay shows a difference. Peter Johnson's runs of the original under Windows ([PR #2](https://github.com/openciv2/openciv2/pull/2), from his independent reconstruction) traced the
  value when it is not 0: the saved frame pointer of an earlier call (pushed at 005ae3bf), a
  stack address, so the count of roads starts from millions.

### B2. A new game in the same session keeps the human players' place in the city name list
- **Where:** `src/auto/game.cpp`, `game_init_new` (004aa9c0), line 6486:
  ```cpp
  for (leader = 0; leader < 0x15; leader++) {
    leaders[leader].times_used = '\0';
  }
  ```
  `leaders[].city_name_index` (the next name from the tribe's list in CITY.TXT) is reset only in
  `civ_create_pick_leader` (`src/auto/civ.cpp` line 4356), which `civ_create` calls only for
  civilizations not played by a human (line 4683).
- **What goes wrong:** after one game, a second new game started without quitting names the
  human player's cities from where the first game stopped, not from the start of the list.
- **Source:** our code. Community: CIV2UIA ([README, item 9](https://github.com/FoxAhead/Civ2-UI-Additions):
  it resets the counter at 004aaa1d).
- **Fix:** reset `city_name_index` with `times_used` in `game_init_new`. **State**, but only
  default city names, at game start.
- **Saves:** none.
- **Status: fixed** (`bugfix_state(2)`).

### B16. A city grows past 127 to a negative size
- **Where:** `City.size` is a signed byte. `city_process_food` (004ebbde) and the celebration
  growth in `city_process_turn` grow a city with no limit but the Aqueduct and Sewer sizes,
  which RULES.TXT caps at 50.
- **What goes wrong:** a city reaching 128 becomes -128: shown negative, and the game's
  arithmetic on its size (workers, happiness, food box) goes wrong. Food limits keep a city of
  the standard rules far below that; modified rules (`@COSMIC` food per citizen 0, say) or
  huge food yields reach it.
- **Source:** community (same page), traced here.
- **Fix:** no growth at 127 (the food box stays full). **State.**
- **Saves:** none (sizes stay in range).
- **Status: fixed** (`bugfix_state(16)`). The cheat menu's Set City Size still accepts any number.

### B25. A human unit under autoplay meets a lone diplomat and does what its stack says
- **Where:** `src/auto/unit_move.cpp`, `unit_move_dir` (0059062c), its part `unit_move_dir_expel`:
  a human player's unit moving onto a lone diplomat of a civilization it has a
  cease-fire or peace with, near that civilization's city, asks "Expel?" (`answer`); under
  autoplay (`autoplay.active`, the debug cheat) the question is not asked and `answer`, a local
  of `unit_move_dir` never set before, is read as it is: 2 stops the move, 0
  expels, anything else attacks.
- **What goes wrong:** under autoplay the unit stops, expels or attacks (breaking the treaty)
  by stale stack.
- **Source:** found by Peter Johnson's reconstruction (BF-105).
- **Fix:** not asked, `answer` is 0: expelled, as an AI unit does. **State.**
- **Saves:** none.
- **Status: fixed** (`bugfix_state(25)`). No replay uses autoplay.

### B36. A ship lost at sea takes every unit on its square with it
- **Where:** `src/auto/unit.cpp`, `unit_delete_with_cargo` (005b5d93): for a ship at sea it calls
  `unit_gather_cargo(unit, 0, 0)`, which marks the ship's cargo `UNITF_ABOARD` (its sentried
  passengers, then others up to its hold), and then ignores that and calls
  `unit_stack_delete_all`, which deletes every unit on the square. Disbanding the ship, losing it
  to its city's support and the other ways a ship is deleted go through it.
- **What goes wrong:** disbanding one of two ships at sea also deletes the other ship, its cargo,
  aircraft flying over the square and any other units there.
- **Source:** community (duplij's civ2.exe fixes on CivFanatics, item 5; issue #24), traced here.
- **Fix:** delete the ship, the units marked aboard, and the owner's land units that the other
  ships on the square can't carry (the last in the stack first; a ship's room is counted as
  `unit_gather_cargo` counts it). Other ships, the rest of their cargo, aircraft in flight and
  other civilizations' units stay. **State.** Tested by `tests/ship_at_sea_test.cpp`.
- **Saves:** none.
- **Status: fixed** (`bugfix_state(36)`).

## AI

### B3. The AI's attitude change starts from an uninitialised local ("aggression bug")
- **Where:** `src/auto/diplomacy.cpp`, `diplo_ai_review_relations` (00560d95), line 6676:
  ```cpp
  int attitude_delta;
  attitude_delta = 0x6715d7a0;  /* stale stack the original reads (FRAME_PRESETS) */
  if (!diplo_review_border_incident(civ,civ2,attitude_delta)) {
  ```
  `diplo_review_border_incident` sets `attitude_delta` only when there is a border incident.
  Otherwise `diplo_review_attitude_delta` adds its adjustments to the stale value and
  `civ_adjust_attitude` applies it.
- **What goes wrong:** in the turns with no border incident, an AI's attitude to a civ it has
  contact with jumps by a stack-garbage amount. In practice the attitude is pushed to the
  hostile end every turn, so AIs stay "enraged" whatever the player does. In our replays the
  WWII scenario shows it: the original's AI goes hostile from this stale value and stops
  calling `rand()`, which is why its recording ends early. The preset above holds the value the
  original's stack has there in that replay.
- **Source:** our code (found reconstructing the WWII replay). Community: the best-known MGE bug
  ([Civilization Wiki, Bugs (Civ2)](https://civilization.fandom.com/wiki/Bugs_(Civ2)), "AI
  attitude ... forced to maximum hostility every turn"); fixed by CIV2UIA
  ([README, item 44](https://github.com/FoxAhead/Civ2-UI-Additions), `UiaPatchAI.pas`: the local
  at `[ebp-30h]` set to 0 at 00560da6); also in Test of Time, where ToTPP has an "AI hostility
  fix".
- **Fix:** start `attitude_delta` at 0. **State.** This changes how AIs treat everyone, so it
  changes play a lot, intended or not.
- **Saves:** none.
- **Status: fixed** (`bugfix_state(3)`). With the fix off the game keeps the preset `0x6715d7a0`. With the fix
  on, the WWII replay diverges where expected (the AI keeps calling `rand()` past the point
  where the original's recording ends). In Peter Johnson's runs of the original under Windows ([PR #2](https://github.com/openciv2/openciv2/pull/2), from his independent reconstruction) the stale value was
  0x00453e93 instead (a return address left by an earlier call at 00453e8e): different, but
  also large and positive, so the attitude goes to the same extreme. His comparison of the
  original with the value started at 0, the game's random numbers the same: an attitude change
  of +2,268,998 against -3, attitude 100 against 0, and a unit that attacked against one that
  held.

### B4. Border intrusions: only the first intruded tile per city is examined
- **Where:** `src/auto/diplomacy.cpp`, `diplo_count_border_intruders` (0055bbc0), line 5385.
  The loop over a city's 20 tiles ends unconditionally after the first tile that has a unit of
  `intruder_civ`:
  ```cpp
  for (tile = 0; tile < 0x14; tile++) {
    ... if (... tile_unit_owner(x,y) != intruder_civ) continue;
    for (unit = tile_first_unit(x,y); ...) { ... count it ... }
    /* only the first tile with a unit of intruder_civ is examined */
    break;
  }
  ```
- **What goes wrong:** the AI counts the intruding units of one tile per city. Intruders spread
  over several tiles are mostly not seen, the "more than 3 intruders" repeat-offence test
  (`if (3 < intruders)`) only sees one stack, and the intrusion weight (roads, mines,
  fortresses under the units) is underestimated. Called from `diplo_review_border_incident`
  (complaints and attitude) and line 4030 (the AI's meeting with the player).
- **Source:** our code.
- **Fix:** remove the `break`, so every tile is examined. **State.**
- **Saves:** none (the unit flags it sets, `UNITF_BORDER_INTRUDER`/`UNITF_BORDER_WARNED`,
  already exist).
- **Status: fixed** (`bugfix_state(4)`: the `break` only with the fix off). The original's
  machine code has it too (re/decomp.c: the loop's end jumps past the loop).

### B5. SAM Battery score tests a city flag nothing sets
- **Where:** `src/auto/city_production.cpp`, `city_choose_production_sam_score` (part of
  `city_choose_production`, 00498e8b):
  ```cpp
  score = 0xc - threat_level;
  score = int_clamp(score,1,0xc);
  if ((cities[city].flags & CITYF_SAM_WANTED) != 0) {
    score -= 3;
  }
  ```
  City flag 0x40000 (`CITYF_SAM_WANTED` in `src/include/enums.h`) is tested here and set nowhere
  in the game, nor in the original (re/decomp.c: its only use of the bit on a city is this
  test).
- **What goes wrong:** the branch never runs.  A lower score is a better choice
  (`city_choose_production_try_improvement` keeps the lowest), so the flag would make a SAM
  Battery more urgent: by analogy with `CITYF_COASTAL_FORT_USED`, which `unit_resolve_attack`
  sets when a Coastal Fortress doubles a defence and the AI's ship routing tests, it most
  likely meant "attacked from the air"; the SAM branch of `unit_resolve_attack` doubles the
  defence and records nothing.  Had it run, `score` is unsigned and `1 - 3` wraps to
  0xfffffffe, which the caller reads as `(int)` -2: a negative score that beats every other
  improvement and unit.
- **Source:** our code.
- **Fix:** needs a decision. Without knowing what the flag was meant to record, the safe fix is
  none: leave it dead (the bug has no visible effect). If we give it that meaning (set it where
  a SAM Battery doubles a defence), clamp the score at 1 after the subtraction. **State** either way.
- **Saves:** a fix that sets the flag writes a bit the original never writes in the city flags.
  The original ignores it everywhere but here, so its saves load either way.
- **Status: not fixed** (dead code: no visible effect).

### B13. A ship's unloading choice tests a flag that starts set
- **Where:** `src/auto/ai.cpp`, `ai_unit_choose_orders` (00538a29), its part
  `ai_orders_transport_unload`: `goal_match` is set to 1 when a cargo unit's goto is on the
  landmass the ship is at, and never cleared; the function does not initialise it. The
  original's stack holds 4 there on every call (probed: tools/gen_c.py FRAME_PRESETS,
  `local_38`).
- **What goes wrong:** the test `if (goal_match != 0)` always passes in the original: every cargo
  unit's `counter` is set to the turn (`game.turn & 7`, the AI's "considered this turn" mark),
  not only those headed for this landmass.
- **Source:** our code (stale-stack reads found by the reconstruction).
- **Fix:** start `goal_match` at 0. **State** (AI). Its effect on play is small and not measured.
  In Peter Johnson's runs of the original under Windows ([PR #2](https://github.com/openciv2/openciv2/pull/2), from his independent reconstruction) the value was 0x013fde70 on two calls: not 4, but
  non-zero too, so the test passes there as well.
- **Saves:** none.
- **Status: fixed** (`bugfix_state(13)`).

### B19. A ship away from its own city keeps every defender aboard fortified
- **Where:** `src/auto/ai.cpp`, `ai_unit_choose_orders` (00538a29), its parts
  `ai_orders_city_guard` and `ai_orders_transport_cargo`. `needed_defenders`, the number of
  defenders the ship's city needs, is set only when the ship is in its own city (and is a
  transport): by the city's size, walls, palace and danger. Elsewhere the function never sets it,
  and `ai_orders_transport_cargo` reads it for each defender aboard that is not fortified:
  ```cpp
  else if ((unit_types[units[unit].type].role == ROLE_DEFEND) &&
          (defender_count < (int)needed_defenders)) {
    defender_count++;
    units[unit].orders = ORDER_FORTIFYING;
  ```
- **What goes wrong:** the stale value is huge (0x64a8c19c in our recordings, 0x013fde70 in
  Peter Johnson's runs of the original under Windows ([PR #2](https://github.com/openciv2/openciv2/pull/2), from his independent reconstruction)), so away from its own city every defender aboard is fortified
  instead of being woken. A fortified defender adds nothing to the roles the ship carries
  (`tasks_mask`), which choose where it sails: a city to stage at, or an enemy city to attack.
  A ship carrying only defenders has no destination for them, and they stay aboard until it
  returns to its own city.
- **Source:** our code (stale-stack reads found by the reconstruction); the varying value from
  Peter Johnson's runs of the original under Windows ([PR #2](https://github.com/openciv2/openciv2/pull/2), from his independent reconstruction).
- **Fix:** start `needed_defenders` at 0: away from its own city no city needs the defenders, so
  they are woken and count toward the ship's destination, like the other units aboard.
  **State** (AI). No replay reaches this path (counted over long50, scenario, ww1, napoleon and
  conquest: no AI ship away from its own city carried a defender that was not fortified), so
  the effect is not measured.
- **Saves:** none.
- **Status: fixed** (`bugfix_state(19)`). With the fix off the game keeps the preset `0x64a8c19c`.

### B21. A city's wonder score doubled by its other locals when it builds something else
- **Where:** `src/auto/city_production.cpp`, `city_choose_production` (00498e8b), its part
  `city_choose_production_try_wonder`: the score of a wonder the city does not
  build is doubled when the civilization (`civ5`) has its bit in `wonder_builders[-0x27 -
  building]`, the 28-byte mask of the civilizations building each wonder. For a city building
  a unit or an improvement (`building` > -0x27) that index is -100 to -1: below the array. The
  original reads its other locals there (EBP-0xcc minus 1 to 100); our build would read past
  its array.
- **What goes wrong:** an AI city building a unit or an improvement scores a wonder twice as
  high (and one more) when a stale byte has the civilization's bit.
- **Source:** found by Peter Johnson's reconstruction (BF-006, BF-007, BF-025, BF-202).
- **Fix:** no rivals for a city that does not build a wonder: the mask is read only for a
  wonder (`city_choose_production_rivals`). **State** (AI).
- **Saves:** none.
- **Status: fixed in every mode.** No replay reaches the read: over the 41 replays no AI city
  scored a wonder there (counted with a temporary probe), so the fix changes none of them.

### B24. A ship with no port to gather at makes its own city a ferry port
- **Where:** `src/auto/ai.cpp`, `ai_unit_choose_orders` (00538a29), its part
  `ai_orders_transport_port` (line 4318): `needs_units` (local_dc) is set by the loop over the
  candidate port cities; a transport that finds none, in its own city, reads it as the stack
  left it. The original's stack held 0x21f238 there (a stack address) on every call
  (`CIV2_STACKPROBE=538a29:dc:4`, the Civil War scenario, 54 calls).
- **What goes wrong:** the value is never 0, so such a ship always marks its city a ferry port
  (`CITYF_AI_FERRY_PORT`) and posts AI goals there for settlers, defenders, attackers and
  diplomats, whether the city's landmass needs units or not.
- **Source:** found by Peter Johnson's reconstruction (BF-090).
- **Fix:** `needs_units` starts at 0: the landmass's own test (`ai_continent_defense_quota`,
  the policy) decides. **State** (AI). No replay reaches this path (counted over the 41 replays).
- **Saves:** none.
- **Status: fixed** (`bugfix_state(24)`). With the fix off the game keeps the preset `0x21f238`.

## Diplomacy and multiplayer

### B6. A stale parley cancel ends the next granted audience at once
- **Where:** `net.nm_parley_cancel_count` (`src/data/state/network.h`), a counter of received
  `NM_PARLEYREQUEST_CANCEL` messages, raised for every one received
  (`NetSession::process_messages` counts each type, `src/auto/network_messages.cpp`). The
  message's handler (`case NM_PARLEYREQUEST_CANCEL` in `NetSession::dispatch_diplomacy_message`)
  ignores a cancel that matches no pending request but leaves the count. The
  requester's code then reads it: `diplo_civs_meet_parley` (`src/auto/diplomacy.cpp` line
  5739), the Foreign Minister's audience request (`src/auto/reports.cpp` line 1753),
  `net_popup_parley_request` (`src/auto/network.cpp`), `mp_meeting_wait_idle`. Only
  `net_heartbeat_send` and those paths clear it.
- **What goes wrong:** an audience granted too late (after the requester gave up) opens the
  other player's parley window alone; its End Negotiations sends a cancel the requester no
  longer expects. The count stays, and the requester's next granted audience ends at once with
  "broken off negotiations". Seen with the network test against the original
  (`docs/xdaemon-protocol.md` §11.1); the original does the same.
- **Source:** our code (network tests).
- **Fix:** clear the count when a request is sent (`diplo_civs_meet_parley`, the reports path).
  **UI only**: the count is local (never sent), and a granted audience opening the requester's
  parley window is the protocol's normal course, which the other side, which granted it,
  expects.
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(6)`, also in network games). Only the clearing on a new request;
  the handler still counts every cancel (counting only matching ones would also change when the
  receiver's `net_popup_parley_request` closes). Checked with `tools/xdnet/nettest.py --diplo
  --after-late fixed` (one more audience after the late grant: the requester's parley window
  must open and its End Negotiations send a cancel): passes ours-ours and ours (the requester)
  against the original, every other check of the run passing too. `--after-late classic`
  (the window must not open: the original as requester, `orig-ours`) is there for the bug
  itself but has not been run yet.

### B7. "Opponent's Response" popups nest
- **Where:** `src/auto/network_parley.cpp`, `parley_poll_net_replies` (004b81dd). Every popup
  it opens (PARLEYCOUNTEROFFER, PARLEYNOTHANKS, PARLEYACCEPT) gets
  `dlg_style.pending_idle_callback = parley_poll_net_replies`: while the popup waits, the same
  function processes network messages and can open the next reply's popup inside it. The
  other side ending the negotiations (`diplo_wnd_close_active`, `src/auto/diplomacy.cpp`, its
  PARLEYCANCEL popup with `turn_popup_timeout_idle`, which also processes messages) adds another.
- **What goes wrong:** two or more "Opponent's Response" boxes stack up, the last on top, each
  needing OK, the ones underneath after the parley window is gone. Those no longer have the
  keyboard (the window's closing gave it to the main window): Enter there goes to the map
  (End of Turn), only a click closes them.
- **Source:** our code (network tests). Reproduced by `tools/xdnet/nettest.py --nest`, both
  ways: A offers B 10 gold, B accepts, and while A's "The offer has been accepted!" is showing,
  B ends the negotiations; A's popup takes the cancel and "...have broken off negotiations!"
  opens inside it. Every time, in the original (ours-orig: the original as the client; orig-ours:
  as the host) and in ours before the fix.
- **Fix:** `parley_response_popup`: a response popup (those four) that comes while one is
  showing waits, and is shown when that one closes, in the order they came. What the game does
  on the message (the parley's mode, `NM_PARLEY_EXECUTE` sent or the transaction executed, the
  window closed) is done at once as before, so the other player sees the same and nothing on
  the network changes. The window closing meanwhile leaves the keyboard with the popup and
  gives it to the main window when the last one has closed. **UI only** (`bugfix_ui(7)`:
  the fix off and replays keep the original's nesting).
- **Saves:** none.
- **Status: fixed by the flag.** `nettest.py ours-orig --nest` and `orig-ours --nest`: the
  original's side shows the two popups at once, ours one after the other, Enter closing each;
  both runs pass otherwise as before (every message received in order and byte for byte), and
  so do `--diplo` both ways. `ours-ours --nest --classic`: ours with `CIV2_CLASSIC=1` nests them as
  the original does.

### B18. Joining a network game sets "Open Game" from a stale value
- **Where:** `src/auto/game_setup.cpp`, the network setup (the function with `setup_net_host` and
  `setup_net_join`): `saved_open_game` is saved before hosting (which sets `open_game = 1`)
  and restored after the loop, but Join skips the save.
- **What goes wrong:** a joining player's `mp_options.open_game` gets the low 16 bits of a stale
  local: the Game Profile shows Open or Closed at random, and if that player later hosts the
  game (host migration) it is advertised as open or closed at random.
- **Source:** our code (`-Wmaybe-uninitialized`).
- **Fix:** start `saved_open_game` from `mp_options.open_game`. **UI only** in effect (a local
  option, set before the game; nothing another player computes from it but whether it may join).
- **Saves:** none (`open_game` is not saved; CIV2.DAT stores `menu_prefs.mp_open_game`).
- **Status: fixed** (`bugfix_ui(18)`).

### B34. Hot-seat: a player whose civilization is destroyed in its own turn becomes an earlier player
- **Where:** `src/auto/game.cpp`, `game_play_loop` (0048b340), lines 5346-5376. Before an AI
  civilization's turn in a hot-seat game the loop saves the player (`saved_human_civ`) and sets
  human_civ's low byte to -1; after the turn, whenever the civilization is not human, it puts
  the saved player back. A human civilization that loses its last city while its own turn is
  processed (`civ_process_turn`: the city starves at size 1, or builds Settlers at size 1 and
  the player lets it go) is destroyed and made an AI civilization (`civ_check_destroyed`, which
  clears its human bit), so the restore runs with no save on that turn.
- **What goes wrong:** the player becomes the one saved at the last AI civilization's turn (or,
  with none yet in the session, whatever the original's stack held).
- **Source:** found by Peter Johnson's reconstruction (BF-164).
- **Fix:** put the player back only when this turn saved it. **State.**
- **Saves:** none.
- **Status: fixed** (`bugfix_state(34)`); with the fix off the game restores the player saved at an earlier
  turn, as the original, and keeps the player when no turn saved one (the original's stack).

### B37. An AI breaks its treaty with the player on its own side only, and says nothing
- **Where:** two places.
  - `src/auto/civ.cpp`, `civ_turn_update_diplomacy` (00560084), line 7104. An AI at cease-fire
    or peace with a civilization is given `TREATY_HOSTILE_INTENT` (0x20) when it decides on war
    in a meeting but has a treaty (`src/auto/diplomacy.cpp`, line 4090), or when the AIs gang up
    on the leading civilization (`GAME_AI_GANG_UP`, line 3154). At the start of that AI's turn,
    a roll that hits more often the worse the other's reputation ends the treaty:
    ```cpp
    *treaty_ptr = *treaty_ptr & 0xffffffd9;   /* no cease-fire, peace or hostile intent */
    *treaty_ptr = *treaty_ptr | 0x80840;      /* broken, aggressor */
    ```
    only in `civs[ai].treaties[other]`.
  - `src/auto/spaceship.cpp`, `spaceship_launch` (line 1616): at Emperor and Deity, when a
    human launches a spaceship that arrives before an AI's, the same two lines end the AI's
    treaty with the human, again on the AI's side only.

  Every other change of a treaty is made on both sides (`diplo_set_treaty_flags`,
  `diplo_clear_treaty_flags`).
- **What goes wrong:** the player's side still records peace, and nothing is shown: the
  Foreign Minister says peace while the AI has only contact. The AI's attack then comes with no
  warning, and costs it no reputation (`diplo_declare_war` looks at its side and finds no
  treaty to break).
- **Source:** our code. Community: a report by a Russian modder of the original game (the AI's
  status showing contact where the player's showed peace, then a sudden attack); his patch also
  changes when the AI breaks the treaty (vendetta, reputation, relative power), a rule change
  not made here.
- **Fix:** in both places, end the treaty on both sides. When the other civilization is the
  player at the screen, the AI's emissary tells them, for a peace treaty and for a cease-fire
  (`diplo_treaty_break_notice`, `src/game/extras.cpp`). It's one popup of the game's usual width,
  titled as the game's emissary messages are ("Enraged French Emissary"). It has the AI's emblem
  in its colour beside the emissary's words ("Hear now the words of the French: we no longer
  consider ourselves bound by our peace treaty with you."; OpenCiv2's text, translatable). Then
  comes "You reply:", with the player's emblem beside it, and our reply as quoted text: GAME.TXT's
  `@CANCELOURPEACE` for a peace treaty, `@NOPEACE` for a cease-fire. It's not a choice, because a
  choice is one line and widened the dialog to fit it.

  Who is told follows how the game tells of other treaties broken (`diplo_treaty_break_tell`):
  - **The player who is the other party:** the emissary's notice, then for a peace treaty
    the Diplomats Report `CANCELPEACE` ("French and Spanish cancel peace treaty."), as an
    attack that breaks one shows them (`unit_combat.cpp`: `SNEAK`, then `CANCELPEACE`). A
    cease-fire gets the notice only, as combat gives no report for a cease-fire.
  - **A third party at the screen** (one AI breaking its peace with another): the Diplomats
    Report, if they have an embassy with either civ, the United Nations or Marco Polo's, or the
    map is revealed, as when two AIs cancel peace (`diplomacy.cpp`, `diplo_civs_meet_war`).
  - **No one** in autoplay, or with no player at the screen (hot seat, during an AI's turn).
  - **Network games:** the fix is off there. Combat sends its report to each human player who
    saw it (0x3a), which would be the model.

  A cease-fire running out on its own (`@CEASEEXPIRE`, `diplomacy.cpp` line 4102) is unchanged.
  How often the AI breaks a treaty is unchanged. **State.**
- **Saves:** none (the existing treaty flags).
- **Status: fixed** (`bugfix_state(37)`). `tests/ai_treaty_break_test.cpp` and
  `tests/spaceship_treaty_test.cpp` check both modes; `tests/treaty_tell_test.cpp` checks
  who is told and in what order, and `tests/treaty_notice_test.cpp` checks the notice.

## Scenario and events

### B8. Compacting the events frees the string pool the editor still points into
- **Where:** `src/auto/game_events.cpp`, `events_compact` (004cef35). It copies the events
  out, resets the events (freeing the string pool and allocating a new one) and copies them
  back, while its callers keep pointers into the old pool: `editor_text_dlg_on_ok`
  (`src/auto/editor.cpp` line 1575) writes the text lines through
  `editor_lists.text_lines_out`, and `events_editor_edit_action` reads `event->actions`.
- **What goes wrong:** a use after free. It works in the original only because the Windows heap
  gives the new pool the same block; another allocator corrupts or loses the event's text.
- **Source:** our code (AddressSanitizer, commit dae23f1).
- **Fix:** done in every mode, since the behaviour is the original's: `events_reset_keep_block`
  (line 7438) keeps the pool's block, as the Windows heap does. No flag needed.
- **Saves:** none.

### B23. A MOVEUNIT event for a named unit type moves nothing
- **Where:** `src/auto/game_events.cpp`, `ScenarioEvents::action_move_unit` (004fb29f):
  the units to move are those with `event->move_unit_type != unit` false, where `unit` is the
  slot number in the unit list, not the unit's type (`units[unit].type`). `unit=ANYUNIT` (-2)
  works.
- **What goes wrong:** a MOVEUNIT with a unit name sends only a unit whose slot number happens
  to equal the type number, usually none. The Civil War scenario's two turn-1 events that send
  the Confederate Blockade Runners on their way do nothing.
- **Source:** found by Peter Johnson's reconstruction (BF-015).
- **Fix:** compare with the unit's type. **State.**
- **Saves:** none.
- **Status: fixed** (`bugfix_state(23)`). Test: `harness/extras/x-b23-moveunit.txt` (the Civil
  War scenario, with the fix and without: fixed, a Blockade Runner sails on the AI's turn).

### B32. Renaming in a rules editor to a shorter name shifts every later name
- **Where:** the rules editors' OK: `tech_editor_save_items` (00415307, `src/auto/civ.cpp`
  line 74), the improvements' (`src/auto/city_editors.cpp`), units' (`src/auto/unit_editors.cpp`), terrain's (`src/auto/map.cpp` line 4611) and the governments' and commodities'
  (`src/auto/editor.cpp` lines 280 to 294) copy each name into its string pool entry at the
  editor's width (20, 25, 15, 15, 15 and 10), padding with NULs. A pool entry can be longer
  (a scenario's RULES.TXT).
- **What goes wrong:** a shorter name leaves the end of the longer old name after the NULs;
  the pool (`label_string`, 00428b0c) counts it as an entry, so every later name in the pool
  is read one entry on, and the next renames are written there.
- **Source:** found by Peter Johnson's reconstruction (BF-047).
- **Fix:** the rest of the old name is cleared too (`editor_copy_pool_name`,
  `src/include/auto_common.h`). **State** (the rules).
- **Saves:** none (the rules editors' RULES.TXT gets the names as shown).
- **Status: fixed** (`bugfix_state(32)`). Not reached with the game's own rules: none of their
  names is longer than its editor's width.

### B33. The events parser's debug output names a stale civilization
- **Where:** `src/auto/game_events.cpp`, `ScenarioEvents::parse_trigger_received_tech` and
  `parse_trigger_city_taken` (lines 9153 and 9226): with `@DEBUG` in EVENTS.TXT the "king id"
  of a receiver or attacker is printed from `ps->parsed_civ`, which these triggers do not set:
  another line's civilization, or nothing set at all.
- **What goes wrong:** a wrong number in the scenario author's debug output (stdout).
- **Source:** found by Peter Johnson's reconstruction (BF-235).
- **Fix:** print the civilization just read. **UI only.**
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(33)`).

## User interface

### B9. The "long move" warning comes too early
- **Where:** `src/auto/unit_move.cpp`, `unit_move_dir_long_move` (part of `unit_move_dir`, 0059062c),
  line 7475:
  ```cpp
  if ((units[unit3].orders == ORDER_GOTO) || ((units[unit3].flags & UNITF_AUTOMATED) != 0)) {
    units[unit3].aux_timer++;
    ...
    if ('/' < (char)units[unit3].aux_timer) {
      city3 = dlg_game_txt_with_unit((const char *)&str_longmove,unit3);
  ```
  The step counter (`Unit.aux_timer`, offset 0x0e) is not reset when a new goto starts:
  `map_window_click` (`src/auto/map.cpp`, the goto at 00411419) and `unit_cmd_goto_city`
  (`src/auto/unit.cpp`, 0058d6af) start the move with whatever the counter holds from earlier
  orders (the byte also serves other purposes, e.g. line 2947).
- **What goes wrong:** the "this unit has been moving for a long time, keep going?" question
  appears after a few steps of a new, short goto. Answering it cancels the orders.
- **Source:** our code. Community: CIV2UIA
  ([README, item 20](https://github.com/FoxAhead/Civ2-UI-Additions), "Reset MoveIteration
  before start moving"; patches at 00411419 and 0058dda0).
- **Fix:** set `aux_timer` to 0 when a human gives a goto order in those two places. Only human
  moves ask (`unit_move_by_local_human`), but the counter is unit data and orders can be
  cancelled, so treat it as **state**.
- **Saves:** none (the byte is saved already).
- **Status: fixed** (`bugfix_state(9)`): the map click in goto mode, Go To a city
  (`unit_cmd_goto_city`), and the city window's "go to this city" for a unit
  (`city_window_supported_unit_menu`, which CIV2UIA does not patch).

### B20. Buttons wider than their dialog run past its left edge
- **Where:** `src/auto/window_dialog.cpp`, `Dialog::layout_buttons` (part of `Dialog::compute_layout`,
  0059fd2a). Every button gets the width of the widest label (or an equal share of the window,
  if larger) and they are placed from the right edge leftward; the window does not grow when
  they do not fit.
- **What goes wrong:** a dialog with a fixed width and a long button label cuts the leftmost
  button off: the casualties popup at the start of a turn (GAME.TXT `@CASUALTIES`,
  `@width=220`) shows "...alty Timeline" for "Zoom to Casualty Timeline".
- **Source:** reported by the user playing the Rome scenario.
- **Fix:** when the buttons are wider than the window, the window grows to hold them.
  **UI only.** Test: `harness/extras/x-b20-buttons.txt` (with the fix and without).
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(20)`).

### B10. Clicking a citizen in the city window changes the one to its right
- **Where:** `src/auto/city_window.cpp`, `CityWindow::draw_citizens`, which adds one hotspot
  per citizen:
  ```cpp
  rect_set_xywh((iptr)hotspot_rect,x2,top + offset,pos,height);
  clickmap.hotspot_add_rect(i,2,(RECT *)hotspot_rect);
  x2 += pos;
  ```
  Clicks go to `city_window_on_mouse` → `city_window_cycle_specialist`.
- **What goes wrong:** in big cities the citizens are drawn closer than their sprite width, and
  the hotspots, cut at the spacing, do not match the sprites: a click a little right of a
  specialist's centre changes the next one.
- **Source:** community, CIV2UIA ([README, item 4](https://github.com/FoxAhead/Civ2-UI-Additions)).
  The rectangles start at each sprite's left edge (`x2 += scale(5)`, the sprites' own start),
  one spacing wide; a citizen's figure is in the middle of its 27-pixel sprite, so when the
  spacing is smaller than the sprite its visible figure is mostly over the next one's rectangle.
- **Fix:** each hotspot centred on its citizen: start at `scale(5) + scale(14) - spacing/2 - 1`
  instead of `scale(5)` (CIV2UIA's formula, its patch at 00502203: `(zoom * 19 - spacing) / 2 -
  1`). **UI only.**
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(10)`).

### B11. Hot-seat: crash in the status panel between players
- **Where:** `src/auto/game_status.cpp`, `status_draw_unit_panel` (00569801), its part for the tile
  under the unit: `if ((game.reveal_map == 0) && (human_civ != 0)) tile = known_at(x, y,
  human_civ)`. CIV2UIA patches that `jnz` to `jg` at 00569ec7. During an AI civilization's turn
  in a hot-seat game the turn loop sets human_civ's low byte to -1 (`game_play_loop`,
  `(*(i8 *)&human_civ) = -1`), and `known_at` then reads `map_data.known[-1]`, which is
  `known_handles[7]`, a memory handle, not a pointer.
- **What goes wrong:** the game can crash drawing the unit panel during the switch between
  hot-seat players.
- **Source:** community, CIV2UIA (`UiaPatchMultiplayer.pas`). Located: the disassembly at
  00569ebf is `cmp [human_civ], 0; jne`, then `known_at(x, y, human_civ)` (00403823 ->
  005b898b). No replay reaches it (the panel is not drawn with that tile during the AI's turns
  there).
- **Fix:** read the human player's view only when `human_civ > 0`, the whole map's otherwise.
  **UI only.**
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(11)`). With the fix off the game reads `known[-1]` as the original does, which
  can crash in ours too.

### B12. A city opened from an advisor leaves the keyboard with the advisor
- **What goes wrong:** opening a city from an advisor's list leaves the focus on the advisor:
  the city window's keys do nothing, and Escape closes the advisor instead.
- **Source:** community, CIV2UIA ([README, item 21](https://github.com/FoxAhead/Civ2-UI-Additions)).
- **Where:** `src/auto/city_window.cpp`, `CityWindow::open` (shows the window, `SurfaceWindow::show`:
  ShowWindow and BringWindowToTop, no SetFocus) and `city_window_close` (gives the focus to the
  main window). The advisors open cities from their click handlers (e.g.
  `attitude_advisor_on_click`). Reproduced in the native build: a replay (F4, a click on Rome,
  Escape) closes the advisor behind the city window, which stays.
- **Fix:** the city window takes the focus when it opens (`WndBase::set_focus`, CIV2UIA's patch
  of 0040138e), and gives it back to an open advisor (`advisor_state.report_id >= 0`) when it
  closes (CIV2UIA: 00509985). **UI only.**
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(12)`). The same replay with the fix: Escape closes the city window,
  a second Escape the advisor.

### B14. An enemy unit that fails to move pauses the game as if seen
- **Where:** `src/auto/unit_move.cpp`, `unit_move_dir` (0059062c): `civ_sees[8]` (which players see
  the move) is filled only when the unit moves; a move that stops early still reads
  `civ_sees[human_civ]` to pause (`ui_delay_ticks`) after an enemy's move. The original's stack
  held non-zero there for players 1 to 5 and zero for 6 and 7 (FRAME_PRESETS; probed in the
  Rome scenario, where the human player is 6).
- **What goes wrong:** every failed move of another civilization's unit, seen or not, makes the
  human player's game wait a moment, which also hints at units the player cannot see.
- **Source:** our code (stale-stack reads).
- **Fix:** `civ_sees` all 0 at the start. **UI only** (a delay).
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(14)`).

### B15. Hall of Fame scores over 32767 are hidden
- **Where:** `src/auto/reports.cpp`: `hall_of_fame_add_entry` (00436f5a) stores the score in a
  signed 16-bit field (`HallOfFameEntry.score`, HALLFAME.DAT); `hall_of_fame_draw` shows an
  entry only if `score > -1` and prints it signed. Entries are ranked by the civilization
  rating (`rating`), not the score.
- **What goes wrong:** a score of 32768 or more is negative: the entry, ranked by its rating
  (often first), is not shown.
- **Source:** community ([Civilization Wiki, Bugs (Civ2)](https://civilization.fandom.com/wiki/Bugs_(Civ2))).
- **Fix:** read the field as unsigned (0..65534; 0xffff is an empty place): shown unless 0xffff,
  printed unsigned, a new score clamped to 65534. **UI only.**
- **Saves:** HALLFAME.DAT keeps its format; the original hides such an entry, as it does today.
- **Status: fixed** (`bugfix_ui(15)`).

### B17. The Defense Minister's Casualties view frames a column at a stale position
- **Where:** `src/auto/reports.cpp`, the Defense Minister's draw function (0042e220): the
  Casualties view draws a frame at `lost_x`, which is set only by the rows it lists.
- **What goes wrong:** with no casualty to list (early in a game) the frame is drawn at whatever
  the stack held: a stray line in the civilization's colour, or nothing.
- **Source:** our code (`-Wmaybe-uninitialized`).
- **Fix:** no frame when no row was listed. **UI only.**
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(17)`).

### B22. Hot-seat: an AI unit's move is shown, paused for and announced by stale values
- **Where:** `unit_move_dir` (0059062c, `src/auto/unit_move.cpp`) and `unit_paradrop`
  (004ca39e, `src/auto/unit.cpp`) read `civ_sees[human_civ]` and `civ_animates[human_civ]`
  (whether this player sees the move and sees it animated): in `unit_move_dir_landing_alert`,
  the two ways of moving the stack (`unit_move_dir_relocate`, `unit_move_dir_relocate_via_host`),
  the pause after an enemy's move (`unit_move_dir_finish_move`), and the paradrop's
  announcement and animation (`unit_paradrop_announce`, `unit_paradrop`). During an AI civilization's turn
  in a hot-seat game human_civ's low byte is -1 (B11): the original reads the local before
  each array, in `unit_move_dir` the hut flag (local_a4, 0 until the unit has moved) and
  `civ_sees[7]`. After Reveal Map's Barbarians (human_civ 0) it reads entry 0, which it never
  sets.
- **What goes wrong:** an AI move in a hot-seat game is paused for when the unit entered a hut,
  animated when player 7 saw it, and an AI landing is counted (`continent_military`, which
  the AI's choices read) and announced to nobody only when those locals say so.
- **Source:** found by Peter Johnson's reconstruction (BF-074, BF-237, BF-238).
- **Fix:** with no player here (human_civ -1 or 0) nobody here sees the move
  (`unit_move_dir_local_view`). **State** (the landing count).
- **Saves:** none.
- **Status: fixed** (`bugfix_state(22)`) in `unit_move_dir`; with the fix off the game reads what the
  original reads: `has_hut`, `civ_sees` and `civ_animates` are one array in the original's
  order (`views`), and entry 0 is 0. In `unit_paradrop` the original reads locals not set at
  that point (the approach direction, `civ_sees[7]` only set for a human player 7), so the fix
  is in every mode (`unit_paradrop_local_view`). No replay reaches these reads with human_civ
  -1 or 0 (counted over the 41 replays).

### B26. Cancelling the custom tribe's portrait picks a portrait by stale stack
- **Where:** `src/auto/graphics.cpp`, `PortraitDialog::run` (004965ff), line 2105:
  `civs[civ].leader_portrait = (u8)result`; only OK sets `result` (`portrait_dlg_on_button`),
  and the dialog is a local of `portrait_dlg_show` (`src/game/unwind.cpp`).
- **What goes wrong:** choosing a portrait for a custom tribe in a new game, then Cancel (or
  closing the dialog), gives the leader a portrait chosen by a stale byte; it is saved.
- **Source:** found by Peter Johnson's reconstruction (BF-034).
- **Fix:** `result` starts at the current portrait: Cancel keeps it. **State** (saved).
- **Saves:** none.
- **Status: fixed in every mode.** No replay cancels the dialog.

### B27. Disband with no active unit can offer to disband a random city
- **Where:** `src/auto/unit.cpp`, `unit_cmd_disband` (0058c295), line 5927: in moving-units
  mode with no active unit, `x2`/`y2` are not set, and in cheat mode the code looks for a city
  at that square to offer "Disband city?".
- **What goes wrong:** with cheat mode on and no unit active, Disband can name a city at a stale
  square, and Yes deletes it.
- **Source:** found by Peter Johnson's reconstruction (BF-163).
- **Fix:** no active unit, nothing to disband: return. **State** (after a Yes).
- **Saves:** none.
- **Status: fixed in every mode.**

### B28. A save whose end could not be written is reported saved
- **Where:** `src/auto/game_save.cpp`, `save_write_game` (004741be): the file is closed
  at the end (`ScopedFile`) and the result of the close, which writes what is still buffered,
  is not looked at.
- **What goes wrong:** on a full disk the save is cut short and the game says it was saved.
- **Source:** found by Peter Johnson's reconstruction (BF-070).
- **Fix:** the file is closed before returning, and a failed close is a failed save
  (SAVEERROR, as the other write errors). **UI only** (the message; the file is the same).
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(28)`).

### B29. The High Council's first advisor button may not clear the speech area
- **Where:** `src/auto/reports.cpp`, `CouncilWindow::draw_speech` (line 6853) reads
  `speech_advisor_index`, which the `CouncilWindow` constructor and `init` do not set,
  on the first button (`draw_speech(-1)`); the window is a local of `council_show`.
- **What goes wrong:** the area is cleared and repainted only when the stale value is 0 to 2.
- **Source:** found by Peter Johnson's reconstruction (BF-067).
- **Fix:** start it at 0. **UI only.**
- **Saves:** none.
- **Status: fixed in every mode.**

### B30. Famine redraws a stale square
- **Where:** `src/auto/city.cpp`, `city_process_food` (004ebbde), line 10161: after a famine
  that disbands a settler or cancels a food route, the area redrawn is at `x2`/`y2`, set only
  when the city shrinks. In the same function the message's name, "FAMINE0" copied into a
  6-byte local and a 1-byte digit after it (the original's two locals filled by one copy), is
  now one 8-byte array.
- **What goes wrong:** the redraw covers a stale area instead of the city (a display matter:
  the settler's removal redraws its own square).
- **Source:** found by Peter Johnson's reconstruction (BF-010). `-Wmaybe-uninitialized` had listed it as harmless.
- **Fix:** the city's square. **UI only.**
- **Saves:** none.
- **Status: fixed in every mode** (with the message name's array: a memory error in our build).

### B31. Shield waste drawn at a stale position on a full production bar
- **Where:** `src/auto/city_window.cpp`, the city window's shield bar (`draw_resource_map`, 005025d5):
  `mid_x`, where the waste icons start, is set only when the waste's part of the
  bar is not 0 wide after the support and surplus parts are measured.
- **What goes wrong:** with very large production and some waste, the waste icons are drawn at
  a stale x.
- **Source:** found by Peter Johnson's reconstruction (BF-031).
- **Fix:** the same centring when that part is 0 wide. **UI only.**
- **Saves:** none.
- **Status: fixed in every mode.**

### B35. The advisors call a democracy a republic
- **Where:** the title line of the advisors (`attitude_advisor_draw`, `defense_minister_draw` and
  the casualty view in `src/auto/reports.cpp`, `trade_advisor_draw` and `city_status_draw` in
  `src/auto/city_advisors.cpp`, `science_advisor_draw` in `src/auto/civ.cpp`): the label is
  `0x148 + int_clamp(government - 1, 0, 4)`, of Empire, Kingdom, People's Republic, Holy
  Empire, Republic and United States (0x148..0x14d).
- **What goes wrong:** the clamp to 4 shows Democracy as "Republic of the ..."; the "United
  States" label is never used.
- **Source:** community (duplij's civ2.exe fixes on CivFanatics, item 24; issue #24), traced here.
- **Fix:** clamp to 5. **UI only.** (Anarchy still shows "Empire".)
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(35)`; also in the `CityStatus` extra's own City Status view).

### B38. A Civilopedia description does not take the keyboard
- **Where:** `src/game/retvals.cpp`, `PediaWindow::load_description` (the description's edit
  control, `description_edit`, is filled and shown, but the focus stays with the Civilopedia's
  window).
- **What goes wrong:** Page Up, Page Down and the arrows do nothing in a description longer than
  its box: only its scroll bar (and with `MouseWheel` the wheel) scrolls it.
- **Source:** Peter Johnson (PR #11, "Fix scrolling in Civilopedia descriptions").
- **Fix:** the description takes the focus when it is shown (`Control::set_focus`). **UI only.**
  Back at the list (Go Back), the arrows move the list's selection as before; Escape does what it
  did.
- **Saves:** none.
- **Status: fixed** (`bugfix_ui(38)`). `harness/extras/x-pediakeys.txt` (with `CIV2_TEST_FIXES=1`).

### B39. A dot in a folder's name taken for the file's extension
- **Where:** `src/game/regargs.cpp`, `path_default_extension` (00472950) and
  `path_set_extension` (004729ab) look for the extension's dot with `strrchr` in the whole
  path; `save_read_game` (`src/game/retvals.cpp`) and `save_write_game`
  (`src/auto/game_save.cpp`) with `strchr`, and `game_load_dialog` looks for ".SCN" anywhere
  in the path.
- **What goes wrong:** after a load, the game's text file is `<save's folder>\GAME`. On Linux
  the saves are under `~/.local/share/OpenCiv2/GameData`: the dot of ".local" counts as
  GAME's extension, ".TXT" is not added and GAME.TXT cannot be opened. A Diplomat or Spy
  moved into a foreign city then does nothing (`spy_city_mission_menu` stops at
  `SPYOPTIONS` as if cancelled), and the pillage and diplomacy menus, the historians and
  the throne room, which read GAME.TXT, fail too. A file name typed without an extension
  in such a folder was saved without ".SAV", and `path_set_extension` cut such a name at the
  folder's dot.
- **Source:** issue #31, reported by blank-file (with the fix).
- **Fix:** the dot is looked for in the file's own name, after the last `\`, `/` or `:`
  (`path_file_part`). In every mode: a path whose folders have no dot comes out as before
  (the original's Windows folders, the recordings). The replay script's file dialog
  (`src/platform/replay.c`) sets `nFileExtension` the same way.
- **Saves:** none.
- **Status: fixed in every mode.** `tests/path_extension_test.cpp`;
  `harness/extras/x-diplomat.txt` loads `SCENARIO\..\Ca_Auto.SAV` and moves a Diplomat into a
  foreign city: the mission dialog opens (before the fix, nothing).

### B40. A popup closes by itself after 20 seconds, also in a single-player game
- **Where:** `src/auto/game.cpp`, `play_loop_casualty_popup` (part of `game_play_loop`,
  0048b340), line 2303. At the start of each civ's turn (not in autoplay), it arms the network
  games' popup timeout for the next dialog, the casualties report:
  ```cpp
  dlg_style.pending_idle_callback = turn_popup_timeout_idle;   /* closes the dialog after 20 s */
  ...
  if (0 < casualties) { ... dialog2->show_modal(0); }
  ```
  The next dialog's window takes it (`Dialog::create_window`, `window_dialog.cpp` line 2538).
- **What goes wrong:** with no casualties no report is shown, and the timeout stays armed. The
  next popup of that turn, whatever it is, takes it and closes by itself after 20 seconds
  (`turn_popup_timeout_idle`, `game.cpp` line 1113), with its default answer. The popup after it
  waits as usual. Seen in a single-player game: "Spanish acquire The Wheel from the Great
  Library!" closed by itself, the next ("... Warrior Code ...") waited, and a Travellers Report
  at the next turn's start closed by itself.
- **Source:** our code (a user's report).
- **Fix:** the timeout is armed only when the report is shown, just before it. The report keeps
  its timeout. In network games every dialog has it anyway (`dlg_style.default_idle_callback`,
  `game.cpp` line 561). **State** (a choice that closes by itself takes its default answer).
- **Saves:** none.
- **Status: fixed** (`bugfix_state(40)`). `tests/popup_timeout_test.cpp` checks both modes.

## Memory errors of the original (fixed in every mode)

These do not change the game when the original "gets away with them", so OpenCiv2 fixes them
for good and keeps the original's results:

- BMP export reserved bytes and the initial RLE8 cursor (BF-011/BF-073):
  [evidence and focused regression](bugfix-evidence-bitmap.md).
- `events_compact` freeing the string pool its callers still use (B8).
- An AI goal added for civilization -1 (`civ_add_ai_goal`, `civ_add_standing_goal`): the goals
  of `civs[-1]` lie over the improvements table (`improvements[19..54]`), whose names, costs
  and upkeep they overwrite, and the game crashes drawing a name. The path found:
  `unit_reveal_surroundings_far` (unit.cpp) takes a square's owner from
  `tile_city_or_unit_owner`, which says -1 when the square's unit flag has gone out of step
  with the units on it (as `unit_stack_validate`'s repairs can leave it) while
  `tile_first_unit` still finds one. Reported by duplij (civ2.exe fixes on CivFanatics, item
  55; issue #24). The goal is not added.
- `diffeng_rle_encode` (the network difference engine) reading one byte past its input; the
  decoded data is the same.
- Saved unit/city counts and PCM format chunk extents (BF-091/BF-111/BF-305):
  [evidence and focused regression](bugfix-evidence-files.md).
- What a save, scenario or map indexes: the game record and civs, map dimensions and squares,
  units and cities, the scenario block, zoom, history and resume civ, saved event triggers
  (Peter Johnson's ledger BF-093, 094, 123..128, 134..136, 166, 172, 176, 193, 201, 224, 230,
  232..234, 236, 281, 282, 284, 287, 288, 291); `CIV2.DAT`'s network names (BF-226): a file
  outside the domains the game writes is refused as one that does not read
  ([evidence, focused regression and remaining work](bugfix-evidence-files.md)).
- A path longer than 79 characters (a save loaded from outside the game's folder) copied whole
  into the 80-byte last-open path and a 136-byte local; event and unit sound paths shorter than
  the scenario folder (BF-058, BF-296).
- Text sections without their closing `@` line read past the end of the file (BF-121);
  `%STRINGn`/`%NUMBERn`/`%HEXn` outside the arguments and `%HEX`'s endless padding of a number
  above 0xffff (BF-197); BMP info headers too short and RLE writes outside the image (BF-097,
  BF-098); the TERRAIN1 export's sprite height read past its block (BF-013).
- The science advisor's first trim length from an unset line (BF-024), a trade route written at
  an unset slot (BF-026), a healing facility unset for a fourth unit domain (BF-028), an events
  file action before any `@IF` (BF-030), a cycle of advance prerequisites recursing until the
  stack ran out (BF-173); the scenario variants' file searches never closed.
- `net_send_message`'s stack buffers, too small for the largest messages.
- Receive envelopes, discovery/chat payloads and diagnostic reads:
  [evidence, focused regression and remaining work](bugfix-evidence-network.md).
- Received message bodies: each message type needs the bytes its handler reads, and a peer's civ,
  unit, city, map-lock, popup, rule-table and record indices must lie inside our tables (Peter
  Johnson's ledger BF-130..133, 137, 167, 169, 171, 178, 196, 203, 205..211, 213, 216, 218, 221, 222,
  227, 228, 233, 240..245, 247..275, 277, 278; and `NM_QUIT_REQUEST`'s civ): the same
  [evidence page](bugfix-evidence-network.md).
- `diffeng_rle_decode` taking the encoder's 32767-byte run (code 0x7fff) for a 65535-byte literal,
  past what it allocated, and reading codes past either end (BF-092). The game's own streams never
  have that code (difference records are at most 1 KB; full images are never sent): only a crafted
  message reached it.
- Host migration with a player missing from the old list, and `diffeng_buf_append`'s overflow
  check, going on past the original's fatal assertion (BF-162, BF-083).
- Stack bytes in the gaps of the discovery, answer, path and player messages (BF-286, BF-293).
- The Leonardo notice and the caption for a civ whose player left (`net_civ_slots` -1; BF-194,
  BF-203); the diplomat's chat reading past its 15-entry attitude table (BF-289, network only).
- `spaceship_wnd_launch`'s local, smaller than the star field object built in it.
- Text of modified game files (a scenario's, a translation's) copied into the original's fixed
  buffers without a bound: names, labels and lines into 24- to 1024-byte locals and members,
  `%STRINGn` expansion into its callers' buffers and the text buffer, Civilopedia indexes past
  their lists, a dialog's options past its 40 and a word past its 80 (Peter Johnson's ledger
  BF-029, 036, 038, 040, 042..044, 046, 048..054, 056, 057, 059..064, 069, 081, 082, 084, 089, 096,
  129, 140, 150..152, 184..187, 190, 198..200, 204, 212, 214, 215, 217, 220, 231): a text that fits
  is copied as before, a longer one is cut where the buffer ends, as on screen
  ([evidence and focused regression](bugfix-evidence-text.md)).
- Choices past the rows a section of the game's text has, from rows a scenario's GAME.TXT or
  DEBUG.TXT adds (hot-seat and e-mail game start, custom city style, gender, Find City, the
  Foreign Minister's civ and attitude, sabotage, the icon editors' sprites; BF-072, 075, 077,
  079, 100..103, 110, 113..118): asked again or no choice, where the original indexed its
  tables with them (the same [evidence page](bugfix-evidence-text.md)).
- The opening video's audio flags never set (BF-297), and the registry key closed when it was
  not opened (BF-301; a bad close in the Windows build only).
- A throne room opened while another is showing (`throneroom_show`, `throneroom_show_upgrade`):
  the window's destructor clears `throneroom_wnd`, and the outer room's next click or key reads
  through it (`throneroom_on_click`, `throneroom_on_key`): the original crashes (an access
  violation at 0044f4dd, recorded under Wine: a replay's `call throneroom_show` in an
  improvement's final screen or its dissolve). Not reached in play: the room covers the screen,
  takes the keys (Shift+H and Alt+K, H do nothing while it shows, in the original as in ours) and
  the improvement comes only from the turn's start; a replay's `call` reached it
  (`harness/hd/hd-throne.txt`, where a click lost in a dissolve let the next improvement open
  inside the last). `src/game/throneroom.cpp` makes the outer room the throne room again when
  the inner one has gone, so its click ends it (`tests/throneroom_nested_test.cpp`).

## Uninitialised locals that are not bugs

The original reads some locals it never sets; our code starts them at the value the original's
stack held (probed with `CIV2_STACKPROBE`; tools/gen_c.py FRAME_PRESETS), in every mode:

- `map_path_next_dir` (004abfe5, `src/auto/map.cpp`): `civ`, the owner looked up last, starts
  at 0 (the original's stack: 0 in 3892 of 4300 calls, -1 in 158). The neighbour test then
  compares with the previous tile's owner when the path has no owner (`pathfind.owner_civ` <
  0, distance estimates only). A start of 0 never matches; it is the natural value. Not a bug.
- `ai_unit_choose_orders` (00538a29, `src/auto/ai.cpp`): besides `goal_match` (B13),
  `needed_defenders` (B19) and `needs_units` (B24), `defender_count` (0x108; written before every
  read), `cand_score` (0x3bd) and `best_score` (0) are set by the paths that read them.
  `target_policy` (local_2c, 0x21f2dc: `CIV2_STACKPROBE=538a29:2c:4`, every call in the Civil
  War scenario) is read when a ship found no port city: only whether it is 4 matters (the
  goals' priority), and the stale value is not (BF-009 in Peter Johnson's reconstruction).

The native build compiled with `-O2 -Wuninitialized -Wmaybe-uninitialized` (98 warnings) was
triaged: 2 real stale reads (B17, B18), 15 in the exception-handling scaffolding, 10 harmless
(a value nobody uses, e.g. return values every caller ignores; the famine redraw, B30, was
among them), 71 false positives (the read guarded by
the condition that writes it).

## Checked, not bugs in MGE

- **Goto across the date line** (reported for Test of Time on CivFanatics, ToTPP thread):
  MGE's path finder works relative to the goal across the wrap (`map_path_next_dir` normalises
  x to the goal's side, `map_path_flood` keeps unwrapped coordinates and looks tiles up with
  `map_wrap_x`, `unit_goto_next_dir` and `map_distance`/`map_dx_wrapped` take the short way round,
  and the step to an adjacent tile across the line uses only the direction's sign). Not
  reproduced in the code.

## Considered, not bugs

Well-known Civ2 "bugs" that are design, balance or art, or fixed before 5.4.0f:

- **Stack kill** (all units on a tile outside a fortress or city die with their defender):
  the rules.
- **AI production, trade and contact bonuses by difficulty**: the design.
- **Caravans on a wonder, then the production switched to another wonder or improvement
  without penalty**, **disbanding in a city for half the shields**: exploits of the rules as
  written.
- **Goto stops on entering a zone of control** (but caravans don't), **the unit rotation
  with Wait**, **engineers on the same tile each keeping their orders** (CIV2UIA items 17–19):
  the original's design; CIV2UIA changes them as options.
- **Revolutions timed by turns divisible by four** ("Oedo years"): the rules.
- **Buildings with no upkeep missing from the Trade Advisor**, **the default zoom on large
  screens**: choices, not mistakes.
- **"Major objective" cities (x3)**: a Test of Time feature CIV2UIA adds, not an MGE bug.
- **Louis XIV and Joan of Arc portraits replaced by Alexander and Hippolyta, the furthest
  throne room wall replaced by the second level**
  ([Civilization Wiki](https://civilization.fandom.com/wiki/Bugs_(Civ2))): errors in the art
  of `mk.dll` and `pv.dll`, not in the code. The right pictures are not in the game's files,
  so the code cannot fix them.
- **A click while the throne room's choices dissolve in is lost**: the original drains the
  mouse and keys when the dissolve ends (`input_flush_events` in `ThroneRoomWindow::run_upgrade`),
  as before its first screen, so a click meant for "continue" cannot pick a section unseen
  (recorded under Wine: the same in ours). A click while the chosen piece's room dissolves is
  kept and ends the last screen as soon as it shows. Kept in every mode.
- **Fixed before or in MGE 5.4.0f** (the patch notes shipped with the game, `PATCH.TXT` and
  `Readmev13.txt`): units without a home city after city 127, Pikemen in double-movement games,
  bribing under Democracy, disbanding a unit several times in a laggy network game, enemy
  ships appearing in cities in scenarios, and the earlier 1.0x fixes.
