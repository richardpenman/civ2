#!/usr/bin/env python3
"""Exercise the live camera option, its HD checkbox, persistence and CPU fallback."""
import argparse
import configparser
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
from PIL import Image, ImageChops

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game',type=Path,required=True)
    ap.add_argument('--exe',type=Path,default=ROOT/('build/OpenCiv2.exe' if os.name=='nt' else 'build/OpenCiv2'))
    ap.add_argument('--densities',type=int,nargs='+',choices=range(1,5),default=[1,4])
    args=ap.parse_args()
    work=Path(tempfile.mkdtemp(prefix='map-options-',dir=ROOT/'build'))
    print('Results:',work,flush=True)
    script=(ROOT/'harness/hd/camera-options.txt').read_text()
    # A fresh process reads the saved setting, then saves it switched off.
    reload_script=script.split('shot options-off')[0]+'''shot options-reloaded
click 340 353
key Return
wait 1000
key Escape
wait 1000
camera_probe 200 250
'''
    for density in args.densities:
        for gpu in (1,0):
            saved=None
            for phase in ('toggle','reload'):
                out=work/f'{"gpu" if gpu else "cpu"}-{density}-{phase}';out.mkdir()
                game=out/'game';game.mkdir()
                for p in args.game.iterdir():
                    if p.is_file() and p.suffix.lower() in ('.txt','.sav'):
                        shutil.copyfile(p,game/p.name)
                shutil.copyfile(ROOT/'harness/CIV2.DAT',game/'CIV2.DAT')
                (game/'CIV.INI').write_text(saved if saved is not None else
                    (ROOT/'harness/CIV.INI').read_text()+f'\n[OpenCiv2]\nOriginalGraphics=0\nGraphicsDensity={density}\nHardwareGraphics={gpu}\n')
                replay=out/'replay.txt';replay.write_text(script if phase=='toggle' else reload_script)
                env=dict(os.environ)
                for k in ('CIV2_CLASSIC','CIV2_DATA','CIV2_GAME','CIV2_TEST_NATIVE','CIV2_NORMAL_DESKTOP',
                          'CIV2_BENCHMARK','CIV2_SHOW','SDL_VIDEODRIVER','CIV2_TEST_SETTINGS'):
                    env.pop(k,None)
                env.update(CIV2_REALTIME='1',CIV2_TEST_HD='1',CIV2_TEST_GPU='1',CIV2_TEST_FIXES='1',
                    CIV2_TEST_EXTRAS='1',CIV2_TEST_FULL='640x480',CIV2_SCRIPT=str(replay),CIV2_SHOT_DIR=str(out),
                    CIV2_HOOK_LOG=str(out/'hooks.log'),SDL_AUDIODRIVER='dummy',CIV2_SEED='7')
                if os.name!='nt':env['CIV2_DATA']=str(ROOT/'build/data')
                with (out/'output.log').open('wb') as log:
                    subprocess.run([str(args.exe.resolve()),str(game)],env=env,stdout=log,
                        stderr=subprocess.STDOUT,timeout=120,check=True)
                hooks=(out/'hooks.log').read_text()
                assert 'script finished' in hooks and 'no function' not in hooks,out
                assert ('HD compositor: SDL GPU' if gpu else 'HD compositor: CPU') in hooks,out
                probes=[(float(scale),int(base)) for scale,base in re.findall(r'camera probe .*? scale=([\d.]+) base=(-?\d+)',hooks)]
                assert len(probes)==(4 if phase=='toggle' else 1),(out,probes)
                if phase=='toggle' and gpu:
                    assert 1.0625<probes[0][0]<1.125 and probes[0]==probes[1],(out,'enable/cancel',probes)
                    assert probes[2]==(0,1) and probes[3][0]==1.125,(out,'disable/re-enable',probes)
                else:
                    assert all(scale==0 and base==0 for scale,base in probes),(out,'fixed zoom fallback',probes)
                def box(name):
                    with Image.open(out/(name+'.bmp')) as im:
                        return im.convert('RGB').crop(tuple(v*density for v in (330,344,348,363)))
                if phase=='toggle':
                    off,on=box('options-off'),box('options-on')
                    assert ImageChops.difference(off,on).getbbox(),(out,'checkbox did not change')
                    # The checked artwork adds a gold tick. Catch swapped HD bindings.
                    def gold(im):
                        return sum(n for n,(r,g,b) in im.getcolors(im.width*im.height)
                                   if r>150 and g>130 and b<170)
                    assert gold(on)>gold(off),(out,'checkbox states reversed')
                    for name in ('options-saved','options-after-cancel'):
                        assert not ImageChops.difference(on,box(name)).getbbox(),(out,name)
                    assert not ImageChops.difference(off,box('options-disabled')).getbbox(),out
                else:
                    assert not ImageChops.difference(on,box('options-reloaded')).getbbox(),(out,'saved checkbox')
                saved=(game/'CIV.INI').read_text()
                ini=configparser.ConfigParser();ini.read_string(saved)
                assert ini.getint('OpenCiv2','GpuMapCamera')==int(phase=='toggle'),(out,'persistence')
                assert set(ini['OpenCiv2'])=={'originalgraphics','graphicsdensity','hardwaregraphics','gpumapcamera'},(out,'unrelated options changed')
            print(f'{"GPU" if gpu else "CPU"}, density {density}: checkbox, OK/Cancel, live toggle and restart persistence passed.',flush=True)

if __name__=='__main__':main()
