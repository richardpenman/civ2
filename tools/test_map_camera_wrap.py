#!/usr/bin/env python3
"""Compare cached/fresh GPU maps while dragging whole-world views at both boundaries."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
from PIL import Image, ImageChops, ImageStat

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game',type=Path,required=True)
    ap.add_argument('--exe',type=Path,default=ROOT/('build/OpenCiv2.exe' if os.name=='nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities',type=int,nargs='+',choices=range(1,5),default=[1,4])
    ap.add_argument('--topologies',nargs='+',choices=['wrapped','fixed'],default=['wrapped','fixed'])
    args=ap.parse_args()
    work=Path(tempfile.mkdtemp(prefix='map-camera-wrap-',dir=ROOT/'build'))
    print('Results:',work,flush=True)
    for density in args.densities:
        for topology in args.topologies:
            flat=int(topology=='fixed')
            runs=[]
            for cache in (0,1):
                out=work/f'density-{density}-flat-{flat}-cache-{cache}';out.mkdir()
                game=out/'game';game.mkdir()
                for p in args.game.iterdir():
                    if p.is_file() and p.suffix.lower() in ('.txt','.sav'):shutil.copyfile(p,game/p.name)
                shutil.copyfile(ROOT/'harness/CIV2.DAT',game/'CIV2.DAT')
                (game/'CIV.INI').write_text((ROOT/'harness/CIV.INI').read_text()+
                    f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\nHardwareGraphics=1\nGpuMapCamera=1\n')
                script=out/'script.txt'
                script.write_text((ROOT/'harness/hd/camera-wrap.txt').read_text().replace(
                    'map_camera_boundary_test 0 0',f'map_camera_boundary_test 0 {flat}'))
                env=dict(os.environ)
                for key in ('CIV2_CLASSIC','CIV2_DATA','CIV2_GAME','CIV2_TEST_NATIVE','CIV2_NORMAL_DESKTOP',
                            'CIV2_SHOW','SDL_VIDEODRIVER','CIV2_TEST_EXTRAS','CIV2_CAMERA_CACHE_VERIFY'):
                    env.pop(key,None)
                env.update(CIV2_REALTIME='1',CIV2_TEST_HD='1',CIV2_TEST_GPU='1',CIV2_TEST_FIXES='1',
                           CIV2_TEST_FULL='1024x768',CIV2_SEED='7',SDL_AUDIODRIVER='dummy',
                           CIV2_CAMERA_BOUNDARY_TEST='1',CIV2_CAMERA_CACHE=str(cache),
                           CIV2_SCRIPT=str(script),CIV2_SHOT_DIR=str(out),CIV2_HOOK_LOG=str(out/'hooks.log'))
                if os.name!='nt':env['CIV2_DATA']=str(ROOT/'build/data')
                with (out/'output.log').open('wb') as log:
                    subprocess.run([str(args.exe.resolve()),str(game)],env=env,stdout=log,
                                   stderr=subprocess.STDOUT,timeout=180,check=True)
                hooks=(out/'hooks.log').read_text()
                assert 'script finished' in hooks and 'no function' not in hooks,out
                assert f'camera boundary mode: flat={flat}' in hooks,out
                assert 'HD compositor: SDL GPU' in hooks,out
                probes=re.findall(r'camera probe .*?result=(\d+) tile=(-?\d+),(-?\d+).*?scale=([\d.]+)',hooks)
                assert len(probes)==13 and all(p[0]=='0' for p in probes),(out,probes)
                assert all(float(p[3])==.125 for p in probes[:9]),out
                xs=[int(p[1]) for p in probes[1:5]]
                if flat:assert max(xs)-min(xs)<=1,(out,'fixed map kept scrolling',xs)
                else:assert len(set(xs))>=3,(out,'wrapped map stopped at a seam',xs)
                if flat:
                    near=[int(p[1]) for p in probes[9:]]
                    assert near[0]==near[1] and near[2]==near[3],(out,'fixed overview did not stop at an edge',near)
                    world=re.findall(r'camera boundary mode: flat=\d map=(\d+),',hooks)
                    extent=int(world[0])+1
                    # At this scale the world is just wider than the viewport;
                    # the pan range must not include the backing image's margins.
                    span=740/(32*float(probes[9][3]))
                    assert max(near)-min(near)<=max(0,extent-span)+3,(out,'scrollable blank margin',near)
                runs.append((out,probes))
            assert runs[0][1]==runs[1][1],(density,flat,'cache changed picking')
            worst=0
            for name in ['wrap-start']+[f'wrap-{i}' for i in range(8)]+[f'fractional-wrap-{i}' for i in range(4)]:
                box=tuple(v*density for v in (10,76,750,748))
                with Image.open(runs[0][0]/f'{name}.bmp') as a,Image.open(runs[1][0]/f'{name}.bmp') as b:
                    diff=ImageChops.difference(a.convert('RGB').crop(box),b.convert('RGB').crop(box))
                    mean=sum(ImageStat.Stat(diff).mean)/3
                    # Independent real-clock runs can capture different cursor blink
                    # phases. Their tiny footprint must not hide missing map cells.
                    assert mean<.15,(density,flat,name,'cached/fresh map mismatch',mean)
                    worst=max(worst,mean)
            print(f'Density {density}, flat={flat}: 13 cached/fresh views and picking matched; worst mean error {worst:.4f}.',flush=True)

if __name__=='__main__':main()
