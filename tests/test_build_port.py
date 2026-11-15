"""tools/build_port.py's exe step on Windows (MSYS2): SDL2.dll beside the program only when SDL2
is not linked in.  Run with Python unittest; nothing is compiled (the compiler and sdl2-config
are stand-ins) and no game files are needed."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import build_port  # noqa: E402


class WindowsExeTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='build port ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        # the MSYS2 prefix sdl2-config reports, with its SDL2.dll
        self.prefix = self.root / 'msys2'
        (self.prefix / 'bin').mkdir(parents=True)
        (self.prefix / 'bin' / 'SDL2.dll').write_bytes(b'')
        (self.root / 'build').mkdir()
        self.sdl2_config = []      # the options sdl2-config was asked for
        self.copied = []           # what shutil.copy copied

    def fake_run(self, cmd, **kw):
        if cmd[0] == 'sdl2-config':
            self.sdl2_config.append(cmd[1:])
            out = str(self.prefix) + '\n' if '--prefix' in cmd else \
                '-I/ucrt64/include/SDL2 -Dmain=SDL_main -L/ucrt64/lib -lmingw32 -mwindows -lSDL2main -lSDL2\n'
            return subprocess.CompletedProcess(cmd, 0, out, '')
        if '-o' in cmd:            # the compiler: an empty output file
            Path(cmd[cmd.index('-o') + 1]).write_bytes(b'')
        return subprocess.CompletedProcess(cmd, 0, '', '')

    def build(self, env):
        env = dict({k: v for k, v in os.environ.items()
                    if k not in ('SDL2_STATIC', 'SDL2_CONFIG', 'CFLAGS', 'LDFLAGS')}, CC='cc', **env)
        with mock.patch.dict(os.environ, env, clear=True), \
                mock.patch.object(build_port, 'ROOT', str(self.root)), \
                mock.patch.object(build_port, 'CDIR', str(self.root / 'c')), \
                mock.patch.object(build_port, 'OBJ', str(self.root / 'obj')), \
                mock.patch.object(build_port, 'on_windows', return_value=True), \
                mock.patch.object(build_port, 'windows_resources', return_value=str(self.root / 'setup.res.o')), \
                mock.patch.object(build_port.subprocess, 'run', side_effect=self.fake_run), \
                mock.patch.object(build_port.shutil, 'copy', side_effect=lambda src, dst: self.copied.append(src)), \
                mock.patch('builtins.print'):
            build_port.exe()
        self.assertTrue((self.root / 'build' / 'OpenCiv2.exe').exists())

    def test_static_sdl2_no_dll(self):
        # SDL2_STATIC=1: SDL2 linked in, so no SDL2.dll is put beside the program
        self.build({'SDL2_STATIC': '1'})
        self.assertIn('--static-libs', self.sdl2_config[0])
        self.assertEqual(self.copied, [])

    def test_dynamic_sdl2_copies_dll(self):
        # SDL2_STATIC unset: the program needs SDL2.dll, copied beside it
        self.build({})
        self.assertIn('--libs', self.sdl2_config[0])
        self.assertEqual(self.copied, [os.path.join(str(self.prefix), 'bin', 'SDL2.dll')])


if __name__ == '__main__':
    unittest.main()
