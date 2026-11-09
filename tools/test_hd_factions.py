#!/usr/bin/env python3
"""Check the built faction variants, including fixed trim and aircraft coverage."""
import io
from pathlib import Path
import struct

from PIL import Image, ImageChops, ImageDraw

ROOT = Path(__file__).resolve().parents[1]


def main():
    data = (ROOT/'build/data/atlas-hd.pack').read_bytes()
    assert data[:8] == b'C2ATLAS1'
    entries = {}
    for i in range(struct.unpack_from('<I', data, 8)[0]):
        pos = 780+i*180
        key = data[pos:pos+160].split(b'\0')[0].decode()
        entries[key] = struct.unpack_from('<5I', data, pos+160)

    def image(key):
        w, h, offset, size, _ = entries[key]
        return Image.open(io.BytesIO(data[offset:offset+size])).convert('RGBA')

    def count(im, rgb):
        return sum(n for n, c in im.getcolors(im.width*im.height) if c == (*rgb, 255))

    def gold_count(im):
        # The narrow flagpole is partially transparent after downsampling.
        return sum(n for n, c in im.getcolors(im.width*im.height)
                   if c[3] > 150 and max(abs(a-b) for a, b in zip(c[:3], (221, 195, 139))) <= 3)

    flag = image('faction/flag')
    badge = image('crop/resource/tiles.dll/gifs/86/2,398,64,64')
    airbase = image('improvement/airbase')
    preview = Image.new('RGB', (1280, 480), '#5d6664')
    labels = ImageDraw.Draw(preview)
    for i in range(8):
        color_index = 208+i*3
        color = tuple(data[12+color_index*3:15+color_index*3])
        flag_art = image(f'tint/faction/flag/{color_index}')
        badge_art = image(f'tint/crop/resource/tiles.dll/gifs/86/2,398,64,64/{color_index}')
        air_art = image(f'tint/crop/file/cities/338,423,64,48/{color_index}')
        for name, art, base in [('flag', flag_art, flag), ('badge', badge_art, badge)]:
            assert art.size == base.size
            assert art.getchannel('A').tobytes() == base.getchannel('A').tobytes(), name
            assert count(art, color) > 20, (name, i, 'missing faction color')
            assert gold_count(art) > 20, (name, i, 'lost gold trim')
        assert count(badge_art, (21, 46, 51)) > 20, (i, 'lost badge detail')
        assert count(air_art, color) > 100, (i, 'missing aircraft color')
        diff = ImageChops.difference(air_art, airbase).convert('RGB').getbbox()
        assert diff and diff[0] >= airbase.width*20/64 and diff[2] <= airbase.width*44/64, diff
        assert diff[1] >= airbase.height*21/48 and diff[3] <= airbase.height*42/48, diff
        # Every source flag slot can follow a changed live civilization color.
        for index in range(color_index, color_index+3):
            for slot in range(8):
                bright = image(f'tint/crop/file/cities/{1+15*slot},425,14,22/{index}')
                dark = image(f'tint/crop/file/cities/{1+15*slot},448,14,22/{index}')
                assert bright.tobytes() == flag_art.tobytes()
                assert count(dark, tuple(round(c*.65) for c in color)) > 20
                assert dark.getchannel('A').tobytes() == flag_art.getchannel('A').tobytes()
        labels.text((i*160+10, 8), f'Color {i}', fill='white')
        for y, art in [(32, flag_art), (180, badge_art), (330, air_art)]:
            art = art.copy();art.thumbnail((150, 140))
            preview.paste(art, (i*160+(160-art.width)//2, y), art)
    out = ROOT/'build/hd-factions.png';preview.save(out)
    print('All eight faction colors passed: flags, dark flags, badges and occupied airbases.')
    print('Preview:', out)


if __name__ == '__main__':
    main()
