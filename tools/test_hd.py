#!/usr/bin/env python3
"""Compile and exercise the actual compositor, including malformed-pack rejection."""
import os
from pathlib import Path
import struct
import subprocess
import sys
import zlib

ROOT = Path(__file__).resolve().parents[1]


def fixture():
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))
    def png(variant):
        rows = b''.join(b'\0' + b''.join(
            bytes((255, 0, 0, 255) if x == y == 1 else (0, 0, 255, 255)) if variant and 0 < x < 3 and 0 < y < 3
            else bytes((0, 255, 0, 255)) if 0 < x < 3 and 0 < y < 3
            else bytes((255, 0, 255, 0)) for x in range(4)) for y in range(4))
        return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>2I5B', 4, 4, 8, 6, 0, 0, 0))
                + chunk(b'IDAT', zlib.compress(rows)) + chunk(b'IEND', b''))
    palette = bytearray(768);palette[7] = 255;palette[9] = 255
    # A narrow white flagpole between the old bilinear sampler's taps.
    rows = b''.join(b'\0'+b''.join(bytes((255,255,255,255) if x == 2 else (255,0,255,0))
                                  for x in range(64)) for y in range(64))
    pole = (b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>2I5B',64,64,8,6,0,0,0))
            +chunk(b'IDAT',zlib.compress(rows))+chunk(b'IEND',b''))
    index, payload = b'', b''
    for key, data in sorted([('file/test', png(False)), ('tint/file/test/2', png(True)), ('test/pole', pole)]):
        offset = 780+3*180+len(payload)
        index += key.encode().ljust(160, b'\0') + struct.pack('<5I', 4, 4, offset, len(data), offset+len(data))
        payload += data+bytes([2]*16)
    return b'C2ATLAS1' + struct.pack('<I', 3) + palette + index + payload


def main():
    out = ROOT / 'build/hd-unit';out.mkdir(parents=True, exist_ok=True)
    exe = out / ('hd-test.exe' if os.name == 'nt' else 'hd-test')
    cc = os.environ.get('CC', 'cc')
    pack = out/'atlas-hd.pack';pack.write_bytes(fixture())
    # A deliberately small cache also exercises eviction and oversized-entry
    # fallback against the same reference pixel hashes.
    for budget in (None,4096):
        flags = [] if budget is None else [f'-DHD_FILTERED_BUDGET={budget}']
        subprocess.run([cc, '-std=gnu99', '-O1', '-g', *flags, str(ROOT/'tests/hd_runtime.c'),
                        str(ROOT/'src/platform/hd.c'), str(ROOT/'tests/hd_gpu_stub.c'), '-lm', '-o', str(exe)], check=True)
        for density in (1, 2, 3, 4):
            env = dict(os.environ, GraphicsDensity=str(density))
            env.pop('CIV2_TEST_HD', None);env.pop('OriginalGraphics', None)
            subprocess.run([str(exe), str(out)], env=env, check=True)
    pack.write_bytes(b'not-an-atlas')
    result = subprocess.run([str(exe), str(out)], capture_output=True)
    assert result.returncode == 2 and b'Invalid Atlas' in result.stderr, result
    pack.unlink()
    subprocess.run([str(exe), str(out)], env=dict(os.environ, OriginalGraphics='1'), check=True)
    print('Malformed pack rejected; original mode works without an atlas.')


if __name__ == '__main__':
    main()
