#!/usr/bin/env python3
"""The portable build: the game and the platform layer compiled to a
wasm32 module (build_native.py --target wasm), turned into C by wasm2c,
then compiled for the machine at hand with the host runtime (src/host:
SDL2 and WASI) -- a native program for any 64-bit (or 32-bit) system,
keeping the original's 32-bit data model.

Steps (each can run on its own):
  module   build/wasm/civ2.wasm            (needs wasi-sdk and the installed game)
  c        build/wasm/c/civ2_wasm*.c,.h    (needs wabt's wasm2c)
  exe      build/OpenCiv2                 (needs a C compiler, SDL2, and unshield's,
                                           zlib's, libogg's and libvorbis's sources; on
                                           Windows (MSYS2) OpenCiv2.exe and SDL2.dll)
  exe-windows  build/OpenCiv2.exe         cross-compiled with MinGW-w64 (e.g. on
                                           Linux; tools/fetch-toolchain.sh windows)
  pack     dist/OpenCiv2-src.tar.gz       what the exe step needs on another
                                           machine, plus the data and run script

The c step's output is plain C: copy build/wasm/c and src/host to another
machine and run only the exe step there (the pack step bundles it).

Usage: build_port.py [module] [c] [exe] [exe-windows] [pack]   (default: module c exe)
What the steps need from tools/fetch-toolchain.sh (wasi-sdk, wabt, unshield's,
zlib's, libogg's and libvorbis's sources, SDL2's MinGW release) is downloaded first where missing
(or set WASI_SDK and WABT; see tools/paths.py).  Environment: CC (default cc; on macOS the
Command Line Tools' clang), CFLAGS, LDFLAGS, SDL2_CONFIG (an sdl2-config),
SDL2_STATIC=1 (SDL2 linked in), JOBS.
"""
import concurrent.futures
import glob
import os
import re
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

ROOT = paths.ROOT
WASM = os.path.join(ROOT, 'build', 'wasm')
CDIR = os.path.join(WASM, 'c')
OBJ = os.path.join(WASM, 'obj')
PARTS = 8


def run(cmd, **kw):
    print(' '.join(cmd) if len(cmd) < 12 else ' '.join(cmd[:10]) + ' ...', flush=True)
    subprocess.run(cmd, check=True, **kw)


def module():
    run([sys.executable, os.path.join(ROOT, 'tools', 'build_native.py'), '--target', 'wasm'])


def to_c():
    os.makedirs(CDIR, exist_ok=True)
    for f in glob.glob(os.path.join(CDIR, 'civ2_wasm*')):
        os.remove(f)
    wasm2c = os.path.join(paths.WABT, 'bin', 'wasm2c')
    if not os.path.exists(wasm2c) and os.path.exists(wasm2c + '.exe'):
        wasm2c += '.exe'
    run([wasm2c if os.path.exists(wasm2c) else 'wasm2c', os.path.join(WASM, 'civ2.wasm'), '-n', 'civ2', '--num-outputs=%d' % PARTS,
         '-o', os.path.join(CDIR, 'civ2_wasm.c')])
    # Calls clang could only make by casting a function to another type
    # become traps; the code must not have any.
    bad = sorted(set(re.findall(r'w2c_civ2_0x2EL(\w+)_bitcast_invalid', ''.join(
        open(f).read() for f in glob.glob(os.path.join(CDIR, 'civ2_wasm_*.c'))))))
    if bad:
        sys.exit('calls through casts to incompatible types (trap in wasm): %s' % ' '.join(bad))
    # Indirect calls: the decompiled code calls through function pointers
    # of other types than their targets (as the original freely did on x86,
    # e.g. constructors returning `this` called as returning nothing).  wasm
    # traps on that; here such calls go ahead, with the host ABI's
    # tolerance, like the original's.  Table bounds are still checked.
    impl = os.path.join(CDIR, 'civ2_wasm-impl.h')
    text = open(impl).read()
    old = '(LIKELY((x) < table.size && table.data[x].func &&      \\\n          func_types_eq(ft, table.data[x].func_type)) || \\\n'
    new = '(LIKELY((x) < table.size && table.data[x].func) ||      \\\n'
    if old not in text:
        sys.exit('wasm2c output: CHECK_CALL_INDIRECT not found as expected')
    open(impl, 'w').write(text.replace(old, new))
    # the runtime wasm2c's output needs, so the directory is self-contained
    rt = os.path.join(paths.WABT, 'share', 'wabt', 'wasm2c')
    inc = os.path.join(paths.WABT, 'include')
    for f in glob.glob(os.path.join(rt, '*.[ch]')) + glob.glob(os.path.join(rt, '*.inc')) + glob.glob(os.path.join(inc, 'wasm-rt*.h')):
        shutil.copy(f, CDIR)


def default_cc():
    """cc; on macOS Apple's clang driver (/usr/bin/clang) using the Command
    Line Tools when they are installed, so that compiler, linker and SDK
    match (an older Xcode's linker fails on a newer SDK with "unknown
    architecture" in .tbd files)."""
    clt = '/Library/Developer/CommandLineTools'
    if sys.platform == 'darwin' and os.path.isdir(clt) and 'DEVELOPER_DIR' not in os.environ:
        os.environ['DEVELOPER_DIR'] = clt
        return '/usr/bin/clang'
    return 'cc'


def on_windows():
    return sys.platform == 'win32' or bool(os.environ.get('MSYSTEM'))


def windows_resources(cc, obj):
    """src/host/setup.rc compiled: the program's own files the set-up puts
    beside it (src/host/setup_win.c), the fonts (data/native), each 8 bytes
    of size and the file deflated."""
    import zlib
    res = os.path.join(obj, 'res')
    os.makedirs(res, exist_ok=True)
    for name in ('fonts.bin', 'fonts_aa.bin'):
        src = os.path.join(ROOT, 'data', 'native', name)
        out = os.path.join(res, name + '.z')
        if not os.path.exists(out) or os.path.getmtime(out) < os.path.getmtime(src):
            print('compressing %s' % os.path.relpath(src, ROOT), flush=True)
            data = open(src, 'rb').read()
            open(out, 'wb').write(len(data).to_bytes(8, 'little') + zlib.compress(data, 9))
    rc = os.path.join(ROOT, 'src', 'host', 'setup.rc')
    o = os.path.join(obj, 'setup.res.o')
    windres = os.environ.get('WINDRES')
    if not windres:
        base = os.path.basename(cc)
        windres = (cc[:-len(base)] + base[:base.rindex('-gcc')] + '-windres') if '-gcc' in base else 'windres'
    newer = max(os.path.getmtime(f) for f in [rc, os.path.join(ROOT, 'src', 'host', 'setup.h')] +
                glob.glob(os.path.join(res, '*')))
    if not os.path.exists(o) or os.path.getmtime(o) < newer:
        run([windres, '-I', os.path.dirname(rc), '-I', res, '-O', 'coff', '-i', rc, '-o', o])
    return o


def exe(cross_windows=False):
    """build/OpenCiv2: compile the generated C with the host runtime.
    cross_windows: build/OpenCiv2.exe with MinGW-w64 and SDL2's MinGW
    release (tools/fetch-toolchain.sh windows), e.g. on Linux."""
    windows = cross_windows or on_windows()
    if cross_windows:
        cc = os.environ.get('CC') or 'x86_64-w64-mingw32-gcc'
        sdl = os.path.join(paths.SDL2_MINGW, 'x86_64-w64-mingw32')
        sdl_cflags = ['-I' + os.path.join(sdl, 'include', 'SDL2'), '-Dmain=SDL_main']
        # (-mwindows: a GUI program, no console window; as sdl2-config gives
        # on MSYS2.)  SDL2 linked in: the program is one file.
        sdl_libs = ['-L' + os.path.join(sdl, 'lib'), '-lmingw32', '-mwindows', '-lSDL2main',
                    '-Wl,-Bstatic', '-lSDL2', '-Wl,-Bdynamic', '-lsetupapi', '-lwinmm', '-limm32', '-lversion',
                    '-loleaut32', '-lcfgmgr32', '-lgdi32']
        sdl_dll = None
        obj = OBJ + '-windows'
    else:
        cc = os.environ.get('CC') or default_cc()
        # (SDL2_CONFIG: another SDL2's sdl2-config; SDL2_STATIC=1: linked in)
        static = bool(os.environ.get('SDL2_STATIC'))
        sdl = subprocess.run([os.environ.get('SDL2_CONFIG') or 'sdl2-config', '--cflags',
                              '--static-libs' if static else '--libs'], capture_output=True, text=True,
                             check=True).stdout.split()
        sdl_cflags = [f for f in sdl if f.startswith(('-I', '-D'))]
        sdl_libs = [f for f in sdl if not f.startswith(('-I', '-D'))]
        sdl_dll = None
        if windows:     # MSYS2: the DLL next to sdl2-config
            prefix = subprocess.run(['sdl2-config', '--prefix'], capture_output=True, text=True).stdout.strip()
            sdl_dll = os.path.join(prefix, 'bin', 'SDL2.dll')
        obj = OBJ
    os.makedirs(obj, exist_ok=True)
    # wasm2c's output: optimized (it is the whole game); no strict aliasing
    # assumptions across the module's memory.
    gen = sorted(glob.glob(os.path.join(CDIR, 'civ2_wasm*.c')))
    rt = [os.path.join(CDIR, f) for f in ('wasm-rt-impl.c', 'wasm-rt-mem-impl.c', 'wasm-rt-exceptions-impl.c')]
    # (the native menu bar: Win32 on Windows, Cocoa on macOS)
    mac = sys.platform == 'darwin' and not cross_windows
    # (the set-up of the game from the user's copy, src/host/setup.h, with
    # unshield and zlib for the CD's cabinet, libogg and libvorbis for a
    # disc image's music: their sources, from tools/fetch-toolchain.sh;
    # stb_vorbis to play the music)
    host = [os.path.join(ROOT, 'src', 'host', f) for f in
            ('port_main.c', 'wasi.c', 'host_sdl.c', 'host_net.c', 'setup.c', 'setup_disc.c') +
            (('host_menu_win.c', 'setup_win.c') if windows else ('setup_unix.c',)) +
            (('host_menu_mac.m', 'setup_mac.m') if mac else ())]
    third = [os.path.join(ROOT, 'src', 'third_party', 'stb_vorbis', 'stb_vorbis.c')] + \
        (sorted(glob.glob(os.path.join(paths.UNSHIELD, 'lib', '*.c'))) +
             [os.path.join(paths.UNSHIELD, 'lib', 'md5', 'md5c.c')] +
             [os.path.join(paths.ZLIB, f + '.c') for f in ('adler32', 'crc32', 'inffast', 'inflate', 'inftrees',
                                                          'zutil')] +
             [os.path.join(paths.OGG, 'src', f + '.c') for f in ('bitwise', 'framing')] +
             [os.path.join(paths.VORBIS, 'lib', f + '.c') for f in
              ('analysis', 'bitrate', 'block', 'codebook', 'envelope', 'floor0', 'floor1', 'info', 'lookup', 'lpc',
               'lsp', 'mapping0', 'mdct', 'psy', 'registry', 'res0', 'sharedbook', 'smallft', 'synthesis',
               'vorbisenc', 'window')])
    third_inc = ['-I' + paths.UNSHIELD, '-I' + os.path.join(paths.UNSHIELD, 'lib'), '-I' + paths.ZLIB,
                 '-I' + os.path.join(paths.OGG, 'include'), '-I' + os.path.join(paths.VORBIS, 'include'),
                 '-I' + os.path.join(paths.VORBIS, 'lib')] + \
        ([] if windows else ['-DHAVE_ALLOCA_H'])
    # (CFLAGS, LDFLAGS: added, e.g. macOS's -arch for each architecture)
    cflags = os.environ.get('CFLAGS', '').split()
    jobs = [(s, ['-O2', '-fno-strict-aliasing', '-w'] + cflags) for s in gen + rt] + \
           [(s, ['-O2', '-w'] + cflags + third_inc) for s in third] + \
           [(s, ['-O2', '-Wall', '-Wno-format-truncation', '-Werror=implicit-function-declaration'] + cflags +
             sdl_cflags + third_inc +
             (['-fobjc-arc'] if s.endswith('.m') else [])) for s in host]

    # Every file depends on the generated header too (the host's files
    # declare the module struct from it: a stale one instantiates a module
    # of the old size over the host's other data).
    header = os.path.join(CDIR, 'civ2_wasm.h')

    def compile_one(job):
        src, flags = job
        o = os.path.join(obj, os.path.basename(src) + '.o')
        deps = [src] + ([header] if os.path.exists(header) else []) + \
            (glob.glob(os.path.join(ROOT, 'src', 'host', '*.h')) if src in host else [])
        if not os.path.exists(o) or os.path.getmtime(o) < max(os.path.getmtime(d) for d in deps):
            subprocess.run([cc, '-c', '-o', o, src, '-I', CDIR, '-I', os.path.join(ROOT, 'src', 'host')] + flags,
                           check=True)
        return o
    # Few at a time: the generated files are large.
    with concurrent.futures.ThreadPoolExecutor(int(os.environ.get('JOBS', '2'))) as ex:
        objs = list(ex.map(compile_one, jobs))
    out = os.path.join(ROOT, 'build', 'OpenCiv2' + ('.exe' if windows else ''))
    # (Windows: the C runtime's threads library linked in, so only SDL2.dll is needed beside the program)
    libs = ['-lm', '-static-libgcc', '-Wl,-Bstatic', '-lpthread', '-Wl,-Bdynamic'] if windows else ['-lm', '-lpthread']
    if windows:
        objs.append(windows_resources(cc, obj))
        libs = ['-lole32', '-lshell32', '-luuid', '-lws2_32'] + libs
    if mac:
        libs += ['-framework', 'Cocoa', '-framework', 'IOKit']   # (IOKit: the CD's ioctls, setup_unix.c)
    run([cc, '-o', out] + cflags + os.environ.get('LDFLAGS', '').split() + objs + sdl_libs + libs)
    if sdl_dll and os.path.exists(sdl_dll):
        shutil.copy(sdl_dll, os.path.join(ROOT, 'build'))      # the game runs with SDL2.dll beside it
    print('built %s' % os.path.relpath(out, ROOT))


def pack():
    import tarfile
    out = os.path.join(ROOT, 'dist', 'OpenCiv2-src.tar.gz')
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with tarfile.open(out, 'w:gz') as t:
        for rel in ('build/wasm/c', 'src/host', 'src/platform/win32_vk.h', 'tools/build_port.py',
                    'tools/paths.py', 'tools/run-civ2.sh', 'tools/toolchain/unshield', 'tools/toolchain/zlib',
                    'tools/toolchain/libogg', 'tools/toolchain/libvorbis', 'data/native', 'harness/CIV2.DAT',
                    'README.md', 'THIRD-PARTY.txt'):
            t.add(os.path.join(ROOT, rel), arcname=os.path.join('OpenCiv2', rel))
    print('wrote %s (%d MB)' % (os.path.relpath(out, ROOT), os.path.getsize(out) >> 20))


def fetch_toolchain(steps):
    """What the steps need and tools/toolchain lacks, downloaded first
    (tools/fetch-toolchain.sh): wasi-sdk and wabt for module and c,
    unshield's, zlib's, libogg's and libvorbis's sources for exe, SDL2's
    MinGW release for exe-windows."""
    global paths
    wasm = ('module' in steps and not os.path.isdir(paths.WASI_SDK)) or \
        ('c' in steps and not os.path.isdir(paths.WABT))
    sources = any(s.startswith('exe') for s in steps) and \
        not all(os.path.exists(f) for f in (os.path.join(paths.UNSHIELD, 'lib', 'libunshield.c'),
                                            os.path.join(paths.OGG, 'include', 'ogg', 'config_types.h'),
                                            os.path.join(paths.VORBIS, 'lib', 'vorbisenc.c')))
    mingw = 'exe-windows' in steps and not os.path.isdir(paths.SDL2_MINGW)
    if wasm or sources or mingw:
        run(['sh', os.path.join(ROOT, 'tools', 'fetch-toolchain.sh')] + (['windows'] if mingw else []) +
            ([] if wasm else ['sources']))
        import importlib
        paths = importlib.reload(paths)


def main():
    steps = sys.argv[1:] or ['module', 'c', 'exe']
    fetch_toolchain(steps)
    for s in steps:
        {'module': module, 'c': to_c, 'exe': exe, 'exe-windows': lambda: exe(True), 'pack': pack}[s]()


if __name__ == '__main__':
    main()
