#!/usr/bin/env python3
"""The translations of OpenCiv2's own texts (docs/TRANSLATING.md): make test.

- data/native/lang/en.txt, the template, is src/host/texts.h's table as the
  game's export writes it (CIV2_EXPORT_TEXT with CIV2_EXPORT_OWN=1);
- each translation (data/native/lang/*.txt) has every key of the template
  (OpenCiv2's own, "openciv2.*") and no other of them, each text the
  template's placeholders (%s, %d) in their order, and is readable in its
  code page; its keys of the game's texts (game.*, label.*, rules.* ...: the
  English is the player's copy of the game, not here) are of the files
  src/game/lang.cpp keys, and no key is there twice;
- a code page's glyphs (data/native/fonts_<code page>.bin, _aa.bin) cover
  every font (face, height, weight, slant) of fonts_win.bin and
  fonts_win_aa.bin, each byte where the code page differs from
  Windows-1252 (src/host/codepages.h).
"""
import glob
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from build_web import english_texts, lang_file  # noqa: E402

LANG = os.path.join(ROOT, 'data', 'native', 'lang')
NATIVE = os.path.join(ROOT, 'data', 'native')
failures = []
# the prefixes of the game's texts' keys (src/game/lang.cpp: section_files,
# menu.*, label.*, rules.*, describe.*)
GAME_KEYS = {'game', 'city', 'advice', 'help', 'tutorial', 'council0', 'council1', 'council2', 'herald', 'pedia',
             'mapmenu', 'debug', 'credits', 'scredits', 'fcredits', 'mpcredits', 'labels', 'menu', 'label', 'rules',
             'describe'}


def fail(msg):
    failures.append(msg)
    print('FAIL', msg)


def export_of(texts):
    """en.txt as src/game/lang.cpp's export (own keys) writes it."""
    out = ['; OpenCiv2 text (English), keyed: "@@key" and its text on the lines after it',
           '; (a line of text that starts with "@@" or "\\" has a "\\" put before it).',
           "; OpenCiv2's own texts only: the template of a translation (docs/TRANSLATING.md).",
           "; Encoding: Windows-1252, as the game's fonts; placeholders (%s, %d) stay, in order."]
    for key, text in texts.items():
        out.append('@@openciv2.' + key)
        for line in text.split('\n'):
            out.append(('\\' if line.startswith('@@') or line.startswith('\\') else '') + line)
    return '\n'.join(out) + '\n'


def placeholders(text):
    return re.findall(r'%[sd]', text.replace('%%', ''))


def codepage_tables():
    """src/host/codepages.h: {name: [128 code points]}."""
    text = open(os.path.join(ROOT, 'src', 'host', 'codepages.h')).read()
    tables = {}
    for name, body in re.findall(r'static const unsigned short codepage_(\w+)\[128\] = \{([^}]*)\}', text):
        tables[name] = [int(v, 16) for v in re.findall(r'0x[0-9a-f]+', body)]
    names = dict(re.findall(r'\{"([\w-]+)", codepage_(\w+)\}', text))
    return {n: tables[t] for n, t in names.items()}


def font_keys(path, magic, delta):
    """The fonts of a font file (or delta): {(face, height, bold, italic)}, its chars (delta)."""
    d = open(path, 'rb').read()
    if d[:8] != magic:
        fail('%s: not a %s file' % (os.path.relpath(path, ROOT), magic.decode()))
        return set(), b''
    count, = struct.unpack_from('<I', d, 8)
    if delta:
        nchars, = struct.unpack_from('<I', d, 12)
        chars = d[16:16 + nchars]
        p = 16 + nchars
    else:
        chars, p = bytes(range(256)), 12
    keys = set()
    aa = magic in (b'C2FONTAA', b'C2FONTDA')
    for _ in range(count):
        face, height, weight, italic = d[p], d[p + 1], d[p + 2] | d[p + 3] << 8, d[p + 4]
        keys.add((face, height, weight >= 600, italic != 0))
        p += 8 if delta else 52
        for _ in chars:
            if aa:
                n, = struct.unpack_from('<H', d, p + 6)
                p += 8 + n
            else:
                w, h = d[p + 6], d[p + 7]
                p += 8 + (w + 7) // 8 * h
    if p != len(d):
        fail('%s: %d bytes, read %d' % (os.path.relpath(path, ROOT), len(d), p))
    return keys, chars


def main():
    en = english_texts()
    template = os.path.join(LANG, 'en.txt')
    if open(template, 'rb').read() != export_of(en).encode('cp1252'):
        fail('data/native/lang/en.txt is not src/host/texts.h (write it with CIV2_EXPORT_TEXT, CIV2_EXPORT_OWN=1)')
    tables = codepage_tables()
    pages = set()
    for path in sorted(glob.glob(os.path.join(LANG, '*.txt'))):
        code = os.path.basename(path)[:-4]
        if code == 'en':
            continue
        name = os.path.relpath(path, ROOT)
        try:
            cp, texts = lang_file(path)
        except (UnicodeDecodeError, LookupError) as e:
            fail('%s: not readable in its code page (%s)' % (name, e))
            continue
        if cp not in tables:
            fail('%s: code page %s is not in src/host/codepages.h' % (name, cp))
        elif tables[cp] != tables['windows-1252']:
            pages.add(cp)
        if 'needs review by a native speaker' not in open(path, 'rb').read().decode('latin-1'):
            fail('%s: no "needs review by a native speaker" in its header' % name)
        keys = {k[9:] for k in texts if k.startswith('openciv2.')}
        other = {k for k in texts if not k.startswith('openciv2.') and k != 'codepage'}
        for k in sorted(set(en) - keys):
            fail('%s: missing %s' % (name, k))
        for k in sorted(keys - set(en)):
            fail('%s: %s is not in the template' % (name, k))
        for k in sorted(other):
            if k.split('.')[0] not in GAME_KEYS or '.' not in k:
                fail('%s: %s is no key of OpenCiv2\'s or of the game\'s texts' % (name, k))
        seen = set()
        for line in open(path, 'rb').read().decode('latin-1').splitlines():
            if line.startswith('@@'):
                if line in seen:
                    fail('%s: %s twice' % (name, line[2:]))
                seen.add(line)
        for k in sorted(keys & set(en)):
            if placeholders(texts['openciv2.' + k]) != placeholders(en[k]):
                fail('%s: %s has placeholders %s, the template %s' % (name, k, placeholders(texts['openciv2.' + k]),
                                                                      placeholders(en[k])))
    base, _ = font_keys(os.path.join(NATIVE, 'fonts_win.bin'), b'C2FONTS2', False)
    base_aa, _ = font_keys(os.path.join(NATIVE, 'fonts_win_aa.bin'), b'C2FONTAA', False)
    for cp in sorted(pages):
        differ = bytes(0x80 + i for i in range(128) if tables[cp][i] != tables['windows-1252'][i])
        for suffix, magic, want in (('', b'C2FONTD1', base), ('_aa', b'C2FONTDA', base_aa)):
            path = os.path.join(NATIVE, 'fonts_%s%s.bin' % (cp, suffix))
            if not os.path.exists(path):
                fail('%s: missing (tools/probe/fontdump%s.c %s)' % (os.path.relpath(path, ROOT), suffix, cp))
                continue
            keys, chars = font_keys(path, magic, True)
            if keys != want:
                fail('%s: %d fonts, not those of the full file (%d)' % (os.path.relpath(path, ROOT), len(keys), len(want)))
            if chars != differ:
                fail('%s: its bytes are not those where %s differs from Windows-1252' % (os.path.relpath(path, ROOT), cp))
    if failures:
        sys.exit(1)
    print('lang: %d texts, translations and code pages (%s) complete' % (len(en), ', '.join(sorted(pages)) or 'none'))


if __name__ == '__main__':
    main()
