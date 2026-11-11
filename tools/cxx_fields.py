#!/usr/bin/env python3
"""Pointer types and struct field access in place of integers and raw
offsets, correct by construction.

    cxx_fields.py fields FILE...      (1) only
    cxx_fields.py auto FILE...        (2) for the locals it can infer, then (1)
    cxx_fields.py retype ROWS.tsv     (2) for the given variables, then (1)

ROWS: KIND<TAB>FUNCTION<TAB>NAME<TAB>TYPE, KIND one of
    local   FUNCTION's local NAME
    param   FUNCTION's parameter NAME (prototype in protos.h and definition)
    global  NAME (FUNCTION -) in gamedata.h/.cpp and every function that does
            not declare its own NAME
    ret     FUNCTION's return value (NAME -)
    field   every use of the struct member NAME (FUNCTION -), declarations
            changed by hand (same size types only)
    member  NAME, a member of the global block variable FUNCTION (gamedata.h;
            same size types only: the layout is fixed)
TYPE: `S *` (S a struct of run/layout.tsv, which tools/cxx_layout.py reads
from the compiler) or `T *` for an array of integers T.

(1) `*(T *)((iptr)p + N)`, `*(T *)((u8 *)p + N)`, `*(T *)p` where p is
    declared `S *`: p->f, when S has a member f at offset N whose type is T
    (or an element p->a[i], or a member of a nested struct p->s.f).  An
    integer member of T's size and the other signedness is read as (T)p->f,
    and written as p->f only by = += -= *= &= |= ^= <<= ++ -- statements
    whose value is unused (the same bits are stored).
(2) An integer variable (or function result) X becomes `S *` / `T *`; each
    use of X is rewritten so that it means what it meant:
      *(T *)(X + N)            -> X->f as in (1), or *(T *)((I)X + N)
      *(T *)(X + E)            -> X[E / sizeof T] for a `T *` (E a sum of
                                  constants and multiples of sizeof T)
      (S *)X                   -> X
      X = E;                   -> X = (S *)E (an integer cast on E dropped)
      X in arithmetic, a relational or bitwise operation, ++ --, a compound
      assignment, a branch of ?:, a comma expression, or compared with
      anything but 0          -> (I)X, I its old integer type
    Everything else that the change affects does not compile any more (a
    pointer and an integer do not convert implicitly): tools/cxx_fix.py
    convcasts puts the old conversion there.
Each rewrite of (1) is listed in run/fields_check.cpp as static_asserts on
offsetof and the member's type and size; compile it to check them.
Log: run/fields.log.
"""
import collections
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from cxx_fix import unary_end, match_paren  # noqa: E402
from cxx_array import index as array_index  # noqa: E402
import cxx_retype  # noqa: E402

SIGNED = {'int': 4, 'i32': 4, 'iptr': 4, 'long': 4, 'LONG': 4, 'BOOL': 4, 'INT': 4,
          'short': 2, 'short int': 2, 'i16': 2, 'SHORT': 2, 'char': 1, 'i8': 1, 'CHAR': 1, 'signed char': 1}
UNSIGNED = {'unsigned int': 4, 'uint': 4, 'u32': 4, 'uptr': 4, 'DWORD': 4, 'UINT': 4, 'ulong': 4,
            'unsigned long': 4, 'undefined4': 4, 'ULONG': 4,
            'unsigned short': 2, 'short unsigned int': 2, 'ushort': 2, 'u16': 2, 'WORD': 2, 'word': 2, 'undefined2': 2,
            'unsigned char': 1, 'uchar': 1, 'u8': 1, 'byte': 1, 'BYTE': 1, 'undefined': 1, 'undefined1': 1,
            # Win32 handles: uint32_t in src/platform/win32_types.h
            **{h: 4 for h in ('HWND', 'HDC', 'HBRUSH', 'HPALETTE', 'HCURSOR', 'HICON', 'HMENU', 'HFONT',
                              'HBITMAP', 'HGDIOBJ', 'HANDLE', 'HGLOBAL', 'HINSTANCE', 'HPEN', 'HRGN', 'HMMIO')}}
INT_TYPES = r'(?:iptr|uptr|i32|u32|int|uint|long|ulong)'
NUM = r'(?:0x[0-9a-fA-F]+|\d+)'
TYPE = r'(?:const\s+)?[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*(?:\s*\*)*'
ANY_CAST = re.compile(r'\(\s*(?:(?:const\s+)?\w+(?:\s+\w+)*\s*\*+|%s)\s*\)\s*$'
                      % '|'.join(map(re.escape, sorted(list(SIGNED) + list(UNSIGNED), key=len, reverse=True))))
KEYWORDS = {'if', 'while', 'for', 'switch', 'return', 'sizeof', 'case', 'else', 'do'}

LAYOUT = None


def canon(t):
    t = re.sub(r'\s+', ' ', t.strip())
    t = re.sub(r'\s*\*', '*', t)
    if t in SIGNED:
        return ('s', SIGNED[t])
    if t in UNSIGNED:
        return ('u', UNSIGNED[t])
    return ('t', t)


def load_layout():
    lay = {}
    for line in open(os.path.join(ROOT, 'run', 'layout.tsv')):
        s, m, off, size, t = line.rstrip('\n').split('\t')
        if not m:
            lay[s] = {'size': int(size), 'members': []}
        else:
            lay[s]['members'].append((m, int(off), int(size), t))
    return lay


def unnamed(m):
    return re.match(r'(field|pad|unk|gap)_', m) is not None


def find_field(s, n, t, depth=0):
    """(path, member type, 'exact'|'sign') of S's member at offset n read as t."""
    want = canon(t)
    want_size = want[1] if want[0] in 'su' else None
    best = []
    for m, off, size, mt in LAYOUT[s]['members']:
        if not off <= n < off + size:
            continue
        am = re.match(r'(.*?) ?\[(\d+)\]$', mt)
        if am:
            et, k = am.group(1), int(am.group(2))
            es = size // k if k else 0
            if not es:
                continue
            idx = (n - off) // es
            ec = canon(et)
            path = '%s[%d]' % (m, idx)
            if (n - off) % es:
                st = re.sub(r'\s+', ' ', et.strip())
                if st in LAYOUT and depth < 4:
                    r = find_field(st, (n - off) % es, t, depth + 1)
                    if r:
                        best.append((('%s.%s' % (path, r[0]), r[1], r[2]), unnamed(m)))
                continue
            if ec == want:
                best.append(((path, et, 'exact'), unnamed(m)))
            elif ec[0] in 'su' and want[0] in 'su' and ec[1] == want_size:
                best.append(((path, et, 'sign'), unnamed(m)))
            elif re.sub(r'\s+', ' ', et.strip()) in LAYOUT and depth < 4:
                r = find_field(re.sub(r'\s+', ' ', et.strip()), (n - off) % es, t, depth + 1)
                if r:
                    best.append((('%s.%s' % (path, r[0]), r[1], r[2]), unnamed(m)))
            continue
        if off == n:
            mc = canon(mt)
            if mc == want:
                best.append(((m, mt, 'exact'), unnamed(m)))
                continue
            if mc[0] in 'su' and want[0] in 'su' and mc[1] == want_size:
                best.append(((m, mt, 'sign'), unnamed(m)))
                continue
            if mt.strip().endswith('*') and (want_size == 4 or want[0] == 't' and want[1].endswith('*')) \
                    or mc[0] in 'su' and mc[1] == 4 and want[0] == 't' and want[1].endswith('*'):
                # a pointer member read as a 4-byte integer or another pointer,
                # a 4-byte integer member read as a pointer
                best.append(((m, mt, 'ptr'), unnamed(m)))
                continue
        st = re.sub(r'\s+', ' ', mt.strip())
        if st in LAYOUT and depth < 4:
            r = find_field(st, n - off, t, depth + 1)
            if r:
                best.append((('%s.%s' % (m, r[0]), r[1], r[2]), unnamed(m)))
    if not best:
        return None
    best.sort(key=lambda b: (b[0][2] != 'exact', b[1], b[0][0].count('.')))
    return best[0][0]


def masked(body):
    """The text with strings, character constants and comments blanked."""
    out = list(body)
    i = 0
    while i < len(body):
        c = body[i]
        if body.startswith('/*', i):
            e = body.find('*/', i + 2)
            e = len(body) if e < 0 else e + 2
            for k in range(i, e):
                if out[k] != '\n':
                    out[k] = ' '
            i = e
        elif body.startswith('//', i):
            e = body.find('\n', i)
            e = len(body) if e < 0 else e
            for k in range(i, e):
                out[k] = ' '
            i = e
        elif c in '"\'':
            j = i + 1
            while j < len(body) and body[j] != c:
                j += 2 if body[j] == '\\' else 1
            for k in range(i + 1, min(j, len(body))):
                out[k] = ' '
            i = j + 1
        else:
            i += 1
    return ''.join(out)


OPS = ('<<=', '>>=', '->', '++', '--', '<<', '>>', '<=', '>=', '==', '!=', '&&', '||',
       '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=')


def prev_tok(s, i):
    j = i - 1
    while j >= 0 and s[j] in ' \t\n':
        j -= 1
    if j < 0:
        return '', j
    for op in OPS:
        if s[max(0, j - len(op) + 1):j + 1] == op:
            return op, j - len(op) + 1
    if s[j].isalnum() or s[j] == '_':
        k = j
        while k >= 0 and (s[k].isalnum() or s[k] == '_'):
            k -= 1
        return s[k + 1:j + 1], k + 1
    return s[j], j


def next_tok(s, i):
    j = i
    while j < len(s) and s[j] in ' \t\n':
        j += 1
    for op in OPS:
        if s.startswith(op, j):
            return op, j
    if j < len(s) and (s[j].isalnum() or s[j] == '_'):
        return re.match(r'\w+', s[j:]).group(0), j
    return (s[j] if j < len(s) else ''), j


STORE_OK = {'=', '+=', '-=', '*=', '&=', '|=', '^=', '<<='}
ARITH_NEXT = {'+', '-', '*', '/', '%', '<', '>', '<=', '>=', '<<', '>>', '&', '|', '^', '++', '--',
              '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=', '[', '?', ':'}
ARITH_PREV = {'+', '-', '*', '/', '%', '<', '>', '<=', '>=', '<<', '>>', '|', '^', '++', '--', '~',
              '?', ':'}


def is_statement_start(m, i):
    p, j = prev_tok(m, i)
    if p == ':':
        # a label or case (not a ?: branch)
        line = m[m.rfind('\n', 0, j) + 1:j]
        return re.match(r'\s*(?:case\b.*|default|\w+)\s*$', line) is not None
    if p == ')':
        # if (...) X, while (...) X, for (...) X; not a cast (T *)X
        d, k = 0, j
        while k >= 0:
            d += {')': 1, '(': -1}.get(m[k], 0)
            if d == 0:
                break
            k -= 1
        return k >= 0 and prev_tok(m, k)[0] in ('if', 'while', 'for', 'switch')
    return p in (';', '{', '}', 'else', '')


def statement_end(m, i):
    """Index of the ; ending the expression statement from i (-1 if a , or
    unbalanced ) comes first)."""
    depth = 0
    k = i
    while k < len(m):
        if m[k] in '([':
            depth += 1
        elif m[k] in ')]':
            if depth == 0:
                return -1
            depth -= 1
        elif m[k] == ';' and depth == 0:
            return k
        elif m[k] == ',' and depth == 0:
            return -1
        k += 1
    return -1


def cast_type(t):
    return re.sub(r'\s*\*', ' *', re.sub(r'\s+', ' ', t.strip())).replace('* *', '**')


def binary_and(m, j):
    """The '&' at j is the binary operator: an operand ends before it."""
    p, k = prev_tok(m, j)
    if p == ')':
        d, i = 0, k
        while i >= 0:
            d += {')': 1, '(': -1}.get(m[i], 0)
            if d == 0:
                break
            i -= 1
        q, _ = prev_tok(m, i)
        if q in (')', ']') or (re.match(r'^\w+$', q) and q not in KEYWORDS):
            return True
        if ANY_CAST.search(m[i:k + 1]) or re.match(r'\(\s*(?:const\s+)?\w+(?:\s+\w+)*\s*\)$', m[i:k + 1]):
            return False
        return True
    return p == ']' or (bool(re.match(r'^\w+$', p)) and p not in KEYWORDS)


PENDING = []      # insertions a pointer store asks for (field_access)


def field_access(m, start, end, obj, s, n, t, checks):
    """Text for the raw access m[start:end] = *(t *)(obj + n), obj an S * (or None)."""
    f = find_field(s, n, t)
    if not f:
        return None
    path, mt, kind = f
    nt, nj = next_tok(m, end)
    pt, pj = prev_tok(m, start)
    if pt == '&' and binary_and(m, pj):
        pt = '&&'
    if pt in ('&', 'sizeof'):
        return None
    expr = '%s->%s' % (obj, path)
    if kind == 'exact':
        checks.append((s, path, n, t))
        return expr
    if kind == 'ptr' and pt != '*' and (nt in STORE_OK or nt in ('++', '--') or pt in ('++', '--')):
        # p->f = (MT)(E);  (the same 4 bytes are stored)
        if nt != '=' or not is_statement_start(m, start):
            return None
        e = statement_end(m, nj + 1)
        if e < 0:
            return None
        checks.append((s, path, n, mt))
        rs = nj + 1
        while m[rs] in ' \n\t':
            rs += 1
        if m[rs:e].strip() not in ('0', '0x0'):
            PENDING.append((rs, rs, '(%s)(' % cast_type(mt)))
            PENDING.append((e, e, ')'))
        return expr
    if kind == 'ptr':
        checks.append((s, path, n, mt))
        return '(%s)%s' % (cast_type(t), expr)
    store = pt != '*' and (nt in STORE_OK or nt in ('++', '--') or pt in ('++', '--'))
    if store:
        if pt in ('++', '--'):
            if not is_statement_start(m, pj) or statement_end(m, end) < 0:
                return None
        elif not is_statement_start(m, start) or statement_end(m, end) < 0:
            return None
        if nt in ('/=', '%=', '>>='):
            return None
        checks.append((s, path, n, t))
        return expr
    checks.append((s, path, n, t))
    return '(%s)%s' % (cast_type(t), expr)


def functions(text):
    """(header start, body start, body end) of each function definition."""
    for fm in re.finditer(r'^\{\n', text, re.M):
        e = fm.start()
        while e > 0 and text[e - 1] == '\n':   # the blank line between header and body
            e -= 1
        hs = text.rfind('\n\n', 0, e)
        hs = 0 if hs < 0 else hs + 2
        be = re.compile(r'^\}', re.M).search(text, fm.end())
        if be:
            yield hs, fm.end(), be.start()


def func_name(header):
    m = re.search(r'(\w+)\s*\([^()]*(?:\([^()]*\)[^()]*)*\)\s*$', header.strip())
    return m.group(1) if m else None


def decls(header, body):
    """{name: (type, kind, start, end)} of the parameters and top-level locals."""
    out = {}
    if header:
        hm = header.find('(', header.find(func_name(header) or '(') if func_name(header) else 0)
        params = header[hm + 1:header.rfind(')')]
        for p in cxx_retype.split_params(params):
            pm = re.match(r'\s*(%s?)\s*\b(\w+)\s*$' % TYPE, p)
            if pm and pm.group(1).strip():
                out[pm.group(2)] = (pm.group(1).strip(), 'param', None, None)
    for dm in re.finditer(r'^  (%s?)\s*\b(\w+)(?:\s*\[[^\]]*\])?;\n' % TYPE, body, re.M):
        if dm.group(1).strip() in ('return', 'goto', 'break', 'continue'):
            continue
        out.setdefault(dm.group(2), (dm.group(1).strip(), 'local', dm.start(), dm.end()))
    return out


def struct_of(t):
    m = re.match(r'(?:const\s+)?(\w+)\s*\*$', t.strip())
    if m and m.group(1) in LAYOUT:
        return m.group(1)
    return None


def elem_of(t):
    m = re.match(r'(\w+)\s*\*$', t.strip())
    if m and canon(m.group(1))[0] in 'su':
        return m.group(1)
    return None


def apply(text, edits):
    edits = sorted(edits, key=lambda e: (e[0], e[1]))
    for x, y in zip(edits, edits[1:]):
        if y[0] < x[1]:
            raise ValueError('overlapping edits %r %r' % (x, y))
    for a, b, r in reversed(edits):
        text = text[:a] + r + text[b:]
    return text


def group_extent(m, a, b):
    """Grow [a, b) over grouping parentheses ( X ) around it."""
    while True:
        pt, pj = prev_tok(m, a)
        nt, nj = next_tok(m, b)
        if pt != '(' or nt != ')':
            return a, b
        p2, _ = prev_tok(m, pj)
        if re.match(r'\w', p2 or '') and p2 not in KEYWORDS or p2 in (')', ']'):
            return a, b             # a call's parentheses
        if re.match(r'\w', p2 or '') and p2 in ('if', 'while', 'switch', 'for'):
            return a, b
        a, b = pj, nj + 1


def raw_around(m, a, b):
    """If m[a:b] is the base of a raw access *(T *)(X + E) or *(T *)X: (start,
    end, T, E or None)."""
    pt, pj = prev_tok(m, a)
    if pt == '(':
        cm = re.search(r'\*\s*\(\s*(%s)\s*\*\s*\)\s*\(\s*$' % TYPE, m[:a])
        if not cm:
            return None
        o = pj
        e = match_paren(m, o)
        if e < 0:
            return None
        nt, nj = next_tok(m, b)
        if nt != '+':
            return None
        expr = m[nj + 1:e - 1].strip()
        if not expr:
            return None
        return cm.start(), e, cm.group(1), expr
    if pt == ')':
        cm = re.search(r'\*\s*\(\s*(%s)\s*\*\s*\)\s*$' % TYPE, m[:a])
        if cm:
            nt, _ = next_tok(m, b)
            if nt in ('->', '.', '[', '('):
                return None
            return cm.start(), b, cm.group(1), None
    return None


def rewrite_uses(text, m, spans, newtype, oldtype, checks, stats, assign=True, report=None):
    """Edits that keep the meaning of each use m[a:b] of an expression whose
    type changes from the integer oldtype to newtype (report counts the uses
    that stay integers: 'wrap')."""
    report = {} if report is None else report
    s = struct_of(newtype)
    el = elem_of(newtype)
    old = 'uptr' if canon(oldtype)[0] == 'u' else 'iptr'
    # an integer pointer T * (X + N is N elements, X[N], *X)
    pel = elem_of(oldtype) or (oldtype.strip()[:-1].strip() if re.fullmatch(r'(?:char|byte|undefined\d?)\s*\*', oldtype.strip()) else None)
    psize = canon(pel)[1] if pel else 1
    if pel:
        old = cast_type(oldtype)
        if not s:
            raise ValueError('%s -> %s: only to a struct pointer' % (oldtype, newtype))
    null_old = r'\(\s*%s\s*\*\s*\)\s*(?:0x0|0)\b(?!x)' % re.escape(pel) if pel else None
    edits = []
    for a, b in spans:
        x = text[a:b]
        r = raw_around(m, a, b)
        if r and pel:
            ra, rb, t, e = r
            new = None
            if e is None:
                new = field_access(m, ra, rb, x, s, 0, t, checks)
            elif re.fullmatch(NUM, e) and int(e, 0) * psize < LAYOUT[s]['size']:
                new = field_access(m, ra, rb, x, s, int(e, 0) * psize, t, checks)
            if new is not None:
                stats['named'] += 1
                edits.append((ra, rb, new))
                edits.extend(PENDING)
                del PENDING[:]
            else:
                del PENDING[:]
                stats['raw_left'] += 1
                report['wrap'] = report.get('wrap', 0) + 1
                edits.append((a, a, '(%s)' % old))
            continue
        if pel:
            nt, nj = next_tok(m, b)
            pt, pj = prev_tok(m, a)
            new = None
            if nt == '[':
                ce = m.index(']', nj)
                ie = m[nj + 1:ce].strip()
                if re.fullmatch(NUM, ie) and '[' not in ie and int(ie, 0) * psize < LAYOUT[s]['size']:
                    new = field_access(m, a, ce + 1, x, s, int(ie, 0) * psize, pel, checks)
                    if new is not None:
                        edits.append((a, ce + 1, new))
                elif ie == '0':
                    pass
            elif pt == '*' and (prev_tok(m, pj)[0] not in (')', ']') and not re.match(r'\w', prev_tok(m, pj)[0] or '(')
                                or prev_tok(m, pj)[0] in KEYWORDS
                                or prev_tok(m, pj)[0] == ')' and ANY_CAST.search(m[:prev_tok(m, pj)[1] + 1])):
                new = field_access(m, pj, b, x, s, 0, pel, checks)
                if new is not None:
                    edits.append((pj, b, new))
            if new is not None:
                stats['named'] += 1
                edits.extend(PENDING)
                del PENDING[:]
                continue
            del PENDING[:]
            if nt in ('==', '!=') and re.match(r'\s*(?:==|!=)\s*' + null_old, m[b:]):
                cm = re.compile(null_old).search(m, nj)
                edits.append((cm.start(), cm.end(), '(%s)0x0' % cast_type(newtype)))
                continue
            if pt in ('==', '!='):
                cm = re.search(null_old + r'\s*(?:==|!=)\s*$', m[:a])
                if cm:
                    edits.append((cm.start(), cm.start() + re.match(null_old, m[cm.start():]).end(), '(%s)0x0' % cast_type(newtype)))
                    continue
            if assign and nt == '=' and assign_end(m, a, nj) is not None:
                e = assign_end(m, a, nj)
                if e > 0:
                    rs = nj + 1
                    while m[rs] in ' \n':
                        rs += 1
                    rhs = m[rs:e].rstrip()
                    cm = re.match(r'\(\s*%s\s*\*\s*\)' % re.escape(pel), rhs)
                    if cm and unary_end(rhs, cm.end()) == len(rhs):
                        edits.append((rs, rs + cm.end(), '(%s)' % cast_type(newtype)))
                        continue
        if r:
            ra, rb, t, e = r
            new = None
            if s and e is None:
                new = field_access(m, ra, rb, x, s, 0, t, checks)
            elif s and re.fullmatch(NUM, e):
                n = int(e, 0)
                if n < LAYOUT[s]['size']:
                    new = field_access(m, ra, rb, x, s, n, t, checks)
            elif el and canon(t) == canon(el):
                idx = '0' if e is None else array_index(text[text.index('+', b, rb) + 1:rb - 1].strip(), canon(el)[1])
                if idx is not None and prev_tok(m, ra)[0] not in ('&', 'sizeof'):
                    new = '%s[%s]' % (x, idx)
            if new is not None:
                stats['named'] += 1
                edits.append((ra, rb, new))
                edits.extend(PENDING)
                del PENDING[:]
                continue
            del PENDING[:]
            stats['raw_left'] += 1
            report['wrap'] = report.get('wrap', 0) + 1
            edits.append((a, a, '(%s)' % old))
            continue
        pt, pj = prev_tok(m, a)
        if pt == ')':
            cm = re.search(r'\(\s*(?:const\s+)?(\w+)\s*\*\s*\)\s*$', m[:a])
            if cm and '%s *' % cm.group(1) == cast_type(newtype):
                # ((S *)X) -> X
                pa, pb = prev_tok(m, cm.start()), next_tok(m, b)
                if pa[0] == '(' and pb[0] == ')' and not re.match(r'\w|[)\]]', prev_tok(m, pa[1])[0] or '(') \
                        or pa[0] == '(' and pb[0] == ')' and prev_tok(m, pa[1])[0] in KEYWORDS - {'if', 'while', 'switch', 'for'}:
                    edits.append((pa[1], cm.start(), ''))
                    edits.append((cm.start(), a, ''))
                    edits.append((pb[1], pb[1] + 1, ''))
                else:
                    edits.append((cm.start(), a, ''))
                continue
            if ANY_CAST.search(m[:a]):
                continue
        nt, nj = next_tok(m, b)
        if assign and nt == '=' and assign_end(m, a, nj) is not None:
            e = assign_end(m, a, nj)
            if e < 0:
                edits.append((a, a, '(%s)' % old))       # does not compile: for hand
                report['wrap'] = report.get('wrap', 0) + 1
                continue
            rs = nj + 1
            while m[rs] in ' \n':
                rs += 1
            rhs = m[rs:e].rstrip()
            ct = cast_type(newtype)
            im = re.match(r'\(\s*%s\s*\)\s*' % INT_TYPES, rhs)
            if rhs in ('0', '0x0', 'nullptr'):
                pass
            elif re.match(r'\(\s*%s\s*\)' % re.escape(ct).replace(r'\ ', r'\s*'), rhs) and unary_end(rhs, 0) == len(rhs):
                pass
            elif im and unary_end(rhs, im.end()) == len(rhs):
                edits.append((rs, rs + im.end(), '(%s)' % ct))
            elif unary_end(rhs, 0) == len(rhs):
                edits.append((rs, rs, '(%s)' % ct))
            else:
                # a call written over lines, maybe under a cast, is a value too
                cm = re.match(r'(?:\(\s*%s\s*\*?\s*\)\s*)?\w+\s*\(' % TYPE, rhs)
                if not (cm and match_paren(rhs, cm.end() - 1) == len(rhs)):
                    report['wrap'] = report.get('wrap', 0) + 1
                edits.append((rs, rs, '(%s)(' % ct))
                edits.append((rs + len(rhs), rs + len(rhs), ')'))
            continue
        ga, gb = group_extent(m, a, b)
        pt, pj = prev_tok(m, ga)
        nt, nj = next_tok(m, gb)
        wrap = cmp = False
        if nt in ARITH_NEXT or pt in ARITH_PREV or pt == '*':
            wrap = True
        elif pt == '&':
            p2, _ = prev_tok(m, pj)
            wrap = bool(re.match(r'\w|[)\]]', p2 or '(')) and p2 not in KEYWORDS
        elif pt == ',' and nt not in ('==', '!=') or nt == ',' and pt not in ('==', '!='):
            # a comma expression (not a call's arguments)
            o = enclosing_open(m, ga)
            if o >= 0:
                p2, _ = prev_tok(m, o)
                wrap = not ((re.match(r'\w', p2 or '') and p2 not in KEYWORDS) or p2 in (')', ']', 'for'))
            else:
                wrap = True
        elif nt in ('==', '!='):
            wrap = not re.match(r'\s*(?:==|!=)\s*(?:\(\s*(?:const\s+)?\w+\s*\*\s*\)\s*)?(?:0x0|0|nullptr)\b(?!x)', m[gb:])
            cmp = wrap
        elif pt in ('==', '!='):
            wrap = not re.search(r'\b(?:0x0|0|nullptr)\s*(?:==|!=)\s*$', m[:ga])
            cmp = wrap
        if wrap and cmp:
            # a comparison keeps its meaning with the old type on one side
            edits.append((a, a, '(%s)' % old))
            stats['wrapped'] += 1
            report['cmp'] = report.get('cmp', 0) + 1
        elif wrap:
            edits.append((a, a, '(%s)' % old))
            stats['wrapped'] += 1
            report['wrap'] = report.get('wrap', 0) + 1
    if pel:
        # ((T *)X): a cast binds less tightly than X[i], X++
        ends = dict(spans)
        out = []
        for ea, eb, er in edits:
            if ea == eb and er == '(%s)' % old and ea in ends:
                out.append((ea, ea, '((%s)' % old))
                out.append((ends[ea], ends[ea], ')'))
            else:
                out.append((ea, eb, er))
        edits = out
    return edits


def enclosing_open(m, i):
    depth = 0
    j = i - 1
    while j >= 0:
        if m[j] in ')]':
            depth += 1
        elif m[j] in '([':
            if depth == 0:
                return j if m[j] == '(' else -1
            depth -= 1
        elif m[j] in ';{}' and depth == 0:
            return -1
        j -= 1
    return -1


def var_spans(m, v, skip=None):
    out = []
    for x in re.finditer(r'(?<![\w$])%s\b' % re.escape(v), m):
        if prev_tok(m, x.start())[0] in ('.', '->', 'goto') or x.start() == skip:
            continue
        if next_tok(m, x.end())[0] == ':' and is_statement_start(m, x.start()):
            continue                # a label
        out.append((x.start(), x.end()))
    return out


def call_spans(m, f):
    out = []
    for x in re.finditer(r'(?<![\w$])%s\s*\(' % re.escape(f), m):
        if prev_tok(m, x.start())[0] in ('.', '->'):
            continue
        e = match_paren(m, x.end() - 1)
        if e > 0:
            out.append((x.start(), e))
    return out


def typed_pass(body, ds, checks, stats):
    """(1): raw accesses through pointers already typed S *."""
    m = masked(body)
    edits = []
    for v, (t, _, _, _) in ds.items():
        s = struct_of(t)
        if not s:
            continue
        pat = re.compile(r'\*\(\s*(%s)\s*\*\s*\)\s*(?:\(\s*\(\s*(?:%s|u8\s*\*|byte\s*\*|char\s*\*|i8\s*\*)\s*\)\s*%s\s*\+\s*(%s)\s*\)|%s\b)'
                         % (TYPE, INT_TYPES, re.escape(v), NUM, re.escape(v)))
        for x in pat.finditer(m):
            if prev_tok(m, x.start())[0] in ('.', '->'):
                continue
            if not x.group(2) and next_tok(m, x.end())[0] in ('->', '.', '[', '('):
                continue
            n = int(x.group(2), 0) if x.group(2) else 0
            if n >= LAYOUT[s]['size']:
                continue
            r = field_access(m, x.start(), x.end(), v, s, n, x.group(1), checks)
            stats['typed' if r else 'typed_left'] += 1
            if r:
                edits.append((x.start(), x.end(), r))
                edits.extend(PENDING)
            del PENDING[:]
    return apply(body, edits)


def infer(body, m, v, ds):
    """The struct an integer local holds: the only struct its value is cast
    to, or that the values assigned to it have."""
    found = set()
    for a, b in var_spans(m, v):
        pt, _ = prev_tok(m, a)
        if pt == ')':
            cm = re.search(r'\(\s*(?:const\s+)?(\w+)\s*\*\s*\)\s*$', m[:a])
            if cm and cm.group(1) in LAYOUT and next_tok(m, b)[0] not in ('+', '-'):
                found.add(cm.group(1))
        nt, nj = next_tok(m, b)
        if nt == '=' and is_statement_start(m, a):
            e = statement_end(m, nj + 1)
            rhs = m[nj + 1:e].strip() if e > 0 else ''
            rm = re.fullmatch(r'(?:\(\s*%s\s*\)\s*)?\(?\s*(\w+)\s*\)?' % INT_TYPES, rhs)
            if rm and rm.group(1) in ds and struct_of(ds[rm.group(1)][0]):
                found.add(struct_of(ds[rm.group(1)][0]))
            rm = re.fullmatch(r'(?:\(\s*%s\s*\)\s*)?\(\s*(\w+)\s*\*\s*\).*' % INT_TYPES, rhs, re.S)
            if rm and rm.group(1) in LAYOUT and unary_end(rhs, 0) == len(rhs):
                found.add(rm.group(1))
            cm = re.match(r'(?:\(\s*%s\s*\)\s*)?(\w+)\s*\(' % INT_TYPES, rhs)
            if cm and cm.group(1) in RETURNS and unary_end(rhs, 0) == len(rhs):
                st = struct_of(RETURNS[cm.group(1)])
                if st:
                    found.add(st)
    return found.pop() if len(found) == 1 else None


RETURNS = {}


def load_returns():
    for line in open(os.path.join(ROOT, 'src', 'auto', 'protos.h')):
        m = re.match(r'(?:EXTERN_C\s+)?(%s?)\s*(?:__\w+\s+)?(\w+)\(' % TYPE, line)
        if m:
            RETURNS[m.group(2)] = m.group(1).strip()


def raw_offsets(m, v):
    return [int(x.group(1), 0) for x in
            re.finditer(r'\*\(\s*%s\s*\*\s*\)\s*\(\s*%s\s*\+\s*(%s)\s*\)' % (TYPE, re.escape(v), NUM), m)]


def retype_local(body, v, t, newtype, ds_, de, checks, stats, strict=False):
    m = masked(body)
    spans = var_spans(m, v, skip=m.index(v, ds_ + 2))
    report = {}
    local_checks, local_stats = [], collections.Counter()
    edits = rewrite_uses(body, m, spans, newtype, t, local_checks, local_stats, report=report)
    if strict and report.get('wrap'):
        return None
    checks.extend(local_checks)
    stats.update(local_stats)
    edits.append((ds_, de, '  %s%s;\n' % (cast_type(newtype) if newtype.endswith('*') else newtype + ' ', v)))
    return apply(body, edits)


def assigned(m, v):
    """The right-hand sides of the statements v = E; (and of v = E in a for
    header's last clause)"""
    out = []
    for a, b in var_spans(m, v):
        nt, nj = next_tok(m, b)
        if nt == '=' and prev_tok(m, a)[0] != '*':
            e = assign_end(m, a, nj)
            out.append(m[nj + 1:e].strip() if e is not None and e > 0 else '?')
    return out


def assign_end(m, a, nj):
    """End of E in the assignment m[a:] = `X = E` (nj: the =) when it is a
    statement, a for header's first or last clause; else None."""
    if for_step(m, a):
        e = expr_end(m, nj + 1)
        return e if e > 0 and m[e] == ')' else -1
    pt, pj = prev_tok(m, a)
    if is_statement_start(m, a) or pt == '(' and prev_tok(m, pj)[0] == 'for':
        return statement_end(m, nj + 1)
    return None


def for_step(m, a):
    """Is m[a:] the start of the last clause of a for header (`; X = E)`)?"""
    return prev_tok(m, a)[0] == ';' and _for_open(m, a)


def _for_open(m, a):
    depth = 0
    j = a - 1
    while j >= 0:
        if m[j] == ')':
            depth += 1
        elif m[j] == '(':
            if depth == 0:
                return prev_tok(m, j)[0] == 'for'
            depth -= 1
        elif m[j] in '{}' and depth == 0:
            return False
        j -= 1
    return False


def expr_end(m, i):
    """Index of the ; , or unbalanced ) ending the expression from i."""
    depth = 0
    k = i
    while k < len(m):
        if m[k] in '([':
            depth += 1
        elif m[k] in ')]':
            if depth == 0:
                return k
            depth -= 1
        elif m[k] in ';,' and depth == 0:
            return k
        k += 1
    return -1


# functions returning a new block as an integer: what they return is what the
# caller casts it to
ALLOCATORS = {'memblock_alloc', 'mem_handle_lock', 'operator_new', '_malloc', 'mem_alloc'}


def member_types(name):
    return {re.sub(r'\s+', '', mt) for st in LAYOUT.values() for mn, _, _, mt in st['members'] if mn == name}


def pointer_source(rhs, s, ds, v=None):
    """Is rhs (maybe under an integer cast) 0, a (S *) cast, an S * variable,
    a call of a function that returns S * or void *, a member X->m of type
    S *, or (v being retyped) a raw read of v's S * member?"""
    rhs = re.sub(r'(\w)\s+\(', r'\1(', re.sub(r'\s+', ' ', rhs.strip()))
    if rhs in ('0', '0x0', 'nullptr'):
        return True
    im = re.match(r'\(\s*(?:%s|(?:%s)\s*\*)\s*\)\s*' % (INT_TYPES, '|'.join(map(re.escape, list(SIGNED) + list(UNSIGNED)))), rhs)
    if im and unary_end(rhs, im.end()) == len(rhs):
        rhs = rhs[im.end():]
    if rhs in ('0', '0x0', 'nullptr'):
        return True
    if re.fullmatch(r'\(\s*(?:0x0|0)\s*\)|\(\s*\w+\s*\*\s*\)\s*(?:0x0|0)', rhs):
        return True
    if unary_end(rhs, 0) != len(rhs):
        return False
    mm = re.search(r'(?:->|\.)\s*(\w+)$', rhs)
    if mm and re.fullmatch(r'[\w\s.>()\[\]*-]+', rhs) and member_types(mm.group(1)) == {s + '*'}:
        return True
    if v:
        rm = re.fullmatch(r'\*\(\s*%s\s*\*\s*\)\s*\(\s*%s\s*\+\s*(%s)\s*\)' % (TYPE, re.escape(v), NUM), rhs)
        el = elem_of(ds[v][0]) if v in ds else None
        n = None
        if rm and not el:
            n = int(rm.group(1), 0)
        im2 = re.fullmatch(r'%s\s*\[\s*(%s)\s*\]' % (re.escape(v), NUM), rhs)
        if im2 and el:
            n = int(im2.group(1), 0) * canon(el)[1]
        if n is not None and any(mo == n and re.sub(r'\s+', '', mt) == s + '*' for _, mo, _, mt in LAYOUT[s]['members']):
            return True
    cm = re.match(r'\(\s*(\w+)\s*\*\s*\)', rhs)
    if cm:
        return cm.group(1) == s
    if rhs in ds:
        return struct_of(ds[rhs][0]) == s
    cm = re.match(r'(\w+)\s*\(', rhs)
    if cm and match_paren(rhs, cm.end() - 1) == len(rhs):
        r = RETURNS.get(cm.group(1), '')
        return struct_of(r) == s or re.sub(r'\s', '', r) in ('void*', 'LPVOID') or cm.group(1) in ALLOCATORS
    return False


def auto_pass(header, body, checks, stats, log, where):
    ds = decls(header, body)
    for v, (t, kind, ds_, de) in list(ds.items()):
        if kind != 'local' or not re.fullmatch(INT_TYPES, t):
            continue
        m = masked(body)
        s = infer(body, m, v, ds)
        if not s:
            continue
        offs = raw_offsets(m, v)
        if any(o >= LAYOUT[s]['size'] for o in offs):
            log.append('%s: %s: offsets beyond %s' % (where, v, s))
            continue
        fits = sum(1 for o in offs if any(mo == o for _, mo, _, _ in LAYOUT[s]['members']))
        if offs and fits * 2 < len(offs):
            log.append('%s: %s: %d of %d offsets fit no member of %s' % (where, v, len(offs) - fits, len(offs), s))
            continue
        bad = [r for r in assigned(m, v) if not pointer_source(r, s, ds, v)]
        if bad:
            log.append('%s: %s: also assigned %s (%s)' % (where, v, bad[0][:60], s))
            continue
        ds = decls(header, body)
        new = retype_local(body, v, t, '%s *' % s, ds[v][2], ds[v][3], checks, stats, strict=True)
        if new is None:
            log.append('%s: %s: also holds numbers (%s)' % (where, v, s))
            continue
        body = new
        stats['retyped'] += 1
        log.append('%s: %s -> %s *' % (where, v, s))
    return body


def sources():
    return sorted(glob.glob(os.path.join(ROOT, 'src', 'auto', '*.cpp')) + glob.glob(os.path.join(ROOT, 'src', 'game', '*.cpp')))


def each_function(path, fn):
    text = open(path).read()
    out, pos = [], 0
    for hs, bs, be in functions(text):
        header = text[hs:bs - 2]
        body = fn(header, text[bs:be])
        out.append(text[pos:bs] + body)
        pos = be
    out.append(text[pos:])
    new = ''.join(out)
    if new != text:
        open(path, 'w').write(new)


def retype_rows(rows, checks, stats, log):
    files = sources()
    for kind, func, name, newtype in rows:
        if kind in ('local', 'param'):
            for f in files:
                text = open(f).read()
                if not re.search(r'(?m)^(?:\w.*\b)?%s\s*\(' % re.escape(func), text):
                    continue

                def one(header, body, kind=kind, func=func, name=name, newtype=newtype):
                    if func_name(header) != func:
                        return body
                    ds = decls(header, body)
                    if name not in ds or ds[name][1] != kind:
                        log.append('%s: no %s %s' % (func, kind, name))
                        return body
                    t = ds[name][0]
                    if kind == 'local':
                        return retype_local(body, name, t, newtype, ds[name][2], ds[name][3], checks, stats)
                    m = masked(body)
                    return apply(body, rewrite_uses(body, m, var_spans(m, name), newtype, t, checks, stats))
                each_function(f, one)
            if kind == 'param':
                retype_param_decl(func, name, newtype, log)
        elif kind == 'global':
            for f in files:
                def one(header, body, name=name, newtype=newtype):
                    ds = decls(header, body)
                    if name in ds:
                        return body
                    m = masked(body)
                    return apply(body, rewrite_uses(body, m, var_spans(m, name), newtype, GLOBAL_TYPES[name], checks, stats))
                each_function(f, one)
            retype_global_decl(name, newtype, log)
        elif kind == 'member':
            # a member of a global block struct (gamedata.h): FUNCTION is the
            # block variable, NAME the member
            oldtype = retype_member_decl(func, name, newtype, log)
            if oldtype is None:
                continue
            for f in files:
                def one(header, body, func=func, name=name, newtype=newtype, oldtype=oldtype):
                    m = masked(body)
                    return apply(body, rewrite_uses(body, m, member_spans(m, func, name), newtype, oldtype, checks, stats))
                each_function(f, one)
        elif kind == 'field':
            # every use X->NAME / X.NAME of a struct member NAME (FUNCTION -):
            # the member's declarations are changed by hand
            oldtypes = {mt for st in LAYOUT.values() for mn, _, _, mt in st['members'] if mn == name}
            if not oldtypes or any(canon(o)[0] not in 'su' for o in oldtypes):
                log.append('field %s: not an integer member (%s)' % (name, oldtypes))
                continue
            oldtype = sorted(oldtypes)[0]
            for f in files:
                def one(header, body, name=name, newtype=newtype, oldtype=oldtype, f=f):
                    m = masked(body)
                    spans, odd = member_use_spans(m, name)
                    for o in odd:
                        log.append('%s: %s: use of %s not rewritten: %s' % (os.path.relpath(f, ROOT), func_name(header), name, m[max(0, o - 40):o + 20].replace('\n', ' ')))
                    return apply(body, rewrite_uses(body, m, spans, newtype, oldtype, checks, stats))
                each_function(f, one)
        elif kind == 'ret':
            oldtype = RETURNS.get(func, 'iptr')
            for f in files:
                def one(header, body, func=func, newtype=newtype, oldtype=oldtype):
                    m = masked(body)
                    return apply(body, rewrite_uses(body, m, call_spans(m, func), newtype, oldtype, checks, stats, assign=False))
                each_function(f, one)
            r = os.path.join(ROOT, 'run', 'fields_ret.tsv')
            open(r, 'w').write('%s\tret\t%s\n' % (func, newtype))
            sys.argv = ['', r]
            cxx_retype.main()
        stats[kind] += 1


GLOBAL_TYPES = {}


def global_headers():
    return ['src/data/gamedata.h'] + sorted(os.path.relpath(p, ROOT) for p in
                                            glob.glob(os.path.join(ROOT, 'src', 'data', 'state', '*.h')))


def load_globals():
    for h in global_headers():
        for line in open(os.path.join(ROOT, h)):
            m = re.match(r'extern\s+(%s?)\s*\b(\w+)\s*;' % TYPE, line)
            if m:
                GLOBAL_TYPES[m.group(2)] = m.group(1).strip()


def retype_global_decl(name, newtype, log):
    nt = cast_type(newtype)
    decl = r'(?m)^(extern\s+)(%s?)\s*\b(%s\s*;)'
    found = False
    for p, pat in [(h, decl) for h in global_headers()] + \
            [('src/data/gamedata.cpp', r'(?m)^((?:GAMEDATA\(\d+\)\s+)?)(%s?)\s*\b(%s\s*=)')]:
        path = os.path.join(ROOT, p)
        text = open(path).read()
        new, n = re.subn(pat % (TYPE, re.escape(name)), lambda x: x.group(1) + nt + ('' if nt.endswith('*') else ' ') + x.group(3), text, count=1)
        if n:
            open(path, 'w').write(new)
            found = found or p.endswith('.h')
        elif p.endswith('.cpp'):
            log.append('%s: definition of %s not found' % (p, name))
    if not found:
        log.append('declaration of %s not found' % name)


def member_spans(m, block, member):
    out = []
    for x in re.finditer(r'(?<![\w$])%s\s*\.\s*%s\b' % (re.escape(block), re.escape(member)), m):
        if prev_tok(m, x.start())[0] in ('.', '->'):
            continue
        out.append((x.start(), x.end()))
    return out


def member_use_spans(m, name):
    """Spans of the expressions X->name / X.name, X a chain of identifiers,
    ->, ., [..] and (..) groups; and the positions of the uses whose start is
    unclear."""
    out, odd = [], []
    for x in re.finditer(r'(?:->|\.)\s*%s\b' % re.escape(name), m):
        if next_tok(m, x.end())[0] in ('(',):
            continue
        a = x.start()
        while True:
            j = a
            while j > 0 and m[j - 1] in ' \n\t':
                j -= 1
            if j > 0 and (m[j - 1] == ']' or m[j - 1] == ')'):
                o = j - 1
                d = 0
                while o >= 0:
                    d += {')': 1, ']': 1, '(': -1, '[': -1}.get(m[o], 0)
                    if d == 0:
                        break
                    o -= 1
                if o < 0:
                    break
                if m[j - 1] == ')':
                    # a group ( ... ) is the start; a call f(...) is not handled
                    k = o
                    while k > 0 and m[k - 1] in ' \n\t':
                        k -= 1
                    if k > 0 and re.match(r'[\w\])]', m[k - 1]):
                        a = -1
                        break
                    a = o
                    break
                a = o
                continue
            w = re.search(r'[A-Za-z_]\w*$', m[:j])
            if not w:
                a = -1
                break
            a = w.start()
            k = a
            while k > 0 and m[k - 1] in ' \n\t':
                k -= 1
            if k >= 2 and m[k - 2:k] == '->' or k >= 1 and m[k - 1] == '.' :
                a = k - (2 if m[k - 2:k] == '->' else 1)
                continue
            break
        if a < 0:
            odd.append(x.start())
            continue
        out.append((a, x.end()))
    return out, odd


def retype_member_decl(block, member, newtype, log):
    """Retype member of the global block variable (gamedata.h: `extern [struct]
    S block;`, `S` declared there); returns the old type."""
    path = os.path.join(ROOT, 'src', 'data', 'gamedata.h')
    text = open(path).read()
    em = re.search(r'(?m)^extern\s+(?:struct\s+)?(\w+)\s+%s\s*;' % re.escape(block), text)
    if not em:
        log.append('member: no block %s' % block)
        return None
    sm = re.search(r'(?m)^(?:typedef\s+)?struct\s+%s\s*\{' % re.escape(em.group(1)), text)
    if not sm:
        log.append('member: no struct %s' % em.group(1))
        return None
    end = text.index('\n}', sm.end())
    dm = re.compile(r'(?m)^(\s*)(%s?)\s*\b(%s\s*;)' % (TYPE, re.escape(member))).search(text, sm.end(), end)
    if not dm:
        log.append('member: %s not in %s' % (member, em.group(1)))
        return None
    oldtype = dm.group(2).strip()
    nt = cast_type(newtype)
    text = text[:dm.start()] + dm.group(1) + nt + ('' if nt.endswith('*') else ' ') + dm.group(3) + text[dm.end():]
    open(path, 'w').write(text)
    return oldtype


def retype_param_decl(func, name, newtype, log):
    protos = open(cxx_retype.PROTOS).read()
    m = re.search(r'(?m)^.*?\b%s\(' % re.escape(func), protos)
    if not m:
        log.append('no prototype: %s' % func)
        return
    i = m.end() - 1
    params = cxx_retype.split_params(protos[i + 1:match_paren(protos, i) - 1])
    k = [j for j, p in enumerate(params) if re.search(r'\b%s\s*$' % re.escape(name), p)]
    if not k:
        log.append('%s: parameter %s not in prototype' % (func, name))
        return
    protos = cxx_retype.edit_signature(protos, m.end() - len(func) - 1, func, k[0] + 1, newtype)
    open(cxx_retype.PROTOS, 'w').write(protos)
    for f in sources():
        text = open(f).read()
        for d in re.finditer(r'(?m)^(?:\w[^\n;]*\b)?%s\s*\(' % re.escape(func), text):
            o = text.index('(', d.start() + d.group(0).rfind(func))
            e = match_paren(text, o)
            if re.match(r'\s*\{', text[e:]):
                open(f, 'w').write(cxx_retype.edit_signature(text, o - len(func), func, k[0] + 1, newtype))
                return
    log.append('no definition: %s' % func)


def write_checks(checks):
    p = os.path.join(ROOT, 'run', 'fields_check.cpp')
    lines = ['#include <stddef.h>', '#include <type_traits>', '#include "auto_common.h"']
    have = set(open(p).read().split('\n')[3:]) if os.path.exists(p) else set()
    new = set()
    for s, path, n, t in checks:
        t = t.strip()
        new.add('static_assert(offsetof(%s, %s) == %d, "");' % (s, path, n))
        if canon(t)[0] == 't':
            new.add('static_assert(std::is_same<decltype(((%s *)0)->%s), %s>::value, "");' % (s, path, cast_type(t)))
        else:
            new.add('static_assert(sizeof(((%s *)0)->%s) == sizeof(%s) && std::is_integral<std::remove_reference<decltype(((%s *)0)->%s)>::type>::value, "");'
                    % (s, path, t, s, path))
    open(p, 'w').write('\n'.join(lines + sorted((have | new) - {''})) + '\n')


def main():
    global LAYOUT
    if len(sys.argv) < 3 or sys.argv[1] not in ('fields', 'auto', 'retype'):
        print(__doc__)
        return 1
    LAYOUT = load_layout()
    load_returns()
    load_globals()
    checks, stats, log = [], collections.Counter(), []
    mode, args = sys.argv[1], sys.argv[2:]
    if mode == 'retype':
        rows = [l.rstrip('\n').split('\t') for f in args for l in open(f) if l.strip() and not l.startswith('#')]
        retype_rows(rows, checks, stats, log)
        files = sources()
    else:
        files = args
    for f in files:
        def one(header, body, f=f):
            if mode == 'auto':
                body = auto_pass(header, body, checks, stats, log, '%s:%s' % (os.path.relpath(f, ROOT), func_name(header)))
            return typed_pass(body, decls(header, body), checks, stats)
        each_function(f, one)
    write_checks(checks)
    open(os.path.join(ROOT, 'run', 'fields.log'), 'w').write('\n'.join(log) + '\n')
    print(dict(stats))
    return 0


if __name__ == '__main__':
    sys.exit(main())
