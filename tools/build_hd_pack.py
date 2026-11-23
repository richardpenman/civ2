#!/usr/bin/env python3
"""HD graphics (docs/HD-GRAPHICS.md): make the game's art, data/native/hd,
from Peter Johnson's runtime art pack (openciv2-assets, MIT) as built from
his repository at the commit pinned in data/hd-pack.json (repo, sha), so
what is merged there can be used without waiting for a release.  The art
is not part of this repository (.gitignore): make hd-art runs this, and the
builds that need the art (make, make port, make native, make windows, make
web; the release's jobs) run it first.

    python3 tools/build_hd_pack.py [--force] [--local] [--dest DIR] [--keep-work]

0. Nothing is done when the destination already holds the art of this pin
   and this tool (its .built: a digest of data/hd-pack.json and this file
   but its docstring); else the art made earlier from them is copied from
   the cache (hd-DIGEST: kept there after each make below), without the
   network; else it is made (1 to 5) and kept in the cache.  --force makes
   it again whatever is there.

1. The files of the pinned commit the pack is built from are fetched into
   a partial clone in the cache (${XDG_CACHE_HOME:-~/.cache}/openciv2/
   openciv2-assets.git, %LOCALAPPDATA%\\openciv2 on Windows;
   CIV2_DEPENDENCY_CACHE overrides): the commit and its trees only (git
   fetch --filter=blob:none --depth=1 SHA), then in one fetch the blobs not
   yet there of its runtime-inputs.json's pictures and data
   (assets/atlas-hd/NAME), of its builder (tools/build_atlas.py,
   runtime_selection.py, dependency.py), contracts/cityview-rects.json,
   requirements.txt and the notices (LICENSE, THIRD-PARTY.txt,
   assets/atlas-hd/PROVENANCE.md); never its authoring and review material
   nor the rest of its history.  Git checks each object against its id, so
   the commit's id pins every file; the blobs are kept (a later commit
   fetches only those it changed).  They are written to a work folder
   (build/hd-pack-*).
1b. The pack is built from them as the repository's release does
   (tools/build.py runs tools/build_atlas.py): its tools/build_atlas.py, in
   a throwaway container of the pinned Python image (BUILD_IMAGE) with the
   Pillow its requirements.txt names, installed from PyPI checked against
   the SHA-256s pinned here (PILLOW; a commit asking for another Pillow
   stops this until they are updated); it writes build/data/atlas-hd.pack.
   (Fonts, layout header and package.json, which the release adds, are not
   used here.)
2. Of that data/atlas-hd.pack (C2ATLAS1: a count, a palette, 180-byte
   entries: key, size in the game's pixels, offset and length of a PNG),
   the pictures the game uses are kept: those it draws, the keys beginning
   with one of data/hd-pack.json's "keep" and with none of its "drop"
   (src/platform/hd.c and src/game/hd_graphics.cpp draw no others), and
   those src/platform/synth.c reads to make the original's pictures where
   the game folder lacks them, the keys beginning with one of its "synth";
   never those beginning with one of its "exclude" (if it has one).
   Pictures with the same PNG are kept once.
3. Each is made no larger than the largest size the game uses it at: one
   drawn, its size in the game's pixels times MAX_DENSITY (hd.c's hd_density
   is at most 4) times MAP_ZOOM for the map's pictures (the map's zoom goes
   up to twice the size); one only read by synth.c, its size in the game's
   pixels (synth.c shrinks it to that); a larger one is shrunk (Lanczos,
   premultiplied alpha).  (The pack's sprites are 4 times the game's size,
   the map's blends, fog and shields 8, its backgrounds 2: no picture drawn
   is.)
4. Each is encoded as WebP by cwebp, pinned (CWEBP_VERSION, Debian's
   package, in a throwaway container of the pinned Debian image: IMAGE;
   Docker needed): pictures with transparent pixels of at most SMALL game
   pixels each way lossless; the others lossy at quality 90 (Q95: 95), -m 4
   -sharp_yuv, alpha lossless, unless the lossless file is at most
   LOSSLESS_MARGIN times as large (flat interface pictures, whole sheets).
   The choices come from a comparison of encoders on these pictures
   (SSIMULACRA2 at least 85 for all but the Q95 backgrounds at quality 90).
5. data/native/hd is written again: a WebP per distinct picture (named by
   its first key, "/" as "_"), pictures.txt (a line "KEY W H FILE" per key,
   the format of data/native/hd-extra's, which hd.c reads), and the
   commit's LICENSE, THIRD-PARTY.txt and assets/atlas-hd/PROVENANCE.md (as
   ATLAS-PROVENANCE.md, the release's name).

The same input gives the same files (Pillow and cwebp pinned, cwebp at one
thread).  Upgrading the art: the sha in data/hd-pack.json (git ls-remote
REPO main), then this.

Without Docker (or with --local) the pack is built with this machine's
Python and Pillow and the pictures encoded by its cwebp, with Pillow and
numpy (steps 1b and 4 as in the containers): the same pictures (another
Pillow than requirements.txt's: perhaps not to the pixel), not always the
same bytes, so not kept in the cache.
"""
import argparse, hashlib, json, os, shutil, struct, subprocess, sys, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = os.path.join(ROOT, 'data', 'hd-pack.json')
DEST = os.path.join(ROOT, 'data', 'native', 'hd')
# the commit's notices, installed beside the pictures (path -> name)
NOTICES = {'LICENSE': 'LICENSE', 'THIRD-PARTY.txt': 'THIRD-PARTY.txt',
           'assets/atlas-hd/PROVENANCE.md': 'ATLAS-PROVENANCE.md'}
# what the pack's builder reads besides runtime-inputs.json's assets/atlas-hd/NAME
BUILDER = ('runtime-inputs.json', 'requirements.txt', 'tools/build_atlas.py', 'tools/runtime_selection.py',
           'tools/dependency.py', 'contracts/cityview-rects.json')
PACK = 'build/data/atlas-hd.pack'

# the pack's builder, pinned: Python 3.13 (as the repository's release job) and the Pillow its
# requirements.txt names, the wheels' SHA-256s (x86-64, arm64) from PyPI
BUILD_IMAGE = 'python:3.13-slim-trixie@sha256:e48f59cdba69a3cbd9ed2cf0f348d90d45a8d350d98134cf1070bc106ee9ed90'
PILLOW = 'Pillow==13.0.0'
PILLOW_SHA256 = ('5f56499c1a74d2b862beb0c33b8016f99d095858455df9d21583bbbc1c15b582',
                 '19b66ca0a4d25cc0200b381ce824c7ea090db61427315e73cfb05dbf1b35c369')
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


def git(repo, *args, **kw):
    """git in the cache's partial clone, never fetching a missing object on its own (one at a time)."""
    env = dict(os.environ, GIT_NO_LAZY_FETCH='1', GIT_TERMINAL_PROMPT='0')
    return subprocess.run(['git', '--git-dir', repo] + list(args), env=env, check=True,
                          stdout=subprocess.PIPE, **kw).stdout


def fetch_source(pin, out):
    """1.: the files of the pinned commit the pack is built from, into out (fetched only where not yet in the cache)."""
    if not shutil.which('git'):
        sys.exit('build_hd_pack: git is needed to fetch the art\'s source (or build without the HD art: make HD_ART=)')
    sha = pin['sha']
    if len(sha) != 40 or any(c not in '0123456789abcdef' for c in sha):
        sys.exit('build_hd_pack: data/hd-pack.json\'s sha is not a full commit id')
    repo = os.path.join(cache_dir(), 'openciv2-assets.git')
    if not os.path.exists(os.path.join(repo, 'HEAD')):
        subprocess.run(['git', 'init', '-q', '--bare', repo], check=True)
        for k, v in (('remote.origin.url', pin['repo']), ('remote.origin.promisor', 'true'),
                     ('remote.origin.partialclonefilter', 'blob:none')):
            git(repo, 'config', k, v)
    git(repo, 'config', 'remote.origin.url', pin['repo'])
    have = lambda: subprocess.run(['git', '--git-dir', repo, 'cat-file', '-e', sha + '^{commit}'],
                                  env=dict(os.environ, GIT_NO_LAZY_FETCH='1'), stderr=subprocess.DEVNULL).returncode == 0
    if not have():
        print('fetching openciv2-assets %s (the commit and its trees)' % sha[:7], flush=True)
        git(repo, 'fetch', '-q', '--no-write-fetch-head', '--filter=blob:none', '--depth=1', 'origin', sha)
        if not have():
            sys.exit('build_hd_pack: %s has no commit %s' % (pin['repo'], sha))

    tree = git(repo, 'ls-tree', '-r', '-z', '--format=%(objecttype) %(objectname)\t%(path)', sha).decode()

    def blobs(paths):
        """{path: blob id} of the commit, the missing blobs fetched in one go."""
        wanted, ids = set(paths), {}
        for line in filter(None, tree.split('\0')):
            meta, path = line.split('\t', 1)
            kind, oid = meta.split()
            if kind == 'blob' and path in wanted:
                ids[path] = oid
        if set(paths) - set(ids):
            sys.exit('build_hd_pack: not in %s: %s' % (sha[:7], ', '.join(sorted(set(paths) - set(ids)))))
        check = git(repo, 'cat-file', '--batch-check', input='\n'.join(sorted(set(ids.values()))).encode())
        missing = [l.split()[0] for l in check.decode().splitlines() if l.endswith(' missing')]
        if missing:
            print('fetching %d files of openciv2-assets %s' % (len(missing), sha[:7]), flush=True)
            git(repo, 'fetch', '-q', '--no-write-fetch-head', 'origin', '--stdin', input='\n'.join(missing).encode())
        return ids

    def write(ids):
        for path, oid in ids.items():
            dst = os.path.join(out, *path.split('/'))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            with open(dst, 'wb') as f:
                f.write(git(repo, 'cat-file', 'blob', oid))

    write(blobs(list(BUILDER) + list(NOTICES)))
    names = json.load(open(os.path.join(out, 'runtime-inputs.json'), encoding='utf-8'))
    if not isinstance(names, list) or any(not isinstance(n, str) or '..' in n.split('/') for n in names):
        sys.exit('build_hd_pack: %s\'s runtime-inputs.json is not a list of paths' % sha[:7])
    write(blobs(['assets/atlas-hd/' + n for n in names]))
    return len(names)


def build_pack(src, local):
    """1b.: src/build/data/atlas-hd.pack, built by the commit's tools/build_atlas.py with the pinned Pillow."""
    req = [l.split('#')[0].strip() for l in open(os.path.join(src, 'requirements.txt'), encoding='utf-8')]
    if [l for l in req if l] != [PILLOW]:
        sys.exit('build_hd_pack: the commit\'s requirements.txt asks for %s, not %s: update PILLOW, '
                 'PILLOW_SHA256 and BUILD_IMAGE in this tool' % (', '.join(l for l in req if l), PILLOW))
    print('building the pack (its tools/build_atlas.py, %s)' % PILLOW, flush=True)
    if local:
        import PIL
        if 'Pillow==' + PIL.__version__ != PILLOW:
            print('build_hd_pack: this machine\'s Pillow is %s, not %s: the pictures may differ slightly'
                  % (PIL.__version__, PILLOW), flush=True)
        subprocess.run([sys.executable, os.path.join(src, 'tools', 'build_atlas.py')], check=True,
                       stdout=subprocess.DEVNULL)
    else:
        with open(os.path.join(src, 'pinned-requirements.txt'), 'w') as f:
            f.write(PILLOW + ''.join(' --hash=sha256:' + h for h in PILLOW_SHA256) + '\n')
        subprocess.run(['docker', 'run', '--rm', '-v', src + ':/w', '-e', 'HOST_IDS=%d:%d' % (os.getuid(), os.getgid()),
                        '-e', 'PYTHONWARNINGS=ignore::DeprecationWarning', BUILD_IMAGE, 'sh', '-c',
                        'pip install -q --root-user-action=ignore --disable-pip-version-check --no-deps '
                        '--require-hashes -r /w/pinned-requirements.txt && python /w/tools/build_atlas.py >/dev/null; '
                        'r=$?; chown -R "$HOST_IDS" /w; exit $r'], check=True)
    return os.path.join(src, *PACK.split('/'))


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
            print('build_hd_pack: no Docker (or --local): this machine\'s Python, Pillow and cwebp make the art', flush=True)
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
                elif len(old.split('.')[0]) == 64 and old.endswith(('.zip', '.png', '.md')):
                    os.remove(os.path.join(cache_dir(), old))    # (the downloads of releases, before commits were pinned)
    with open(stamp, 'w') as f:
        f.write(digest + '\n')


def build(args, local=False):
    dest = args.dest
    pin = json.load(open(PIN))
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    work = tempfile.mkdtemp(prefix='hd-pack-', dir=os.path.join(ROOT, 'build'))
    try:
        src = os.path.join(work, 'src')
        n = fetch_source(pin, src)
        print('openciv2-assets %s: %d inputs' % (pin['sha'][:7], n), flush=True)
        pack = build_pack(src, local)
        entries = read_pack(pack, lambda k: used(k, pin))
        os.makedirs(os.path.join(work, 'notices'))
        for path, name in NOTICES.items():
            shutil.copy(os.path.join(src, *path.split('/')), os.path.join(work, 'notices', name))
        shutil.rmtree(src)
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
            f.write('# The HD graphics\' art (docs/HD-GRAPHICS.md): openciv2-assets by Peter Johnson (MIT: LICENSE)\n'
                    '# at commit %s, its pictures the game draws, made by tools/build_hd_pack.py (WebP).\n'
                    '# A line per key: KEY W H FILE (the picture\'s size in the game\'s pixels).\n' % pin['sha'])
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
    ap.add_argument('--local', action='store_true', help='build the pack and encode with this machine\'s Python, Pillow and cwebp, not in Docker')
    ap.add_argument('--dest', default=DEST, help='where to write (default data/native/hd)')
    ap.add_argument('--keep-work', action='store_true', help='keep the work folder (build/hd-pack-*)')
    ensure(ap.parse_args())


if __name__ == '__main__':
    main()
