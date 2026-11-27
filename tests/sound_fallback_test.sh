#!/bin/sh
# OpenCiv2's own sound effects (data/native/sound, made by make own-assets:
# tools/build_own_assets.py; src/platform/sound.c: sound_fallback; files.c:
# synth_path; docs/NO-INSTALL.md), with audio (CIV2_TEST_AUDIO=1, SDL's dummy
# driver) and the start menu's sound (MENULOOP, which the game plays where it
# has no music):
# 1. with Classic graphics (a replay's) and without the game's Sound folder
#    (CIV2_NO_SOUND=1) it is made from data/native/sound/menuloop.ogg, a WAV
#    as the game reads them (22050 Hz, 8 bits, mono), where the game opens it;
# 2. with Classic graphics and the folder nothing is made (the game's own);
# 3. with HD graphics (CIV2_TEST_HD=1) it is made from data/native/sound
#    first, with the game's Sound folder and without it.
# Inside the container (make test-sound): sh tests/sound_fallback_test.sh
# (EXE: the build to run, default civ2-linux32)
cd "$(dirname "$0")/.." || exit 1
# (without them, OWN_ASSETS=, there is nothing to make, and the game plays
# its own or nothing, as before)
if [ ! -f data/native/sound/menuloop.ogg ]; then
    echo "SKIP sound fallback: no data/native/sound/menuloop.ogg (make own-assets)"
    exit 0
fi
export EXE=${EXE:-civ2-linux32}
script=/tmp/sound_fallback.txt
printf 'wait 8000\nquit\n' > $script
mkdir -p run
fail=0
# run NAME ENV...: the script with audio, run/NAME/hook.log
run() {
    name=$1; shift
    rm -rf "$PWD/run/$name"
    env "$@" CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 \
        sh tools/native-run.sh $script "$PWD/run/$name" > "$PWD/run/$name.log" 2>&1
}
# own NAME WHAT: MENULOOP.WAV made from data/native/sound/menuloop.ogg, as the game reads them
own() {
    if ! grep -q "sound: MENULOOP.WAV made from .*/sound/menuloop.ogg" "run/$1/hook.log"; then
        echo "FAIL: $2, no MENULOOP.WAV made from data/native/sound (run/$1/hook.log)"; fail=1
    elif ! python3 - /tmp/g/OpenCiv2Sounds/MENULOOP.WAV <<'PY'
import sys, wave
w = wave.open(sys.argv[1])
assert (w.getframerate(), w.getsampwidth(), w.getnchannels()) == (22050, 1, 1), w.getparams()
assert w.getnframes() > 22050, w.getnframes()
PY
    then
        echo "FAIL: $2, the MENULOOP.WAV made is not 22050 Hz, 8 bits, mono"; fail=1
    fi
}
# Classic graphics (a replay's), without the Sound folder: OpenCiv2's
run sound-fallback CIV2_TEST_HD= CIV2_NO_SOUND=1
own sound-fallback "Classic graphics without the Sound folder"
# Classic graphics with it: the game's own, nothing made
run sound-fallback-install CIV2_TEST_HD=
if grep -q "sound: .* made from" run/sound-fallback-install/hook.log || [ -d /tmp/g/OpenCiv2Sounds ]; then
    echo "FAIL: Classic graphics with the Sound folder, a sound was made (run/sound-fallback-install/hook.log)"; fail=1
fi
# HD graphics (CIV2_TEST_HD=1; the art, data/native/hd, must be there):
# OpenCiv2's first, with the Sound folder and without it
if [ -f data/native/hd/pictures.txt ]; then
    run sound-fallback-hd CIV2_TEST_HD=1 CIV2_NO_SOUND=1
    own sound-fallback-hd "HD graphics without the Sound folder"
    run sound-fallback-hd-install CIV2_TEST_HD=1
    own sound-fallback-hd-install "HD graphics with the Sound folder"
    hd="; with HD graphics OpenCiv2's first, with the folder or without"
else
    echo "SKIP sound effects with HD graphics: no data/native/hd (make hd-art)"
    hd=
fi
[ $fail = 0 ] && echo "PASS sound fallback (with Classic graphics $(grep -c 'sound: .* made from' run/sound-fallback/hook.log) made without the Sound folder, none with it$hd)"
exit $fail
