#!/usr/bin/env bash
# The release's smoke test (.github/workflows/release.yml, the job smoke):
# the packaged program started as a user starts it, on a fresh account
# without the original game: an empty game folder (`OpenCiv2 FOLDER`, the
# folder played where it is), which OpenCiv2's own files fill in, HD
# graphics on (docs/NO-INSTALL.md).  Passes when the main menu is shown:
# CIV2_TEST_QUIT_AT_MENU=1 (src/platform/replay.c) saves the screen
# (main-menu.bmp, main-menu-hd.bmp), logs "smoke: main menu shown" and quits.
#
# Usage: tools/smoke-test.sh PROGRAM OUTDIR
#   PROGRAM  the unpacked package's program: OpenCiv2/OpenCiv2 (Linux),
#            OpenCiv2/OpenCiv2.exe (Windows, from Git Bash),
#            OpenCiv2.app/Contents/MacOS/OpenCiv2 (macOS)
#   OUTDIR   made anew: game/ (the empty game folder), home/ (the fresh
#            account's home), hook.log, output.log, the screenshots
# TIMEOUT (seconds, default 120): longer, the program is stopped, the screen
# captured (screen.png, where a tool for it is found) and the test fails.
# Audio: SDL's dummy driver.  The display: the one there is (Linux: run it
# under xvfb-run; Windows and macOS: the runner's desktop session).
set -u
prog=$1
out=$2
timeout=${TIMEOUT:-120}

rm -rf "$out"
mkdir -p "$out/game" "$out/home"
out=$(cd "$out" && pwd)

# Paths as the program takes them (Windows: C:\..., from Git Bash's /c/...)
native() {
    if command -v cygpath >/dev/null 2>&1; then cygpath -w "$1"; else printf '%s' "$1"; fi
}

# The whole screen, where a tool for it is found (on failure)
screen_shot() {
    case "$(uname -s)" in
    Darwin) screencapture -x "$out/screen.png" ;;
    MINGW*|MSYS*|CYGWIN*)
        powershell -NoProfile -Command "Add-Type -AssemblyName System.Windows.Forms,System.Drawing;
            \$b = [System.Windows.Forms.SystemInformation]::VirtualScreen;
            \$i = New-Object System.Drawing.Bitmap \$b.Width, \$b.Height;
            [System.Drawing.Graphics]::FromImage(\$i).CopyFromScreen(\$b.Left, \$b.Top, 0, 0, \$i.Size);
            \$i.Save('$(native "$out/screen.png")')" ;;
    *)
        if command -v import >/dev/null 2>&1; then import -window root "$out/screen.png"
        elif command -v xwd >/dev/null 2>&1; then xwd -root -silent -out "$out/screen.xwd"
        else false
        fi ;;
    esac >/dev/null 2>&1 || echo "smoke: no screenshot of the screen"
}

HOME="$out/home"
USERPROFILE=$(native "$out/home")
XDG_DATA_HOME="$out/home/.local/share"
XDG_CONFIG_HOME="$out/home/.config"
CIV2_SHOT_DIR=$(native "$out")
CIV2_HOOK_LOG=$(native "$out/hook.log")
export HOME USERPROFILE XDG_DATA_HOME XDG_CONFIG_HOME CIV2_SHOT_DIR CIV2_HOOK_LOG
export CIV2_TEST_QUIT_AT_MENU=1 SDL_AUDIODRIVER=dummy
echo "smoke: $prog $out/game (at most $timeout s)"
start=$(date +%s)
"$prog" "$(native "$out/game")" >"$out/output.log" 2>&1 &
pid=$!
waited=0
while kill -0 "$pid" 2>/dev/null && [ "$waited" -lt "$timeout" ]; do
    sleep 1
    waited=$((waited + 1))
done
if kill -0 "$pid" 2>/dev/null; then
    echo "smoke: FAIL: still running after $timeout s, stopped"
    screen_shot
    kill "$pid" 2>/dev/null
    sleep 2
    kill -9 "$pid" 2>/dev/null
    wait "$pid" 2>/dev/null
    status=timeout
else
    wait "$pid"
    status=$?
fi
echo "smoke: exit status $status after $(($(date +%s) - start)) s"
cat "$out/output.log"
fail=
[ "$status" = 0 ] || fail="the exit status is $status"
grep -q "smoke: main menu shown" "$out/hook.log" 2>/dev/null || fail="${fail:+$fail; }no \"smoke: main menu shown\" in hook.log"
grep -q "smoke: main menu shown .*HD graphics on" "$out/hook.log" 2>/dev/null || fail="${fail:+$fail; }HD graphics not on"
[ -s "$out/main-menu.bmp" ] || fail="${fail:+$fail; }no main-menu.bmp"
if [ -n "$fail" ]; then
    [ "$status" = timeout ] || screen_shot
    echo "smoke: FAIL: $fail"
    echo "smoke: the end of hook.log:"
    tail -40 "$out/hook.log" 2>/dev/null
    exit 1
fi
grep "smoke: main menu shown" "$out/hook.log"
echo "smoke: PASS"
