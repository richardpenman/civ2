#!/usr/bin/env python3
"""OpenCiv2's own assets: make data/native/music (docs/NO-INSTALL.md, Music:
the tracks the game plays where its folder lacks Civilization II's CD
music, Music\\TrackNN.ogg or .wav: src/platform/sound.c) and
data/native/heralds (docs/HD-GRAPHICS.md, Your own heralds: the diplomacy
room's animated heralds, src/platform/hd.c: hd_herald_find) from OpenCiv2's
own repository openciv2/openciv2-assets (MIT, as OpenCiv2; not Peter
Johnson's PeterJohnson/openciv2-assets, the HD art's: tools/build_hd_pack.py)
at the commit pinned in data/own-assets.json (repo, sha, and each file's
path, SHA-256 and size).  They are not part of this repository
(.gitignore): make own-assets runs this, and the builds that pack or run
with them (make, make port, make native, make windows, make web; the
release's jobs) run it first.

    python3 tools/build_own_assets.py [--force] [--local] [--music DIR] [--heralds DIR]

0. Nothing is done when both destinations already hold what this pin and
   this tool make (their .built: a digest of data/own-assets.json and this
   file but its docstring); else what was made earlier from them is copied
   from the cache (own-DIGEST: kept there after each make below), without
   the network; else it is made (1 to 3) and kept in the cache.  --force
   makes it again whatever is there.
1. Each file the pin lists is fetched at the pinned commit from
   raw.githubusercontent.com into the cache (${XDG_CACHE_HOME:-~/.cache}/
   openciv2/own-assets/SHA256, %LOCALAPPDATA%\\openciv2 on Windows;
   CIV2_DEPENDENCY_CACHE overrides) and checked against the SHA-256 and size
   pinned for it: a file that is not is not used; one already in the cache
   is not fetched again.  Only the files listed: the music's tracks 02 to 12
   (the CD's audio tracks 2 to 12; not 01), the heralds' heralds.txt and
   the WebPs it names (not the clips they were made from), the LICENSE.
2. The music is Ogg Opus (48 kHz stereo), which stb_vorbis, the game's
   decoder, cannot read: each track is decoded (opusdec, 32-bit float, so
   nothing is rounded between the two) and encoded again as Ogg Vorbis
   (oggenc, libvorbis's VBR at quality QUALITY, at the tracks' own 48 kHz:
   stb_vorbis and the hosts' audio take any rate, so nothing is resampled;
   the stream's serial number the track's, so the same input gives the same
   file), in a throwaway container of the pinned Debian image (IMAGE) with
   its packages pinned (PACKAGES).  Quality 5 (about 140 kbit/s on these
   tracks, 26.4 MB for the eleven, 25 minutes) rather than the 4 the set-up
   encodes the CD's music with (src/host/setup_disc.c: MUSIC_QUALITY 0.4,
   about 113 kbit/s here, 21.5 MB): the source is itself lossy (Opus at 256
   kbit/s), and a second lossy encoding is kept a step above the first's
   usual quality, for 5 MB more.  The heralds are copied as they are.
3. data/native/music (trackNN.ogg, README.txt, LICENSE) and
   data/native/heralds (heralds.txt, its WebPs, LICENSE) are written again.

Without Docker running (or with --local) this machine's opusdec and oggenc
(opus-tools, vorbis-tools), else its ffmpeg (with libvorbis), encode the
music: the same music, perhaps not the same bytes (another libvorbis), so
not kept in the cache.
"""
import argparse, hashlib, json, os, shutil, subprocess, sys, tempfile, urllib.parse, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = os.path.join(ROOT, 'data', 'own-assets.json')
MUSIC = os.path.join(ROOT, 'data', 'native', 'music')
HERALDS = os.path.join(ROOT, 'data', 'native', 'heralds')

QUALITY = 5                 # libvorbis VBR quality (-1 to 10)
# the encoder, pinned: Debian 13's image (as tools/build_hd_pack.py's) and its packages
IMAGE = 'debian:trixie@sha256:9c6e381421d0f48d1b8133e573f33c185ee16cbf505eda9c4a8a4791b25ea8a6'
PACKAGES = ('opus-tools=0.2-1+b3', 'vorbis-tools=1.4.3-1', 'libvorbisenc2=1.3.7-3', 'libvorbis0a=1.3.7-3',
            'libopus0=1.5.2-2')
SKIP = 'or build without them: make OWN_ASSETS='

README = """OpenCiv2's own music (docs/NO-INSTALL.md, Music)

The game plays trackNN.ogg as the CD's audio track NN where its folder has
no Music\\TrackNN.ogg or Music\\TrackNN.wav (src/platform/sound.c).

From OpenCiv2's own repository openciv2/openciv2-assets
({repo}) at commit {sha}:
{tracks}
made Ogg Vorbis (quality {quality}) by tools/build_own_assets.py.

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
    for group in ('music', 'heralds', 'notices'):
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


def docker_ready():
    """Docker installed and its daemon answering (Docker Desktop may be installed but not started)."""
    if not shutil.which('docker'):
        return False
    try:
        return subprocess.run(['docker', 'info'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                              timeout=20).returncode == 0
    except (OSError, subprocess.TimeoutExpired):
        return False


def local_encoder():
    """Without Docker: 'opus-tools' (opusdec and oggenc) or 'ffmpeg'; else stops, saying what to install."""
    if shutil.which('opusdec') and shutil.which('oggenc'):
        return 'opus-tools'
    if shutil.which('ffmpeg'):
        return 'ffmpeg'
    sys.exit('build_own_assets: without Docker the music needs opusdec and oggenc (macOS: brew install opus-tools '
             'vorbis-tools; Debian/Ubuntu: apt install opus-tools vorbis-tools) or ffmpeg with libvorbis (macOS: '
             'brew install ffmpeg; Debian/Ubuntu: apt install ffmpeg). Or start Docker (Docker Desktop on macOS) and '
             'run make again, ' + SKIP)


def encode(work, tracks, local):
    """2.: work/in/NN.ogg (Opus) -> work/out/trackNN.ogg (Vorbis)."""
    titles = {n: os.path.splitext(t['path'].split('/')[-1])[0].split(' - ', 1)[-1] for n, t in tracks.items()}
    with open(os.path.join(work, 'jobs.txt'), 'w', newline='\n') as f:
        for n in sorted(tracks):
            f.write('%s %s\n' % (n, titles[n]))
    script = ('cd "$1" && mkdir -p out && while read n title; do '
              'opusdec --quiet --float "in/$n.ogg" "tmp.wav" && '
              'oggenc -Q -q %d -s $((1$n - 100)) -N $((1$n - 100)) -t "$title" -o "out/track$n.ogg" tmp.wav '
              '|| exit 1; rm -f tmp.wav; done < jobs.txt' % QUALITY)
    if local == 'ffmpeg':
        os.makedirs(os.path.join(work, 'out'), exist_ok=True)
        for n in sorted(tracks):
            subprocess.run(['ffmpeg', '-nostdin', '-loglevel', 'error', '-y', '-i', os.path.join(work, 'in', n + '.ogg'),
                            '-map', '0:a', '-c:a', 'libvorbis', '-q:a', str(QUALITY), '-map_metadata', '-1',
                            '-metadata', 'title=' + titles[n], '-metadata', 'tracknumber=%d' % int(n),
                            os.path.join(work, 'out', 'track%s.ogg' % n)], check=True)
    elif local:
        subprocess.run(['sh', '-c', script, 'sh', work], check=True)
    else:
        subprocess.run(['docker', 'run', '--rm', '-v', work + ':/w', '-e', 'HOST_IDS=%d:%d' % (os.getuid(), os.getgid()),
                        IMAGE, 'sh', '-c',
                        'apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq '
                        '--no-install-recommends %s >/dev/null && sh -c \'%s\' sh /w; r=$?; chown -R "$HOST_IDS" /w; '
                        'exit $r' % (' '.join(PACKAGES), script)], check=True)


def clear(dest):
    os.makedirs(dest, exist_ok=True)
    for name in os.listdir(dest):
        if os.path.isfile(os.path.join(dest, name)):
            os.remove(os.path.join(dest, name))


def build(music, heralds, local):
    pin = load_pin()
    sources = {n: fetch(pin, t) for n, t in sorted(pin['music'].items())}
    herald_files = {name: fetch(pin, f) for name, f in pin['heralds'].items()}
    notices = {name: fetch(pin, f) for name, f in pin['notices'].items()}
    # (every WebP heralds.txt names pinned too)
    named = [l.split()[-1] for l in open(herald_files['heralds.txt'], encoding='utf-8')
             if l.strip() and not l.startswith('#')]
    if set(named) - set(herald_files):
        sys.exit('build_own_assets: heralds.txt names %s, not pinned in data/own-assets.json'
                 % ', '.join(sorted(set(named) - set(herald_files))))
    os.makedirs(os.path.join(ROOT, 'build'), exist_ok=True)
    work = tempfile.mkdtemp(prefix='own-assets-', dir=os.path.join(ROOT, 'build'))
    try:
        os.makedirs(os.path.join(work, 'in'))
        for n, path in sources.items():
            shutil.copy(path, os.path.join(work, 'in', n + '.ogg'))
        how = {'opus-tools': 'this machine\'s opusdec and oggenc', 'ffmpeg': 'this machine\'s ffmpeg'}.get(
            local, '%s in %s' % (', '.join(PACKAGES[:2]), IMAGE.split('@')[0]))
        print('encoding %d tracks as Ogg Vorbis, quality %d (%s)' % (len(sources), QUALITY, how), flush=True)
        encode(work, pin['music'], local)
        made = {}
        for n in sorted(sources):
            out = os.path.join(work, 'out', 'track%s.ogg' % n)
            info = vorbis_info(out) if os.path.exists(out) else None
            if not info or info[0] != 2:
                sys.exit('build_own_assets: track %s was not made Ogg Vorbis, stereo' % n)
            made[n] = (out, info)
        clear(music)
        total = 0
        for n, (out, info) in made.items():
            shutil.copy(out, os.path.join(music, 'track%s.ogg' % n))
            total += os.path.getsize(out)
        with open(os.path.join(music, 'README.txt'), 'w', newline='\n') as f:
            f.write(README.format(repo=pin['repo'], sha=pin['sha'], quality=QUALITY, tracks=''.join(
                '  track%s.ogg  %s\n' % (n, t['path']) for n, t in sorted(pin['music'].items()))))
        clear(heralds)
        for name, path in herald_files.items():
            shutil.copy(path, os.path.join(heralds, name))
        for dest in (music, heralds):
            for name, path in notices.items():
                shutil.copy(path, os.path.join(dest, name))
        rate = sorted({info[1] for _, info in made.values()})
        print('wrote %s: %d tracks, %.1f MB (Ogg Vorbis, stereo, %s Hz); %s: %d heralds, %.1f MB' % (
            os.path.relpath(music, ROOT), len(made), total / 1e6, '/'.join(map(str, rate)),
            os.path.relpath(heralds, ROOT), len(named),
            sum(os.path.getsize(p) for p in herald_files.values()) / 1e6), flush=True)
    finally:
        shutil.rmtree(work, ignore_errors=True)


def digest_of():
    """the digest naming what this pin and this tool make."""
    h = hashlib.sha256(open(PIN, 'rb').read())
    code = open(os.path.abspath(__file__), 'rb').read()
    h.update(code[code.index(b'\nimport '):])        # (not the docstring)
    return h.hexdigest()[:16]


def ensure(args):
    """0.: the music and the heralds in their destinations, from what is there, the cache or made."""
    digest = digest_of()
    dests = {'music': args.music, 'heralds': args.heralds}
    stamps = [os.path.join(d, '.built') for d in dests.values()]
    if not args.force and all(os.path.exists(s) and open(s).read().strip() == digest for s in stamps) and \
            os.path.exists(os.path.join(args.music, 'README.txt')) and \
            os.path.exists(os.path.join(args.heralds, 'heralds.txt')):
        print('data/native/music, heralds: OpenCiv2\'s own assets up to date (%s)' % digest, flush=True)
        return
    cached = os.path.join(cache_dir(), 'own-' + digest)
    if not args.force and os.path.exists(os.path.join(cached, 'music', 'README.txt')) and \
            os.path.exists(os.path.join(cached, 'heralds', 'heralds.txt')):
        for sub, dest in dests.items():
            clear(dest)
            for name in os.listdir(os.path.join(cached, sub)):
                shutil.copy(os.path.join(cached, sub, name), os.path.join(dest, name))
        print('data/native/music, heralds: OpenCiv2\'s own assets copied from %s' % cached, flush=True)
    else:
        local = None
        if args.local or not docker_ready():
            print('build_own_assets: no Docker running (or --local): this machine\'s tools make the music', flush=True)
            local = local_encoder()
        build(args.music, args.heralds, local)
        if not local:
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
    ap.add_argument('--local', action='store_true', help='encode with this machine\'s opusdec and oggenc (or ffmpeg), not in Docker')
    ap.add_argument('--music', default=MUSIC, help='where to write the music (default data/native/music)')
    ap.add_argument('--heralds', default=HERALDS, help='where to write the heralds (default data/native/heralds)')
    ensure(ap.parse_args())


if __name__ == '__main__':
    main()
