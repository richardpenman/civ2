#!/bin/sh
# Inside a civ2-dev container: OUR build (build/${EXE:-civ2-linux32}; EXE=OpenCiv2: the
# portable build; EXE=OpenCiv2.exe: the Windows build under Wine; GDB=1: under gdb) on its own Xvfb :${DISP:-7}, its XDaemon log in xdlog-ROLE.txt.
# usage: start_ours.sh RUNNAME ROLE      (then host_lan.sh / join_lan.sh as for the original)
OUT=/work/run/xd/cap/$1; ROLE=$2; D=${DISP:-7}; G=/tmp/ours$D
mkdir -p $OUT
pkill -f "Xvfb :$D" 2>/dev/null; sleep 1
rm -rf $G; cp -r /civ2 $G; chmod -R u+w $G
cp /work/harness/CIV2.DAT /work/harness/CIV.INI $G/
[ -n "$SIMUL" ] && printf 'Simultaneous=1\r\n' >> $G/CIV.INI   # SIMUL=1: simultaneous moves offered
[ -n "$SAVE" ] && cp "$SAVE" $G/     # SAVE=FILE: a saved game to load
rm -f /tmp/.X$D-lock /tmp/.X11-unix/X$D
Xvfb :$D -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
sleep 2
case "${EXE:-civ2-linux32}" in
OpenCiv2) set -- /work/build/OpenCiv2 $G ;;
OpenCiv2.exe)    # the Windows build (64-bit) under Wine: its Winsock code; paths as Z:\...
    w() { printf 'Z:%s' "$1" | tr / '\\'; }
    [ -f $HOME/wine64/system.reg ] || DISPLAY=:$D WINEARCH=win64 WINEPREFIX=$HOME/wine64 /usr/lib/wine/wine64 wineboot -i >/dev/null 2>&1
    cd $G && DISPLAY=:$D SDL_AUDIODRIVER=dummy WINEDEBUG=-all WINEARCH=win64 WINEPREFIX=$HOME/wine64 \
        CIV2_DATA=$(w /work/data/native) CIV2_WINDIR=$(w $G) CIV2_XDLOG=$OUT/xdlog-$ROLE.txt \
        /usr/lib/wine/wine64 /work/build/OpenCiv2.exe "$(w $G)" > $OUT/ours-$ROLE.log 2>&1 &
    sleep 1
    echo started
    exit 0 ;;
*) set -- /work/build/${EXE:-civ2-linux32} ;;
esac
# GDB=1: a crash's (or a fatal error's) backtrace in the log
[ -n "$GDB" ] && set -- gdb -batch -ex "handle SIGPIPE nostop noprint" -ex "break plat_fatal" -ex run -ex "bt 80" -ex "bt -40" --args "$@"
cd $G && DISPLAY=:$D SDL_AUDIODRIVER=dummy CIV2_DATA=/work/data/native CIV2_WINDIR=$G \
    CIV2_XDLOG=$OUT/xdlog-$ROLE.txt "$@" > $OUT/ours-$ROLE.log 2>&1 &
sleep 1
echo started
