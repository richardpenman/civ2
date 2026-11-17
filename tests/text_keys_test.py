#!/usr/bin/env python3
"""Every text of the game's text files is keyed (src/game/lang.cpp): make test.

A text the export (CIV2_EXPORT_TEXT) does not key can be neither exported
nor translated (docs/TRANSLATING.md).  Each section ("@NAME") and block
("@@name" after a line "@" alone: ADVICE.TXT's advice by improvement or unit)
of each of the game's text files that are translated (GAME, MENU, LABELS,
CITY, DESCRIBE, PEDIA, ADVICE, HELP, TUTORIAL, COUNCIL0-2, HERALD, MAPMENU,
DEBUG, the credits) must have its key in the export the game writes
(build/civ2-unittest --text DIR OUT, src/game/lang.cpp lang_export_check):

- of OpenCiv2's own English of them (data/native/text: a folder without the
  game's text files), always;
- of the game's own files, with the game present (GAME_DIR, or $CIV2_GAME);
  skipped without it.

    python3 tests/text_keys_test.py [GAME_DIR]
"""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT = os.path.join(ROOT, 'data', 'native', 'text')
UNITTEST = os.path.join(ROOT, 'build', 'civ2-unittest')
# the files keyed by section (lang.cpp section_files), and the others
SECTION_FILES = ['GAME', 'CITY', 'ADVICE', 'HELP', 'TUTORIAL', 'COUNCIL0', 'COUNCIL1', 'COUNCIL2', 'HERALD', 'PEDIA',
                 'MAPMENU', 'DEBUG', 'CREDITS', 'SCREDITS', 'FCREDITS', 'MPCREDITS']
HDR = re.compile(r'^\s*@([A-Z0-9_]+)\s*$')
BARE = re.compile(r'^\s*@\s*$')
BLOCK = re.compile(r'^\s*@@(.*?)\s*$')
failures = []


def fail(msg):
    failures.append(msg)
    print('FAIL', msg)


def lines_of(path):
    return open(path, 'rb').read().decode('latin-1').replace('\r', '').split('\n')


def find(folder, name):
    """folder's file name.TXT, whatever its case"""
    for f in os.listdir(folder):
        if f.upper() == name + '.TXT':
            return os.path.join(folder, f)
    return None


def block_keys(path, prefix):
    """its sections' and blocks' keys: a block starts at a "@@name" line
    after a line "@" alone (or after a block's end), its other names are
    the "@@" lines after it"""
    keys = []
    state = 'none'          # 'section', 'block', 'none'
    after_at = False
    for line in lines_of(path):
        m = HDR.match(line)
        b = BLOCK.match(line)
        if m:
            state = 'section'
            if m.group(1) not in ('STOP', 'END'):
                keys.append(prefix + m.group(1))
        elif b and b.group(1) and (after_at or state == 'none'):
            keys.append(prefix + '@@' + b.group(1))
            state = 'block'
        elif BARE.match(line) and state == 'block':
            state = 'none'
        after_at = bool(BARE.match(line))
    return keys


def expected(folder):
    """{key: where} of the files in folder"""
    want = {}
    for name in SECTION_FILES:
        path = find(folder, name)
        if path:
            for k in block_keys(path, name.lower() + '.'):
                want.setdefault(k, os.path.basename(path))
    path = find(folder, 'MENU')
    if path:
        n = sum(1 for line in lines_of(path) if HDR.match(line))
        for i in range(1, n + 1):
            want['menu.title.%d' % i] = os.path.basename(path)
    path = find(folder, 'LABELS')
    if path:
        lines = lines_of(path)
        for i, line in enumerate(lines):
            m = HDR.match(line)
            if m and m.group(1) == 'POPUPS':
                want['labels.POPUPS'] = os.path.basename(path)
            elif m and m.group(1) == 'LABELS':
                count = int(lines[i + 1].split()[0])
                for n in range(count):
                    want['label.%d' % n] = os.path.basename(path)
    path = find(folder, 'DESCRIBE')
    if path:
        group = None
        for line in lines_of(path):
            if not line.startswith('@'):
                continue
            name = line[2:].rstrip() if line.startswith('@@') else line[1:].strip()
            if not line.startswith('@@') or name.endswith('_INDEX'):
                group = re.sub(r'(_INDEX|_DESCRIPTIONS)$', '', name.strip())
                continue
            want.setdefault('describe.%s.%s' % (group, name), os.path.basename(path))
    return want


def exported(folder):
    """the keys of the export of the game in folder"""
    with tempfile.TemporaryDirectory() as tmp:
        out = os.path.join(tmp, 'export.txt')
        env = dict(os.environ, CIV2_DATA=os.path.join(ROOT, 'data', 'native'))
        r = subprocess.run([UNITTEST, '--text', os.path.abspath(folder), out], env=env, cwd=os.path.join(ROOT, 'build'),
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        if r.returncode != 0 or not os.path.exists(out):
            fail('%s: no export (civ2-unittest --text: %d)\n%s' % (folder, r.returncode, r.stdout.decode('latin-1')))
            return set()
        return {line[2:] for line in open(out, 'rb').read().decode('latin-1').split('\n') if line.startswith('@@')}


def check(folder, what):
    want = expected(folder)
    keys = exported(what[1])
    missing = sorted(k for k in want if k not in keys)
    for k in missing:
        fail('%s: %s (%s) is not keyed' % (what[0], k, want[k]))
    print('text keys: %s: %d texts keyed' % (what[0], len(want) - len(missing)))


def main():
    if not os.path.exists(UNITTEST):
        print('text keys: no build/civ2-unittest (python3 tools/build_native.py --target linux32test): skipped')
        return
    # OpenCiv2's own English: a folder without the game's text files
    with tempfile.TemporaryDirectory() as empty:
        check(TEXT, ("OpenCiv2's own English (data/native/text)", empty))
    game = sys.argv[1] if len(sys.argv) > 1 else os.environ.get('CIV2_GAME', '')
    if game and os.path.isdir(game) and find(game, 'GAME'):
        check(game, (game, game))
    else:
        print('text keys: no game (GAME_DIR, $CIV2_GAME): its own files skipped')
    if failures:
        sys.exit(1)


if __name__ == '__main__':
    main()
