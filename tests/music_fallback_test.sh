#!/bin/sh
# OpenCiv2's own music (data/native/music, made by make own-assets:
# tools/build_own_assets.py; src/platform/sound.c: cd_open_track;
# docs/NO-INSTALL.md, Music):
# 1. every track decodes whole with stb_vorbis, as the music player decodes
#    it (tests/music_decode.c): stereo, 48 kHz, as long as the source says
#    (data/own-assets.json's tracks: openciv2/openciv2-media' README, to a
#    second);
# 2. with Classic graphics (a replay's), without the game's Music folder
#    (the replays leave it out) and with
#    audio (CIV2_TEST_AUDIO=1, SDL's dummy driver, which consumes the
#    samples in real time; CIV2_REALTIME=1) the game, asked for CD track 2
#    (snd_cd_play_track) at once (before the main menu's looping sound: the
#    dummy driver opens one device at a time), finds 12 tracks, plays track 2 from data/native/music/track02.ogg, and
#    it is still playing 5 seconds later (the player fed with its samples:
#    one it could not decode would have ended at once);
# 3. with Classic graphics and the game's Music folder (CIV2_KEEP_MEDIA=1)
#    it plays the game's own Music\Track02;
# 4. with HD graphics (CIV2_TEST_HD=1) it plays data/native/music/track02.ogg
#    first, with the game's Music folder and without it (as 2).
# Inside the container (make test-music): sh tests/music_fallback_test.sh
# (EXE: the build to run, default civ2-linux32)
cd "$(dirname "$0")/.." || exit 1
if [ ! -f data/native/music/track02.ogg ]; then
    echo "SKIP music fallback: no data/native/music (make own-assets)"
    exit 0
fi
export EXE=${EXE:-civ2-linux32}
mkdir -p run build
fail=0
cc -O2 -Isrc/third_party/stb_vorbis -o build/music_decode tests/music_decode.c -lm || exit 1
# (the lengths: 0:35 ... as the source's README has them, to the second)
if ! build/music_decode data/native/music/track*.ogg > run/music-decode.txt; then
    echo "FAIL: a track of data/native/music does not decode (run/music-decode.txt)"; fail=1
elif ! python3 - run/music-decode.txt <<'PY'
import sys
want = {'02': 35, '03': 42, '04': 89, '05': 110, '06': 161, '07': 205, '08': 165, '09': 150, '10': 165,
        '11': 145, '12': 150}
got = {}
for line in open(sys.argv[1]):
    path, chans, rate, secs = line.split()
    n = path[-6:-4]
    assert (int(chans), int(rate)) == (2, 48000), line
    assert abs(float(secs) - want[n]) <= 1.5, line
    got[n] = float(secs)
assert sorted(got) == sorted(want), sorted(got)
print('decoded: %d tracks, %.0f s' % (len(got), sum(got.values())))
PY
then
    echo "FAIL: a track of data/native/music is not stereo, 48 kHz, of its length (run/music-decode.txt)"; fail=1
fi
script=/tmp/music_fallback.txt
printf 'call snd_cd_play_track 5ddbc7 2\nwait 5000\nquit\n' > $script
# run NAME ENV...: the script with audio, run/NAME/hook.log
run() {
    name=$1; shift
    rm -rf "$PWD/run/$name"
    env "$@" CIV2_TEST_AUDIO=1 CIV2_REALTIME=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 \
        sh tools/native-run.sh $script "$PWD/run/$name" > "$PWD/run/$name.log" 2>&1
}
# own NAME WHAT: track 2 played from data/native/music (and not ended at once)
own() {
    if ! grep -q "cdaudio: playing track 2 (notify) from .*/music/track02.ogg" "run/$1/hook.log"; then
        echo "FAIL: $2, track 2 not played from data/native/music (run/$1/hook.log)"; fail=1
    elif grep -q "cdaudio: track 2 ended" "run/$1/hook.log"; then
        echo "FAIL: $2, track 2 ended at once: the player was not fed (run/$1/hook.log)"; fail=1
    fi
}
# Classic graphics (a replay's), without the Music folder: OpenCiv2's
run music-fallback CIV2_TEST_HD=
own music-fallback "Classic graphics without the Music folder"
# Classic graphics with it (CIV2_KEEP_MEDIA=1): the game's own
run music-fallback-install CIV2_TEST_HD= CIV2_KEEP_MEDIA=1
if ! grep -q 'cdaudio: playing track 2 (notify) from Music\\Track02\.' run/music-fallback-install/hook.log; then
    echo "FAIL: Classic graphics with the Music folder, track 2 not played from it (run/music-fallback-install/hook.log)"; fail=1
fi
# HD graphics (CIV2_TEST_HD=1; the art, data/native/hd, must be there):
# OpenCiv2's first, with the Music folder and without it
if [ -f data/native/hd/pictures.txt ]; then
    run music-fallback-hd CIV2_TEST_HD=1
    own music-fallback-hd "HD graphics without the Music folder"
    run music-fallback-hd-install CIV2_TEST_HD=1 CIV2_KEEP_MEDIA=1
    own music-fallback-hd-install "HD graphics with the Music folder"
    hd="; with HD graphics from data/native/music, with the folder or without"
else
    echo "SKIP music with HD graphics: no data/native/hd (make hd-art)"
    hd=
fi
[ $fail = 0 ] && echo "PASS music fallback ($(wc -l < run/music-decode.txt) tracks decoded; with Classic graphics track 2 from data/native/music without the Music folder, from the game's Music folder with it$hd)"
exit $fail
