#!/usr/bin/env python3
"""Compare HD selection and movement redraws with complete map renders."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', type=Path, required=True, help='local game containing rules/text and, by default, He_Auto.SAV')
    ap.add_argument('--exe', type=Path, default=ROOT / ('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities', type=int, nargs='+', choices=range(1, 5), default=[1, 2, 4])
    ap.add_argument('--cpu', action='store_true', help='check CPU rendering with fixed zoom')
    ap.add_argument('--fixed', action='store_true', help='check GPU rendering with fixed zoom')
    ap.add_argument('--new-game', action='store_true', help='move a settler on a new unexplored map using virtual time and fixed zoom')
    ap.add_argument('--all-zooms', action='store_true', help='check every fixed zoom from -7 through +8')
    args = ap.parse_args()
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt', 'rules.txt', 'labels.txt') + (() if args.new_game else ('he_auto.sav',)):
        if name not in files:
            ap.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='map-redraw-', dir=ROOT / 'build'))
    print('Results:', work, flush=True)
    script = (ROOT / 'harness/hd/map-redraw.txt').read_text()
    if args.new_game:
        script = ((ROOT / 'harness/scripts/newgame.txt').read_text() +
                  '\nkey Right\nwait 15000\nkey Return # dismiss the settler tutorial\n'
                  'wait 5000\nshot moved-unit\n' +
                  script[script.index('call_sync map_redraw_probe 0'):])
    fixed_zoom = args.cpu or args.fixed or args.new_game or args.all_zooms
    if fixed_zoom:
        # Explicit zoom presets also exercise CPU, whose mouse wheel retains
        # its original meaning instead of controlling the smooth camera.
        script = script.replace('wheel_precise 400 400 0.5', 'call_sync menu_command 4e2803 0 802')
        script = script.replace('wheel 400 400 -4\n', 'call_sync menu_command 4e2803 0 800\n')
        script = script.replace('wheel 400 400 8', 'call_sync menu_command 4e2803 0 801')
        script = script.replace('wheel 400 400 -40', 'call_sync menu_command 4e2803 0 804')
    if args.all_zooms:
        script = script[:script.index('call_sync map_redraw_probe 0')] + 'call_sync menu_command 4e2803 0 804\n'
        script += 'call_sync map_redraw_probe 0\n'
        script += 'call_sync menu_command 4e2803 0 784\ncall_sync map_redraw_probe 0\n' * 15
        script += 'quit\n'
    probes = 16 if args.all_zooms else 6
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
            f'HardwareGraphics={int(not args.cpu)}\nGpuMapCamera={int(not fixed_zoom)}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC', 'CIV2_DATA', 'CIV2_GAME', 'CIV2_TEST_NATIVE', 'CIV2_NORMAL_DESKTOP',
                    'CIV2_SHOW', 'CIV2_TEST_EXTRAS', 'CIV2_TEST_SETTINGS', 'CIV2_BENCHMARK',
                    'CIV2_TEST_GPU', 'SDL_VIDEODRIVER'):
            env.pop(key, None)
        env.update(CIV2_MAP_REDRAW_VERIFY='1', CIV2_REALTIME='1', CIV2_TEST_HD='1', CIV2_TEST_FIXES='1',
                   CIV2_TEST_FULL='1024x768', CIV2_SEED='7', SDL_AUDIODRIVER='dummy',
                   CIV2_SCRIPT=str(work / 'script.txt'), CIV2_SHOT_DIR=str(out),
                   CIV2_HOOK_LOG=str(out / 'hooks.log'))
        if args.new_game:
            # World generation uses a long virtual wait in the shared replay.
            env.pop('CIV2_REALTIME', None)
        if args.cpu:
            env['SDL_VIDEODRIVER'] = 'dummy'
        else:
            env['CIV2_TEST_GPU'] = '1'
        if os.name != 'nt':
            env['CIV2_DATA'] = str(ROOT / 'build/data')
        with (out / 'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()), str(game)], env=env, stdout=log,
                           stderr=subprocess.STDOUT, timeout=240 if args.all_zooms else 90, check=True)
        hooks = (out / 'hooks.log').read_text()
        assert 'script finished' in hooks and 'no function' not in hooks, out
        assert hooks.count('map redraw verified:') == probes, out
        assert hooks.count('movement radii 1-3') == probes, out
        assert hooks.count('unit animation verified:') == probes, out
        assert 'full redraw at zoom -7' in hooks, (out, 'minimum zoom was not exercised')
        if args.all_zooms:
            for zoom in range(-7, 9):
                assert f'full redraw at zoom {zoom}\n' in hooks, (out, 'missing zoom', zoom)
        if not args.cpu:
            assert 'HD compositor: SDL GPU' in hooks, out
        print(f'Density {density}: blink, selection, movement radii and single/stacked trireme animations leave clean backgrounds at {probes} zoom/grid settings.', flush=True)


if __name__ == '__main__':
    main()
