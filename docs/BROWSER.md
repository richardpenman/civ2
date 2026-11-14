# The browser version

OpenCiv2 also runs in a web browser: the same game, compiled to WebAssembly, with the original
graphics and your own copy of Civilization II. How it is built and how it works inside:
[DEVELOPMENT.md](DEVELOPMENT.md) (the browser section).

## Building and serving it

`make web` builds a static site into `build/web`: a page, its scripts, the game as a 3.0 MB
WebAssembly module, the set-up as a second 0.8 MB module, and the fonts (`make web HD=1`: the HD
graphics' art too, below). There is no server-side
code: any web server can serve the folder.

```sh
make web                                   # first time: downloads Binaryen into tools/toolchain
python3 -m http.server -d build/web 8000   # then open http://localhost:8000
```

It is tested in Chrome, and should run in any browser from 2023 on (Edge 103, Firefox 113,
Safari 16.4).

## Your copy of the game

The first time, the page asks for your copy of Civilization II Multiplayer Gold Edition. Any of
these can be chosen, or dropped on the page:

- **The installed game:** the folder it is installed in, or a `.zip` of that folder. A folder the
  desktop OpenCiv2 set up (`GameData`) works too, music included.
- **The CD:** the folder it is mounted at (the one with `DATA1.CAB` and `CIV2`).
- **A disc image of the CD:** its `.cue` and `.bin`, chosen or dropped together.

From the CD or a disc image, the page sets the game up as the desktop version's set-up does: it
unpacks the game from the installer's cabinet and copies the videos. From a disc image it also
encodes the music tracks as Ogg Vorbis (`Music/TrackNN.ogg`); this takes a few minutes, with a
progress line and a Stop button. A mounted CD gives no music: browsers cannot read a CD's audio
tracks, but a disc image has them.

The game is copied into the browser's storage (IndexedDB), so later visits start the game at
once. The page asks the browser to keep that storage for good, so that it is not cleared when
disk space runs low. `?setup` (`http://localhost:8000/?setup`) shows the set-up page again: to
replace the game files or remove them.

Music plays where the game's folder has it (`Music/TrackNN.ogg`).

## How to play: the opening window's choices

The desktop version asks how to play in its opening window ([EXTRAS.md](EXTRAS.md#the-opening-window));
the browser version asks on its `?setup` page, under *How to play*, once the game files are there:
the mode (Standard or Classic), HD graphics and their density, the rule changes and limits, and the
interface additions with the colour correction and the messages shown on the map, with the opening
window's own texts. They are kept as soon as they are changed, in the game's `CIV.INI` (`[OpenCiv2]`,
the same keys as natively) in the browser's storage, and take effect the next time the game starts.
Full screen and *Don't show this window again* are not there: the browser has its own full screen
(F11), and the game starts at once on later visits anyway.

## HD graphics

The browser version draws the map with the HD art ([HD-GRAPHICS.md](HD-GRAPHICS.md)) as the desktop
version does: tick *HD graphics* on the `?setup` page. The art (openciv2-assets, 234 MB, the release
pinned in `data/hd-pack.json`) is needed once:

- **Download** (or ticking *HD graphics*) fetches it from the site itself where it has it (below),
  else from GitHub. GitHub's release downloads do not allow a page of another site to fetch them
  (no `Access-Control-Allow-Origin` on the release's redirect, nor on the file it leads to), so
  from GitHub the page says so and links the `.zip`:
- **Choose the .zip or atlas-hd.pack**: the release's `.zip` once downloaded (or `atlas-hd.pack`
  alone, e.g. from `data/native/hd` after `make hd`, or a folder with either).

Either way the page checks it as `tools/fetch_hd_pack.py` does: the archive's SHA-256, then
`data/atlas-hd.pack`'s against the archive's `package.json` (the pack alone: against the pin's
`pack_sha256`), with its progress and a Stop button, and keeps the pack in the browser's storage
(IndexedDB, 250 MB; *Remove the art* takes it out), so that it works offline from then on. A site
that should offer the art itself: `make web HD=1` copies the installed art (`make hd`) to
`build/web/hd/atlas-hd.pack`, which the page downloads instead (250 MB more to serve; the art's
licence beside it).

`?hd=1` (or `?hd=N`, N the density 2 to 4) in the address turns them on for that visit, `?hd=0` off,
as `--hd` does natively. The density *Auto* is the device's own pixels: on a screen with a device
pixel ratio of 2 the map is drawn at twice the desktop's pixels and shown pixel for pixel.

What it takes: the game gets only the pictures it draws (65 MB of the pack's 250 MB, loaded from
the storage in about 0.3 s); the module's memory was 175 MB on a saved game's map at density 2 (87 MB
without); showing a 2560x1600 frame takes about 10 ms (Chrome, this machine).

## Saved games

Saved games, `CIV.INI`, `CIV2.DAT` and the Hall of Fame are kept in the browser's storage with the
game. A save is stored as soon as the game has written it, so closing the tab right after saving
keeps it.

- **Export saves** (on the `?setup` page, or under the small *Saves* button at the top right while
  playing) downloads your files as a `.zip`: the saved games, the scenarios and maps you made,
  `CIV.INI`, `CIV2.DAT` and `HALLFAME.DAT`. The game's own files are left out.
- **Import saves** puts `.SAV` files, or such a `.zip`, back. During a game they go straight into
  it too, so *Load a Game* finds them at once.

Use them to keep your saves safe, or to move them to another browser or computer, or to and from
the desktop version.

## Network games

A web page cannot open the game's TCP connections itself, so network games go through a small
relay: `tools/webrelay.py` (Python 3, nothing else to install) bridges the page's WebSockets to
TCP. Run it on the machine of the player who hosts, or on any machine the browsers can reach, and
give the page its address on the `?setup` page, or in the address as `?relay=ws://HOST:8765`:

```sh
tools/webrelay.py                                        # browsers on this machine; join any game on the LAN
tools/webrelay.py --host 0.0.0.0 --listen-host 0.0.0.0   # LAN party: other machines' browsers use it, and
                                                         # other machines join a game a browser hosts
```

- **Joining:** a browser joins a game hosted by the desktop OpenCiv2, the original game or another
  browser: *Multiplayer Game > Internet Game > Join*, then the host's address.
- **Hosting:** a game hosted in a browser is joined at the relay's address (the game shows it),
  port 4993.
- **Only Internet games, by address:** the local network's game lists are found by UDP broadcast,
  which does not reach a browser.

### The relay's security

By default the relay is not an open proxy:

- it listens on 127.0.0.1 (this machine) only;
- it connects only to port 4993 (`--ports` to change), and only to this machine and private
  network addresses (`--allow CIDR`, or `--allow any`, for games elsewhere on the Internet);
- it hosts games for browsers on 127.0.0.1 unless `--listen-host` says otherwise;
- it can be limited to the pages of one site (`--origin http://...`), and caps the number of
  connections (`--max`, 32 by default).

It speaks `ws://`: a page served over `https://` needs it behind a TLS proxy (`wss://`). All the
options: `tools/webrelay.py --help`.

## Limits

- The browser keeps a few shortcuts for itself (Ctrl+W, Ctrl+T and Ctrl+N close or open tabs and
  windows): use the game's menus for those commands.
- No LAN game lists (see above).
- The HD graphics' art cannot be downloaded from GitHub by the page itself (above): the player
  downloads the `.zip` and chooses it, unless the site serves the art (`make web HD=1`).
- HD graphics need memory: about 300 MB of the browser's storage with the game, and some 175 MB for
  the game's module on the map at density 2 (more at 3 or 4).
