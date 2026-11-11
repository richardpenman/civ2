#!/usr/bin/env python3
"""Drop the casts around members that became objects of a class (a byte
array or an integer before, cast to the class pointer at each use).

    cxx_membercasts.py [--lines A-B] NAME=TYPE[,NAME=TYPE...] FILE...

For each member NAME (of type TYPE now), with P = NAME, this->NAME,
X->NAME or X.NAME:
    ((T *)&P)->f   ((T *)P)->f   ((T *)((u8 *)&P))->f    -> P.f
    (T *)&P        (T *)P        (T *)((u8 *)&P)          -> &P
when T is TYPE or one of its bases (tools/cxx_subobjects.py reads the
class hierarchy).  The compiler checks the result; compare the machine code.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import cxx_subobjects as S  # noqa: E402

OBJ = r'(?:[A-Za-z_]\w*(?:\s*(?:\.|->)\s*[A-Za-z_]\w*)*\s*(?:\.|->)\s*)?'


def main():
    args = sys.argv[1:]
    lines = None
    if args[0] == '--lines':
        lines = tuple(int(x) for x in args[1].split('-'))
        args = args[2:]
    pairs = dict(p.split('=', 1) for p in args[0].split(','))
    S.load()
    total = 0
    for path in args[1:]:
        whole = open(path).read()
        if lines:
            ls = whole.split('\n')
            head, text, tail = ls[:lines[0] - 1], '\n'.join(ls[lines[0] - 1:lines[1]]), ls[lines[1]:]
        else:
            text = whole
        n = 0
        for name, ty in pairs.items():
            ok = lambda t: S.norm(t) == ty or S.is_base(S.norm(t), ty)  # noqa: E731
            inner = r'(?P<p>%s\b%s\b)' % (OBJ, re.escape(name))
            operand = r'(?:&\s*%s|%s|\(\s*\(\s*u8\s*\*\s*\)\s*&\s*%s\s*\))' % (inner, inner.replace('?P<p>', '?P<q>'),
                                                                             inner.replace('?P<p>', '?P<r>'))
            cast = r'\(\s*(?P<t>[A-Za-z_]\w*)\s*\*\s*\)\s*' + operand

            def arrow(m):
                if not ok(m.group('t')):
                    return m.group(0)
                p = m.group('p') or m.group('q') or m.group('r')
                return re.sub(r'\s+', '', p) + '.'
            text, k1 = re.subn(r'\(\s*' + cast + r'\s*\)\s*->\s*', lambda m: arrow(m), text)

            def addr(m):
                if not ok(m.group('t')):
                    return m.group(0)
                p = m.group('p') or m.group('q') or m.group('r')
                return '&' + re.sub(r'\s+', '', p)
            text, k2 = re.subn(cast + r'(?!\s*(?:\[|\.|->|\w))', addr, text)
            n += k1 + k2
        if n:
            if lines:
                text = '\n'.join(head + [text] + tail)
            open(path, 'w').write(text)
        total += n
    print(total, 'rewritten')


if __name__ == '__main__':
    main()
