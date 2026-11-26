#!/bin/sh
# OpenCiv2's own music (data/native/music, made by make own-assets:
# tools/build_own_assets.py; src/platform/sound.c: cd_open_track;
# docs/NO-INSTALL.md, Music):
# 1. every track decodes whole with stb_vorbis, as the music player decodes
#    it (tests/music_decode.c): stereo, 48 kHz, as long as the source says
#    (data/own-assets.json's tracks: openciv2/openciv2-assets' README, to a
#    second);
# 2. without the game's Music folder (the replays leave it out) and with
#    audio (CIV2_TEST_AUDIO=1, SDL's dummy driver, which consumes the
#    samples in real time; CIV2_REALTIME=1) the game, asked for CD track 2
#    (snd_cd_play_track) at once (before the main menu's looping sound: the
#    dummy driver opens one device at a time), finds 12 tracks, plays track 2 from data/native/music/track02.ogg, and
#    it is still playing 5 seconds later (the player fed with its samples:
#    one it could not decode would have ended at once);
# 3. with the game's Music folder (CIV2_KEEP_MEDIA=1) it plays the game's own
#    Music\Track02.
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
out=$PWD/run/music-fallback
rm -rf "$out"
CIV2_TEST_AUDIO=1 CIV2_REALTIME=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 \
    sh tools/native-run.sh $script "$out" > "$out.log" 2>&1
if ! grep -q "cdaudio: playing track 2 (notify) from .*/music/track02.ogg" "$out/hook.log"; then
    echo "FAIL: without the Music folder, track 2 not played from data/native/music (run/music-fallback/hook.log)"; fail=1
elif grep -q "cdaudio: track 2 ended" "$out/hook.log"; then
    echo "FAIL: track 2 ended at once: the player was not fed (run/music-fallback/hook.log)"; fail=1
fi
out2=$PWD/run/music-fallback-install
rm -rf "$out2"
CIV2_KEEP_MEDIA=1 CIV2_TEST_AUDIO=1 CIV2_REALTIME=1 SDL_AUDIODRIVER=dummy TIMEOUT=120 \
    sh tools/native-run.sh $script "$out2" > "$out2.log" 2>&1
if ! grep -q 'cdaudio: playing track 2 (notify) from Music\\Track02\.' "$out2/hook.log"; then
    echo "FAIL: with the Music folder, track 2 not played from it (run/music-fallback-install/hook.log)"; fail=1
fi
[ $fail = 0 ] && echo "PASS music fallback ($(wc -l < run/music-decode.txt) tracks decoded; track 2 from data/native/music without the Music folder, from the game's Music folder with it)"
exit $fail
