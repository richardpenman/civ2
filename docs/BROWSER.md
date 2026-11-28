# The browser version

OpenCiv2 also runs in a web browser: the same game, compiled to WebAssembly. As the desktop game
does without the original's files ([NO-INSTALL.md](NO-INSTALL.md)), it starts at once with HD
graphics and OpenCiv2's own texts, rules, music, sounds, heralds and wonder movie; your own copy of
Civilization II, added when you like, gives the original's look (Classic graphics). How it is built and how it works inside:
[DEVELOPMENT.md](DEVELOPMENT.md) (the browser section).

## Building and serving it

`make web` builds a static site into `build/web`: a page, its scripts, the game as a 3.0 MB
WebAssembly module, the set-up as a second 0.8 MB module, the fonts and the HD graphics' art
(below; `make web GAME=1`: the game itself, for a private site only, below). There is no server-side
code: any web server can serve the folder.

```sh
make serve        # builds build/web (first time: downloads Binaryen into tools/toolchain)
                  # and serves it: open http://localhost:8000 (PORT=8080 for another port)
```

It is tested in Chrome, and should run in any browser from 2023 on (Edge 103, Firefox 113,
Safari 16.4). `make web-test` tests the page and a game in it; `make web-parity` replays a set of
the desktop's test scripts in it and compares every screenshot and the game's random numbers with
the desktop's, so that what goes wrong only in the browser shows ([DEVELOPMENT.md](DEVELOPMENT.md),
the browser section).

Every push to main publishes it at <https://openciv2.github.io/openciv2/> once CI has passed for
it (`.github/workflows/pages.yml`, after `ci.yml`): built as `make web` builds it, with the pinned HD art and
OpenCiv2's own music, sound effects, heralds and wonder movie made as the release makes them, never
with the game (no `GAME=1`), and checked before it is published to hold none of the original's
files. Its URLs are all relative, so it plays under that sub-path as at a site's root. There is no
relay there: network games need `?relay=` pointing at one of your own (Network games, below).

## The start: no copy of the game needed

A visit opens on the start screen: the game's main window backdrop (`hd/title.webp`, the HD art's
picture the main menu shows) with a small dialog over it: *Click to play* (Enter or Space too),
*Export saves* and *Import saves* (below: [Saved games](#saved-games)), and, once the browser keeps
anything, *Remove from this browser* (the game files, a copy of the original among them, the saved
games and settings: the next visit starts again without a copy). Browsers let a page make no
sound until it is clicked or typed at; the start screen's click starts the page's audio
(`main.js`: `startAudio`, its context handed to `host.js`), so the opening menu's music is heard
from the start. *Import saves* goes straight into the game too (its click starts the sound). The
start screen is all of the page that is not the game: an old link's `?setup` opens it too, and
the game's Settings' *Restart now* reloads the page straight into the game, without it.

After the click, the first time (nothing in the
browser's storage) the game starts from an empty game folder, which OpenCiv2's own files fill in
exactly as they do natively for a folder that lacks them: HD graphics (always, without the
original's pictures), OpenCiv2's English of the game's texts, the built-in rules, its own sound
effects, music, heralds and wonder movie. The page shows *Starting the game ...* with the
install's progress bar while it fetches the HD art (`hd/art.bin`, 40 MB: by its bytes) and
OpenCiv2's texts, sound effects and fonts (by their files), then the game's regular main menu
(some 5 s here from a local server). The game's folder is noted as one without the original's
files (the store's `meta` `game` with no names: `src/web/main.js`, `play`); what the game writes
(`CIV.INI`, `CIV2.DAT`, saved games) is kept in the browser's storage, the pictures and sounds it
makes from the art again at each start (`OpenCiv2Pictures/`, `OpenCiv2Sounds/`) are not. Later
visits start at once from what was kept, the art from the browser's cache, without the bar.

While the start screen is shown, the page already fetches and reads what a start waits for
(`main.js`: `preload`): the game's module and fonts, the files kept in the browser, the HD art
(where the game will draw it), OpenCiv2's texts, sound effects, font files and translations, then
its music and wonder movies. A thin bar under *Click to play* says how far (*Loading ... 45%*), and
goes once all has come; the click then starts the game at once, its first frame some 1.6-2.7 s
later here (the game's own start), where before it waited for the downloads too (some 20 s for a
first visit at 50 Mbit/s). A click before then shows the progress bar or line, carrying on from
what has come. Nothing is played or written before the click; saves imported meanwhile are read
again at the start (`src/web/test/start.mjs` checks the line, and a later visit's first frame
within 6 s of the key, the art fetched before it).

Your copy of the original is chosen in the game: Settings, Classic graphics ticked, *Find the
original files...* (below).

## Phones and tablets

On a touch screen whose page is under 800x600 CSS pixels (a phone either way up, a tablet held
upright), the game's desktop is sized by the page's CSS pixels rather than the device's, at least
640x480, the page's shape kept (`host.js`: `smallDesktop`): a phone of 390x844 gets a 640x1385
desktop upright, drawn at density 2 and filling the screen, where before it was 1170x2532, the
interface a little over half the size. Sideways it gets 999x480 (the page's sides black: from
1000 wide the game's city window is half as large again, as the original's, too tall for 480
rows), where before it was 1266x585, the city window's lower buttons (*Exit* among them) then
below the screen. Settings' Scale, when set,
is kept as before; computers and larger tablets are as before. Sideways is better: the game's
windows were made for a landscape screen. *Click to play* also asks for full screen, where the
browser can (not Safari on an iPhone), and then turns a phone sideways where the browser lets it
(Chrome on Android); the system's Back or the browser's own gesture leaves it.

A tap is the left button's click (pressed as the finger lifts, or once it moves: a drag); a press
held half a second without moving is the right button's click (with a short buzz where the phone
can). The page around the game neither zooms, scrolls nor pulls to reload under a finger.
Limits: the game's interface is still that of a 640x480 game on a 6-inch screen: its text and
buttons are small (some 0.6-0.8 of a CSS pixel each); there is no on-screen keyboard for the
game's keyboard shortcuts or for naming cities (the names offered are kept with OK), and no pinch
to zoom the map (its View menu's zoom works).

## Your copy of the game

For Classic graphics, the original's look, the game needs your copy of Civilization II
Multiplayer Gold Edition. In the game's Settings, Classic graphics ticked, *Find the original
files...* (below) shows the page's panel for it over the game: *Choose your game...* offers *A folder* (the installed game, or the CD) or *A file* (a `.zip` of the folder,
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
saved games are kept there too. The copy is added to the game's files (those it lacks, any
case), the saved games and `CIV.INI` kept. Later visits start the game at
once from the start screen's click: the stored files, the fonts and the game's module (compiled as it downloads) are read side by
side, with a line over the screen saying what it is doing until the game's first frame (about 2.5 s
here with the HD art, 2 s without; `src/web/test/hd.mjs` fails a later visit's start over 15 s, and
the page's `window.civ2Timing` has each step's time). The page asks the browser to keep that storage for good, so that it is not cleared when
disk space runs low. The start screen's *Remove from this browser* removes the game files; the
next visit starts again without a copy.

Music: with HD graphics OpenCiv2's own (`data/music/trackNN.ogg` on the site, 26 MB, fetched
by `make own-assets`: [NO-INSTALL.md](NO-INSTALL.md#music)), the game folder's
(`Music/TrackNN.ogg`) only for a track OpenCiv2 lacks; with Classic graphics the game folder's,
else OpenCiv2's.  The page fetches OpenCiv2's tracks (with HD graphics all of them, with Classic
graphics those the game lacks) while the game starts, without waiting for them, and gives them to
the game together once all have come (the browser keeps them in its cache for later visits);
until then, on a first visit, the game plays its own music, or none.  Sound effects likewise:
with HD graphics OpenCiv2's own (`data/sound/name.ogg` on the site, 2 MB, fetched by `make
own-assets`: [NO-INSTALL.md](NO-INSTALL.md#sounds)), the game folder's (`Sound/NAME.WAV`) only
for one OpenCiv2 lacks; with Classic graphics the game folder's, else OpenCiv2's; the page
fetches them (all with HD graphics, with Classic graphics those the game lacks) before the game
starts.  Wonder movies likewise: with HD graphics OpenCiv2's own (the Pyramids' so far,
`data/wonders/wonderNN.webp` and `.ogg` on the site, 3 MB a movie, fetched by `make own-assets`:
[NO-INSTALL.md](NO-INSTALL.md#wonder-movies)) in the place of the game's; with Classic graphics
the game folder's (`Video/WONDERNN.AVI`), else OpenCiv2's (the site's own copy of the game, `make
web GAME=1`, has no `Video`); the page fetches them (all with HD graphics, with Classic graphics
those the game lacks) while the game starts, without waiting for them.  OpenCiv2's own heralds
(`data/native/heralds`) are in `hd/art.bin` with the HD art. The start screen's click starts the
sound; where a browser still holds it back (the reload of Settings' *Restart now*, without a
click) the sound effects are silent until the page is clicked or typed at, but still take their
time (`host.js`, `host_audio_queue`), so a looping one, the main menu's theme, does not hold the
game up.

## A site with the game on it (GAME=1)

`make web GAME=1` puts the installed game (`~/Civ2`, or
`CIV2_GAME`) on the site too, for people who have no copy of their own, such as the reviewers of a
translation: **a site built so distributes Civilization II's files, which are not ours to give
away: keep such a site private (behind a password, as below), for people who may play the game,
and never publish it.**

- The game but its music and animated advisors is `game/core.zip` (55 MB); `Music/` and
  `KINGS/` (the animated advisors and heralds) are `game/files/` as they are (the music's `.wav`
  tracks encoded to Ogg Vorbis, 20 MB rather than 233; `tools/wav2ogg.c`), listed in
  `game/index.json`. On a first visit (no game in the browser's storage) the page downloads all
  of it, 109 MB (at 20 Mbit/s about 45 s, the game's main menu after about a minute), and keeps
  it in the browser without asking, on one
  progress bar (the download in MB, then the files unpacked and kept), then shows the start
  screen: an address such as `.../?lang=de` opens it and then the game in German. Nothing is read from the site while the
  game plays, so the game never waits for it. A browser that kept the game before the site had
  those files (or kept a copy of its own without them) downloads them, on the same bar, on its
  next visit.
- The CD's videos (`Video/`: the opening, the wonders', the High Council's; 285 MB) are not on
  the site: the game plays as from a folder without them, at once, as the original does. It
  starts at its main menu, a wonder's movie is skipped, and the High Council does not open.
- The site's copy has a `CIV.INI` (where the installed game has none) with the animated heralds of
  `KINGS/` on, without their first-start question.
- Without `GAME=1` nothing of the game is on the site (`build/web/game` is removed).

`make deploy-review` (`tools/deploy_review.sh`) builds such a site (`GAME=1`), with the HD art (made
by `make hd-art`, from its cache once made), and serves it
on this machine with Caddy in Docker (container `civ2-review`, restarted with the machine; down:
`docker rm -f civ2-review`) at `/civ2/`, behind a user name and password (HTTP Basic Auth; the
root and anything else is a plain 404, with `X-Robots-Tag: noindex` and a `robots.txt` that
disallows all). By default over HTTPS at the machine's `sslip.io` name with a Let's Encrypt
certificate (ports 80 and 443 reachable from outside); `REVIEW_HTTP=1`: plain HTTP on port 80
(the password in clear text). Its files are outside the repository, in `~/civ2-review`: the site's
copy, the `Caddyfile` and `config` (the user name and password; `REVIEW_USER=` and
`REVIEW_PASSWORD=` set them). The site also has the local staging folder `data/native/hd-extra`
(OpenCiv2's own pictures and heralds being tried, not in the repository:
[HD-GRAPHICS.md](HD-GRAPHICS.md#openciv2s-own-pictures-and-heralds-datanativehd-extra)) where the
checkout built has it: the deploy after each commit on `main` (a local hook, not in the repository,
running `~/civ2-review/autodeploy.sh`, which builds in the worktree `~/civ2-review/src`) copies
the main checkout's folder into the worktree first. Other projects' routes on the same server go in
`~/web-routes/*.caddy`. The page works over plain HTTP too: where the browser has no
`crypto.subtle` (an `http://` page but localhost), the HD art's check computes SHA-256 itself.

## Language

The page is in the game's language: its address's `?lang=CODE`, for the start screen and the
game alike, e.g. `http://localhost:8000/?lang=de` (German), `?lang=la` (Latin), `?lang=el` (Greek), `?lang=eo` (Esperanto) or
`?lang=zh-TW`; without it, the language chosen in the game's Settings (`CIV.INI`'s
`Language=`, as this browser keeps it), else English.  The code is matched loosely against the site's
translations (those of `data/native/lang`, listed in `lang/index.json` by `make web`): `de-AT`
finds `de`, a `zh-TW`, `zh-HK` or `zh-Hant` the Traditional Chinese, and one the site has no
translation of is English.  The browser's own language is not looked at; the page has no
language choice of its own (the game's Settings > Language is it).  The page's texts are OpenCiv2's own,
keyed as the game's (`web.*` keys of `src/host/texts.h`, read by `src/web/i18n.js` from the same
translation files, in their code page: Esperanto's letters as Unicode, Greek and Chinese in the system's
fonts that have its characters, else in the game's own Noto Serif files from the site, loaded
before the texts are put in); the panel choosing the original's files, and the set-up from a CD
or disc image, show their progress in it too.

The game: with `?lang=`, its `CIV.INI` gets `[OpenCiv2] Language=CODE` before it starts
(Settings > Language shows it), for that visit only: what this browser keeps of `CIV.INI` keeps the
language the player had chosen before, unless the game itself changes it.  Without `?lang=`,
`CIV.INI` is as the game left it: a language chosen in the game's Settings applies, and
*Automatic* is English (`CIV2_SYSTEM_LANG=en`; [TRANSLATING.md](TRANSLATING.md)).  The Options'
*Restart now* puts the game's language in the address (`lang=`, none for Automatic or English),
so that a language changed in the game is the one the page is loaded again in.

## How to play: Settings

How to play is chosen in the game, as natively: *Settings*, the main menu's last choice
([EXTRAS.md](EXTRAS.md#settings)): the bug fixes, Classic graphics (unticked, the
default: HD graphics, the art fetched from the site; ticked: the original's pictures), sharp text and interface, the rule changes and limits, and the interface additions
with the colour correction and the messages shown on the map.  They are kept in the game's
`CIV.INI` (`[OpenCiv2]`, the same keys as natively) in the browser's storage.  Those that take
effect when the game starts again (the graphics, the language) offer *Restart now*: the page is loaded again,
straight into the game, not stopping at the start screen (without `?hd` or `?sharp`, which would
win over them; `?lang=` as the game's language now).  Full screen is
not there: the browser has its own (F11).

Where the game's files lack the original's pictures (the start without a copy, or a game set up from files without them),
*Classic graphics* ticked opens the dialog that offers to find the original's files
([EXTRAS.md](EXTRAS.md#classic-graphics-without-the-original-files)); its *Find the original
files...* (the game's `host_add_original`, `src/web/main.js`: `addOriginal`) shows the page's
panel for it over the game, with a note and *Back to the game*: the same *Choose your game...* (a folder,
a `.zip` of it, the CD or a disc image, or dropped there) adds the files the game lacks (any
case; a CD or disc image set up into memory first, as at a first visit) to its files and to this
browser's storage, its own files (saved games, `CIV.INI`) kept.  When they then have the
original's pictures the game comes back with *Restart now* offered (Classic graphics from the
reload); otherwise the page says the files lack them, and *Back to the game* returns with HD
graphics kept.

## HD graphics

The browser version draws the map with the HD art ([HD-GRAPHICS.md](HD-GRAPHICS.md)) as the desktop
version does: `make web` makes the art first (`make hd-art`: `data/native/hd`, not in the
repository) and puts it (and, where present, the local staging folder `data/native/hd-extra`) on the site as one
file, `hd/art.bin` (about 50 MB, its files listed in
`hd/index.json`; the art's licence beside it). HD graphics are the default: unless *Classic
graphics* is ticked in Settings (Graphics and screen), as the game starts the page fetches that file (the browser's cache keeps
it) and puts the pictures into the game's files, where the game reads them. Art an earlier
version downloaded and kept in the browser's storage (IndexedDB, 250 MB) is removed at a start.

`?hd=1` (or `?hd=N`, N the density 2 to 4) in the address turns them on for that visit, `?hd=0` off,
as `--hd` does natively. The page has no density choice: it is *Auto* (unless `CIV.INI` has
`GraphicsDensity=` under `[OpenCiv2]`, or `?hd=N` asks), the device's own pixels: on a screen with a device
pixel ratio of 2 the map is drawn at twice the desktop's pixels and shown pixel for pixel.

*Sharp text and interface* ([EXTRAS.md](EXTRAS.md#sharp-text-and-interface); on by default,
`SharpText=` in `CIV.INI`) works the same way without the art: on a screen with a
device pixel ratio of 2 or more the canvas is the device's pixels, its text, menus and controls
drawn at that resolution, the pictures enlarged.  `?sharp=0` (or `?sharp=1`) in the address
switches it for that visit, as `--sharp=0` natively.

What it takes: 25 MB from the site (the pictures the game uses, WebP); the module's memory was 175 MB on a saved game's map at density 2 (87 MB
without, 179 MB with sharp text alone); showing a whole 2560x1600 frame takes about 10 ms (Chrome, this
machine), so only the frame's rows that changed since the last one are put on the canvas (`hd_frame_changed`,
`host_present_hd`'s `top` and `bottom`): a unit's slide, 17 frames 15 ms apart in the original, changes a band of
some 100 rows of each, and moves at the original's speed rather than half of it (`src/web/test/perf.mjs`).

## Saved games

Saved games, `CIV.INI`, `CIV2.DAT` and the Hall of Fame are kept in the browser's storage with the
game. A save is stored as soon as the game has written it, so closing the tab right after saving
keeps it.

Both are on the start screen (reload the page to reach it from a game: what the game wrote is
kept first).

- **Export saves** downloads your files as a `.zip`: the saved games, the scenarios and maps you made,
  `CIV.INI`, `CIV2.DAT` and `HALLFAME.DAT`. The game's own files are left out.
- **Import saves** puts `.SAV` files, or such a `.zip`, back, and starts the game, whose *Load a
  Game* finds them.

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
- HD graphics need memory: some 175 MB for the game's module on the map at density 2 (more at 3
  or 4).
