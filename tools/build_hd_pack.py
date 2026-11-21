#!/usr/bin/env python3
"""HD graphics (docs/HD-GRAPHICS.md): make the game's art, data/native/hd,
from Peter Johnson's runtime art pack (openciv2-assets, MIT) at the release
pinned in data/hd-pack.json, with the pictures added to it since, from its
repository at the commit pinned there.  The art is not part of this
repository (.gitignore): make hd-art runs this, and the builds that need
the art (make, make port, make native, make windows, make web; the
release's jobs) run it first.

    python3 tools/build_hd_pack.py [--force] [--local] [--zip FILE] [--checkout DIR] [--dest DIR] [--keep-work]

0. Nothing is done when the destination already holds the art of this pin
   and this tool (its .built: a digest of data/hd-pack.json and this file
   but its docstring); else the art made earlier from them is copied from
   the cache (hd-DIGEST: kept there after each make below), without the
   network; else it is made (1 to 5) and kept in the cache.  --force makes
   it again whatever is there.

1. The release's archive is downloaded (url) into the cache
   (${XDG_CACHE_HOME:-~/.cache}/openciv2, %LOCALAPPDATA%\\openciv2 on Windows;
   CIV2_DEPENDENCY_CACHE overrides; named by its SHA-256, so fetched once) or
   taken from --zip, and checked: the pinned SHA-256 and size, then each file
   used against the SHA-256 and size its package.json lists.
1b. The pictures added since the release (data/hd-pack.json's "commit": the
   city view's houses of the other eras, its medieval and modern
   landscapes) are each downloaded from the repository at the pinned commit
   (raw.githubusercontent.com, into the cache, named by its SHA-256) or
   taken from a clone (--checkout: git show SHA:PATH), and checked against
   the SHA-256 and size pinned for it; each is a picture of the pack (its
   key, its size in the game's pixels, the PNG unchanged, as the pack's
   builder adds a binding), in place of the release's of that key if any;
   the commit's notices in place of the release's of those names.
2. Of its data/atlas-hd.pack (C2ATLAS1: a count, a palette, 180-byte entries:
   key, size in the game's pixels, offset and length of a PNG), the pictures
   the game uses are kept: those it draws, the keys beginning with one of
   data/hd-pack.json's "keep" and with none of its "drop" (src/platform/hd.c
   and src/game/hd_graphics.cpp draw no others), and those
   src/platform/synth.c reads to make the original's pictures where the game
   folder lacks them, the keys beginning with one of its "synth"; never
   those beginning with one of its "exclude" (if it has one).  Pictures with the same PNG are kept once.
3. Each is made no larger than the largest size the game uses it at: one
   drawn, its size in the game's pixels times MAX_DENSITY (hd.c's hd_density
   is at most 4) times MAP_ZOOM for the map's pictures (the map's zoom goes
   up to twice the size); one only read by synth.c, its size in the game's
   pixels (synth.c shrinks it to that); a larger one is shrunk (Lanczos,
   premultiplied alpha).  (With release v2026.11.08.1 no picture drawn is:
   its sprites are 4 times the game's size, the map's blends, fog and
   shields 8, its backgrounds 2.)
4. Each is encoded as WebP by cwebp, pinned (CWEBP_VERSION, Debian's
   package, in a throwaway container of the pinned Debian image; Docker
   needed): pictures with transparent pixels of at most SMALL game pixels
   each way lossless; the others lossy at quality 90 (Q95: 95), -m 4
   -sharp_yuv, alpha lossless, unless the lossless file is at most
   LOSSLESS_MARGIN times as large (flat interface pictures, whole sheets).
   The choices come from a comparison of encoders on these pictures
   (SSIMULACRA2 at least 85 for all but the Q95 backgrounds at quality 90).
5. data/native/hd is written again: a WebP per distinct picture (named by
   its first key, "/" as "_"), pictures.txt (a line "KEY W H FILE" per key,
   the format of data/native/hd-extra's, which hd.c reads), and the
   release's LICENSE, THIRD-PARTY.txt and ATLAS-PROVENANCE.md.

The same input gives the same files (cwebp is deterministic at one thread).
Upgrading the art: tag, url, sha256 and size in data/hd-pack.json (and its
"commit": the sha and each picture's path, w, h, sha256 and size from the
repository's assets/atlas-hd/compatibility/bindings.json there; removed
when a release has them all), then this.

Without Docker (or with --local) the pictures are encoded by this machine's
cwebp, with Python's PIL and numpy (step 4 as in the container): the same
pictures, not always the same bytes (another cwebp), so not kept in the
cache.
"""
import argparse, hashlib, io, json, os, shutil, struct, subprocess, sys, tempfile, urllib.request, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = os.path.join(ROOT, 'data', 'hd-pack.json')
DEST = os.path.join(ROOT, 'data', 'native', 'hd')
# the release's notices, installed beside the pictures (archive member -> name)
NOTICES = {'LICENSE': 'LICENSE', 'THIRD-PARTY.txt': 'THIRD-PARTY.txt',
           'data/ATLAS-PROVENANCE.md': 'ATLAS-PROVENANCE.md'}
PACK = 'data/atlas-hd.pack'

# the encoder, pinned: Debian 13's image and its webp package
IMAGE = 'debian:trixie@sha256:9c6e381421d0f48d1b8133e573f33c185ee16cbf505eda9c4a8a4791b25ea8a6'
CWEBP_VERSION = '1.5.0-0.1'

MAX_DENSITY = 4             # hd.c: hd_density 1 to 4
MAP_ZOOM = 2                # the map's largest zoom (+8: 16/8)
# the pictures the map draws (zoomed): its sheets' sprites, their tints, the blends, fog, the shield
MAP = ('crop/file/terrain1/', 'crop/file/terrain2/', 'crop/file/units/', 'crop/file/cities/',
       'tint/crop/file/terrain1/', 'tint/crop/file/terrain2/', 'tint/crop/file/units/', 'tint/crop/file/cities/',
       'dither/', 'fog/', 'shield')
SMALL = 53                  # (3 x 53 <= 160 screen pixels at density 3)
LOSSLESS_MARGIN = 1.15
# the backgrounds below SSIMULACRA2 85 at quality 90: the throne room's hall,
# the spaceship's window, the diplomacy rooms, the credits
Q95 = ('resource/pv.dll/gifs/100', 'resource/ss.dll/gifs/499', 'resource/mk.dll/gifs/20',
       'resource/tiles.dll/gifs/90')


def cache_dir():
    d = os.environ.get('CIV2_DEPENDENCY_CACHE')
    if not d:
        if os.name == 'nt' and os.environ.get('LOCALAPPDATA'):
            d = os.path.join(os.environ['LOCALAPPDATA'], 'openciv2')
        else:
            d = os.path.join(os.environ.get('XDG_CACHE_HOME') or os.path.expanduser('~/.cache'), 'openciv2')
    os.makedirs(d, exist_ok=True)
    return d


def sha256_file(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def download(url, sha, path):
    if os.path.exists(path) and sha256_file(path) == sha:
        return
    print('downloading %s' % url, flush=True)
    tmp = path + '.part'
    with urllib.request.urlopen(url) as r, open(tmp, 'wb') as f:
        shutil.copyfileobj(r, f, 1 << 20)
    got = sha256_file(tmp)
    if got != sha:
        os.remove(tmp)
        sys.exit('build_hd_pack: %s has SHA-256 %s, not the pinned %s' % (url, got, sha))
    os.replace(tmp, path)


def extract(z, manifest, member, out):
    """member of the archive into out, checked against package.json."""
    want = manifest['files'][member]
    h = hashlib.sha256()
    with z.open(member) as src, open(out, 'wb') as dst:
        for block in iter(lambda: src.read(1 << 20), b''):
            h.update(block)
            dst.write(block)
    if h.hexdigest() != want['sha256'] or os.path.getsize(out) != want['size']:
        sys.exit('build_hd_pack: %s does not match its package.json' % member)


def read_pack(path, wanted):
    """[(key, w, h, png bytes)] of a C2ATLAS1 pack, of the keys wanted(key)."""
    out = []
    with open(path, 'rb') as f:
        head = f.read(12)
        if head[:8] != b'C2ATLAS1':
            sys.exit('build_hd_pack: %s is not a C2ATLAS1 pack' % path)
        n = struct.unpack('<I', head[8:])[0]
        f.seek(768, 1)
        index = [f.read(180) for _ in range(n)]
        for e in index:
            key = e[:160].split(b'\0')[0].decode()
            w, h, off, ln = struct.unpack('<4I', e[160:176])
            if not wanted(key):
                continue
            f.seek(off)
            out.append((key, w, h, f.read(ln)))
    return out


def fetch_commit(pin, checkout, item):
    """the bytes of a file of the pinned commit (item: path, sha256, size), checked."""
    c = pin['commit']
    if checkout:
        data = subprocess.run(['git', '-C', checkout, 'show', '%s:%s' % (c['sha'], item['path'])],
                              capture_output=True, check=True).stdout
    else:
        path = os.path.join(cache_dir(), item['sha256'] + os.path.splitext(item['path'])[1])
        download(c['raw'].format(sha=c['sha'], path=item['path']), item['sha256'], path)
        data = open(path, 'rb').read()
    if hashlib.sha256(data).hexdigest() != item['sha256'] or len(data) != item['size']:
        sys.exit('build_hd_pack: %s at %s is not the pinned file' % (item['path'], c['sha']))
    return data


def drawn(key, pin):
    return key.startswith(tuple(pin['keep'])) and not key.startswith(tuple(pin['drop']))


def used(key, pin):
    if key.startswith(tuple(pin.get('exclude', ()))):
        return False            # never in the art
    return drawn(key, pin) or key.startswith(tuple(pin['synth']))


def file_name(key):
    return key.replace('/', '_') + '.webp'


def art_digest():
    """the digest naming the art made from this pin with this tool."""
    h = hashlib.sha256(open(PIN, 'rb').read())
    code = open(os.path.abspath(__file__), 'rb').read()
    h.update(code[code.index(b'\nimport '):])        # (not the docstring)
    return h.hexdigest()[:16]


def copy_art(src, dest):
    """the art files of src into dest, those of an earlier art removed."""
    os.makedirs(dest, exist_ok=True)
    for name in os.listdir(dest):
        if name.endswith('.webp') or name in ('pictures.txt', 'atlas-hd.pack', 'installed.json', '.built'):
            os.remove(os.path.join(dest, name))
    for name in os.listdir(src):
        if name != '.built':
            shutil.copy(os.path.join(src, name), os.path.join(dest, name))


def ensure(args):
    """0.: the art in args.dest, from what is there, the cache or made."""
    digest = art_digest()
    stamp = os.path.join(args.dest, '.built')
    rel = os.path.relpath(args.dest, ROOT)
    if not args.force and os.path.exists(os.path.join(args.dest, 'pictures.txt')) and \
            os.path.exists(stamp) and open(stamp).read().strip() == digest:
        print('%s: the HD art up to date (%s)' % (rel, digest), flush=True)
        return
    cached = os.path.join(cache_dir(), 'hd-' + digest)
    if not args.force and os.path.exists(os.path.join(cached, 'pictures.txt')):
        copy_art(cached, args.dest)
        print('%s: the HD art copied from %s' % (rel, cached), flush=True)
    else:
        local = args.local or not shutil.which('docker')
        if local:
            print('build_hd_pack: no Docker (or --local): this machine\'s cwebp encodes the art', flush=True)
        build(args, local)
        if not local:
            tmp = cached + '.part'
            shutil.rmtree(tmp, ignore_errors=True)
            shutil.copytree(args.dest, tmp, ignore=shutil.ignore_patterns('.built'))
            shutil.rmtree(cached, ignore_errors=True)
            os.replace(tmp, cached)
            for old in os.listdir(cache_dir()):          # (the art of other pins: not kept)
                if old.startswith('hd-') and old != 'hd-' + digest:
                    shutil.rmtree(os.path.join(cache_dir(), old), ignore_errors=True)
    with open(stamp, 'w') as f:
        f.write(digest + '\n')


def build(args, local=False):
    dest = args.dest
    pin = json.load(open(PIN))
    archive = args.zip or os.path.join(cache_dir(), pin['sha256'] + '.zip')
    if args.zip:
        if sha256_file(args.zip) != pin['sha256']:
            sys.exit('build_hd_pack: %s is not the pinned archive (%s)' % (args.zip, pin['tag']))
    else:
        download(pin['url'], pin['sha256'], archive)
    if os.path.getsize(archive) != pin['size']:
        sys.exit('build_hd_pack: %s is not %d bytes' % (archive, pin['size']))
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    work = tempfile.mkdtemp(prefix='hd-pack-', dir=os.path.join(ROOT, 'build'))
    try:
        with zipfile.ZipFile(archive) as z:
            manifest = json.loads(z.read('package.json'))
            if manifest.get('kind') != 'openciv2-assets' or manifest.get('schema') != 1:
                sys.exit('build_hd_pack: not an openciv2-assets runtime pack (schema 1)')
            os.makedirs(os.path.join(work, 'notices'))
            for member, name in NOTICES.items():
                extract(z, manifest, member, os.path.join(work, 'notices', name))
            extract(z, manifest, PACK, os.path.join(work, 'atlas-hd.pack'))
        entries = read_pack(os.path.join(work, 'atlas-hd.pack'), lambda k: used(k, pin))
        os.remove(os.path.join(work, 'atlas-hd.pack'))
        # the pictures added since the release, from the pinned commit
        if pin.get('commit'):
            added = {k: p for k, p in pin['commit']['pictures'].items() if used(k, pin)}
            entries = [e for e in entries if e[0] not in added]
            entries += [(k, p['w'], p['h'], fetch_commit(pin, args.checkout, p)) for k, p in added.items()]
            for name, item in pin['commit'].get('notices', {}).items():
                open(os.path.join(work, 'notices', name), 'wb').write(fetch_commit(pin, args.checkout, item))
        # the distinct pictures, each with its keys
        pictures, by_png = [], {}
        for key, w, h, png in sorted(entries):
            digest = hashlib.sha256(png).hexdigest()
            if digest not in by_png:
                by_png[digest] = {'id': len(pictures), 'keys': [], 'file': file_name(key)}
                pictures.append(by_png[digest])
                open(os.path.join(work, '%d.png' % by_png[digest]['id']), 'wb').write(png)
            p = by_png[digest]
            p['keys'].append([key, w, h])
        for p in pictures:
            keys = [k for k, _, _ in p['keys']]
            # (the largest size used: drawn, the density's and the zoom's; else synth.c's, the game's)
            scale = max((MAX_DENSITY * (MAP_ZOOM if k.startswith(MAP) else 1) if drawn(k, pin) else 1)
                        for k in keys)
            p['max_w'] = max(w for _, w, _ in p['keys']) * scale
            p['max_h'] = max(h for _, _, h in p['keys']) * scale
            p['small'] = max(max(w, h) for _, w, h in p['keys']) <= SMALL
            p['q'] = 95 if any(k.startswith(Q95) for k in keys) else 90
        json.dump(pictures, open(os.path.join(work, 'jobs.json'), 'w'))
        print('%d keys, %d distinct pictures: encoding (cwebp %s in %s)' % (len(entries), len(pictures),
                                                                          CWEBP_VERSION, IMAGE), flush=True)
        script = os.path.abspath(__file__)
        if local:
            if not shutil.which('cwebp'):
                sys.exit('build_hd_pack: neither Docker nor cwebp: install one (cwebp: the webp package, '
                         'with Python\'s PIL and numpy), or build without the HD art (make HD_ART=)')
            encode(work)
        else:
            subprocess.run(['docker', 'run', '--rm', '-v', work + ':/w', '-v', script + ':/build_hd_pack.py:ro',
                            '-e', 'HOST_IDS=%d:%d' % (os.getuid(), os.getgid()), IMAGE, 'sh', '-c',
                            'apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq '
                            '--no-install-recommends webp=%s python3-pil python3-numpy >/dev/null && '
                            'python3 -I /build_hd_pack.py --encode /w; r=$?; chown -R "$HOST_IDS" /w; exit $r'
                            % CWEBP_VERSION], check=True)
        done = json.load(open(os.path.join(work, 'done.json')))
        # data/native/hd written again
        os.makedirs(dest, exist_ok=True)
        for name in os.listdir(dest):
            if name.endswith('.webp') or name in ('pictures.txt', 'atlas-hd.pack', 'installed.json', '.built'):
                os.remove(os.path.join(dest, name))
        for name in NOTICES.values():
            shutil.copy(os.path.join(work, 'notices', name), os.path.join(dest, name))
        lines = []
        for p in pictures:
            shutil.copy(os.path.join(work, '%d.webp' % p['id']), os.path.join(dest, p['file']))
            lines += ['%s %d %d %s\n' % (k, w, h, p['file']) for k, w, h in p['keys']]
        with open(os.path.join(dest, 'pictures.txt'), 'w', newline='\n') as f:
            f.write('# The HD graphics\' art (docs/HD-GRAPHICS.md): openciv2-assets %s%s by Peter Johnson (MIT: LICENSE),\n'
                    '# its pictures the game draws, made by tools/build_hd_pack.py (WebP).  A line per key:\n'
                    '# KEY W H FILE (the picture\'s size in the game\'s pixels).\n'
                    % (pin['tag'], ' and commit %s' % pin['commit']['sha'][:7] if pin.get('commit') else ''))
            f.writelines(sorted(lines))
        total = sum(d['bytes'] for d in done.values())
        kinds = {}
        for d in done.values():
            kinds.setdefault(d['how'], [0, 0])
            kinds[d['how']][0] += 1
            kinds[d['how']][1] += d['bytes']
        print('wrote %s: %d pictures, %d keys, %.2f MB (%s); %d shrunk' % (
            os.path.relpath(dest, ROOT), len(pictures), len(lines), total / 1e6,
            ', '.join('%s %d, %.2f MB' % (k, n, b / 1e6) for k, (n, b) in sorted(kinds.items())),
            sum(1 for d in done.values() if d['shrunk'])))
    finally:
        if args.keep_work:
            print('work: %s' % work)
        else:
            shutil.rmtree(work, ignore_errors=True)


def encode(work):
    """(in the container) work/N.png -> work/N.webp for each job; done.json."""
    from concurrent.futures import ThreadPoolExecutor
    from PIL import Image
    import numpy as np
    pictures = json.load(open(os.path.join(work, 'jobs.json')))

    def cwebp(src, out, lossless, q):
        opts = ['-lossless', '-z', '6', '-exact'] if lossless else \
            ['-q', str(q), '-m', '4', '-sharp_yuv', '-alpha_q', '100']
        subprocess.run(['cwebp', '-quiet'] + opts + ['-o', out, src], check=True)
        return os.path.getsize(out)

    def one(p):
        n = p['id']
        im = Image.open(os.path.join(work, '%d.png' % n)).convert('RGBA')
        alpha = bool((np.asarray(im)[:, :, 3] < 255).any())
        w, h = im.size
        shrunk = w > p['max_w'] or h > p['max_h']
        if shrunk:
            k = min(p['max_w'] / w, p['max_h'] / h)
            im = im.resize((max(1, round(w * k)), max(1, round(h * k))), Image.LANCZOS)  # (RGBA: premultiplied)
        if not alpha:
            im = im.convert('RGB')
        src = os.path.join(work, '%d.in.png' % n)
        im.save(src, compress_level=1)
        out = os.path.join(work, '%d.webp' % n)
        if alpha and p['small']:
            how, size = 'lossless (small)', cwebp(src, out, True, 0)
        else:
            lossy = cwebp(src, out + '.lossy', False, p['q'])
            ll = cwebp(src, out, True, 0)
            if ll <= LOSSLESS_MARGIN * lossy:
                how, size = 'lossless', ll
                os.remove(out + '.lossy')
            else:
                how, size = 'q%d' % p['q'], lossy
                os.replace(out + '.lossy', out)
        os.remove(src)
        return str(n), {'how': how, 'bytes': size, 'shrunk': shrunk, 'size': list(im.size)}

    with ThreadPoolExecutor(os.cpu_count() or 1) as ex:
        done = dict(ex.map(one, pictures))
    json.dump(done, open(os.path.join(work, 'done.json'), 'w'))


def main():
    if len(sys.argv) == 3 and sys.argv[1] == '--encode':
        encode(sys.argv[2])
        return
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--force', action='store_true', help='make the art again, whatever the destination and the cache hold')
    ap.add_argument('--local', action='store_true', help='encode with this machine\'s cwebp, not in Docker')
    ap.add_argument('--zip', help='the release\'s archive already downloaded (still checked against the pin)')
    ap.add_argument('--checkout', help='a clone of openciv2-assets to take the pinned commit\'s files from '
                    '(instead of downloading them; still checked against the pin)')
    ap.add_argument('--dest', default=DEST, help='where to write (default data/native/hd)')
    ap.add_argument('--keep-work', action='store_true', help='keep the work folder (build/hd-pack-*)')
    ensure(ap.parse_args())


if __name__ == '__main__':
    main()
