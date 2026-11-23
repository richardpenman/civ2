# HD graphics

OpenCiv2 can draw the game with the replacement art of
[openciv2-assets](https://github.com/PeterJohnson/openciv2-assets) by Peter Johnson (MIT
licence): painted terrain, units, cities and resources at 4 to 8 times the original's
resolution, shown at the screen's full resolution, with text drawn sharp at that resolution
too.  It is optional and off by default: the original graphics stay the game's look.  This is
a prototype: the map is done, and many of the other windows' pictures (below).

## Playing with it

The art comes with OpenCiv2's builds: it is not in the repository, but made when OpenCiv2 is
built (`make hd-art`, which `make`, `make port`, `make native`, `make windows` and `make web` run
first: `data/native/hd`, below) and packed with every release (beside the program's `data` on
Linux and macOS, in the Windows program's resources, unpacked into its `data`, and on the
browser version's site), so the game itself downloads nothing.  In the game's
main menu choose *OpenCiv2 Options*, then *Graphics and screen*, untick *Classic graphics* and
OK (docs/EXTRAS.md, "OpenCiv2 Options"): the game offers to restart, which turns them on.
Otherwise:

1. The art is read from the program's data folder's `hd` (`data/native/hd`): its `pictures.txt`
   and the pictures it lists.
2. Switch it on, any of these ways (the first that says anything wins):

   * on the command line: `OpenCiv2 --hd` (or `--hd=N`), e.g.
     `build/OpenCiv2 ~/Civ2 --hd`;
   * in the environment: `CIV2_HD=1` (or `=N`);
   * in `CIV.INI` (the game's settings, in its Windows directory; OpenCiv2 Options writes
     them):

     ```ini
     [OpenCiv2]
     Graphics=HD
     ```

   `1` (and the bare `--hd`, and `Graphics=HD`) is on at the screen's own density; `N` from 2 to
   4 is on at that density; `0` is off (`--hd=0` turns it off whatever the environment or
   `CIV.INI` say, `CIV2_HD=0` whatever `CIV.INI` says).  The density (not a choice in OpenCiv2
   Options: always the screen's) is how many screen pixels
   each of the game's pixels gets each way: by default the game's own scale (docs/EXTRAS.md,
   Screen scale), so on a 4K screen 3, on a 1920x1080 one 1 (the new art at the original's
   size); `GraphicsDensity=N` (1 to 4) in `CIV.INI`, for tests, sets it for the bare `--hd`, `CIV2_HD=1`
   and `Graphics=HD`.  It takes effect the next time the game starts.  The portable build's
   set-up takes `--hd` out of its arguments (as `--fix-cue-gaps`) and passes it to the game
   as `CIV2_HD_ARG`.

Where the game folder lacks any of the original's pictures (a sheet, a picture library,
`civ2.exe`: `synth_missing`), HD graphics are always on, whatever `Graphics=`, `--hd`,
`CIV2_HD` or the mode say (Classic, which is the original's look, is not offered then; in a
replay at the density of `CIV2_TEST_HD`, else 1), and the art is also what the original's
pictures the game's code reads are made from, at their size and layout
([NO-INSTALL.md](NO-INSTALL.md), `src/platform/synth.c`).  Only two situations are supported:
the original's look with its install, and HD graphics with or without it.  Without either the
original's pictures or the art the game stops at once with a message saying where each goes
(`hd.err.no_graphics`, translated).

Whether HD graphics are on where nothing chooses is one line: `HD_BY_DEFAULT` in
`src/platform/options.c` (`plat_graphics_hd`, read by `hd_init` and OpenCiv2 Options; its
mirror for the browser's page is `HD_BY_DEFAULT` in `src/web/options.js`).  It is off today.

Without the art (and with the original's pictures) the game says so in its log and plays with the original graphics (sharp text and
interface still, where they are on: below).  HD graphics are never on with `CIV2_CLASSIC=1`, in
replays (but with `CIV2_TEST_HD=N`, density N) or in the unit tests, unless the original's
pictures are missing.

With HD graphics on (the art in use, not sharp text alone), the game goes straight to its main
menu: the opening video (`Video\OPENING.AVI`, 92 seconds, where the game folder has the CD's
`Video`) is not played, the start-up being the original's without the CD's `Video` (the menu
starts the theme music; `intro_play_opening_video`, src/auto/game.cpp).  The one `rand()` call
opening the video makes (the player's palette, `avi_open_file`) is still made, so the game's
random numbers are those of a start with the video.  With HD graphics off the video plays as
ever (a click, or Escape, Space or Enter, ends it).

With HD graphics on there is no High Council: the Advisors menu has no *Consult High Council*
(`menu_build_main`, src/auto/window_menu.cpp), `council_show` does nothing (the menu's command, the
map's key for it), and the game option's offer every 50 turns (`COUNCILTIME`) is not made, its
`rand()` calls still made.  The council is made of the CD's videos (`Video\COUNCILn.AVI`), which
OpenCiv2 does not replace; with Classic graphics it is the original's as ever.

In the browser version: *HD graphics* in the game's OpenCiv2 Options (the page loads the art from
its site), or `?hd=N` in the address ([BROWSER.md](BROWSER.md#hd-graphics)).

## What the art is

The art is Peter Johnson's runtime art pack, [openciv2-assets](https://github.com/PeterJohnson/openciv2-assets)
(MIT licence), as built from his repository at the commit pinned in `data/hd-pack.json` (`repo`,
`sha`; not a release, so that what is merged there can be used without waiting for one): the
part of it the game uses, in `data/native/hd`, made by `tools/build_hd_pack.py` (`make hd-art`)
and not in the repository (`.gitignore`: OpenCiv2's builds and releases carry it, git does not).
The repository (some 4 GB of history: the authoring and review material, the pictures' earlier
versions) builds its pack with `tools/build.py`, which runs `tools/build_atlas.py` on the files
its `runtime-inputs.json` lists (about 890 under `assets/atlas-hd`: the paintings, sprites,
their bindings and layout data, 316 MB) and `contracts/cityview-rects.json`: it crops, fits,
recolours and arranges them into `data/atlas-hd.pack`, and its releases add two bitmap font
caches, a layout header and contract for PR #11's renderer and `package.json` (none used here).
Built at the commit of release v2026.11.08.1 as below, the pack is that release's, byte for
byte.  Its `LICENSE` (MIT), `THIRD-PARTY.txt` and `assets/atlas-hd/PROVENANCE.md` come with the
art.  The provenance says the paintings were made with an image generator from written
descriptions and the rest drawn as vector geometry, with no pixels of the original game's or
Freeciv's art.

`atlas-hd.pack`: `C2ATLAS1`, a count, a 256-colour palette, then 2,166 entries of 180 bytes (at d770b0b; release v2026.11.08.1's 2,105)
(a key, the picture's size in the game's pixels, the offset and length of its PNG, and for
whole sheets an indexed copy), keys in ascending order, then the PNGs.

`data/native/hd` (about 32 MB; not in git): `pictures.txt`, a line per key, `KEY W H FILE`
(the pack's key, the picture's size in the game's pixels, its file there; the format of
`data/native/hd-extra`'s, below), a WebP file per distinct picture (named by its first key, `/`
as `_`; keys with the same picture share it), and the commit's `LICENSE`, `THIRD-PARTY.txt` and
`ATLAS-PROVENANCE.md`, and `.built` (the digest of the pin and the tool it was made with).
The tool does nothing when `data/native/hd` is already the art of this pin and tool, copies
it from the cache (`~/.cache/openciv2/hd-DIGEST`, kept there after each time it is made: no
network needed) when it was made before, and otherwise (or with `--force`, `make hd-art
FORCE=1`):

1. fetches the files the pack is built from, at the pinned commit, into a partial clone in the
   cache (`~/.cache/openciv2/openciv2-assets.git`): the commit and its trees only (`git fetch
   --filter=blob:none --depth=1 SHA`, under 1 MB), then in one fetch the blobs not yet there of
   its `runtime-inputs.json`'s files, its builder (`tools/build_atlas.py`, `runtime_selection.py`,
   `dependency.py`), `contracts/cityview-rects.json`, `requirements.txt` and the notices: 316 MB
   the first time (about 25 seconds), afterwards only the files a newer commit changed; never
   the rest of the repository or its history.  Git checks each object against its id, so the
   commit's id pins every file;
1b. builds the pack as the repository's release does: its `tools/build_atlas.py`, in a throwaway
   container of the pinned Python 3.13 image with the Pillow its `requirements.txt` names
   (13.0.0), installed from PyPI and checked against the SHA-256s pinned in the tool (a commit
   asking for another Pillow stops the tool until the pins are updated); about 2.5 minutes;
2. keeps of the pack the pictures the game uses: those hd.c and `src/game/hd_graphics.cpp`
   draw (the keys beginning with one of `data/hd-pack.json`'s `keep` and none of its `drop`:
   *The pictures drawn*, below) and those `src/platform/synth.c` reads to make the original's
   pictures where the game folder lacks them (its `synth`: [NO-INSTALL.md](NO-INSTALL.md)),
   but never those beginning with one of its `exclude` (the pack's leaders' portraits whole and
   its sheets of small faces, `mk.dll` 210, 211, 220 to 261, and the portraits' crops no tribe is
   given: not used), 1,569 keys, 973 distinct
   pictures (the pack has 2,166);
3. makes each no larger than the largest size the game uses it at: a picture drawn, its size in
   the game's pixels times 4 (the largest density, `hd_density`) and times 2 more for the map's
   (its largest zoom, +8: twice the size); one only read by synth.c, its size in the game's
   pixels (synth.c shrinks it to that, into the 8-bit picture HD mode then shows enlarged).  A
   larger one is shrunk (Lanczos, premultiplied alpha).  None drawn is: the pack's
   sprites are 4 times the game's size, the map's blends, fog and shields 8 and its backgrounds
   2, so at density 4 (a 5K screen, or `--hd=4`) none is drawn larger than it is stored; the
   synth-only ones (the libraries' pictures hd.c does not paint, the crops it does not draw)
   are shrunk to the original's size (157 pictures);
4. encodes each as WebP with cwebp 1.5.0 (Debian 13's package, in a throwaway container of the
   pinned Debian image: Docker; without Docker, or with `--local`, this machine's `cwebp`,
   with Python's PIL and numpy: the same pictures, not always the same bytes, so not kept in
   the cache), as an earlier comparison of encoders on these pictures chose
   (SSIMULACRA2 85 or more for all but ten backgrounds at quality 90): a picture with
   transparent pixels at most 53 of the game's pixels each way lossless (small sprites,
   icons); the others lossy at quality 90 (`-m 4 -sharp_yuv`, alpha lossless), or 95 for the
   backgrounds below 85 at 90 (the throne room's hall `pv.dll` 100, the spaceship's window
   `ss.dll` 499, the diplomacy rooms `mk.dll` 200 to 206, the credits `tiles.dll` 90), unless
   the lossless file is at most 1.15 times as large (flat pictures);
5. writes `data/native/hd` again (and the cache's copy).  The same commit gives the same files.
   Without Docker (or with `--local`) this machine's Python and Pillow build the pack (another
   Pillow: perhaps not to the pixel) and its `cwebp` encodes, not kept in the cache.

`make HD_ART=` (or `HD_ART=0`) builds without making the art.  The release's build
(`.github/workflows/release.yml`) makes it once (its job `hd-art`, `~/.cache/openciv2` cached
between runs) and packs it with each system's program.

Upgrading the art (what is merged into openciv2-assets' `main`): put the commit's id (`git
ls-remote https://github.com/PeterJohnson/openciv2-assets main`) as `sha` in
`data/hd-pack.json`, run `make hd-art` (`python3 tools/build_hd_pack.py`), look at the HD
scripts' screenshots (*Tests*, below), and commit `data/hd-pack.json` (never `data/native/hd`).
A picture of a kind not yet
used is added to `keep` (drawn) or `synth` (read by synth.c) first.  The game reads PNG too
(stb_image), so a picture can also be put there by hand.

The keys that matter here:

| Key | What |
| --- | --- |
| `crop/file/SHEET/x,y,w,h` | 499 pictures, each replacing the sprite the game takes from that rectangle of `TERRAIN1`, `TERRAIN2`, `UNITS`, `CITIES`, `ICONS`, `PEOPLE` or `EDITORPT` (4 times as large) |
| `tint/crop/.../COLOUR` | 456 of those in each civilization's colour (flags, shields) |
| `dither/T/Q`, `fog/Q` | the blends of each terrain into its neighbours, by quarter of a tile (the game makes these itself from a mask) |
| `file/SHEET` | the whole sheets at the original's size |
| `resource/MODULE/gifs/ID` | the pictures of the game's libraries (`mk.dll`, `pv.dll`, `cv.dll`, `tiles.dll` ...) by resource id, at the original's size (below) |
| `crop/resource/MODULE/gifs/ID/x,y,w,h` | the sprites the game takes from those pictures (the city view's improvements and houses, the leaders' portraits, the diplomacy screen's icons, the spaceship's buttons) |
| `palace/FAMILY-LEVEL`, `palace/bonus-I` | the throne room's furnishings (arch, rug, dais, columns, tapestry, brazier, throne, ceiling medallion; levels 1 to 4) and its seven bonus pieces, which `src/game/hd_graphics.cpp` places in the pack's empty hall (`resource/pv.dll/gifs/100`) |
| `palace-resource/ID` | the original's throne room pieces redrawn (`pv.dll`; not used here) |
| `city/...`, `building/...`, `house/...`, `lot/...`, `advance/...`, `wonder/...` ... | pictures that PR #11's renderers of their own place themselves (not used here) |

### OpenCiv2's own pictures and heralds (`data/native/hd-extra`)

`data/native/hd-extra` is a local staging folder, not in the repository (`.gitignore`): art being
tried before it is offered to the art pack (openciv2-assets), which is where OpenCiv2's art goes.
Everything works without it (a fresh clone, the releases): the pack's pictures are drawn, and
heralds are the CD's (`KINGS`) or, without them, none.  Where it is present it is packaged with
the build as the art is (beside the program's `data` on Linux and macOS, in the Windows
program's resources, unpacked into its `data`; on the browser version's site in `hd/art.bin`
with the art, put into the game's files with it), and the review site's deploy takes the main
checkout's copy ([BROWSER.md](BROWSER.md#a-site-with-the-game-on-it-game1)).

Its `pictures.txt` lists pictures, a line each, `KEY W H FILE` (`#` a comment): the pack's key,
the picture's size in the game's pixels, the picture in that folder, WebP (libwebp's decoder,
`src/third_party/libwebp`) or PNG (stb_image), told apart by its first bytes (`hd_decode_image`,
`src/platform/hd_image.c`).  When the art is read, hd.c (`load_pictures`) adds them to its
pictures, in place of the pack's of the same key: they are drawn exactly as the pack's would be
(a sprite's crop, a sheet, a resource picture); a resource picture among them is painted whole,
as `WHOLE`'s are, even where the pack's is not drawn and the original's kept, and so is a whole
sheet of the game's folder (`file/SHEET` at the sheet's size: `hd_sheet_loaded`).  Without the
art (`data/native/hd`) they are not drawn either.  Its `heralds.txt` lists heralds (*Your own
heralds*, below).  Art made for it: WebP by cwebp 1.5.0 (lossy `-q 90 -m 4 -sharp_yuv`, or
`-lossless -exact` with alpha); an AI-generated picture's model, prompt and references are
recorded with it (and go with it to the pack).

| Key | Picture |
| --- | --- |
| `resource/tiles.dll/gifs/50` to `59`, `65`, `66` | the advisors' backgrounds (`tiles-ID.webp`, below; each 1280x960: density 2, as the pack's backgrounds; lossy WebP at quality 90, 40 to 121 KB, 0.7 MB in all): dark painted still lifes and scenes, each fitting its report, made with Gemini 3 Pro Image from two of the pack's backgrounds as references (`tools/gen_hd_backgrounds.py`).  The advisors' windows show the top left 600x400 of their 640x480 picture, so the painting fills that area (1200x800) and the rest is it mirrored; the tax rates' dialog shows its top left 400x256 on small screens (65: the painting fills 800x512) and 600x384 on large ones (66: the same painting, prepared as the advisors'). |

The scenario editors' backdrops (`file/editoras`: `EDITORAS.GIF`, the improvements editor;
`file/editorsa`: `EDITORSA.GIF`, the units and tribe editors; `file/editorsq`: `EDITORSQ.GIF`, the
cities editor; the original's are grey embossed reliefs, an astronaut, a warrior with a spear, a sea
monster) and the wonders' movie window's border tile (`crop/resource/wonder.dll/gifs/20000/1,1,64,32`
to `20027`: the original's is the same grey marble in each; the pack's pictures are each wonder's
painting, whose corner would be no border) are the pack's since its commit d770b0b
([openciv2-assets pull request 7](https://github.com/PeterJohnson/openciv2-assets/pull/7):
OpenCiv2's own, once here, faint bas-reliefs in dark slate and a seamless grey marble, made with
Gemini 3 Pro Image): the backdrops are painted whole on the sheet when it is loaded
(`hd_sheet_loaded`) and the tile is drawn as any crop, and without the install the sheets and
the tile are made from them (`src/platform/synth.c`, which takes nothing else from the pack's
wonder paintings).  With art without them, with the install the original's are drawn, enlarged;
without it the editors' backdrop is plain dark grey and the movie window's border plain grey,
which a wonder's movie shows only where the CD's `Video` is in the game folder.

The advisors' backgrounds, by `tiles.dll` id (`PictureWindow::open`'s first argument + 49, at
600x400 of the 640x480 picture; the original's are dark grey photographs and engravings on a map,
the pack's the same pale parchment until its commit ad6791d, since OpenCiv2's own below, given to
it: [pull request 6](https://github.com/PeterJohnson/openciv2-assets/pull/6)), and OpenCiv2's own:

| Id | Screen | OpenCiv2's own |
| --- | --- | --- |
| 50 | Domestic Advisor, city status (F1) | a steward's table: a ledger, grain sacks, an oil lamp, keys |
| 51 | Defense Minister (F2), its statistics and casualties; the casualty timeline; the power graph (World > Civilization Score with the map revealed) | a war-room table with a world map |
| 52 | Foreign Minister's intelligence report (F3, a civilization's report) | an envoy's desk: sealed letters, sealing wax, a quill |
| 53 | Attitude Advisor (F4) | a crowded town square at dusk, from above |
| 54 | Trade Advisor (F5) | coins, a balance, cloth and spice jars |
| 55 | Science Advisor (F6) | an astrolabe, a telescope and star charts |
| 56 | Wonders of the World (F7) | statues, a model pyramid and columns in a dim gallery |
| 57 | Top 5 Cities (F8); the Hall of Fame | a city's skyline of every era at night |
| 58 | Demographics (F11) | census ledgers, record books, an abacus |
| 59 | Civilization Score (F9); the civilization rating | a laurel wreath and trophies on a plinth |
| 65 | the tax rates (Kingdom > Tax Rate), small screens (the dialog 400x256) | a treasury table: coin chests, a tally board |
| 66 | the tax rates, large screens, 1000 pixels wide or more (the dialog 1.5 times as large) | the same painting as 65 |

(The credits, 95, are a 640x480 picture of their own.)

#### Your own heralds

With *Animated Heralds* on (Game > Graphic Options), the original shows, over the diplomacy
room, the CD's video of the tribe's herald (`KINGS\HRLDxxx.AVI`, 10 frames a second, five
segments of 30 frames: a greeting played once as the room opens, an idle loop, then a
reaction when the AI is pleased, declines, or is angry, each followed by the idle loop again;
`DiploLeaderWindow::init_video`, `diplo_leader_video_end_cb`, `diplo_leader_play_reaction` in
`src/auto/diplomacy.cpp`, the segments in the game's table `leader_anim_frames[tribe][5][2]`).
With HD graphics, a herald of OpenCiv2's own plays in its place where
`data/native/hd-extra/heralds.txt` has one for the tribe, a line each:

    CODE X Y W H FPS G0 G1 I0 I1 P0 P1 D0 D1 A0 A1 FILE
    RUS 86 72 218 400 12 0 89 90 131 132 185 186 239 240 herald-RUS.webp

* `CODE`: the three letters of the original's video (`ROM BAB GER EGY AME GRE IND RUS ZUL FRE
  AZT CHI ENG MON CEL JAP VIK SPA PER CAR SIO`, `leader_video_tribe_codes`).
* `X Y W H`: where the herald's window stands in the room and its size, in the game's pixels
  (the room is 640x480; the original Russian's video is 296x416 at 92,64, its figure about 395
  pixels tall, its feet near 185,470).
* `FPS` and the first and last frame (inclusive) of each segment in the game's order: greeting,
  idle loop, pleased, polite refusal, angry.
* `FILE`: an animated WebP with alpha beside it, the segments' frames back to back; its canvas
  is the window `W x H` at any scale (drawn scaled to the plane's resolution).

How it plays: the game is unchanged but for one call in `init_video` (`hd_herald`,
`src/game/hd_graphics.cpp`): for a tribe with its own herald it moves the video's window to `X
Y` and puts the herald's segments into the game's table for that tribe (the original's are put
back for a tribe without), and the video counts as found even where the game folder has no
`KINGS`.  The game then opens `KINGS\HRLDxxx.AVI` as ever; `src/platform/video.c` (Video for
Windows) opens the herald's WebP in its place: an AVI of one video stream at `FPS` whose frames
(`OHRL`, every one a key frame) its decompressor draws.  So the game plays it with its own
`play_range`/`loop_range`, timing (`timeGetTime`), end callback, reactions and their sounds,
exactly as it plays the CD's video, and calls `rand()` as it does then.  Each frame
(`hd_anim_frame` in `src/platform/hd_image.c`: libwebp's demuxer, `src/third_party/libwebp/src/demux`,
the frames composited as libwebp's animation decoder does but from any frame: from the last key
frame before it or on from the last shown) is drawn over the room the game has put back behind
the window: on the plane at its resolution, each plane pixel the frame's pixels it covers
averaged (or bilinear where enlarged), premultiplied, over the room with its alpha; and in the
game's 8-bit pixels, those at least half covered, the nearest colour of the palette.  The game's
pixels it touched are taken as shown (`draw_own` in `video.c`).  Its cost, measured on this
machine (one slow core, `OpenCiv2`) with the Russians' herald: about 8 ms a frame to decode it
and draw the game's pixels, and 3 ms on the plane at density 2, 12 at density 4 (12 frames a
second; `CIV2_LOG_VIDEO=1` logs each).

With the CD's `KINGS` or without it: the own herald plays wherever HD graphics are on and
*Animated Heralds* is ticked (without the install's videos the option otherwise shows nothing;
with them the own herald is preferred).  The option keeps its meaning: unticked, no herald.
With HD graphics it is on by default: where the original would ask its one-time question
(`HERALDWARNING`, 16 MB of memory; CIV.INI's `[Civilization Gold] Herald Warning Shown=` not
yet 1) nothing is asked and the answer taken as yes (`app_startup_init`), so fresh settings
have the heralds; unticked later, it stays off (CIV2.DAT, as ever).  Classic graphics ask as
the original does.
Classic graphics (and HD off, `CIV2_CLASSIC=1`, replays without `CIV2_TEST_HD`) play the CD's
video as ever: the table and the window's place are the original's.

The Russians' herald (`herald-RUS.webp`: 324 frames, 12 a second, 27 seconds; 327x600 for the
window's 218x400, so 1.5 times the game's pixels, drawn enlarged smoothly at densities 2 to 4;
lossy at quality 70, alpha lossless, every frame a key frame; 6.9 MB) is an 18th-century Russian court envoy in a
crimson kaftan with a mace, made with Google's video model from a still (below); it is in the
local staging folder only, not in the repository: without it the Russians' herald is the CD's
(with `KINGS`; without them, none), as for every other tribe.
`tools/build_herald.py CODE GREETING IDLE PLEASED DECLINE ANGRY` makes a herald and its line from
five clips of the figure on a flat chroma-key green (Docker: ffmpeg, cwebp and webpmux 1.5.0 in a
throwaway Debian 13 container): each clip resampled to 12 frames a second, keyed (ffmpeg's
`chromakey`, then `despill`), its last half second blended into one pose shared by all five (the
idle loop's first frame) and, but for the greeting's, its first half second out of it (so every
join the game makes, greeting to idle, round the idle loop, idle to a reaction and back, is
seamless; the clips, made apart, start in slightly different poses), cropped to the figure's bounds over every frame,
scaled so it stands `--height` game pixels tall with its feet at `--foot`, and encoded; then
`tools/check_herald.py CODE` (also on its own: `tools/dk python3 tools/check_herald.py RUS`) plays
the segments in the game's order and fails if a join changes more than twice the median frame
(`--video OUT.mp4`: that play, labelled, to look at).
`harness/hd/hd-herald-own.sh` shows it, the man's and the woman's leader, with `KINGS`,
without it, without the install, and with Classic graphics (contact sheets in
`run/hd-herald-own`; it says SKIP without the herald); in the browser version, `tools/toolchain/node/bin/node
src/web/test/smoke.mjs --replay hd/hd-herald-own --hd 2` (after `make web`; its shots in
`run/web-hd-hd-herald-own`).

How the Russians' herald was made (AI-generated; its provenance goes with it to the pack).  The
still: Google's Gemini 3 Pro Image (`gemini-3-pro-image`, 2K: 1536x2752, 9:16), with two
references, the pack's diplomacy rooms under Monarchy and Communism (for the palette's harmony
only), the prompt:

    A full-length figure of a Russian imperial court envoy (a herald) of the 18th century, painted
    in the same painterly digital-illustration style as the reference images: a long crimson
    kaftan with gold braid and brass buttons, a dark-blue sash, a brown sable fur hat with a red
    crown, brown leather boots, a short gilded ceremonial staff held loosely in one hand. Costume
    and props use only reds, golds, browns, blues, black and white; there is NO green anywhere on
    the figure, his clothes or props. He stands facing the viewer at three-quarters, in a neutral
    relaxed pose with his arms hanging at his sides. The whole figure is in frame, head to feet,
    with a generous margin on all sides. The background is a perfectly flat, uniform chroma-key
    green (#00B140), with no shadow, floor, gradient, texture or vignette on it, and no shadow
    cast on it. No text, letters or logos. Portrait 9:16.

The five clips: Google's Gemini Omni video model (`gemini-omni-1.2-flash`: 720x1280, 24 frames a
second, silent), the still (on its green, scaled to 768x1376) given twice with the prompt; the
greeting 8 seconds, the idle loop 4, each reaction 5.  Each prompt is "Animate this exact figure:
the Russian court herald ..." with its action, then the same ending:

* greeting: "bows respectfully and makes a slow welcoming gesture with his free hand, then
  returns to exactly the starting neutral pose, so the clip loops seamlessly. Static camera, no
  zoom or pan. The background stays a perfectly flat, uniform chroma-green with no shadows,
  lighting changes or objects. Keep his face, costume and staff identical to the image. No text.
  Silent: no music, no speech."
* idle: "stands still, waiting attentively. Only subtle idle motion: he breathes calmly, shifts
  his weight very slightly from one foot to the other, and makes a small adjustment of his grip
  on the staff and of his sash. His arms stay close to his body; no gestures, no bow."
* pleased (an offer accepted): "is pleased, an offer has been accepted. He smiles warmly, nods
  approvingly twice and makes a small, contained approving gesture with his free hand held close
  in front of his chest, like a gracious acknowledgement."
* polite refusal: "politely declines. He frowns slightly, slowly shakes his head, and raises his
  free hand a little in front of his chest, palm outward, in a calm, regretful 'no, thank you'
  gesture; restrained and courteous."
* angry: "is angry and offended. He scowls, stamps the end of his staff firmly on the ground
  once, and jabs a finger of his free hand sharply toward the viewer in a stern warning, his arm
  staying in front of his body rather than stretched to the side."
* the ending (but the greeting's, which has its own): "Then he returns to exactly the starting
  neutral pose, so the clip ends on the first frame and loops seamlessly. Static camera, no zoom
  or pan. Keep the whole figure, his hands, fingertips, staff and hat well inside the frame at
  all times, with a clear margin of green on every side; no part of him ever touches or leaves
  the edge of the frame. The background stays a perfectly flat, uniform chroma-green with no
  shadows, lighting changes or objects. Keep his face, costume and staff identical to the image.
  No text. Silent: no music, no speech."

Then `tools/build_herald.py RUS greeting.mp4 idle.mp4 pleased.mp4 decline.mp4 angry.mp4` (cropped
to the figure, 400 game pixels tall at 1.5 times the game's pixels, 327x600, each frame lossy WebP
at quality 70 with lossless alpha, put together whole by webpmux; 324 frames, 6.9 MB) and
`tools/check_herald.py RUS`.

## How it is drawn

The game is left to draw exactly as it does, into its 8-bit surfaces; its code is not changed
but for calls added at a few of its drawing functions, which do nothing unless HD graphics are
on (`src/auto/graphics_surface.cpp`, `src/auto/graphics.cpp`, `src/game/hd_graphics.cpp`).
The rest is in the platform layer (`src/platform/hd.c`):

* A surface (a `Bitmap`) can have a **plane**: true-colour pixels, density times larger each
  way, and a copy of the surface's own pixels as the plane last took them.  A surface gets one
  when an HD sprite, text or a control's shape is drawn on it, a surface with one is copied to
  it, or it is a window shown.  (Text gives the surface a plane at once: the game draws much of
  its text on surfaces of its own, the first screens' dialogs and the menu bar among them, which
  it then copies to its windows; before, such text was enlarged until something else gave the
  surface a plane, which in the browser looked like jagged menus until a game was loaded.)
* **Sheets**: when the game loads one of its sheets (`TERRAIN1.BMP`/`.GIF` ...) from its own
  folder, the surface remembers which; a scenario's own sheets are its own art and keep it.
* **Sprites**: when the game takes a sprite from a sheet (`Sprite::encode_from_surface`) at a
  rectangle the pack has a picture for, the sprite gets that picture; the terrain blends get
  theirs by role after `gfx_load_terrain_sprites`.  A sprite taken from what is on a surface
  with a plane (a unit's background, kept to restore it) keeps the plane's pixels.  When the
  game draws a sprite (`Sprite::blit_to_surface`, `blit_silhouette_to_surface`), it draws as
  ever, then the picture is drawn on the plane over the sprite's frame, scaled to the zoom and
  the density (filtered; the base terrain diamonds exactly inside their diamond, so that tiles
  meet without seams, a pixel of the diamond where the picture's antialiased edge leaves
  nothing taking the colour of its nearest pixel that is there, not black; the terrain's
  overlays (`TERRAIN1`, `TERRAIN2` tiles and quarters) and blends only inside their tile's
  diamond, so that none reaches a neighbour, where it was blended again over what a partial
  redraw of the neighbour left), and the game's pixels it covered are taken as shown.  A flag recoloured
  to a civilization (`Sprite::replace_color`) takes the pack's picture in that colour.
* **Everything else** the game draws (text, fills, lines, sprites without a picture) is found
  by comparing the surface's pixels with the plane's copy, and shown enlarged.  Fills, and
  copies from surfaces without a plane, are shown even where the pixels did not change.
* **Copies** between surfaces (`BitBlt`, `gfx_copy_pixels`) copy the planes too, with which
  of their pixels show the game's pixels enlarged (`hd_big`: not art, nor sharp text; of a
  picture with transparent pixels copied, those where it is opaque), so that
  a copy between surfaces whose colour tables differ recolours only those (below: the
  herald's backdrop); before, a black pixel of a unit's picture copied back from the slide's
  background surface, whose colour table had black for that index, took the map's colour.
* **Text** on a surface with a plane is drawn again there from the glyphs of the same font
  density times as tall (OpenCiv2's own fonts, to 48 pixels), each glyph where the game's is.
* **The platform's shapes** that are not straight lines (the check boxes' marks and the
  radio buttons, the scroll bars' arrows, the combo box's arrow, the caption's close box) are drawn
  again on the plane, antialiased, after the same in the surface's pixels (`hd_dc_disc`,
  `hd_dc_line`, `hd_dc_polygon`, `hd_dc_chevron`); straight edges and lines are the surface's
  pixels enlarged, as many screen pixels wide as a game pixel, as a Windows desktop at that scale
  draws them.
* **The screen**: the desktop is composed as before, and beside it the HD frame, from the
  windows' planes (or their pixels enlarged), the desktop's background (with a plane of its own,
  enlarged once) only where no window covers it, with the colour correction; the host shows that
  frame (`host_present_hd`) in the same place, at its density.  A replay's `shot NAME` also
  saves `NAME-hd.bmp` (the frame; `tools/native-run.sh` makes it `NAME-hd.png` when the game
  has ended, as it does `NAME.bmp`).

* **The other windows' pictures**: when the game loads a picture from one of its libraries' GIF
  resources (`DibSurface::load_gif_resource`), the surface remembers it as
  `resource/MODULE/gifs/ID` (the module the platform's `FindResourceA` found it in,
  `plat_res_module`), and the sprites the game takes from it get the pack's
  `crop/resource/MODULE/gifs/ID/x,y,w,h` as sprites from sheets do.  The pack's whole pictures were made for
  PR #11's renderers: many are laid out otherwise than the original's (the throne room's
  pieces, the spaceship's parts, the announcements' pictures are vignettes, the advisors'
  photographs a framed parchment), so only those that work as the same picture are painted on
  the surface's plane when it is loaded (`WHOLE` in hd.c, each looked at in its window): the
  diplomacy rooms, the new game's backdrop and pictures, the city view's landscapes, the
  spaceship's window, five of the announcements' pictures.  The announcements' pictures
  (`tiles.dll` 70 to 77, by two: a city captured, in civil disorder, celebrating, founded; + 1
  once the civilization has Industrialization; the popup draws the whole picture as a sprite
  beside its text, `popup_city_picture_zoom`) are the pack's where it has a picture of the same
  subject that fits: its are vignettes on a transparent ground (the popup's marble shows around
  them), at the original's size; the wide ones (70, 72, 73, 74, 76: a city's gate, a crowd in the
  market, a crowd in caps before a parliament, a festival in a court, builders at a new
  wall) fill their frame as the original's photographs and engravings do, but in the tall
  frames (71, 75, 77: the modern capture, celebration and founding) the pack's pictures are wide
  ones shrunk into the middle, a third of the frame, so those are the original's, enlarged
  (`harness/hd/hd-pictures.txt`, `hd_test_city_picture`).  `wonder.dll`'s pictures (20000 +
  the wonder) are, in the original, all the same grey marble from which the wonder's movie
  window takes its 64 x 32 border tile; the pack's are each wonder's own painted picture, 74 x 74,
  which as a border tile would be absurd, so the window's tile is the pack's marble tile (above:
  pull request 7; with art without it the original's marble, enlarged, without the install plain grey;
  `harness/media/m-wonder.txt`, HD run: the Oracle's and the Pyramids' windows).  The advisors' backgrounds (`tiles.dll` 50 to 59: the advisors, the power
  graph, the Hall of Fame ...; 65 and 66: the tax rates) were not the release's: the release's
  are a pale parchment, and the game's text on them (white, light grey, yellow, coloured for the
  original's dark grey photographs) could not be read: measured, the WCAG contrast of that text
  is 1.0 to 1.4 on the parchment (its average colour under the text, 229,220,198) against 4.0 to
  7.5 on the original's (about 90,90,90); a dark overlay bringing light grey text to 4.5 would
  leave the parchment nearly black.  They are OpenCiv2's own (above), each chosen for the
  contrast of the text on it, measured the same way in the World War I scenario's screens (at
  800x600; the tax rates also at 1280x1024): the white and light grey text 7.4 to 16.3 against
  3.5 to 7.9 on the original's, the yellow 7.2 to 10.3 against 3.7 to 5.5, the cyan 4.8 to 7.2
  against 2.6 to 3.3, the orange and green 5.3 to 8.1 against 2.7 to 4.3, the civilizations'
  lilac 5.1 to 5.7 against 2.9 to 3.5 and their blue 2.9 to 3.4 against 1.6 to 1.9 (that blue
  is at most 4.9 even on black).  Single crops not drawn either (`NOT_CROPPED_KEYS` in hd.c):
  in `tiles.dll` 86, the information seal of the popups (the quit confirmation's), the tutorial's
  owl and the podium in a civilization's colour, which the pack has as a thin pale "i", a "?"
  and a flat shield on nothing.  The diplomacy room (`mk.dll` 200 to 206, by the leader's
  government: anarchy to democracy) is the pack's, an empty hall with the panels where the game
  draws the military icons, the portrait and the advances.  The leader's portrait drawn in it
  (`mk.dll` 220 + the tribe's slot, + 21 for a woman: the 225x275 sprite at 1,1) is one of the
  pack's 42 portraits, but not the one at its own id: those are regional archetypes of the
  pack's own (its manifest's `leaders.leader-00` to `-41`, at `mk.dll` 220 + N), not the
  tribes' leaders in the original's order (at the Vikings' id a 19th-century man in a frock
  coat, at the Babylonians' a red-bearded Celt), so a portrait's own crop is never drawn
  (`NOT_CROPPED` in hd.c, with the sheets of small faces, 210 and 211, of which the game takes
  no sprites: the custom tribe's portrait chooser copies them whole, the original's) and each
  tribe and gender is given the portrait that fits it best, by region, then era, several tribes
  sharing one (`hd_portraits` in `src/game/hd_graphics.cpp`, bound by `hd_diplomacy_portrait`
  in `DiploLeaderWindow::init_portrait`).  The portrait's number chooses, as in the game: the
  tribe's slot (a scenario's tribe in a slot has the slot's portrait), or the face a custom
  tribe chose in the portrait chooser.  The pack's bust is drawn in the sprite's frame, centred
  on the panel, as its art is laid out.

  | Tribe | Man | Woman |
  |---|---|---|
  | Romans | 220: Mediterranean civic leader, ivory mantle | 241: the same, laurel wreath |
  | Babylonians, Persians | 228: Near Eastern leader, blue robe, cap | 249: Near Eastern ruler, silver circlet |
  | Germans | 232: medieval European ruler, circlet, blue velvet | 253: the same |
  | Egyptians, Carthaginians | 223: North African statesman, linen headcloth | 244: North African stateswoman |
  | Americans | 236: industrial-era civic leader, suit | 257: industrial-era civic leader, jacket |
  | Greeks | 233: Byzantine statesman, grey beard, purple robe | 254: Byzantine stateswoman |
  | Indians | 224: South Asian ruler, saffron turban | 245: South Asian ruler, sari |
  | Russians, French | 235: early modern northern European diplomat, powdered hair, cravat | 256: the same, lace collar |
  | Zulus | the original's | the original's |
  | Aztecs | 226: Central American leader, jade collar | 247: the same |
  | Chinese | 222: East Asian scholar-ruler, teal silk robe | 243: the same |
  | English, Spanish | 234: Renaissance civic leader, black cap, brocade | 255: the same, ruff |
  | Mongols | 229: steppe leader, fur-trimmed coat | 250: the same |
  | Celts, Vikings | 221: northern European chieftain, red beard, green cloak | 242: the same, red braids |
  | Japanese | 237: modern East Asian diplomat | 258: the same |
  | Sioux | 239: Indigenous North American diplomat, blue wool | 260: the same |

  The Zulus keep the original's (a warrior with shield and spear, enlarged): the pack's only
  Africans (225, 246) are West African, in indigo robes and headwraps.  Not given to any tribe,
  and left out of the art (`data/hd-pack.json`'s `exclude`, with the pack's whole pictures of
  the portraits and the sheets of small faces): the Andean (227, 248), West African (225, 246),
  Southeast Asian (230, 251), Pacific (231, 252), modern South Asian (238, 259) and modern
  Mediterranean (240, 261) portraits.  Without the install the game's own pictures of the
  portraits and the small faces are a plain dark frame in the room's slate (`portrait_frames` in
  `src/platform/synth.c`), under the pack's portrait as above: the Zulus' leader is that frame.
  `harness/hd/hd-talks-all.sh` shows every tribe's leader, man and woman, beside the
  original's.  The leader's animation (*Animated
  Heralds*: the video `KINGS\HRLDxxx.AVI` from the CD, over the portrait) is the video's frames
  enlarged smoothly (below, *Videos*): the pack has no pictures for it; the herald, a whole
  figure standing at the portrait's left, is drawn over the portrait (its transparent pixels
  show the room and the portrait).  Each pixel the decoder
  writes is shown, opaque, even where its colour index is the one already there (`draw_frame` in
  `src/platform/video.c`, run by run); before, such pixels were
  not seen as changed and the pack's room (in the video's backdrop, a copy of the room with its
  plane) showed through the leader in specks.  The pixels a herald leaves transparent show the
  room behind it, the civilization's portrait included: the video's backdrop is a copy of the
  room taken while the portrait's colours were not yet in the room's colour table, and a copy
  between 8-bit surfaces whose colour tables differ now gives the pixels shown enlarged the
  destination's colours for their index, as the copied pixels have (`recolor_copied` in hd.c);
  before, the part of the portrait under the video's window (the left half of the Romans'
  medallion) was black from the herald's first frame on.  `harness/hd/hd-herald.txt` (with the
  CD's `KINGS` videos; where they are missing, a stand-in).  The city view's houses (`cv.dll` 310: the
  original's cottages for ancient and renaissance cities, a row of towns from Industrialization
  on, by `civ_city_style_era` of the owner, the same for the whole view) are the pack's set of
  that era, in both sizes: ancient `house/ancient/large|small/N`, renaissance
  `house/medieval/...`, industrial the pack's crops of both rows (19th-century buildings:
  terraced houses, a tenement with a fire escape, a clapboard house, a shop row), modern
  `house/modern/...` (the era sets; `hd_bind_cityview` in `src/game/hd_graphics.cpp`,
  called once the view has taken its sprites; a set the art lacks: the original's, enlarged).
  The empty lots stay the original's woods, which the pack leaves empty (its renderer drew lot pictures of its
  own).  All of such a picture's pixels are shown over the pack's landscape, also those the
  same colour as the landscape the game has underneath, which were not seen as drawn and let
  the landscape show through the trees in specks (`hd_sprite_pixels_shown` after each lot's
  draw in `CityViewWindow::draw_buildings`, as for the city's disorder marker; the pixels put on
  the plane in one call, `hd_bits_redrawn_mask`: one call per pixel, each looking the surface up
  among the bitmaps, took 5 seconds to open the view).  Its landscapes (`cv.dll` 340 + 5 x the site + `civ_city_art_era`) are grass with dirt
  tracks in every era in the pack, so only those of era 0 (340, 345, 350) are the
  pack's; the later eras' are the pack's `city/landscape/SITE/medieval` (era 1:
  stone-paved roads) and `.../modern` (eras 2 and 3, Superhighways: divided highways) (hd.c's
  `landscape_key`; without them, era 1 the pack's grass and the others the original's,
  enlarged).  Its improvements and wonders are one picture each in every era, the original's as
  the pack's.  A picture with transparent parts (the new game's pictures) is drawn, where the
  game copies it, over what the destination's plane showed; a sprite taken from such a picture
  keeps its transparency.  A surface's plane is drawn anew from its pixels whenever another
  picture is loaded into it; a whole picture of the pack painted there (a landscape, a room) is
  scaled once and kept with the other pictures drawn, not again each time it is loaded (the city
  view's landscape, 3840x1440 at density 3, took over 2 seconds each time the view was opened).
* **Videos** (the heralds, the High Council, the wonders', the opening): wherever the planes are
  on (HD graphics or sharp text), each frame is enlarged on the plane smoothly, not as its
  8-bit pixels repeated: the decoder's own colours (before the game's palette and its dithering)
  sampled bilinearly at the plane's resolution (`frame_plane_row` in `src/platform/video.c`),
  from the frame's opaque pixels within the source rectangle only, so that neither a herald's
  transparent black nor anything outside the frame bleeds into its edges.  A frame with a
  transparent area (the heralds) has its outline smooth at the plane's resolution, not a
  staircase of game pixels: its opacity, smoothed by a 3 x 3 binomial filter, is sampled
  bilinearly too, sharpened by a smoothstep from 1/4 to 3/4 (a crisp edge about one plane pixel
  wide at density 2, isolated pixels of the mask rounded off), and the frame's colour is drawn
  with that coverage over what the plane shows (the room and the portrait behind a herald, which
  the game puts back before each frame); the plane pixels of the game pixels next to the figure
  can so take some of it, and those at its edge some of the room.  The game's pixels are as
  ever.  Its cost, measured on this machine (one slow core) with the Romans' herald (640x240):
  about 5 ms a frame at density 2 and 7 at density 3 (4 and 6 with the outline at the game's
  pixels; 1.5 ms without the planes); frames without transparency (the High Council, the
  opening) take the same path without the coverage, as before.
* **Units' shields, owners' colours**: the units' shields are the pack's `shield`, an alpha mask,
  drawn in the colour the game gives each (the owner's, the stack's second shield's, the
  shadows'), where the game draws its own and then recolours it (`replace_color` of its marker
  colours 5 and 6, whose result is taken as shown: `hd_shield_begin`/`hd_shield_end` in
  `src/game/hd_graphics.cpp`); the black bands, the hit points and the order's letter are drawn
  over it as ever.  The occupied airbase's aircraft is the pack's variant in the owner's colour
  (`hd_recolour_begin`/`end`).  The city's disorder marker is the original's, enlarged (its box
  in the owner's colour, as the game recolours it, with a black fist): the pack's
  (`ICONS` 166,223) is a thin "!" on nothing, which tinted in the owner's colour was all but
  invisible over the city (`harness/hd/hd-disorder.txt`); all of its pixels are shown over the
  city's picture, also those the same colour as the city's underneath, which were not seen as
  drawn (`hd_sprite_pixels_shown`).  The pack numbers
  its colour variants (`tint/KEY/COLOUR`) 208 + 3 x the colour's slot (red, white, green, blue,
  yellow, cyan, orange, purple), not by the game's palette (`pack_tint`).  The diplomacy room,
  whose original shows the civilization's colour on its banners (palette entry 0x91), has the
  pack's flag (`faction/flag`) in that colour in its corner (`hd_diplomacy_flag`).  After Peter
  Johnson's `gfx_draw_unit_shield`, `hd_sprite_tint` and `hd_diplomacy_flag` (PR #11).  The
  game's own pixels (which map picking reads) are the original's.
* **Units on sentry** (*SentryHalftone*, [EXTRAS.md](EXTRAS.md)): the unit's picture in grey
  shades (`hd_sprite_mode(HD_SPRITES_GREY)` around the game's draw in
  `extras_draw_grey_sprite`, the game's own halftone pixels then taken as shown).
* **Advances**: where the game draws an advance's icon (its category's, 36x20, by category and
  epoch: the Science Advisor's list, the intelligence report's, the Civilopedia's list, an
  advance's page and its tree, the choice of research and its advice, the announcements of a
  discovery, of an advance acquired or stolen, the diplomacy's exchange lists), the plane shows
  the pack's picture of that advance (`advance/N`, 58 of them, 72x48), fitted inside the icon's
  frame; the game draws its icon as ever.  The calls added at those places
  (`hd_advance_begin`/`hd_advance_end`, `src/game/hd_graphics.cpp`) bind the icon's sprite to
  the picture for that draw only (`hd_sprite_as` in hd.c); a dialog draws its sprites later, so
  the entry it adds for the icon is remembered with its advance (`hd_dialog_advance`,
  `Dialog::add_sprite`).  The advances the pack has no picture for keep the original's icon.
  (Peter Johnson's idea: `hd_advance` in PR #11.)  The advances editor keeps the category icons
  it edits.
* **The throne room** is drawn whole from the pack's art, as PR #11's renderer drew it (its
  placement and order of the pieces): the game draws its own room as ever (the same pictures
  loaded, palettes and `rand()` calls), then, at the end of `ThroneRoomWindow::render`,
  `hd_throne_room` (`src/game/hd_graphics.cpp`) paints on the room's plane the pack's empty
  hall (`resource/pv.dll/gifs/100`), the furnishings at the civilization's levels
  (`palace/arch-1` ... `palace/ceiling-medallion-4`, from `throne_room[0..7]`; 0: none) and,
  the room complete, the bonus pieces built (`palace/bonus-0` to `6`, the bits of
  `throne_room[8]`), and the game's pixels are taken as shown (`hd_picture`,
  `hd_picture_done` in hd.c).  The text the game writes over it afterwards is drawn sharp on
  it.  The choice ("Which section shall we improve?", end of `render_choices`): the game
  still chooses by the regions of its click map over its own colour-coded hall
  (`add_choice_regions`: the first region holding the point gives the section, or 10 + the bonus
  piece); the plane shows the pack's room, darkened, and for each section that can be chosen
  the piece that would be built (`palace/FAMILY-(level+1)`, or the bonus piece), framed, in the
  largest rectangle of the part of the screen whose clicks choose it, so that a click on a
  piece chooses it (PR #11 drew its boxes at the screen's sides, with a click map of its own;
  bonus pieces 4 and 5 share their regions: the one shown is the one the game gives, 5 first).
  Some regions are narrow (the throne's, the bonus pieces'), so some pieces are small.
  The pictures the original drew for the room, enlarged, are no longer seen.
* **The spaceship** is drawn whole from the pack's side-view pieces (`ship/...`, 96 x 80, made
  for PR #11's own spaceship screen), in a layout of OpenCiv2's own built from the ship's actual
  parts: the game draws its ship as ever (`render_ship`: the original's cells, in its pixels;
  the same pictures, palettes and `rand()` calls), then `hd_spaceship`
  (`src/game/hd_graphics.cpp`; at the end of `render_ship`, or of `init_starfield` once
  launched) paints on the window's plane, over the ship's rectangle (457 x 304 at 175,99), the
  window's own picture (`ss.dll` 499; launched: the starfield's colour and the stars the game
  drew there) and then the ship, and the game's pixels there are taken as shown.  The ship lies
  along the pieces' own axis (sloping down to the right, 0.28), stern at the left: the
  components are up to eight rows, filled from the middle outwards, each a propulsion unit
  (`ship/propulsion`; launched, `ship/engine-active`, its engine firing) and a fuel tank
  (`ship/fuel`); the structurals are four truss rails (`ship/structural-spine`, at 0.44 of the
  scale, sheared by -0.084 so that its own slope, 0.364, is the axis') growing from the four
  middle tanks towards the bow a bay at a time, four structurals a bay (the game's rule: a pair
  of components for each four structurals), ending in a docking hub
  (`ship/structural-dock`); the habitation modules run along above the frame, the life support
  modules below it, the solar panels spread above and below them (two of each side).  A part
  built but not yet supported by the frame (beyond `spaceship_part_count`, which the original
  draws as its "unconnected" cell, the status's `*`) is drawn as a faded grey outline (40%,
  grey); a part not built is not drawn.  The scale is that of the complete ship (39
  structurals, 8 and 8 components, 4, 4 and 4 modules), fitted in the window's ship area clear
  of the text panel (236,104 to 626,398), so the ship grows in place, centred.  Once launched,
  the stars the game moves (`spaceship_wnd_starfield_tick`, pixel by pixel) stay behind the
  ship: where the ship is drawn, their pixels are taken as shown (`hd_spaceship_stars`).  The
  pack's other pieces (antenna, arrival, command, escape, fusion, radiator, the truss's cross
  and elbow) map to nothing the game counts, and are not drawn.
* **Palettes**: when a surface's colour table changes (`SetDIBColorTable`: a palette loaded
  for another picture, the throne room's colours given to its surface, all black until it is
  first shown), the plane's pixels that are the surface's pixels enlarged in the old colour
  take the new one.  The art (sprites, pictures) and sharp text keep their colours: they do
  not fade with the palette.  A plane composed whole (the throne room's) is left as it is by
  such a change (`hd_own`: it holds no pixels of the game's, enlarged, and a black part of
  the art must not take the colour of the pixel under it), so the room appears at once, in its
  colours, where the original's turns from black to its colours.

So nothing the game computes changes, nor any `rand()` call: the game's own pixels, which its
code reads back (map picking masks, editors), are the original's.  PR #11 learned the hard way
that the game's palettes take a version number from the game's random generator
(`GamePalette::new_version` calls `rand()`), so its renderers, which skipped loading the
original pictures, had to make up those calls ("Preserve original gameplay RNG consumption in
HD rendering"); here the original pictures are always loaded and drawn, so there is nothing to
make up.

The idea of the second plane kept up to date by the differences is Peter Johnson's, from
PR #11's `src/platform/hd.c`; this is a smaller implementation of it (about 900 lines against
some 20,000).

## The planes: sharp text and interface

The planes are the renderer; the art is one use of them.  `hd_on` says the planes are in use,
`hd_art` that the art is: with sharp text and interface ([EXTRAS.md](EXTRAS.md#sharp-text-and-interface),
`SharpText=`, on by default) the planes run without the art at the screen's own
density (as HD graphics' *Auto*), so that text and the platform's controls are drawn at the
screen's resolution while every sprite keeps its original pixels, enlarged (the sprites' calls
find no picture in an empty pack).  With HD graphics on, both: the art's density, the art, and
the sharp text and controls.  Where the screen's scale is 1 and HD graphics are off, there are no
planes.  In replays only `CIV2_TEST_SHARP=N` (density N) or
`CIV2_TEST_HD=N` turn the planes on, never in the unit tests.

Its cost, measured on this machine (one slow core) on a 1280x720 desktop (a 4K screen at
scale 3): composing the frame takes about 3 ms at density 2 and 5 to 7 ms at density 3 (0.3 ms
without the planes; it was 9 to 15 ms before the background was taken only where no window covers
it); a replay of loading a game, its advisor, city window and menus takes some 20% (density 2)
to 40% (density 3) more time than without (including the container's start).  In the browser, at a device pixel ratio
of 2 (a 2560x1600 frame), showing a frame takes some 11 ms either way; the module's memory on the
map is 179 MB with sharp text (87 MB without, 218 MB with HD graphics, its pictures of the other windows included).  The frame is composed
only when the screen changed (`plat_present`'s signature), as before.

## The choice (November 2026)

| | Effort | Risk | Look |
| --- | --- | --- | --- |
| 1. Port PR #11 onto main | Weeks: about 20,000 lines on a base some 90 commits older, much of it in `src/auto` files since split and retyped; its own renderers for the reports, city view, throne room, spaceship, diplomacy and Civilopedia; a GPU compositor and a smooth map camera | High: it replaces the original pictures' loading, so their palettes' `rand()` calls must be made up one by one; it reworks `src/auto` drawing; HD is its default | All of it, and a smooth zoomable camera |
| 2. A smaller renderer of our own using the pack (chosen) | Days | Low: the game's drawing untouched and always run; off by default; the pack fetched, pinned | The map (terrain, units, cities, improvements, resources), and wherever the same sprites and text appear (city window, status panel, advisors' unit and terrain pictures), sharp text everywhere |
| 3. Upscaling the original's pixels (a pixel-art filter) | Hours | None | Sharper edges, the same art |

The second gives most of the improved look where the player spends the game for a small part
of the code, keeps every original pixel and random number, and leaves the rest to be added
window by window.

## Licence and credits

The art: openciv2-assets by Peter Johnson, MIT licence (its `LICENSE`, `THIRD-PARTY.txt` and
`ATLAS-PROVENANCE.md` beside its pictures in `data/native/hd`, and on the browser version's site in
`hd/`); `THIRD-PARTY.txt` lists it.
The PNG decoder: stb_image 2.30 by Sean Barrett, public domain or MIT
(`src/third_party/stb_image`); the WebP decoder: libwebp 1.5.0's, Google's, BSD licence
(`src/third_party/libwebp`).  The renderer is OpenCiv2's, after Peter Johnson's design.
OpenCiv2's own pictures and heralds (`data/native/hd-extra`, the local staging folder) are under
its licence (MIT); the AI-generated ones' model, prompt and references are recorded with them
(the Russians' herald's under *Your own heralds*).

## Tests

* The browser version: `make web-test` (`src/web/test/hd.mjs`: Classic graphics unticked in the game's
  OpenCiv2 Options and the game restarted, the art loaded from the site as the page is loaded
  again, then a saved game's map at a device pixel ratio of 2, checked to be drawn in HD; then
  without the original's pictures, Classic graphics ticked: the set-up page over the game, the
  original's files added, the restart, Classic graphics; screenshots in `run/web-hd`).
* HD off (the default): `make test` (11,119 cases), `make native-suite EXE=OpenCiv2` (44 PASS),
  `make test-extras` (OpenCiv2 Options in the main menu: `x-options`).
* Sharp text and interface: `make test-extras` runs `harness/extras/x-sharp.txt` (the main menu,
  the real file dialog, the map, the Trade Advisor's scroll bar, the city window, the Game menu)
  and OpenCiv2 Options' `x-options-sharp` with `CIV2_TEST_SHARP=2`: their game pixels and
  `rand()` traces are those of the same runs without it, their `-hd` frames looked at by eye and
  blessed.  `src/web/test/hd.mjs` checks that the first screen's text is sharp, with HD graphics
  and with sharp text alone (`?hd=0`), and `?hd=0&sharp=0` the original's.
* The opening video: `CIV2_KEEP_MEDIA=1 make test-media` (`harness/media/m-opening.txt`): with
  HD off the video to its end, then the menu; its HD run (`m-opening-hd`) the menu from the
  first shot, its screens those of a start without the CD's `Video`, its `rand()` trace (773
  calls) and its sounds those of the run with the video.
* HD on: `harness/hd/hd-map.txt` (a saved game's map, zoomed), `harness/hd/hd-cityview.txt`
  (the city view), and any replay script with `CIV2_TEST_HD=2`, looking at its `-hd` shots:
  `newgame.txt` (the new game's backdrop and pictures), `harness/hd/hd-pictures.txt` (the eight
  announcements' pictures, each in its popup), `talks.txt` (the diplomacy room, the
  leader's portrait, the icons), `harness/hd/hd-talks.txt` (six civilizations' leaders, and the
  room under each of the seven governments), `harness/hd/hd-talks-all.sh` (every tribe's
  leader, man and woman, from a new game: a contact sheet beside the original's), `harness/hd/hd-cityview-eras.txt` (a city view
  ancient, industrial and modern, on a 640x480 desktop), `harness/hd/hd-herald.txt` (an animated herald in the room), `harness/hd/hd-herald-own.sh` (OpenCiv2's own Russian herald and leaders: *Your own heralds*), `harness/hd/hd-redraw.txt` (the map's redraws under a moving unit, at
  every zoom, each slide's plane compared with a full render: its results in `hook.log`, "0 of
  16 slides left pixels" at every zoom; the script waits for the probe, `replay_hold`; near
  the game's own pixels that a partial redraw leaves otherwise, at zooms 4, 6 and 8 where its
  scaled terrain clipped rounds otherwise, a unit's frame is not compared),
  `harness/hd/hd-sentry.txt` with `CIV2_TEST_EXTRAS=41` (a unit on sentry, grey, on the
  view mode's cursor's square), `harness/hd/hd-disorder.txt` (a city in civil disorder, `hd_test_city_disorder`: the
  marker at five zooms, the Romans' and the Sioux'; logical screenshots and `rand()` trace those
  of HD off), `harness/hd/hd-units.txt` (the shields, a stack, an
  occupied airbase, at several zooms), `harness/hd/hd-advances.txt` (the
  advances' pictures in the Science Advisor, a discovery's announcement, the Civilopedia's page,
  tree and list, the choice of research), `advisors.txt` (the advisors' backgrounds, OpenCiv2's own),
  `harness/hd/hd-advisors.txt` (with `CIV2_TEST_FULL=800x600`, in the World War I scenario: every
  screen with OpenCiv2's own background once, F1 to F9, F11, the Foreign Minister's intelligence
  report, the Defense Minister's casualties and the tax rates; with `CIV2_TEST_FULL=1280x1024`,
  `harness/hd/hd-advisors-large.txt`, the tax rates on a large screen), `reports.txt` (the
  throne room), `harness/hd/hd-throne.txt` (the throne room at several levels, improved 34
  times by clicks on its choice screen, with the choice screens, on a 640x480 desktop: its
  logical screenshots and `rand()` trace are those of the same run with HD graphics off), `screens.txt` (the spaceship's window), `harness/hd/hd-spaceship.txt` (the ship's window empty, partly built with parts not yet supported, complete, launched: its logical screenshots and `rand()` trace are those of the same run with HD graphics off).  With `CIV2_TEST_HD=2`, for
  `harness/scripts/citywindow.txt` and `newgame.txt` every logical
  screenshot, the `rand()` trace (1,049 and 13,280 calls) and the sounds are identical to the
  original's recording; the `-hd` screenshots are looked at by eye.  On the replays'
  reference desktop the game's window is 600x438 at its top left and the city window larger,
  so `citywindow.txt` shows it cut off, as the original's recording does (the HD frame is that
  area times the density); `harness/hd/hd-city.txt` shows it whole on a 640x480 desktop.  The native and portable
  builds draw identical HD frames.  Without the art (no `data/native/hd/pictures.txt`: a build
made with `HD_ART=`, or the folder removed) the game logs it and plays as ever: HD graphics
chosen give the original's pictures with sharp text and interface where the screen has the
pixels; without the install's pictures too (*Without the install*, NO-INSTALL.md) it stops with
`hd.err.no_graphics` (neither the original's pictures nor the art).

## The pictures drawn

The keys hd.c draws (`data/hd-pack.json`'s `keep` but its `drop` and `exclude`: 1,355 keys, 833 distinct
pictures; the whole pack holds 2,166, 309 MB):

* `crop/file/...`, `tint/crop/...`, `dither/...`, `fog/...`, `file/...`: the map's sheets, but
  the city's disorder marker (`crop/file/icons/166,223,30,30`: `NOT_CROPPED_KEYS`);
* `crop/resource/...` (157): the sprites taken from the libraries' pictures, but for the
  sheet of small faces (`mk.dll` 210, 211: hd.c's `NOT_CROPPED`), the leaders' portraits no
  tribe is given (`mk.dll` 225, 227, 230, 231, 238, 240, 246, 248, 251, 252, 259, 261:
  `hd_portraits`; `exclude`), three of
  `tiles.dll` 86 (`NOT_CROPPED_KEYS`), and of the city
  view's `cv.dll` 310 in the rows at y 1, 116, 346 and 429 (`drop`; the houses of the eras
  but the industrial are bound by key instead, `house/ERA/...`);
* `advance/` (58): the advances;
* `shield`: the units' shields;
* `palace/` (the furnishings and bonus pieces, 39, but not `palace/decor-`) and
  `resource/pv.dll/gifs/100` (the empty hall): the throne room;
* `resource/mk.dll/gifs/200` to `206`, `resource/tiles.dll/gifs/70`, `72`, `73`, `74`, `76`, `90`, `resource/intro.dll/gifs/901` to `913`, `resource/cv.dll/gifs/340`, `341`, `345`, `346`, `350`, `351`,
  `resource/ss.dll/gifs/498`, `499` (hd.c's `WHOLE`);
* OpenCiv2's own (`data/native/hd-extra`): `resource/tiles.dll/gifs/50` to `59`, `65`, `66` (the
  pack has the same paintings since pull request 6, kept by the tool for synth.c only, at the
  original's size);
* `file/editoras`, `file/editorsa`, `file/editorsq`, `crop/resource/wonder.dll/gifs/20000` to `20027` (`1,1,64,32`):
  the pack's since openciv2-assets pull request 7 (above);
* `ship/propulsion`, `engine-active`, `fuel`, `structural-spine`, `structural-dock`,
  `habitation`, `life-support`, `solar`: the spaceship (`hd_spaceship`).

## Not yet done

* *SentryHalftone*: a sentried unit on the cursor's square was once seen in its colours in the
  HD frame; not seen again.  Every draw of a sleeping unit on the map goes through
  `extras_draw_grey_sprite` (`gfx_draw_unit`; the blinking redraws the cursor's tile from
  scratch, `MapView::redraw_area`, and keeps no unit's picture), and `harness/hd/hd-sentry.txt`
  (the phalanx of the autosave active, then on sentry with the view mode's cursor on its square)
  shows it grey at every moment looked at.  A possible cause, since removed: a copy between
  surfaces whose colour tables differ (the slide's and the blink's background surfaces) could
  give art the destination's colour for its pixel's index (`hd_big`, above).
* The Zulus' leaders have no portrait in the pack that fits (above): the original's, enlarged,
  or without the install the dark frame.  The heralds' videos are the original's, enlarged smoothly (the pack has none), but where OpenCiv2
  has one of its own (*Your own heralds*: the Russians').
* The city view's empty lots (woods) are the original's, enlarged, awaiting art.
* The pack's pictures not drawn (made for PR #11's own renderers, laid out otherwise): the
  throne room's other pictures (`palace-resource/`, `pv.dll` 105 to 184, `palace/decor-`);
  the announcements' tall pictures (`tiles.dll` 71, 75, 77: wide vignettes in a tall frame)
  and the wonders' (`wonder.dll`: the original's are the movie window's border tile); and
  the framed parchments of `mk.dll` 10000 to 10002 and
  `civ2art.dll` (the credits' text is drawn for a black background); the spaceship's pieces not
  drawn (below); the city view's
  larger `city/buildings.*` and `wonders.*`, `building/`, `house/`, `lot/` (its sprites have
  their crops; `house/` and `lot/` are the same 19th-century houses); the cursors and icons (`resource/civ2.exe/12/...`, `14/...`) and the map
  editor's (`resource/civ2map.exe/...`: the editor program is not part of OpenCiv2).
* The spaceship's arrival (the game's own screen) and its launch video are the original's;
  the spaceship's window is not the original's composition (above: its own layout), and the
  pack's pieces not drawn (antenna, arrival, command, escape, fusion, radiator, the truss's
  cross and elbow) await a use; the original's cells (`ss.dll` 400 to 491) have no pictures
  in the pack (its pictures under those keys are placeholders).  `harness/hd/hd-spaceship.txt`
  (`hd_test_spaceship`: none built; 12/4/4/1/1/1, with parts not yet supported; 20/4/3/2/1/1;
  the complete ship; the complete ship launched).
* The new game's pictures are vignettes over black (the frame the game draws around them
  holds nothing else); the city view's landscape is the pack's, whose coast need not be where
  the game puts its harbour.
* The city size box is the original's, enlarged; in places the larger unit art covers the
  shield (the game draws the unit after its shield).
* Fades: none to do.  The game fades only through the display's palette
  (`GamePalette::fade_step`, from `screen_fade_out`/`screen_fade_in`: the screen to grey and
  back around the council and the wonders' movies), and only when the display is palettized
  (`display_palette.palettized`, from `GetDeviceCaps(RASTERCAPS) & RC_PALETTE`); the platform
  is a true-colour display (as Wine's, where the reference recordings were made), so those
  fades never run, in the original under Wine nor here.  Every colour table change of a
  surface seen in the reports, the end of the game and the council (a log of each
  `SetDIBColorTable`: the table's brightness before and after) sets a whole palette at once,
  from black (a new surface) or for another picture; none steps towards or from black.  The
  throne room's, the spaceship's arrival, the destroyed civilizations' and the end of the game's
  "dissolves" are `gfx_dissolve_blit`: the new picture copied to the window square by square in
  a random order, which the planes follow as any copy (the throne room's pack art appears
  square by square).  Should a fade ever run (a palettized display), the art and sharp text
  would keep their colours (above, *Palettes*).
* Speed: the frame is composed on the CPU each time it is shown (at density 3 on a 4K screen,
  some 33 MB per frame); a texture per window (PR #11's GPU compositor) would be faster.
* The game's own dialogs' frames and buttons are its own pixels enlarged (straight lines, so
  as crisp as a Windows desktop at that scale draws them; their text is sharp); its radio
  buttons and check boxes are its own pictures, enlarged.
* The art has only the pictures hd.c draws and synth.c reads
  (`data/hd-pack.json`: `keep`, `drop`, `synth`, the keys' first parts); a picture of another
  kind drawn or read is to be added there and the art made again (`make hd-art`).
