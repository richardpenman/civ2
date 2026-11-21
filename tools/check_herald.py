#!/usr/bin/env python3
"""HD graphics (docs/HD-GRAPHICS.md, "Your own heralds"): check that one of
OpenCiv2's own animated heralds (data/native/hd-extra/herald-CODE.webp, its
line in heralds.txt) plays without a jump where the game goes from one
segment to the next.

    python3 tools/check_herald.py CODE [--dir DIR] [--video OUT.mp4]

(Python with Pillow and numpy: on the host, tools/dk python3 tools/check_herald.py
RUS; tools/build_herald.py runs it on what it has made.)  The segments are
played in an order the game plays them: the greeting, the idle loop twice,
pleased, idle twice, the refusal, idle twice, angry, idle twice (so every
join the game makes: greeting to idle, idle to idle, idle to each reaction
and back).  Each frame's change is the mean absolute difference of its RGBA
from the frame before; a frame changing more than twice the median is
reported (once), and the check fails (exit 1) if any of those is at a join between
two segments (fast motion inside a clip, a nod or a stamp of the staff, is
only reported).  --video writes the sequence as a video for looking at (each
frame on a dark ground, labelled with its segment, frame and change, a red
border round a jump; ffmpeg needed).
"""
import argparse, os, shutil, subprocess, sys, tempfile

import numpy as np
from PIL import Image, ImageDraw

NAMES = ('greeting', 'idle', 'pleased', 'refusal', 'angry')
ORDER = ('greeting', 'idle', 'idle', 'pleased', 'idle', 'idle', 'refusal', 'idle', 'idle', 'angry', 'idle', 'idle')
LIMIT = 2.0             # times the median change


def segments(path, code):
    for line in open(path):
        w = line.split()
        if w and not w[0].startswith('#') and w[0].upper() == code:
            t = [int(v) for v in w[6:16]]
            return dict(zip(NAMES, zip(t[0::2], t[1::2]))), w[16], int(w[5])
    sys.exit('%s: no line for %s' % (path, code))


def check(webp, segs, fps=12, video=None, out=sys.stdout):
    """Prints the report; returns the joins that jump."""
    im = Image.open(webp)
    frames = []
    for i in range(im.n_frames):
        im.seek(i)
        frames.append(np.asarray(im.convert('RGBA')))
    seq = [(name, f) for name in ORDER for f in range(segs[name][0], segs[name][1] + 1)]
    change = [0.0] + [float(np.abs(frames[f].astype(np.int16) - frames[seq[j - 1][1]]).mean())
                      for j, (_, f) in enumerate(seq) if j]
    base = float(np.median(change[1:]))
    print('%s: %d frames, played %d (%s), median change %.2f' % (webp, len(frames), len(seq), ' '.join(ORDER), base),
          file=out)
    bad = []
    seen = set()
    for j in range(1, len(seq)):
        if (seq[j - 1][1], seq[j][1]) in seen:
            continue                    # (each change reported once)
        seen.add((seq[j - 1][1], seq[j][1]))
        join = seq[j][1] == segs[seq[j][0]][0]
        what = '%s %d -> %s %d: %.2f, %.2fx' % (seq[j - 1][0], seq[j - 1][1], seq[j][0], seq[j][1], change[j],
                                                 change[j] / base)
        if join:
            print('  join %s%s' % (what, '  JUMP' if change[j] > LIMIT * base else ''), file=out)
            if change[j] > LIMIT * base:
                bad.append(what)
        elif change[j] > LIMIT * base:
            print('  in the clip %s (fast motion, not a join)' % what, file=out)
    print('%s' % ('FAILED: %d joins jump' % len(bad) if bad else 'ok: no join jumps'), file=out)
    if video:
        tmp = tempfile.mkdtemp()
        try:
            h, w = frames[0].shape[:2]
            for j, (name, f) in enumerate(seq):
                bg = Image.new('RGBA', (w + 40, h + 70), (58, 38, 28, 255))
                bg.alpha_composite(Image.fromarray(frames[f], 'RGBA'), (20, 50))
                d = ImageDraw.Draw(bg)
                d.text((10, 6), os.path.basename(webp), fill=(255, 255, 255))
                d.text((10, 24), '%s  frame %d  change %.1f' % (name, f, change[j]), fill=(255, 230, 120))
                if change[j] > LIMIT * base:
                    d.rectangle([0, 0, w + 39, h + 69], outline=(255, 0, 0), width=8)
                    d.text((w - 60, 24), 'JUMP', fill=(255, 60, 60))
                bg.convert('RGB').save('%s/%05d.png' % (tmp, j), compress_level=1)
            subprocess.run(['ffmpeg', '-y', '-v', 'error', '-framerate', str(fps), '-i', tmp + '/%05d.png', '-vf',
                            'pad=ceil(iw/2)*2:ceil(ih/2)*2', '-c:v', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '20',
                            video], check=True)
            print('video: %s' % video, file=out)
        finally:
            shutil.rmtree(tmp, ignore_errors=True)
    return bad


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('code')
    ap.add_argument('--dir', default=os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                                                  'data', 'native', 'hd-extra'))
    ap.add_argument('--video')
    a = ap.parse_args()
    segs, name, fps = segments(os.path.join(a.dir, 'heralds.txt'), a.code.upper())
    sys.exit(1 if check(os.path.join(a.dir, name), segs, fps, a.video) else 0)


if __name__ == '__main__':
    main()
