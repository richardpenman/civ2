# Playing without the original's install

What OpenCiv2 still takes from an installed Civilization II (MGE), file by file and resource by
resource, and what stands in for it.  Videos (`Video\`, `KINGS\`, `LOSER.AVI`: the game runs
without them, as the recorded scenarios always have) and scenarios (`SCENARIO\`, `*.SCN`, `*.MP`)
are out of scope; music is listed only.

## How it was measured

`CIV2_AUDIT=1` logs every file the game asks for and every resource, in the hook log
(`CIV2_HOOK_LOG`), a line each:

* `audit path found|missing|create|dir WINPATH`: each path the platform resolves
  (`plat_host_path`: the C runtime's `fopen`, `CreateFile`, `OpenFile`, `GetFileAttributes`, the
  sounds, the videos, the libraries);
* `audit find PATTERN`: `FindFirstFile`;
* `audit lib found|missing|builtin NAME`: `LoadLibrary`;
* `audit res found|missing MODULE TYPE NAME`: each resource looked up (`FindResource`,
  `LoadBitmap`, the cursors and icons), in each module the game searches (civ2.exe first, then its
  libraries in turn: `res_load_impl`);

It was run on every recorded scenario (`harness/scripts`, 44), every extras, HD and media script
(`harness/extras`, `harness/hd`, `harness/media`): the new game's set-up, the map at every zoom,
units, cities, the city window and view, every advisor and report, diplomacy, the High Council
(without its videos), the Civilopedia (every category, the tech tree), wonders, the throne room,
the spaceship, the Hall of Fame, the end of the game, the editors (cheat, sprites, rules, events),
the menus, the file dialog and the multiplayer set-up.  Then with the install's files removed
(`tools/native-run.sh`): its text files and `RULES.TXT` (`CIV2_NO_TEXT=1`, `CIV2_NO_RULES=1`), its
`.DAT` files (`CIV2_NO_DAT=1`) and its `Sound` folder (`CIV2_NO_SOUND=1`).  Without the text,
rules and sounds the 44 scenarios run to their end with as many screenshots; without the `.DAT`
files too a fresh install's first start sets up and plays a new game.  For its pictures (the
sheets, `*.GIF`; the libraries, `*.dll`; `civ2.exe`) the game has no stand-in yet: the next step
(the pictures made from the HD art).

## Summary

| Status | Files | Resources |
| --- | --- | --- |
| replaced: built in (OpenCiv2's text, rules, timerdll, XDaemon) | 22 | |
| replaced: made by the game when missing (`CIV2.DAT`, `HALLFAME.DAT`) | 2 | |
| needs code: the pictures, to be made from the HD art (the pack has a picture for each, at the original's size, and the sprites taken from them) | 18 | 243 GIF + 30 of civ2.exe's |
| missing art: no picture in the pack | 3 (`EDITORAS`, `EDITORSA`, `EDITORSQ.GIF`) | |
| missing sound: the game plays nothing | 78 (`Sound\*.WAV`) | |
| out of scope: videos, music | 59 videos (`Video` 37, `KINGS` 21, `LOSER.AVI`), 11 music tracks | 21 (mk.dll `CTAB`, the heralds' video palettes) |
| droppable: never read by the game | 22 | |

## The game folder's files

| File | Where the game uses it | Status |
| --- | --- | --- |
| `RULES.TXT` | the rules (units, advances, improvements, terrain, governments ...) | replaced: built in (`src/game/rules_default.cpp`; `CIV2_NO_RULES`) |
| `GAME.TXT`, `MENU.TXT`, `PEDIA.TXT`, `DESCRIBE.TXT`, `HELP.TXT`, `LABELS.TXT`, `ADVICE.TXT`, `CITY.TXT`, `COUNCIL0`-`2.TXT`, `CREDITS.TXT`, `SCREDITS.TXT`, `FCREDITS.TXT`, `MPCREDITS.TXT`, `TUTORIAL.TXT`, `DEBUG.TXT`, `HERALD.TXT`, `MAPMENU.TXT` (19) | every text the game shows | replaced: OpenCiv2's own English (`data/native/text`; `CIV2_NO_TEXT`) |
| `timerdll.dll` | the game's multimedia timers (5 functions: `SetTimerID` ...) | replaced: built in (`src/platform/resources.c`, never read) |
| `XDaemon.dll` | network play | replaced: built in (`src/platform/xdaemon.c`, never read) |
| `CIV2.DAT` | the preferences (game options, the multiplayer set-up) | replaced: the game's defaults when missing (`prefs_load_civ2dat`: `prefs_set_defaults`, `prefs_set_mp_defaults`), written when changed, as by the original on a fresh install |
| `HALLFAME.DAT` | the Hall of Fame | replaced: empty when missing (`hall_of_fame_load`), written at a game's end |
| `CITYPREF.TXT`, `EVENTS.TXT`, `*.SAV`, `*.HOT`, `CITY.TMP`, `RULES.BAK`, `EVENTS.BAK` | the player's own files (asked for, made by the game) | the player's: none needed |
| `TERRAIN1.GIF`, `TERRAIN2.GIF`, `UNITS.GIF`, `CITIES.GIF`, `ICONS.GIF`, `PEOPLE.GIF` | the map's and the interface's sprites (`gfx_load_*_sprites`; the `.BMP` of each is looked for first, never there) | needs code: made from the pack's sprites (below) |
| `EDITORPT.GIF` | the sprite editor's tools and backdrops | needs code: made from the pack's sprites |
| `CITY.GIF` | the city window's background | needs code: the pack's picture |
| `SCREDITS.GIF` | the credits' picture | needs code: the pack's picture |
| `EDITORAS.GIF`, `EDITORSA.GIF`, `EDITORSQ.GIF` | the editors' backdrops | missing art |
| `Civ2Art.dll`, `Intro.dll`, `cv.dll`, `mk.dll`, `pv.dll`, `ss.dll`, `Tiles.dll`, `Wonder.dll` | the pictures of the windows (below) | needs code: made from the pack's pictures |
| `civ2.exe` | its resources (below); the game looks for it (`file_require_cdrom`) | needs code: made from the pack's pictures |
| `Sound\*.WAV` (78) | the sounds (below) | missing sound: silence |
| `Music\Track02`-`12.wav` (11) | the CD's music | out of scope (the user's own music is planned) |
| `Video\*.AVI` (37), `KINGS\*.AVI` (21), `LOSER.AVI` | the opening, the High Council, the wonders, the launch, the heralds, the defeat | out of scope: the game runs without them |
| `SCENARIO\`, `*.SCN`, `*.MP`, `ROME.TXT`, `WWII.TXT`, `ALEXANDR.TXT` | scenarios and premade maps | out of scope |
| `CIV2FANW.TXT`, `CIV2FAQ.TXT`, `PATCH.TXT`, `README.TXT`, `Readmev13.txt`, `LEADERS.TXT`, `MISC.TXT`, `SC.TXT`, `TUTORIAL.SAV`, `Civ2Map.exe`, `Uninst.isu`, `Vfwfix.reg`, `smeds.log`, `XDaemon.log`, `EReg\` (8 files) | read by no part of the game (documents, the installer, the map editor's program, logs, registration) | droppable |

## The pictures

What the game takes from each and what the pack has for it (`docs/HD-GRAPHICS.md`): the
statuses are those for pictures made from the pack's at the original's size and layout.

### The sheets

| Sheet | Sprites the game takes | In the pack | Status |
| --- | --- | --- | --- |
| `TERRAIN1` | the terrain, its resources, irrigation, farmland, mines, pollution, the hut, roads, railroads, the masks | 62 | replaced; the masks (the map's picking, the blends' dither) to be made by code |
| `TERRAIN2` | rivers, forests, hills, mountains, coasts, river mouths | 100 | replaced |
| `UNITS` | the 63 units, the shield | 64 | replaced; the shields' places a fixed default (12, 8): the art gives none |
| `CITIES` | cities by style and size, walled, flags, fortress, airbase | 68 | replaced; the flags' and size boxes' places a default |
| `ICONS` | the map's cursor and grid, improvements', wonders' and advances' icons, the resource and tax icons, explosions, the windows' textures, the caption's buttons | 133 | replaced; 11 sprites have no picture in the pack, left empty: three 32 x 32 beside the pollution icon (y 223), three beside the map's cursor (y 256), the two tall ones at x 311, the pair at x 350 (y 255, 288), the unused tall one |
| `PEOPLE` | the citizens | 55 | replaced |
| `EDITORPT` | the sprite editor's tools, the units' and tiles' backdrops | 17 | replaced |

### The libraries' GIF resources (243)

| Module | Ids | Where the game uses them | Status |
| --- | --- | --- | --- |
| `tiles.dll` | 50-59, 65, 66 | the advisors' backgrounds, the tax rates | replaced: OpenCiv2's own paintings (`data/native/hd-extra`) |
| | 70-77 | the announcements' pictures (city captured, disorder, celebration, founded) | replaced: the pack's |
| | 85, 86 | the diplomacy's and the icons' sheets (crops) | replaced |
| | 90 | the new game's and main menu's backdrop | replaced |
| | 95 | the credits | replaced |
| `intro.dll` | 901-913 | the new game's set-up pictures | replaced |
| `cv.dll` | 300, 305, 310 | the city view's improvements, wonders and houses (crops) | replaced |
| | 340-353 | the city view's landscapes (site x 5 + era) | replaced |
| | 399 | the city view's icons | replaced (greys) |
| `mk.dll` | 200-206 | the diplomacy rooms, by government | replaced: the pack's rooms are laid out otherwise (the leader's medallion hidden behind a panel without HD) |
| | 210, 211, 215 | the diplomacy room's panels (crops) | replaced |
| | 220-261 | the leaders' portraits | replaced |
| | 299 | the diplomacy's tile | replaced (greys) |
| | 10000-10002 | the destroyed civilizations' window | replaced |
| `pv.dll` | 100, 105-184 | the throne room's hall and its pieces by section and level | replaced, layout differs: the pack's pieces are vignettes placed otherwise; HD mode composes the room whole (`hd_throne_room`); without HD it shows misplaced pieces: **needs code** (an 8-bit composition) or art |
| `ss.dll` | 400-499 | the spaceship's window, its parts and cells | replaced, layout differs: HD mode composes the ship (`hd_spaceship`); without HD the parts are misplaced: **needs code** or art |
| | 20000-20007 | the spaceship's launch and arrival frames | replaced |
| `civ2art.dll` | 999 | the windows' frame texture | replaced |
| | 1000, 2000, 4000, 30000 | the end of the game: the arrival, the defeat, Alpha Centauri beaten | replaced |
| `wonder.dll` | 20000-20027 | the wonders' movie window's border tile (a grey marble in the original, the same in all) | **missing art**: the pack's are each wonder's painting, tiled as a border |

(Not every one was reached by the scripts: those of other governments, eras, leaders and levels
are made the same way.)

### civ2.exe's resources

| Type | Ids | Use | Status |
| --- | --- | --- | --- |
| bitmaps | 301-306 | the radio buttons' and check boxes' marks | replaced: the pack's, in the original's 16 x 16 place, on the transparent teal |
| cursors | 500-517, 530 (groups; images 1-24) | the map's and the Civilopedia's cursors | replaced: the pack's (hot spot: the arrow's tip) |
| icons | 1-5 (groups) | the window's icon | replaced: the pack's |
| GIFS | (none: every id is looked for there first) | | |

### mk.dll's colour tables (21 `CTAB`)

The heralds' videos' palettes (`KINGS\HRLDxxx.AVI`, `load_ctab_res`): out of scope with the videos.

## Sounds

`Sound\*.WAV`, 78 files, the game's sound effects: missing sound (without them the game runs
silently; `CIV2_NO_SOUND`): `AIRCOMBT AQUEDUCT BARRACKS BIGGUN BLDCITY BLDSPCSH BOATSINK CATAPULT
CATHEDRL CAVALRY CHEERS1-3 CIVDISOR CRWDBUGL DIESEL DIVCRASH DIVEBOMB DRUMA0 DRUMAL DRUMAN DRUMAY
DRUMB0 DRUMBL DRUMBN DRUMBY DRUMC0 DRUMCL DRUMCN DRUMCY ELEPHANT ENDOTURN ENGNSPUT FANFARE1-8
FEEDBK03 FEEDBK04 FEEDBKXX FIRE--- GUILLOTN HELISHOT INFANTRY JETBOMB JETCOMBT JETCRASH JETSPUTR
LARGEXPL LETTER MCHNGUNS MEDEXPL MEDGUN MENUEND MENULOOP MENUOK MISSILE MOVPIECE MRKTPLCE NAVBTTLE
NEG1 NEWBANK NEWGOVT NEWONDER NUKEXPLO POMPCIRC POS1 SELL SMALLEXP SPYSOUND STKMARKT SWORDFGT
SWRDHORS TORPEDOS`.

## Music

`Music\Track02.wav` to `Track12.wav` (the CD's audio tracks: the theme and ten pieces), out of
scope here.

## What remains

* **Code**: the pictures made from the HD art at the original's size and layout, keeping the
  palette indices the game's code relies on (transparency and border colours, the units' shield
  colours and places, the cities' flags and civilizations' colours, the map's picking masks).
* **Art**: the wonders' movie border tile; the editors' three backdrops; the icons the pack
  lacks (above); the sounds; music (planned).
