# The browser version

OpenCiv2 also runs in a web browser: the same game, compiled to WebAssembly, with the original
graphics and your own copy of Civilization II. How it is built and how it works inside:
[DEVELOPMENT.md](DEVELOPMENT.md) (the browser section).

## Building and serving it

`make web` builds a static site into `build/web`: a page, its scripts, the game as a 3.0 MB
WebAssembly module, the set-up as a second 0.8 MB module, and the fonts (`make web HD=1`: the HD
graphics' art too, below; `make web GAME=1`: the game itself, for a private site only, below). There is no server-side
code: any web server can serve the folder.

```sh
make serve        # builds build/web (first time: downloads Binaryen into tools/toolchain)
                  # and serves it: open http://localhost:8000 (PORT=8080 for another port)
```

It is tested in Chrome, and should run in any browser from 2023 on (Edge 103, Firefox 113,
Safari 16.4).

## Your copy of the game

The first time, the page asks for your copy of Civilization II Multiplayer Gold Edition: *Choose
your game...* offers *A folder* (the installed game, or the CD) or *A file* (a `.zip` of the folder,
or a disc image's `.cue` and `.bin`); any of them can be dropped on the page too. The page then says
what was chosen (e.g. *Folder: Civ2 (412 files)*, *Disc image: CIV2.cue + CIV2.bin*) and how the
copy is going. Any of these works:

- **The installed game:** the folder it is installed in, or a `.zip` of that folder. A folder the
  desktop OpenCiv2 set up (`GameData`) works too, music included.
- **The CD:** the folder it is mounted at (the one with `DATA1.CAB` and `CIV2`).
- **A disc image of the CD:** its `.cue` and `.bin`, chosen or dropped together.

From the CD or a disc image, the page sets the game up as the desktop version's set-up does: it
unpacks the game from the installer's cabinet and copies the videos. From a disc image it also
encodes the music tracks as Ogg Vorbis (`Music/TrackNN.ogg`); this takes a few minutes, with a
progress line and a Stop button. A mounted CD gives no music: browsers cannot read a CD's audio
tracks, but a disc image has them.

The game is copied into the browser's storage (IndexedDB), and never leaves your computer; your
saved games are kept there too. Later visits start the game at
once: the stored files, the fonts and the game's module (compiled as it downloads) are read side by
side, with a line over the screen saying what it is doing until the game's first frame (about 2.5 s
here with the HD art, 2 s without; `src/web/test/hd.mjs` fails a later visit's start over 15 s, and
the page's `window.civ2Timing` has each step's time). The page asks the browser to keep that storage for good, so that it is not cleared when
disk space runs low. `?setup` (`http://localhost:8000/?setup`) shows the set-up page again: to
replace the game files or remove them.

Music plays where the game's folder has it (`Music/TrackNN.ogg`).

## A site with the game on it (GAME=1)

`make web GAME=1` (with `HD=1` as well, usually) puts the installed game (`~/Civ2`, or
`CIV2_GAME`) on the site too, for people who have no copy of their own, such as the reviewers of a
translation: **a site built so distributes Civilization II's files, which are not ours to give
away: keep such a site private (behind a password, as below), for people who may play the game,
and never publish it.**

- The game but its videos, music and the advisors' videos is `game/core.zip` (55 MB). On a first
  visit (no game in the browser's storage) the page keeps it in the browser without asking, its
  progress on the screen, and starts the game: an address such as `.../?lang=de` opens straight
  into the game in German. `?setup` still offers to choose another copy.
- `Video/`, `KINGS/` and `Music/` are `game/files/` as they are (the music's `.wav` tracks
  encoded to Ogg Vorbis, 30 MB rather than 233; `tools/wav2ogg.c`), listed in `game/index.json`,
  and are not kept in the browser's storage: the page reads one from the site when the game opens
  it, whole (the game reads them whole), which pauses the game for that download (a council video
  is 30 MB, a wonder's 6 MB, a music track 2 to 5 MB); the browser's HTTP cache keeps them after.
- The site's copy has a `CIV.INI` (where the installed game has none) with HD graphics on (with
  `HD=1`) and the animated heralds of `KINGS/` on, without their first-start question.
- Without `GAME=1` nothing of the game is on the site (`build/web/game` is removed).

`make deploy-review` (`tools/deploy_review.sh`) builds such a site (`HD=1 GAME=1`) and serves it
on this machine with Caddy in Docker (container `civ2-review`, restarted with the machine; down:
`docker rm -f civ2-review`) at `/civ2/`, behind a user name and password (HTTP Basic Auth; the
root and anything else is a plain 404, with `X-Robots-Tag: noindex` and a `robots.txt` that
disallows all). By default over HTTPS at the machine's `sslip.io` name with a Let's Encrypt
certificate (ports 80 and 443 reachable from outside); `REVIEW_HTTP=1`: plain HTTP on port 80
(the password in clear text). Its files are outside the repository, in `~/civ2-review`: the site's
copy, the `Caddyfile` and `config` (the user name and password; `REVIEW_USER=` and
`REVIEW_PASSWORD=` set them). Other projects' routes on the same server go in
`~/web-routes/*.caddy`. The page works over plain HTTP too: where the browser has no
`crypto.subtle` (an `http://` page but localhost), the HD art's check computes SHA-256 itself.

## Language

The page is in English unless its address asks for a language: `?lang=CODE`, for the set-up
page and the game alike, e.g. `http://localhost:8000/?lang=de` (German), `?lang=la` (Latin), `?lang=el` (Greek), `?lang=eo` (Esperanto) or
`http://localhost:8000/?setup&lang=zh-TW`.  The code is matched loosely against the site's
translations (those of `data/native/lang`, listed in `lang/index.json` by `make web`): `de-AT`
finds `de`, a `zh-TW`, `zh-HK` or `zh-Hant` the Traditional Chinese, and one the site has no
translation of is English.  The browser's own language is not looked at.  The set-up page's
*Language* choice (English, or a translation) sets or removes `lang=` in the address and loads the
page again in it, so that a reload or a bookmark keeps it.  The page's texts are OpenCiv2's own,
keyed as the game's (`web.*` keys of `src/host/texts.h`, read by `src/web/i18n.js` from the same
translation files, in their code page: Esperanto's letters as Unicode, Greek and Chinese in the system's
fonts that have its characters); the set-up from a CD or disc image shows its progress in it too.

The game: with `?lang=`, its `CIV.INI` gets `[OpenCiv2] Language=CODE` before it starts (OpenCiv2
Options > Language shows it), for that visit only: what this browser keeps of `CIV.INI` keeps the
language the player had chosen before, unless the game itself changes it.  Without `?lang=`,
`CIV.INI` is as the game left it: a language chosen in the game's OpenCiv2 Options applies, and
*Automatic* is English (`CIV2_SYSTEM_LANG=en`; [TRANSLATING.md](TRANSLATING.md)).  The Options'
*Restart now* puts the game's language in the address (`lang=`, none for Automatic or English),
so that a language changed in the game is the one the page is loaded again in.

## How to play: OpenCiv2 Options

How to play is chosen in the game, as natively: *OpenCiv2 Options*, the main menu's last choice
([EXTRAS.md](EXTRAS.md#openciv2-options)): the mode (Standard or Classic), sharp text and
interface, HD graphics, the rule changes and limits, and the interface additions with the colour
correction and the messages shown on the map.  They are kept in the game's `CIV.INI`
(`[OpenCiv2]`, the same keys as natively) in the browser's storage.  Those that take effect when
the game starts again (the mode, the graphics) offer *Restart now*: the page is loaded again,
into the game (without `?setup`, `?hd` or `?sharp`, which would win over them; `?lang=` as the
game's language now).  Full screen is
not there: the browser has its own (F11).  The set-up page itself only has the game files: choose
them, *Remove them* (with the HD art), *Play*, and the saved games' *Export* and *Import*.

## HD graphics

The browser version draws the map with the HD art ([HD-GRAPHICS.md](HD-GRAPHICS.md)) as the desktop
version does, where the site has the art: `make web HD=1` copies the installed art (`make hd`,
openciv2-assets, the release pinned in `data/hd-pack.json`) to `build/web/hd/atlas-hd.pack` (250 MB
more to serve; the art's licence beside it), with `hd/pack.json` saying so. On such a site, tick
*HD graphics* in OpenCiv2 Options (Graphics and screen) and restart: as the page is loaded again
it downloads the art from the site, with its progress on the screen, checks it against the pin's
`pack_sha256` and keeps it in the browser's storage (IndexedDB, 250 MB; unticking *HD graphics*
keeps it, *Remove them* on the `?setup` page takes it out with the game files), so that it works
offline from then on. A site built without it (`make web`) does not offer HD graphics: the tick is
greyed, with *This site was built without the art*; art already kept in the browser still works
there (the page tells the game which: kept, on the site, or neither).

`?hd=1` (or `?hd=N`, N the density 2 to 4) in the address turns them on for that visit, `?hd=0` off,
as `--hd` does natively. The page has no density choice: it is *Auto* (unless `CIV.INI` has
`GraphicsDensity=` under `[OpenCiv2]`, or `?hd=N` asks), the device's own pixels: on a screen with a device
pixel ratio of 2 the map is drawn at twice the desktop's pixels and shown pixel for pixel.

*Sharp text and interface* ([EXTRAS.md](EXTRAS.md#sharp-text-and-interface); on by default in
Standard mode, `SharpText=` in `CIV.INI`) works the same way without the art: on a screen with a
device pixel ratio of 2 or more the canvas is the device's pixels, its text, menus and controls
drawn at that resolution, the pictures enlarged.  `?sharp=0` (or `?sharp=1`) in the address
switches it for that visit, as `--sharp=0` natively.

What it takes: the game gets only the pictures it draws (65 MB of the pack's 250 MB), kept beside
the pack as their own copy when the art is kept, so that a start reads that copy alone (its 16 chunks
at once, under a second) and never the whole pack; the module's memory was 175 MB on a saved game's map at density 2 (87 MB
without, 179 MB with sharp text alone); showing a 2560x1600 frame takes about 10 ms (Chrome, this
machine).

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
TCP. Run it on the machine that serves the site: the page uses port 8765 of the site's own host
(`ws://HOST:8765`, `wss://` on an `https://` page). A relay elsewhere: `?relay=ws://HOST:PORT` in the
address. When a network game cannot reach the relay, the page says so (run `tools/webrelay.py`
there) and asks for its address, kept in this browser for the next connection:

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
- HD graphics only where the site has the art (`make web HD=1`): GitHub's release downloads do not
  allow a page of another site to fetch them.
- HD graphics need memory: about 300 MB of the browser's storage with the game, and some 175 MB for
  the game's module on the map at density 2 (more at 3 or 4).
