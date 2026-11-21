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
lang/, listed in lang/index.json (code, name), the font files of
characters beyond Windows-1252 that UTF-8 ones need (data/native/fonts,
gzipped, listed in data/fonts/index.json, their licence beside them; the
serifs also as they are, the page's fonts where the system has none for
Greek or Chinese) into data/fonts/, and
texts.js, the page's English (src/host/texts.h): docs/TRANSLATING.md.
OpenCiv2's English of the game's text files (data/native/text/*.TXT, used
where the game has none of its own) into data/text/, gzipped, listed in
data/text/index.json: the page fetches those the player's game lacks.
OpenCiv2's own sound effects (data/native/sound/*.ogg, CC0, played where
the game has none of its own: src/platform/sound.c) into data/sound/, as
they are, listed in data/sound/index.json: the page fetches those the
player's game lacks.

The HD graphics' art (data/native/hd, tools/build_hd_pack.py) and
OpenCiv2's own HD pictures (data/native/hd-extra) as one file, hd/art.bin,
the files one after another as hd/index.json lists them ([path, size]:
hd/..., hd-extra/...): the page fetches it when the game wants HD graphics
and puts the files where hd.c looks for them; the art's licence and
notices beside it.

With GAME=1 (make web GAME=1) the site carries the game too, for a
private site of people who have no copy of their own (docs/BROWSER.md:
it distributes the game's files): the installed game (paths.GAME, ~/Civ2)
as game/core.zip, but its music and the animated advisors (Music/,
KINGS/), which are game/files/<path> as they are (the music's .wav tracks
encoded to Ogg Vorbis, tools/wav2ogg.c), listed in game/index.json ({core:
{size, files}, files: [[path, size]]}); not the logs, nor the CD's videos
(Video/: the opening, the wonders', the High Council's, 285 MB; the game
then plays as from a folder without them: it starts at its main menu, a
wonder's movie is skipped, the High Council does not open).  The page
downloads and keeps all of it on a first visit, without asking, on one
progress bar, so that the game never waits for the site.  Without GAME=1,
build/web/game is removed.

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
import time

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
        'setup_worker.js', 'net.js', 'options.js', 'i18n.js']
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
        if cp in ('utf-8', 'utf8'):
            pages.add('utf-8')      # (UTF-8 text: data/native/fonts' files, text.c)
    with open(os.path.join(out, 'index.json'), 'w', encoding='utf-8') as f:
        json.dump(index, f, ensure_ascii=False)
    fonts = os.path.join(OUT, 'data', 'fonts')
    if os.path.isdir(fonts):
        shutil.rmtree(fonts)
    if not pages:
        return []
    os.makedirs(fonts)
    names = sorted(os.path.basename(f) for f in glob.glob(os.path.join(ROOT, 'data', 'native', 'fonts', '*.[ot]tf')))
    with open(os.path.join(fonts, 'index.json'), 'w') as f:
        json.dump(names, f)
    shutil.copy(os.path.join(ROOT, 'data', 'native', 'fonts', 'OFL.txt'), os.path.join(fonts, 'OFL.txt'))
    # (the serifs as they are, for the page's own texts too: index.html's
    # @font-face, fetched only where the system has no font for a character)
    for n in names:
        if 'Serif' in n:
            shutil.copy(os.path.join(ROOT, 'data', 'native', 'fonts', n), os.path.join(fonts, n))
    return ['fonts/' + n for n in names]


def write_texts():
    """data/text/<NAME>.TXT.gz and data/text/index.json (their names)."""
    src = os.path.join(ROOT, 'data', 'native', 'text')
    out = os.path.join(OUT, 'data', 'text')
    os.makedirs(out, exist_ok=True)
    names = sorted(os.path.basename(f) for f in glob.glob(os.path.join(src, '*.TXT')))
    for name in names:
        s, d = os.path.join(src, name), os.path.join(out, name + '.gz')
        if newer(s, d):
            with open(s, 'rb') as i, gzip.open(d, 'wb', 9) as o:
                shutil.copyfileobj(i, o)
    for f in os.listdir(out):
        if f != 'index.json' and f[:-3] not in names:
            os.remove(os.path.join(out, f))
    with open(os.path.join(out, 'index.json'), 'w') as f:
        json.dump(names, f)


def write_sounds():
    """data/sound/<name>.ogg and data/sound/index.json (their names)."""
    src = os.path.join(ROOT, 'data', 'native', 'sound')
    out = os.path.join(OUT, 'data', 'sound')
    os.makedirs(out, exist_ok=True)
    names = sorted(os.path.basename(f) for f in glob.glob(os.path.join(src, '*.ogg')))
    for name in names:
        s, d = os.path.join(src, name), os.path.join(out, name)
        if newer(s, d):
            shutil.copy(s, d)
    for f in os.listdir(out):
        if f != 'index.json' and f not in names:
            os.remove(os.path.join(out, f))
    with open(os.path.join(out, 'index.json'), 'w') as f:
        json.dump(names, f)


def write_art():
    """hd/art.bin and hd/index.json: the HD art (data/native/hd) and
    OpenCiv2's own pictures (data/native/hd-extra), the files one after
    another; the art's notices beside them."""
    out = os.path.join(OUT, 'hd')
    if os.path.isdir(out):
        shutil.rmtree(out)
    os.makedirs(out)
    native = os.path.join(ROOT, 'data', 'native')
    index = []
    with open(os.path.join(out, 'art.bin'), 'wb') as f:
        for d in ('hd', 'hd-extra'):
            for name in sorted(os.listdir(os.path.join(native, d))):
                if name == 'pictures.txt' or name.endswith(('.webp', '.png')):
                    data = open(os.path.join(native, d, name), 'rb').read()
                    f.write(data)
                    index.append([d + '/' + name, len(data)])
    with open(os.path.join(out, 'index.json'), 'w') as f:
        json.dump(index, f)
    for name in ('LICENSE', 'THIRD-PARTY.txt', 'ATLAS-PROVENANCE.md'):
        shutil.copy(os.path.join(native, 'hd', name), os.path.join(out, name))
    shutil.copy(os.path.join(native, 'hd-extra', 'README'), os.path.join(out, 'README-hd-extra'))


def run(cmd):
    print(' '.join(cmd), flush=True)
    subprocess.run(cmd, check=True)


def newer(src, dst):
    return not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src)


# Beside game/core.zip (game/files: the music encoded): downloaded with it
BESIDE_DIRS = ('music', 'kings')
# Not on the site at all: the CD's videos (285 MB, each read whole before its
# first frame); the game then plays as from a folder without the CD's Video
# (src/auto/game.cpp: intro_play_opening_video; no wonder movie,
# WonderMovieWindow::play; no High Council, CouncilWindow::init)
LEFT_OUT_DIRS = ('video',)


def wav2ogg():
    """build/wav2ogg (tools/wav2ogg.c, the host's cc): raw PCM to Ogg Vorbis."""
    out = os.path.join(ROOT, 'build', 'wav2ogg')
    src = os.path.join(ROOT, 'tools', 'wav2ogg.c')
    if not newer(src, out):
        return out
    if not os.path.exists(os.path.join(paths.VORBIS, 'lib', 'vorbisenc.c')):
        run(['sh', os.path.join(ROOT, 'tools', 'fetch-toolchain.sh')])
    sources = [src] + [os.path.join(paths.OGG, 'src', f + '.c') for f in ('bitwise', 'framing')] + \
        [os.path.join(paths.VORBIS, 'lib', f + '.c') for f in
         ('analysis', 'bitrate', 'block', 'codebook', 'envelope', 'floor0', 'floor1', 'info', 'lookup', 'lpc', 'lsp',
          'mapping0', 'mdct', 'psy', 'registry', 'res0', 'sharedbook', 'smallft', 'synthesis', 'vorbisenc', 'window')]
    # (libogg's config_types.h: setup_web.c's build has it in its include folder)
    run([os.environ.get('CC', 'cc'), '-O2', '-w', '-I' + os.path.join(paths.OGG, 'include'),
         '-I' + os.path.join(paths.VORBIS, 'include'), '-I' + os.path.join(paths.VORBIS, 'lib'), '-o', out] +
        sources + ['-lm'])
    return out


def write_game():
    """game/: the installed game (GAME=1): core.zip, files/, index.json."""
    import wave
    import zipfile
    game, out = paths.GAME, os.path.join(OUT, 'game')
    if not os.path.exists(os.path.join(game, 'Civ2Art.dll')):
        sys.exit('GAME=1: no installed game in %s (CIV2_GAME)' % game)
    os.makedirs(out, exist_ok=True)
    core, beside = [], []
    for d, dirs, files in os.walk(game):
        dirs.sort()
        for f in sorted(files):
            path = os.path.join(d, f)
            rel = os.path.relpath(path, game).replace(os.sep, '/')
            if f.lower().endswith('.log') or rel.split('/')[0].lower() in LEFT_OUT_DIRS:
                continue
            (beside if rel.split('/')[0].lower() in BESIDE_DIRS else core).append((rel, path))
    zpath = os.path.join(out, 'core.zip')
    # (what core.zip was made of, outside the site: made again when that changes)
    stamp, spath = json.dumps([[r, os.path.getsize(p), os.path.getmtime(p)] for r, p in core]), \
        os.path.join(ROOT, 'build', 'web-game.stamp')
    if not os.path.exists(zpath) or not os.path.exists(spath) or open(spath).read() != stamp:
        print('packing %s into %s' % (game, os.path.relpath(zpath, ROOT)), flush=True)
        # (a game with none of its own set-up, as the site gives it: the
        # animated heralds of KINGS/ on, their warning already shown --
        # CIV2.DAT's game options, offset 14, OPT_ANIMATED_HERALDS)
        ini = not any(r.lower() == 'civ.ini' for r, _ in core)
        with zipfile.ZipFile(zpath + '.part', 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
            for rel, path in core:
                data = open(path, 'rb').read()
                if ini and rel.lower() == 'civ2.dat' and len(data) >= 18:
                    opts = int.from_bytes(data[14:18], 'little') | 0x200000
                    data = data[:14] + opts.to_bytes(4, 'little') + data[18:]
                z.writestr(zipfile.ZipInfo(rel, time.localtime(os.path.getmtime(path))[:6]), data,
                           zipfile.ZIP_DEFLATED, 9)
            if ini:
                z.writestr('CIV.INI', '[Civilization Gold]\r\nHerald Warning Shown=1\r\n')
        os.replace(zpath + '.part', zpath)
        with open(spath, 'w') as f:
            f.write(stamp)
    files, listed = os.path.join(out, 'files'), []
    have = {r.lower() for r, _ in beside}
    for rel, path in beside:
        name = rel
        if rel.lower().startswith('music/') and rel.lower().endswith('.wav'):
            name = rel[:-4] + '.ogg'
            if name.lower() in have:
                continue        # (the game plays the .ogg: TrackNN.ogg, else .wav)
            dst = os.path.join(files, name)
            if newer(path, dst):
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                print('encoding %s' % rel, flush=True)
                with wave.open(path) as w:
                    if (w.getnchannels(), w.getsampwidth(), w.getframerate()) != (2, 2, 44100):
                        sys.exit('%s: not 16-bit stereo 44.1 kHz' % path)
                    with open(dst + '.part', 'wb') as o:
                        p = subprocess.Popen([wav2ogg(), rel[-6:-4] if rel[-6:-4].isdigit() else '1'],
                                             stdin=subprocess.PIPE, stdout=o)
                        while True:
                            b = w.readframes(1 << 16)
                            if not b:
                                break
                            p.stdin.write(b)
                        p.stdin.close()
                        if p.wait():
                            sys.exit('wav2ogg failed on ' + path)
                os.replace(dst + '.part', dst)
        else:
            dst = os.path.join(files, name)
            if newer(path, dst):
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                shutil.copy2(path, dst)
        listed.append([name, os.path.getsize(dst)])
    keep = {os.path.join(files, n) for n, _ in listed}
    for d, _, fs in os.walk(files, topdown=False):
        for f in fs:
            if os.path.join(d, f) not in keep:
                os.remove(os.path.join(d, f))
        if d != files and not os.listdir(d):
            os.rmdir(d)
    with open(os.path.join(out, 'index.json'), 'w') as f:
        json.dump({'core': {'size': os.path.getsize(zpath), 'files': len(core)}, 'files': listed}, f)
    return os.path.getsize(zpath), sum(n for _, n in listed)


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
    for f in ('hd-pack.json', 'choices.json', 'hdpack.js'):
        if os.path.exists(os.path.join(OUT, f)):
            os.remove(os.path.join(OUT, f))     # (an older build's)
    write_art()
    if os.environ.get('GAME', '') not in ('', '0'):
        core, beside = write_game()
        print('the game on the site (GAME=1: for private use only): game/core.zip %.0f MB, game/files %.0f MB' %
              (core / 1e6, beside / 1e6), flush=True)
    elif os.path.isdir(os.path.join(OUT, 'game')):
        shutil.rmtree(os.path.join(OUT, 'game'))
    write_texts()
    write_sounds()
    for f in glob.glob(os.path.join(OUT, 'data', 'fonts_*.bin.gz')):
        if os.path.basename(f)[:-3] not in FONTS:
            os.remove(f)        # (the code pages' glyphs of before: text.c makes them)
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
