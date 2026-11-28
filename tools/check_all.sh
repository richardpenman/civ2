#!/bin/bash
# make check-all: the whole local test suite, with the original game
# installed ($CIV2_GAME, default ~/Civ2: what tools/dk mounts at /civ2 and
# tools/paths.py reads), including everything GitHub's CI
# (.github/workflows/ci.yml) cannot run without it.  Run it before pushing.
#
# Each step runs in turn (serially: a 2-CPU machine with little memory),
# the quick ones first; a failed step does not stop the rest (a step whose
# build failed is SKIP).  Each step's output is in run/check-all/NN-STEP.log;
# at the end a table of the steps (PASS, FAIL, SKIP or WARN, the time, the
# log) and the total time.  Exit status 1 if any step failed (or the install
# or Docker is missing: then nothing is run).  ONLY="step step ...": only
# those steps (the names in the table; the builds they need are not added),
# their logs in run/check-all/only-DATE-TIME.
# Usage: tools/check_all.sh   (make check-all)
cd "$(dirname "$0")/.." || exit 1
ROOT=$PWD
OUT=run/check-all
MAKE=${MAKE:-make}

# --- the install and Docker, first: without them nothing is worth running
GAME_DIR=${CIV2_GAME:-$HOME/Civ2}
case "$GAME_DIR" in "~") GAME_DIR=$HOME ;; "~/"*) GAME_DIR=$HOME/${GAME_DIR#"~/"} ;; esac
if [ ! -d "$GAME_DIR" ] || ! ls "$GAME_DIR" 2>/dev/null | grep -qix 'civ2.exe'; then
    echo "check-all: FAIL: no installed game in $GAME_DIR (no civ2.exe)." >&2
    echo "check-all needs the original game: install it in ~/Civ2 or set CIV2_GAME." >&2
    exit 1
fi
for f in RULES.TXT CIV2.DAT; do
    ls "$GAME_DIR" | grep -qix "$f" || { echo "check-all: FAIL: $GAME_DIR has no $f: not a complete install" >&2; exit 1; }
done
if ! docker info >/dev/null 2>&1; then
    echo "check-all: FAIL: Docker is not usable (docker info failed)." >&2
    echo "If you are not in the docker group in this shell, run:  sg docker -c \"make check-all\"" >&2
    exit 1
fi
export CIV2_GAME=$GAME_DIR
# (GAME is make web's flag: GAME=1 puts the game on the site; the tests use
# the site without it, as CI and the release do)
unset GAME

# (ONLY: a re-run of some steps beside the full run's logs, not over them)
if [ -n "$ONLY" ]; then OUT=$OUT/only-$(date +%Y%m%d-%H%M%S); else rm -rf "$OUT"; fi
mkdir -p "$OUT/tmp"
# (/tmp is a small tmpfs: the steps' temporary files here)
export TMPDIR=$ROOT/$OUT/tmp
NODE=tools/toolchain/node/bin/node

names=() results=() times=() logs=() notes=()
declare -A status
n=0
any_fail=0
t_start=$(date +%s)

fmt() { printf '%d:%02d:%02d' $(($1 / 3600)) $(($1 % 3600 / 60)) $(($1 % 60)); }

summary() {
    local total=$(( $(date +%s) - t_start ))
    echo
    echo "=== check-all summary (install: $GAME_DIR) ==="
    printf '%-22s %-5s %9s  %-40s %s\n' STEP RESULT TIME LOG NOTE
    local i
    for i in "${!names[@]}"; do
        printf '%-22s %-5s %9s  %-40s %s\n' "${names[$i]}" "${results[$i]}" "${times[$i]}" "${logs[$i]}" "${notes[$i]}"
    done
    echo "total: $(fmt $total)"
    if [ "$any_fail" -ne 0 ]; then echo "check-all: FAILED"; else echo "check-all: passed"; fi
    { printf '%-22s %-5s %9s  %-40s %s\n' STEP RESULT TIME LOG NOTE
      for i in "${!names[@]}"; do
          printf '%-22s %-5s %9s  %-40s %s\n' "${names[$i]}" "${results[$i]}" "${times[$i]}" "${logs[$i]}" "${notes[$i]}"
      done
      echo "total: $(fmt $total)"; } > "$OUT/summary.txt"
}
trap 'echo; echo "check-all: interrupted"; any_fail=1; summary; exit 130' INT TERM

wanted() { [ -z "$ONLY" ] || [[ " $ONLY " == *" $1 "* ]]; }

# step NAME [--advisory] [--needs STEP,...] -- COMMAND...
#   --advisory: a failure is WARN, not a failure of check-all (timings)
#   --needs: SKIP when one of those steps did not pass
step() {
    local name=$1 advisory=0 needs="" ; shift
    while [ "$1" != "--" ]; do
        case $1 in --advisory) advisory=1 ;; --needs) needs=$2; shift ;; esac
        shift
    done
    shift
    wanted "$name" || return 0
    n=$((n + 1))
    local log
    log=$(printf '%s/%02d-%s.log' "$OUT" "$n" "$name")
    names+=("$name"); logs+=("$log")
    local dep
    for dep in ${needs//,/ }; do
        if [ -n "${status[$dep]}" ] && [ "${status[$dep]}" != PASS ]; then
            echo "--- $name: SKIP ($dep did not pass)"
            echo "SKIP: $dep did not pass" > "$log"
            results+=(SKIP); times+=(-); notes+=("$dep did not pass"); status[$name]=SKIP
            any_fail=1
            return 0
        fi
    done
    echo "--- $name: $*  ($(date +%H:%M:%S))"
    local t0=$(date +%s) rc
    "$@" > "$log" 2>&1 < /dev/null
    rc=$?
    local dt=$(( $(date +%s) - t0 ))
    times+=("$(fmt $dt)")
    # (a step's own SKIP lines: with the install there should be none)
    local skips note=""
    skips=$(grep -ac '^ *SKIP' "$log")
    [ "$skips" -gt 0 ] && note="$skips SKIP line(s) in log"
    if [ $rc -eq 0 ]; then
        results+=(PASS); status[$name]=PASS
    elif [ $advisory -eq 1 ]; then
        results+=(WARN); status[$name]=WARN; note="advisory, exit $rc${note:+; $note}"
    else
        results+=(FAIL); status[$name]=FAIL; any_fail=1; note="exit $rc${note:+; $note}"
    fi
    notes+=("$note")
    echo "    ${results[-1]} in $(fmt $dt)${note:+ ($note)}"
}

# The prerequisites (quick no-ops when up to date)
step hd-art                  -- $MAKE hd-art
step own-assets              -- $MAKE own-assets
# The quick tests: the replay checker's and build_port.py's own tests, the unit tests
step harness-selftest        -- tools/dk python3 tests/test_replay_harness.py -v
step build-port-selftest     -- python3 tests/test_build_port.py -v
step test                    -- $MAKE test
# The builds the rest needs
step port                    -- $MAKE port
step native                  -- $MAKE native
step web                     -- $MAKE web
# The desktop's tests with the install (each make target makes native again: at once)
step test-sound  --needs native -- $MAKE test-sound
step test-music  --needs native -- $MAKE test-music
step test-wonder --needs native -- $MAKE test-wonder
step test-extras --needs native -- $MAKE test-extras
step test-media  --needs native -- $MAKE test-media
step hd-portraits --needs port  -- sh harness/hd/hd-portraits.sh
# The browser version's tests (make web-test's, each its own step)
step fetch-browser           -- sh tools/fetch-toolchain.sh browser
step web-unit   --needs web,fetch-browser -- $NODE src/web/test/unit.mjs
step web-setup  --needs web,fetch-browser -- $NODE src/web/test/setup.mjs
step web-net    --needs web,fetch-browser -- $NODE src/web/test/net.mjs
step web-start  --needs web,fetch-browser -- $NODE src/web/test/start.mjs
step web-smoke  --needs web,fetch-browser -- $NODE src/web/test/smoke.mjs
step web-hd     --needs web,fetch-browser -- $NODE src/web/test/hd.mjs
step web-sound  --needs web,fetch-browser -- $NODE src/web/test/sound.mjs
step web-mobile --needs web,fetch-browser -- $NODE src/web/test/mobile.mjs
step web-perf   --advisory --needs web,fetch-browser -- $NODE src/web/test/perf.mjs
# The long ones last: the browser against the desktop, every scenario
# against the original's recordings, the two native builds against each other
step web-parity   --needs web,fetch-browser -- $NODE src/web/test/web-parity.mjs
step native-suite --needs port -- $MAKE native-suite EXE=OpenCiv2
step diff-suite   --needs port,native -- tools/dk sh harness/diff_suite.sh

summary
exit $any_fail
