# OpenCiv2

Goal: reconstruct the source of *Sid Meier's Civilization II: Multiplayer Gold Edition*
(1998) as portable C++ that builds and runs natively on Linux, macOS and Windows,
and prove it behaves exactly like the original binary: the same screens, pixel for pixel,
and the same random numbers, call for call, on recorded games.

## Download and play

You need your own copy of Civilization II Multiplayer Gold Edition: installed, on CD, or a
disc image (`.cue`/`.bin`). Download the program for your system:

- **Windows:** [OpenCiv2-windows.zip](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-windows.zip): unzip and run `OpenCiv2.exe`.
- **macOS** 11+: [OpenCiv2-macos.zip](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-macos.zip): unzip and open `OpenCiv2.app`. The first time macOS
  blocks it: System Settings > Privacy & Security > "Open Anyway".
- **Linux** x86-64: [OpenCiv2-linux-x86_64.tar.gz](https://github.com/richardpenman/civ2/releases/download/latest/OpenCiv2-linux-x86_64.tar.gz): unpack and run `OpenCiv2/OpenCiv2`.

The first time, it finds your copy of the game (or asks where it is) and sets it up, music
included when it comes from the CD or a disc image. `--setup [FOLDER or IMAGE.cue]` sets it
up again.

OpenCiv2 fixes the original game's bugs ([BUGS.md](docs/BUGS.md)) and adds the interface
improvements of FoxAhead's [CIV2UIA](https://github.com/FoxAhead/Civ2-UI-Additions) (mouse
wheel, scrolling lists, more figures in the advisors, quick information with Ctrl ...:
[EXTRAS.md](docs/EXTRAS.md); each can be switched off in `CIV.INI`), and, off unless switched
on in `CIV.INI`, its rule changes (Go To past enemy units, moving a stack's units of a type
together, higher limits for gold, population and the map ...). Set `CIV2_CLASSIC=1` to play
the original game instead: its bugs, its interface, and its menu bar and window caption drawn as
the original's.

The game opens in a window filling the screen; on a high-resolution screen it is drawn at
2×, 3× or 4×, sharp, at about the original's 1024×768 (a MacBook Air: 980×594, a 4K screen:
1280×720). To play full screen, see [EXTRAS.md](docs/EXTRAS.md#screen-scale).

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

What to install first:

- **macOS:** `xcode-select --install` (compiler and make) and `brew install sdl2`.
- **Linux:** a C compiler, make, Python 3 and `libsdl2-dev` from the package manager.
- **Windows:** in an [MSYS2](https://www.msys2.org) **MINGW64** shell, `pacman -S git make
  mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-python`, then `make` and
  `make run` there as above (the game is looked for in `%USERPROFILE%\Civ2`). Outside MSYS2,
  play with `tools\run-civ2.bat [GAMEDIR]` from a Command Prompt or by double-clicking it.

`make windows` cross-compiles `build/OpenCiv2.exe` on Linux (it also downloads SDL2's MinGW
release): the one file the release has, SDL2 and the fonts built in.

## Status

Playable. Every function of the game that can run has source; the only parts not reconstructed
are the original's MSVC runtime (replaced by the platform's C library) and unreachable 16-bit
leftovers. All 41 recorded scenarios match the original exactly, pixel for pixel, with the same
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
return). Its logic has recorded unit tests (`make test`: 10,481 calls of 836 of the game's
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
