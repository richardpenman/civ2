#!/usr/bin/env python3
"""The game's struct layouts as the compiler sees them: every member of every
struct in civ2types.h, civ2classes.h, gamedata.h and src/data/state, with its offset, size and
type, from a generated program built with the linux32 flags (offsetof,
sizeof, and the type's name from __PRETTY_FUNCTION__).  Output: run/layout.tsv
(STRUCT, MEMBER, OFFSET, SIZE, TYPE) plus STRUCT, '', 0, SIZE rows.

    tools/dk python3 tools/cxx_layout.py
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HEADERS = ['src/include/civ2types.h', 'src/include/civ2classes.h', 'src/data/gamedata.h'] + \
    sorted(os.path.relpath(p, ROOT) for p in glob.glob(os.path.join(ROOT, 'src/data/state/*.h')))
FLAGS = ['-m32', '-std=gnu++17', '-w', '-include', 'native.h', '-I', 'src/include', '-I', 'src/auto',
         '-I', 'tools', '-I', 'build/native/gen', '-DCIV2_NATIVE', '-DCIV2_LOWMEM', '-fno-exceptions',
         '-fno-rtti', '-fno-pie', '-no-pie']


def strip_comments(t):
    t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
    return re.sub(r'//[^\n]*', ' ', t)


def structs():
    """{name: [member names]} for `typedef struct X { ... } X;` and
    `typedef struct X : B { ... } X;` (members at any depth of anonymous
    unions/structs: their names are the struct's; a derived struct lists its
    bases' members first, offsetof(X, base_member) is X's offset)."""
    out, bases = {}, {}
    for h in HEADERS:
        t = strip_comments(open(os.path.join(ROOT, h)).read())
        for m in re.finditer(r'typedef\s+struct\s+(\w+)\s*(?::\s*(?:public\s+)?(\w+)\s*)?\{', t):
            i, depth = m.end(), 1
            while depth:
                depth += {'{': 1, '}': -1}.get(t[i], 0)
                i += 1
            body = t[m.end():i - 1]
            tail = re.match(r'\s*(\w+)\s*;', t[i:])
            if not tail:
                continue
            body = re.sub(r'\b(union|struct)\s*\{', ' ', body).replace('}', ' ')
            names = []
            for decl in body.split(';'):
                decl = re.sub(r'\[[^\]]*\]', '', decl).strip()
                if re.search(r'\(', decl) or re.match(r'(struct|union|class)\s+\w+$', decl):
                    continue    # a method, a nested type's declaration
                dm = re.search(r'(\w+)\s*$', decl)
                if dm and len(decl.split()) > 1 or (dm and '*' in decl):
                    names.append(dm.group(1))
            out[tail.group(1)] = names
            if m.group(2):
                bases[tail.group(1)] = m.group(2)

    def inherited(s):
        return (inherited(bases[s]) if s in bases else []) + out[s]
    return {s: inherited(s) for s in out}


# Win32 structs the game reads through raw offsets (platform/win32_types.h).
EXTRA = {'RECT': ['left', 'top', 'right', 'bottom'], 'tagRECT': ['left', 'top', 'right', 'bottom']}


def main():
    ss = structs()
    ss.update(EXTRA)
    src = ['#include "auto_common.h"', '#include <stddef.h>',
           'template<class T> const char *tn() { return __PRETTY_FUNCTION__; }',
           '#define M(S, m) printf(#S "\\t" #m "\\t%u\\t%u\\t%s\\n", (unsigned)offsetof(S, m), '
           '(unsigned)sizeof(((S *)0)->m), tn<decltype(((S *)0)->m)>())',
           'int main() {']
    for s, names in ss.items():
        src.append('printf("%s\\t\\t0\\t%%u\\t\\n", (unsigned)sizeof(%s));' % (s, s))
        for n in names:
            src.append('M(%s, %s);' % (s, n))
    src.append('return 0; }')
    os.makedirs(os.path.join(ROOT, 'run'), exist_ok=True)
    cpp = os.path.join(ROOT, 'run', 'layout_gen.cpp')
    exe = os.path.join(ROOT, 'run', 'layout_gen')
    open(cpp, 'w').write('\n'.join(src) + '\n')
    r = subprocess.run(['g++', '-o', exe, cpp, '-x', 'none'] + FLAGS + ['-c', '-o', exe + '.o'],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr[:4000])
    r = subprocess.run(['g++', '-m32', '-no-pie', '-o', exe, exe + '.o'], cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr[:4000])
    rows = subprocess.run([exe], capture_output=True, text=True, check=True).stdout
    out = []
    for line in rows.splitlines():
        f = line.split('\t')
        if f[1]:
            m = re.search(r'\[with T = (.*)\]$', f[4])
            f[4] = m.group(1) if m else f[4]
        out.append('\t'.join(f))
    open(os.path.join(ROOT, 'run', 'layout.tsv'), 'w').write('\n'.join(out) + '\n')
    for p in (cpp, exe, exe + '.o'):
        os.remove(p)
    print(len(ss), 'structs,', len(out) - len(ss), 'members')


if __name__ == '__main__':
    main()
