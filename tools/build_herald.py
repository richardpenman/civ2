#!/usr/bin/env python3
"""HD graphics (docs/HD-GRAPHICS.md, "Your own heralds"): make a tribe's own
animated herald, data/native/hd-extra/herald-CODE.webp, and its line in
data/native/hd-extra/heralds.txt, from five video clips of the figure on a
flat chroma-key green, in the game's order:

    python3 tools/build_herald.py CODE GREETING IDLE PLEASED DECLINE ANGRY
        [--fps 12] [--foot X,Y] [--height H] [--density D] [--quality Q] [--video OUT.mp4]

CODE is the three letters of the original's video, KINGS\\HRLDCODE.AVI (ROM
BAB GER EGY AME GRE IND RUS ZUL FRE AZT CHI ENG MON CEL JAP VIK SPA PER CAR
SIO).  The clips (any size and frame rate ffmpeg reads; the same size each,
the figure the same size in each) are: 0 the greeting, played once as the
room opens; 1 the idle loop; 2 pleased (an offer accepted); 3 a polite
refusal; 4 angry.  Each should start and end in about the same pose.

In a throwaway container of Debian 13's pinned image (Docker needed: ffmpeg,
cwebp and webpmux 1.5.0), each clip is:
1. cut into frames at its own rate, resampled to --fps (12: the game shows
   a herald at the stream's rate);
2. keyed: ffmpeg's chromakey at the green of the first frame's borders
   (similarity 0.12, blend 0.06), then despill (green, mix 0.6, expand 0.1);
3. blended over its last 0.5 s into one pose shared by every clip, the
   idle loop's first frame (keyed), and over its first 0.5 s out of it,
   but for the greeting, which nothing comes before (premultiplied, the
   pose's weight 6/7 at the clip's end, falling by 1/7 a frame): the game
   goes from the greeting to the idle loop, round the loop, from the loop
   to a reaction and back to the loop, at any of these joins from the end
   of one clip to the start of another, and the clips, made apart, do not
   quite meet (the sash, the staff's angle); now each join changes no more
   than a frame within a clip;
4. cropped to the figure: the union of every frame's opaque pixels, all
   clips together (the figure never moves between segments), with a margin.
The figure is then scaled so that it stands --height game pixels tall (the
original's heralds: about 395), its feet's centre at --foot (in the
diplomacy room's 640x480 game pixels; the original Russian's: 185,470), and
stored at --density times the game's pixels (1.5: drawn smoothly enlarged
at the game's HD densities 2 to 4, the file about 7 MB for 27 seconds),
every frame of the five clips back to back in one animated WebP (each a
whole canvas, lossy at --quality, 70, alpha lossless, by cwebp, put
together by webpmux without blending: every frame a key frame, so the game
starts any segment at once; the figure changes all over in each anyway).
Its line in heralds.txt, CODE X Y W H FPS then the first and last frame of
each segment, then the file, is written (replacing an earlier line for
CODE).  Then tools/check_herald.py plays the segments in the game's order and
fails the build (the files written all the same, to look at) if a join
jumps.  The clips' sources and prompts belong in docs/HD-GRAPHICS.md (Your own heralds).
"""
import argparse, glob, json, os, shutil, subprocess, sys, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEST = os.path.join(ROOT, 'data', 'native', 'hd-extra')
IMAGE = 'debian:trixie@sha256:9c6e381421d0f48d1b8133e573f33c185ee16cbf505eda9c4a8a4791b25ea8a6'
WEBP_VERSION = '1.5.0-0.1'
NAMES = ('greeting', 'idle', 'pleased', 'decline', 'angry')
CROSSFADE = 0.5         # seconds
KEY = '0.12:0.06'       # chromakey similarity:blend
DESPILL = 'despill=type=green:mix=0.6:expand=0.1'
MARGIN = 2              # game pixels round the figure


def inside(work):
    """In the container: the frames, keyed, cropped, scaled and encoded."""
    import numpy as np
    from PIL import Image
    job = json.load(open(os.path.join(work, 'job.json')))
    fps = job['fps']
    segs = []
    for i, name in enumerate(NAMES):
        d = os.path.join(work, name)
        os.makedirs(d + '/f')
        os.makedirs(d + '/c')
        os.makedirs(d + '/k')
        out = subprocess.run(['ffprobe', '-v', 'error', '-select_streams', 'v:0', '-show_entries',
                              'stream=r_frame_rate', '-of', 'csv=p=0', job['clips'][i]],
                             capture_output=True, text=True, check=True).stdout.strip()
        num, den = out.split('/')
        src_fps = float(num) / float(den)
        subprocess.run(['ffmpeg', '-v', 'error', '-i', job['clips'][i], '-an', d + '/f/%04d.png'], check=True)
        fs = sorted(glob.glob(d + '/f/*.png'))
        A = lambda p: np.asarray(Image.open(p).convert('RGB')).astype(np.float32)
        n = len(fs)
        f0 = A(fs[0])
        border = np.concatenate([f0[:40].reshape(-1, 3), f0[:, :40].reshape(-1, 3), f0[:, -40:].reshape(-1, 3)])
        key = '0x%02X%02X%02X' % tuple(int(v) for v in border.mean(0))
        # the frames at fps
        picks = []
        t = 0
        while True:
            j = int(round(t * src_fps / fps))
            if j >= n:
                break
            picks.append(j)
            t += 1
        for o, j in enumerate(picks):
            os.link(fs[j], d + '/c/%04d.png' % (o + 1))
        subprocess.run(['ffmpeg', '-v', 'error', '-framerate', str(fps), '-i', d + '/c/%04d.png', '-vf',
                        'format=rgba,chromakey=%s:%s,%s,format=rgba' % (key, KEY, DESPILL), d + '/k/%04d.png'],
                       check=True)
        shutil.rmtree(d + '/f')
        shutil.rmtree(d + '/c')
        segs.append(sorted(glob.glob(d + '/k/*.png')))
        print('%s: %d frames at %g fps, %.2f s -> %d frames at %d fps (key %s)' %
              (name, n, src_fps, n / src_fps, len(segs[-1]), fps, key), flush=True)
    # every clip into and out of one pose, the idle loop's first frame (premultiplied)
    def load(p):
        a = np.asarray(Image.open(p).convert('RGBA')).astype(np.float32) / 255
        return np.concatenate([a[..., :3] * a[..., 3:], a[..., 3:]], -1)
    def save(a, p):
        al = a[..., 3:]
        rgb = np.where(al > 1e-4, a[..., :3] / np.maximum(al, 1e-4), 0)
        Image.fromarray(np.clip(np.concatenate([rgb, al], -1) * 255 + 0.5, 0, 255).astype(np.uint8),
                        'RGBA').save(p, compress_level=1)
    pose = load(segs[1][0])
    k = int(round(CROSSFADE * fps))
    for i, seg in enumerate(segs):
        ends = [(seg[-1 - o], o) for o in range(k)] + ([(seg[o], o) for o in range(k)] if i else [])
        for p, o in ends:
            w = (o + 1) / (k + 1)                       # the clip's own weight
            save(pose * (1 - w) + load(p) * w, p)
    # the figure's bounds over every frame
    union = None
    for seg in segs:
        for p in seg:
            al = np.asarray(Image.open(p).getchannel('A')) > 16
            union = al if union is None else union | al
    ys, xs = np.nonzero(union)
    x0, x1, y0, y1 = int(xs.min()), int(xs.max()) + 1, int(ys.min()), int(ys.max()) + 1
    s = job['height'] / (y1 - y0)                        # game pixels per source pixel
    W = int(np.ceil((x1 - x0) * s)) + 2 * MARGIN
    H = int(np.ceil((y1 - y0) * s)) + 2 * MARGIN
    # the feet's centre: the middle of the opaque pixels of the bottom tenth, frame 0 of the idle loop
    a0 = np.asarray(Image.open(segs[1][0]).getchannel('A'))[y0:y1, x0:x1] > 128
    fx = float(np.nonzero(a0[int((y1 - y0) * 0.9):].any(0))[0].mean())
    X = int(round(job['foot'][0] - fx * s)) - MARGIN
    Y = job['foot'][1] - H + MARGIN
    D = job['density']
    cw, ch = int(round(W * D)), int(round(H * D))
    print('figure %dx%d source pixels at %d,%d: %dx%d game pixels at %d,%d, stored %dx%d' %
          (x1 - x0, y1 - y0, x0, y0, W, H, X, Y, cw, ch), flush=True)
    os.makedirs(work + '/out')
    files = []
    n = 0
    table = []
    for seg in segs:
        table.append((n, n + len(seg) - 1))
        for p in seg:
            im = Image.open(p).convert('RGBA').crop((x0, y0, x1, y1))
            # premultiplied for the resampling, so that no keyed-out colour bleeds in
            pm = np.asarray(im).astype(np.float32) / 255
            pm[..., :3] *= pm[..., 3:4]
            sw, sh = int(round((x1 - x0) * s * D)), int(round((y1 - y0) * s * D))
            ch_ = [Image.fromarray(pm[..., c]).resize((sw, sh), Image.LANCZOS) for c in range(4)]
            r = np.clip(np.stack([np.asarray(c) for c in ch_], -1), 0, 1)
            al = r[..., 3:4]
            rgb = np.where(al > 1e-4, r[..., :3] / np.maximum(al, 1e-4), 0)
            out = np.zeros((ch, cw, 4), np.uint8)
            ox, oy = int(round(MARGIN * D)), int(round(MARGIN * D))
            out[oy:oy + sh, ox:ox + sw, :3] = np.clip(rgb * 255 + 0.5, 0, 255).astype(np.uint8)
            out[oy:oy + sh, ox:ox + sw, 3] = np.clip(al[..., 0] * 255 + 0.5, 0, 255).astype(np.uint8)
            f = work + '/out/%04d.png' % n
            Image.fromarray(out, 'RGBA').save(f, compress_level=1)
            files.append(f)
            n += 1
    # each frame a still WebP of the whole canvas (cwebp, in parallel), then put together with
    # webpmux, each frame drawn whole without blending: key frames all (libwebp's test), so
    # the game starts any segment at once (the figure changes all over in each frame anyway)
    from concurrent.futures import ThreadPoolExecutor
    def enc(f):
        subprocess.run(['cwebp', '-quiet', '-q', str(job['quality']), '-m', '6', '-metadata', 'none',
                        f, '-o', f[:-4] + '.webp'], check=True)
    with ThreadPoolExecutor(os.cpu_count() or 4) as ex:
        list(ex.map(enc, files))
    d = '+%d+0+0+0-b' % int(round(1000 / job['fps']))
    mux = ['webpmux']
    for f in files:
        mux += ['-frame', f[:-4] + '.webp', d]
    subprocess.run(mux + ['-loop', '0', '-bgcolor', '0,0,0,0', '-o', work + '/herald.webp'], check=True)
    sys.path.insert(0, '/')
    import check_herald
    bad = check_herald.check(work + '/herald.webp', dict(zip(check_herald.NAMES, table)), fps,
                             job.get('video') and work + '/check.mp4')
    json.dump({'x': X, 'y': Y, 'w': W, 'h': H, 'table': table, 'frames': n, 'jumps': bad},
              open(work + '/result.json', 'w'))


def main():
    if len(sys.argv) > 2 and sys.argv[1] == '--inside':
        inside(sys.argv[2])
        return
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('code')
    ap.add_argument('clips', nargs=5)
    ap.add_argument('--fps', type=int, default=12)
    ap.add_argument('--foot', default='185,470')
    ap.add_argument('--height', type=float, default=395)
    ap.add_argument('--density', type=float, default=1.5)
    ap.add_argument('--quality', type=float, default=70)
    ap.add_argument('--dest', default=DEST)
    ap.add_argument('--video', help="tools/check_herald.py's video of the segments in the game's order, written here")
    ap.add_argument('--image', help='a Docker image that already has ffmpeg, webp, python3-pil and python3-numpy')
    a = ap.parse_args()
    code = a.code.upper()
    if len(code) != 3 or not code.isalpha():
        sys.exit('CODE: the three letters of KINGS\\HRLDCODE.AVI')
    # (the frames take about 1 GB: in build/, not a small /tmp)
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    work = tempfile.mkdtemp(prefix='herald-', dir=os.path.join(ROOT, 'build'))
    try:
        clips = []
        for i, c in enumerate(a.clips):
            dst = os.path.join(work, 'clip%d%s' % (i, os.path.splitext(c)[1]))
            shutil.copy(c, dst)
            clips.append('/w/' + os.path.basename(dst))
        json.dump({'clips': clips, 'fps': a.fps, 'foot': [int(v) for v in a.foot.split(',')], 'height': a.height,
                   'density': a.density, 'quality': a.quality, 'video': bool(a.video)}, open(os.path.join(work, 'job.json'), 'w'))
        setup = '' if a.image else ('apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq '
                                    '--no-install-recommends webp=%s ffmpeg python3-pil python3-numpy >/dev/null && '
                                    % WEBP_VERSION)
        subprocess.run(['docker', 'run', '--rm', '-v', work + ':/w', '-v', os.path.abspath(__file__) + ':/b.py:ro',
                        '-v', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'check_herald.py') +
                        ':/check_herald.py:ro',
                        '-e', 'HOST_IDS=%d:%d' % (os.getuid(), os.getgid()), a.image or IMAGE, 'sh', '-c',
                        setup + 'python3 -I /b.py --inside /w; r=$?; chown -R "$HOST_IDS" /w; exit $r'], check=True)
        r = json.load(open(os.path.join(work, 'result.json')))
        name = 'herald-%s.webp' % code
        os.makedirs(a.dest, exist_ok=True)     # (data/native/hd-extra: local, not in the repository)
        shutil.copy(os.path.join(work, 'herald.webp'), os.path.join(a.dest, name))
        line = '%s %d %d %d %d %d %s %s' % (code, r['x'], r['y'], r['w'], r['h'], a.fps,
                                            ' '.join('%d %d' % tuple(t) for t in r['table']), name)
        path = os.path.join(a.dest, 'heralds.txt')
        lines = open(path).read().splitlines() if os.path.exists(path) else [
            "# OpenCiv2's own heralds (docs/HD-GRAPHICS.md, Your own heralds): "
            'CODE X Y W H FPS G0 G1 I0 I1 P0 P1 D0 D1 A0 A1 FILE']
        lines = [l for l in lines if l.startswith('#') or not l.split() or l.split()[0].upper() != code]
        open(path, 'w').write('\n'.join(lines + [line]) + '\n')
        print('%s: %d frames, %.2f MB\n%s' % (name, r['frames'], os.path.getsize(os.path.join(a.dest, name)) / 1e6,
                                             line))
        if a.video:
            shutil.copy(os.path.join(work, 'check.mp4'), a.video)
        if r['jumps']:
            sys.exit('tools/check_herald.py: %d joins jump' % len(r['jumps']))
    finally:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == '__main__':
    main()
