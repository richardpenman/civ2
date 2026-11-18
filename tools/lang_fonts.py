#!/usr/bin/env python3
"""The font files UTF-8 translations (Chinese, Greek ...) are drawn with
beyond Windows-1252's glyphs, in data/native/fonts (src/platform/text.c
draws them at run time with stb_truetype; docs/TRANSLATING.md): a serif and
a sans per family of scripts,

- Greek and Cyrillic: NotoSerif-Regular.ttf and NotoSans-Regular.ttf (Noto
  Serif and Noto Sans 2.015, unhinted);
- the others (Chinese): NotoSerifTC-Regular.otf and NotoSansTC-Regular.otf
  (Noto Serif TC 2.003, Noto Sans TC 2.004).

Scans data/native/lang/*.txt for the translations whose @@codepage is
utf-8, takes every character they use that Windows-1252's glyphs
(fonts_win*.bin) and text.c's composed letters (marks[]: ĉ, ŭ ...) have
not (plus common full-width punctuation for Chinese), each in its family,
fetches the fonts (SIL Open Font License 1.1, no Reserved Font Name but
Source Han's 'Source': pinned releases, checked by SHA-256, kept in
run/lang-fonts) and fontTools (pinned, checked, run from its wheel), and
writes the fonts with only those characters (no hinting, no layout tables:
text.c needs outlines and advances).  A family no translation needs has
no files.  Run it after adding characters to a translation (in Docker or
not; it needs no installed package):

    tools/dk python3 tools/lang_fonts.py    (--check: only say whether the
                                             fonts have every character;
                                             make test)
"""
import glob
import hashlib
import os
import re
import struct
import sys
import urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LANG = os.path.join(ROOT, 'data', 'native', 'lang')
OUT = os.path.join(ROOT, 'data', 'native', 'fonts')
WORK = os.path.join(ROOT, 'run', 'lang-fonts')

NOTO = 'https://github.com/notofonts/notofonts.github.io/raw/28b15b4b43b7bed62b5cf6e6b0b5ff5846270535/fonts/'
FONTS = [   # (file, url, sha256, family): per family the serif (Times New Roman from 20 pixels up), the sans
    ('NotoSerif-Regular.ttf', NOTO + 'NotoSerif/unhinted/ttf/',
     'a15cfbbc1539d707115111d672d590a3d70d4f74b4c0a315956da20ae19a14e1', 'alphabets'),
    ('NotoSans-Regular.ttf', NOTO + 'NotoSans/unhinted/ttf/',
     'f3961a9cde016d41a4879aecda1474d3a36d6bf54fa0e4643de029cc2248b0e8', 'alphabets'),
    ('NotoSerifTC-Regular.otf', 'https://github.com/notofonts/noto-cjk/raw/Serif2.003/Serif/SubsetOTF/TC/',
     '63515eb0622d589a2f4062e8757c598b3da3bffd053f69abd181c210158ef10c', 'cjk'),
    ('NotoSansTC-Regular.otf', 'https://github.com/notofonts/noto-cjk/raw/Sans2.004/Sans/SubsetOTF/TC/',
     '5bab0cb3c1cf89dde07c4a95a4054b195afbcfe784d69d75c340780712237537', 'cjk'),
]


def family(c):
    """the family of fonts that draws character c: Greek and Cyrillic (and
    Greek Extended) in Noto's, the others in Noto TC's"""
    return 'alphabets' if 0x370 <= c < 0x530 or 0x1f00 <= c < 0x2000 else 'cjk'
FONTTOOLS = ('fonttools-4.55.0-py3-none-any.whl',
             'https://files.pythonhosted.org/packages/b4/4a/786589606d4989cb34d8bc766cd687d955aaf3039c367fe7104bcf82dc98/',
             '12db5888cd4dd3fcc9f0ee60c6edd3c7e1fd44b7dd0f31381ea03df68f8a153f')

# always there: the full-width punctuation a player may type in a name
EXTRA = '，。、；：？！「」『』（）《》〈〉【】～·…—'

CP1252 = set(range(0x20, 0x7f)) | set(range(0xa0, 0x100)) | {
    0x20ac, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x017d,
    0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x017e,
    0x0178}


def composed():
    """the characters src/platform/text.c puts together from the captured
    glyphs (its table marks[])"""
    text = open(os.path.join(ROOT, 'src', 'platform', 'text.c'), encoding='utf-8').read()
    table = text[text.index('} marks[] = {'):]
    table = table[:table.index('};')]
    return {int(c, 16) for c in re.findall(r'\{0x([0-9a-f]{4}),', table)}


def utf8_translations():
    """{path: text} of the translations in UTF-8"""
    out = {}
    for path in sorted(glob.glob(os.path.join(LANG, '*.txt'))):
        data = open(path, 'rb').read()
        lines = data.split(b'\n')
        for i, line in enumerate(lines):
            if line.rstrip(b'\r') == b'@@codepage':
                if i + 1 < len(lines) and lines[i + 1].strip().lower() in (b'utf-8', b'utf8'):
                    out[path] = data.decode('utf-8')
                break
    return out


def used():
    """the characters the UTF-8 translations use (and, with Chinese, EXTRA)
    beyond Windows-1252's, ascending"""
    chars = set()
    for text in utf8_translations().values():
        for line in text.split('\n'):
            if not line.startswith(';'):
                chars |= {ord(c) for c in line}
    if any(family(c) == 'cjk' and c >= 0x2e80 for c in chars):
        chars |= {ord(c) for c in EXTRA}
    return sorted(c for c in chars if c >= 0x80 and c not in CP1252 and not 0x80 <= c < 0xa0)


def wanted(fam=None):
    """the characters the font files (of family fam) must have, ascending"""
    made = composed()
    return [c for c in used() if c not in made and (fam is None or family(c) == fam)]


def cmap(path):
    """the code points an OpenType font maps (its cmap's Unicode subtables,
    formats 4 and 12), read without fontTools"""
    try:
        d = open(path, 'rb').read()
    except OSError:
        return set()
    num, = struct.unpack_from('>H', d, 4)
    out = set()
    for i in range(num):
        tag, _, off, _ = struct.unpack_from('>4sIII', d, 12 + 16 * i)
        if tag != b'cmap':
            continue
        n, = struct.unpack_from('>H', d, off + 2)
        for k in range(n):
            pid, eid, sub = struct.unpack_from('>HHI', d, off + 4 + 8 * k)
            if pid not in (0, 3):
                continue
            p = off + sub
            fmt, = struct.unpack_from('>H', d, p)
            if fmt == 4:
                segs = struct.unpack_from('>H', d, p + 6)[0] // 2
                ends = struct.unpack_from('>%dH' % segs, d, p + 14)
                starts = struct.unpack_from('>%dH' % segs, d, p + 16 + 2 * segs)
                deltas = struct.unpack_from('>%dh' % segs, d, p + 16 + 4 * segs)
                ro = p + 16 + 6 * segs
                offs = struct.unpack_from('>%dH' % segs, d, ro)
                for s in range(segs):
                    for c in range(starts[s], ends[s] + 1):
                        if c == 0xffff:
                            continue
                        if offs[s]:
                            g, = struct.unpack_from('>H', d, ro + 2 * s + offs[s] + 2 * (c - starts[s]))
                            g = (g + deltas[s]) & 0xffff if g else 0
                        else:
                            g = (c + deltas[s]) & 0xffff
                        if g:
                            out.add(c)
            elif fmt == 12:
                ngroups, = struct.unpack_from('>I', d, p + 12)
                for gi in range(ngroups):
                    a, b, g = struct.unpack_from('>III', d, p + 16 + 12 * gi)
                    out.update(range(a, b + 1))
    return out


def fetch(name, url, sha):
    os.makedirs(WORK, exist_ok=True)
    path = os.path.join(WORK, name)
    if not os.path.exists(path) or hashlib.sha256(open(path, 'rb').read()).hexdigest() != sha:
        print('fetching', name)
        data = urllib.request.urlopen(url + name).read()
        if hashlib.sha256(data).hexdigest() != sha:
            sys.exit('%s: not the expected file (SHA-256)' % name)
        open(path, 'wb').write(data)
    return path


def needed():
    """the font files the translations need: [(file, url, sha256, family)]"""
    return [f for f in FONTS if wanted(f[3])]


def check(chars):
    """the characters of chars not in their family's two files"""
    missing = set()
    for name, _, _, fam in FONTS:
        missing |= {c for c in chars if family(c) == fam} - cmap(os.path.join(OUT, name))
    return sorted(missing)


def main():
    chars = wanted()
    if '--check' in sys.argv:
        missing = check(chars)
        if missing:
            sys.exit('data/native/fonts lack %d characters (%s): run tools/dk python3 tools/lang_fonts.py'
                     % (len(missing), ''.join(map(chr, missing[:20]))))
        print('data/native/fonts: all %d characters' % len(chars))
        return
    sys.path.insert(0, fetch(*FONTTOOLS))
    from fontTools import subset, ttLib
    os.makedirs(OUT, exist_ok=True)
    for name in os.listdir(OUT):
        if name.endswith(('.otf', '.ttf')) and name not in [f[0] for f in needed()]:
            os.remove(os.path.join(OUT, name))
            print('removed', name)
    for name, url, sha, fam in needed():
        part = wanted(fam)
        options = subset.Options()
        options.hinting = False
        options.layout_features = []
        options.notdef_outline = False
        options.name_IDs = ['*']
        options.name_languages = ['*']
        font = ttLib.TTFont(fetch(name, url, sha), recalcTimestamp=False)
        sub = subset.Subsetter(options)
        sub.populate(unicodes=part)
        sub.subset(font)
        out = os.path.join(OUT, name)
        font.save(out)
        print('%s: %d characters, %d bytes' % (os.path.relpath(out, ROOT), len(part), os.path.getsize(out)))
    if check(chars):
        sys.exit('the fonts lack characters')


if __name__ == '__main__':
    main()
