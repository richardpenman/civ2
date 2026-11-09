#!/usr/bin/env python3
"""Exercise the opt-in GPU camera and real map picking in an isolated game."""
import argparse
import os
from pathlib import Path
import re
import shutil
import statistics
import subprocess
import tempfile
from PIL import Image, ImageChops

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game',type=Path,required=True)
    ap.add_argument('--exe',type=Path,default=ROOT/('build/OpenCiv2.exe' if os.name=='nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities',type=int,nargs='+',choices=range(1,5),default=[1,2,3,4])
    ap.add_argument('--verify-cache',action='store_true',help='compare cached GPU pixels with fresh draws, including state changes')
    args=ap.parse_args()
    work=Path(tempfile.mkdtemp(prefix='map-camera-',dir=ROOT/'build'))
    print('Results:',work,flush=True)
    script=ROOT/'harness/hd/camera.txt'
    for density in args.densities:
        out=work/f'density-{density}';out.mkdir();game=out/'game';game.mkdir()
        for p in args.game.iterdir():
            if p.is_file() and p.suffix.lower() in ('.txt','.sav'):
                shutil.copyfile(p,game/p.name)
        shutil.copyfile(ROOT/'harness/CIV2.DAT',game/'CIV2.DAT')
        (game/'CIV.INI').write_text((ROOT/'harness/CIV.INI').read_text()+
            f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\nHardwareGraphics=1\nGpuMapCamera=1\n')
        env=dict(os.environ)
        if args.verify_cache:env['CIV2_CAMERA_CACHE_VERIFY']='1'
        for k in ('CIV2_CLASSIC','CIV2_DATA','CIV2_GAME','CIV2_TEST_NATIVE','CIV2_NORMAL_DESKTOP',
                  'CIV2_BENCHMARK','CIV2_SHOW','SDL_VIDEODRIVER','CIV2_TEST_SETTINGS','CIV2_TEST_EXTRAS'):
            env.pop(k,None)
        env.update(CIV2_REALTIME='1',CIV2_TEST_HD='1',CIV2_TEST_GPU='1',CIV2_TEST_FIXES='1',CIV2_TEST_FULL='1024x768',
                   CIV2_SCRIPT=str(script),CIV2_SHOT_DIR=str(out),CIV2_HOOK_LOG=str(out/'hooks.log'),
                   CIV2_RAND_LOG=str(out/'rand.log'),SDL_AUDIODRIVER='dummy',CIV2_SEED='7')
        if os.name!='nt':env['CIV2_DATA']=str(ROOT/'build/data')
        with (out/'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()),str(game)],env=env,stdout=log,stderr=subprocess.STDOUT,
                           timeout=180,check=True)
        hooks=(out/'hooks.log').read_text()
        assert 'script finished' in hooks and 'HD compositor: SDL GPU' in hooks and 'no function' not in hooks,out
        if args.verify_cache:
            assert 'camera cache verified:' in hooks,out
            assert 'camera cache mutations verified: terrain/fog, 1 unit, 1 city' in hooks,out
        probes=re.findall(r'camera probe (\d+) (\d+): result=(\d+) tile=(-?\d+),(-?\d+) source=(-?\d+),(-?\d+) scale=([\d.]+) base=(-?\d+) mapped=(\d+) cursor=(-?\d+),(-?\d+)',hooks)
        assert len(probes)==55,(out,probes)
        world=[tuple(map(float,p)) for p in re.findall(r'camera world: (-?[\d.]+),(-?[\d.]+)',hooks)]
        assert len(world)==len(probes),(out,'missing precise camera probes')
        for indices in (range(4),range(4,6),range(11,13),range(13,55)):
            anchor=world[indices.start]
            error=max(max(abs(p[i]-anchor[i]) for i in (0,1)) for p in (world[j] for j in indices))
            assert error<1e-6,(out,'world point jumped during zoom/pan',indices,error,world)
        assert all(p[2]=='0' for p in probes[:8]),(out,probes)
        assert all(p[3:5]==probes[0][3:5] for p in probes[1:4]),(out,'anchor/pan tile changed',probes)
        # Selecting can legitimately recenter the map. Check the tile that was
        # under the pointer before the click, rather than its new contents.
        assert probes[4][10:12]==probes[3][3:5],(out,'click selected wrong tile',probes[4])
        assert probes[5][3:5]==probes[4][3:5],(out,'zoom-out anchor tile changed')
        assert 1<float(probes[1][7])<1.15,(out,'fractional wheel lost')
        assert float(probes[6][7])==.125 and float(probes[7][7])==2,(out,'zoom limits')
        assert probes[8][2]=='1',(out,'chrome accepted as map')
        assert float(probes[9][7])==1 and float(probes[10][7])==1.125,(out,'preset/step zoom',probes)
        timings=[int(n)/1000 for n in re.findall(r'camera compose: (\d+) us',hooks)]
        assert len(timings)==31
        print(f'Density {density}: camera composition median {statistics.median(timings):.2f} ms; p95 {sorted(timings)[29]:.2f} ms (refresh/present excluded).',flush=True)
        with Image.open(out/'before.bmp') as im: before=im.convert('RGB')
        header=tuple(v*density for v in (774,248,1014,306))
        for name in ('fractional','zoomed','panned','out','minimum','maximum'):
            with Image.open(out/(name+'.bmp')) as im: current=im.convert('RGB')
            assert not ImageChops.difference(before.crop(header),current.crop(header)).getbbox(),(name,'status rescaled')
            assert ImageChops.difference(before,current).getbbox(),(name,'map unchanged')
        print(f'Density {density}: precise anchors, queued drag, repeated recentering, limits and stationary chrome passed.',flush=True)

if __name__=='__main__':main()
