#!/usr/bin/env python3
"""A contact sheet of a run's shots (harness/hd/hd-herald-own.sh): the
diplomacy room (640x480 at 128,96 of the 1024x768 screen) of each shot,
its HD plane (NAME-hd.png, or .bmp) where there is one, else the game's pixels
enlarged, labelled.

    hd-herald-own-sheet.py DIR NAME,NAME,... OUT.png [WIDTH] [COLUMNS]
"""
import os, sys
from PIL import Image, ImageDraw

d, names, out = sys.argv[1], sys.argv[2].split(','), sys.argv[3]
width = int(sys.argv[4]) if len(sys.argv) > 4 else 384
cols = int(sys.argv[5]) if len(sys.argv) > 5 else 5
ims = []
for n in names:
    # (tools/native-run.sh makes the shots PNG; .bmp: CIV2_KEEP_BMP=1 or older runs)
    p = next((p for p in (os.path.join(d, n + s) for s in ('-hd.png', '-hd.bmp', '.png', '.bmp'))
              if os.path.exists(p)), None)
    if not p:
        continue
    im = Image.open(p).convert('RGB')
    k = im.width / 1024
    im = im.crop((int(128 * k), int(96 * k), int(768 * k), int(576 * k)))
    im = im.resize((width, width * 3 // 4), Image.LANCZOS)
    ImageDraw.Draw(im).text((6, 4), n, fill=(255, 255, 0))
    ims.append(im)
if ims:
    W, H = ims[0].size
    rows = (len(ims) + cols - 1) // cols
    s = Image.new('RGB', (W * cols, H * rows))
    for i, im in enumerate(ims):
        s.paste(im, ((i % cols) * W, (i // cols) * H))
    s.save(out)
