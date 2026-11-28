#!/bin/sh
# Looping sounds stopped on every way out (harness/sound/*.txt; with audio:
# CIV2_TEST_AUDIO=1, SDL's dummy driver).  The game loops only these
# sounds (snd_play_effect's loop: streamed, snd_wave_open_stream, read
# again from the start at the end): the menus' theme MENULOOP (game_main_menu_loop,
# the opening video's end and key, the scenario and multiplayer set-ups;
# stopped by snd_menu_end or the main menu's end), and the diplomacy room's
# drums DRUMxL (diplo_greeting_sound_done_cb once the leader's greeting has
# ended; stopped by diplo_leader_wnd_close).  The scripts' `sounds LABEL`
# log what the mixer has (src/auto/sound.cpp: snd_log_state):
#   noloop    no looping sound playing (MENULOOP, the drums or any other);
#   drums     the room's drums looping (the test is of something);
#   theme     the menus' theme looping;
#   greeting  the drums not started yet (the greeting still playing);
#   (none)    only logged.
# Whatever the label, no looping sound twice in the mixer at once (two
# copies of one loop: in a `sounds` line, or in the mixer's log of what it
# mixes, "sound: mixing ID ...", of an ID seen looping).
# waitsound (src/platform/replay.c) waits, in real time, for the sounds
# that end (a greeting, the menus' clicks) to end first.  The cases:
#  - a new game set up to its first turn's map, and a game loaded from the
#    main menu: with Classic graphics (with the game's Sound folder and
#    without it: OpenCiv2's own), HD graphics, and without the install
#    (CIV2_NO_GRAPHICS, NO_TEXT, NO_RULES, NO_SOUND: HD graphics, OpenCiv2's
#    own sounds; CI, which has no game, runs the new game this way);
#  - the diplomacy room: talks ended with Escape, gone through with OK and
#    their popups, an emissary sent away before the drums start (then a
#    room opened again at once: CIV2_REALTIME=1, the keys within the
#    greeting's seconds), the AIs coming to the player; Classic and HD;
#    with the CD's animated heralds (KINGS, CIV2_KEEP_MEDIA=1).  (A room
#    opened while another is open, diplo_leader_wnd_launch, would leave the
#    first one's drums playing, its sound_ids[0] overwritten; the game never
#    does: diplo_greet_emissary opens one once a session, emissary_asked,
#    and diplo_session_reset closes it with the session.  A script's `call`
#    made in the middle of a talk can, so the scripts wait for each talk's
#    end before the next);
#  - the opening video (the CD's Video) ended with a click, a key, or played
#    to its end (CIV2_REALTIME=1), the theme then looping in the main menu,
#    stopped once a game is loaded.
# Inside the container (make test-sound): sh tests/sound_loops_test.sh
# (EXE: the build to run, default civ2-linux32; NAMES="case ...": only those)
# Cases that need what the game folder ($CIV2_GAME, /civ2) lacks (the
# install's pictures, its Sound folder, the autosave Ca_Auto.SAV, KINGS,
# Video) or the HD art (data/native/hd) are skipped, each saying so.
cd "$(dirname "$0")/.." || exit 1
export EXE=${EXE:-civ2-linux32}
mkdir -p run
game=${CIV2_GAME:-/civ2}
has() { [ -n "$(find "$game" -maxdepth 1 -iname "$1" 2>/dev/null | head -1)" ]; }
classic=; has civ2.exe && classic=1
folder=; has sound && folder=1
save=; has Ca_Auto.SAV && save=1
kings=; has kings && kings=1
video=; [ -n "$(find "$game" -maxdepth 2 -ipath '*/video/opening.avi' 2>/dev/null | head -1)" ] && video=1
hd=; [ -f data/native/hd/pictures.txt ] && hd=1
own=; [ -f data/native/sound/menuloop.ogg ] && own=1
NOINSTALL="CIV2_NO_GRAPHICS=1 CIV2_NO_TEXT=1 CIV2_NO_RULES=1 CIV2_NO_SOUND=1"
fail=0; passed=; skipped=

# check NAME: run/sound-loops-NAME/hook.log's `sounds` as their labels want
check() {
    python3 - "run/sound-loops-$1/hook.log" <<'PY'
import re, sys
lines = open(sys.argv[1], encoding='latin-1').read().splitlines()
probs, seen, label = [], 0, None
# the sounds seen looping; (a loop twice in the mixer at once)
loop_ids = set(re.findall(r' (\d+)\([^,)]*,loop', '\n'.join(l for l in lines if l.startswith('sound: state:'))))
twice = set()
for l in lines:
    if l.startswith('sound: mixing'):
        ids = l.split()[2:]
        for i in sorted(set(ids)):
            if ids.count(i) > 1 and i in loop_ids and i not in twice:
                twice.add(i)
                probs.append(f'looping sound {i} mixed twice at once: {l}')
    m = re.match(r't=(\d+): sounds ?(\S*)', l)
    if m:
        label = (m.group(2), m.group(1))
        continue
    if l.startswith('replay: waitsound done (timed out)'):
        probs.append('a sound that ends still playing after 30 s: ' + l)
    if label and l.startswith('sound: state:'):
        what, t = label
        label = None
        seen += 1
        loops = int(re.search(r'loops (\d+)', l).group(1))
        queued = int(re.search(r'queued (\d+) ms', l).group(1))
        ids = re.findall(r' (\d+\([^,)]*),loop', l)
        dup = sorted(set(i for i in ids if ids.count(i) > 1))
        if dup:
            probs.append(f'{what or "-"} at t={t} ms: a loop twice at once ({", ".join(d + ")" for d in dup)}): {l}')
        if what in ('noloop', 'greeting') and loops:
            probs.append(f'{what} at t={t} ms: a sound loops: {l}')
        elif what == 'noloop' and queued > 3000:
            # (the game's buffers: 8 of some 186 ms, 12 once a video's
            # sound has played, an animated herald's)
            probs.append(f'noloop at t={t} ms: {queued} ms queued (at most the game\'s buffers, 3000): {l}')
        elif what == 'drums' and not re.search(r'\(DRUM..,loop', l):
            probs.append(f'drums at t={t} ms: the diplomacy room\'s drums not looping: {l}')
        elif what == 'theme' and '(MENULOOP,loop' not in l:
            probs.append(f'theme at t={t} ms: the menus\' theme not looping: {l}')
        print(f'  t={t} {what or "-"}: {l[len("sound: state:"):].strip()}')
if not any(l.startswith('replay: script finished') for l in lines):
    probs.append('the script did not finish')
if not seen:
    probs.append('no sound state logged')
for p in probs:
    print('  PROBLEM: ' + p)
sys.exit(1 if probs else 0)
PY
}

# case NAME SCRIPT NEEDS ENV...: harness/sound/SCRIPT.txt with audio, its
# log in run/sound-loops-NAME; NEEDS: the words of what it needs
case_() {
    name=$1; script=$2; needs=$3; shift 3
    if [ -n "$NAMES" ] && ! echo " $NAMES " | grep -q " $name "; then return; fi
    for n in $needs; do
        eval "ok=\$$n"
        if [ -z "$ok" ]; then
            echo "SKIP $name: no $n"; skipped="$skipped $name"; return
        fi
    done
    rm -rf "$PWD/run/sound-loops-$name"
    env "$@" CIV2_TEST_AUDIO=1 SDL_AUDIODRIVER=dummy TIMEOUT=${CASE_TIMEOUT:-600} \
        sh tools/native-run.sh "$PWD/harness/sound/$script.txt" "$PWD/run/sound-loops-$name" \
        > "$PWD/run/sound-loops-$name.log" 2>&1
    echo "$name ($script.txt, $*):"
    if check "$name"; then
        passed="$passed $name"
    else
        echo "FAIL: $name (run/sound-loops-$name/hook.log)"; fail=1
    fi
}

# a new game to its first turn, a game loaded
case_ newgame-classic newgame "classic folder" CIV2_TEST_HD=
case_ newgame-classic-own newgame "classic own" CIV2_TEST_HD= CIV2_NO_SOUND=1
case_ newgame-hd newgame "hd" CIV2_TEST_HD=1
case_ newgame-noinstall newgame "hd" $NOINSTALL
case_ loadgame-classic loadgame "classic folder save" CIV2_TEST_HD=
case_ loadgame-hd loadgame "hd save" CIV2_TEST_HD=1
case_ loadgame-noinstall loadgame "hd save" $NOINSTALL
# the diplomacy room
case_ talk-escape-classic talk-escape "classic folder save" CIV2_TEST_HD=
case_ talk-escape-hd talk-escape "hd save" CIV2_TEST_HD=1
case_ talk-escape-noinstall talk-escape "hd save own" $NOINSTALL
case_ talk-ok-classic talk-ok "classic folder save" CIV2_TEST_HD=
case_ talk-early-classic talk-early "classic folder save" CIV2_TEST_HD= CIV2_REALTIME=1
case_ talk-early-hd talk-early "hd save" CIV2_TEST_HD=1 CIV2_REALTIME=1
case_ talk-ai-classic talk-ai "classic folder save" CIV2_TEST_HD=
case_ talk-herald talk-herald "classic folder save kings video" CIV2_TEST_HD= CIV2_KEEP_MEDIA=1
# the opening video
case_ opening-click opening-click "classic folder save video" CIV2_TEST_HD= CIV2_KEEP_MEDIA=1
case_ opening-key opening-key "classic folder save video" CIV2_TEST_HD= CIV2_KEEP_MEDIA=1
case_ opening-end opening-end "classic folder save video" CIV2_TEST_HD= CIV2_KEEP_MEDIA=1 CIV2_REALTIME=1

if [ $fail = 0 ]; then
    echo "PASS sound loops (no looping sound left after the menus, the diplomacy room or the opening video:${passed:- no case ran}${skipped:+; skipped:$skipped})"
fi
exit $fail
