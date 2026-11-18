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
- every character a code page has beyond Windows-1252 (src/host/codepages.h)
  is one src/platform/text.c puts together from the captured glyphs (its
  table marks[]);
- a translation in UTF-8 ("@@codepage utf-8": Chinese) is valid UTF-8 and
  every character it uses beyond Windows-1252's and marks[] is in both font
  files of its family (Chinese, Greek ...) in data/native/fonts (tools/lang_fonts.py writes them).
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from build_web import english_texts, lang_file  # noqa: E402
import lang_fonts  # noqa: E402

LANG = os.path.join(ROOT, 'data', 'native', 'lang')
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


def check_utf8():
    """the UTF-8 translations' characters: Windows-1252's, made by text.c
    (marks[]) or in both font files of its family in data/native/fonts"""
    for name, _, _, _ in lang_fonts.needed():
        if not os.path.exists(os.path.join(lang_fonts.OUT, name)):
            fail('data/native/fonts/%s: missing (tools/dk python3 tools/lang_fonts.py)' % name)
    missing = lang_fonts.check(lang_fonts.wanted())
    if missing:
        fail('data/native/fonts: no glyphs for %d characters of the translations (%s...): '
             'tools/dk python3 tools/lang_fonts.py' % (len(missing), ''.join(map(chr, missing[:10]))))
    if not os.path.exists(os.path.join(lang_fonts.OUT, 'OFL.txt')):
        fail('data/native/fonts/OFL.txt: missing (the fonts\' licence)')


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
        if cp in ('utf-8', 'utf8'):
            pages.add('utf-8')
        elif cp not in tables:
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
    if 'utf-8' in pages:
        check_utf8()
    made = lang_fonts.composed()
    for cp in sorted(pages - {'utf-8'}):
        for i in range(128):
            u = tables[cp][i]
            if u != tables['windows-1252'][i] and u and u not in made:
                fail('%s: byte 0x%02X (U+%04X) has no glyph: add it to marks[] in src/platform/text.c'
                     % (cp, 0x80 + i, u))
    if failures:
        sys.exit(1)
    print('lang: %d texts, translations and code pages (%s) complete' % (len(en), ', '.join(sorted(pages)) or 'none'))


if __name__ == '__main__':
    main()
