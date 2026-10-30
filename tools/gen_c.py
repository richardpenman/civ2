#!/usr/bin/env python3
"""Turn Ghidra's decompiler export into compilable C (the "auto" tier).

Inputs (from tools/ghidra_scripts/ExportAll.java and tools/conventions.py):
    re/decomp.c, re/functions.tsv, re/conventions.tsv
Outputs:
    src/auto/blk_XXXX.c   function bodies, grouped by 64 KiB of address space
    src/auto/protos.h     prototypes for every original function and thunk
    src/auto/globals.h    typed lvalue macros for unnamed globals
    re/manifest.tsv       one row per function: address, name, file, status

Functions whose output still contains decompiler artifacts that have no C
meaning (register inputs, unrecovered stack) are emitted inside `#if 0`
with status "manual" so they keep running as original code until a person
reconstructs them.  Overrides in src/manual/ always win over the auto tier.

Retired (2026-10): src/auto is now the source, edited directly; running
this again would overwrite those edits.  It refuses unless
CIV2_REGENERATE=1 is set (kept for reference and for comparing a fresh
decompilation with the sources).
"""
import collections
import csv
import glob
import os
import re
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (the game's directory)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RE = os.path.join(ROOT, 're')
OUT = os.path.join(ROOT, 'src', 'auto')
UNUSED = os.path.join(ROOT, 'src', 'unused')     # functions nothing refers to (re/unreferenced.tsv)
CRT_START = 0x5F1B80
ASMGRAF_START = 0x61A000

HEX = r'[0-9a-f]{8}'


def load_tsv(name):
    with open(os.path.join(RE, name)) as f:
        return list(csv.DictReader(f, delimiter='\t'))


KNOWN_TYPES = set('''void int uint char byte uchar short ushort long ulong longlong ulonglong float double float10
    bool undefined undefined1 undefined2 undefined4 undefined8 size_t uintptr_t intptr_t time_t
    LPSTR LPCSTR LPVOID LPCVOID BOOL DWORD WORD BYTE HANDLE HWND HINSTANCE UINT LONG ULONG LPDWORD
    FILE wchar_t code unsigned signed const struct'''.split())


def parse_decomp():
    """Split decomp.c into per-function records."""
    text = open(os.path.join(RE, 'decomp.c')).read()
    # Methods live in class namespaces (C_xxxxxxxx::name); C has no namespaces.
    text = re.sub(r'\bC_[0-9a-f]{8}(?:_[0-9a-f]+)?::', '', text)
    text = re.sub(r'\b(__thiscall|__stdcall|__cdecl)\s+\1\b', r'\1', text)
    parts = re.split(r'^// ==== FUNCTION ([0-9a-f]{8}) (\S+)\n', text, flags=re.M)
    funcs = []
    for i in range(1, len(parts), 3):
        addr, name, body = int(parts[i], 16), parts[i + 1], parts[i + 2]
        globs = {}
        lines = []
        for line in body.split('\n'):
            if line.startswith('// GLOBAL '):
                gname, gaddr, gtype, gsize = line[len('// GLOBAL '):].split('\t')
                globs[gname] = (gaddr, gtype, int(gsize))
            else:
                lines.append(line)
        funcs.append({'addr': addr, 'name': name, 'globals': globs, 'body': '\n'.join(lines).strip() + '\n'})
    return funcs


SIG_RE = re.compile(r'^(?P<ret>[^\n(]*?)\s*\b(?P<cc>__cdecl|__stdcall|__thiscall|__fastcall)?\s*(?P<name>[A-Za-z_][\w:$`\']*)\((?P<params>[^)]*)\)\s*$', re.M)


def split_signature(body):
    """Return (signature_line, rest) where rest starts at the opening brace."""
    i = body.find('\n{')
    if i < 0:
        return None, body
    head = re.sub(r'/\*.*?\*/', ' ', body[:i], flags=re.S)
    head = re.sub(r'//[^\n]*', ' ', head)
    sig = ' '.join(head.split())
    return sig, body[i + 1:]


def parse_sig(sig):
    m = re.match(r'^(.*?)\s*\b(__cdecl|__stdcall|__thiscall|__fastcall)?\s*([A-Za-z_][\w]*)\s*\((.*)\)$', sig)
    if not m:
        return None
    ret, cc, name, params = m.group(1).strip(), m.group(2), m.group(3), m.group(4).strip()
    return {'ret': ret or 'void', 'cc': cc, 'name': name, 'params': params}


def c_type_of_global(gtype):
    """Map a Ghidra display type to a C type usable in a cast."""
    t = gtype.strip()
    t = t.replace('struct ', '').replace('union ', '')
    return t


# Identifiers in decompiled text that we turn into address-based macros.
GLOBAL_NAME_RE = re.compile(r'\b(_?(?:DAT|PTR|BYTE|WORD|DWORD|INT|UINT|SHORT|USHORT|CHAR|UCHAR|LAB|switchdataD|s|u|PTR_[A-Za-z0-9_]+|PTR_FUN|PTR_LAB|PTR_DAT|PTR_s|PTR_u|PTR_PTR|PTR_PTR_DAT|PTR_PTR_FUN)_[A-Za-z0-9_]*?_?(' + HEX + r'))\b')

UNSUPPORTED = re.compile(r'\b(__ftol\(\)|in_[A-Z]{2,3}|in_[A-Z]{2}\b|unaff_(?!FS_OFFSET)\w+|extraout_\w+|in_stack_\w+|in_FS_OFFSET|in_GS_OFFSET|unaff_retaddr|in_ST\d|in_CF|in_ZF|in_SF|in_OF|in_PF|in_AF|in_DF|in_TF|in_IF|in_NT|in_ID|in_VIP|in_VIF|in_AC|in_IOPL|in_XMM\w*|CONCAT1616|SUB168|SUB164)\b')


SEH_LINES = [
    re.compile(r'^\s*\w+ = \*unaff_FS_OFFSET;\s*$'),
    re.compile(r'^\s*\*unaff_FS_OFFSET = (&\w+|\w+);\s*$'),
    re.compile(r'^\s*\w+\s*\*unaff_FS_OFFSET;\s*$'),
]


def strip_seh(body):
    # Every statement that touches fs:[0] is exception-frame bookkeeping, as is
    # storing the frame's handler address (`puStack_c = &LAB_xxx;`).
    body = '\n'.join(l for l in body.split('\n') if 'unaff_FS_OFFSET' not in l
                     and not re.match(r'^\s*\w+_c = &LAB_[0-9a-f]{8};\s*$', l))
    return _strip_seh(body)


def _strip_seh(body):
    """Drop MSVC C++ exception-frame registration (fs:[0] chain).

    Civ2 never throws, so the frame is only bookkeeping.  The state variable
    writes (local_8 = N) are left in place; they are plain locals.
    """
    out = []
    for line in body.split('\n'):
        if any(p.match(line) for p in SEH_LINES):
            continue
        out.append(line)
    return '\n'.join(out)


RAM_TYPES = {'u': 'uint', 'i': 'int', 'c': 'char', 'b': 'byte', 's': 'short', 'p': 'void *',
             'us': 'ushort', 'pu': 'uint *', 'pc': 'char *', 'pi': 'int *'}


DYN_STRING = re.compile(r'(?<![\w.])([su]_[^\s,;]*?_([0-9a-f]{8}))(?![\w])')


THUNK_NAMES = None
CODE_NAMES = None


def thunk_symbols(body):
    """&LAB_x where x is an incremental-link thunk: that thunk's function
    (same address in the 32-bit build, a real function in native builds).
    Thunk slots Ghidra did not define, and function entries, become
    ADDR(x, function)."""
    global THUNK_NAMES, CODE_NAMES
    if THUNK_NAMES is None:
        THUNK_NAMES = {}
        CODE_NAMES = {}
        for r in load_tsv('functions.tsv'):
            if not re.match(r'^[A-Za-z_]\w*$', r['name']):
                continue
            if r['thunk_target'] and not r['thunk_target'].startswith('EXT'):
                THUNK_NAMES[r['entry']] = r['name']
            CODE_NAMES[int(r['entry'], 16)] = r['name']
        CODE_NAMES.update(thunk_table_targets(CODE_NAMES))

    def rep(m):
        if m.group(1) in THUNK_NAMES:
            return '(void *)%s' % THUNK_NAMES[m.group(1)]
        a = int(m.group(1), 16)
        if a in CODE_NAMES:
            return '((void *)ADDR(0x%08x, %s))' % (a, CODE_NAMES[a])
        return m.group(0)
    return re.sub(r'&LAB_([0-9a-f]{8})\b', rep, body)


VCALL = re.compile(r'\*\(code \*\*\)\((\*?[A-Za-z_][\w>.\-\[\]]*) \+ (0x[0-9a-f]+|\d+)\)')


THISLESS_VCALL = re.compile(r'\(\*\*\(code \*\*\)(?:(\w+)->(vtable|field_0)|\(\*\(int \*\)(\w+) \+ (0x[0-9a-f]+|\d+)\))\)\(')


def fix_thisless_vcalls(body):
    """A C++ virtual call on an object, (**(code **)obj->vtable)(args) or
    (**(code **)(*(int *)obj + K))(args): Ghidra drops the object, which
    the original passes in ECX.  It becomes the first argument, as for every
    other thiscall function here (COM calls, which pass it on the stack,
    already have it)."""
    out, pos = [], 0
    for m in THISLESS_VCALL.finditer(body):
        obj = m.group(1) or m.group(3)
        slot = int(m.group(4), 0) // 4 if m.group(4) else 0
        if m.group(4) and int(m.group(4), 0) % 4:
            continue
        rest = body[m.end():]
        if re.match(r'\s*%s\s*[,)]' % re.escape(obj), rest):
            continue
        table = '%s->%s' % (obj, m.group(2)) if m.group(1) else '*(int **)%s' % obj
        out.append(body[pos:m.start()])
        out.append('(*((code **)(%s))[%d])(%s%s' % (table, slot, obj, ')' if rest.lstrip().startswith(')') else ','))
        pos = m.end()
        if rest.lstrip().startswith(')'):
            pos += rest.index(')') + 1
    out.append(body[pos:])
    return ''.join(out)


def fix_vtable_calls(body):
    """*(code **)(vt + K): slot K/4 of a table of function pointers.  The
    original slots are 4 bytes; indexing keeps this right when they are not."""
    def rep(m):
        k = int(m.group(2), 0)
        return '((code **)(%s))[%d]' % (m.group(1), k // 4) if k % 4 == 0 else m.group(0)
    return VCALL.sub(rep, body)


GETC = re.compile(
    r'(?P<i>[ \t]*)(?P<f>\w+)->_cnt = (?P=f)->_cnt \+ -1;\n\s*if \((?P=f)->_cnt < 0\) \{\n'
    r'(?:\s*(?:(?P<v>\w+) = )?__filbuf\((?P=f)\);\n)(?:\s*(?P<s1>[^\n{};]+;)\n)?\s*\}\n\s*else \{\n'
    r'(?:\s*(?P<s2>[^\n{};]+;)\n)?\s*(?P=f)->_ptr = (?P=f)->_ptr \+ 1;\n\s*\}')


def fix_stdio_macros(body):
    """The MSVC getc() and feof() macros, expanded inline by the compiler,
    read FILE internals; spell them as the CRT_GETC/CRT_FEOF helpers.
    getc's two branches use the character the same way (the buffer branch
    reads *f->_ptr, the refill branch takes __filbuf's result)."""
    def rep(m):
        i, f, v, s1, s2 = m.group('i', 'f', 'v', 's1', 's2')
        if v is None:
            return m.group(0) if (s1 or s2) else '%sCRT_GETC(%s);' % (i, f)
        deref = re.escape('*%s->_ptr' % f)
        if s1 is None:
            # if: v = __filbuf(f);  else: v = (cast)*f->_ptr;
            if s2 and re.match(r'^%s = (?:\(\w+\)\(\w+\))?%s;$' % (re.escape(v), deref), s2):
                return '%s%s = CRT_GETC(%s);' % (i, v, f)
            return m.group(0)
        # if: v = __filbuf(f); S(v)   else: S(*f->_ptr)
        for spelled in (v, '(char)' + v):
            pat = re.escape(s1).replace(re.escape(spelled), '(?:\\(uint\\)\\(byte\\))?' + deref)
            if spelled in s1 and s2 and re.match('^' + pat + '$', s2):
                return '%s%s = CRT_GETC(%s);\n%s%s' % (i, v, f, i, s1)
        return m.group(0)
    body = GETC.sub(rep, body)
    return re.sub(r'\b(\w+)->_flag & 0x10\b', r'CRT_FEOF(\1)', body)


ADDR_LITERAL = re.compile(r'(?<!ADDR\()(?:\b0x00([4-6][0-9a-f]{5})\b|\b0x([4-6][0-9a-f]{5})\b)')


def thunk_table_targets(entry_names):
    """Slots of the incremental-link thunk table (5-byte `jmp rel32` from
    0x401000 to 0x403d3c) that Ghidra did not define as functions: the name
    of the function each jumps to."""
    import pefile
    img = pefile.PE(paths.game_file('civ2.exe')).get_memory_mapped_image()
    out = {}
    for a in range(0x401000, 0x403d3c, 5):
        o = a - 0x400000
        if img[o] == 0xe9 and a not in entry_names:
            t = (a + 5 + int.from_bytes(img[o + 1:o + 5], 'little', signed=True)) & 0xffffffff
            if t in entry_names:
                out[a] = entry_names[t]
    return out


# GDI raster operation codes that fall in the image's address range.
RASTER_OPS = {0x660046, 0x550009, 0x5a0049, 0x440328}


def fix_addr_literals(body, func_names):
    """Integer constants that are addresses of the original image: data in
    .rdata/.data, and functions passed as callbacks.  ADDR(original, native)
    keeps the number in the 32-bit build and is the symbol in native builds."""
    def rep(m):
        v = int(m.group(1) or m.group(2), 16)
        if v in RASTER_OPS:
            return m.group(0)
        if 0x61c000 <= v < 0x6e6b6c:
            return 'ADDR(%s, %s)' % (m.group(0), data_location(v))
        if v in func_names:
            return 'ADDR(%s, %s)' % (m.group(0), func_names[v])
        return m.group(0)
    out = []
    for line in body.split('\n'):
        t = line.lstrip()
        if t.startswith(('#', '/*')) or re.match(r'\*(\s|/|$)', t) or 'case ' in line:
            out.append(line)
        else:
            out.append(ADDR_LITERAL.sub(rep, line))
    return '\n'.join(out)


def fix_misc_names(body):
    """Decompiler spellings with no declaration: xRamADDR (typed absolute
    references), _func_* (function-pointer typedef names), and string
    references named after their contents (s_Hello_world_00612345), whose
    names may contain any character."""
    body = DYN_STRING.sub(lambda m: re.sub(r'\W', '_', m.group(1)), body)
    # A handle-typed integer: &h->unused is the handle's own value.
    body = re.sub(r'\(int\)&(\w+)->unused\b', r'(iptr)\1', body)
    body = re.sub(r'\b([a-z]{1,2})Ram([0-9a-f]{8})\b',
                  lambda m: '(*(%s *)0x%s)' % (RAM_TYPES.get(m.group(1), 'undefined4'), m.group(2)), body)
    body = re.sub(r'\b_func_\w+\b', 'code', body)
    return body


FIELD_RE = re.compile(r'(\b[A-Za-z_]\w*(?:\[[^\[\]]*\]|\.(?!_\d)\w+)*)\._(\d+)_(\d+)_')


def fix_fields(body):
    """`var._N_M_` (bytes N..N+M of var) -> typed lvalue."""
    size_type = {1: 'undefined1', 2: 'undefined2', 4: 'undefined4', 8: 'undefined8'}

    def rep(m):
        n, sz = int(m.group(2)), int(m.group(3))
        t = size_type.get(sz, 'undefined%d' % sz)
        return '(*(%s *)((char *)&%s + %d))' % (t, m.group(1), n)
    return FIELD_RE.sub(rep, body)


def this_slots():
    """Frame offset where each __thiscall function spills ECX (`mov [ebp-N], ecx`)."""
    import capstone, pefile
    pe = pefile.PE(paths.game_file('civ2.exe'))
    img = pe.get_memory_mapped_image()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    out = {}
    for r in load_tsv('conventions.tsv'):
        if r['cc'] != '__thiscall':
            continue
        a = int(r['entry'], 16)
        for ins in md.disasm(img[a - 0x400000:a - 0x400000 + 64], a):
            m = re.match(r'dword ptr \[ebp - (0x[0-9a-f]+)\], ecx$', ins.op_str)
            if ins.mnemonic == 'mov' and m:
                out[a] = int(m.group(1), 16)
                break
            if ins.mnemonic in ('call', 'ret') or ins.mnemonic.startswith('j'):
                break
    return out


FUNCLET_EBP = re.compile(r'unaff_EBP\s*\+\s*-0x([0-9a-f]+)')
DECL = re.compile(r'^\s+[\w\s\*]+?\b([A-Za-z_]\w*)\s*(\[[^\]]*\])?;\s*$')


def funclet_statements(body):
    """Statements of an unwind funclet body, without its declarations/return."""
    inner = body[body.find('{') + 1:body.rfind('}')]
    out = []
    for line in inner.split('\n'):
        t = line.strip()
        if not t or t == 'return;' or re.match(r'^\w+ unaff_EBP;$', t) or t.startswith('/*'):
            continue
        if 'unaff_FS_OFFSET' in t:
            continue          # exception-frame bookkeeping (fs:[0]); see strip_seh
        out.append(t)
    return out


def inline_funclets(body, funclets, this_off):
    """Replace calls to unwind funclets with their statements, rewriting
    frame-relative accesses (EBP-N) to the caller's own locals."""
    head = body[:body.find('{')]
    decl_part = body[body.find('{'):]
    names = {}
    for line in decl_part.split('\n')[1:]:
        if not line.strip():
            break
        m = DECL.match(line)
        if m:
            suffix = m.group(1).rsplit('_', 1)[-1]
            names[suffix] = m.group(1)

    starts = sorted((int(k, 16) - 4, v) for k, v in names.items() if re.match(r'^[0-9a-f]+$', k))

    def frame_ref(m):
        n = int(m.group(1), 16)
        if this_off is not None and n == this_off:
            return '(iptr)&this'
        var = names.get('%x' % (n + 4))
        if var is not None:
            return '(iptr)&' + var
        # Inside a local (an object embedded in a larger one): the nearest
        # local starting below the address, plus the offset.
        inside = [(st, v) for st, v in starts if st > n]
        if not inside:
            raise KeyError('ebp-0x%x' % n)
        st, var = inside[0]
        return '(iptr)((u8 *)&%s + 0x%x)' % (var, st - n)

    def rep(m):
        stmts = funclets[m.group(2)]
        out = []
        for st in stmts:
            out.append(FUNCLET_EBP.sub(frame_ref, st))
        return m.group(1) + '/* inlined unwind funclet %s */ { ' % m.group(2) + ' '.join(out) + ' }'
    pat = re.compile(r'^(\s*)\b(' + '|'.join(map(re.escape, funclets)) + r')\(\);', re.M)
    return pat.sub(rep, body)


STACK_NAME = re.compile(r'^(?:local|[a-z]+Stack)_([0-9a-f]+)$')
DECL_FULL = re.compile(r'^\s+(?P<type>[A-Za-z_][\w\s\*]*?)\s*(?P<name>\b[A-Za-z_]\w*)\s*(?P<dims>(?:\[\d+\])*)\s*;\s*$')


# The C runtime functions the game calls, with their real types (the
# decompiler's guesses do not distinguish pointers).  Native builds use
# these, implemented over the platform's C library in src/native/crt.cpp;
# MSVC's `long` is 32-bit (i32).
CRT_NATIVE_SIGS = {
    'operator_new': 'void *operator_new(size_t n)',
    'operator_delete': 'void operator_delete(void *p)',
    'crt_srand': 'void crt_srand(u32 seed)',                          # srand
    '_rand': 'int _rand(void)',
    'crt_strcpy': 'void *crt_strcpy(void *dst, const void *src)',       # strcpy
    'crt_strcat': 'void *crt_strcat(void *dst, const void *src)',       # strcat
    'crt_memcpy': 'void *crt_memcpy(void *dst, const void *src, uint n)',   # memcpy
    'crt_memmove': 'void *crt_memmove(void *dst, const void *src, uint n)',   # memmove
    'crt_realloc': 'void *crt_realloc(void *p, size_t n)',                # realloc
    'crt_rename': 'int crt_rename(const char *from, const char *to)',  # rename
    'crt_remove': 'int crt_remove(const char *path)',                  # remove
    'crt_abs': 'int crt_abs(int v)',                             # abs
    'FUN_006076a0': 'int FUN_006076a0(FILE *f)',                           # fileno
    'crt_mkdir': 'int crt_mkdir(const char *path)',                  # mkdir
    'crt_toupper': 'uint crt_toupper(uint c)',                           # toupper
    'crt_vector_ctor_iterator': 'void crt_vector_ctor_iterator(void *array, uint size, int count, code *ctor, code *dtor)',
    'crt_vector_dtor_iterator': 'void crt_vector_dtor_iterator(void *array, uint size, int count, code *dtor)',
    '_printf': 'int _printf(const char *fmt, ...)',
    '_sprintf': 'int _sprintf(char *buf, const char *fmt, ...)',
    '_time': 'i32 _time(i32 *t)',
    '__strlwr': 'char *__strlwr(char *s)',
    '__strupr': 'char *__strupr(char *s)',
    '__chdir': 'int __chdir(const char *path)',
    '__filelength': 'i32 __filelength(int fd)',
    '__fsopen': 'FILE *__fsopen(const char *path, const char *mode, int share)',
    '__getcwd': 'char *__getcwd(char *buf, int n)',
    '__itoa': 'char *__itoa(int v, char *buf, int radix)',
    '__ltoa': 'char *__ltoa(i32 v, char *buf, int radix)',
    '__ultoa': 'char *__ultoa(u32 v, char *buf, int radix)',
    '__msize': 'size_t __msize(void *p)',
    '__strcmpi': 'int __strcmpi(const char *a, const char *b)',
    '__strnicmp': 'int __strnicmp(const char *a, const char *b, size_t n)',
    '_atexit': 'int _atexit(void (*fn)(void))',
    '_atoi': 'int _atoi(const char *s)',
    '_atol': 'i32 _atol(const char *s)',
    '_fclose': 'int _fclose(FILE *f)',
    '_fgetc': 'int _fgetc(FILE *f)',
    '_fgets': 'char *_fgets(char *buf, int n, FILE *f)',
    '_fopen': 'FILE *_fopen(const char *path, const char *mode)',
    '_fputc': 'int _fputc(int c, FILE *f)',
    '_fputs': 'int _fputs(const char *s, FILE *f)',
    '_fread': 'size_t _fread(void *buf, size_t size, size_t n, FILE *f)',
    '_fseek': 'int _fseek(FILE *f, i32 off, int whence)',
    '_ftell': 'i32 _ftell(FILE *f)',
    '_fwrite': 'size_t _fwrite(const void *buf, size_t size, size_t n, FILE *f)',
    '_isalpha': 'int _isalpha(int c)',
    '_isdigit': 'int _isdigit(int c)',
    '_memcmp': 'int _memcmp(const void *a, const void *b, size_t n)',
    '_memset': 'void *_memset(void *p, int c, size_t n)',
    '_rewind': 'void _rewind(FILE *f)',
    '_strchr': 'char *_strchr(const char *s, int c)',
    '_strcmp': 'int _strcmp(const char *a, const char *b)',
    '_strlen': 'size_t _strlen(const char *s)',
    '_strncat': 'char *_strncat(char *dst, const char *src, size_t n)',
    '_strncmp': 'int _strncmp(const char *a, const char *b, size_t n)',
    '_strncpy': 'char *_strncpy(char *dst, const char *src, size_t n)',
    '_strrchr': 'char *_strrchr(const char *s, int c)',
    '_strstr': 'char *_strstr(const char *s, const char *sub)',
    '__CxxUnhandledExceptionFilter': 'int __CxxUnhandledExceptionFilter(EXCEPTION_POINTERS *p)',
}


# Stack slots the original reads before writing (bugs in the 1997 code).  What
# it reads is whatever earlier calls left on the stack, which no compiler
# reproduces; these presets give the value the original was seen to read
# (hook/stackprobe.c, CIV2_STACKPROBE), so native runs match the recordings
# whichever compiler built them.  Found with valgrind (tools/native-run.sh).
FRAME_PRESETS = {
    # A unit that does not move skips filling "which civs see the move" and
    # then reads the entry for the human player: non-zero in the original.
    'unit_move_dir': 'aiStack_a0[1] = aiStack_a0[2] = aiStack_a0[3] = aiStack_a0[4] = '
                    'aiStack_a0[5] = aiStack_a0[6] = aiStack_a0[7] = 1;',
    # City advice counts tiles with an improvement (bit 0x10) into a counter
    # it never zeroes; the original's stack held 0 there (in 687 of 713 calls
    # over 50 turns; the rest saw a stack address left by an earlier call).
    'city_calc_commodity_demand': 'local_12c = 0;',
    # The AI's review of a neighbour starts its attitude change from the
    # count of border intruders, set only when there are some: otherwise it
    # adds to what the stack held, a stale pointer in the original
    # (0x6715d7a0 in the WWII scenario under Wine; the attitude clamps, so
    # what matters is that it is large and positive).
    'diplo_ai_review_relations': 'local_34 = 0x6715d7a0;',
    # The AI's choice of orders reads seven locals some paths leave unset;
    # the original, always called from the same place, holds the same
    # values there on every call (2367 calls probed over 15 turns; local_dc
    # and local_2c: 54 calls in the Civil War scenario).
    'ai_unit_choose_orders': 'local_9c = 0x108; local_11c = 0x64a8c19c; local_70 = 0x3bd; '
                             'local_38 = 0; local_16c = 4; local_dc = 0x21f238; '
                             'local_2c = 0x21f2dc;',
    # The pathfinder compares the tile owner it looked up last with the
    # unit's owner, before any lookup when the owner is negative: 0 in the
    # original's stack in 3892 of 4300 calls (-1 in 158).
    'map_path_next_dir': 'local_80 = 0;',
}


def emulate_args(body):
    """Stack arguments as one contiguous block.

    The 1997 code takes the address of an argument and reads the following
    arguments through it (a RECT passed by value as four arguments, say).
    The arguments of a 32-bit x86 call lie next to each other on the stack;
    a wasm function's parameters do not.  From the first argument whose
    address is taken, the arguments are copied into 4-byte slots and used
    from there.  Returns (body, macro names to #undef).
    """
    taken = [int(n) for n in re.findall(r'&param_(\d+)\b(?!\s*(?:->|\.))', body)]
    lb = body.find('{')
    if not taken or lb < 0:
        return body, []
    head = body[:lb]
    plist = head[head.find('(', head.find(' ')) + 1:head.rfind(')')] if '(' in head else ''
    types = {}
    for item in plist.split(','):
        m = re.match(r'^\s*(.*?)\s*\bparam_(\d+)\s*$', item.strip())
        if m:
            types[int(m.group(2))] = m.group(1).strip() or 'int'
    first = min(taken)
    later = sorted(k for k in types if k >= first)
    if len(later) < 2 or later != list(range(first, first + len(later))):
        return body, []
    if any(re.search(r'double|float|longlong|8\b', types[k]) for k in later):
        return body, []
    lines = body[lb:].split('\n')
    i = 1
    while i < len(lines) and lines[i].strip() and DECL_FULL.match(lines[i]) or (i < len(lines) and 'frame_[' in lines[i]):
        i += 1
    init = '  u32 pargs_[%d] = {%s};  /* the arguments, contiguous as on the original stack */' % (
        len(later), ', '.join('(u32)(uptr)param_%d' % k for k in later))
    defs = ['#define param_%d (*(%s *)&pargs_[%d])' % (k, types[k], n) for n, k in enumerate(later)]
    lines[i:i] = [init] + defs
    return body[:lb] + '\n'.join(lines), ['param_%d' % k for k in later]


def emulate_frame(body, name=None):
    """Place stack locals at their original frame offsets.

    Ghidra names stack variables after their distance below the return
    address (local_14 lives at entry ESP - 0x14).  Declaring them all inside
    one byte array at those offsets reproduces the original frame layout, so
    overflows between neighbouring locals (which the 1997 code has, harmlessly
    in its own layout) behave exactly as in the original.
    Returns (body, macro_defs, macro_names).
    """
    lb = body.find('{')
    if lb < 0:
        return body, [], []
    lines = body[lb:].split('\n')
    decls = []
    keep = [lines[0]]
    i = 1
    while i < len(lines) and lines[i].strip() and DECL_FULL.match(lines[i]):
        m = DECL_FULL.match(lines[i])
        sm = m and STACK_NAME.match(m.group('name'))
        if sm:
            decls.append((m.group('name'), m.group('type').strip(), m.group('dims'), int(sm.group(1), 16)))
        else:
            keep.append(lines[i])
        i += 1
    if not decls:
        return body, [], []
    size = (max(off for _, _, _, off in decls) + 7) & ~7
    keep.append('  undefined1 frame_[FRAME_OFF(%d)] __attribute__((aligned(8)));  /* original stack frame */' % size)
    if name in FRAME_PRESETS:
        keep.append('  ' + FRAME_PRESETS[name] + '  /* stale stack the original reads (FRAME_PRESETS) */')
    defs, names = [], []
    for name, typ, dims, off in decls:
        where = '(frame_ + FRAME_OFF(%d))' % (size - off)
        if dims:
            defs.append('#define %s (*(%s (*)%s)%s)' % (name, typ, dims, where))
        else:
            defs.append('#define %s (*(%s *)%s)' % (name, typ, where))
        names.append(name)
    new = body[:lb] + '\n'.join(keep + lines[i:])
    return new, defs, names


def main():
    funcs_tsv = load_tsv('functions.tsv')
    entry_names = {int(r['entry'], 16): r['name'] for r in funcs_tsv if re.match(r'^[A-Za-z_]\w*$', r['name'])}
    entry_names.update(thunk_table_targets(entry_names))
    conv = {int(r['entry'], 16): r for r in load_tsv('conventions.tsv')}
    funcs = parse_decomp()
    contiguous_args = []     # functions reading arguments through an earlier one's address
    by_addr = {f['addr']: f for f in funcs}

    # Thunks: thunk symbol name -> (thunk address, target address)
    thunks = {}
    tsv_by_addr = {}
    for r in funcs_tsv:
        a = int(r['entry'], 16)
        tsv_by_addr[a] = r
        if r['thunk_target'] and not r['thunk_target'].startswith('EXTERNAL'):
            thunks[r['name']] = (a, int(r['thunk_target'], 16))

    os.makedirs(OUT, exist_ok=True)

    # Signatures for prototypes, from each function's own definition.
    protos = {}
    names = {}
    for f in funcs:
        sig, rest = split_signature(f['body'])
        p = parse_sig(sig) if sig else None
        f['sig'] = p
        f['rest'] = rest
        names[f['name']] = f['addr']
        if p:
            protos[f['name']] = p

    def proto_decl(name, p, addr, cc_override=None):
        cc = cc_override or p['cc'] or '__cdecl'
        params = p['params']
        if addr >= CRT_START:
            # Library code.  Use Ghidra's signature when every type in it is
            # one we know; otherwise declare generically (calls still work,
            # and a definition then fails to compile and is demoted).
            if not re.match(r'^[A-Za-z_]\w*$', name):
                return '/* %s: not a C identifier */' % name
            types = re.findall(r'[A-Za-z_]\w*', p['ret'] + ' ' + re.sub(r'\b\w+\s*(,|$)', r'\1', p['params']))
            if all(t in KNOWN_TYPES for t in types):
                params = '' if cc == '__cdecl' and not re.search(r'\b(char|byte|short|ushort|undefined1|undefined2|float|bool)\b(?!\s*\*)', p['params']) else p['params']
                return '#undef %s\n%s %s %s(%s); /* %08x */' % (name, p['ret'], cc, name, params, addr)
            ret = 'void *' if '*' in p['ret'] else ('void' if p['ret'] == 'void' else 'int')
            return '#undef %s\n%s %s %s(); /* %08x */' % (name, ret, cc, name, addr)
        narrow = re.search(r'\b(char|byte|uchar|sbyte|schar|undefined1|undefined2|short|ushort|word|bool|float|i8|u8|i16|u16)\b(?!\s*\*)', params)
        if cc == '__cdecl' and not narrow:
            # Unprototyped: tolerate callers passing a different argument count,
            # exactly like the original caller-cleans-up convention does.
            # (Not possible with sub-int parameters: those need a prototype.)
            params = ''
        return '%s %s %s(%s); /* %08x */' % (p['ret'], cc, name, params, addr)

    # Globals with a name and type of our own (re/symbols.tsv).
    named_globals = {}
    for r in load_tsv('symbols.tsv'):
        if r['kind'] == 'data':
            named_globals[r['name']] = (r['addr'], r['type'])

    # Globals: collect per-name types across all functions.
    gtypes = collections.defaultdict(collections.Counter)
    gaddr = {}
    func_names = {r['name'] for r in funcs_tsv}
    # Ghidra names string symbols after their contents, which may contain
    # characters that are not legal in C identifiers.  Sanitize uniformly.
    for f in funcs:
        bad = [g for g in f['globals'] if not re.match(r'^[A-Za-z_]\w*$', g)]
        for g in sorted(bad, key=len, reverse=True):
            clean = re.sub(r'\W', '_', g)
            f['body'] = f['body'].replace(g, clean)
            f['globals'][clean] = f['globals'].pop(g)
    for f in funcs:
        for gname in [g for g in f['globals'] if g in func_names]:
            del f['globals'][gname]   # functions referenced as values: declared in protos.h
        for gname, (ga, gt, gs) in f['globals'].items():
            gtypes[gname][gt] += 1
            gaddr[gname] = ga

    # Sizes of absolute memory accesses per function (tools/callsites.py).
    gacc = collections.defaultdict(dict)
    for r in load_tsv('gaccess.tsv'):
        fa, ga, sz = int(r['func'], 16), int(r['addr'], 16), int(r['size'])
        gacc[fa][ga] = max(gacc[fa].get(ga, 0), sz)

    # Functions that failed to compile in an earlier round (tools/build_recon.py --keep-going).
    failed = {}
    fpath = os.path.join(RE, 'compile_failures.tsv')
    if os.path.exists(fpath):
        for line in open(fpath):
            a, _, err = line.rstrip('\n').partition('\t')
            failed[int(a, 16)] = err

    # Unwind funclets called directly by their parent (MSVC /Od emits
    # `call $L` to destroy locals at scope exit): inline them.
    ehfunclets = {}
    for f in funcs:
        if f['addr'] < CRT_START and 'unaff_EBP' in f['body'] and int(tsv_by_addr[f['addr']]['size']) <= 48:
            ehfunclets[f['name']] = funclet_statements(f['body'])
    tslots = this_slots()

    # Names Ghidra gave to several functions (static helpers in different
    # CRT objects) cannot be linked by name; symbols the last build could not
    # resolve (re/unresolved.txt) make their users manual.
    name_count = collections.Counter(f['name'] for f in funcs)
    upath = os.path.join(RE, 'unresolved.txt')
    unresolved = open(upath).read().split() if os.path.exists(upath) else []
    unresolved_re = re.compile(r'\b(' + '|'.join(map(re.escape, unresolved)) + r')\b') if unresolved else None

    # Hand-written reconstructions (src/game/) replace the generated ones.
    overrides = {}
    override_protos = {}
    shared_types = set(re.findall(r'\}\s*(\w+);', open(os.path.join(ROOT, 'src', 'include', 'civ2types.h')).read()))
    shared_types |= {'i8', 'u8', 'i16', 'u16', 'i32', 'u32', 'FILE', 'LPSTR', 'LPCSTR', 'WCHAR', 'LCID', 'HWND', 'HDC',
                     'MMTIME', 'code', 'float10', 'size_t', 'wchar_t'}
    for path in sorted(glob.glob(os.path.join(ROOT, 'src', 'game', '**', '*.cpp'), recursive=True) +
                       glob.glob(os.path.join(ROOT, 'src', 'crt', '*.c'))):
        for m in re.finditer(r'^([A-Za-z_][^;{}]*?\b([A-Za-z_]\w*)\s*\([^;{}()]*\))\s*\{', open(path).read(), re.M):
            overrides[m.group(2)] = os.path.relpath(path, ROOT)
            head = ' '.join(m.group(1).split())
            idents = set(re.findall(r'[A-Za-z_]\w*', re.sub(r'\b\w+\s*(?=[,)\[])', '', head)))
            unknown = {i for i in idents if i not in KNOWN_TYPES and not i.isupper() and i not in shared_types
                       and i not in ('__cdecl', '__stdcall', '__thiscall', '__fastcall', 'struct', m.group(2))}
            if not re.search(r'\b(static|__attribute__|__asm__)\b', head) and not unknown:
                override_protos[m.group(2)] = head   # the hand-written definition is authoritative

    manifest = []
    blocks = collections.defaultdict(list)
    unref = set()                   # re/unreferenced.tsv: written to src/unused/, not compiled
    if os.path.exists(os.path.join(RE, 'unreferenced.tsv')):
        unref = {int(r['addr'], 16) for r in load_tsv('unreferenced.tsv')}
    for f in funcs:
        a = f['addr']
        status = 'auto'
        is_crt = CRT_START <= a < ASMGRAF_START
        if a >= ASMGRAF_START:
            status = 'asm'
        body = f['body']
        reason = ''
        if f['sig'] is None and status == 'auto':
            status, reason = 'manual', 'signature'
        if status == 'auto' and name_count[f['name']] > 1:
            status, reason = 'manual', 'duplicate name'
        if status == 'auto' and unresolved and unresolved_re.search(body):
            status, reason = 'manual', 'unresolved: ' + unresolved_re.search(body).group(0)
        if f['name'] in overrides:
            status, reason = 'game', overrides[f['name']]
        if f['sig'] and (conv.get(a, {}).get('cc') == '__thiscall' or f['sig']['cc'] == '__thiscall') \
                and not re.match(r'^\s*\w[\w\s\*]*\bthis\b', f['sig']['params']):
            # The decompiler drops an unused `this`; the ABI still passes it in ECX.
            params = f['sig']['params']
            newp = 'void *this' if params in ('', 'void') else 'void *this,' + params
            body = body.replace('%s(%s)' % (f['sig']['name'], params), '%s(%s)' % (f['sig']['name'], newp), 1)
            f['sig']['params'] = newp
        if ehfunclets and status == 'auto' and re.search(r'\bFUN_[0-9a-f]{8}\(\);', body):
            called = set(re.findall(r'\b(FUN_[0-9a-f]{8})\(\);', body)) & set(ehfunclets)
            if called:
                try:
                    body = inline_funclets(body, {k: ehfunclets[k] for k in called},
                                           tslots.get(a) if 'this' in (f['sig'] or {}).get('params', '') else None)
                except KeyError as e:
                    status, reason = 'manual', 'funclet frame ref ' + str(e)
        if status == 'auto' and re.search(r'\bstack0x[0-9a-f]+\b', body):
            # an object the decompiler saw only as an unnamed stack address
            status, reason = 'manual', 'unnamed stack object ' + re.search(r'\bstack0x[0-9a-f]+\b', body).group(0)
        if f['sig'] and not f['sig']['cc'] and conv.get(a, {}).get('cc'):
            # Ghidra omits the keyword for some functions; the prototype has it.
            cc = conv[a]['cc']
            body = re.sub(r'\b%s\(' % re.escape(f['sig']['name']), '%s %s(' % (cc, f['sig']['name']), body, count=1)
            f['sig']['cc'] = cc
        # __chkstk (0x5f35f0) takes the frame size in EAX, which C cannot pass;
        # the compiler probes large frames itself (___chkstk_ms).
        body = re.sub(r'^\s*(thunk_)?FUN_005f35f0\(\);\n', '', body, flags=re.M)
        body = strip_seh(body)
        body = thunk_symbols(body)
        body = fix_thisless_vcalls(body)
        body = fix_vtable_calls(body)
        body = fix_stdio_macros(body)
        body = fix_addr_literals(body, entry_names)
        body = fix_misc_names(body)
        body, frame_defs, frame_names = emulate_frame(body, f['sig']['name'] if f['sig'] else None)
        body, arg_names = emulate_args(body)
        frame_names = frame_names + arg_names
        if arg_names:
            contiguous_args.append(f['sig']['name'] if f['sig'] else '%08x' % a)
        body = fix_fields(body)
        body = body.replace('unaff_FS_OFFSET', 'seh_dummy_fs')
        m = UNSUPPORTED.search(body)
        if status == 'auto' and m:
            status, reason = 'manual', m.group(1)
            # C++ exception-unwind funclets: tiny bodies that run with the
            # parent's frame in EBP and only call destructors.  Civ2 never
            # throws, so these are unreachable and are not reconstructed.
            if reason == 'unaff_EBP' and int(tsv_by_addr[a]['size']) <= 48:
                status = 'ehfunclet'
        # Per-function overrides for globals whose type differs by function.
        local_defs = []
        for gname, (ga, gt, gs) in sorted(f['globals'].items()):
            if gname in named_globals:
                continue
            if re.search(r'\(\*%s\)\(' % re.escape(gname), body) and '*' in gt and 'code' not in gt:
                # Called through: a function pointer in this function's view.
                local_defs.append((gname, ga, 'code *'))
            elif len(gtypes[gname]) > 1:
                local_defs.append((gname, ga, gt))
        # String references without a symbol.
        for name, m in sorted(set(re.findall(r'\b([su]_\w*_(' + HEX + r'))\b', body))):
            if name not in f['globals']:
                local_defs.append((name, m, 'string'))
        # `_DAT_x`: an access wider than the symbol at x; size from the machine code.
        sized = {1: 'undefined1', 2: 'undefined2', 4: 'undefined4', 8: 'undefined8', 10: 'float10'}
        for name, m in sorted(set(re.findall(r'\b(_[A-Za-z][A-Za-z0-9]*_(?:[0-9A-Za-z]+_)*?(' + HEX + r'))\b', body))):
            if name in f['globals']:
                continue
            ga = int(m, 16)
            sz = gacc[a].get(ga, 4)
            local_defs.append((name, m, sized.get(sz, 'undefined4')))
        f['local_defs'] = local_defs
        f['frame_defs'] = frame_defs
        f['frame_names'] = frame_names
        if status in ('auto', 'crt') and a in failed:
            status, reason = 'manual', 'compile: ' + failed[a][:80]
        f['status'] = status
        f['reason'] = reason
        f['body_c'] = body
        if is_crt:
            # Statically linked MSVC 4.2 debug C runtime: translated like game
            # code (its heap layout and stdio behaviour are observable by the
            # game), but kept in separate crt_*.c files.
            status = {'auto': 'crt', 'manual': 'crt-manual'}.get(status, status)
            f['status'] = status
        if status == 'auto' and a in unref:
            status = 'unused'
            blocks[('unused', a >> 16)].append(f)
        elif status in ('auto', 'manual', 'game', 'crt', 'crt-manual'):
            blocks[('crt' if is_crt else 'blk', a >> 16)].append(f)
        manifest.append((a, f['name'], status, reason))

    # protos.h
    with open(os.path.join(OUT, 'protos.h'), 'w') as h:
        h.write('/* Generated by tools/gen_c.py: prototypes of every original function. */\n')
        h.write('#ifndef CIV2_AUTO_PROTOS_H\n#define CIV2_AUTO_PROTOS_H\n\n')
        for f in funcs:
            a = f['addr']
            p = f['sig']
            if not p:
                continue
            cc = conv.get(a, {}).get('cc') if a in conv else None
            if f['name'] in CRT_NATIVE_SIGS:
                h.write('#ifdef CIV2_NATIVE\n%s; /* %08x, C runtime: src/native/crt.cpp */\n#else\n' % (
                    CRT_NATIVE_SIGS[f['name']], a))
            if f['name'] in override_protos:
                h.write('#undef %s\n%s; /* %08x, hand-written */\n' % (f['name'], override_protos[f['name']], a))
            else:
                h.write(proto_decl(f['name'], p, a, cc) + '\n')
            if f['name'] in CRT_NATIVE_SIGS:
                h.write('#endif\n')
        for tname, (ta, target) in sorted(thunks.items()):
            tf = by_addr.get(target)
            if tf and tf['sig']:
                cc = conv.get(target, {}).get('cc') if target in conv else None
                h.write(proto_decl(tname, tf['sig'], ta, cc) + '\n')
        h.write('\n#endif\n')

    # globals.h: one macro per unnamed global with a single consistent type.
    with open(os.path.join(OUT, 'globals.h'), 'w') as h:
        h.write('/* Generated by tools/gen_c.py: globals of the original image, by address. */\n')
        h.write('#ifndef CIV2_AUTO_GLOBALS_H\n#define CIV2_AUTO_GLOBALS_H\n\n')
        # Named globals (re/symbols.tsv): real declarations.  The hook build
        # binds them to their original addresses at link time.
        h.write('/* Named globals: see src/data/gamedata.h. */\n')
        h.write('\n/* Not yet named: by address. */\n')
        for gname in sorted(gtypes):
            if gname in named_globals:
                continue
            if len(gtypes[gname]) != 1:
                continue
            gt = c_type_of_global(next(iter(gtypes[gname])))
            ga = gaddr[gname]
            if ga == '?':
                continue
            h.write(global_macro(gname, ga, gt) + '\n')
        # Addresses still used by hand-written code (src/game, src/crt) but no
        # longer by generated code keep a macro, typed as Ghidra last saw them.
        defined = {g for g in gtypes if len(gtypes[g]) == 1} | set(named_globals)
        wanted = set()
        # Globals named in re/symbols.tsv (kind var) that hand-written code uses.
        var_names = {r['name']: r['addr'] for r in load_tsv('symbols.tsv') if r['kind'] == 'var'}
        for path in glob.glob(os.path.join(ROOT, 'src', 'game', '*.cpp')) + glob.glob(os.path.join(ROOT, 'src', 'crt', '*.c')):
            src_text = open(path).read()
            wanted |= set(re.findall(r'\b(_?(?:DAT|PTR)_[0-9a-f]{8}|[su]_\w*?_[0-9a-f]{8})\b', src_text))
            wanted |= {n for n in re.findall(r'\b[a-z_][a-z0-9_]*\b', src_text) if n in var_names}
            wanted |= set(re.findall(r'&(LAB_[0-9a-f]{8})\b', src_text))   # code addresses (not goto labels)
        legacy = sorted(wanted - defined)
        if legacy:
            h.write('\n/* Used only by hand-written code. */\n')
            for gname in legacy:
                gt = c_type_of_global(next(iter(gtypes[gname]))) if gname in gtypes else \
                    ('string' if gname.startswith(('s_', 'u_')) else 'undefined4')
                h.write('#ifndef %s\n%s\n#endif\n' % (gname, global_macro(gname, var_names.get(gname, gname[-8:]), gt)))
        h.write('\n#endif\n')

    for path in glob.glob(os.path.join(UNUSED, 'blk_*.c')):
        os.remove(path)
    for (kind, blk), fs in sorted(blocks.items()):
        if kind == 'unused':
            os.makedirs(UNUSED, exist_ok=True)
            path = os.path.join(UNUSED, 'blk_%04x.c' % blk)
        else:
            path = os.path.join(OUT, '%s_%04x.c' % (kind, blk))
        with open(path, 'w') as c:
            what = 'C runtime functions' if kind == 'crt' else 'functions'
            if kind == 'unused':
                what = 'functions nothing in the game refers to (re/unreferenced.tsv; not compiled)'
            c.write('/* Generated by tools/gen_c.py from Ghidra output: %s at 0x%04x0000-0x%04xffff. */\n' % (what, blk, blk))
            c.write('#include "auto_common.h"\n\n')
            for f in fs:
                c.write('/* %08x %s */\n' % (f['addr'], f['status'] + (' (%s)' % f['reason'] if f['reason'] else '')))
                if f['status'] not in ('auto', 'crt', 'unused'):
                    c.write('#if 0\n')
                for gname, ga, gt in f['local_defs']:
                    c.write(global_macro(gname, ga, c_type_of_global(gt)) + '\n')
                for d in f['frame_defs']:
                    c.write(d + '\n')
                body = f['body_c']
                # a label must label a statement (before C23; older compilers)
                c.write(re.sub(r'^(\s*(?:[A-Za-z_]\w*|case [^:\n]+|default)\s*:)[ \t]*\n(\s*\})', r'\1 ;\n\2', body,
                               flags=re.M))
                for gname, ga, gt in f['local_defs']:
                    c.write('#undef %s\n' % gname)
                for n in f['frame_names']:
                    c.write('#undef %s\n' % n)
                if f['status'] not in ('auto', 'crt', 'unused'):
                    c.write('#endif\n')
                c.write('\n')

    with open(os.path.join(RE, 'manifest.tsv'), 'w') as m:
        m.write('addr\tname\tstatus\treason\n')
        for a, n, s, r in manifest:
            m.write('%08x\t%s\t%s\t%s\n' % (a, n, s, r))
    cnt = collections.Counter(s for _, _, s, _ in manifest)
    reasons = collections.Counter(r for _, _, s, r in manifest if s == 'manual')
    print(dict(cnt))
    print(reasons.most_common(15))
    # The x86 stack layout the 1997 code relies on, emulated (emulate_args):
    # listed so a new case is noticed.
    print('arguments made contiguous (emulate_args): %d functions' % len(contiguous_args))
    with open(os.path.join(RE, 'contiguous_args.txt'), 'w') as fh:
        fh.write(''.join(n + '\n' for n in sorted(contiguous_args)))


_FIELDS = None


def gen_data_overhang():
    sys.path.insert(0, os.path.join(ROOT, 'tools'))
    import gen_data
    return gen_data.OVERHANG


_OVERHANG = None


def _overhang():
    """name -> (start, end, length) of the extra elements of overhanging arrays."""
    global _OVERHANG
    if _OVERHANG is None:
        _OVERHANG = {}
        sizes = {}
        for r in load_tsv('data_fields.tsv'):
            if r['kind'] == 'named':
                sizes[r['field']] = (int(r['start'], 16), int(r['end'], 16))
        types = {r['name']: r['type'] for r in load_tsv('symbols.tsv') if r['kind'] == 'data'}
        for n, extra in gen_data_overhang().items():
            start, end = sizes[n]
            ln = int(re.search(r'\[(\w+)\]', types[n]).group(1), 0)
            elem = (end - start) // ln
            _OVERHANG[n] = (end, end + elem * extra, ln)
    return _OVERHANG


def data_location(ga):
    """C expression for the address of original data address ga: a path into
    the GameData struct `g` (src/data/gamedata.h), or the absolute address for
    memory outside it (code labels, import table)."""
    global _FIELDS
    if _FIELDS is None:
        _FIELDS = []
        path = os.path.join(RE, 'data_fields.tsv')
        if os.path.exists(path):
            for r in load_tsv('data_fields.tsv'):
                # Named globals are symbols of their own; the rest are fields of g.
                _FIELDS.append((int(r['start'], 16), int(r['end'], 16),
                                r['field'] if r['kind'] == 'named' else 'g.' + r['field'], r['kind']))
        _FIELDS.sort()
    import bisect
    # Data past the end of a named array that the game reaches by indexing
    # it (gen_data.OVERHANG): addressed through the array.
    for n, (start, end, ln) in _overhang().items():
        if start <= ga < end:
            k, off = divmod(ga - start, (end - start) // gen_data_overhang()[n])
            field = '%s[%d]' % (n, ln + k)
            return '((u8 *)&%s + %d)' % (field, off) if off else '((u8 *)&%s)' % field
    i = bisect.bisect_right(_FIELDS, (ga, 1 << 40, '', '')) - 1
    if i >= 0 and _FIELDS[i][0] <= ga < _FIELDS[i][1]:
        start, end, field, kind = _FIELDS[i]
        if kind == 'ptr' and end - start > 4:
            # An array of address slots (pointer-sized in native builds).
            k, rem = divmod(ga - start, 4)
            field = '%s[%d]' % (field, k)
            start += 4 * k
        return '((u8 *)&%s + %d)' % (field, ga - start) if ga != start else '((u8 *)&%s)' % field
    return '0x%08x' % ga


def global_macro(gname, ga, gt):
    ga = int(ga, 16)
    loc = data_location(ga)
    if gt.endswith(']'):
        # Array: the name denotes the array itself.
        base, dims = gt.split('[', 1)
        return '#define %s (*(%s (*)[%s)%s)' % (gname, base.strip(), dims, loc)
    if gname.startswith(('s_', 'u_')) and gt.startswith(('char', 'string', 'wchar')):
        return '#define %s ((char *)%s)' % (gname, loc)
    if gt in ('string', 'unicode', 'TerminatedCString'):
        return '#define %s ((char *)%s)' % (gname, loc)
    if gt.startswith('LAB') or gname.startswith('LAB_'):
        return '#define %s (*(undefined1 *)%s)' % (gname, loc)
    return '#define %s (*(%s *)%s)' % (gname, gt, loc)


def apply_type_names():
    """Readable class and field names (re/type_names.tsv, re/field_names.tsv)
    in everything written."""
    import type_names
    if not type_names.mapping() and not type_names.fields():
        return
    for path in glob.glob(os.path.join(OUT, '*.[ch]')) + glob.glob(os.path.join(UNUSED, '*.c')):
        t = open(path).read()
        n = type_names.apply_code(type_names.apply(t))
        if n != t:
            open(path, 'w').write(n)


def apply_struct_access():
    """Address arithmetic into the arrays of structs as field access
    (tools/struct_access.py)."""
    import struct_access
    for path in glob.glob(os.path.join(OUT, 'blk_*.c')) + glob.glob(os.path.join(UNUSED, 'blk_*.c')):
        t = open(path).read()
        n = struct_access.apply(t)
        if n != t:
            open(path, 'w').write(n)


def apply_tidy_expr():
    """Decompiler idioms as C is usually written (tools/tidy_expr.py)."""
    import tidy_expr
    for path in glob.glob(os.path.join(OUT, 'blk_*.c')) + glob.glob(os.path.join(UNUSED, 'blk_*.c')):
        t = open(path).read()
        n = tidy_expr.apply(t)
        if n != t:
            open(path, 'w').write(n)


def apply_direct_calls():
    """Functions called by their own names, not through the original's
    incremental-link thunks (tools/direct_calls.py)."""
    import direct_calls
    for path in glob.glob(os.path.join(OUT, 'blk_*.c')) + glob.glob(os.path.join(UNUSED, 'blk_*.c')):
        t = open(path).read()
        n = direct_calls.apply(t)
        if n != t:
            open(path, 'w').write(n)


if __name__ == '__main__' and not os.environ.get('CIV2_REGENERATE'):
    sys.exit('tools/gen_c.py: src/auto is edited directly now and would be overwritten; '
             'set CIV2_REGENERATE=1 to regenerate anyway (into a scratch checkout)')
if __name__ == '__main__':
    main()
    apply_type_names()
    apply_struct_access()
    apply_tidy_expr()
    apply_direct_calls()
    import name_locals
    print('%d parameters and locals named (tools/name_locals.py)' % name_locals.apply_all())
