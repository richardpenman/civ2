#!/bin/sh
# Inside the civ2-dev container: two game dirs with hook -> log proxy -> original XDaemon,
# two Xvfb displays (:7 host, :8 client), two wine civ2.exe.
set -x
OUT=/work/run/xd/cap/$1
mkdir -p $OUT
wineserver -k 2>/dev/null
for n in 1 2; do
  g=/tmp/g$n; rm -rf $g; cp -r /civ2 $g; chmod -R u+w $g; [ -n "$CIV2_KEEP_MEDIA" ] || find $g -maxdepth 1 -type d \( -iname kings -o -iname video -o -iname music \) -exec rm -rf {} +
  mv $g/XDaemon.dll $g/XDaemon_orig.dll
  cp /work/run/xd/logproxy/XDaemon.dll $g/XDaemon_real.dll
  cp /work/hook/XDaemon.dll $g/XDaemon.dll
  cp /work/harness/CIV2.DAT $g/CIV2.DAT
done
cp /work/harness/CIV.INI /wine/drive_c/windows/CIV.INI 2>/dev/null
rm -f /tmp/.X7-lock /tmp/.X8-lock /tmp/.X11-unix/X7 /tmp/.X11-unix/X8
Xvfb :7 -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
Xvfb :8 -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
sleep 2
[ -f /wine/system.reg ] || DISPLAY=:7 wineboot -i
DISPLAY=:7 wine reg delete 'HKLM\Software\Microsoft\Windows NT\CurrentVersion\AeDebug' /v Debugger /f >/dev/null 2>&1
sleep 5; cd /tmp/g1 && DISPLAY=:7 XDLOG='Z:'$OUT/xdlog-host.txt CIV2_HOOK_LOG='Z:'$OUT/hook-host.log CIV2_HOOKS=none wine civ2.exe > $OUT/wine-host.log 2>&1 &
sleep 25
cd /tmp/g2 && DISPLAY=:8 XDLOG='Z:'$OUT/xdlog-client.txt CIV2_HOOK_LOG='Z:'$OUT/hook-client.log CIV2_HOOKS=none wine civ2.exe > $OUT/wine-client.log 2>&1 &
sleep 1
echo started
