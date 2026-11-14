# OpenCiv2

Goal: reconstruct the source of *Sid Meier's Civilization II: Multiplayer Gold Edition*
(1998) as portable C++ that builds and runs natively on Linux, macOS and Windows.

## Download and play

You need your own copy of Civilization II Multiplayer Gold Edition: installed, on CD, or a
disc image (`.cue`/`.bin`). Download the program for your system:

- **Windows:** [OpenCiv2-windows.zip](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-windows.zip): unzip and run `OpenCiv2.exe`.
- **macOS** 11+: [OpenCiv2-macos.zip](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-macos.zip): unzip and open `OpenCiv2.app`. The first time macOS
  blocks it: System Settings > Privacy & Security > "Open Anyway".
- **Linux** x86-64: [OpenCiv2-linux-x86_64.tar.gz](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-linux-x86_64.tar.gz): unpack and run `OpenCiv2/OpenCiv2`.

The first time, it finds your copy of the game (or asks where it is) and sets it up, music
included when it comes from the CD or a disc image. `--setup [FOLDER or IMAGE.cue]` sets it
up again. From a disc image, each music track ends at the silence before the next one where
the cue sheet's track marks are a second or two off (`--no-fix-cue-gaps` with `--setup` cuts
them exactly where the cue sheet says).

OpenCiv2 fixes the original game's bugs ([BUGS.md](docs/BUGS.md)) and adds the interface
improvements of FoxAhead's [CIV2UIA](https://github.com/FoxAhead/Civ2-UI-Additions) (mouse
wheel, scrolling lists, more figures in the advisors, quick information with Ctrl ...:
[EXTRAS.md](docs/EXTRAS.md); each can be switched off in `CIV.INI`), and, off unless switched
on in `CIV.INI`, its rule changes (Go To past enemy units, moving a stack's units of a type
together, higher limits for gold, population and the map ...). Set `CIV2_CLASSIC=1` to play
the original game instead: its bugs, its interface, and its menu bar and window caption drawn as
the original's.

The game opens in a window filling the screen. To play full screen, see [EXTRAS.md](docs/EXTRAS.md#screen-scale).

On Linux, the window uses the system's title bar with the game's menu bar inside it.
`CIV2_CLASSIC=1` also draws the original title bar and frame inside that window.

### Multiplayer

All of the original's multiplayer modes are supported: hot seat on one computer, and network
games on a local network or over the Internet (Multiplayer Game > Network or Internet). Players
can mix OpenCiv2 on any system with the original game, and either one can host. Network games
use TCP port 4993, and UDP port 4994 to find games on a local network.

## Build and run

To build it yourself, install your copy of Civilization II in `~/Civ2` (the directory with
`civ2.exe`, `Tiles.dll`, the `.TXT` files …), or point `CIV2_GAME` at it.

```sh
git clone https://github.com/richardpenman/civ2 && cd civ2
make            # -> build/OpenCiv2 (several minutes; the first time it downloads wasi-sdk,
                #    wabt and the libraries' sources into tools/toolchain)
make run        # plays the game in ~/Civ2 (or: make run GAME=/path/to/Civ2)
```

`make` (or `make build`) builds the native executable for the local machine without
Docker. `make native` is the separate 32-bit Linux development build; it needs
working Docker and the development images built with `make images`. See
[DEVELOPMENT.md](docs/DEVELOPMENT.md#tooling) for those development targets.

What to install first:

- **macOS:** `xcode-select --install` (compiler and make) and `brew install sdl2`.
- **Linux:** a C compiler, make, Python 3 and `libsdl2-dev` from the package manager.
- **Windows:** in an [MSYS2](https://www.msys2.org) **MINGW64** shell, `pacman -S git make
  mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-python`, then `make` and
  `make run` there as above (the game is looked for in `%USERPROFILE%\Civ2`). Outside MSYS2,
  play with `tools\run-civ2.bat [GAMEDIR]` from a Command Prompt or by double-clicking it.

`make windows` cross-compiles `build/OpenCiv2.exe` on Linux (it also downloads SDL2's MinGW
release): the one file the release has, SDL2 and the fonts built in.

## Browser version

`make web` builds the game for web browsers into `build/web`: a static site (a page, its
scripts, the game as a 2.9 MB WebAssembly module, the fonts) with no server-side code. Serve
that folder from any web server, e.g.

```sh
make web                                   # first time: downloads Binaryen into tools/toolchain
python3 -m http.server -d build/web 8000   # then open http://localhost:8000
```

It plays single-player games with the original graphics and your own copy of the game: the first
time, the page asks for the folder Civilization II is installed in (or a `.zip` of it; both can be
dropped on the page), or the game's CD (the folder it is mounted at, with `DATA1.CAB` and `CIV2`),
or a disc image of the CD (its `.cue` and `.bin`, chosen or dropped together), and copies the game
into the browser's storage (IndexedDB): from the CD or a disc image it is set up as the desktop
version's set-up does, unpacked from the installer's cabinet with its videos, and from a disc image
its music encoded as Ogg Vorbis (`Music/TrackNN.ogg`; a few minutes; the browser cannot read a
CD's music tracks, a disc image has them); later visits start the
game at once (`?setup` shows the page again, to replace or remove the files). Saved games,
`CIV.INI` and the Hall of Fame are kept there too: **Export saves** (on the `?setup` page, or
under the small *Saves* button at the top right while playing) downloads them as a `.zip` (saved
games, the scenarios and maps you made, `CIV.INI`, `CIV2.DAT`, `HALLFAME.DAT`; not the game's own
files), and **Import saves** puts `.SAV` files, or such a `.zip`, back (into the running game too,
so that *Load a Game* finds them at once): to keep them safe, or move them to another browser or
computer. Music plays where the game's folder has it (`Music/TrackNN.ogg`, as
the set-up makes from a disc image; a folder the desktop version set up, `GameData`, has it too). Network games are not available in the browser.
Tested in Chrome; it should run in any browser from 2023 on (Edge 103, Firefox 113,
Safari 16.4). The browser keeps a few shortcuts for itself (Ctrl+W, Ctrl+T, Ctrl+N close or
open tabs and windows): use the game's menus for those commands.

## Status

Playable. Every function of the game that can run has source; the only parts not reconstructed
are the original's MSVC runtime (replaced by the platform's C library) and unreachable 16-bit
leftovers. All 44 recorded scenarios match the original exactly, pixel for pixel, with the same
sounds and videos asked for. They run 72% of the game's functions (78% leaving out networking);
playing sound and video and the High Council's window can't be compared under Wine. Multiplayer is supported and compatible with the original: OpenCiv2 and
the original game play each other on a LAN or over the Internet, either one hosting. Automated
tests (`tools/xdnet/nettest.py`) play two or three games against each other, including
diplomacy, saving and loading, players leaving and rejoining, and 50-turn games
(`docs/xdaemon-protocol.md`).

The code is being modernized step by step, each step checked by the harness: it compiles as
strict C++, keeps the original's data layout, has its classes as C++ class hierarchies with
member functions, typed callbacks, structured control flow, smaller functions and named
constants (enums for unit types, improvements, advances, orders, network messages ..., flag
constants for units, cities, treaties, tiles) and its state grouped by subject (records such as
`prefs.difficulty` or `net.session_phase` where the original's variables lie together, a header
per subject, the read-only texts and tables `const`), and releases bound to scope (a local
dialog, surface or file released by a guard on every path out of the function instead of on each
return). Its logic has recorded unit tests (`make test`: 11,119 calls of 880 of the game's
functions recorded from the replays, run again and compared in seconds, no game files needed).
That completes step 6 of the clean-up.

Details: [STATUS.md](docs/STATUS.md) (progress and verification) and
[DEVELOPMENT.md](docs/DEVELOPMENT.md) (the code's layout, the approach and the tooling).

| | |
| --- | --- |
| ![Main menu](docs/screenshots/menu.png) | ![A city](docs/screenshots/city.png) |
| ![The World War II scenario](docs/screenshots/europe-1940.png) | ![The Top Five Cities](docs/screenshots/top5.png) |
| ![A foreign leader receiving an envoy](docs/screenshots/diplomacy.png) | ![London's city view](docs/screenshots/city-view.png) |

*The native build: the main menu, a city, the World War II scenario, the Top Five Cities, a foreign
leader receiving an envoy and London's city view with its wonders.*
