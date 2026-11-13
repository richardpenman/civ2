#!/usr/bin/env python3
"""Give a function's parameter (or return value) its real type, in
src/auto/protos.h (a member function: its class in
src/include/civ2classes.h) and in the definition:

    cxx_retype.py ROWS.tsv     rows: FUNCTION<TAB>PARAM<TAB>NEWTYPE
                               FUNCTION: a free name or Class::member
                               PARAM: 1-based position (self counts) or ret

The call sites are left to the compiler: tools/cxx_retype.sh compiles,
lets tools/cxx_fix.py put the casts C made implicitly where the new type
does not convert, and compares the machine code (tools/cxx_compare.py).
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROTOS = os.path.join(ROOT, 'src', 'auto', 'protos.h')
CLASSES = os.path.join(ROOT, 'src', 'include', 'civ2classes.h')


def sources():
    return sorted(glob.glob(os.path.join(ROOT, 'src', d, '*.cpp')) for d in ('auto', 'game', 'native', 'data'))


def split_params(text):
    out, depth, cur = [], 0, ''
    for c in text:
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        if c == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += c
    out.append(cur)
    return out


def retype_param(p, newtype):
    """'iptr str' -> 'const char *str' (spacing kept)"""
    lead = p[:len(p) - len(p.lstrip())]
    m = re.match(r'(.*?)(\w+)\s*$', p.strip(), re.S)
    name = m.group(2)
    sep = '' if newtype.endswith('*') else ' '
    return lead + newtype + sep + name


def edit_signature(text, start, func, index, newtype):
    """Retype in the declaration/definition of func whose name is at start."""
    i = text.index('(', start)
    depth, j = 0, i
    while True:
        if text[j] == '(':
            depth += 1
        elif text[j] == ')':
            depth -= 1
            if depth == 0:
                break
        j += 1
    if index == 'ret':
        # the return type: what precedes the name back to the line start (or
        # the line before, Ghidra style), without the calling convention
        line_start = text.rfind('\n', 0, start) + 1
        head = text[line_start:start]
        if not head.strip():
            prev = text.rfind('\n', 0, line_start - 1) + 1
            head_start, head = prev, text[prev:line_start - 1]
        else:
            head_start = line_start
        m = re.match(r'(\s*(?:EXTERN_C\s+)?)(.*?)(\s*(?:__cdecl|__stdcall|__thiscall|__fastcall)?\s*)$', head, re.S)
        new_head = m.group(1) + newtype + m.group(3)
        if not m.group(3) and not newtype.endswith('*'):
            new_head += ' '
        return text[:head_start] + new_head + text[head_start + len(head):]
    params = split_params(text[i + 1:j])
    k = int(index) - (2 if '::' in func else 1)
    params[k] = retype_param(params[k], newtype)
    return text[:i + 1] + ','.join(params) + text[j:]


def member_decl(classes, func):
    """Index of the member's name in its class's declaration (or None)."""
    cls, member = func.split('::')
    m = re.search(r'(?m)^(?:typedef )?struct %s\b[^;{]*\{' % re.escape(cls), classes)
    if not m:
        return None
    end = re.compile(r'(?m)^\}').search(classes, m.end()).start()
    d = re.compile(r'(?m)^\s+[^;/]*?\b%s\(' % re.escape(member)).search(classes, m.end(), end)
    return d.end() - len(member) - 1 if d else None


def main():
    if len(sys.argv) != 2:
        print(__doc__)
        return 1
    rows = [l.rstrip('\n').split('\t') for l in open(sys.argv[1]) if l.strip() and not l.startswith('#')]
    protos = open(PROTOS).read()
    classes = open(CLASSES).read()
    files = {f: open(f).read() for group in sources() for f in group}
    done = 0
    for func, index, newtype in rows:
        if '::' in func:
            at = member_decl(classes, func)
            if at is None:
                print('no declaration: %s' % func)
                continue
            classes = edit_signature(classes, at, func, index, newtype)
        else:
            m = re.search(r'(?m)^.*?\b%s\(' % re.escape(func), protos)
            if not m:
                print('no prototype: %s' % func)
                continue
            protos = edit_signature(protos, m.end() - len(func) - 1, func, index, newtype)
        found = False
        for f, text in files.items():
            # the definition: the name, its parameters, then the body
            for d in re.finditer(r'(?<!::)\b%s\s*\(' % re.escape(func), text):
                k = text.index('(', d.start())
                depth, j = 0, k
                while j < len(text):
                    if text[j] == '(':
                        depth += 1
                    elif text[j] == ')':
                        depth -= 1
                        if depth == 0:
                            break
                    j += 1
                if re.match(r'\s*\{', text[j + 1:]) and re.search(r'(^|\n)[^\n;{}]*$', text[:d.start()]):
                    files[f] = edit_signature(text, d.start(), func, index, newtype)
                    found = True
                    break
            if found:
                break
        if not found:
            print('no definition: %s' % func)
        done += 1
    open(PROTOS, 'w').write(protos)
    open(CLASSES, 'w').write(classes)
    for f, text in files.items():
        if open(f).read() != text:
            open(f, 'w').write(text)
    print('%d retyped' % done)
    return 0


if __name__ == '__main__':
    sys.exit(main())
