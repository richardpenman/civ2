#!/bin/sh
# OpenCiv2's own sound effects (data/native/sound, src/platform/sound.c:
# sound_fallback; docs/NO-INSTALL.md): without the game's Sound folder
# (CIV2_NO_SOUND=1) and with audio (CIV2_TEST_AUDIO=1, SDL's dummy driver)
# the start menu's sound (MENULOOP, which the game plays where it has no
# music) is made from data/native/sound/menuloop.ogg, a WAV as the game reads
# them (22050 Hz, 8 bits, mono), where the game opens it; with the folder
# nothing is made.
# Inside the container (make test-sound): sh tests/sound_fallback_test.sh
# (EXE: the build to run, default civ2-linux32)
cd "$(dirname "$0")/.." || exit 1
# (the sound effects are still being chosen and not in the repository: without
# the folder there is nothing to make, and the game plays nothing, as before)
if [ ! -f data/native/sound/menuloop.ogg ]; then
    echo "SKIP sound fallback: no data/native/sound/menuloop.ogg (OpenCiv2's own sound effects are not yet in the repository)"
    exit 0
fi
export EXE=${EXE:-civ2-linux32}
script=/tmp/sound_fallback.txt
printf 'wait 8000\nquit\n' > $script
mkdir -p run
fail=0
out=$PWD/run/sound-fallback
rm -rf "$out"
CIV2_NO_SOUND=1 CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 \
    sh tools/native-run.sh $script "$out" > "$out.log" 2>&1
if ! grep -q "sound: MENULOOP.WAV made from .*/sound/menuloop.ogg" "$out/hook.log"; then
    echo "FAIL: without the Sound folder, no MENULOOP.WAV made (run/sound-fallback/hook.log)"; fail=1
elif ! python3 - /tmp/g/OpenCiv2Sounds/MENULOOP.WAV <<'PY'
import sys, wave
w = wave.open(sys.argv[1])
assert (w.getframerate(), w.getsampwidth(), w.getnchannels()) == (22050, 1, 1), w.getparams()
assert w.getnframes() > 22050, w.getnframes()
PY
then
    echo "FAIL: MENULOOP.WAV made is not 22050 Hz, 8 bits, mono"; fail=1
fi
out2=$PWD/run/sound-fallback-install
rm -rf "$out2"
CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 sh tools/native-run.sh $script "$out2" > "$out2.log" 2>&1
if grep -q "sound: .* made from" "$out2/hook.log" || [ -d /tmp/g/OpenCiv2Sounds ]; then
    echo "FAIL: with the Sound folder, a sound was made (run/sound-fallback-install/hook.log)"; fail=1
fi
[ $fail = 0 ] && echo "PASS sound fallback ($(grep -c 'sound: .* made from' "$out/hook.log") made without the Sound folder, none with it)"
exit $fail
