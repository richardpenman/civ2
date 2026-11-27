#!/bin/sh
# OpenCiv2's own wonder movies (data/native/wonders, made by make own-assets:
# tools/build_own_assets.py; src/platform/hd.c: hd_wonder_find; video.c:
# open_own_wonder; the game's WonderMovieWindow::play; docs/NO-INSTALL.md,
# Wonder movies), on the autosave with the Pyramids' movie (wonder 0,
# wonder_show_movie) asked for:
# 1. with HD graphics (CIV2_TEST_HD=2) and audio (CIV2_TEST_AUDIO=1, SDL's
#    dummy driver, which consumes the samples in real time; CIV2_REALTIME=1)
#    the game opens data/native/wonders/wonder00.webp and its sound
#    wonder00.ogg (22050 Hz, 8 bits, stereo, as the CD's), and plays them
#    to the end: the picture's frames drawn (at least 120 of the 165, the
#    player dropping those it is late for, as with the CD's; to the last) and
#    its sound read to its end (as the game reads the CD's movies' sound, in
#    time with the picture), its window 427 x 240 and the movie in it (the
#    screenshot mid-movie: run/wonder-hd/w1-hd.png); with the CD's Video
#    folder (CIV2_KEEP_MEDIA=1) too, ours first, as for the music;
# 2. with Classic graphics and the CD's Video folder the game's own
#    WONDER00.AVI, not ours;
# 3. with Classic graphics without the folder ours (as the music and sound
#    effects: OpenCiv2's where the game folder has none);
# 4. a replay without CIV2_TEST_WONDER=1 never plays ours (the recordings,
#    with Classic or HD graphics, were made without them): nothing opened.
# 1 with the Video folder and 2 need the CD's Video in the game folder
# (skipped without it).
# Inside the container (make test-wonder): sh tests/wonder_movie_test.sh
# (EXE: the build to run, default civ2-linux32)
cd "$(dirname "$0")/.." || exit 1
if [ ! -f data/native/wonders/wonder00.webp ]; then
    echo "SKIP wonder movies: no data/native/wonders/wonder00.webp (make own-assets)"
    exit 0
fi
export EXE=${EXE:-civ2-linux32}
script=/tmp/wonder_movie.txt
cat > $script <<'EOF'
wait 3000
click 20 400
wait 5000
key Down Down Down Down
wait 500
file Ca_Auto.SAV
key Return
wait 20000
key Return
wait 5000
call wonder_show_movie 4bb8e0 0
wait 5000
shot w1
wait 10000
shot w2-after
quit
EOF
mkdir -p run
fail=0
video=$(ls -d "${CIV2_GAME:-/civ2}"/[Vv][Ii][Dd][Ee][Oo] 2>/dev/null | head -1)
# run NAME ENV...: the script, run/NAME/hook.log (CIV2_LOG_VIDEO: each frame and read)
run() {
    name=$1; shift
    rm -rf "$PWD/run/$name"
    env "$@" CIV2_LOG_VIDEO=1 TIMEOUT=300 sh tools/native-run.sh $script "$PWD/run/$name" > "$PWD/run/$name.log" 2>&1
}
# own NAME WHAT: ours opened, picture and sound, played to the end
own() {
    log=run/$1/hook.log
    if ! grep -q "AVIFileOpen(.*wonder00.avi): OpenCiv2's own wonder movie .*/wonders/wonder00.webp, 165 frames in 11000 ms, 427x240; its sound .*/wonders/wonder00.ogg, 24[0-9][0-9][0-9][0-9] samples" "$log"; then
        echo "FAIL: $2, OpenCiv2's Pyramids movie not opened with its sound ($log)"; fail=1; return
    fi
    frames=$(grep -c "video: own wonder movie frame" "$log")
    last=$(grep "audio bytes at sample" "$log" | tail -1 | sed 's/.*read \([0-9]*\) audio bytes at sample \([0-9]*\).*/\1 \2/')
    end=$(echo "$last" | awk '{print $2 + $1 / 2}')
    lastframe=$(grep "video: own wonder movie frame" "$log" | tail -1 | sed 's/.*frame \([0-9]*\):.*/\1/')
    if [ "$frames" -lt 120 ] || [ "${lastframe:-0}" -lt 160 ]; then
        echo "FAIL: $2, $frames of its 165 frames drawn, the last ${lastframe:-none} ($log)"; fail=1
    elif [ -z "$end" ] || [ "${end%.*}" -lt 240000 ]; then
        echo "FAIL: $2, its sound read to sample ${end:-none} of 242550 ($log)"; fail=1
    else
        echo "ok: $2: $frames frames drawn (the last $lastframe), its sound read to sample ${end%.*}"
    fi
}
# HD graphics (the art, data/native/hd, must be there), with audio on the real clock
if [ -f data/native/hd/pictures.txt ]; then
    run wonder-hd CIV2_TEST_WONDER=1 CIV2_TEST_HD=2 CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy CIV2_REALTIME=1
    own wonder-hd "HD graphics without the Video folder"
    if [ -n "$video" ]; then
        run wonder-hd-install CIV2_TEST_WONDER=1 CIV2_TEST_HD=2 CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy CIV2_REALTIME=1 CIV2_KEEP_MEDIA=1
        own wonder-hd-install "HD graphics with the Video folder"
    fi
else
    echo "SKIP wonder movies with HD graphics: no data/native/hd (make hd-art)"
fi
# Classic graphics with the Video folder: the game's own
if [ -n "$video" ]; then
    run wonder-classic-install CIV2_TEST_WONDER=1 CIV2_KEEP_MEDIA=1
    if grep -q "own wonder movie" run/wonder-classic-install/hook.log ||
       ! grep -q "AVIFileOpen(.*[Ww][Oo][Nn][Dd][Ee][Rr]00.[Aa][Vv][Ii]): 2 streams" run/wonder-classic-install/hook.log; then
        echo "FAIL: Classic graphics with the Video folder, not the game's WONDER00.AVI (run/wonder-classic-install/hook.log)"; fail=1
    else
        echo "ok: Classic graphics with the Video folder: the game's WONDER00.AVI"
    fi
else
    echo "SKIP wonder movies with the CD's Video folder: the game folder has none"
fi
# Classic graphics without it: ours (the picture, on the replay's clock)
run wonder-classic CIV2_TEST_WONDER=1
if ! grep -q "OpenCiv2's own wonder movie" run/wonder-classic/hook.log ||
   [ "$(grep -c 'video: read frame .* of 165 ' run/wonder-classic/hook.log)" -lt 150 ]; then
    echo "FAIL: Classic graphics without the Video folder, OpenCiv2's movie not played (run/wonder-classic/hook.log)"; fail=1
else
    echo "ok: Classic graphics without the Video folder: OpenCiv2's"
fi
# A replay without CIV2_TEST_WONDER: none (with HD graphics, where ours would come first)
run wonder-replay CIV2_TEST_HD=2
if grep -q "own wonder movie\|AVIFileOpen(.*wonder00" run/wonder-replay/hook.log; then
    echo "FAIL: a replay without CIV2_TEST_WONDER opened a wonder movie (run/wonder-replay/hook.log)"; fail=1
fi
[ $fail = 0 ] && echo "PASS wonder movies (OpenCiv2's Pyramids, picture and sound, with HD graphics$([ -n "$video" ] && echo ', also over the game'"'"'s'); with Classic graphics the game's own where it has it, ours where not; none in other replays)"
exit $fail
