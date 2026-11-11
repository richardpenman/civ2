#!/usr/bin/env python3
"""Members in place of raw byte offsets: `(T *)((int)p + N)`,
`(T *)((u8 *)p + N)`, `*(T *)((iptr)p + N)`, `((T *)(((int)this) + N))->f()`
where p is declared `S *` (a parameter, a local, `this`, a global or a
member chain of them) and S has a member at offset N of type T (or of a
class derived from T, an element of an array of T, a member of a nested
struct).

    cxx_subobjects.py [--apply] [--generic] FILE...

Rewrites:
    ((T *)(B + N))->f    -> P.f
    *(T *)(B + N)        -> P            (T the member's type)
                            (T)P         (an integer of T's size, read)
    (T *)(B + N)         -> &P           (T the member's type or a base of it)
                            (T *)&P, (T *)P (void, char ...: any member,
                                            with --generic; else a member array)
                            (T *)&P      (a scalar member of T's size)
    *(T *)(B + N)        -> *(T *)&P     (the same)
    (I)(B + N)           -> (I)&P        (an address as an integer)
P is p->path (`path` alone for this in a member function).  Without
--apply: what would change (run/subobjects.tsv) and what stays (with the
reason).  Every rewrite's offset is listed as a static_assert in
run/subobjects_check.cpp (compile it).  Base layout: run/layout.tsv
(tools/cxx_layout.py).
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import cxx_fields  # noqa: E402
from cxx_fields import masked, functions, canon, prev_tok, next_tok, is_statement_start, STORE_OK  # noqa: E402
from cxx_fix import match_paren  # noqa: E402

LAY = None
GENERIC_OK = False
BASES = {}
INT_CASTS = r'(?:int|iptr|uptr|u32|uint|i32|long|ulong|unsigned int|DWORD)'
BYTE_PTRS = r'(?:u8|byte|char|i8|uchar|undefined|BYTE|unsigned char)\s*\*'
EXPR = r'[A-Za-z_]\w*(?:\s*(?:\.|->)\s*[A-Za-z_]\w*)*'
NUM = r'(?:0x[0-9a-fA-F]+|\d+)'
TYPE = r'(?:const\s+)?[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*'
# (T *)( BASE + N ): BASE = ((int)E) | (int)E | (u8 *)E | E
# (T *)(I)(BASE + N), (I)(BASE + N): an address, its integer value
RAW = re.compile(r'(?:\(\s*(?P<T>%s)\s*(?P<stars>\*+)\s*\)\s*(?:\(\s*%s\s*\)\s*)?|(?P<it>\(\s*%s\s*\))\s*)'
                 r'\(\s*(?P<base>'
                 r'\(\s*\(\s*%s\s*\)\s*(?P<e1>%s)\s*\)'
                 r'|\(\s*(?P<c2>%s|%s)\s*\)\s*(?P<e2>%s)'
                 r'|(?P<e3>%s))\s*\+\s*(?P<n>%s)\s*\)'
                 % (TYPE, INT_CASTS, INT_CASTS, INT_CASTS, EXPR, INT_CASTS, BYTE_PTRS, EXPR, EXPR, NUM))
GENERIC = {'void', 'char', 'u8', 'i8', 'byte', 'uchar', 'unsigned char', 'signed char', 'undefined'}
INTS = set(cxx_fields.SIGNED) | set(cxx_fields.UNSIGNED)


def norm(t):
    t = re.sub(r'\s+', ' ', t.strip())
    t = re.sub(r'^const ', '', t)
    t = re.sub(r'\s*\*', '*', t)
    t = re.sub(r'^struct ', '', t)
    return t


def load():
    global LAY
    cxx_fields.LAYOUT = cxx_fields.load_layout()
    LAY = cxx_fields.LAYOUT
    for h in ['src/include/civ2types.h', 'src/include/civ2classes.h'] + glob.glob(os.path.join(ROOT, 'src/data/state/*.h')):
        t = open(os.path.join(ROOT, h)).read()
        for m in re.finditer(r'struct\s+(\w+)\s*:\s*(?:public\s+)?(\w+)\s*\{', t):
            BASES[m.group(1)] = m.group(2)


def is_base(b, d):
    while d in BASES:
        d = BASES[d]
        if d == b:
            return True
    return False


def split_array(t):
    m = re.match(r'^(.*?) ?\[(\d+)\](.*)$', t)
    if not m:
        return None
    return norm(m.group(1) + m.group(3)), int(m.group(2))


def size_of(t):
    t = norm(t)
    if t.endswith('*'):
        return 4
    a = split_array(t)
    if a:
        s = size_of(a[0])
        return s * a[1] if s else None
    if t in LAY:
        return LAY[t]['size']
    c = canon(t)
    return c[1] if c[0] in 'su' else None


def unnamed(m):
    return re.match(r'(field|pad|unk|gap)_|.*_bytes$|.*_rest$', m) is not None


def find(s, n, t, deref, depth=0):
    """[(kind, path, member type)] of S's members at offset n read/addressed as t."""
    out = []
    for m, off, size, mt in LAY[s]['members']:
        if not off <= n < off + size:
            continue
        mt = norm(mt)
        a = split_array(mt)
        if a:
            et, k = a
            es = size // k if k else 0
            if not es:
                continue
            idx, rem = (n - off) // es, (n - off) % es
            p = '%s[%d]' % (m, idx)
            if rem == 0:
                kd = match(et, t, deref)
                if kd:
                    out.append((kd, p, et, unnamed(m)))
                if idx == 0 and not deref and (t in GENERIC or t is None):
                    out.append(('array', m, mt, unnamed(m)))
            if et in LAY and depth < 5:
                for kd, pp, tt, un in find(et, rem, t, deref, depth + 1):
                    out.append((kd, p + '.' + pp, tt, un or unnamed(m)))
            continue
        if off == n:
            kd = match(mt, t, deref)
            if kd:
                out.append((kd, m, mt, unnamed(m)))
        if mt in LAY and depth < 5:
            for kd, pp, tt, un in find(mt, n - off, t, deref, depth + 1):
                out.append((kd, m + '.' + pp, tt, un or unnamed(m)))
    return out


def scalar_size(t):
    if t.endswith('*') or t.endswith(')'):
        return 4
    c = canon(t)
    return c[1] if c[0] in 'su' else None


def match(mt, t, deref):
    if t is None:
        return 'addr'
    if mt == t:
        return 'exact'
    if deref:
        a, b = canon(mt), canon(t)
        if a[0] in 'su' and b[0] in 'su' and a[1] == b[1]:
            return 'sign'
    if not deref and is_base(t, mt):
        return 'base'
    if not deref and t in GENERIC:
        return 'generic'
    if scalar_size(mt) and scalar_size(mt) == scalar_size(t):
        return 'alias'      # the same bytes as another scalar type: *(T *)&P
    return None


RANK = {'exact': 0, 'base': 1, 'sign': 2, 'addr': 2, 'array': 3, 'alias': 4, 'generic': 5}


def best(cands):
    if not cands:
        return None
    cands.sort(key=lambda c: (c[3], RANK[c[0]], c[1].count('.') + c[1].count('['), len(c[1])))
    return cands[0]


def decl_type(text, name, start=0, end=None):
    """Declared type of name in text[start:end] (a declaration `T *name`), or None."""
    seg = text[start:end]
    for m in re.finditer(r'(?:^|[;{(,]|\n)\s*((?:const\s+)?[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*\s*\**)\s*\b%s\s*(?=[;=,)\[])'
                         % re.escape(name), seg):
        t = m.group(1).strip()
        if t.split()[0] in ('return', 'goto', 'case', 'else', 'delete'):
            continue
        return norm(t)
    return None


GLOBALS = None


def global_type(name):
    global GLOBALS
    if GLOBALS is None:
        GLOBALS = {}
        for h in [os.path.join(ROOT, 'src/data/gamedata.h')] + glob.glob(os.path.join(ROOT, 'src/data/state/*.h')):
            for line in open(h):
                m = re.match(r'\s*(?:extern\s+|inline\s+constexpr\s+)*((?:const\s+)?[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*\s*[*&]*)\s*(\w+)\s*(?:\[[^\]]*\])?\s*(?:=|;)', line)
                if m and ('extern' in line or 'constexpr' in line):
                    GLOBALS.setdefault(m.group(2), norm(m.group(1).replace('&', '')))
    return GLOBALS.get(name)


def member_type(s, name):
    for m, off, size, mt in LAY.get(s, {'members': []})['members']:
        if m == name:
            return norm(mt)
    return None


def expr_type(e, text, hs, bs, be, cls):
    parts = re.split(r'\s*(\.|->)\s*', e.strip())
    first = parts[0]
    if first == 'this':
        t = cls + '*' if cls else None
    else:
        t = decl_type(text, first, hs, be) or (member_type(cls, first) if cls else None) or global_type(first)
    i = 1
    while t and i < len(parts):
        op, name = parts[i], parts[i + 1]
        s = t[:-1] if op == '->' and t.endswith('*') else t if op == '.' else None
        if s is None or s not in LAY:
            return None
        t = member_type(s, name)
        i += 2
    return t


def class_of(header):
    m = re.search(r'\b(\w+)::~?\w+\s*\(', header)
    return m.group(1) if m and m.group(1) in LAY else None


def process(path, apply, rows, checks):
    text = open(path).read()
    mt = masked(text)
    edits = []
    for hs, bs, be in functions(text):
        header = text[hs:bs]
        cls = class_of(header)
        for m in RAW.finditer(mt, bs, be):
            it = m.group('it')
            if it and not (m.group('e1') or m.group('e2')):
                continue        # integer arithmetic
            t = norm(m.group('T')) if not it else None
            stars = m.group('stars') or ''
            e = m.group('e1') or m.group('e2') or m.group('e3')
            n = int(m.group('n'), 0)
            line = text.count('\n', 0, m.start()) + 1
            where = '%s:%d' % (path, line)
            src = re.sub(r'\s+', ' ', text[m.start():m.end()])
            if len(stars) > 1:
                t = t + '*' * (len(stars) - 1)
            et = expr_type(e, text, hs, bs, be, cls)
            bytewise = bool(m.group('e1') or m.group('e2'))
            if not et or not et.endswith('*'):
                rows.append((where, 'stay', src, 'base %s: %s' % (e, et)))
                continue
            if not bytewise:
                pt = et[:-1]
                if pt not in GENERIC and canon(pt)[0] not in 'su':
                    rows.append((where, 'stay', src, 'scaled: %s is %s' % (e, et)))
                    continue
                rows.append((where, 'stay', src, 'byte pointer %s is %s' % (e, et)))
                continue
            s = et[:-1].strip()
            if s not in LAY:
                rows.append((where, 'stay', src, 'not a struct: %s' % et))
                continue
            # the context
            a, b = m.start(), m.end()
            pt_, pj = prev_tok(mt, a)
            deref = pt_ == '*' and not re.search(r'[\w)\]]\s*$', mt[:pj])
            arrow = False
            if not deref and pt_ == '(':
                nt_, nj = next_tok(mt, b)
                if nt_ == ')':
                    nt2, nj2 = next_tok(mt, nj + 1)
                    p2, _ = prev_tok(mt, pj)
                    if nt2 in ('->',) and not (re.match(r'\w', p2 or '') and p2 not in cxx_fields.KEYWORDS):
                        arrow = True
            c = best(find(s, n, t, deref))
            if not c:
                rows.append((where, 'stay', src, 'no member of %s at 0x%x as %s' % (s, n, t)))
                continue
            kind, mpath, mtype, _ = c
            if kind == 'generic' and not GENERIC_OK:
                rows.append((where, 'stay', src, 'generic: %s %s' % (mpath, mtype)))
                continue
            first = re.match(r'\w+', mpath).group(0)
            if e.strip() == 'this' and cls and not decl_type(text, first, hs, be):
                obj = ''
            else:
                obj = re.sub(r'\s+', '', e) + '->'
            P = obj + mpath
            if it:
                if deref or arrow:
                    rows.append((where, 'stay', src, 'integer address used as a pointer'))
                    continue
                new = '%s&%s' % (it, P)
                edits.append((a, b, new))
            elif arrow:
                if kind not in ('exact', 'base'):
                    rows.append((where, 'stay', src, 'arrow on %s member %s' % (kind, mpath)))
                    continue
                nt_, nj = next_tok(mt, b)
                nt2, nj2 = next_tok(mt, nj + 1)
                edits.append((pj, nj2 + 2, P + '.'))
                new = P + '.'
            elif deref:
                nt_, nj = next_tok(mt, b)
                if kind == 'exact':
                    new = P
                elif kind == 'sign':
                    store = nt_ in STORE_OK or nt_ in ('++', '--')
                    if store:
                        if not is_statement_start(mt, pj):
                            rows.append((where, 'stay', src, 'store in an expression, other sign'))
                            continue
                        new = P
                    else:
                        new = '(%s)%s' % (m.group('T').strip(), P)
                elif kind == 'alias':
                    p0, _ = prev_tok(mt, pj)
                    store = nt_ in STORE_OK or nt_ in ('++', '--', '=') or p0 in ('&', '++', '--')
                    vt = cxx_fields.cast_type(m.group('T').strip() + ' ' + stars[1:]) if len(stars) > 1 \
                        else m.group('T').strip()
                    if store:
                        new = '*(%s%s)&%s' % (m.group('T').strip(), ' ' + stars, P)
                        edits.append((a, b, '(%s%s)&%s' % (m.group('T').strip(), ' ' + stars, P)))
                    else:
                        new = '(%s)%s' % (vt, P)
                        edits.append((pj, b, new))
                    rows.append((where, kind, src, new))
                    checks.append((s, mpath, n))
                    continue
                else:
                    continue
                edits.append((pj, b, new))
            else:
                p2, _ = prev_tok(mt, a)
                if p2 == 'sizeof':
                    rows.append((where, 'stay', src, 'sizeof'))
                    continue
                if kind in ('exact', 'base'):
                    new = '&' + P
                elif kind == 'alias':
                    new = '(%s%s)&%s' % (m.group('T').strip(), ' ' + stars, P)
                elif kind == 'array':
                    new = '(%s%s)%s' % (m.group('T').strip(), ' ' + stars, P)
                else:
                    new = '(%s%s)&%s' % (m.group('T').strip(), ' ' + stars, P)
                nt_, _ = next_tok(mt, b)
                if new.startswith('&') and nt_ in ('[', '->', '.', '++', '--', '('):
                    new = '(' + new + ')'
                edits.append((a, b, new))
            rows.append((where, kind, src, new))
            checks.append((s, mpath, n))
    if apply and edits:
        open(path, 'w').write(cxx_fields.apply(text, edits))
    return len(edits)


def main():
    args = sys.argv[1:]
    global GENERIC_OK
    apply = '--apply' in args
    GENERIC_OK = '--generic' in args
    files = [a for a in args if not a.startswith('--')]
    load()
    rows, checks = [], []
    total = 0
    for f in files:
        total += process(f, apply, rows, checks)
    os.makedirs(os.path.join(ROOT, 'run'), exist_ok=True)
    with open(os.path.join(ROOT, 'run', 'subobjects.tsv'), 'w') as o:
        for r in rows:
            o.write('\t'.join(r) + '\n')
    with open(os.path.join(ROOT, 'run', 'subobjects_check.cpp'), 'w') as o:
        o.write('#include "auto_common.h"\n#include <stddef.h>\n')
        for s, p, n in sorted(set(checks)):
            o.write('static_assert(__builtin_offsetof(%s, %s) == 0x%x, "");\n' % (s, p, n))
    print('%d rewrites, %d stay (run/subobjects.tsv)' % (total, sum(1 for r in rows if r[1] == 'stay')))


if __name__ == '__main__':
    main()
