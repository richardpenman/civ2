#!/bin/sh
# Run the native build on a fresh copy of the game files (inside the
# container).  Usage: sh tools/native-run.sh SCRIPT OUTDIR [gdb|valgrind]
# (valgrind needs the civ2-dbg image: see tools/docker-dbg)
# EXE=OpenCiv2.exe: the Windows build, run with wine (paths as Z:\...).
# TIMEOUT (seconds, default 600) ends a run that hangs: the game ignores
# SIGTERM (SDL makes it a quit event, which a replay does not read), so it is
# killed TIMEOUT_KILL seconds (default 10) after.
script=$1; out=$2
rm -rf /tmp/g && cp -r "${CIV2_GAME:-/civ2}" /tmp/g && chmod -R u+w /tmp/g
# CIV2_NO_DAT=1: the game's .DAT files left out (CIV2.DAT, HALLFAME.DAT: the
# game starts as on a fresh install), else the harness's CIV2.DAT (it skips
# the question of the heralds' memory)
if [ -n "$CIV2_NO_DAT" ]; then find /tmp/g -maxdepth 1 -iname '*.dat' -exec rm -f {} +; else cp /work/harness/CIV2.DAT /tmp/g/; fi
# CIV2_NO_GRAPHICS=1: the original's pictures left out (the sheets, *.GIF; the
# picture libraries, *.dll; civ2.exe, whose resources are cursors, icons and
# bitmaps): made from the HD art instead (src/platform/synth.c,
# docs/NO-INSTALL.md; the art in data/native/hd)
[ -z "$CIV2_NO_GRAPHICS" ] || find /tmp/g -maxdepth 1 \( -iname '*.gif' -o -iname '*.dll' -o -iname 'civ2.exe' \) -exec rm -f {} +
# CIV2_NO_SOUND=1: the game's Sound folder left out (it plays nothing)
[ -z "$CIV2_NO_SOUND" ] || find /tmp/g -maxdepth 1 -type d -iname sound -exec rm -rf {} +
# (the recordings were made without the CD's videos and music: a copy of the
# game that has them, KINGS, Video, Music, plays them, and its timing differs;
# CIV2_KEEP_MEDIA=1 keeps them)
[ -n "$CIV2_KEEP_MEDIA" ] || find /tmp/g -maxdepth 1 -type d \( -iname kings -o -iname video -o -iname music \) -exec rm -rf {} +
# CIV2_NO_RULES=1: the game's RULES.TXT left out (the built-in rules, src/include/rules.h)
[ -z "$CIV2_NO_RULES" ] || find /tmp/g -maxdepth 1 -iname rules.txt -exec rm -f {} +
# CIV2_NO_TEXT=1: the game's text files left out (OpenCiv2's own English, data/native/text)
[ -z "$CIV2_NO_TEXT" ] || for f in /work/data/native/text/*.TXT; do
    find /tmp/g -maxdepth 1 -iname "$(basename "$f")" -exec rm -f {} +
done
[ -f /work/harness/CIV.INI ] && cp /work/harness/CIV.INI /tmp/g/
mkdir -p "$out"
cd /tmp/g
exe=${EXE:-civ2}
# Text measured as Wine measures it (src/platform/text.c: wine_text), as in
# the original's recordings; CIV2_PLAY_TEXT=1 (the translations' tests, which
# have no recording of the original) measures it as in play, as drawn.
[ -n "$CIV2_PLAY_TEXT" ] || export CIV2_WINE_TEXT=1
case "$exe" in
*.exe)
    w() { printf 'Z:%s' "$1" | tr / '\\'; }
    export CIV2_DATA=$(w /work/data/native) CIV2_SCRIPT=$(w "$script") CIV2_SHOT_DIR=$(w "$out") \
           CIV2_SEED=${SEED:-7} CIV2_RAND_LOG=$(w "$out/rand.log") CIV2_HOOK_LOG=$(w "$out/hook.log") \
           CIV2_CALL_LOG=$(w "$out/calls.log") \
           CIV2_WINDIR=$(w /tmp/g) WINEDEBUG=-all WINEARCH=win64 WINEPREFIX=$HOME/wine64
    exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} /usr/lib/wine/wine64 /work/build/$exe ;;
esac
export CIV2_DATA=/work/data/native CIV2_SCRIPT=$script CIV2_SHOT_DIR=$out CIV2_SEED=${SEED:-7} \
       CIV2_RAND_LOG=$out/rand.log CIV2_HOOK_LOG=$out/hook.log CIV2_CALL_LOG=$out/calls.log \
       CIV2_WINDIR=/tmp/g
if [ "$3" = gdb ]; then
    exec gdb -batch ${GDBX:--ex run -ex bt} /work/build/$exe
fi
if [ "$3" = valgrind ]; then
    exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} valgrind --error-limit=no --num-callers=12 --track-origins=yes \
        --log-file="$out/valgrind.txt" /work/build/$exe
fi
exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} /work/build/$exe
