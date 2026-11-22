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
* `audit synth NAME`: a file made from the art, or `audit synth Sound\NAME` a sound effect made
  from OpenCiv2's own (below).

It was run on every recorded scenario (`harness/scripts`, 44), every extras, HD and media script
(`harness/extras`, `harness/hd`, `harness/media`): the new game's set-up, the map at every zoom,
units, cities, the city window and view, every advisor and report, diplomacy, the High Council
(without its videos), the Civilopedia (every category, the tech tree), wonders, the throne room,
the spaceship, the Hall of Fame, the end of the game, the editors (cheat, sprites, rules, events),
the menus, the file dialog and the multiplayer set-up.  Then with the install's files removed
(`tools/native-run.sh`): its text files and `RULES.TXT` (`CIV2_NO_TEXT=1`, `CIV2_NO_RULES=1`), its
`.DAT` files (`CIV2_NO_DAT=1`), its `Sound` folder (`CIV2_NO_SOUND=1`) and its pictures: the
sheets (`*.GIF`), the libraries (`*.dll`) and `civ2.exe` (`CIV2_NO_GRAPHICS=1`).  Without the
pictures, text, rules and sounds the 44 scenarios run to their end with as many screenshots;
without the `.DAT` files too, `harness/extras/x-no-install.txt` and `x-no-install2.txt` (HD
graphics not asked for, so on at density 1, and an HD run at density 2 each) are the regression tests: a fresh install's first start, a new game set up (its
pictures), the world made, a city founded, its window; the autosave's advisors, tax rates,
Civilopedia, an announcement's picture, a city's view, the spaceship, the Hall of Fame, the map
at its largest zoom and the throne room; `x-no-install-options.txt` shows OpenCiv2 Options
without the pictures (below): Classic graphics unticked with the note, ticked the dialog that
offers to find the original's files, *Keep HD graphics* and its note, HD graphics kept.

## Graphics without the install: HD only

Two situations are supported, by design: the original's look (Classic graphics, or
`CIV2_CLASSIC=1`) **with** the install's pictures, pixel for pixel as the original; and HD
graphics, with or without them.  Only the graphics depend on the original's files: the bug
fixes, the interface additions and the rule changes are chosen apart from them, so the
original's rules (the fixes switched off, [BUGS.md](BUGS.md#switching-the-fixes)) can be played
without the install too.  Where the game folder lacks any of the original's pictures (a sheet,
a picture library or `civ2.exe`: `synth_missing` in `src/platform/synth.c`, at the start,
`src/platform/main.c`: `plat_hd_forced`):

* HD graphics are on whatever `Graphics=`, `--hd` / `CIV2_HD` or `GraphicsDensity=` say (the
  density as asked, else the screen's; in a replay `CIV2_TEST_HD`'s, else 1); `CIV2_CLASSIC=1`
  keeps everything else original (its bugs, none of the additions, its caption and frame);
* OpenCiv2 Options shows *Classic graphics* (Graphics and screen) unticked, with the note
  "Classic graphics need the original Civilization II files: without them HD graphics are always
  on." (`graphics.need_original`, in every shipped language); ticked, a dialog offers to find
  the original's files and copy those the game folder lacks into it, as the set-up does (an
  installed game, its CD, a `.zip` of the installed folder, a disc image; in the browser the set-up
  page's choice): found, Classic graphics from the next start (the restart offered); not,
  HD graphics stay, with a note ([EXTRAS.md](EXTRAS.md#classic-graphics-without-the-original-files));
* without the HD art as well (it comes with OpenCiv2: the program's `data/hd`) the game stops at
  once with a message (`hd.err.no_graphics`, translated): the game's folder, where the
  original's files go, and where the art should be;
* the browser's page does the same (`src/web/options.js`: `hasOriginalPictures`, `hdWanted`):
  it loads the art where the game's files lack the original's pictures.

The 8-bit pictures made from the art (below) stay: the game's code takes its sprites from
their rectangles, reads their marker pixels and loads their palette under HD graphics too.
They are not meant to be looked at: where the HD art is drawn whole over a picture whose
8-bit layout differs (the throne room, the spaceship, the diplomacy room's medallion), or the
art gives no marker places (the units' shields, the cities' flags and size boxes: a default),
that is out of scope, as there is no original look without the original's files.

Whether HD graphics are on where nothing chooses is decided in one place, `HD_BY_DEFAULT`
in `src/platform/options.c` (its mirror for the browser in `src/web/options.js`): making HD
the default later is that line; Classic graphics, unticked by default then, would be the step
that needs the original's files (the dialog above).

## Summary

| Status | Files | Resources |
| --- | --- | --- |
| replaced: built in (OpenCiv2's text, rules, timerdll, XDaemon) | 22 | |
| replaced: made by the game when missing (`CIV2.DAT`, `HALLFAME.DAT`) | 2 | |
| replaced: made from the HD art (sheets, libraries, civ2.exe; OpenCiv2's own pictures where the pack has none) | 21 | 243 GIF + 30 of civ2.exe's |
| of those, the 8-bit picture's layout differs (HD mode, always on then, draws it whole): out of scope by design | | 93 (pv.dll 55, ss.dll 400-499 38) |
| replaced: OpenCiv2's own sound effects (`data/native/sound`, public domain) | 78 (`Sound\*.WAV`) | |
| out of scope: videos, music | 59 videos (`Video` 37, `KINGS` 21, `LOSER.AVI`), 11 music tracks | 21 (mk.dll `CTAB`, the heralds' video palettes) |
| droppable: never read by the game | 22 | |

So a game can be set up and played with none of the install's files but its music and videos, given the HD art (which comes with OpenCiv2's builds: `data/native/hd`, made by `make hd-art`, not in the repository), in HD
graphics (above).  Without the art too the game does not start: it says what is missing.

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
| `TERRAIN1.GIF`, `TERRAIN2.GIF`, `UNITS.GIF`, `CITIES.GIF`, `ICONS.GIF`, `PEOPLE.GIF` | the map's and the interface's sprites (`gfx_load_*_sprites`; the `.BMP` of each is looked for first, never there) | replaced: made from the pack's sprites (below) |
| `EDITORPT.GIF` | the sprite editor's tools and backdrops | replaced: made from the pack's sprites |
| `CITY.GIF` | the city window's background | replaced: the pack's picture |
| `SCREDITS.GIF` | the credits' picture | replaced: the pack's picture |
| `EDITORAS.GIF`, `EDITORSA.GIF`, `EDITORSQ.GIF` | the editors' backdrops (improvements, units, cities) | replaced: made from OpenCiv2's own (`data/native/hd-extra`: dark slate reliefs, dithered into the shared palette's colours without changing it; the editors' white labels at a WCAG contrast of 8.1 to 13.0) |
| `Civ2Art.dll`, `Intro.dll`, `cv.dll`, `mk.dll`, `pv.dll`, `ss.dll`, `Tiles.dll`, `Wonder.dll` | the pictures of the windows (below) | replaced: made from the pack's pictures |
| `civ2.exe` | its resources (below); the game looks for it (`file_require_cdrom`) | replaced: made from the pack's pictures; an empty stand-in for the look-up |
| `Sound\*.WAV` (78) | the sound effects (below) | replaced: OpenCiv2's own (`data/native/sound`) |
| `Music\Track02`-`12.wav` (11) | the CD's music | out of scope (the user's own music is planned) |
| `Video\*.AVI` (37), `KINGS\*.AVI` (21), `LOSER.AVI` | the opening, the High Council, the wonders, the launch, the heralds, the defeat | out of scope: the game runs without them; with HD graphics a tribe's herald of OpenCiv2's own plays without `KINGS` (the Russians': [HD-GRAPHICS.md](HD-GRAPHICS.md), *Your own heralds*) |
| `SCENARIO\`, `*.SCN`, `*.MP`, `ROME.TXT`, `WWII.TXT`, `ALEXANDR.TXT` | scenarios and premade maps | out of scope |
| `CIV2FANW.TXT`, `CIV2FAQ.TXT`, `PATCH.TXT`, `README.TXT`, `Readmev13.txt`, `LEADERS.TXT`, `MISC.TXT`, `SC.TXT`, `TUTORIAL.SAV`, `Civ2Map.exe`, `Uninst.isu`, `Vfwfix.reg`, `smeds.log`, `XDaemon.log`, `EReg\` (8 files) | read by no part of the game (documents, the installer, the map editor's program, logs, registration) | droppable |

## The pictures made from the art

`src/platform/synth.c` (`docs/HD-GRAPHICS.md` for the art): where the game folder has no picture
the game asks for, an 8-bit one is made at the original's size and layout, the HD art shrunk into
it, so that the game's code, which takes its sprites from fixed rectangles and reads some of
their pixels back, works unchanged; HD mode then draws the art over it as with the original's.  The
pictures of the art it reads that HD mode does not draw (the libraries' pictures not painted
whole, the crops not drawn, civ2.exe's) are in `data/native/hd` at the original's size
(`data/hd-pack.json`: `synth`; `tools/build_hd_pack.py`), the others as HD mode draws them.

* **When**: `plat_host_path` (`src/platform/files.c`) asks `synth_file` for a sheet the game
  folder lacks (only there: a scenario's own art is its own), which is written, once a run, in
  `OpenCiv2Pictures` in the Windows directory; a library the folder lacks is loaded as a
  stand-in whose `FindResource` makes its GIF resources (`synth_resource`; the ids and sizes of
  the original's, `src/platform/synth_resources.h`); with no `civ2.exe` its bitmaps, cursors and
  icon are the pack's (`resources.c`: `synth_bitmap`, `synth_cursor`).  The libraries and
  `civ2.exe` are found as empty files there too (the game looks for them first).  With the
  install's files present none of this runs.
* **Layout**: each sprite the pack has (`crop/file/SHEET/x,y,w,h`,
  `crop/resource/MODULE/gifs/ID/x,y,w,h`) shrunk into its rectangle (a box filter), a one-pixel
  border round it; a whole picture (`resource/MODULE/gifs/ID`, `file/city` ...) under them, or alone,
  dithered (Floyd-Steinberg), a vignette's transparent ground dark grey.
* **Colours**: one palette, as the game loads every sheet into its palette's entries 10 to 201
  and draws many other pictures in it: the greys (0 to 31), the 42 other colours the game's code
  draws with by number (text, lines, fills: measured in the 44 scenarios), near the original's,
  among them the civilizations' 24 (the flag, its shade, the text's: red, white, green, blue,
  yellow, cyan, orange, purple), and 118 for the art (median cut over all the sheets).  The
  pictures whose colours the game takes into a palette of its own making have theirs in the
  entries it takes (the diplomacy room: the portrait's 32 to 95, the room's 96 to 134; the
  destroyed civilizations' backgrounds 96 to 235; mk.dll 299 and cv.dll 399, loaded into the
  game's greys, in those greys).  The windows' textures (the dialogs' ground, the frames' tile)
  are greys, as the original's, so that they keep their look in the diplomacy room's palette.
* **Indices the game gives a meaning**: 253 transparent, 254 a cell's border, 255 the
  background (the game's 7, 8, 9); `UNITS`: the shield's place marked on each cell's top and
  left border (250, the game's 4), the shield in the colour the game recolours per owner (252);
  `CITIES`: the flag's and the size box's places (250, 249), the civilizations' colour numbers
  where the game reads them (beside the flags); `TERRAIN1`: the map's picking mask (the tile and
  its four corners) and the directions' (eight and the middle), by index / 16 as the game reads
  them, and the blends' dither (the terrain kept at scattered pixels near the tile's edge).

Made in about 0.7 seconds (this machine, native build) at the first sheet loaded.

### The sheets

| Sheet | Sprites the game takes | In the pack | Status |
| --- | --- | --- | --- |
| `TERRAIN1` | the terrain, its resources, irrigation, farmland, mines, pollution, the hut, roads, railroads, the masks | 62 | replaced; the masks made (above) |
| `TERRAIN2` | rivers, forests, hills, mountains, coasts, river mouths | 100 | replaced |
| `UNITS` | the 63 units, the shield | 64 | replaced; the shields' places a fixed default (12, 8): the art gives none |
| `CITIES` | cities by style and size, walled, flags, fortress, airbase | 68 | replaced; the flags' and size boxes' places a default |
| `ICONS` | the map's cursor and grid, improvements', wonders' and advances' icons, the resource and tax icons, explosions, the windows' textures, the caption's buttons | 133 | replaced; 11 sprites the game takes have no picture in the pack, left empty, but none is drawn and the original's are empty too (nothing missing): three 32 x 32 beside the pollution icon (y 223: at 34 and 100 the sheet's background, at 133 an empty cell; 34 is the second sprite the cities editor's Edit Misc > fortress opens in the sprite editor, empty in the original too), three beside the map's cursor (y 256: 232 the cursor's right half, 265 and 298 empty cells), the two tall ones at x 311 (y 1, 98: background), the pair at x 350 (y 255, 288: across the advances' icons, never drawn), the tall one at 265, 289 (an empty cell) |
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
| `mk.dll` | 200-206 | the diplomacy rooms, by government | replaced: the pack's rooms are laid out otherwise (HD mode draws them whole; the 8-bit picture's medallion behind a panel, never shown) |
| | 210, 211, 215 | the diplomacy room's panels (crops) | replaced |
| | 220-261 | the leaders' portraits | replaced |
| | 299 | the diplomacy's tile | replaced (greys) |
| | 10000-10002 | the destroyed civilizations' window | replaced |
| `pv.dll` | 100, 105-184 | the throne room's hall and its pieces by section and level | replaced, layout differs: the pack's pieces are vignettes placed otherwise; HD mode, always on without the install, composes the room whole (`hd_throne_room`); the 8-bit pieces are not shown (out of scope by design) |
| `ss.dll` | 400-499 | the spaceship's window, its parts and cells | replaced, layout differs: HD mode, always on without the install, composes the ship (`hd_spaceship`); the 8-bit parts are not shown (out of scope by design) |
| | 20000-20007 | the spaceship's launch and arrival frames | replaced |
| `civ2art.dll` | 999 | the windows' frame texture | replaced |
| | 1000, 2000, 4000, 30000 | the end of the game: the arrival, the defeat, Alpha Centauri beaten | replaced |
| `wonder.dll` | 20000-20027 | the wonders' movie window's border tile (a grey marble in the original, the same in all) | replaced: the pack's painting, its border tile (1,1,64,32) OpenCiv2's own seamless grey marble (`data/native/hd-extra/wonder-border.webp`) |

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

The heralds' videos' palettes (`KINGS\HRLDxxx.AVI`, `load_ctab_res`): out of scope with the videos.  (OpenCiv2's own heralds,
with HD graphics, do not need them: their colours are their own, on the plane.)

## Sounds

`Sound\*.WAV`, 78 files, the game's sound effects: `AIRCOMBT AQUEDUCT BARRACKS BIGGUN BLDCITY
BLDSPCSH BOATSINK CATAPULT CATHEDRL CAVALRY CHEERS1-3 CIVDISOR CRWDBUGL DIESEL DIVCRASH DIVEBOMB
DRUMA0 DRUMAL DRUMAN DRUMAY DRUMB0 DRUMBL DRUMBN DRUMBY DRUMC0 DRUMCL DRUMCN DRUMCY ELEPHANT ENDOTURN
ENGNSPUT FANFARE1-8 FEEDBK03 FEEDBK04 FEEDBKXX FIRE--- GUILLOTN HELISHOT INFANTRY JETBOMB JETCOMBT
JETCRASH JETSPUTR LARGEXPL LETTER MCHNGUNS MEDEXPL MEDGUN MENUEND MENULOOP MENUOK MISSILE MOVPIECE
MRKTPLCE NAVBTTLE NEG1 NEWBANK NEWGOVT NEWONDER NUKEXPLO POMPCIRC POS1 SELL SMALLEXP SPYSOUND
STKMARKT SWORDFGT SWRDHORS TORPEDOS`.  (The game's table names 25 more, `AIRPLANE`, `RIFLE`,
`CUSTOM1` ...: no file has them, and the game plays nothing for them, with or without the install;
`FEEDBKXX` and `POMPCIRC` are not in it: only a scenario's events, which play any file, may.)

Replaced: OpenCiv2's own, `data/native/sound/name.ogg` for each (78 files, 1.98 MB, Ogg Vorbis,
22050 Hz mono), made from sounds in the public domain (CC0: Freesound, OpenGameArt and Kenney;
`data/native/sound/SOURCES.md` has each one's sources, authors and how it was made from them:
mixed where several, trimmed to about the original's length, its level matched to the
original's).  Where the game asks for `Sound\NAME.WAV` in the game folder and there is none
(`src/platform/sound.c`: `sound_fallback`, from `plat_host_path`), the Ogg is decoded
(stb_vorbis) into a WAV as the game reads its own (22050 Hz, 8 bits, mono; `snd_wave_load_file`
plays nothing else), written once a run into the folder `OpenCiv2Sounds` of the Windows
directory, and the game is given that file: the game's code plays it unchanged.  Only with
sound: a replay without it (the recorded scenarios) finds no file, as before, so the game takes
the same steps.  The install's own sounds are always played where they are; a scenario's
`SOUND` folder first, as ever.  They are packaged with the programs (`data/sound`; the Windows
program's resources) and on the browser version's site (`data/sound`: the page fetches those the
player's game lacks).  `make test-sound` (`tests/sound_fallback_test.sh`): without the `Sound`
folder (`CIV2_NO_SOUND=1`) and with audio (`CIV2_TEST_AUDIO=1`, SDL's dummy driver) the start
menu's `MENULOOP.WAV` is made and the game opens it; with the folder nothing is made.

## Music

`Music\Track02.wav` to `Track12.wav` (the CD's audio tracks: the theme and ten pieces), out of
scope here.

## What remains

* **Art**: music (planned).
* (Out of scope by design: the original's look without its files, so an 8-bit composition of
  the throne room and the spaceship, the diplomacy room's medallion in 8 bits, and the units'
  shields', the cities' flags' and size boxes' places from the art; HD graphics, always on
  then, have them.)
