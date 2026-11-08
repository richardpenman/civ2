#!/usr/bin/env python3
"""Measure map redraws and first zoom transitions using real elapsed time."""
import argparse
import json
import math
import os
from pathlib import Path
import re
import shutil
import statistics
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--exe', type=Path, default=ROOT/('build/OpenCiv2.exe' if os.name == 'nt' else 'build/OpenCiv2'))
    parser.add_argument('--densities', type=int, nargs='+', choices=range(1,5), default=[1,2,3,4])
    parser.add_argument('--include-original', action='store_true')
    parser.add_argument('--gpu', action='store_true',help='use a hidden hardware window and fence GPU completion')
    parser.add_argument('--samples',type=int,default=21,help='warm redraw samples per zoom (default: 21)')
    parser.add_argument('--frame',action='store_true',help='include desktop/window composition before the completion fence')
    parser.add_argument('--realtime',action='store_true',help='use live message-clock timing instead of accelerated replay')
    parser.add_argument('--no-atlas',action='store_true',help='disable sprite atlases to measure the private-texture fallback')
    args = parser.parse_args()
    if args.samples<3:parser.error('--samples must be at least 3')
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt','rules.txt','labels.txt','he_auto.sav'):
        if name not in files: parser.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='hd-benchmark-',dir=ROOT/'build'))
    print('Results:',work,flush=True)
    script=work/'benchmark.txt'
    redraw='call_sync map_redraw_all_views 47cf9e 6 1\n'
    script.write_text((ROOT/'harness/hd/benchmark.txt').read_text().replace(redraw*4,redraw*(args.samples+1)))
    (work/'configuration.json').write_text(json.dumps(dict(exe=str(args.exe.resolve()),gpu=args.gpu,
        frame=args.frame,realtime=args.realtime,samples=args.samples,atlas_disabled=args.no_atlas),indent=2))
    results = {}
    for mode in (['original'] if args.include_original else [])+args.densities:
        out = work/str(mode);out.mkdir()
        game = out/'game';game.mkdir()
        for p in files.values():
            if p.suffix.lower() in ('.txt','.sav','.dat') or (mode == 'original' and p.suffix.lower() in ('.gif','.bmp','.dll','.exe')):
                shutil.copyfile(p,game/p.name)
        shutil.copyfile(ROOT/'harness/CIV2.DAT',game/'CIV2.DAT')
        (game/'CIV.INI').write_text((ROOT/'harness/CIV.INI').read_text()+
            f'\n[OpenCiv2]\nOriginalGraphics={int(mode == "original")}\nGraphicsDensity={1 if mode == "original" else mode}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC','CIV2_DATA','CIV2_GAME','CIV2_TEST_NATIVE','CIV2_NORMAL_DESKTOP','CIV2_SHOW','CIV2_TEST_GPU','CIV2_REALTIME','CIV2_BENCHMARK_FRAME','CIV2_GPU_NO_ATLAS'):
            env.pop(key,None)
        env.update(CIV2_TEST_HD='1',CIV2_TEST_FIXES='1',CIV2_TEST_EXTRAS='0',CIV2_TEST_FULL='1024x768',
                   CIV2_SEED='7',CIV2_BENCHMARK='1',CIV2_SCRIPT=str(script),
                   CIV2_SHOT_DIR=str(out),CIV2_HOOK_LOG=str(out/'hooks.log'),
                   SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy')
        if args.gpu:
            env.pop('SDL_VIDEODRIVER',None)
            env['CIV2_TEST_GPU']='1'
        if args.realtime:env['CIV2_REALTIME']='1'
        if args.frame:env['CIV2_BENCHMARK_FRAME']='1'
        if args.no_atlas:env['CIV2_GPU_NO_ATLAS']='1'
        if os.name != 'nt': env['CIV2_DATA'] = str(ROOT/'build/data')
        with (out/'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()),str(game)],env=env,stdout=log,
                           stderr=subprocess.STDOUT,timeout=300,check=True)
        hooks = (out/'hooks.log').read_text()
        assert 'script finished' in hooks and 'no function' not in hooks,out
        if args.frame:assert 'benchmark: includes desktop composition' in hooks,(out,'executable lacks --frame timing support')
        if args.gpu and mode != 'original': assert 'HD compositor: SDL GPU' in hooks,(out,'GPU unavailable')
        samples = [int(n)/1000 for n in re.findall(r'^benchmark: map_redraw_all_views (\d+) us$',hooks,re.M)]
        transitions = [int(n)/1000 for n in re.findall(r'^benchmark: menu_command (\d+) us$',hooks,re.M)]
        assert len(samples) == 3*(args.samples+1),(out,'missing timing samples',len(samples))
        assert len(transitions) == 3,(out,'missing first-zoom timing samples',len(transitions))
        results[str(mode)] = {}
        for i,zoom in enumerate((0,-3,8)):
            group = samples[i*(args.samples+1):(i+1)*(args.samples+1)]
            median = statistics.median(group[1:])
            p95=sorted(group[1:])[math.ceil(args.samples*0.95)-1]
            results[str(mode)][str(zoom)] = dict(samples_ms=group,warm_median_ms=median,
                                                warm_p95_ms=p95,first_zoom_command_ms=transitions[i])
            print(f'{mode}: zoom {zoom:+d}, warm median {median:.2f} ms, p95 {p95:.2f} ms; first zoom command {transitions[i]:.2f} ms',flush=True)
        (work/'results.json').write_text(json.dumps(results,indent=2))
    print(('Redraw + desktop composition' if args.frame else 'Offscreen redraw only')+
          ' (GPU completion included with --gpu); excludes presentation/vsync; not an FPS measurement.')


if __name__ == '__main__':
    main()
