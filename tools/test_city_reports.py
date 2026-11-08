#!/usr/bin/env python3
"""Check HD report sprites across redraws and scrolling in an isolated game."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from PIL import Image, ImageChops

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True)
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--gpu', action='store_true')
    ap.add_argument('--extras', action='store_true', help='Use the sortable City Status report')
    ap.add_argument('--original', action='store_true')
    ap.add_argument('--density', type=int, choices=range(1, 5), default=2)
    args = ap.parse_args()
    work = Path(tempfile.mkdtemp(prefix='city-reports-', dir=ROOT / 'build'))
    game = work / 'game'
    game.mkdir()
    print('Results:', work, flush=True)
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'he_auto.sav'):
        if name not in files:
            ap.error(f'--game needs {name}')
    allowed = {'.txt'}
    if args.original:
        allowed |= {'.gif', '.bmp', '.dll', '.exe'}
    for name, p in files.items():
        if p.suffix.lower() in allowed or name == 'he_auto.sav':
            shutil.copyfile(p, game / p.name)
    shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
    (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
        f'\n[OpenCiv2]\nOriginalGraphics={int(args.original)}\nGraphicsDensity={args.density}\nHardwareGraphics={int(args.gpu)}\n')
    env = dict(os.environ)
    for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                'CIV2_TEST_EXTRAS', 'CIV2_SHOW', 'SDL_VIDEODRIVER', 'CIV2_BENCHMARK',
                'CIV2_TEST_SETTINGS', 'CIV2_TEST_GPU'):
        env.pop(key, None)
    env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_FULL='1024x768', CIV2_TEST_FIXES='1',
               CIV2_SCRIPT=str(ROOT / 'harness/hd/city-reports.txt'), CIV2_SHOT_DIR=str(work),
               CIV2_HOOK_LOG=str(work / 'hooks.log'), CIV2_RAND_LOG=str(work / 'rand.log'),
               CIV2_SEED='7', SDL_AUDIODRIVER='dummy')
    if args.extras:
        env['CIV2_TEST_EXTRAS'] = '31'
    if args.gpu:
        env['CIV2_TEST_GPU'] = '1'
    else:
        env['SDL_VIDEODRIVER'] = 'dummy'
    if os.name != 'nt':
        env['CIV2_DATA'] = str(ROOT / 'build/data')
    with (work / 'output.log').open('wb') as log:
        subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                       stderr=subprocess.STDOUT, timeout=90, check=True)
    hooks = (work / 'hooks.log').read_text()
    assert 'script finished' in hooks and 'no function' not in hooks
    if not args.original:
        assert 'original image loading disabled' in hooks
        assert ('HD compositor: SDL GPU' if args.gpu else 'HD compositor: CPU') in hooks
    frames = {}
    for name in ('cities', 'cities-scroll', 'cities-end', 'cities-reset', 'trade', 'trade-scroll'):
        with Image.open(work / (name + '.bmp')) as im:
            frames[name] = im.convert('RGB')
    scale = frames['cities'].width // 1024

    def crop(name, rect):
        return frames[name].crop(tuple(v * scale for v in rect))

    # Exclude controls and the map's blinking unit. Returning to the first row
    # must restore identical pixels; later rows must really change the contents.
    rect = (216, 269, 790, 576)
    assert ImageChops.difference(crop('cities', rect), crop('cities-reset', rect)).getbbox() is None
    for name in ('cities-scroll', 'cities-end'):
        assert ImageChops.difference(crop('cities', rect), crop(name, rect)).getbbox(), name
    assert ImageChops.difference(crop('trade', rect), crop('trade-scroll', rect)).getbbox()
    if not args.original:
        # Follow the same city from the final visible row to the penultimate
        # row. Its bottom must survive the following row's background paint.
        # Standard: Dover; sortable: Liverpool (staggered column also changes).
        x, y = (216, 533) if not args.extras else (280, 527)
        next_x = 216 if args.extras else x
        backgrounds = ((244, 238, 225), (215, 198, 161))
        for name, left, top in (('cities', x, y), ('cities-scroll', next_x, y - 24)):
            pixels = crop(name, (left + 8, top + 32, left + 56, top + 44))
            colors = (pixels.getpixel((px, py)) for py in range(pixels.height) for px in range(pixels.width))
            count = sum(all(max(abs(c[i] - bg[i]) for i in range(3)) > 25 for bg in backgrounds) for c in colors)
            assert count > 50 * scale * scale, f'{name}: city bottom was overpainted ({count} pixels)'
    print('Report scrolling, redraw restoration, and city sprite lower edges passed.', flush=True)


if __name__ == '__main__':
    main()
