#!/usr/bin/env python3
"""Replay sleeping units and grey transitions without original graphics."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--exe', type=Path, default=ROOT/'build/OpenCiv2.exe' if os.name == 'nt' else ROOT/'build/OpenCiv2')
    parser.add_argument('--gpu', action='store_true', help='exercise a hidden SDL hardware window')
    args = parser.parse_args()
    work = Path(tempfile.mkdtemp(prefix='hd-effects-', dir=ROOT/'build'))
    print('Results:', work, flush=True)
    game = work/'game';game.mkdir()
    for p in args.game.iterdir():
        if p.is_file() and p.suffix.lower() == '.txt':
            shutil.copyfile(p, game/p.name)
    shutil.copyfile(ROOT/'harness/CIV2.DAT', game/'CIV2.DAT')
    (game/'CIV.INI').write_text((ROOT/'harness/CIV.INI').read_text()+
                              '\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity=2\n')
    script = work/'effects.txt'
    script.write_text((ROOT/'harness/scripts/newgame.txt').read_text()+
                      (ROOT/'harness/hd/effects.txt').read_text())
    env = dict(os.environ)
    for key in ('CIV2_DATA', 'CIV2_GAME', 'CIV2_CLASSIC', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP','CIV2_TEST_GPU','CIV2_SHOW'):
        env.pop(key, None)
    env.update(CIV2_TEST_HD='1', CIV2_TEST_EXTRAS='41', CIV2_TEST_FIXES='1',
               CIV2_TEST_FULL='1024x768', CIV2_SEED='7', CIV2_SCRIPT=str(script),
               CIV2_SHOT_DIR=str(work), CIV2_HOOK_LOG=str(work/'hooks.log'),
               SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
    if os.name != 'nt':env['CIV2_DATA'] = str(ROOT/'build/data')
    if args.gpu:
        env.pop('SDL_VIDEODRIVER',None);env['CIV2_TEST_GPU']='1'
    with (work/'output.log').open('wb') as log:
        subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                       stderr=subprocess.STDOUT, timeout=120, check=True)
    hooks = (work/'hooks.log').read_text()
    assert 'script finished' in hooks and 'no function' not in hooks
    assert 'original image loading disabled' in hooks
    if args.gpu:assert 'HD compositor: SDL GPU' in hooks, 'GPU unavailable'
    with Image.open(work/'sleeping.bmp') as im:
        # The default seed-7 settlers' map position. Detailed greyscale must
        # retain more shades than the old small indexed-grey ramp could hold.
        colors = im.convert('RGB').crop((600,700,1000,1050)).getcolors(400*350)
        greys = {r for n, (r,g,b) in colors if r == g == b and n > 1}
        assert len(greys) > 64, ('sentry lost HD detail', len(greys))
    for name, grey in [('grey-backdrop', True), ('restored', False)]:
        with Image.open(work/(name+'.bmp')) as im:
            colors = im.convert('RGB').crop((20,200,1500,1400)).getcolors(1480*1200)
            colored = sum(n for n, (r,g,b) in colors if max(r,g,b)-min(r,g,b) > 3)
            if grey:assert colored < 100, ('backdrop not grey', colored)
            else:assert colored > 10000, ('fade-in did not restore color', colored)
    pack = (args.exe.resolve().parent/'data/atlas-hd.pack').read_bytes()
    def color(i):return tuple(pack[12+i*3:15+i*3])
    r,g,b = color(42);assert g > r and g > b, 'healthy bar is not green'
    r,g,b = color(122);assert r > 180 and g > 140 and b < 110, 'damaged bar is not yellow'
    r,g,b = color(106);assert r > 180 and g < 130 and b < 130, 'critical bar is not red'
    shadow = color(15);assert max(shadow) < 50 and max(shadow)-min(shadow) < 20
    sentry = color(23);assert max(sentry)-min(sentry) < 10 and 80 < min(sentry) < 180
    print('Detailed grey sentries, grey backdrop/fade restoration and unit-status colors passed.')


if __name__ == '__main__':
    main()
