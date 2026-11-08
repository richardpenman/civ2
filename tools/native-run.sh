#!/bin/sh
# Run the native build on a fresh copy of the game files (inside the
# container).  Usage: sh tools/native-run.sh SCRIPT OUTDIR [gdb|valgrind]
# (valgrind needs the civ2-dbg image: see tools/docker-dbg)
# EXE=OpenCiv2.exe: the Windows build, run with wine (paths as Z:\...).
# TIMEOUT (seconds, default 600) ends a run that hangs: the game ignores
# SIGTERM (SDL makes it a quit event, which a replay does not read), so it is
# killed TIMEOUT_KILL seconds (default 10) after.
script=$1; out=$2
rm -rf /tmp/g && cp -r "${CIV2_GAME:-/civ2}" /tmp/g && chmod -R u+w /tmp/g && cp /work/harness/CIV2.DAT /tmp/g/
[ -f /work/harness/CIV.INI ] && cp /work/harness/CIV.INI /tmp/g/
mkdir -p "$out"
cd /tmp/g
exe=${EXE:-civ2}
case "$exe" in
*.exe)
    w() { printf 'Z:%s' "$1" | tr / '\\'; }
    export CIV2_DATA=$(w /work/build/data) CIV2_SCRIPT=$(w "$script") CIV2_SHOT_DIR=$(w "$out") \
           CIV2_SEED=${SEED:-7} CIV2_RAND_LOG=$(w "$out/rand.log") CIV2_HOOK_LOG=$(w "$out/hook.log") \
           CIV2_CALL_LOG=$(w "$out/calls.log") \
           CIV2_WINDIR=$(w /tmp/g) CIV2_WINE_TEXT=1 WINEDEBUG=-all WINEARCH=win64 WINEPREFIX=$HOME/wine64
    exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} /usr/lib/wine/wine64 /work/build/$exe ;;
esac
export CIV2_DATA=/work/build/data CIV2_SCRIPT=$script CIV2_SHOT_DIR=$out CIV2_SEED=${SEED:-7} \
       CIV2_RAND_LOG=$out/rand.log CIV2_HOOK_LOG=$out/hook.log CIV2_CALL_LOG=$out/calls.log \
       CIV2_WINDIR=/tmp/g CIV2_WINE_TEXT=1
if [ "$3" = gdb ]; then
    exec gdb -batch ${GDBX:--ex run -ex bt} /work/build/$exe
fi
if [ "$3" = valgrind ]; then
    exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} valgrind --error-limit=no --num-callers=12 --track-origins=yes \
        --log-file="$out/valgrind.txt" /work/build/$exe
fi
exec timeout -k ${TIMEOUT_KILL:-10} ${TIMEOUT:-600} /work/build/$exe
