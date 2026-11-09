#!/usr/bin/env python3
"""Prepare game graphics from a local asset clone or a cached, pinned release."""
import argparse
import ast
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
from dependency import copy_changed, inventory, read_json, resolve, validate, write_json

ROOT = Path(__file__).resolve().parents[1]


def check_contract(path):
    game_data = ROOT / 'src/data/gamedata.cpp'
    if not game_data.exists():
        return  # A portable C package already compiled this source/contract pair.
    data = game_data.read_text(encoding='utf-8')
    match = re.search(r'cityview_improvement_art_x_block_init\[\] = ("(?:\\.|[^"\\])*");', data)
    if not match:
        raise ValueError('Game city-view rectangle table missing; update the asset contract')
    raw = ast.literal_eval('b' + match.group(1))
    expected = [list(rect) for rect in struct.iter_unpack('<4i', raw.ljust(992, b'\0'))]
    if read_json(path) != expected:
        raise ValueError('Runtime assets do not match the game city-view rectangle contract')


def prepare(local=None, url=None, sha256=None):
    # Portable C packages and downstream CI jobs explicitly carry these files.
    # Verify their hashes rather than accepting an arbitrary stale pack.
    receipt = ROOT / 'build/data/assets-receipt.json'
    portable_package = (receipt.exists() and not (ROOT / 'src/data/gamedata.cpp').exists()
                        and not os.environ.get('CIV2_ASSETS_DIR')
                        and not os.environ.get('CIV2_ASSETS_URL'))
    if (os.environ.get('CIV2_ATLAS_PREBUILT') or portable_package) and local is None and url is None:
        expected = read_json(receipt)
        if inventory(ROOT, expected['files']) != expected['files']:
            raise ValueError('Prepared assets changed; prepare the dependency again')
        check_contract(ROOT / 'build/data/cityview-rects.json')
        return
    dep = resolve(ROOT, 'assets', local, url, sha256)
    if (dep / 'tools/build.py').is_file():
        # The graphics clone builds its own explicit runtime artwork selection.
        subprocess.run([sys.executable, str(dep / 'tools/build.py')], check=True)
        dep = dep / 'build'
    manifest = validate(dep, 'openciv2-assets')
    required = ['data/atlas-hd.pack', 'data/fonts.bin', 'data/fonts_aa.bin',
                'data/ATLAS-PROVENANCE.md', 'data/cityview-rects.json', 'include/hd_layouts.h', 'THIRD-PARTY.txt']
    if any(n not in manifest['files'] for n in required):
        raise ValueError(f'{dep}: incomplete runtime asset package')
    check_contract(dep / 'data/cityview-rects.json')
    copied = []
    for name in manifest['files']:
        if name.startswith('data/'):
            dest = 'build/' + name
        elif name == 'THIRD-PARTY.txt':
            dest = 'build/data/ASSET-NOTICES.txt'
        elif name == 'include/hd_layouts.h':
            dest = 'build/native/gen/hd_layouts.h'
        else:
            continue
        copy_changed(dep / name, ROOT / dest)
        copied.append(dest)
    write_json(ROOT / 'build/data/assets-receipt.json',
               {'schema': 1, 'files': inventory(ROOT, copied)})
    print(f'Game assets prepared from {dep}', flush=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    choice = ap.add_mutually_exclusive_group()
    choice.add_argument('--assets', type=Path, help='local openciv2-assets clone or extracted release')
    choice.add_argument('--assets-url', help='release ZIP URL (requires --assets-sha256)')
    ap.add_argument('--assets-sha256')
    args = ap.parse_args()
    prepare(args.assets, args.assets_url, args.assets_sha256)


if __name__ == '__main__':
    main()
