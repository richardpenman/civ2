#!/bin/sh
# HD graphics (docs/HD-GRAPHICS.md, "Your own heralds"): harness/hd/hd-herald-own.txt
# (OpenCiv2's own Russian herald, where the local data/native/hd-extra has it) run for the man and the woman,
# with the CD's KINGS videos in the game folder (CIV2_KEEP_MEDIA=1) and
# without them, without the install's pictures (CIV2_NO_GRAPHICS=1, no
# KINGS), and with Classic graphics (no CIV2_TEST_HD: the original's herald
# and portrait, with KINGS); then a contact sheet of each run's room:
# run/hd-herald-own/G-V/ (G m or f; V kings, nokings, noinstall, classic),
# run/hd-herald-own/G-V.png.
# Usage (on the host): sh harness/hd/hd-herald-own.sh [GENDERS] [VARIANTS]
cd "$(dirname "$0")/../.." || exit 1
# (the Russians' herald is in the local staging folder data/native/hd-extra,
# not in the repository: without it there is nothing to show)
if ! grep -q '^RUS ' data/native/hd-extra/heralds.txt 2>/dev/null || [ ! -f data/native/hd-extra/herald-RUS.webp ]; then
    echo "SKIP: no own Russian herald (data/native/hd-extra/heralds.txt, herald-RUS.webp: the local staging folder, not in the repository)"
    exit 0
fi
genders=${1:-"m f"}
variants=${2:-"kings nokings noinstall classic"}
out=run/hd-herald-own
mkdir -p $out
shots=room,g0,g1,g2,g3,i0,i1,i2,d1,d2,p0,p1,p2,d4,a0,a1,a2,d7,e0,e1,e2,x0,x1,x2,x3
for g in $genders; do
    gender="key Return"; [ $g = f ] && gender="key Down\nkey Return"
    sed -e "s/^key Return *# GENDER.*/$gender/" harness/hd/hd-herald-own.txt > $out/$g.txt
    # (the man's game without the install: no threat first, its answer and shots left out)
    sed -e '/ THREAT$/,/^shot d2$/d' $out/$g.txt > $out/$g-noinstall.txt
    for v in $variants; do
        script=$out/$g.txt
        [ $v$g = noinstallm ] && script=$out/$g-noinstall.txt
        # (Classic graphics with the CD's Video: the opening video first, ended by a click)
        if [ $v = classic ]; then
            script=$out/$g-classic.txt
            sed -e '0,/^wait 8000$/s//wait 3000\nclick 20 400\nwait 5000/' $out/$g.txt > $script
        fi
        d=$out/$g-$v
        rm -rf $d
        case $v in
        kings) env="CIV2_KEEP_MEDIA=1 CIV2_TEST_HD=2" ;;
        nokings) env="CIV2_TEST_HD=2" ;;
        noinstall) env="CIV2_NO_GRAPHICS=1 CIV2_TEST_HD=2" ;;
        classic) env="CIV2_KEEP_MEDIA=1" ;;
        esac
        tools/dk sh -c "$env EXE=${EXE:-OpenCiv2} TIMEOUT=400 sh tools/native-run.sh /work/$script /work/$d" >/dev/null 2>&1
        echo "$g-$v: $(/usr/bin/grep -c 'own herald' $d/hook.log) own herald opened"
        tools/dk python3 harness/hd/hd-herald-own-sheet.py $d $shots $out/$g-$v.png
    done
done
