#!/usr/bin/env python3
"""Check the real SDL GPU compositor against CPU pixels on a hidden window.

Set CC and SDL2_DIR (a prefix containing include/SDL2, lib, and optionally bin)
when cross-compiling on Windows. Other platforms use sdl2-config by default.
An accelerated display session is required; unavailable hardware fails the test.
"""
import os
import argparse
from pathlib import Path
import shlex
import subprocess

from test_hd import fixture, ROOT


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--small-atlas',action='store_true',help='force page exhaustion and private-texture fallback')
    args=parser.parse_args()
    out = ROOT/'build/hd-gpu-unit';out.mkdir(parents=True, exist_ok=True)
    exe = out/('hd-gpu-test.exe' if os.name == 'nt' else 'hd-gpu-test')
    (out/'atlas-hd.pack').write_bytes(fixture())
    env = dict(os.environ)
    if os.environ.get('SDL2_DIR'):
        sdl = Path(os.environ['SDL2_DIR']).resolve()
        cflags = ['-I'+str(sdl/'include/SDL2')]
        libs = ['-L'+str(sdl/'lib'), '-lSDL2']
        env['PATH'] = str(sdl/'bin')+os.pathsep+env.get('PATH','')
    else:
        cflags = shlex.split(subprocess.check_output(['sdl2-config','--cflags'], text=True))
        libs = shlex.split(subprocess.check_output(['sdl2-config','--libs'], text=True))
    atlas_flags=['-DGPU_ATLAS_SIZE=128','-DGPU_ATLAS_PAGES=2'] if args.small_atlas else []
    subprocess.run([os.environ.get('CC','cc'), '-std=gnu99', '-O2', *atlas_flags, *cflags,
                    str(ROOT/'tests/hd_gpu_runtime.c'), str(ROOT/'src/platform/hd.c'),
                    str(ROOT/'src/host/host_gpu.c'), *libs, '-lm', '-o', str(exe)], check=True)
    for key in ('OriginalGraphics','CIV2_TEST_HD','SDL_VIDEODRIVER'):
        env.pop(key,None)
    for density in range(1,5):
        subprocess.run([str(exe),str(out)], env=dict(env,GraphicsDensity=str(density)), check=True)


if __name__ == '__main__':
    main()
