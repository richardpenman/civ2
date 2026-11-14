# HD graphics

OpenCiv2 can draw the game with the replacement art of
[openciv2-assets](https://github.com/PeterJohnson/openciv2-assets) by Peter Johnson (MIT
licence): painted terrain, units, cities and resources at 4 to 8 times the original's
resolution, shown at the screen's full resolution, with text drawn sharp at that resolution
too.  It is optional and off by default: the original graphics stay the game's look.  This is
a prototype: the map is done, much of the rest follows from it (below).

## Playing with it

The simplest way: tick *HD graphics* in the game's opening window (docs/EXTRAS.md, "The opening
window"), which downloads and installs the art the first time (below), then Play.  Otherwise:

1. Install the art, any of these ways:

   * in the opening window: ticking *HD graphics* (or *Download*) downloads the release pinned
     in `data/hd-pack.json` (234 MB, once) with the system's `curl` (on Windows 10 and later
     `curl.exe`; on Linux `wget` where there is no curl), showing its progress, with a Stop
     button; checks its SHA-256 and each file's (as below), and installs the files in the
     user's own OpenCiv2 folder: `${XDG_DATA_HOME:-~/.local/share}/OpenCiv2/hd` on Linux (beside
     `GameData`), `~/Library/Application Support/OpenCiv2/hd` on macOS,
     `%LOCALAPPDATA%\OpenCiv2\hd` on Windows.  The archive is kept in the same cache as
     `tools/fetch_hd_pack.py`'s (it can be deleted once installed).  If the download or a check
     fails, the window says why and HD graphics stay off (src/host/host_launcher.c);
   * `make hd` (or `python3 tools/fetch_hd_pack.py`) in a source checkout: it downloads the
     release pinned in `data/hd-pack.json` (234 MB, once, into `~/.cache/openciv2`), checks its
     SHA-256 and each file's, and installs `atlas-hd.pack` with its licence and provenance in
     `data/native/hd` (not part of the repository: `.gitignore`).

   The game looks for `atlas-hd.pack` in `CIV2_HD_PACK`, then its data folder's `hd`
   (`data/native/hd`), then the user's folder's `hd`.
2. Switch it on, any of these ways (the first that says anything wins):

   * on the command line: `OpenCiv2 --hd` (or `--hd=N`), e.g.
     `build/OpenCiv2 ~/Civ2 --hd`;
   * in the environment: `CIV2_HD=1` (or `=N`);
   * in `CIV.INI` (the game's settings, in its Windows directory; the opening window writes
     them):

     ```ini
     [OpenCiv2]
     Graphics=HD
     ```

   `1` (and the bare `--hd`, and `Graphics=HD`) is on at the screen's own density; `N` from 2 to
   4 is on at that density; `0` is off (`--hd=0` turns it off whatever the environment or
   `CIV.INI` say, `CIV2_HD=0` whatever `CIV.INI` says).  The density (Auto, 1 to 4 in the
   opening window) is how many screen pixels
   each of the game's pixels gets each way: by default the game's own scale (docs/EXTRAS.md,
   Screen scale), so on a 4K screen 3, on a 1920x1080 one 1 (the new art at the original's
   size); `GraphicsDensity=N` (1 to 4) in `CIV.INI` sets it for the bare `--hd`, `CIV2_HD=1`
   and `Graphics=HD`.  It takes effect the next time the game starts.  The portable build's
   set-up takes `--hd` out of its arguments (as `--fix-cue-gaps`) and passes it to the game
   as `CIV2_HD_ARG`.

Without the art the game says so in its log and plays with the original graphics.  HD graphics
are never on with `CIV2_CLASSIC=1`, in replays (but with `CIV2_TEST_HD=N`, density N) or in
the unit tests, and not in the browser version.  `CIV2_HD_PACK=FILE` names another pack.

## What the art is

The release (`openciv2-assets-2026.11.08.1.zip`) holds `data/atlas-hd.pack` (250 MB), two
bitmap font caches of DejaVu glyphs (`fonts.bin`, `fonts_aa.bin`, not used here: OpenCiv2 has
its own fonts), a generated layout header and a layout contract for PR #11's renderer
(`include/hd_layouts.h`, `data/cityview-rects.json`, not used here), `package.json` (each
file's SHA-256), `LICENSE` (MIT), `THIRD-PARTY.txt` and `data/ATLAS-PROVENANCE.md`.  The
provenance says the paintings were made with an image generator from written descriptions and
the rest drawn as vector geometry, with no pixels of the original game's or Freeciv's art.

`atlas-hd.pack`: `C2ATLAS1`, a count, a 256-colour palette, then 2,105 entries of 180 bytes
(a key, the picture's size in the game's pixels, the offset and length of its PNG, and for
whole sheets an indexed copy), keys in ascending order, then the PNGs.  The keys that matter
here:

| Key | What |
| --- | --- |
| `crop/file/SHEET/x,y,w,h` | 499 pictures, each replacing the sprite the game takes from that rectangle of `TERRAIN1`, `TERRAIN2`, `UNITS`, `CITIES`, `ICONS`, `PEOPLE` or `EDITORPT` (4 times as large) |
| `tint/crop/.../COLOUR` | 456 of those in each civilization's colour (flags, shields) |
| `dither/T/Q`, `fog/Q` | the blends of each terrain into its neighbours, by quarter of a tile (the game makes these itself from a mask) |
| `file/SHEET` | the whole sheets at the original's size |
| `resource/MODULE/gifs/ID`, `city/...`, `palace/...`, `advance/...` ... | pictures for the other windows (city view, throne room, advisors, Civilopedia ...) that PR #11 draws with renderers of its own |

## How it is drawn

The game is left to draw exactly as it does, into its 8-bit surfaces; its code is not changed
but for calls added at a few of its drawing functions, which do nothing unless HD graphics are
on (`src/auto/graphics_surface.cpp`, `src/auto/graphics.cpp`, `src/game/hd_graphics.cpp`).
The rest is in the platform layer (`src/platform/hd.c`):

* A surface (a `Bitmap`) can have a **plane**: true-colour pixels, density times larger each
  way, and a copy of the surface's own pixels as the plane last took them.  A surface gets one
  when an HD sprite is drawn on it, a surface with one is copied to it, or it is a window
  shown.
* **Sheets**: when the game loads one of its sheets (`TERRAIN1.BMP`/`.GIF` ...) from its own
  folder, the surface remembers which; a scenario's own sheets are its own art and keep it.
* **Sprites**: when the game takes a sprite from a sheet (`Sprite::encode_from_surface`) at a
  rectangle the pack has a picture for, the sprite gets that picture; the terrain blends get
  theirs by role after `gfx_load_terrain_sprites`.  A sprite taken from what is on a surface
  with a plane (a unit's background, kept to restore it) keeps the plane's pixels.  When the
  game draws a sprite (`Sprite::blit_to_surface`, `blit_silhouette_to_surface`), it draws as
  ever, then the picture is drawn on the plane over the sprite's frame, scaled to the zoom and
  the density (filtered; the base terrain diamonds exactly inside their diamond, so that tiles
  meet without seams), and the game's pixels it covered are taken as shown.  A flag recoloured
  to a civilization (`Sprite::replace_color`) takes the pack's picture in that colour.
* **Everything else** the game draws (text, fills, lines, sprites without a picture) is found
  by comparing the surface's pixels with the plane's copy, and shown enlarged.  Fills, and
  copies from surfaces without a plane, are shown even where the pixels did not change.
* **Copies** between surfaces (`BitBlt`, `gfx_copy_pixels`) copy the planes too.
* **Text** on a surface with a plane is drawn again there from the glyphs of the same font
  density times as tall (OpenCiv2's own fonts, to 48 pixels), each glyph where the game's is.
* **The screen**: the desktop is composed as before, and beside it the HD frame, from the
  windows' planes (or their pixels enlarged), with the colour correction; the host shows that
  frame (`host_present_hd`) in the same place, at its density.  A replay's `shot NAME` also
  saves `NAME-hd.bmp` (the frame).

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

The art: openciv2-assets by Peter Johnson, MIT licence (installed beside the pack as `LICENSE`,
`THIRD-PARTY.txt` and `ATLAS-PROVENANCE.md`), never committed here; `THIRD-PARTY.txt` lists it.
The PNG decoder: stb_image 2.30 by Sean Barrett, public domain or MIT
(`src/third_party/stb_image`).  The renderer is OpenCiv2's, after Peter Johnson's design.

## Tests

* HD off (the default): `make test` (11,119 cases), `make native-suite EXE=OpenCiv2` (44 PASS),
  `make test-extras` (28 PASS: the opening window's script, x-launcher, replaced the options
  dialog's).
* HD on: `harness/hd/hd-map.txt` (a saved game's map, zoomed) and any replay script with
  `CIV2_TEST_HD=2`: for `harness/scripts/citywindow.txt` and `newgame.txt` every logical
  screenshot, the `rand()` trace (1,049 and 13,280 calls) and the sounds are identical to the
  original's recording; the `-hd` screenshots are looked at by eye.  On the replays'
  reference desktop the game's window is 600x438 at its top left and the city window larger,
  so `citywindow.txt` shows it cut off, as the original's recording does (the HD frame is that
  area times the density); `harness/hd/hd-city.txt` shows it whole on a 640x480 desktop.  The native and portable
  builds draw identical HD frames.  Without the pack the game logs it and plays as ever.

## Not yet done

* The other windows' own pictures (city view, throne room, advisors' backgrounds, Civilopedia
  and advance pictures, diplomacy portraits, spaceship): the pack has them, keyed by resource
  (`resource/MODULE/gifs/ID`); a loaded GIF resource with a whole-picture entry could get a
  plane drawn from it, as sheets do.
* Unit shields and the city size box are the original's, enlarged (the pack's `shield` is an
  alpha mask to be tinted); in places the larger unit art covers the shield.
* Palette changes of an 8-bit surface (a flash, a fade) do not reach pixels already in a
  plane; the screen's grey fade does not grey the frame.
* Speed: the frame is composed on the CPU each time it is shown (at density 3 on a 4K screen,
  some 33 MB per frame); a texture per window (PR #11's GPU compositor) would be faster.
* The game's own dialogs' frames and buttons are enlarged pixels (their text is sharp).
* The browser version (its set-up page has no HD choice).
