#!/usr/bin/env python3
"""HD graphics (docs/HD-GRAPHICS.md): download Peter Johnson's runtime art
pack (openciv2-assets, MIT) at the release pinned in data/hd-pack.json, check
it, and install the game's part of it in data/native/hd (or --dest DIR):
atlas-hd.pack, its LICENSE, THIRD-PARTY.txt and ATLAS-PROVENANCE.md.

The download is kept in the cache (${XDG_CACHE_HOME:-~/.cache}/openciv2, or
%LOCALAPPDATA%\\openciv2 on Windows; CIV2_DEPENDENCY_CACHE overrides), named
by its SHA-256, so it is fetched once.  The archive must have the pinned
SHA-256 and each file the SHA-256 its package.json lists; nothing is
installed otherwise.  Standard library only.

    python3 tools/fetch_hd_pack.py [--dest DIR] [--zip FILE]
"""
import argparse, hashlib, json, os, shutil, sys, tempfile, urllib.request, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = os.path.join(ROOT, 'data', 'hd-pack.json')
# archive member -> installed name
FILES = {'data/atlas-hd.pack': 'atlas-hd.pack', 'LICENSE': 'LICENSE', 'THIRD-PARTY.txt': 'THIRD-PARTY.txt',
         'data/ATLAS-PROVENANCE.md': 'ATLAS-PROVENANCE.md'}


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
    print('downloading %s' % url)
    tmp = path + '.part'
    with urllib.request.urlopen(url) as r, open(tmp, 'wb') as f:
        total = int(r.headers.get('Content-Length') or 0)
        done = 0
        while True:
            block = r.read(1 << 20)
            if not block:
                break
            f.write(block)
            done += len(block)
            if total and sys.stdout.isatty():
                print('\r%d / %d MB' % (done >> 20, total >> 20), end='', flush=True)
    if sys.stdout.isatty():
        print()
    got = sha256_file(tmp)
    if got != sha:
        os.remove(tmp)
        sys.exit('fetch_hd_pack: %s has SHA-256 %s, not the pinned %s' % (url, got, sha))
    os.replace(tmp, path)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--dest', default=os.path.join(ROOT, 'data', 'native', 'hd'))
    ap.add_argument('--zip', help='an archive already downloaded (still checked against the pin)')
    args = ap.parse_args()
    pin = json.load(open(PIN))
    archive = args.zip or os.path.join(cache_dir(), pin['sha256'] + '.zip')
    if args.zip:
        if sha256_file(args.zip) != pin['sha256']:
            sys.exit('fetch_hd_pack: %s is not the pinned archive (%s)' % (args.zip, pin['tag']))
    else:
        download(pin['url'], pin['sha256'], archive)
    stamp = os.path.join(args.dest, 'installed.json')
    if os.path.exists(stamp) and json.load(open(stamp)).get('sha256') == pin['sha256'] and \
            all(os.path.exists(os.path.join(args.dest, n)) for n in FILES.values()):
        print('%s: the pinned pack %s is installed' % (args.dest, pin['tag']))
        return
    with zipfile.ZipFile(archive) as z:
        manifest = json.loads(z.read('package.json'))
        if manifest.get('kind') != 'openciv2-assets' or manifest.get('schema') != 1:
            sys.exit('fetch_hd_pack: not an openciv2-assets runtime pack (schema 1)')
        os.makedirs(args.dest, exist_ok=True)
        staging = tempfile.mkdtemp(dir=args.dest)
        try:
            for member, name in FILES.items():
                want = manifest['files'][member]
                out = os.path.join(staging, name)
                h = hashlib.sha256()
                with z.open(member) as src, open(out, 'wb') as dst:
                    for block in iter(lambda: src.read(1 << 20), b''):
                        h.update(block)
                        dst.write(block)
                if h.hexdigest() != want['sha256'] or os.path.getsize(out) != want['size']:
                    sys.exit('fetch_hd_pack: %s does not match its package.json' % member)
            for name in FILES.values():
                os.replace(os.path.join(staging, name), os.path.join(args.dest, name))
        finally:
            shutil.rmtree(staging, ignore_errors=True)
    json.dump({'tag': pin['tag'], 'sha256': pin['sha256'], 'version': manifest.get('version')},
              open(stamp, 'w'), indent=2)
    print('%s: installed the pack %s (%s)' % (args.dest, pin['tag'], ', '.join(FILES.values())))


if __name__ == '__main__':
    main()
