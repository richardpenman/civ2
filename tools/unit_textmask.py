#!/usr/bin/env python3
"""Where the game's data holds text, for the unit tests (tests/unit_format.h):
every `char` array in the game's data (src/data/gamedata.cpp) and every i8
array whose name says text (TEXTY), at any depth (the members of records,
of records in records and of arrays of them), from the program's debugging
information (gdb ptype/o), plus the members named in EXTRA_NAMES and the
ranges in EXTRA_RANGES.  Names
and messages from the game's files land there; the recorder and the runner
set those bytes to what the game's initializers left there, so the recorded
cases hold none of that text.  Writes tests/unit/textmask.tsv: offset in
the data, length, where.  Runs in the container on a unit-test build:

    python3 tools/unit_textmask.py build/civ2-unittest > tests/unit/textmask.tsv
"""
import re
import subprocess
import sys

BASE = 0x30000000           # the data's address in the unit-test builds (build_native.py)
# i8 arrays (the decompiler's type for the game's text buffers, and for
# numbers too) whose name says text.
TEXTY = re.compile(r'(name|title|path|text|buf|description|address|password|subdir|filter|txt|info|version|adjective|'
                   r'caption|message|label|section_name|_str|names|titles|strs|dir|home|name_ends|name_recs)s?$')
# Members (u8, or i8 not named so) that hold text.
EXTRA_NAMES = {'credits_text_block', 'casualty_log_killer_name_ends', 'textbuf'}
# Ranges no member names: (variable, offset, length).  Windows' titles at
# +0x134 of their window objects; the formatted message's arguments.
EXTRA_RANGES = [('city_window_block', 0x134, 0x100), ('map_views_block', 0x134, 0x100),
                ('minimap_window_block', 0x134, 0x100), ('status_window_block', 0x134, 0x100),
                ('minimap_window_block', 1048 + 24, 6280 - 24),
                ('civ_saved_cursor_x_block', 0x34, 255)]   # the passwords (a union's members are not listed)
MEMBER = re.compile(r'^/\*\s*(\d+)(?::\s*\d+)?\s*\|\s*(\d+)\s*\*/\s*(.*)$')
FIELD = re.compile(r'^(?:const |volatile |struct |class |union |enum )*([\w:]+)\s+(\w+)((?:\[\d+\])*)(?:\s*:\s*\d+)?;$')
SCALARS = {'char', 'i8', 'u8', 'i16', 'u16', 'i32', 'u32', 'i64', 'u64', 'int', 'uint', 'short', 'long', 'unsigned',
           'iptr', 'uptr', 'byte', 'float', 'double', 'ushort', 'uchar', 'ghidra_bool', 'bool'}


def gdb(exe, cmds):
    script = '/tmp/unit_textmask.gdb'
    with open(script, 'w') as f:
        for key, cmd in cmds:
            f.write('echo @@%s\\n\n%s\n' % (key, cmd))
    out = subprocess.run(['gdb', '-batch', '-x', script, exe], capture_output=True, text=True).stdout
    res = {}
    for block in out.split('@@')[1:]:
        key, _, rest = block.partition('\n')
        res[key] = rest
    return res


def count(dims):
    n = 1
    for d in re.findall(r'\[(\d+)\]', dims):
        n *= int(d)
    return n


def is_record(text):
    return bool(re.search(r'type = (struct|class|union)\b', text[:200]))


class Types:
    def __init__(self, exe):
        self.exe = exe
        self.text = {}          # type name -> ptype/o output
        self.memo = {}

    def load(self, names):
        names = [n for n in set(names) if n not in self.text]
        if names:
            res = gdb(self.exe, [(n, 'ptype/o %s' % n) for n in names])
            for n in names:
                self.text[n] = res.get(n, '')

    @staticmethod
    def wanted(text):
        """The named types a ptype/o output's members are of."""
        out = []
        for line in text.splitlines():
            m = MEMBER.match(line)
            if m and '*' not in m.group(3):
                f = FIELD.match(m.group(3).strip())
                if f and f.group(1) not in SCALARS:
                    out.append(f.group(1))
        return out

    def ranges(self, text, size):
        """Text ranges (offset, length, member) in a value whose ptype/o is text."""
        stack = [[0, size, []]]
        for line in text.splitlines():
            m = MEMBER.match(line)
            if m:
                o, s, decl = int(m.group(1)), int(m.group(2)), m.group(3).strip()
                if decl.endswith('{'):
                    stack.append([o, s, []])
                    continue
                f = FIELD.match(decl)
                if not f or '*' in decl:
                    continue
                t, name, dims = f.groups()
                if (dims and (t == 'char' or (t == 'i8' and TEXTY.search(name)))) or name in EXTRA_NAMES:
                    stack[-1][2].append((o, s, name))
                elif is_record(self.text.get(t, '')):
                    inner = self.of_type(t)
                    n = count(dims)
                    el = s // n if n else s
                    for k in range(n):
                        stack[-1][2] += [(o + k * el + a, b, name + '.' + p) for a, b, p in inner if a + b <= el]
                continue
            m = re.match(r'^\s*}\s*(\w*)\s*((?:\[\d+\])*)\s*;\s*$', line)
            if m and len(stack) > 1:
                o, s, inner = stack.pop()
                n = count(m.group(2))
                el = s // n if n else s
                for k in range(n):
                    # (inline records: their members' offsets are the outer record's)
                    stack[-1][2] += [(a + k * el, b, p) for a, b, p in inner]
        return stack[0][2]

    def of_type(self, t):
        if t not in self.memo:
            self.memo[t] = []           # (recursion)
            text = self.text.get(t, '')
            m = re.search(r'total size \(bytes\):\s*(\d+)', text)
            self.memo[t] = self.ranges(text, int(m.group(1)) if m else 0)
        return self.memo[t]


def main():
    exe = sys.argv[1]
    nm = subprocess.run(['nm', '-S', '--defined-only', exe], capture_output=True, text=True).stdout
    syms = []
    for line in nm.splitlines():
        p = line.split()
        if len(p) == 4 and p[2] in 'dDbB' and BASE <= int(p[0], 16) < BASE + 0x200000:
            syms.append((int(p[0], 16) - BASE, int(p[1], 16), p[3]))
    syms.sort()
    res = gdb(exe, [(n, "whatis '%s'\nptype/o '%s'" % (n, n)) for o, s, n in syms])
    types = Types(exe)
    todo = []
    for o, s, n in syms:
        todo += types.wanted(res.get(n, ''))
    while todo:                         # the record types, as deep as they go
        types.load(todo)
        todo = [x for t in set(todo) for x in types.wanted(types.text.get(t, '')) if x not in types.text]
    ranges = []
    for off, size, name in syms:
        lines = res.get(name, '').strip().splitlines()
        first = lines[0] if lines else ''          # whatis: the declared type
        text = '\n'.join(lines[1:])
        m = re.match(r'^type = (\w+) ((?:\[\d+\])+)$', first)
        if m and (m.group(1) == 'char' or (m.group(1) == 'i8' and TEXTY.search(name))):
            ranges.append((off, size, name))
            continue
        inner = types.ranges(text, size)
        m = re.search(r'^\s*}\s*((?:\[\d+\])+)\s*$', text, re.M)     # an array of records: "} [256]"
        if m and inner:
            el = size // count(m.group(1))
            inner = [(a + k * el, b, p) for k in range(count(m.group(1))) for a, b, p in inner if a < el]
        for a, b, p in inner:
            if a < size:
                ranges.append((off + a, min(b, size - a), name + '.' + p))
    where = {n: o for o, s, n in syms}
    ranges += [(where[n] + o, ln, '%s+0x%x' % (n, o)) for n, o, ln in EXTRA_RANGES if n in where]
    ranges.sort()
    merged = []
    for a, b, n in ranges:
        if merged and a <= merged[-1][0] + merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], a + b - merged[-1][0])
        else:
            merged.append([a, b, n])
    sys.stdout.write('# where the game\'s data holds text (tools/unit_textmask.py): offset, length, where\n')
    for a, b, n in merged:
        sys.stdout.write('%x\t%d\t%s\n' % (a, b, n))
    sys.stderr.write('%d ranges, %d bytes\n' % (len(merged), sum(b for a, b, n in merged)))


if __name__ == '__main__':
    main()
