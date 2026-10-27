#!/bin/sh
# Compare two native builds on the same scripts: a difference is a bug of one
# of them, with no recording of the original needed (the 32-bit build has the
# original's x86 behaviour; the portable build is the one that differs most).
# Also fails on a crash, a timeout or a SUSPICIOUS request (CIV2_STRICT).
# Runs inside the civ2-dev container:
#   tools/dk sh harness/diff_suite.sh [SCRIPT...]   (default: harness/diffscripts: autoplay, monkeys)
# A=, B= select the builds (default civ2-linux32 and OpenCiv2).
cd "$(dirname "$0")/.." || exit 1
mkdir -p run || exit 1
A=${A:-civ2-linux32}
B=${B:-OpenCiv2}
if [ "$#" -eq 0 ]; then
    set -- harness/diffscripts/*.txt
fi
fail=0
echo "comparing build/$A with build/$B"
for s do
    n=$(basename "$s" .txt)
    scenario_failed=0
    for side in a b; do
        exe=$A; [ "$side" = b ] && exe=$B
        rm -rf -- "run/diff$side-$n"
        EXE=$exe CIV2_STRICT=1 TIMEOUT=${TIMEOUT:-5400} sh tools/native-run.sh "$PWD/$s" "$PWD/run/diff$side-$n" \
            > "run/diff$side-$n.log" 2>&1
        status=$?
        if [ "$status" -ne 0 ]; then
            echo "FAIL $n: $exe exited with status $status (124: timeout)"
            scenario_failed=1
            continue
        fi
        if ! python3 - "run/diff$side-$n" >> "run/diff$side-$n.log" 2>&1 <<'PY'
import glob
import os
import sys
from PIL import Image
for f in glob.glob(os.path.join(sys.argv[1], '*.bmp')):
    Image.open(f).save(f[:-4] + '.png')
PY
        then
            echo "FAIL $n: $exe screenshot conversion failed (see run/diff$side-$n.log)"
            scenario_failed=1
            continue
        fi
        if grep -q SUSPICIOUS "run/diff$side-$n/hook.log" 2>/dev/null; then
            echo "FAIL $n: $exe:"; grep SUSPICIOUS "run/diff$side-$n/hook.log" | head -3 | sed 's/^/    /'
            scenario_failed=1
        fi
    done
    if [ "$scenario_failed" -ne 0 ]; then
        fail=1
        continue
    fi
    if python3 harness/compare.py "run/diffa-$n" "run/diffb-$n" --report "run/report-diff-$n" > "run/compare-diff-$n.txt" 2>&1; then
        echo "SAME $n ($(tail -1 "run/compare-diff-$n.txt"))"
    else
        echo "DIFF $n"; grep -v ": identical" "run/compare-diff-$n.txt" | head -12 | sed 's/^/    /'
        fail=1
    fi
done
exit $fail
