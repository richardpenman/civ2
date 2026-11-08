#!/usr/bin/env python3
"""Check active-unit tracking against the displayed smooth-camera viewport."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True, help='local game containing He_Auto.SAV')
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities', type=int, nargs='+', choices=range(1, 5), default=[1, 4])
    args = ap.parse_args()
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'he_auto.sav'):
        if name not in files:
            ap.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='camera-focus-', dir=ROOT / 'build'))
    print('Results:', work, flush=True)
    for density in args.densities:
        out = work / f'density-{density}'
        game = out / 'game'
        game.mkdir(parents=True)
        for p in files.values():
            if p.suffix.lower() == '.txt' or p.name.lower() == 'he_auto.sav':
                shutil.copyfile(p, game / p.name)
        shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
        (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
            f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\nHardwareGraphics=1\nGpuMapCamera=1\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                    'CIV2_SHOW', 'CIV2_TEST_EXTRAS', 'CIV2_TEST_SETTINGS', 'CIV2_BENCHMARK', 'SDL_VIDEODRIVER'):
            env.pop(key, None)
        env.update(CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_GPU='1', CIV2_TEST_FIXES='1',
                   CIV2_TEST_FULL='1024x768', CIV2_SEED='7', SDL_AUDIODRIVER='dummy',
                   CIV2_SCRIPT=str(ROOT / 'harness/hd/camera-focus.txt'), CIV2_SHOT_DIR=str(out),
                   CIV2_HOOK_LOG=str(out / 'hooks.log'))
        if os.name != 'nt':
            env['CIV2_DATA'] = str(ROOT / 'build/data')
        with (out / 'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=90, check=True)
        hooks = (out / 'hooks.log').read_text()
        assert 'script finished' in hooks and 'no function' not in hooks, out
        assert 'HD compositor: SDL GPU' in hooks, out
        world = [tuple(map(float, p)) for p in re.findall(r'camera world: (-?[\d.]+),(-?[\d.]+)', hooks)]
        scales = re.findall(r'camera probe .* scale=([\d.]+)', hooks)
        assert len(world) == len(scales) == 8, (out, world, scales)
        assert all(float(s) == 2 for s in scales), (out, 'tracking changed zoom', scales)
        assert world[0][0] - 124 > 6, (out, 'regression tile was not offscreen', world[0])
        for i, target in ((1, (124, 68)), (5, (138, 68)), (6, (132, 44)), (7, (132, 90))):
            assert max(abs(world[i][j] - target[j]) for j in (0, 1)) < 2, (out, 'unit not centered', i, world[i], target)
        assert world[1] == world[2] and world[3] == world[4], (out, 'visible unit reset camera', world)
        assert world[2] != world[3], (out, 'pan was not exercised')
        print(f'Density {density}: offscreen tracking centers all four directions, preserves zoom and leaves visible units stationary.', flush=True)


if __name__ == '__main__':
    main()
