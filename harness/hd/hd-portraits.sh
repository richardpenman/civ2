#!/bin/sh
# HD graphics (docs/HD-GRAPHICS.md, Historical leaders' portraits): each
# tribe's own historical portrait (data/native/portraits, made by make
# own-assets) in the diplomacy room, for the Babylonians, the Russians and
# the Japanese, man and woman, with the install and without it
# (CIV2_NO_GRAPHICS=1); then the custom tribe's portrait chooser
# (portrait_dlg_show) of the same game, its 21 faces of that gender the
# historical portraits.  harness/hd/hd-portraits.txt made for each tribe and
# gender (as harness/hd/hd-talks-all.sh does), run with HD on.  Checked:
# hook.log names the portrait drawn for the player's leader
# ("hd: leader's portrait N: portrait/ID", ID = 220 + the tribe, + 21 for a
# woman) and the chooser's 21 faces; FAIL otherwise.  The shots:
# run/hd-portraits/V/T-G/{room,talk,faces}{,-hd}.png (V install or
# noinstall, T the tribe, G m or f) and a contact sheet of each variant's
# rooms, run/hd-portraits/V/contact.png (hd-talks-sheet.py).
# Usage (on the host): sh harness/hd/hd-portraits.sh [TRIBES] [GENDERS] [VARIANTS]
cd "$(dirname "$0")/../.." || exit 1
if [ ! -f data/native/portraits/leader-220.webp ]; then
    echo "SKIP: no historical portraits (data/native/portraits: make own-assets)"
    exit 0
fi
tribes=${1:-"1 7 15"}
genders=${2:-"m f"}
variants=${3:-"install noinstall"}
out=run/hd-portraits
mkdir -p $out
fail=0
for v in $variants; do
    env="CIV2_TEST_HD=2"; [ $v = noinstall ] && env="CIV2_NO_GRAPHICS=1 CIV2_TEST_HD=2"
    rm -rf $out/$v
    mkdir -p $out/$v
    for t in $tribes; do
        for g in $genders; do
            d=$out/$v/$t-$g
            leader=$((t % 7 + 1)); viewer=$((leader % 7 + 1))
            downs=""; i=0
            while [ $i -lt $t ]; do downs="$downs Down"; i=$((i + 1)); done
            gender="key Return"; [ $g = f ] && gender="key Down\nkey Return"
            tribe="wait 1"; [ -n "$downs" ] && tribe="key$downs"
            sed -e "s/^key Return *# GENDER.*/$gender/" -e "s/^key Down.*# TRIBE.*/$tribe/" \
                -e "s/^\(call [a-z_]* [0-9a-f]*\) 3 2\(.*\)# VIEWER LEADER/\1 $viewer $leader\2/" \
                -e "s/^\(call portrait_dlg_show [0-9a-f]*\) 2 .*# PLAYER/\1 $leader/" \
                harness/hd/hd-portraits.txt > $out/$v/$t-$g.txt
            tools/dk sh -c "$env EXE=${EXE:-OpenCiv2} TIMEOUT=300 \
                sh tools/native-run.sh /work/$out/$v/$t-$g.txt /work/$d" >/dev/null 2>&1
            id=$((220 + t)); [ $g = f ] && id=$((id + 21))
            p=$((id - 220))
            faces=$(/usr/bin/grep -o "portrait chooser: [0-9]* historical portraits ([a-z]*)" $d/hook.log | head -1)
            want_faces="portrait chooser: 21 historical portraits ($([ $g = f ] && echo women || echo men))"
            if /usr/bin/grep -q "^hd: leader's portrait $p: portrait/$id\$" $d/hook.log && [ "$faces" = "$want_faces" ]; then
                echo "PASS $v $t-$g: portrait/$id; $faces"
            else
                echo "FAIL $v $t-$g: wanted portrait/$id and $want_faces; hook.log: $(/usr/bin/grep -o "leader's portrait.*\|portrait chooser.*" $d/hook.log | tr '\n' ';')"
                fail=1
            fi
        done
    done
    tools/dk python3 harness/hd/hd-talks-sheet.py $out/$v | tail -1
done
exit $fail
