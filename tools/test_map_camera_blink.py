#!/usr/bin/env python3
"""Check menu input and zoom recovery with a blinking unit at minimum GPU zoom."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from PIL import Image, ImageChops, ImageStat

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True, help='local game containing vi_a160.sav')
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities', type=int, nargs='+', choices=range(1, 5), default=[1, 2, 4])
    args = ap.parse_args()
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'vi_a160.sav'):
        if name not in files:
            ap.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='map-camera-blink-', dir=ROOT / 'build'))
    print('Results:', work, flush=True)
    for density in args.densities:
        out = work / f'density-{density}'
        game = out / 'game'
        game.mkdir(parents=True)
        for p in files.values():
            if p.suffix.lower() == '.txt' or p.name.lower() == 'vi_a160.sav':
                shutil.copyfile(p, game / p.name)
        shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
        (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
            f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\nHardwareGraphics=1\nGpuMapCamera=1\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                    'CIV2_SHOW', 'CIV2_TEST_EXTRAS', 'CIV2_TEST_SETTINGS', 'CIV2_BENCHMARK'):
            env.pop(key, None)
        env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_GPU='1', CIV2_TEST_FIXES='1',
                   CIV2_TEST_FULL='1024x768', CIV2_SEED='7', SDL_AUDIODRIVER='dummy',
                   CIV2_SCRIPT=str(ROOT / 'harness/hd/camera-blink.txt'),
                   CIV2_SHOT_DIR=str(out), CIV2_HOOK_LOG=str(out / 'hooks.log'))
        if os.name != 'nt':
            env['CIV2_DATA'] = str(ROOT / 'build/data')
        with (out / 'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=60, check=True)
        hooks = (out / 'hooks.log').read_text()
        assert 'script finished' in hooks and 'HD compositor: SDL GPU' in hooks, out
        probes = re.findall(r'camera probe .*?result=(\d+).*?scale=([\d.]+)', hooks)
        assert len(probes) == 3 and all(p[0] == '0' for p in probes), (out, probes)
        assert float(probes[1][1]) == .125, (out, 'minimum zoom stalled', probes)
        assert float(probes[2][1]) > .2, (out, 'zoom input stopped responding', probes)
        box = tuple(v * density for v in (0, 40, 260, 210))
        with Image.open(out / 'minimum.bmp') as a, Image.open(out / 'menu.bmp') as b:
            diff = ImageChops.difference(a.convert('RGB').crop(box), b.convert('RGB').crop(box))
            assert sum(ImageStat.Stat(diff).mean) / 3 > 10, (out, 'menu did not open')
        print(f'Density {density}: minimum zoom, menu click, Escape and zoom recovery passed.', flush=True)


if __name__ == '__main__':
    main()
