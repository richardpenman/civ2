#!/usr/bin/env python3
"""Run the built game in isolated copies, never modifying an installed game."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from PIL import Image, ImageChops

ROOT = Path(__file__).resolve().parents[1]


def check_unit_shields(path):
    # The seed-7 default new game starts with Roman settlers. At density 2 the
    # shield must contain their ivory faction color in both the map and status
    # pane. A dark, untinted replacement mask used to pass the general image
    # checks. Keep the boxes clear of the header/text and check more than one pixel.
    with Image.open(path) as im:
        assert im.size == (2048, 1536)
        for name, box in (('map', (0, 560, 1536, 1150)),
                          ('status', (1536, 640, 1720, 800))):
            colors = dict((rgb, n) for n, rgb in im.crop(box).convert('RGB').getcolors(2048*1536))
            assert colors.get((238, 228, 191), 0) > 100, f'{name}: unit shield lacks faction color'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--exe', type=Path, default=ROOT/'build/OpenCiv2.exe' if os.name == 'nt' else ROOT/'build/OpenCiv2')
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--gpu', action='store_true', help='exercise a hidden SDL hardware window')
    args = parser.parse_args()
    ROOT.joinpath('build').mkdir(exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='hd-game-test-', dir=ROOT/'build'))
    print('Results:', work, flush=True)
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'he_auto.sav'):
        if name not in files:
            parser.error(f'--game needs {name}')

    def run(name, exe, script, original, hardware=True):
        out = work/name;out.mkdir()
        game = out/'game';game.mkdir()
        allowed = {'.txt', '.sav', '.dat'}
        if original:allowed |= {'.gif', '.bmp', '.dll', '.exe'}
        for p in files.values():
            if p.suffix.lower() in allowed:shutil.copyfile(p, game/p.name)
        shutil.copyfile(ROOT/'harness/CIV2.DAT', game/'CIV2.DAT')
        ini = (ROOT/'harness/CIV.INI').read_text()
        (game/'CIV.INI').write_text(ini+f'\n[OpenCiv2]\nOriginalGraphics={int(original)}\nGraphicsDensity=2\nHardwareGraphics={int(hardware)}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE',
                    'CIV2_TEST_FULL', 'CIV2_NORMAL_DESKTOP', 'CIV2_TEST_EXTRAS', 'CIV2_TEST_FIXES','CIV2_TEST_GPU','CIV2_SHOW'):
            env.pop(key, None)
        env.update(CIV2_TEST_HD='1', CIV2_SCRIPT=str(script), CIV2_SHOT_DIR=str(out),
                   CIV2_HOOK_LOG=str(out/'hooks.log'), CIV2_RAND_LOG=str(out/'rand.log'),
                   SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy', CIV2_SEED='7')
        if os.name != 'nt':env['CIV2_DATA'] = str(ROOT/'build/data')
        if args.gpu:
            env.pop('SDL_VIDEODRIVER',None);env['CIV2_TEST_GPU']='1'
        if not original:
            env['CIV2_TEST_FULL'] = '1024x768'
            env['CIV2_TEST_FIXES'] = '1'
        with (out/'output.log').open('wb') as log:
            subprocess.run([str(exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=120, check=True)
        assert 'script finished' in (out/'hooks.log').read_text(), out
        if args.gpu and not original:
            expected='HD compositor: SDL GPU' if hardware else 'HD compositor: CPU'
            assert expected in (out/'hooks.log').read_text(), (out,'wrong compositor')
        return out

    # On Windows exercise extraction from the executable in a fresh directory.
    exe = args.exe
    if os.name == 'nt':
        bundle = work/'bundle';bundle.mkdir();exe = bundle/'OpenCiv2.exe'
        shutil.copyfile(args.exe, exe)
    hd = run('hd', exe, ROOT/'harness/hd/windows.txt', False)
    for name in ('map', 'cities', 'panorama', 'diplomacy', 'spacecraft', 'throne', 'throne-choices', 'throne-improved'):
        with Image.open(hd/(name+'.bmp')) as im:
            assert im.size == (2048, 1536), (name, im.size)
            assert len(im.convert('RGB').getcolors(im.width*im.height)) > 32, name
    assert 'original image loading disabled' in (hd/'hooks.log').read_text()
    city_script = work/'found-city.txt'
    city_script.write_text((ROOT/'harness/scripts/newgame.txt').read_text() +
                          (ROOT/'harness/hd/found-city.txt').read_text())
    city = run('new-city', exe, city_script, False)
    check_unit_shields(city/'12-map.bmp')
    with Image.open(city/'city.bmp') as im:
        assert im.size == (2048, 1536)
    assert 'CreateFont(Arial, -13' in (city/'hooks.log').read_text(), 'Stockpile meters were not drawn'
    print('HD windows rendered without original image files.', flush=True)
    if args.gpu:
        run('cpu-fallback',exe,ROOT/'harness/hd/windows.txt',False,hardware=False)
        print('HardwareGraphics=0 rendered HD windows through the CPU fallback.',flush=True)
    if args.baseline:
        # Use only calls present on main so the same replay works in both builds.
        script = work/'compare.txt'
        script.write_text((ROOT/'harness/hd/windows.txt').read_text().split('call city_status_show')[0] +
                          'call cityview_open 454260 0\nwait 3000\nshot panorama\nkey Escape\nwait 2000\n' +
                          'call intel_report_show 43060b 2 0\nwait 3000\nshot intelligence\nkey Escape\nwait 2000\n' +
                          'call spaceship_wnd_run 4d0208 1\nwait 3000\nshot spacecraft\n')
        baseline = run('baseline', args.baseline, script, True)
        classic = run('original', exe, script, True)
        for f in baseline.glob('*.bmp'):
            with Image.open(f) as a, Image.open(classic/f.name) as b:
                assert a.size == b.size and not ImageChops.difference(a.convert('RGB'), b.convert('RGB')).getbbox(), f.name
        assert (baseline/'rand.log').read_bytes() == (classic/'rand.log').read_bytes(), 'RNG changed'
        assert 'Atlas HD:' not in (classic/'hooks.log').read_text()
        print('Original-mode pixels and RNG match the baseline.')


if __name__ == '__main__':
    main()
