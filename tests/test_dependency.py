"""Release resolver regressions; no external network or game data."""
import functools
import hashlib
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import os
from pathlib import Path
import stat
import sys
import tempfile
import threading
import unittest
from unittest.mock import patch
import zipfile

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from dependency import cached_release, digest, release, resolve, safe_name, validate, write_json


class QuietHandler(SimpleHTTPRequestHandler):
    def log_message(self, *_):
        pass


class DependencyTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.payload = self.root / 'payload'
        self.payload.mkdir()
        (self.payload / 'file.bin').write_bytes(b'asset pixels\0' * 100)
        self.archive = self.root / 'assets.zip'
        self.sha = release(self.payload, ['file.bin'], self.archive, 'openciv2-assets', '1')
        self.cache = self.root / 'cache'

    def test_determinism_and_mtime_independence(self):
        os.utime(self.payload / 'file.bin', (1, 1))
        second = self.root / 'second.zip'
        self.assertEqual(self.sha, release(self.payload, ['file.bin'], second, 'openciv2-assets', '1'))
        with zipfile.ZipFile(second) as archive:
            for info in archive.infolist():
                self.assertEqual(info.create_system, 3)
                self.assertEqual(info.date_time, (1980, 1, 1, 0, 0, 0))
                self.assertEqual(info.external_attr >> 16, stat.S_IFREG | 0o644)

    def test_http_download_then_offline_cache(self):
        server = ThreadingHTTPServer(('127.0.0.1', 0), functools.partial(QuietHandler, directory=str(self.root)))
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        url = f'http://127.0.0.1:{server.server_port}/assets.zip'
        try:
            out = cached_release(url, self.sha, 'openciv2-assets', self.cache)
        finally:
            server.shutdown()
            thread.join()
            server.server_close()
        self.assertEqual((out / 'file.bin').read_bytes(), (self.payload / 'file.bin').read_bytes())
        self.assertEqual(out, cached_release(url, self.sha, 'openciv2-assets', self.cache))
        (out / 'file.bin').write_bytes(b'tampered')
        with self.assertRaisesRegex(ValueError, 'missing or changed'):
            cached_release(url, self.sha, 'openciv2-assets', self.cache)

    def test_checksum_failure_never_publishes_cache(self):
        with self.assertRaisesRegex(ValueError, 'Checksum mismatch'):
            cached_release(self.archive.as_uri(), '0' * 64, 'openciv2-assets', self.cache)
        self.assertFalse((self.cache / ('0' * 64)).exists())
        self.assertEqual(list(self.cache.iterdir()), [])

    def test_bad_archive_paths_and_symlinks(self):
        for name in ('../outside', '/outside', 'C:/outside', 'a\\outside'):
            with self.subTest(name=name):
                bad = self.root / 'bad.zip'
                with self.assertRaisesRegex(ValueError, 'Unsafe'):
                    safe_name(name)
                with zipfile.ZipFile(bad, 'w') as z:
                    info = zipfile.ZipInfo(name)
                    info.filename = name  # Preserve Windows separators in this malicious fixture.
                    z.writestr(info, b'bad')
                with self.assertRaisesRegex(ValueError, 'Unsafe'):
                    cached_release(bad.as_uri(), digest(bad), 'openciv2-assets', self.cache)
        self.assertFalse((self.root / 'outside').exists())
        with zipfile.ZipFile(self.root / 'link.zip', 'w') as z:
            info = zipfile.ZipInfo('link')
            info.external_attr = 0o120777 << 16
            z.writestr(info, '../outside')
        with self.assertRaisesRegex(ValueError, 'Unsupported'):
            p = self.root / 'link.zip'
            cached_release(p.as_uri(), digest(p), 'openciv2-assets', self.cache)

    def test_wrong_package_kind(self):
        with self.assertRaisesRegex(ValueError, 'expected openciv2-wrong'):
            cached_release(self.archive.as_uri(), self.sha, 'openciv2-wrong', self.cache)
        self.assertFalse((self.cache / self.sha).exists())

    def test_selection_and_no_silent_fallback(self):
        consumer = self.root / 'openciv2'
        sibling = self.root / 'openciv2-assets'
        consumer.mkdir()
        sibling.mkdir()
        write_json(consumer / 'dependencies.json', {'assets': {'url': self.archive.as_uri(), 'sha256': self.sha}})
        with patch.dict(os.environ, {'CIV2_DEPENDENCY_CACHE': str(self.cache)}, clear=True):
            self.assertEqual(resolve(consumer, 'assets'), sibling)
            out = resolve(consumer, 'assets', url=self.archive.as_uri(), sha256=self.sha)
            self.assertNotEqual(out, sibling)
            self.assertEqual(validate(out, 'openciv2-assets')['version'], '1')
            with self.assertRaisesRegex(ValueError, 'not found'):
                resolve(consumer, 'assets', local=self.root / 'missing')
            with self.assertRaisesRegex(ValueError, 'checksum is required'):
                resolve(consumer, 'assets', url=self.archive.as_uri())
            with patch.dict(os.environ, {'CIV2_ASSETS_DIR': str(self.payload)}):
                self.assertEqual(resolve(consumer, 'assets'), self.payload)
            sibling.rmdir()
            self.assertEqual(resolve(consumer, 'assets'), out)


if __name__ == '__main__':
    unittest.main()
