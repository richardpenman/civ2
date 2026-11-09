#!/usr/bin/env python3
"""Exercise the real Load Game file list with filename and list keyboard focus."""
import argparse
import os
from pathlib import Path
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
    work = Path(tempfile.mkdtemp(prefix='file-dialog-scroll-', dir=ROOT / 'build'))
    game = work / 'game'
    game.mkdir()
    print('Results:', work, flush=True)
    allowed = {'.txt'}
    if args.original:
        allowed |= {'.gif', '.bmp', '.dll', '.exe'}
    for p in args.game.iterdir():
        if p.is_file() and p.suffix.lower() in allowed:
            shutil.copyfile(p, game / p.name)
    # Directory entries only: the replay cancels and never loads these files.
    for i in range(60):
        (game / f'Wheel-{i:02d}.sav').touch()
    shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
    (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
        f'\n[OpenCiv2]\nOriginalGraphics={int(args.original)}\nGraphicsDensity={args.density}\nHardwareGraphics={int(args.gpu)}\n')
    env = dict(os.environ)
    for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                'CIV2_TEST_EXTRAS', 'CIV2_SHOW', 'SDL_VIDEODRIVER', 'CIV2_BENCHMARK',
                'CIV2_TEST_SETTINGS', 'CIV2_TEST_GPU'):
        env.pop(key, None)
    env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_FULL='640x480', CIV2_TEST_FIXES='1',
               CIV2_REAL_FILEDLG='1', CIV2_SCRIPT=str(ROOT / 'harness/hd/file-dialog-scroll.txt'),
               CIV2_SHOT_DIR=str(work), CIV2_HOOK_LOG=str(work / 'hooks.log'),
               CIV2_SEED='7', SDL_AUDIODRIVER='dummy')
    if args.gpu:
        env['CIV2_TEST_GPU'] = '1'
    else:
        env['SDL_VIDEODRIVER'] = 'dummy'
    if os.name != 'nt':
        env['CIV2_DATA'] = str(ROOT / 'build/data')
    with (work / 'output.log').open('wb') as log:
        subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                       stderr=subprocess.STDOUT, timeout=60, check=True)
    hooks = (work / 'hooks.log').read_text()
    assert 'script finished' in hooks and 'Civ2FileDialog' in hooks
    frames = {}
    for name in ('top', 'wheel', 'top-restored', 'bottom', 'bottom-limit', 'home',
                 'selected', 'selected-wheel', 'selected-restored'):
        with Image.open(work / (name + '.bmp')) as im:
            frames[name] = im.convert('RGB')
    scale = frames['top'].width // 640

    def region(name, rect):
        return frames[name].crop(tuple(v * scale for v in rect)).tobytes()

    # List contents, excluding borders/scrollbar; the filename's first four
    # characters exclude the blinking caret when its edit has initial focus.
    listing = (116, 112, 500, 318)
    filename = (190, 342, 232, 357)
    selected_filename = (190, 340, 522, 359)
    assert region('wheel', listing) != region('top', listing), 'Wheel did not scroll with filename focus'
    assert region('bottom', listing) != region('wheel', listing)
    assert region('bottom-limit', listing) == region('bottom', listing), 'Scrolled past the last page'
    for name in ('top-restored', 'home'):
        assert region(name, listing) == region('top', listing), 'Top of list was not restored'
    for name in ('wheel', 'bottom', 'home'):
        assert region(name, filename) == region('top', filename), 'Wheel changed the filename'
    assert region('selected', filename) != region('top', filename), 'Click did not select a file'
    assert region('selected-wheel', listing) != region('selected', listing), 'Wheel did not scroll with list focus'
    assert region('selected-restored', listing) == region('selected', listing), 'Wheel changed the selection'
    for name in ('selected-wheel', 'selected-restored'):
        assert region(name, selected_filename) == region('selected', selected_filename), 'Wheel changed the selected filename'
    print('File dialog wheel scrolling, focus, limits, selection and filename preservation passed.', flush=True)


if __name__ == '__main__':
    main()
