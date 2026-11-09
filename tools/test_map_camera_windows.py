#!/usr/bin/env python3
"""Compare foreground city windows/popups with the GPU camera on and off."""
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
    ap.add_argument('--density', type=int, choices=range(1, 5), default=2)
    args = ap.parse_args()
    work = Path(tempfile.mkdtemp(prefix='camera-windows-', dir=ROOT / 'build'))
    print('Results:', work, flush=True)
    shots = ('city', 'city-idle', 'city-info', 'city-popup', 'city-production', 'city-restored')
    reference = {}
    for camera in (0, 1):
        out = work / f'camera-{camera}'
        game = out / 'game'
        game.mkdir(parents=True)
        for p in args.game.iterdir():
            if p.is_file() and p.suffix.lower() in ('.txt', '.sav'):
                shutil.copyfile(p, game / p.name)
        shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
        (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
            f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={args.density}\nHardwareGraphics=1\nGpuMapCamera={camera}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                    'CIV2_BENCHMARK', 'CIV2_SHOW', 'SDL_VIDEODRIVER', 'CIV2_TEST_SETTINGS', 'CIV2_TEST_EXTRAS'):
            env.pop(key, None)
        env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_GPU='1', CIV2_TEST_FIXES='1',
                   CIV2_TEST_FULL='1024x768', CIV2_SCRIPT=str(ROOT / 'harness/hd/camera-windows.txt'),
                   CIV2_SHOT_DIR=str(out), CIV2_HOOK_LOG=str(out / 'hooks.log'),
                   SDL_AUDIODRIVER='dummy', CIV2_SEED='7')
        if os.name != 'nt':
            env['CIV2_DATA'] = str(ROOT / 'build/data')
        with (out / 'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=120, check=True)
        hooks = (out / 'hooks.log').read_text()
        assert 'script finished' in hooks and 'HD compositor: SDL GPU' in hooks and 'no function' not in hooks, out
        assert 'City of Brighton' in hooks or 'Atlas crop: crop/file/city/' in hooks, 'City window did not open'
        # The native city's contents (including its caption and nested popups)
        # are independent of camera scale. Exclude its outer border and the map.
        box = tuple(v * args.density for v in (28, 68, 999, 744))
        frames = {}
        for shot in shots:
            with Image.open(out / (shot + '.bmp')) as im:
                assert im.size == (1024 * args.density, 768 * args.density)
                frames[shot] = im.convert('RGB').crop(box)
            if camera:
                diff = ImageChops.difference(reference[shot], frames[shot])
                assert not diff.getbbox(), (out, shot, 'Camera changed foreground pixels', diff.getbbox())
        assert ImageChops.difference(frames['city-info'], frames['city-popup']).getbbox(), 'Popup did not open'
        assert ImageChops.difference(frames['city-restored'], frames['city-production']).getbbox(), 'Production dialog did not open'
        reference = frames
        print(f'Camera={camera}: city, idle, info and modal dialogs rendered.', flush=True)
    print('Foreground pixels are identical with the camera on and off.', flush=True)


if __name__ == '__main__':
    main()
