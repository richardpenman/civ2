"""Replay checker regressions; run with Python unittest and Pillow, no game assets."""
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
import tempfile
import unittest

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
TRACE = 'srand 7\nknown_caller 123\n'


def write(path, text):
    with path.open('w', encoding='utf-8', newline='\n') as stream:
        stream.write(text)


class CompareTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='replay compare ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.a = self.root / 'reference'
        self.b = self.root / 'candidate'
        for directory in (self.a, self.b):
            directory.mkdir()
            Image.new('RGB', (3, 2), 'black').save(directory / 'frame.png')
            write(directory / 'rand.log', TRACE)

    def compare(self, *, report=False):
        command = [sys.executable, str(ROOT / 'harness/compare.py'), str(self.a), str(self.b)]
        if report:
            command += ['--report', str(self.root / 'report')]
        return subprocess.run(command, capture_output=True, text=True, timeout=30)

    def assert_failed(self, result, diagnostic):
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn(diagnostic, result.stdout)

    def test_identical_runs_and_report(self):
        result = self.compare(report=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('rand trace: identical', result.stdout)
        self.assertTrue((self.root / 'report/index.html').is_file())

    def test_different_pixels(self):
        Image.new('RGB', (3, 2), 'white').save(self.b / 'frame.png')
        self.assert_failed(self.compare(report=True), 'DIFFERS')
        self.assertTrue((self.root / 'report/frame-diff.png').is_file())

    def test_different_image_dimensions(self):
        # Identical overlapping pixels must not hide a size mismatch.
        Image.new('RGB', (4, 2), 'black').save(self.b / 'frame.png')
        self.assert_failed(self.compare(report=True), 'size')

    def test_missing_or_extra_screenshot(self):
        for directory in (self.a, self.b):
            with self.subTest(extra_in=directory.name):
                extra = directory / 'extra.png'
                Image.new('RGB', (3, 2), 'black').save(extra)
                self.assert_failed(self.compare(report=True), 'MISSING')
                extra.unlink()

    def test_no_screenshots_even_with_matching_rng(self):
        for directory in (self.a, self.b):
            (directory / 'frame.png').unlink()
        self.assert_failed(self.compare(), 'no screenshots')

    def test_empty_run_directories(self):
        for directory in (self.a, self.b):
            for path in directory.iterdir():
                path.unlink()
        self.assert_failed(self.compare(), 'no screenshots')

    def test_missing_rng_log_on_either_or_both_sides(self):
        for missing in ((self.a,), (self.b,), (self.a, self.b)):
            with self.subTest(missing=[p.name for p in missing]):
                for directory in (self.a, self.b):
                    write(directory / 'rand.log', TRACE)
                for directory in missing:
                    (directory / 'rand.log').unlink()
                self.assert_failed(self.compare(report=True), 'rand.log: MISSING')
                report = (self.root / 'report/index.html').read_text()
                self.assertIn('FAIL', report)
                self.assertIn('rand.log: MISSING', report)

    def test_empty_rng_logs(self):
        for empty in ('', '\0' * 32, ' \n\t\n'):
            for directories in ((self.a,), (self.b,), (self.a, self.b)):
                with self.subTest(contents=repr(empty), sides=[p.name for p in directories]):
                    for directory in (self.a, self.b):
                        write(directory / 'rand.log', TRACE)
                    for directory in directories:
                        write(directory / 'rand.log', empty)
                    self.assert_failed(self.compare(), 'rand.log: EMPTY')

    def test_seed_only_trace_is_valid(self):
        # Evidence of initialization is valid even if a run makes no rand calls.
        for directory in (self.a, self.b):
            write(directory / 'rand.log', 'srand 7\n')
        result = self.compare()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_original_log_null_padding(self):
        write(self.a / 'rand.log', TRACE + '\0' * 32)
        result = self.compare()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_changed_or_truncated_rng_trace(self):
        for trace in ('srand 8\nknown_caller 123\n', 'srand 7\nknown_caller 124\n',
                      'srand 7\nother_caller 123\n', 'srand 7\n', TRACE + 'known_caller 456\n'):
            with self.subTest(trace=trace):
                write(self.b / 'rand.log', trace)
                self.assert_failed(self.compare(), 'rand trace: DIVERGES')


@unittest.skipUnless(shutil.which('sh'), 'native suite regression tests require a POSIX sh')
class SuiteTestCase(unittest.TestCase):
    suite_script = 'native_suite.sh'

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='replay suite ')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for directory in ('harness/scripts', 'tools', 'bin', 'fixtures'):
            (self.root / directory).mkdir(parents=True)
        for name in ('native_suite.sh', 'diff_suite.sh', 'compare.py'):
            write(self.root / 'harness' / name, (ROOT / 'harness' / name).read_text())
        # Exercise the real suite and comparator; only the game runner is replaced.
        write(self.root / 'tools/native-run.sh', '''#!/bin/sh
name=$(basename "$1" .txt)
mkdir -p "$2" || exit 1
cp "fixtures/$name/"* "$2/" || exit 1
if [ -f "fixtures/$name/layout-error" ]; then
    echo 'not laid out as in the original'
fi
read first_status second_status < "$1"
status=$first_status
if [ "$EXE" = right ] && [ -n "$second_status" ]; then
    status=$second_status
fi
exit "$status"
''')
        # Use the test interpreter, including on hosts where only "python" exists.
        python3 = self.root / 'bin/python3'
        write(python3, '#!/bin/sh\nexec ' + shlex.quote(Path(sys.executable).as_posix()) + ' "$@"\n')
        python3.chmod(0o755)
        self.env = dict(os.environ, A='left', B='right',
                        PATH=str(self.root / 'bin') + os.pathsep + os.environ['PATH'])

    def scenario(self, name='first', *, exit_code=0, bad_bitmap=False, layout_error=False):
        write(self.root / 'harness/scripts' / (name + '.txt'), str(exit_code) + '\n')
        fixture = self.root / 'fixtures' / name
        reference = self.root / 'run' / ('ref-' + name)
        fixture.mkdir()
        reference.mkdir(parents=True)
        for directory in (fixture, reference):
            Image.new('RGB', (3, 2), 'black').save(directory / 'frame.png')
            write(directory / 'rand.log', TRACE)
        Image.new('RGB', (3, 2), 'black').save(fixture / 'frame.bmp')
        if bad_bitmap:
            write(fixture / 'bad.bmp', 'not a bitmap')
        if layout_error:
            write(fixture / 'layout-error', '')

    def suite(self, *names):
        command = [shutil.which('sh'), (self.root / 'harness' / self.suite_script).as_posix()]
        command += ['harness/scripts/' + name + '.txt' for name in names]
        # Run from outside the fixture root, whose path deliberately contains spaces.
        return subprocess.run(command, cwd=self.root.parent, env=self.env,
                              capture_output=True, text=True, timeout=30)


class NativeSuiteTests(SuiteTestCase):
    def test_matching_replay(self):
        self.scenario()
        result = self.suite()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('PASS first', result.stdout)

    def test_runner_failure_after_matching_output(self):
        for code in (7, 124, 139):
            name = 'exit' + str(code)
            self.scenario(name, exit_code=code)
            with self.subTest(exit_code=code):
                result = self.suite(name)
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn('runner exited ' + str(code), result.stdout)
                self.assertNotIn('PASS', result.stdout)
                self.assertFalse((self.root / 'run' / ('compare-native-' + name + '.txt')).exists())

    def test_conversion_failure_after_matching_output(self):
        self.scenario(bad_bitmap=True)
        result = self.suite('first')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('screenshot conversion failed', result.stdout)
        self.assertNotIn('PASS', result.stdout)
        self.assertFalse((self.root / 'run/compare-native-first.txt').exists())

    def test_layout_failure(self):
        self.scenario(layout_error=True)
        result = self.suite('first')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('not laid out as in the original', result.stdout)
        self.assertNotIn('PASS', result.stdout)

    def test_comparator_failure(self):
        self.scenario()
        write(self.root / 'fixtures/first/rand.log', 'srand 99\n')
        result = self.suite('first')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('rand trace: DIVERGES', result.stdout)
        self.assertNotIn('PASS', result.stdout)

    def test_failure_does_not_skip_later_scenarios(self):
        self.scenario(exit_code=7)
        self.scenario('second')
        result = self.suite('first', 'second')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('FAIL first', result.stdout)
        self.assertIn('PASS second', result.stdout)


class DiffSuiteTests(SuiteTestCase):
    suite_script = 'diff_suite.sh'

    def test_matching_runs(self):
        self.scenario()
        result = self.suite('first')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('SAME first', result.stdout)

    def test_runner_failure_after_matching_output(self):
        for name, status in (('left_fails', '7 0'), ('right_fails', '0 7')):
            self.scenario(name, exit_code=status)
            with self.subTest(failure=name):
                result = self.suite(name)
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn('exited with status 7', result.stdout)
                self.assertNotIn('SAME', result.stdout)

    def test_conversion_failure_after_matching_output(self):
        self.scenario(bad_bitmap=True)
        result = self.suite('first')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('screenshot conversion failed', result.stdout)
        self.assertNotIn('SAME', result.stdout)

    def test_suspicious_request(self):
        self.scenario()
        write(self.root / 'fixtures/first/hook.log', 'SUSPICIOUS request\n')
        result = self.suite('first')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('SUSPICIOUS request', result.stdout)
        self.assertNotIn('SAME', result.stdout)

    def test_failure_does_not_skip_later_scenarios(self):
        self.scenario(exit_code=7)
        self.scenario('second')
        result = self.suite('first', 'second')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn('FAIL first', result.stdout)
        self.assertIn('SAME second', result.stdout)


if __name__ == '__main__':
    unittest.main()
