#!/usr/bin/env python3
"""OpenCiv2's own English of the game's text files (data/native/text, used
where the game has none of its own: src/game/lang.cpp lang_builtin_text_file):
make test.

Each file must have the structure the game's readers need, line for line:
its sections in order, every line that is not text as it is (@width=,
@options, @listbox, @columns=, the High Council's advisor codes and video
frames, DESCRIBE.TXT's indexes ...), as many text lines in each section
(the options, LABELS.TXT's numbered labels, MENU.TXT's items, CITY.TXT's
names ...), each text line's codes (^, ^^, #, ~, a menu's & and |shortcut)
and its placeholders (%STRING0, %NUMBER0, %s ...): in a dialog's lines of
the same codes one after the other, which the game joins and wraps, the
placeholders may move between the lines.  A Civilopedia description
(DESCRIBE.TXT) has any number of paragraphs, and their placeholders.
Comments (";") are free.

The structure expected is tests/text_skeleton.txt (structure only: no
text), written from an installed game by
    python3 tests/text_test.py --write GAME_DIR
With the game present (GAME_DIR, or $CIV2_GAME), the files are compared
with its files' structure too.
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT = os.path.join(ROOT, 'data', 'native', 'text')
SKELETON = os.path.join(ROOT, 'tests', 'text_skeleton.txt')
# the files OpenCiv2 has its own English of (the credits are its own: free)
FILES = ['GAME.TXT', 'LABELS.TXT', 'MENU.TXT', 'CITY.TXT', 'PEDIA.TXT', 'DESCRIBE.TXT', 'ADVICE.TXT', 'HELP.TXT',
         'TUTORIAL.TXT', 'COUNCIL0.TXT', 'COUNCIL1.TXT', 'COUNCIL2.TXT', 'HERALD.TXT', 'MAPMENU.TXT', 'DEBUG.TXT']
CREDITS = ['CREDITS.TXT', 'SCREDITS.TXT', 'FCREDITS.TXT', 'MPCREDITS.TXT']
HDR = re.compile(r'^\s*@([A-Z0-9_]+)\s*$')
PH = re.compile(r'%STRING\d+|%NUMBER\d+|%HEX\d+|%string\d+|%number\d+|%-?\d*l?[sdux]|%%')
# the files whose & (a menu item's letter) and |shortcut are codes
MENU_CODES = {'MENU.TXT', 'MAPMENU.TXT', 'LABELS.TXT'}
# the files whose text lines are read one by one (names, labels, items, spoken lines)
PER_LINE = {'LABELS.TXT', 'MENU.TXT', 'MAPMENU.TXT', 'CITY.TXT', 'COUNCIL0.TXT', 'COUNCIL1.TXT', 'COUNCIL2.TXT'}
COUNCIL = re.compile(r'^((?:\s*[#a-z]?\d*\s*,)+)\s*(.*)$')


def lines_of(path):
    data = open(path, 'rb').read().decode('latin-1').replace('\r', '')
    lines = data.split('\n')
    if lines and lines[-1] == '':
        lines.pop()
    return lines


def text_row(s, name):
    lead = re.match(r'^[\^#~]*', s).group(0)
    menu = ''
    if name not in MENU_CODES:
        pass
    elif '&' in s:
        menu += '&'
    if '|' in s:
        menu += '|' + s.split('|', 1)[1].strip()
    return 'X %s %s %s' % (lead or '-', ','.join(PH.findall(s)) or '-', menu or '-')


def skeleton(path, name):
    """the file's structure: one row per line that matters"""
    rows = []
    sec = None
    entry = None
    paragraph = None
    for l in lines_of(path):
        s = l.strip()
        m = HDR.match(l)
        if name == 'DESCRIBE.TXT' and entry is not None and not s.startswith('@') and not s.startswith(';'):
            if s:
                paragraph |= set(PH.findall(s))
            continue
        if entry is not None:
            rows.append('P %s' % (','.join(sorted(paragraph)) or '-'))
            entry = None
        if s.startswith(';') or (s == '' and name == 'DESCRIBE.TXT'):
            continue
        if s == '':
            rows.append('B')
            continue
        if m:
            sec = m.group(1)
            rows.append('H ' + sec)
            continue
        if name == 'DESCRIBE.TXT':
            if s.startswith('@@'):
                rows.append('E ' + s[2:].strip())
                if not s.endswith('_INDEX'):
                    entry, paragraph = s[2:].strip(), set()
                continue
            if s.startswith('@'):
                rows.append('@')
                continue
            if re.match(r'^-?\d+\s*,', s):
                rows.append('N ' + re.sub(r'\s*;.*$', '', s).replace(' ', '').replace('\t', ''))
                continue
            rows.append('?')
            continue
        if s.startswith('@'):
            low = s.lower()
            if low.startswith('@title=') or low.startswith('@button='):
                rows.append('T%s %s' % (low[1], ','.join(PH.findall(s)) or '-'))
            elif s.startswith('@@'):
                rows.append('E ' + s[2:].strip())
            elif re.match(r'^@[a-z]', s) or s in ('@', '@END'):
                rows.append('D ' + re.sub(r'\s+', '', s))
            else:
                rows.append('@')
            continue
        if sec is None:
            continue        # (before the first section: nothing reads it)
        if name.startswith('COUNCIL'):
            c = COUNCIL.match(s)
            if c and c.group(2).strip() in ('*', 'end', ''):
                rows.append('C %s %s' % (re.sub(r'\s+', '', c.group(1)), c.group(2).strip() or '-'))
            elif s == 'end' or s.startswith('# ('):
                rows.append('C ' + s.split()[0])
            elif c:
                sp = re.match(r'^(\w+:)', c.group(2))
                rows.append('C %s %s %s' % (re.sub(r'\s+', '', c.group(1)), sp.group(1) if sp else '-',
                                            ','.join(PH.findall(s)) or '-'))
            else:
                rows.append('C?')
            continue
        if re.match(r'^\d+(\s*;.*)?$', s) and name in ('LABELS.TXT', 'CITY.TXT'):
            rows.append('N ' + s.split(';')[0].strip())
            continue
        rows.append(text_row(s, name))
    if entry is not None:
        rows.append('P %s' % (','.join(sorted(paragraph)) or '-'))
    return rows if name in PER_LINE else grouped(rows)


def grouped(rows):
    """a dialog's text lines with the same codes, one after the other, as one
    row: their number and placeholders (%STRING0 ... in any order and line,
    %s, %d in order), as the game joins and wraps them"""
    out = []
    run = None
    for r in rows + ['']:
        if r.startswith('X'):
            _, lead, ph, menu = r.split(' ')
            if run and run[0] == lead and menu == '-' and run[3] == '-':
                run[1] += 1
                run[2] += [] if ph == '-' else ph.split(',')
                continue
            if run:
                out.append(group_row(run))
            run = [lead, 1, [] if ph == '-' else ph.split(','), menu]
            continue
        if run:
            out.append(group_row(run))
            run = None
        if r:
            out.append(r)
    return out


def group_row(run):
    lead, n, ph, menu = run
    named = sorted(p for p in ph if p[1].isupper())
    ordered = [p for p in ph if not p[1].isupper()]
    return 'X %s %d %s %s' % (lead, n, ','.join(named + ordered) or '-', menu)


def game_file(game, name):
    for f in os.listdir(game):
        if f.upper() == name:
            return os.path.join(game, f)
    return None


def write(game):
    with open(SKELETON, 'w') as f:
        f.write('# The structure of the game\'s text files, line by line, as the game reads them\n'
                '# (tests/text_test.py; no text): H section, D directive, T title/button, X text\n'
                '# line (codes, placeholders, menu & and |), B blank, E @@entry, N number, P a\n'
                '# description\'s paragraphs (placeholders), C a council line, @ an end mark.\n')
        for name in FILES:
            f.write('== %s\n' % name)
            for r in skeleton(game_file(game, name), name):
                f.write(r + '\n')
    print('wrote', SKELETON)


def expected():
    out, cur = {}, None
    for l in open(SKELETON):
        l = l.rstrip('\n')
        if l.startswith('#'):
            continue
        if l.startswith('== '):
            cur = out.setdefault(l[3:], [])
        else:
            cur.append(l)
    return out


def compare(name, want, got, what):
    if want == got:
        return 0
    for i, (a, b) in enumerate(zip(want, got)):
        if a != b:
            print('FAIL %s: row %d differs from %s: wants %r, has %r' % (name, i + 1, what, a, b))
            return 1
    print('FAIL %s: %d rows, %s has %d' % (name, len(got), what, len(want)))
    return 1


def main():
    if len(sys.argv) > 2 and sys.argv[1] == '--write':
        write(sys.argv[2])
        return 0
    fails = 0
    exp = expected()
    game = sys.argv[1] if len(sys.argv) > 1 else os.environ.get('CIV2_GAME')
    have = sorted(os.path.basename(p) for p in glob.glob(os.path.join(TEXT, '*.TXT')))
    for name in FILES + CREDITS:
        if name not in have:
            print('FAIL %s: not in data/native/text' % name)
            fails += 1
    for name in have:
        if name not in FILES + CREDITS:
            print('FAIL %s: not one of the game\'s text files' % name)
            fails += 1
    for name in FILES:
        if name not in have:
            continue
        got = skeleton(os.path.join(TEXT, name), name)
        fails += compare(name, exp.get(name, []), got, 'tests/text_skeleton.txt')
        if game and os.path.isdir(game) and game_file(game, name):
            fails += compare(name, skeleton(game_file(game, name), name), got, 'the game\'s')
    for name in CREDITS:
        if name in have:
            rows = skeleton(os.path.join(TEXT, name), name)
            if rows[:1] != ['H CREDITS'] or 'H END' not in rows:
                print('FAIL %s: no @CREDITS ... @END' % name)
                fails += 1
    # the files are single bytes (Windows-1252, as the game's fonts)
    for name in have:
        data = open(os.path.join(TEXT, name), 'rb').read()
        try:
            data.decode('cp1252')
        except UnicodeDecodeError as e:
            print('FAIL %s: not Windows-1252: %s' % (name, e))
            fails += 1
        if any(len(l) > 0xfe for l in data.split(b'\n')) and name != 'DESCRIBE.TXT':
            print('FAIL %s: a line longer than the game reads (254 bytes)' % name)
            fails += 1
    print('text_test: %d files, %s' % (len(have), 'FAILED' if fails else 'passed'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
