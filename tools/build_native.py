#!/usr/bin/env python3
"""Build the game as a native program from source (no original machine code
or memory layout).  Runs inside the civ2-dev container.

Targets:
  linux   native Linux (x86_64 or any 64-bit), Windows API replaced by the
          SDL2 platform layer in src/platform/
  win64   x86_64 Windows (mingw-w64), using the Windows API directly: checks
          that the reconstructed code is 64-bit clean before the platform
          layer is replaced.
Generated helpers (build/native/gen/):
  thunks.h     incremental-link thunk names -> the functions they jump to
  inits.c      the C and C++ static initializer tables the MSVC runtime ran
  linux32rec, linux32test   the unit tests' recorder and runner (tests/unit_*.c)
Usage: build_native.py [--target win64] [--jobs-limit]
"""
import argparse
import concurrent.futures
import csv
import glob
import os
import re
import subprocess
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402  (the game's directory)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'build', 'native')
GEN = os.path.join(OUT, 'gen')

TARGETS = {
    'linux32': {
        'cc': 'gcc',
        'exe': 'civ2-linux32',
        'cflags': ['-m32'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie'],
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    'linux32cov': {         # coverage of the test runs: gcov data in build/native/linux32cov
        'cc': 'gcc',
        'exe': 'civ2-linux32cov',
        'cflags': ['-m32', '--coverage'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie', '--coverage'],
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    'linux32gc': {          # which functions nothing refers to: build/native/linux32gc/unreferenced.txt
        'cc': 'gcc',
        'exe': 'civ2-linux32gc',
        'cflags': ['-m32', '-ffunction-sections'],
        'ldflags': ['-m32', '-no-pie', '-Wl,--gc-sections', '-Wl,--print-gc-sections'],
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    'linux32asan': {        # AddressSanitizer: code that relies on locals or globals being adjacent
        'cc': 'gcc',
        'exe': 'civ2-linux32asan',
        'cflags': ['-m32', '-fsanitize=address', '-fsanitize-recover=address', '-fno-omit-frame-pointer'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie', '-fsanitize=address'],
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    # The unit tests (tests/unit_record.c, tests/unit_run.c; docs/DEVELOPMENT.md):
    # the game's data at a fixed address in both.
    'linux32rec': {         # records cases of the game's functions while a replay runs
        'cc': 'gcc',
        'exe': 'civ2-linux32rec',
        'cflags': ['-m32', '-DCIV2_FIXED_DATA', '-DCIV2_UNITREC', '-fno-omit-frame-pointer'],
        'instrument': ['-finstrument-functions', '-finstrument-functions-exclude-file-list=scoped.h,/usr/'],
        'extra': ['tests/unit_record.c'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie', '-Wl,--section-start=civ2data=0x30000000'],
        'libs': ['-lSDL2', '-lm', '-ldl', '-lz'],
    },
    'linux32test': {        # runs the recorded cases: make test
        'cc': 'gcc',
        'exe': 'civ2-unittest',
        'cflags': ['-m32', '-DCIV2_FIXED_DATA', '-DCIV2_UNITTEST'],
        # as the recording build: gcc passes the arguments of a static
        # function whose address is not taken in registers, and the
        # instrumentation takes every function's address (tests/unit_run.c
        # has empty hooks)
        'instrument': ['-finstrument-functions', '-finstrument-functions-exclude-file-list=scoped.h,/usr/'],
        'extra': ['tests/unit_run.c'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie', '-Wl,--section-start=civ2data=0x30000000'],
        'libs': ['-lSDL2', '-lm', '-ldl', '-lz'],
    },
    'linux32clang': {       # cross-check: the linux32 build compiled by clang (needs clang installed)
        'cc': 'clang',
        'exe': 'civ2-linux32clang',
        'cflags': ['-m32', '-Wno-error=int-conversion', '-Wno-error=incompatible-pointer-types',
                   '-Wno-error=implicit-function-declaration', '-Wno-error=implicit-int', '-Wno-error=return-type',
                   '-Wno-error=incompatible-function-pointer-types'],
        'ldflags': ['-m32', '-rdynamic', '-no-pie'],
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    'linux': {
        'cc': 'gcc',
        'exe': 'civ2',
        'ldflags': ['-rdynamic', '-no-pie'],   # game symbols name the callers in rand traces
        'libs': ['-lSDL2', '-lm', '-ldl'],
    },
    'wasm': {
        # The portable build: the game and the platform layer as a wasm32
        # module (the original's 32-bit data model), turned into C by
        # wasm2c and linked with the host runtime (tools/build_port.py).
        'cc': os.path.join(paths.WASI_SDK, 'bin', 'clang'),
        'exe': 'wasm/civ2.wasm',
        'cflags': ['--target=wasm32-wasip1', '-D_WASI_EMULATED_PROCESS_CLOCKS',
                   # what gcc accepts in the decompiled code, clang by default not
                   '-Wno-error=int-conversion', '-Wno-error=incompatible-pointer-types',
                   '-Wno-error=implicit-function-declaration', '-Wno-error=implicit-int',
                   '-Wno-error=return-type', '-Wno-error=incompatible-function-pointer-types'],
        'ldflags': ['--target=wasm32-wasip1', '-Wl,--error-limit=0', '-Wl,-z,stack-size=16777216',
                    # nothing below 64K, as on Windows: smaller pointers are taken for atoms and resource ids
                    '-Wl,--global-base=65536', '-Wl,--initial-memory=67108864',
                    '-Wl,--export=malloc', '-Wl,--export=plat_modal_tick'],
        'libs': ['-lwasi-emulated-process-clocks'],
    },
    'win64': {
        'cc': 'x86_64-w64-mingw32-gcc',
        'exe': 'civ2_win64.exe',
        'ldflags': ['-mwindows', '-municode'] [:1] + ['-static-libgcc'],
        'libs': ['-lwinmm', '-lcomdlg32', '-lgdi32', '-luser32', '-lkernel32', '-ladvapi32', '-lddraw',
                 '-lavifil32', '-lmsvfw32', '-lcomctl32', '-lole32'],
    },
}
CFLAGS = ['-std=gnu89', '-O1', '-g', '-w', '-fno-strict-aliasing', '-fwrapv', '-fno-delete-null-pointer-checks',
          '-fno-inline', '-include', 'native.h',
          '-I', os.path.join(ROOT, 'src', 'include'), '-I', os.path.join(ROOT, 'src', 'auto'),
          '-I', os.path.join(ROOT, 'tools'), '-I', GEN, '-DCIV2_NATIVE', '-fno-pie']
if os.environ.get('CIV2_LOWMEM', '1') == '1':
    CFLAGS.append('-DCIV2_LOWMEM')      # src/platform/main.c: addresses below 2 GB (development aid)


def functions():
    return list(csv.DictReader(open(os.path.join(ROOT, 're', 'functions.tsv')), delimiter='\t'))


def gen_thunks(rows):
    """Thunk names become macros for their targets.  A target defined
    without parameters is called through an unprototyped pointer (the
    decompiler may not have seen parameters callers pass)."""
    names = {r['entry']: r['name'] for r in rows}
    noparams = {}
    for m in re.finditer(r'^(\w[\w\s*]*?)\s*\b(?:__\w+\s+)?(\w+)\((void)?\);', open(os.path.join(
            ROOT, 'src', 'auto', 'protos.h')).read(), re.M):
        noparams[m.group(2)] = m.group(1).strip()
    lines = ['/* Generated by tools/build_native.py: incremental-link thunk names',
             ' * refer to the function they jump to. */', '#ifndef CIV2_THUNKS_H', '#define CIV2_THUNKS_H']
    for r in rows:
        t = r['thunk_target']
        if t and not t.startswith('EXT') and t in names and r['name'] != names[t] \
                and re.match(r'^[A-Za-z_]\w*$', r['name']) and re.match(r'^[A-Za-z_]\w*$', names[t]):
            target = names[t]
            if target in noparams:
                lines.append('#define %s ((%s (*)())%s)' % (r['name'], noparams[target], target))
            else:
                lines.append('#define %s %s' % (r['name'], target))
    lines += ['#endif', '']
    open(os.path.join(GEN, 'thunks.h'), 'w').write('\n'.join(lines))


def gen_inits(rows):
    """The runtime's initializer tables: C at 0x6246ac..0x6248bc, C++ at
    0x624000..0x6245a8 (function addresses in .data, run in order)."""
    import json
    # The original's sections as extracted into data/image (no civ2.exe needed).
    layout = json.load(open(os.path.join(ROOT, 'data', 'image', 'layout.json')))
    base = layout['image_base']
    sections = [(base + sec['va'], open(os.path.join(ROOT, sec['file']), 'rb').read()) for sec in layout['sections']]

    def dword(a):
        for va, data in sections:
            if va <= a < va + len(data):
                return int.from_bytes(data[a - va:a - va + 4], 'little')
        return 0
    by_addr = {int(r['entry'], 16): r['name'] for r in rows}
    thunk_target = {int(r['entry'], 16): int(r['thunk_target'], 16) for r in rows
                    if r['thunk_target'] and not r['thunk_target'].startswith('EXT')}
    out = ['/* Generated by tools/build_native.py: static initializers, in the order',
           ' * the original C runtime ran them. */', '#include "auto_common.h"', '']
    for label, lo, hi in (('civ2_c_inits', 0x6246ac, 0x6248bc), ('civ2_cpp_inits', 0x624000, 0x6245a8)):
        fns = []
        for a in range(lo, hi, 4):
            v = dword(a)
            if not v:
                continue
            v = thunk_target.get(v, v)
            if v in by_addr and v < 0x5f1b80:     # game initializers (the runtime's own are not needed)
                fns.append(by_addr[v])
        out.append('EXTERN_C void (*const %s[])(void) = {' % label)
        out += ['    (void (*)(void))%s,' % f for f in fns]
        out += ['    0', '};', '']
    open(os.path.join(GEN, 'inits.cpp'), 'w').write('\n'.join(out))


def gen_layout():
    """gamedata_check_layout(): whether the game's data lies as in the
    original (the address comments of src/data/gamedata.cpp), for
    src/platform/main.c."""
    rows = []
    for line in open(os.path.join(ROOT, 'src', 'data', 'gamedata.cpp')):
        m = re.match(r'GAMEDATA\(\d+\) [^=;\[]*?\b(\w+)\s*[\[=;].*/\* ([0-9a-f]{8}) \*/$', line)
        if m:
            rows.append((m.group(1), int(m.group(2), 16)))
    out = ['/* Generated by tools/build_native.py from src/data/gamedata.cpp. */',
           '#include <stdio.h>', '#include "auto_common.h"', '',
           'static const struct { const void *at; u32 addr; } layout[] = {']
    out += ['    {&%s, 0x%08x},' % (n, a) for n, a in rows]
    out += ['};', '',
            '/* Reports where the data does not lie as in the original; returns how',
            '   many variables are out of place. */',
            'EXTERN_C int gamedata_check_layout(void)',
            '{',
            '    const char *base = (const char *)layout[0].at;',
            '    int i, bad = 0;',
            '',
            '    for (i = 1; i < (int)(sizeof layout / sizeof layout[0]); i++)',
            '        if ((const char *)layout[i].at - base != (long)(layout[i].addr - layout[0].addr) && bad++ == 0)',
            '            fprintf(stderr, "civ2: the game data is not laid out as in the original from %08x on\\n",',
            '                    (unsigned)layout[i].addr);',
            '    return bad;',
            '}', '']
    open(os.path.join(GEN, 'layout.cpp'), 'w').write('\n'.join(out))


def gen_calltable():
    """Game functions the replay scripts run with `call NAME ADDR ...`
    (harness/*/*.txt): a table by name for src/platform/replay.c."""
    names = set()
    for path in glob.glob(os.path.join(ROOT, 'harness', '*', '*.txt')):
        for line in open(path, errors='replace'):
            m = re.match(r'call\s+([A-Za-z_]\w*)\s', line)
            if m:
                names.add(m.group(1))
    out = ['/* Generated by tools/build_native.py: functions replay scripts call. */',
           '#include "auto_common.h"', '',
           'EXTERN_C const struct ReplayCall { const char *name; void (*fn)(void); } replay_calltable[] = {']
    out += ['    {"%s", (void (*)(void))%s},' % (n, n) for n in sorted(names)]
    out += ['    {0, 0}', '};', '']
    open(os.path.join(GEN, 'calltable.cpp'), 'w').write('\n'.join(out))


# The game (not the platform layer, the host or third-party code) is C++.
# CIV2_CXX=permissive: g++ -fpermissive, which converts between integers and
# pointers as C does (objects in build/native/TARGET-cxxp, the program with
# -cxxp in its name): a reference to compare a change with, function by
# function (tools/cxx_compare.py).
CXX_MODE = os.environ.get('CIV2_CXX', 'strict')
CXX = True
CXX_SUFFIX = '-cxxp' if CXX_MODE == 'permissive' else ''
GAME_DIRS = [os.path.join(ROOT, 'src', d) + os.sep for d in ('auto', 'game', 'data', 'native')] + [GEN + os.sep]


def is_game(src):
    return any(src.startswith(d) for d in GAME_DIRS)


def cxx_of(cc):
    """The C++ driver that goes with a C compiler."""
    if cc.endswith('gcc'):
        return cc[:-3] + 'g++'
    if cc.endswith('clang'):
        return cc + '++'
    return cc


def compile_one(job):
    cc, src, o, stale, tflags, instrument = job
    if instrument and (('/src/platform/' in src and not src.endswith('/aaa_begin.c')) or
                       (is_game(src) and '/src/native/' not in src)):
        # the recording build: the game's and the platform's functions call
        # tests/unit_record.c on entry and exit
        tflags = tflags + instrument
    if stale:
        flags, extra = CFLAGS, []
        lang = []
        if CXX and is_game(src):
            cc = cxx_of(cc)
            flags = ['-std=gnu++17' if f == '-std=gnu89' else f for f in CFLAGS]
            lang = ['-x', 'c++']
            extra = ['-fno-exceptions', '-fno-rtti'] + (['-fpermissive'] if CXX_MODE == 'permissive' else [])
        if '/src/platform/' in src or '/tests/' in src:
            flags = [f for f in CFLAGS if f != '-w']
            # The platform layer is ordinary portable C, with warnings.
            extra = ['-Wall', '-Wno-unused-function', '-std=gnu99', '-D_GNU_SOURCE']
        elif src.endswith('/src/data/gamedata.cpp') and 'clang' not in cc:
            # the data in the original's order (it is defined in address order)
            extra = extra + ['-fno-toplevel-reorder']
        elif '/src/third_party/' in src:
            # Third-party C (src/third_party/*/), built as its authors' code.
            flags = [f for f in CFLAGS if not f.startswith('-std') and f not in ('-O1',)]
            extra = ['-std=gnu99', '-O2', '-I', os.path.dirname(src)]
        elif '/src/host/' in src:
            # The host interface over SDL2: plain C, none of the game's headers.
            flags = []
            extra = subprocess.run(['sdl2-config', '--cflags'], capture_output=True, text=True).stdout.split()
            extra += ['-O1', '-g', '-Wall', '-std=gnu99', '-fno-pie']
        r = subprocess.run([cc, '-c', '-o', o] + lang + [src] + tflags + flags + extra, capture_output=True, text=True)
        if r.returncode:
            return o, r.stderr
    return o, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--target', default='linux', choices=sorted(TARGETS))
    args = ap.parse_args()
    t = TARGETS[args.target]
    os.makedirs(GEN, exist_ok=True)
    rows = functions()
    gen_thunks(rows)
    gen_inits(rows)
    gen_calltable()
    gen_layout()
    sources = sorted(glob.glob(os.path.join(ROOT, 'src', 'auto', '*.cpp')) +
                     glob.glob(os.path.join(ROOT, 'src', 'game', '*.cpp')) +
                     [os.path.join(ROOT, 'src', 'data', 'gamedata.cpp'), os.path.join(GEN, 'inits.cpp'), os.path.join(GEN, 'layout.cpp'),
                      os.path.join(GEN, 'calltable.cpp')] +
                     glob.glob(os.path.join(ROOT, 'src', 'native', '*.cpp')))
    objdir = os.path.join(OUT, args.target + CXX_SUFFIX)
    os.makedirs(objdir, exist_ok=True)
    if args.target.startswith('linux') or args.target == 'wasm':
        sources += sorted(glob.glob(os.path.join(ROOT, 'src', 'platform', '*.c')))     # aaa_begin.c first
        # The Indeo 4 decoder (FFmpeg's, LGPL): indeo4.c is included by iv41.c.
        sources += [os.path.join(ROOT, 'src', 'third_party', 'indeo4', f + '.c')
                    for f in ('iv41', 'ivi', 'ivi_dsp', 'vlc', 'reverse', 'zigzag')]
    if args.target.startswith('linux'):
        sources += [os.path.join(ROOT, 'src', 'host', f) for f in ('host_sdl.c', 'host_net.c')]
        # The Ogg Vorbis decoder (stb_vorbis, public domain / MIT): the music, which the host plays.
        sources.append(os.path.join(ROOT, 'src', 'third_party', 'stb_vorbis', 'stb_vorbis.c'))
    sources += [os.path.join(ROOT, f) for f in t.get('extra', [])]
    headers = max(os.path.getmtime(h) for h in glob.glob(os.path.join(ROOT, 'src', '*', '*.h')))
    jobs = []
    tflags = t.get('cflags', [])
    for src in sources:
        o = os.path.join(objdir, os.path.relpath(src, ROOT).replace('/', '_') + '.o')
        newest = max(os.path.getmtime(src), headers)
        if '/tests/' in src:
            newest = max([newest] + [os.path.getmtime(h) for h in glob.glob(os.path.join(ROOT, 'tests', '*.h'))])
        stale = not os.path.exists(o) or os.path.getmtime(o) < newest
        jobs.append((t['cc'], src, o, stale, tflags, t.get('instrument')))
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        results = list(ex.map(compile_one, jobs))
    objs = [o for o, err in results if err is None]
    failed = [(o, err) for o, err in results if err is not None]
    for o, err in failed:
        sys.stderr.write(err[-2000:])
    if failed:
        raise SystemExit('%d files failed to compile' % len(failed))
    exe = os.path.join(ROOT, 'build', t['exe'])
    if CXX:
        exe = re.sub(r'(\.\w+)?$', lambda m: CXX_SUFFIX + (m.group(1) or ''), exe, count=1)
    os.makedirs(os.path.dirname(exe), exist_ok=True)
    r = subprocess.run([cxx_of(t['cc']) if CXX else t['cc'], '-o', exe] + objs + t['ldflags'] + t['libs'], capture_output=True, text=True)
    undefined = sorted(set(re.findall(r"undefined reference to `([^']+)'", r.stderr) +
                           re.findall(r"undefined symbol: (\S+)", r.stderr)))
    if r.returncode:
        with open(os.path.join(OUT, 'undefined.txt'), 'w') as f:
            f.write('\n'.join(undefined) + '\n')
        print('link failed: %d undefined symbols (build/native/undefined.txt): %s' % (len(undefined), ' '.join(undefined[:30])))
        if not undefined:
            sys.stderr.write(r.stderr[-3000:])
        raise SystemExit(1)
    gc = re.findall(r"removing unused section '\.text\.([^']+)' in file '([^']+)'", r.stderr)
    if gc:
        with open(os.path.join(os.path.dirname(objs[0]), 'unreferenced.txt'), 'w') as f:
            f.write(''.join('%s\t%s\n' % (n, os.path.basename(o)) for n, o in sorted(gc)))
        print('%d functions nothing refers to (unreferenced.txt)' % len(gc))
    # wasm: a call whose prototype differs from the callee's definition is
    # linked to a trap -- fatal here rather than when the game gets there.
    mismatch = sorted(set(re.findall(r'function signature mismatch: (\S+)', r.stderr)))
    if mismatch:
        sys.stderr.write(r.stderr[-6000:])
        raise SystemExit('calls that do not match their callee: %s' % ' '.join(mismatch))
    print('built %s' % os.path.relpath(exe, ROOT))


if __name__ == '__main__':
    main()
