#!/usr/bin/env python3
"""The browser version (make web): build/web, a static site.

The portable build's module (build/wasm/civ2.wasm, tools/build_port.py
module) passed through Binaryen's wasm-opt:
  --fpcast-emu   calls through function pointers of another type than their
                 target go ahead (the decompiled code makes them, as the
                 original freely did on x86; wasm traps on them, and the
                 native port patches wasm2c's check out instead);
  --asyncify     the module's stack can be unwound and wound back where it
                 calls the imports that wait (src/web/host.js), so that the
                 game's blocking loop runs on the page's event loop;
  -O2            and the debugging information left out.
Then the page (src/web) and the program's fonts (data/native, gzipped:
the page inflates them).  And setup.wasm, the set-up from the game's CD or
a disc image (src/web/setup/setup_web.c with src/host/setup.c,
setup_disc.c, unshield, zlib, libogg and libvorbis, as the native build has
them: wasi-sdk's clang; the page runs it in a Web Worker).

The translations (data/native/lang/*.txt but the template en.txt) into
lang/, listed in lang/index.json (code, name), the glyphs of their code
pages (data/native/fonts_<code page>*.bin, gzipped) into data/, and
texts.js, the page's English (src/host/texts.h): docs/TRANSLATING.md.

The HD graphics' pin (data/hd-pack.json) is copied as hd-pack.json; with
HD=1 in the environment (make web HD=1) the installed art
(data/native/hd/atlas-hd.pack, make hd) is copied to hd/atlas-hd.pack too,
with hd/pack.json saying so: the game's OpenCiv2 Options offer HD graphics
only where the site has the art (otherwise build/web/hd is removed).

Usage: build_web.py [--no-module]   (--no-module: build/wasm/civ2.wasm as it is)
Binaryen comes from tools/fetch-toolchain.sh web (or $BINARYEN: its folder).
"""
import glob
import gzip
import json
import os
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

ROOT = paths.ROOT
OUT = os.path.join(ROOT, 'build', 'web')
SRC = os.path.join(ROOT, 'src', 'web')
BINARYEN = os.environ.get('BINARYEN') or os.path.join(ROOT, 'tools', 'toolchain', 'binaryen')
# The imports that may wait (src/web/host.js: blocking)
ASYNC_IMPORTS = ['host.host_wait_event', 'host.host_poll_event', 'host.host_delay_ms', 'host.host_message_box',
                 'host.host_restart']
PAGE = ['index.html', 'main.js', 'host.js', 'wasi.js', 'store.js', 'zip.js', 'saves.js', 'setup_run.js',
        'setup_worker.js', 'net.js', 'hdpack.js', 'options.js', 'i18n.js']
FONTS = ['fonts_win.bin', 'fonts_win_aa.bin']
LANG = os.path.join(ROOT, 'data', 'native', 'lang')


def english_texts():
    """OpenCiv2's own texts' English (src/host/texts.h): {key: text}."""
    import re
    text = open(os.path.join(ROOT, 'src', 'host', 'texts.h'), encoding='ascii').read()
    out = {}
    for key, value in re.findall(r'^T\("([^"]+)", ((?:"(?:\\.|[^"\\])*"\s*)+)\)', text, re.M):
        parts = re.findall(r'"((?:\\.|[^"\\])*)"', value)
        out[key] = ''.join(parts).encode('ascii').decode('unicode_escape')
    return out


def lang_file(path):
    """A translation file: (its code page, {key: text}), as src/web/i18n.js reads it."""
    data = open(path, 'rb').read()
    cp = 'windows-1252'
    lines = data.decode('latin-1').splitlines()
    for i, line in enumerate(lines):
        if line == '@@codepage' and i + 1 < len(lines):
            cp = lines[i + 1].strip().lower()
    texts, key, body = {}, None, []
    for line in data.decode(cp).splitlines():
        if line.startswith('@@'):
            if key is not None:
                texts[key] = '\n'.join(body)
            key, body = line[2:], []
        elif key is not None:
            body.append(line[1:] if line.startswith('\\') else line)
    if key is not None:
        texts[key] = '\n'.join(body)
    return cp, texts


def write_languages():
    """texts.js (the page's English), lang/ (the translations, lang/index.json
    listing them: code, name) and the glyphs of their code pages."""
    with open(os.path.join(OUT, 'texts.js'), 'w', encoding='utf-8') as f:
        f.write('// OpenCiv2\'s own texts, English (src/host/texts.h; tools/build_web.py)\n')
        f.write('export const EN = %s;\n' % json.dumps(english_texts(), indent=0, ensure_ascii=False))
    out = os.path.join(OUT, 'lang')
    if os.path.isdir(out):
        shutil.rmtree(out)
    os.makedirs(out)
    index, pages = [], set()
    for path in sorted(glob.glob(os.path.join(LANG, '*.txt'))):
        code = os.path.basename(path)[:-4]
        if code == 'en':
            continue
        cp, texts = lang_file(path)
        shutil.copy(path, os.path.join(out, code + '.txt'))
        index.append({'code': code, 'name': texts.get('openciv2.language.name', code)})
        if cp not in ('windows-1252', 'cp1252'):
            pages.add(cp)
    with open(os.path.join(out, 'index.json'), 'w', encoding='utf-8') as f:
        json.dump(index, f, ensure_ascii=False)
    return ['fonts_%s.bin' % cp for cp in sorted(pages)] + ['fonts_%s_aa.bin' % cp for cp in sorted(pages)]


def run(cmd):
    print(' '.join(cmd), flush=True)
    subprocess.run(cmd, check=True)


def newer(src, dst):
    return not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src)


def build_setup():
    """build/web/setup.wasm: the set-up (its sources as tools/build_port.py's exe step has them)."""
    if not all(os.path.exists(f) for f in (os.path.join(paths.UNSHIELD, 'lib', 'libunshield.c'),
                                            os.path.join(paths.ZLIB, 'inflate.c'),
                                            os.path.join(paths.VORBIS, 'lib', 'vorbisenc.c'))) or \
            not os.path.isdir(paths.WASI_SDK):
        run(['sh', os.path.join(ROOT, 'tools', 'fetch-toolchain.sh')])
    host = os.path.join(ROOT, 'src', 'host')
    sources = [os.path.join(SRC, 'setup', 'setup_web.c'), os.path.join(host, 'setup.c'),
               os.path.join(host, 'setup_disc.c'), os.path.join(host, 'host_lang.c')] + \
        sorted(glob.glob(os.path.join(paths.UNSHIELD, 'lib', '*.c'))) + \
        [os.path.join(paths.UNSHIELD, 'lib', 'md5', 'md5c.c')] + \
        [os.path.join(paths.ZLIB, f + '.c') for f in ('adler32', 'crc32', 'inffast', 'inflate', 'inftrees',
                                                     'zutil')] + \
        [os.path.join(paths.OGG, 'src', f + '.c') for f in ('bitwise', 'framing')] + \
        [os.path.join(paths.VORBIS, 'lib', f + '.c') for f in
         ('analysis', 'bitrate', 'block', 'codebook', 'envelope', 'floor0', 'floor1', 'info', 'lookup', 'lpc', 'lsp',
          'mapping0', 'mdct', 'psy', 'registry', 'res0', 'sharedbook', 'smallft', 'synthesis', 'vorbisenc', 'window')]
    # (an 8 MB stack, below the data so that running out of it traps: the
    # set-up's paths are 4 kB arrays on the stack)
    out = os.path.join(OUT, 'setup.wasm')
    deps = sources + glob.glob(os.path.join(host, 'setup*.h')) + [__file__]
    if os.path.exists(out) and all(not newer(d, out) for d in deps):
        return
    run([os.path.join(paths.WASI_SDK, 'bin', 'clang'), '--target=wasm32-wasip1', '-O2', '-w', '-std=gnu17',
         '-Wl,--strip-all', '-Wl,-z,stack-size=8388608', '-Wl,--stack-first', '-I' + host, '-I' + paths.UNSHIELD,
         '-I' + os.path.join(paths.UNSHIELD, 'lib'), '-I' + paths.ZLIB, '-I' + os.path.join(paths.OGG, 'include'), '-I' + os.path.join(paths.VORBIS, 'include'),
         '-I' + os.path.join(paths.VORBIS, 'lib'), '-o', out] + sources)


def main():
    if '--no-module' not in sys.argv:
        run([sys.executable, os.path.join(ROOT, 'tools', 'build_port.py'), 'module'])
    wasm_opt = os.path.join(BINARYEN, 'bin', 'wasm-opt')
    if not os.path.exists(wasm_opt) and not os.path.exists(wasm_opt + '.exe'):
        run(['sh', os.path.join(ROOT, 'tools', 'fetch-toolchain.sh'), 'web'])
    os.makedirs(os.path.join(OUT, 'data'), exist_ok=True)
    src = os.path.join(ROOT, 'build', 'wasm', 'civ2.wasm')
    out = os.path.join(OUT, 'civ2.wasm')
    if newer(src, out) or newer(__file__, out):
        run([wasm_opt, src, '--strip-debug', '--strip-producers', '--fpcast-emu', '--asyncify',
             '--pass-arg=asyncify-imports@' + ','.join(ASYNC_IMPORTS), '-O2', '-o', out])
    build_setup()
    for f in PAGE:
        shutil.copy(os.path.join(SRC, f), os.path.join(OUT, f))
    if os.path.exists(os.path.join(OUT, 'choices.json')):
        os.remove(os.path.join(OUT, 'choices.json'))     # (an older build's)
    shutil.copy(os.path.join(ROOT, 'data', 'hd-pack.json'), os.path.join(OUT, 'hd-pack.json'))
    hd = os.path.join(OUT, 'hd')
    if os.environ.get('HD', '') not in ('', '0'):
        pack = os.path.join(ROOT, 'data', 'native', 'hd', 'atlas-hd.pack')
        if not os.path.exists(pack):
            run([sys.executable, os.path.join(ROOT, 'tools', 'fetch_hd_pack.py')])
        os.makedirs(hd, exist_ok=True)
        if newer(pack, os.path.join(hd, 'atlas-hd.pack')):
            shutil.copy(pack, os.path.join(hd, 'atlas-hd.pack'))
        # (what tells the page the site has the art: without it, no HD graphics there)
        with open(os.path.join(hd, 'pack.json'), 'w') as f:
            json.dump({'size': os.path.getsize(pack)}, f)
        for f in ('LICENSE', 'THIRD-PARTY.txt', 'ATLAS-PROVENANCE.md'):
            if os.path.exists(os.path.join(ROOT, 'data', 'native', 'hd', f)):
                shutil.copy(os.path.join(ROOT, 'data', 'native', 'hd', f), os.path.join(hd, f))
    elif os.path.isdir(hd):
        shutil.rmtree(hd)
    for f in FONTS + write_languages():
        s, d = os.path.join(ROOT, 'data', 'native', f), os.path.join(OUT, 'data', f + '.gz')
        if newer(s, d):
            print('compressing %s' % os.path.relpath(s, ROOT), flush=True)
            with open(s, 'rb') as i, gzip.open(d, 'wb', 9) as o:
                shutil.copyfileobj(i, o)
    print('built %s (civ2.wasm %.1f MB, setup.wasm %.1f MB): serve it, e.g. python3 -m http.server -d build/web' %
          (os.path.relpath(OUT, ROOT), os.path.getsize(out) / 1e6, os.path.getsize(os.path.join(OUT, 'setup.wasm')) / 1e6))


if __name__ == '__main__':
    main()
