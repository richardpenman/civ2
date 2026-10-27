#!/bin/sh
# Run every scenario on the native build and compare against the original's
# recordings (run/ref-NAME, made by harness/run.py: make ref).
# Runs inside the civ2-dev container:  tools/dk sh harness/native_suite.sh [SCRIPT...]
# EXE selects the executable in build/ (default civ2-linux32).
cd "$(dirname "$0")/.." || exit 1
mkdir -p run || exit 1
export EXE=${EXE:-civ2-linux32}
if [ "$#" -eq 0 ]; then
    set -- harness/scripts/*.txt
fi
fail=0
echo "testing build/$EXE"
for s do
    n=$(basename "$s" .txt)
    rm -rf -- "run/native-$n"
    TIMEOUT=${TIMEOUT:-900} sh tools/native-run.sh "$PWD/$s" "$PWD/run/native-$n" > "run/native-$n.log" 2>&1
    status=$?
    if [ "$status" -ne 0 ]; then
        echo "FAIL $n (runner exited $status; see run/native-$n.log)"
        fail=1
        continue
    fi
    if ! python3 - "run/native-$n" >> "run/native-$n.log" 2>&1 <<'PY'
import glob
import os
import sys
from PIL import Image
for f in glob.glob(os.path.join(sys.argv[1], '*.bmp')):
    Image.open(f).save(f[:-4] + '.png')
PY
    then
        echo "FAIL $n (screenshot conversion failed; see run/native-$n.log)"
        fail=1
        continue
    fi
    if grep -q "not laid out as in the original" "run/native-$n.log"; then
        echo "FAIL $n"; grep "not laid out" "run/native-$n.log" | sed 's/^/    /'
        fail=1
    elif python3 harness/compare.py "run/ref-$n" "run/native-$n" --report "run/report-native-$n" > "run/compare-native-$n.txt" 2>&1; then
        echo "PASS $n ($(tail -1 "run/compare-native-$n.txt"))"
    else
        echo "FAIL $n"; grep -v ": identical" "run/compare-native-$n.txt" | sed 's/^/    /'
        fail=1
    fi
done
exit $fail
