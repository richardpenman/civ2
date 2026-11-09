#!/usr/bin/env python3
"""Exercise Civilopedia text scrolling through real input in an isolated game."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True)
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--gpu', action='store_true')
    ap.add_argument('--original', action='store_true')
    ap.add_argument('--density', type=int, choices=range(1, 5), default=2)
    args = ap.parse_args()
    work = Path(tempfile.mkdtemp(prefix='pedia-scroll-', dir=ROOT / 'build'))
    game = work / 'game'
    game.mkdir()
    print('Results:', work, flush=True)
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'describe.txt', 'he_auto.sav'):
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
    # Replace one description only in the disposable fixture. Numbered lines
    # wrap, extend beyond the viewport and keep screenshots easy to inspect.
    desc = game / files['describe.txt'].name
    body = ''.join(f'Scroll test line {i:02d}: Read every line of the game concept.\n' for i in range(1, 41))
    contents, count = re.subn(r'(@@Airbases\n).*?(?=\n@@)', lambda m: m[1] + body,
                             desc.read_text(encoding='latin1'), count=1, flags=re.S)
    assert count == 1, 'Airbases fixture entry not found'
    desc.write_text(contents, encoding='latin1')
    env = dict(os.environ)
    for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                'CIV2_TEST_EXTRAS', 'CIV2_SHOW', 'SDL_VIDEODRIVER', 'CIV2_BENCHMARK',
                'CIV2_TEST_SETTINGS', 'CIV2_TEST_GPU'):
        env.pop(key, None)
    env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_FULL='640x480', CIV2_TEST_FIXES='1',
               CIV2_SCRIPT=str(ROOT / 'harness/hd/pedia-scroll.txt'), CIV2_SHOT_DIR=str(work),
               CIV2_HOOK_LOG=str(work / 'hooks.log'), CIV2_SEED='7', SDL_AUDIODRIVER='dummy')
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
    # The text region excludes the caret's first column, scrollbar, and map.
    # Pixel comparisons verify that the visible text really moves, not just
    # the thumb, and that limits/reset restore precisely the same content.
    frames = {}
    for name in ('top', 'wheel', 'wheel-top', 'page', 'page-top', 'arrow', 'arrow-top',
                 'drag', 'bottom', 'bottom-limit', 'home', 'reset'):
        with Image.open(work / (name + '.bmp')) as im:
            scale = im.width // 640
            assert im.size == (640 * scale, 480 * scale)
            frames[name] = im.convert('RGB').crop(tuple(v * scale for v in (30, 72, 610, 365))).tobytes()
    for name in ('wheel', 'page', 'arrow', 'drag', 'bottom'):
        assert frames[name] != frames['top'], f'{name}: text did not scroll'
    for name in ('wheel-top', 'page-top', 'arrow-top', 'home', 'reset'):
        assert frames[name] == frames['top'], f'{name}: top of text was not restored'
    assert frames['bottom-limit'] == frames['bottom'], 'Scrolled past the last page'
    print('Wheel, arrows, thumb dragging, keyboard paging, limits and reopening passed.', flush=True)


if __name__ == '__main__':
    main()
