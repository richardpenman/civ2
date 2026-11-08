#!/usr/bin/env python3
"""Replay all 16 map zooms at each HD density, without original graphics."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

from PIL import Image, ImageDraw, ImageChops, ImageStat

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--exe', type=Path, default=ROOT/'build/OpenCiv2.exe' if os.name == 'nt' else ROOT/'build/OpenCiv2')
    parser.add_argument('--densities', type=int, nargs='+', choices=range(1,5), default=[1,2,3,4])
    renderer = parser.add_mutually_exclusive_group()
    renderer.add_argument('--gpu', action='store_true', help='exercise a hidden SDL hardware window')
    renderer.add_argument('--cpu-camera-fallback', action='store_true',
                          help='force CPU rendering with GpuMapCamera=1 on a hardware-capable hidden window')
    parser.add_argument('--reference',type=Path,help='compare against a previous hd-zoom result directory')
    parser.add_argument('--max-channel-error',type=int,default=8)
    parser.add_argument('--max-mean-error',type=float,default=0.15)
    args = parser.parse_args()
    files = {p.name.lower(): p for p in args.game.iterdir() if p.is_file()}
    for name in ('game.txt','rules.txt','labels.txt','he_auto.sav'):
        if name not in files:parser.error(f'--game needs {name}')
    work = Path(tempfile.mkdtemp(prefix='hd-zoom-', dir=ROOT/'build'))
    print('Results:', work, flush=True)

    def run(density):
        out = work/f'density-{density}';out.mkdir()
        game = out/'game';game.mkdir()
        for p in files.values():
            if p.suffix.lower() in ('.txt','.sav'):shutil.copyfile(p,game/p.name)
        shutil.copyfile(ROOT/'harness/CIV2.DAT',game/'CIV2.DAT')
        (game/'CIV.INI').write_text((ROOT/'harness/CIV.INI').read_text()+
                                  f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\n'
                                  f'HardwareGraphics={int(args.gpu)}\nGpuMapCamera={int(args.cpu_camera_fallback)}\n')
        env = dict(os.environ)
        for key in ('CIV2_CLASSIC','CIV2_DATA','CIV2_GAME','CIV2_TEST_NATIVE','CIV2_TEST_EXTRAS','CIV2_NORMAL_DESKTOP','CIV2_TEST_GPU','CIV2_SHOW'):
            env.pop(key,None)
        env.update(CIV2_TEST_HD='1',CIV2_TEST_FIXES='1',CIV2_TEST_FULL='1024x768',CIV2_SEED='7',
                   CIV2_SCRIPT=str(ROOT/'harness/hd/zoom.txt'),CIV2_SHOT_DIR=str(out),
                   CIV2_HOOK_LOG=str(out/'hooks.log'),SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy')
        if os.name != 'nt':env['CIV2_DATA']=str(ROOT/'build/data')
        if args.gpu or args.cpu_camera_fallback:
            env.pop('SDL_VIDEODRIVER',None);env['CIV2_TEST_GPU']='1'
        if args.cpu_camera_fallback:env['CIV2_CAMERA_TRACE']='1'
        with (out/'output.log').open('wb') as log:
            subprocess.run([str(args.exe.resolve()),str(game)],env=env,stdout=log,
                           stderr=subprocess.STDOUT,timeout=240,check=True)
        hooks=(out/'hooks.log').read_text()
        assert 'script finished' in hooks and 'no function' not in hooks, out
        assert f'density {density}; original image loading disabled' in hooks
        expected='HD compositor: SDL GPU' if args.gpu else 'HD compositor: CPU'
        assert expected in hooks,(out,'wrong compositor')
        if args.cpu_camera_fallback:
            assert 'camera refresh:' not in hooks and 'camera cache:' not in hooks,(out,'GPU camera ran in CPU mode')
        maps=set();status=None;worst_channel=0;worst_mean=0
        preview=Image.new('RGB',(1280,1056),'#202622');draw=ImageDraw.Draw(preview)
        for i in range(16):
            with Image.open(out/f'zoom-{i:02d}.bmp') as raw:
                assert raw.size == (1024*density,768*density)
                im=raw.convert('RGB')
                if args.reference:
                    with Image.open(args.reference/f'density-{density}'/f'zoom-{i:02d}.bmp') as reference:
                        assert reference.size==im.size,(density,i-7,'reference dimensions')
                        diff=ImageChops.difference(im,reference.convert('RGB'))
                        channel=max(hi for lo,hi in diff.getextrema())
                        mean=sum(ImageStat.Stat(diff).mean)/3
                        assert channel<=args.max_channel_error and mean<=args.max_mean_error,(density,i-7,'pixel difference',channel,mean)
                        worst_channel=max(worst_channel,channel);worst_mean=max(worst_mean,mean)
                region=im.crop(tuple(v*density for v in (10,76,750,748)))
                maps.add(hashlib.sha256(region.tobytes()).digest())
                # The stable status header must not rescale or be overwritten.
                # The lower pane legitimately follows the current map selection.
                pane=im.crop(tuple(v*density for v in (774,248,1014,306))).tobytes()
                if status is None:status=pane
                else:assert pane == status, (density,i-7,'map zoom changed status header')
                region.thumbnail((312,236))
                x,y=(i%4)*320,(i//4)*264
                preview.paste(region,(x,y+24));draw.text((x+6,y+5),f'Zoom {i-7:+d} / density {density}',fill='white')
        assert len(maps)==16, (density,'not all zoom levels rendered',len(maps))
        preview.save(out/'overview.png')
        print(f'Density {density}: all 16 zooms rendered; status header unchanged.',flush=True)
        if args.reference:print(f'Density {density}: max channel difference {worst_channel}, worst scene mean {worst_mean:.6f}.',flush=True)

    with ThreadPoolExecutor(max_workers=2) as pool:
        list(pool.map(run,args.densities))


if __name__ == '__main__':
    main()
