#!/bin/sh
# HD graphics (docs/HD-GRAPHICS.md): the diplomacy room with every tribe's
# leader, male and female, each from a new game: harness/hd/hd-talks-new.txt
# (the Vikings' male leader) made for each tribe and gender, run with HD on,
# then a contact sheet of the portraits, HD beside the original's frame:
# run/hd-talks-all/contact.png.
# Usage (on the host): sh harness/hd/hd-talks-all.sh [TRIBES] [GENDERS]
#   e.g. sh harness/hd/hd-talks-all.sh "8 16" "m f"; default all 21, m f.
# The shots: run/hd-talks-all/T-G/{room,talk}{,-hd}.bmp (T the tribe 0 to
# 20, G m or f).
cd "$(dirname "$0")/../.." || exit 1
tribes=${1:-"0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"}
genders=${2:-"m f"}
out=run/hd-talks-all
mkdir -p $out
for t in $tribes; do
    for g in $genders; do
        d=$out/$t-$g
        leader=$((t % 7 + 1)); viewer=$((leader % 7 + 1))
        downs=""; i=0
        while [ $i -lt $t ]; do downs="$downs Down"; i=$((i + 1)); done
        gender="key Return"; [ $g = f ] && gender="key Down\nkey Return"
        tribe="wait 1"; [ -n "$downs" ] && tribe="key$downs"
        sed -e "s/^key Return *# GENDER.*/$gender/" -e "s/^key Down.*# TRIBE.*/$tribe/" \
            -e "s/^\(call [a-z_]* [0-9a-f]*\) 4 3\(.*\)# VIEWER LEADER/\1 $viewer $leader\2/" \
            harness/hd/hd-talks-new.txt > $out/$t-$g.txt
        rm -rf $d
        tools/dk sh -c "CIV2_TEST_HD=2 CIV2_HD_PACK=/work/data/native/hd/atlas-hd.pack EXE=OpenCiv2 TIMEOUT=300 \
            sh tools/native-run.sh /work/$out/$t-$g.txt /work/$d" >/dev/null 2>&1
        echo "$t-$g: $(/usr/bin/grep -o 'hrld[A-Z]*' $d/calls.log | tr '\n' ' ')"
    done
done
tools/dk python3 harness/hd/hd-talks-sheet.py $out
