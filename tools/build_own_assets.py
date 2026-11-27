#!/usr/bin/env python3
"""OpenCiv2's own assets: make data/native/music (docs/NO-INSTALL.md, Music:
the tracks the game plays as Civilization II's CD music, with HD graphics
first, with Classic graphics where its folder lacks Music\\TrackNN.ogg or
.wav: src/platform/sound.c: cd_open_track), data/native/sound
(docs/NO-INSTALL.md, Sound effects: name.ogg, played as the game's
Sound\\NAME.WAV, with HD graphics first, with Classic graphics where its
folder lacks it: sound.c: sound_fallback; their credits SOURCES.md) and
data/native/heralds (docs/HD-GRAPHICS.md, Your own heralds: the diplomacy
room's animated heralds, src/platform/hd.c: hd_herald_find) and
data/native/wonders (docs/NO-INSTALL.md, Wonder movies: wonderNN.webp, the
picture, and wonderNN.ogg, the sound, played together as the game's
Video\\WONDERNN.AVI, with HD graphics first, with Classic graphics where its
folder lacks it: src/platform/video.c: open_own_wonder) and
data/native/portraits (docs/HD-GRAPHICS.md, Leaders: the historical
leaders' portraits, leader-ID.webp for the original's mk.dll ID, drawn
with HD graphics for the tribe's leader: src/platform/hd.c:
load_portraits; their credits README.md) from OpenCiv2's
repository of assets openciv2/openciv2-assets, its assets/music,
assets/sound, assets/heralds, assets/wonders and assets/portraits-historical
(MIT, as OpenCiv2: its LICENSE-media; the portraits' sources public domain
or CC0, six AI-generated: their README.md; the same repository's assets/atlas-hd is the HD art, Peter
Johnson's: tools/build_hd_pack.py) at the commit pinned in data/own-assets.json (repo, sha, and each file's
path, SHA-256 and size).  They are not part of this repository
(.gitignore): make own-assets runs this, and the builds that pack or run
with them (make, make port, make native, make windows, make web; the
release's jobs) run it first.

    python3 tools/build_own_assets.py [--force] [--music DIR] [--heralds DIR] [--sound DIR]
        [--wonders DIR] [--portraits DIR]

0. Nothing is done when the five destinations already hold what this pin and
   this tool make (their .built: a digest of data/own-assets.json and this
   file but its docstring); else what was made earlier from them is copied
   from the cache (own-DIGEST: kept there after each make below), without
   the network; else it is made (1 and 2) and kept in the cache.  --force
   makes it again whatever is there.
1. Each file the pin lists is fetched at the pinned commit from
   raw.githubusercontent.com into the cache (${XDG_CACHE_HOME:-~/.cache}/
   openciv2/own-assets/SHA256, %LOCALAPPDATA%\\openciv2 on Windows;
   CIV2_DEPENDENCY_CACHE overrides) and checked against the SHA-256 and size
   pinned for it: a file that is not is not used; one already in the cache
   is not fetched again.  Only the files listed (about 51 MB): the music's
   tracks 02 to 12 (the CD's audio tracks 2 to 12), the heralds' heralds.txt
   and the WebPs it names (not the clips they were made from), the 78 sound
   effects and their SOURCES.md, the wonder movies' WebPs and Oggs (not
   their sources), the 42 historical portraits (WebP, 900x1100, about
   16 MB) and their README.md, the LICENSE-media (copied as LICENSE).
2. They are copied as they are, nothing converted: the repository holds the
   music as the game plays it (Ogg Vorbis, 48 kHz stereo, which stb_vorbis,
   the game's decoder, reads; each track's header checked; the sound
   effects too, mono, which sound.c turns into the WAVs the game reads), so
   the build needs nothing but Python 3 (its standard library) and the
   network once.  data/native/music (trackNN.ogg, README.txt, LICENSE),
   data/native/heralds (heralds.txt, its WebPs, LICENSE),
   data/native/sound (name.ogg, SOURCES.md) and data/native/wonders
   (wonderNN.webp, wonderNN.ogg, LICENSE; each WebP's and Ogg's header
   checked) and data/native/portraits (leader-ID.webp, README.md, LICENSE;
   each WebP's header checked) are written again.
"""
import argparse, hashlib, json, os, shutil, sys, urllib.parse, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = os.path.join(ROOT, 'data', 'own-assets.json')
MUSIC = os.path.join(ROOT, 'data', 'native', 'music')
HERALDS = os.path.join(ROOT, 'data', 'native', 'heralds')
SOUND = os.path.join(ROOT, 'data', 'native', 'sound')
WONDERS = os.path.join(ROOT, 'data', 'native', 'wonders')
PORTRAITS = os.path.join(ROOT, 'data', 'native', 'portraits')

SKIP = 'or build without them: make OWN_ASSETS='

README = """OpenCiv2's own music (docs/NO-INSTALL.md, Music)

The game plays trackNN.ogg as the CD's audio track NN where its folder has
no Music\\TrackNN.ogg or Music\\TrackNN.wav (src/platform/sound.c).

From OpenCiv2's repository of assets openciv2/openciv2-assets
({repo}), its assets/music, at commit {sha}:
{tracks}
copied as they are by tools/build_own_assets.py.

Copyright (c) 2026 Richard Penman (the OpenCiv2 project); MIT licence:
LICENSE.
"""


def cache_dir():
    d = os.environ.get('CIV2_DEPENDENCY_CACHE')
    if not d:
        if os.name == 'nt' and os.environ.get('LOCALAPPDATA'):
            d = os.path.join(os.environ['LOCALAPPDATA'], 'openciv2')
        else:
            d = os.path.join(os.environ.get('XDG_CACHE_HOME') or os.path.expanduser('~/.cache'), 'openciv2')
    os.makedirs(d, exist_ok=True)
    return d


def load_pin():
    pin = json.load(open(PIN, encoding='utf-8'))
    sha = pin['sha']
    if len(sha) != 40 or any(c not in '0123456789abcdef' for c in sha):
        sys.exit('build_own_assets: data/own-assets.json\'s sha is not a full commit id')
    if not pin['repo'].startswith('https://github.com/'):
        sys.exit('build_own_assets: data/own-assets.json\'s repo is not on GitHub')
    for n in pin['music']:
        if not (len(n) == 2 and n.isdigit() and 2 <= int(n) <= 99):
            sys.exit('build_own_assets: data/own-assets.json\'s music has a track %s' % n)
    for name in pin['sound']:
        if not (name == 'SOURCES.md' or (name.endswith('.ogg') and name == name.lower() and len(name) <= 12)):
            sys.exit('build_own_assets: data/own-assets.json\'s sound has a file %s' % name)
    for name in pin['wonders']:
        if not (len(name) in (12, 13) and name.startswith('wonder') and name[6:8].isdigit() and name[8:] in ('.webp', '.ogg')):
            sys.exit('build_own_assets: data/own-assets.json\'s wonders has a file %s' % name)
    for name in pin['portraits']:
        if not (name == 'README.md' or (len(name) == 15 and name.startswith('leader-') and name[7:10].isdigit() and
                                        220 <= int(name[7:10]) <= 261 and name.endswith('.webp'))):
            sys.exit('build_own_assets: data/own-assets.json\'s portraits has a file %s' % name)
    for group in ('music', 'heralds', 'sound', 'wonders', 'portraits', 'notices'):
        for name, f in pin[group].items():
            if '..' in f['path'].split('/') or len(f['sha256']) != 64 or '/' in name or name.startswith('.'):
                sys.exit('build_own_assets: data/own-assets.json\'s %s %s is not as it should be' % (group, name))
    return pin


def sha256_of(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def fetch(pin, f):
    """1.: the file f ({path, sha256, size}) of the pinned commit, its path in the cache."""
    src = os.path.join(cache_dir(), 'own-assets')
    os.makedirs(src, exist_ok=True)
    path = os.path.join(src, f['sha256'])
    if os.path.exists(path) and os.path.getsize(path) == f['size'] and sha256_of(path) == f['sha256']:
        return path
    url = 'https://raw.githubusercontent.com/%s/%s/%s' % (
        pin['repo'][len('https://github.com/'):].rstrip('/'), pin['sha'], urllib.parse.quote(f['path']))
    print('fetching %s (%.1f MB)' % (f['path'], f['size'] / 1e6), flush=True)
    tmp = path + '.part'
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'openciv2-build'})
        with urllib.request.urlopen(req, timeout=120) as r, open(tmp, 'wb') as out:
            shutil.copyfileobj(r, out)
    except OSError as e:
        sys.exit('build_own_assets: cannot fetch %s: %s (%s)' % (url, e, SKIP))
    if os.path.getsize(tmp) != f['size'] or sha256_of(tmp) != f['sha256']:
        os.remove(tmp)
        sys.exit('build_own_assets: %s is not the file pinned in data/own-assets.json (SHA-256 or size)' % url)
    os.replace(tmp, path)
    return path


def vorbis_info(path):
    """(channels, rate) of an Ogg Vorbis file's identification header, else None."""
    with open(path, 'rb') as f:
        head = f.read(4096)
    if head[:4] != b'OggS':
        return None
    at = 27 + head[26]           # (the first page: its segment table, then the packet)
    if head[at:at + 7] != b'\x01vorbis':
        return None
    return head[at + 11], int.from_bytes(head[at + 12:at + 16], 'little')


def clear(dest):
    os.makedirs(dest, exist_ok=True)
    for name in os.listdir(dest):
        if os.path.isfile(os.path.join(dest, name)):
            os.remove(os.path.join(dest, name))


def is_webp_anim(path):
    """whether the file is an animated WebP (RIFF WEBP, its VP8X header's animation flag)."""
    with open(path, 'rb') as f:
        head = f.read(30)
    return head[:4] == b'RIFF' and head[8:16] == b'WEBPVP8X' and bool(head[20] & 2)


def is_webp(path):
    """whether the file is a WebP (RIFF WEBP)."""
    with open(path, 'rb') as f:
        head = f.read(12)
    return head[:4] == b'RIFF' and head[8:12] == b'WEBP'


def build(music, heralds, sound, wonders, portraits):
    """1. and 2.: fetched (or from the cache's own-assets) and copied into music, heralds, sound, wonders and portraits."""
    pin = load_pin()
    tracks = {n: fetch(pin, t) for n, t in sorted(pin['music'].items())}
    herald_files = {name: fetch(pin, f) for name, f in pin['heralds'].items()}
    sound_files = {name: fetch(pin, f) for name, f in sorted(pin['sound'].items())}
    wonder_files = {name: fetch(pin, f) for name, f in sorted(pin['wonders'].items())}
    portrait_files = {name: fetch(pin, f) for name, f in sorted(pin['portraits'].items())}
    notices = {name: fetch(pin, f) for name, f in pin['notices'].items()}
    # (every WebP heralds.txt names pinned too)
    named = [l.split()[-1] for l in open(herald_files['heralds.txt'], encoding='utf-8')
             if l.strip() and not l.startswith('#')]
    if set(named) - set(herald_files):
        sys.exit('build_own_assets: heralds.txt names %s, not pinned in data/own-assets.json'
                 % ', '.join(sorted(set(named) - set(herald_files))))
    infos = {}
    for n, path in tracks.items():
        infos[n] = vorbis_info(path)
        if not infos[n] or infos[n][0] != 2:
            sys.exit('build_own_assets: track %s (%s) is not Ogg Vorbis, stereo' % (n, pin['music'][n]['path']))
    effects = [name for name in sound_files if name.endswith('.ogg')]
    for name in effects:
        if not vorbis_info(sound_files[name]):
            sys.exit('build_own_assets: sound %s (%s) is not Ogg Vorbis' % (name, pin['sound'][name]['path']))
    for name, path in wonder_files.items():
        if not (is_webp_anim(path) if name.endswith('.webp') else vorbis_info(path)):
            sys.exit('build_own_assets: wonders %s (%s) is not an animated WebP or Ogg Vorbis' % (name, pin['wonders'][name]['path']))
    for name, path in portrait_files.items():
        if name.endswith('.webp') and not is_webp(path):
            sys.exit('build_own_assets: portraits %s (%s) is not a WebP' % (name, pin['portraits'][name]['path']))
    clear(music)
    for n, path in tracks.items():
        shutil.copy(path, os.path.join(music, 'track%s.ogg' % n))
    with open(os.path.join(music, 'README.txt'), 'w', newline='\n') as f:
        f.write(README.format(repo=pin['repo'], sha=pin['sha'], tracks=''.join(
            '  track%s.ogg  %s\n' % (n, t.get('title', t['path'])) for n, t in sorted(pin['music'].items()))))
    clear(heralds)
    for name, path in herald_files.items():
        shutil.copy(path, os.path.join(heralds, name))
    clear(wonders)
    for name, path in wonder_files.items():
        shutil.copy(path, os.path.join(wonders, name))
    clear(portraits)
    for name, path in portrait_files.items():
        shutil.copy(path, os.path.join(portraits, name))
    for dest in (music, heralds, wonders, portraits):
        for name, path in notices.items():
            shutil.copy(path, os.path.join(dest, name))
    clear(sound)
    for name, path in sound_files.items():
        shutil.copy(path, os.path.join(sound, name))
    rate = sorted({info[1] for info in infos.values()})
    movies = [name for name in wonder_files if name.endswith('.webp')]
    print('wrote %s: %d tracks, %.1f MB (Ogg Vorbis, stereo, %s Hz); %s: %d heralds, %.1f MB; %s: %d sound effects, %.1f MB; '
          '%s: %d wonder movies, %.1f MB; %s: %d portraits, %.1f MB' % (
        os.path.relpath(music, ROOT), len(tracks), sum(os.path.getsize(p) for p in tracks.values()) / 1e6,
        '/'.join(map(str, rate)), os.path.relpath(heralds, ROOT), len(named),
        sum(os.path.getsize(p) for p in herald_files.values()) / 1e6, os.path.relpath(sound, ROOT), len(effects),
        sum(os.path.getsize(sound_files[n]) for n in effects) / 1e6, os.path.relpath(wonders, ROOT), len(movies),
        sum(os.path.getsize(p) for p in wonder_files.values()) / 1e6, os.path.relpath(portraits, ROOT),
        len([n for n in portrait_files if n.endswith('.webp')]),
        sum(os.path.getsize(p) for p in portrait_files.values()) / 1e6), flush=True)


def digest_of():
    """the digest naming what this pin and this tool make."""
    h = hashlib.sha256(open(PIN, 'rb').read())
    code = open(os.path.abspath(__file__), 'rb').read()
    h.update(code[code.index(b'\nimport '):])        # (not the docstring)
    return h.hexdigest()[:16]


def ensure(args):
    """0.: the music, the heralds, the sound effects, the wonder movies and the portraits in their destinations, from what is there, the cache or made."""
    digest = digest_of()
    dests = {'music': args.music, 'heralds': args.heralds, 'sound': args.sound, 'wonders': args.wonders,
             'portraits': args.portraits}
    stamps = [os.path.join(d, '.built') for d in dests.values()]
    if not args.force and all(os.path.exists(s) and open(s).read().strip() == digest for s in stamps) and \
            os.path.exists(os.path.join(args.music, 'README.txt')) and \
            os.path.exists(os.path.join(args.heralds, 'heralds.txt')) and \
            os.path.exists(os.path.join(args.sound, 'SOURCES.md')) and \
            os.path.exists(os.path.join(args.wonders, 'LICENSE')) and \
            os.path.exists(os.path.join(args.portraits, 'README.md')):
        print('data/native/music, heralds, sound, wonders, portraits: OpenCiv2\'s own assets up to date (%s)' % digest,
              flush=True)
        return
    cached = os.path.join(cache_dir(), 'own-' + digest)
    if not args.force and os.path.exists(os.path.join(cached, 'music', 'README.txt')) and \
            os.path.exists(os.path.join(cached, 'heralds', 'heralds.txt')) and \
            os.path.exists(os.path.join(cached, 'sound', 'SOURCES.md')) and \
            os.path.exists(os.path.join(cached, 'wonders', 'LICENSE')) and \
            os.path.exists(os.path.join(cached, 'portraits', 'README.md')):
        for sub, dest in dests.items():
            clear(dest)
            for name in os.listdir(os.path.join(cached, sub)):
                shutil.copy(os.path.join(cached, sub, name), os.path.join(dest, name))
        print('data/native/music, heralds, sound, wonders, portraits: OpenCiv2\'s own assets copied from %s' % cached,
              flush=True)
    else:
        build(args.music, args.heralds, args.sound, args.wonders, args.portraits)
        tmp = cached + '.part'
        shutil.rmtree(tmp, ignore_errors=True)
        for sub, dest in dests.items():
            shutil.copytree(dest, os.path.join(tmp, sub), ignore=shutil.ignore_patterns('.built'))
        shutil.rmtree(cached, ignore_errors=True)
        os.replace(tmp, cached)
        for old in os.listdir(cache_dir()):          # (what other pins or tools made: not kept)
            if old.startswith('own-') and old != 'own-' + digest and old != 'own-assets':
                shutil.rmtree(os.path.join(cache_dir(), old), ignore_errors=True)
    for s in stamps:
        with open(s, 'w') as f:
            f.write(digest + '\n')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--force', action='store_true', help='make them again, whatever the destinations and the cache hold')
    ap.add_argument('--music', default=MUSIC, help='where to write the music (default data/native/music)')
    ap.add_argument('--heralds', default=HERALDS, help='where to write the heralds (default data/native/heralds)')
    ap.add_argument('--sound', default=SOUND, help='where to write the sound effects (default data/native/sound)')
    ap.add_argument('--wonders', default=WONDERS, help='where to write the wonder movies (default data/native/wonders)')
    ap.add_argument('--portraits', default=PORTRAITS,
                    help='where to write the historical leaders\' portraits (default data/native/portraits)')
    ensure(ap.parse_args())


if __name__ == '__main__':
    main()
