# HD graphics

OpenCiv2 can draw the game with the replacement art of
[openciv2-assets](https://github.com/PeterJohnson/openciv2-assets) by Peter Johnson (MIT
licence): painted terrain, units, cities and resources at 4 to 8 times the original's
resolution, shown at the screen's full resolution, with text drawn sharp at that resolution
too.  It is optional and off by default: the original graphics stay the game's look.  This is
a prototype: the map is done, and many of the other windows' pictures (below).

## Playing with it

The simplest way: in the game's main menu choose *OpenCiv2 Options*, then *Graphics and screen*,
tick *HD graphics* and OK (docs/EXTRAS.md, "OpenCiv2 Options"): the art is downloaded and
installed the first time (below), and the game offers to restart, which turns them on.
Otherwise:

1. Install the art, any of these ways:

   * in OpenCiv2 Options: ticking *HD graphics* (and OK) downloads the release pinned
     in `data/hd-pack.json` (234 MB, once) with the system's `curl` (on Windows 10 and later
     `curl.exe`; on Linux `wget` where there is no curl), showing its progress, with a Stop
     button; checks its SHA-256 and each file's (as below), and installs the files in the
     user's own OpenCiv2 folder: `${XDG_DATA_HOME:-~/.local/share}/OpenCiv2/hd` on Linux (beside
     `GameData`), `~/Library/Application Support/OpenCiv2/hd` on macOS,
     `%LOCALAPPDATA%\OpenCiv2\hd` on Windows.  The archive is kept in the same cache as
     `tools/fetch_hd_pack.py`'s (it can be deleted once installed).  If the download or a check
     fails, the dialog says why and HD graphics stay off (src/game/options.cpp,
     src/host/host_options.c);
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

Without the art the game says so in its log and plays with the original graphics (sharp text and
interface still, where they are on: below).  HD graphics are never on with `CIV2_CLASSIC=1`, in
replays (but with `CIV2_TEST_HD=N`, density N) or in the unit tests.  `CIV2_HD_PACK=FILE` names another pack.

With HD graphics on (the art in use, not sharp text alone), the game goes straight to its main
menu: the opening video (`Video\OPENING.AVI`, 92 seconds, where the game folder has the CD's
`Video`) is not played, the start-up being the original's without the CD's `Video` (the menu
starts the theme music; `intro_play_opening_video`, src/auto/game.cpp).  The one `rand()` call
opening the video makes (the player's palette, `avi_open_file`) is still made, so the game's
random numbers are those of a start with the video.  With HD graphics off the video plays as
ever (a click, or Escape, Space or Enter, ends it).

In the browser version: *HD graphics* on its `?setup` page, the art downloaded or chosen there and
kept in the browser, or `?hd=N` in the address ([BROWSER.md](BROWSER.md#hd-graphics)).

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
| `resource/MODULE/gifs/ID` | the pictures of the game's libraries (`mk.dll`, `pv.dll`, `cv.dll`, `tiles.dll` ...) by resource id, at the original's size (below) |
| `crop/resource/MODULE/gifs/ID/x,y,w,h` | the sprites the game takes from those pictures (the city view's improvements and houses, the leaders' portraits, the diplomacy screen's icons, the spaceship's buttons) |
| `palace/FAMILY-LEVEL`, `palace/bonus-I` | the throne room's furnishings (arch, rug, dais, columns, tapestry, brazier, throne, ceiling medallion; levels 1 to 4) and its seven bonus pieces, which `src/game/hd_graphics.cpp` places in the pack's empty hall (`resource/pv.dll/gifs/100`) |
| `palace-resource/ID` | the original's throne room pieces redrawn (`pv.dll`; not used here) |
| `city/...`, `building/...`, `house/...`, `lot/...`, `advance/...`, `wonder/...` ... | pictures that PR #11's renderers of their own place themselves (not used here) |

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
  saves `NAME-hd.bmp` (the frame).

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
  which as a border tile would be absurd, so the window keeps the original's
  (`harness/media/m-wonder.txt`, HD run: the Oracle's and the Pyramids' windows).  The advisors' backgrounds (`tiles.dll` 50 to 59: the advisors, the power
  graph, the Hall of Fame ...; 65 and 66: the tax rates) are the original's, enlarged: the pack's
  are a pale parchment, and the game's text on them (white, light grey, yellow, coloured for the
  original's dark grey photographs) could not be read: measured, the WCAG contrast of that text
  is 1.0 to 1.4 on the parchment (its average colour under the text, 229,220,198) against 4.0 to
  7.5 on the original's (about 90,90,90); a dark overlay bringing light grey text to 4.5 would
  leave the parchment nearly black.  Single crops not drawn either (`NOT_CROPPED_KEYS` in hd.c):
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
  no sprites: the custom tribe's portrait chooser copies them whole, the original's) and each tribe and gender is given the portrait that fits it best, by region, then
  era, several tribes sharing one (`hd_portraits` in `src/game/hd_graphics.cpp`, bound by
  `hd_diplomacy_portrait` in `DiploLeaderWindow::init_portrait`).  The portrait's number chooses,
  as in the game: the tribe's slot (a scenario's tribe in a slot has the slot's portrait), or
  the face a custom tribe chose in the portrait chooser.  The pack's bust is drawn in the
  sprite's frame, centred on the panel, as its art is laid out.

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
  Africans (225, 246) are West African, in indigo robes and headwraps.  Not given to any tribe:
  the Andean (227, 248), Southeast Asian (230, 251), Pacific (231, 252), modern South Asian
  (238, 259) and modern Mediterranean (240, 261) portraits.  `harness/hd/hd-talks-all.sh` shows
  every tribe's leader, man and woman, beside the original's.  The leader's animation (*Animated
  Heralds*: the video `KINGS\HRLDxxx.AVI` from the CD, over the portrait) is the video's frames
  enlarged smoothly (below, *Videos*): the pack has no pictures for it; the herald, a whole
  figure standing at the portrait's left, is drawn over the pack's portrait as over the
  original's (its transparent pixels show the room and the portrait).  Each pixel the decoder
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
  on, by `civ_city_style_era` of the owner, the same for the whole view) are the pack's only in
  the industrial era: its houses are all 19th-century buildings (terraced houses, a tenement
  with a fire escape, a clapboard house, a shop row), the same pictures for both rows, so the
  other eras keep the original's, enlarged (`hd_bind_cityview` in `src/game/hd_graphics.cpp`,
  called once the view has taken its sprites), awaiting art of their own; so do the empty lots,
  the original's woods, which the pack leaves empty (its renderer drew lot pictures of its
  own).  All of such a picture's pixels are shown over the pack's landscape, also those the
  same colour as the landscape the game has underneath, which were not seen as drawn and let
  the landscape show through the trees in specks (`hd_sprite_pixels_shown` after each lot's
  draw in `CityViewWindow::draw_buildings`, as for the city's disorder marker; the pixels put on
  the plane in one call, `hd_bits_redrawn_mask`: one call per pixel, each looking the surface up
  among the bitmaps, took 5 seconds to open the view).  Its landscapes (`cv.dll` 340 + 5 x the site + `civ_city_art_era`) are grass with dirt
  tracks in every era, so only those of the eras before the paved roads (340, 341, 345, 346,
  350, 351) are the pack's; the modern ones and those with Superhighways are the original's,
  enlarged.  Its improvements and wonders are one picture each in every era, the original's as
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
`SharpText=`, on by default in Standard mode) the planes run without the art at the screen's own
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

The art: openciv2-assets by Peter Johnson, MIT licence (installed beside the pack as `LICENSE`,
`THIRD-PARTY.txt` and `ATLAS-PROVENANCE.md`), never committed here; `THIRD-PARTY.txt` lists it.
The PNG decoder: stb_image 2.30 by Sean Barrett, public domain or MIT
(`src/third_party/stb_image`).  The renderer is OpenCiv2's, after Peter Johnson's design.

## Tests

* The browser version: `make web-test` (`src/web/test/unit.mjs`: the art's checks, its chunks in
  the store, the pack pruned; `src/web/test/hd.mjs`: HD graphics not offered in the game's
  OpenCiv2 Options where the site has no art, then ticked there where it has and the game
  restarted, the art downloaded from the site, checked and kept as the page is loaded again, then a saved game's map at a device pixel ratio of 2, checked to be drawn
  in HD; screenshots in `run/web-hd`).
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
  ancient, industrial and modern, on a 640x480 desktop), `harness/hd/hd-herald.txt` (an animated herald in the room), `harness/hd/hd-redraw.txt` (the map's redraws under a moving unit, at
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
  tree and list, the choice of research), `advisors.txt` (the advisors' backgrounds, the original's), `reports.txt` (the
  throne room), `harness/hd/hd-throne.txt` (the throne room at several levels, improved 34
  times by clicks on its choice screen, with the choice screens, on a 640x480 desktop: its
  logical screenshots and `rand()` trace are those of the same run with HD graphics off), `screens.txt` (the spaceship's window), `harness/hd/hd-spaceship.txt` (a ship with parts).  With `CIV2_TEST_HD=2`, for
  `harness/scripts/citywindow.txt` and `newgame.txt` every logical
  screenshot, the `rand()` trace (1,049 and 13,280 calls) and the sounds are identical to the
  original's recording; the `-hd` screenshots are looked at by eye.  On the replays'
  reference desktop the game's window is 600x438 at its top left and the city window larger,
  so `citywindow.txt` shows it cut off, as the original's recording does (the HD frame is that
  area times the density); `harness/hd/hd-city.txt` shows it whole on a 640x480 desktop.  The native and portable
  builds draw identical HD frames.  Without the pack the game logs it and plays as ever.

## The pictures drawn

For a stripped pack, the keys hd.c draws (1,288 pictures, 116 MB of PNGs, about 96 MB with each
repeated PNG once; the whole pack holds 2,105, 250 MB):

* `crop/file/...`, `tint/crop/...`, `dither/...`, `fog/...`, `file/...`: the map's sheets, but
  the city's disorder marker (`crop/file/icons/166,223,30,30`: `NOT_CROPPED_KEYS`);
* `crop/resource/...` (159): the sprites taken from the libraries' pictures, but for the
  sheet of small faces (`mk.dll` 210, 211: hd.c's `NOT_CROPPED`), the leaders' portraits no
  tribe is given (`mk.dll` 225, 227, 230, 231, 238, 240, 246, 248, 251, 252, 259, 261:
  `hd_portraits`), three of
  `tiles.dll` 86 (`NOT_CROPPED_KEYS`), and of the city
  view's houses (`cv.dll` 310) all but the industrial era's (y 231 and 512);
* `advance/` (58): the advances;
* `shield`: the units' shields;
* `palace/` (the furnishings and bonus pieces, 39, but not `palace/decor-`) and
  `resource/pv.dll/gifs/100` (the empty hall): the throne room;
* `resource/mk.dll/gifs/200` to `206`, `resource/tiles.dll/gifs/70`, `72`, `73`, `74`, `76`, `90`, `resource/intro.dll/gifs/901` to `913`, `resource/cv.dll/gifs/340`, `341`, `345`, `346`, `350`, `351`,
  `resource/ss.dll/gifs/498`, `499` (hd.c's `WHOLE`).

## Not yet done

* *SentryHalftone*: a sentried unit on the cursor's square was once seen in its colours in the
  HD frame; not seen again.  Every draw of a sleeping unit on the map goes through
  `extras_draw_grey_sprite` (`gfx_draw_unit`; the blinking redraws the cursor's tile from
  scratch, `MapView::redraw_area`, and keeps no unit's picture), and `harness/hd/hd-sentry.txt`
  (the phalanx of the autosave active, then on sentry with the view mode's cursor on its square)
  shows it grey at every moment looked at.  A possible cause, since removed: a copy between
  surfaces whose colour tables differ (the slide's and the blink's background surfaces) could
  give art the destination's colour for its pixel's index (`hd_big`, above).
* The leaders' portraits are the pack's regional archetypes, not the tribes' own leaders
  (*The diplomacy room*, above: the table): candidates for portraits of their own are the
  Zulus' (the original's kept: the pack has no southern African), the Japanese (modern
  diplomats for a tribe of samurai and court ladies in the original), the Egyptians' and
  Carthaginians' (North Africans in headcloths, not pharaonic or Punic), and the tribes that
  share one (Babylonians and Persians, Russians and French, English and Spanish, Celts and
  Vikings).  The heralds' videos are the original's, enlarged smoothly: the pack has none.
* The city view's houses but in the industrial era, its empty lots (woods), and its modern
  landscapes are the original's, enlarged, awaiting art of their eras.
* The pack's pictures not drawn (made for PR #11's own renderers, laid out otherwise): the
  throne room's other pictures (`palace-resource/`, `pv.dll` 105 to 184, `palace/decor-`);
  the announcements' tall pictures (`tiles.dll` 71, 75, 77: wide vignettes in a tall frame)
  and the wonders' (`wonder.dll`: the original's are the movie window's border tile); the advisors' parchments (`tiles.dll` 50 to 59, 65, 66: above) and
  the framed parchments of `mk.dll` 10000 to 10002 and
  `civ2art.dll` (the credits' text is drawn for a black background); the spaceship's parts
  (below); the city view's
  larger `city/buildings.*` and `wonders.*`, `building/`, `house/`, `lot/` (its sprites have
  their crops; `house/` and `lot/` are the same 19th-century houses); the cursors and icons (`resource/civ2.exe/12/...`, `14/...`) and the map
  editor's (`resource/civ2map.exe/...`: the editor program is not part of OpenCiv2).
* The spaceship's parts are the original's, enlarged, over the pack's window (`ss.dll` 498,
  499).  The game draws the ship from its sheets of parts (`render_ship`: the structure, 400 to
  440, a picture of the whole frame at each count of structurals, and the components' and
  modules' sheets 441/442, 455/456, 470/471, 480/481, 489/490, 491, each a row of cells, built
  and not built, of the shapes seen at the ship's own angle: wedges, panels, discs, cut to fit
  their neighbours) into one picture.  The pack has none of those cells: its pictures under
  those keys are placeholders (400 to 440 one picture, each sheet a single truss at the
  sheet's size), and its `ship/` modules (a command module, fuel tank, solar panel ... 96x80,
  isometric) are made for PR #11's own spaceship screen, a grid of its own (`hd_spaceship`), and
  map to no cell of the original's; laid in the original's cells they would be a collage
  without the ship's shape.  `harness/hd/hd-spaceship.txt` (a ship with 20 structurals and
  some components and modules, `hd_test_spaceship`).
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
* The browser version gives the game only the pictures hd.c draws (`src/web/hdpack.js`: `KEEP`,
  the keys' first parts, and the same PNG once where the pack repeats it); a picture of another
  kind drawn here must be added there.  The page
  cannot download the art from GitHub itself (no CORS on release downloads): the player chooses
  the downloaded `.zip`, or the site serves it (`make web HD=1`).
