#!/usr/bin/env python3
"""A state record's member reached by byte offsets, as an array of its real
element type (step 6, the records' layout):

    cxx_records.py SPEC FILE...

SPEC lines (# comments): OLD NEW BASE DIMS ELEM KIND
  OLD   the member as the code names it (`pollution_sprite_block.unit_sprites`)
  NEW   the array it is part of (`pollution_sprite_block.unit_sprites`)
  BASE  OLD's byte offset in NEW (0x3c: NEW[1])
  DIMS  NEW's strides in bytes, outermost first, the last the element's size
        (0x1e0,0x78,0x3c: Sprite city_sprites[6][4][2])
  ELEM  the element's type (Sprite, i32, i8[260] for a string slot ...)
  KIND  ptr: `(ELEM *)(((i8 *)&OLD) + E)` -> `&NEW[i]...`;
        val: `*(ELEM *)(((i8 *)&OLD) + E)` -> `NEW[i]...`;
        str: `((i8 *)&OLD) + E` -> `NEW[i]...` (a char array, decaying as the
        byte pointer did)
Also `(((T *)&OLD))[E]` (E in units of T) for val, and the same forms
without an offset (OLD's own element).  E's terms keep their text and type:
`X * c` with c a multiple of a stride indexes that dimension (the largest
stride dividing c), `(A * k + B) * c` is expanded, constants go to the
innermost dimension indexed (the rest inwards).  A form the rules do not
cover is listed for hand.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from cxx_fix import match_paren  # noqa: E402


def num(t):
    t = t.strip()
    m = re.fullmatch(r'(-?)\s*(0x[0-9a-fA-F]+|\d+)[uU]?', t)
    return (-1 if m.group(1) else 1) * int(m.group(2), 0) if m else None


def split_terms(e):
    """Top-level terms of a sum: [(sign, text)] (`->` is no minus)."""
    terms, depth, cur, sign = [], 0, '', '+'
    for i, c in enumerate(e):
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        if (depth == 0 and c in '+-' and cur.strip() and not re.search(r'[*/(]\s*$', cur)
                and not (c == '-' and i + 1 < len(e) and e[i + 1] == '>')):
            terms.append((sign, cur.strip()))
            sign, cur = c, ''
        else:
            cur += c
    terms.append((sign, cur.strip()))
    return terms

TSIZE = {'i8': 1, 'u8': 1, 'char': 1, 'byte': 1, 'i16': 2, 'u16': 2, 'short': 2, 'ushort': 2,
         'i32': 4, 'u32': 4, 'int': 4, 'uint': 4, 'iptr': 4, 'uptr': 4}


def strip_parens(t):
    t = t.strip()
    while t.startswith('(') and match_paren(t, 0) == len(t):
        t = t[1:-1].strip()
    return t


def terms(e, mult=1):
    """[(coef, text|None)] of a sum (text None: a constant)."""
    out = []
    for sign, t in split_terms(re.sub(r'\s+', ' ', e)):
        s = -1 if sign == '-' else 1
        t = t.strip()
        n = num(t)
        if n is not None:
            out.append((s * mult * n, None))
            continue
        m = re.fullmatch(r'(.*\S)\s*\*\s*(-?\s*(?:0x[0-9a-fA-F]+|\d+))', t)
        m2 = re.fullmatch(r'(-?(?:0x[0-9a-fA-F]+|\d+))\s*\*\s*(.*)', t)
        if m and match_ok(m.group(1)):
            x, c = m.group(1), num(m.group(2))
        elif m2 and match_ok(m2.group(2)):
            x, c = m2.group(2), num(m2.group(1))
        else:
            x, c = t, 1
            if not match_ok(t):
                return None
        inner = strip_parens(x)
        if (inner != x.strip() and len(split_terms(inner)) > 1 and re.fullmatch(r'\(.*\)', x.strip()) and
                all(match_ok(t2) for _, t2 in split_terms(inner))):
            sub = terms(inner, s * mult * c)
            if sub is None:
                return None
            out += sub
            continue
        out.append((s * mult * c, x.strip()))
    return out


def match_ok(x):
    """x is one operand (no top-level operator of lower precedence than *)."""
    x = x.strip()
    depth = 0
    for i, ch in enumerate(x):
        if ch in '([':
            depth += 1
        elif ch in ')]':
            depth -= 1
        elif depth == 0 and ch in '+-<>=&|^?:,' and i > 0:
            if ch == '-' and x[i + 1:i + 2] == '>' or ch == '>' and x[i - 1] == '-':
                continue
            return False
    return True


def index(total_terms, base, dims):
    """Index texts per dimension, or None."""
    k = base
    var = [[] for _ in dims]
    for c, t in total_terms:
        if t is None:
            k += c
            continue
        for d, s in enumerate(dims):
            if c % s == 0:
                var[d].append((c // s, t))
                break
        else:
            return None
    inner = max([d for d in range(len(dims)) if var[d]], default=None)
    consts = [0] * len(dims)
    if inner is None:
        for d, s in enumerate(dims):
            consts[d], k = divmod(k, s) if d < len(dims) - 1 else (k // s, k % s)
    else:
        consts[inner], k = divmod(k, dims[inner])
        for d in range(inner + 1, len(dims)):
            consts[d], k = divmod(k, dims[d])
    if k:
        return None
    out = []
    for d in range(len(dims)):
        s = ''
        for q, t in var[d]:
            tt = t if q in (1, -1) else '%s * %d' % (t, abs(q))
            if not s:
                s = ('-' if q < 0 else '') + tt
            else:
                s += (' - ' if q < 0 else ' + ') + tt
        c = consts[d]
        if not s:
            s = str(c)
        elif c:
            s += (' - %d' % -c) if c < 0 else (' + %d' % c)
        out.append(s)
    return out


def load(spec):
    rows = []
    for line in open(spec):
        line = line.split('#')[0].strip()
        if not line:
            continue
        old, new, base, dims, elem, kind = line.split()
        rows.append((old, new, int(base, 0), [int(x, 0) for x in dims.split(',')], elem, kind))
    return rows


def rewrite(s, row, fname, report):
    old, new, base, dims, elem, kind = row
    pat = re.compile(r'\(\((i8|u8|char|byte|i16|u16|short|ushort|i32|u32|int|uint) \*\)&%s\)(?![\w.\[])' % re.escape(old))
    out, pos, n = [], 0, 0
    for m in pat.finditer(s):
        if m.start() < pos:
            continue
        line = s.count('\n', 0, m.start()) + 1
        ctype = m.group(1)
        after = s[m.end():]
        start, end = m.start(), m.end()
        e = '0'
        mb = re.match(r'\)\[', after)
        if mb and s[m.start() - 1] == '(':
            # (((T *)&OLD))[E]
            o = m.end() + 1
            cl = match_paren(s, o)
            e = '(%s) * %d' % (s[o + 1:cl - 1], TSIZE[ctype])
            start, end = m.start() - 1, cl
            wrapped = None
        else:
            # find the enclosing parenthesis when followed by ' + '
            ma = re.match(r'\s*\+\s*', after)
            if ma:
                o = m.start() - 1
                while o >= 0 and s[o] == ' ':
                    o -= 1
                if s[o] == '(':
                    cl = match_paren(s, o)
                    inner_end = cl - 1
                    rest = s[m.end() + ma.end():inner_end]
                    # inside a call's argument list: up to the next top-level comma
                    depth = 0
                    for i, ch in enumerate(rest):
                        if ch in '([':
                            depth += 1
                        elif ch in ')]':
                            depth -= 1
                        elif ch == ',' and depth == 0:
                            rest = rest[:i]
                            break
                    e = rest
                    end = m.end() + ma.end() + len(rest)
                    if end == inner_end and not (re.search(r'[\w\]]\s*$', s[:o]) or
                                                   (re.search(r'\)\s*$', s[:o]) and not
                                                    re.search(r'\(\s*(?:const\s+)?\w+\s*\*+\s*\)\s*$', s[:o]))):
                        start, end = o, cl          # the whole parenthesis
                elif re.search(r'(^|[^=!<>])=\s*$', s[:m.start()]):
                    semi = s.index(';', m.end())
                    e = s[m.end() + ma.end():semi]
                    end = semi
                else:
                    report.append('%s:%d: left (no parenthesis): %s' % (fname, line, s[m.start():m.start() + 90]))
                    continue
            e = '(%s) * %d' % (e, TSIZE[ctype]) if TSIZE[ctype] != 1 and e != '0' else e
        tt = terms(e)
        idx = index(tt, base, dims) if tt is not None else None
        if idx is None:
            report.append('%s:%d: left: %s' % (fname, line, ' '.join(s[m.start():end + 10].split())))
            continue
        ref = new + ''.join('[%s]' % i for i in idx)
        # the cast round it
        pre = s[:start]
        mc = re.search(r'(\*\s*)?\(\s*((?:const\s+)?[A-Za-z_]\w*)\s*\*\s*\)\s*$', pre)
        if mc and (s[start] == '(' or start == m.start()):
            castT = mc.group(2).replace('const ', '')
            if kind == 'ptr' and castT == elem and not mc.group(1):
                out.append(s[pos:mc.start()] + '&' + ref)
                pos = end
                n += 1
                continue
            if kind == 'val' and castT == elem and mc.group(1):
                out.append(s[pos:mc.start()] + ref)
                pos = end
                n += 1
                continue
            if kind == 'str' and castT in ('char', 'i8', 'const char') and not mc.group(1):
                if castT == 'i8':
                    out.append(s[pos:mc.start()] + ref)
                else:
                    out.append(s[pos:mc.start()] + '(%s *)%s' % (mc.group(2), ref))
                pos = end
                n += 1
                continue
        if kind == 'str' and ctype in ('i8', 'char'):
            out.append(s[pos:start] + ref)
            pos = end
            n += 1
            continue
        if kind == 'val' and mb:
            out.append(s[pos:start] + ref)
            pos = end
            n += 1
            continue
        if kind == 'ptr':
            out.append(s[pos:start] + '&' + ref)
            pos = end
            n += 1
            report.append('%s:%d: as &%s (check the type)' % (fname, line, ref))
            continue
        report.append('%s:%d: left (form): %s' % (fname, line, ' '.join(s[max(0, start - 20):end + 10].split())))
    out.append(s[pos:])
    return ''.join(out), n


def tidy(s, rows):
    """(&NEW[i])->f -> NEW[i].f; f((&NEW[i]),x) -> f(&NEW[i],x)."""
    for new in sorted({r[1] for r in rows}):
        i = 0
        pat = re.compile(r'\(&%s\[' % re.escape(new))
        while True:
            m = pat.search(s, i)
            if not m:
                break
            j = m.end() - 1
            while j < len(s) and s[j] == '[':
                j = match_paren(s, j)
            if j < len(s) and s[j] == ')':
                ref = s[m.start() + 2:j]
                if s.startswith('->', j + 1):
                    s = s[:m.start()] + ref + '.' + s[j + 3:]
                    i = m.start()
                    continue
                if s[m.start() - 1] in '(,' and s[j + 1] in ',)':
                    s = s[:m.start()] + '&' + ref + s[j + 1:]
                    i = m.start()
                    continue
            i = m.end()
    return s


def main():
    rows = load(sys.argv[1])
    total = 0
    report = []
    for f in sys.argv[2:]:
        s = open(f).read()
        orig = s
        for row in rows:
            s, n = rewrite(s, row, f, report)
            total += n
        if s != orig:
            s = tidy(s, rows)
            open(f, 'w').write(s)
    for r in report:
        print(r)
    print('%d rewritten' % total, file=sys.stderr)


if __name__ == '__main__':
    main()
