#!/usr/bin/env python3
"""Check that HD map redraws erase selection marks at several zooms and densities."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True, help='local game containing He_Auto.SAV')
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities', type=int, nargs='+', choices=range(1, 5), default=[1, 2, 4])
    ap.add_argument('--cpu', action='store_true', help='check CPU rendering with fixed zoom')
    ap.add_argument('--fixed', action='store_true', help='check GPU rendering with fixed zoom')
    args = ap.parse_args()
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt', 'he_auto.sav'):
        if name not in files:
            ap.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='map-redraw-', dir=ROOT / 'build'))
    print('Results:', work, flush=True)
    script = (ROOT / 'harness/hd/map-redraw.txt').read_text()
    if args.cpu or args.fixed:
        # Explicit zoom presets also exercise CPU, whose mouse wheel retains
        # its original meaning instead of controlling the smooth camera.
        script = script.replace('wheel_precise 400 400 0.5', 'call_sync menu_command 4e2803 0 802')
        script = script.replace('wheel 400 400 -4\n', 'call_sync menu_command 4e2803 0 800\n')
        script = script.replace('wheel 400 400 8', 'call_sync menu_command 4e2803 0 801')
        script = script.replace('wheel 400 400 -40', 'call_sync menu_command 4e2803 0 804')
    (work / 'script.txt').write_text(script)
    for density in args.densities:
        out = work / f'density-{density}'
        game = out / 'game'
        game.mkdir(parents=True)
        for p in files.values():
            if p.suffix.lower() == '.txt' or p.name.lower() == 'he_auto.sav':
                shutil.copyfile(p, game / p.name)
        shutil.copyfile(ROOT / 'harness/CIV2.DAT', game / 'CIV2.DAT')
        (game / 'CIV.INI').write_text((ROOT / 'harness/CIV.INI').read_text() +
            f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\n'
            f'HardwareGraphics={int(not args.cpu)}\nGpuMapCamera={int(not (args.cpu or args.fixed))}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                    'CIV2_SHOW', 'CIV2_TEST_EXTRAS', 'CIV2_TEST_SETTINGS', 'CIV2_BENCHMARK',
                    'CIV2_TEST_GPU', 'SDL_VIDEODRIVER'):
            env.pop(key, None)
        env.update(CIV2_MAP_REDRAW_VERIFY='1', CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_FIXES='1',
                   CIV2_TEST_FULL='1024x768', CIV2_SEED='7', SDL_AUDIODRIVER='dummy',
                   CIV2_SCRIPT=str(work / 'script.txt'), CIV2_SHOT_DIR=str(out),
                   CIV2_HOOK_LOG=str(out / 'hooks.log'))
        if args.cpu:
            env['SDL_VIDEODRIVER'] = 'dummy'
        else:
            env['CIV2_TEST_GPU'] = '1'
        if os.name != 'nt':
            env['CIV2_DATA'] = str(ROOT / 'build/data')
        with (out / 'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=90, check=True)
        hooks = (out / 'hooks.log').read_text()
        assert 'script finished' in hooks and 'no function' not in hooks, out
        assert hooks.count('map redraw verified:') == 6, out
        assert 'full redraw at zoom -7' in hooks, (out, 'minimum zoom was not exercised')
        if not args.cpu:
            assert 'HD compositor: SDL GPU' in hooks, out
        print(f'Density {density}: blink, moved selection and full redraw match clean terrain, including grid and minimum zoom.', flush=True)


if __name__ == '__main__':
    main()
