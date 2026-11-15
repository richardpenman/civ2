#!/bin/sh
# Inside a civ2-dev container: one original game (hook -> log proxy -> original XDaemon)
# on its own Xvfb :${DISP:-7}.   usage: start1.sh RUNNAME ROLE
OUT=/work/run/xd/cap/$1; ROLE=$2
mkdir -p $OUT
wineserver -k 2>/dev/null; pkill Xvfb; sleep 1
rm -rf /tmp/g; cp -r /civ2 /tmp/g; chmod -R u+w /tmp/g; [ -n "$CIV2_KEEP_MEDIA" ] || find /tmp/g -maxdepth 1 -type d \( -iname kings -o -iname video -o -iname music \) -exec rm -rf {} +
mv /tmp/g/XDaemon.dll /tmp/g/XDaemon_orig.dll
cp /work/run/xd/logproxy/XDaemon.dll /tmp/g/XDaemon_real.dll
cp /work/hook/XDaemon.dll /tmp/g/XDaemon.dll
cp /work/harness/CIV2.DAT /tmp/g/CIV2.DAT
[ -n "$SAVE" ] && cp "$SAVE" /tmp/g/     # SAVE=FILE: a saved game to load
cp /work/harness/CIV.INI /wine/drive_c/windows/CIV.INI 2>/dev/null
[ -n "$SIMUL" ] && printf 'Simultaneous=1\r\n' >> /wine/drive_c/windows/CIV.INI   # SIMUL=1: simultaneous moves offered
rm -f /tmp/.X${DISP:-7}-lock /tmp/.X11-unix/X${DISP:-7}
Xvfb :${DISP:-7} -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
sleep 2
[ -f /wine/system.reg ] || DISPLAY=:${DISP:-7} wineboot -i
DISPLAY=:${DISP:-7} wine reg delete 'HKLM\Software\Microsoft\Windows NT\CurrentVersion\AeDebug' /v Debugger /f >/dev/null 2>&1
sleep 3
cd /tmp/g && WINEDEBUG=${WINEDEBUG:-} DISPLAY=:${DISP:-7} XDLOG='Z:'$OUT/xdlog-$ROLE.txt CIV2_HOOK_LOG='Z:'$OUT/hook-$ROLE.log CIV2_HOOKS=none wine civ2.exe > $OUT/wine-$ROLE.log 2>&1 &
sleep 1
echo started
