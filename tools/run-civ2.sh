#!/bin/sh
# Play the portable build: tools/run-civ2.sh [GAMEDIR]
# GAMEDIR: the installed game (default $CIV2_GAME, or ~/Civ2); the game
# saves there.  macOS: the screen below the menu bar, the menus in it;
# CIV2_FULLSCREEN=1: full screen; CIV2_WINDOW=1: a window.  CIV2_CLASSIC=1: the
# original game: menus and caption drawn as the original's, and the original's
# bugs, which OpenCiv2 otherwise fixes (README, docs/BUGS.md).
here=$(cd "$(dirname "$0")/.." && pwd)
game=${1:-${CIV2_GAME:-$HOME/Civ2}}
[ -d "$game" ] || { echo "no game in $game: install Civilization II there or name its directory" >&2; exit 1; }
[ -f "$game/CIV2.DAT" ] || cp "$here/harness/CIV2.DAT" "$game/"
CIV2_DATA="$here/build/data" exec "$here/build/OpenCiv2" "$(cd "$game" && pwd)"
