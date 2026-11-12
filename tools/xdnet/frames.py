#!/usr/bin/env python3
"""The windows on a screenshot: the top-left corners of their raised 3-D
frames (the system colours of the frame both games draw: 227 outside, then
255, 245, 223 inwards), as 'x y' lines, top to bottom.  nettest.py finds
the game's windows with it (the original's and ours are not always in the
same place: ours has no caption of its own unless CIV2_CLASSIC=1).

usage: frames.py SCREEN.png [X]     (X: only corners in that column)
"""
import sys
from PIL import Image


def corners(path, col=None):
    im = Image.open(path).convert('L')
    w, h = im.size
    px = im.load()
    xs = [col] if col is not None else range(w - 4)
    out = []
    for y in range(h - 4):
        for x in xs:
            if (px[x, y] == 227 and px[x + 1, y] == 227 and px[x, y + 1] == 227 and
                    px[x + 1, y + 1] == 255 and px[x + 3, y + 1] == 255 and px[x + 1, y + 3] == 255 and
                    px[x + 2, y + 2] == 245 and px[x + 3, y + 3] == 223 and
                    (x == 0 or px[x - 1, y + 2] != 227) and (y == 0 or px[x + 2, y - 1] != 227)):
                out.append((x, y))
    return out


if __name__ == '__main__':
    for x, y in corners(sys.argv[1], int(sys.argv[2]) if len(sys.argv) > 2 else None):
        print(x, y)
