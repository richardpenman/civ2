"""Check package-to-game installation, contract drift and prepared-package checks."""
import os
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import prepare_assets
from dependency import inventory, write_json


class PreparationTests(unittest.TestCase):
    def test_contract_and_prepared_inputs(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            game, package = root / 'game', root / 'package'
            (game / 'src/data').mkdir(parents=True)
            cpp = game / 'src/data/gamedata.cpp'
            declaration = 'static const char cityview_improvement_art_x_block_init[] = "\\x01\\x00\\x00\\x00";'
            cpp.write_text(declaration)
            rects = [list(x) for x in struct.iter_unpack('<4i', b'\x01'.ljust(992, b'\0'))]
            files = ['data/atlas-hd.pack', 'data/fonts.bin', 'data/fonts_aa.bin',
                     'data/ATLAS-PROVENANCE.md', 'data/cityview-rects.json', 'include/hd_layouts.h', 'THIRD-PARTY.txt']
            for name in files:
                p = package / name
                p.parent.mkdir(parents=True, exist_ok=True)
                p.write_bytes(b'fixture')
            write_json(package / 'data/cityview-rects.json', rects)
            write_json(package / 'package.json', {'schema': 1, 'kind': 'openciv2-assets',
                       'files': inventory(package, files)})
            with patch.object(prepare_assets, 'ROOT', game), patch.dict(os.environ, {}, clear=True):
                prepare_assets.prepare(local=package)
                header = game / 'build/native/gen/hd_layouts.h'
                notice = game / 'build/data/ASSET-NOTICES.txt'
                self.assertEqual(notice.read_bytes(), (package / 'THIRD-PARTY.txt').read_bytes())
                stamp = header.stat().st_mtime_ns
                prepare_assets.prepare(local=package)
                self.assertEqual(header.stat().st_mtime_ns, stamp)
                with patch.dict(os.environ, {'CIV2_ATLAS_PREBUILT': '1'}):
                    prepare_assets.prepare()
                    notice.write_bytes(b'changed')
                    with self.assertRaisesRegex(ValueError, 'Prepared assets changed'):
                        prepare_assets.prepare()
                    notice.write_bytes((package / 'THIRD-PARTY.txt').read_bytes())
                    cpp.write_text(declaration.replace('x01', 'x02'))
                    with self.assertRaisesRegex(ValueError, 'rectangle contract'):
                        prepare_assets.prepare()
                    cpp.write_text(declaration)
                    header.write_text('changed')
                    with self.assertRaisesRegex(ValueError, 'Prepared assets changed'):
                        prepare_assets.prepare()
                cpp.write_text(declaration.replace('x01', 'x02'))
                with self.assertRaisesRegex(ValueError, 'rectangle contract'):
                    prepare_assets.prepare(local=package)
                cpp.write_text(declaration)
                prepare_assets.prepare(local=package)
                cpp.unlink()  # A prepared portable-C package needs neither source nor a download.
                prepare_assets.prepare()

                # Explicit dependency overrides also work in portable packages.
                with patch.dict(os.environ, {'CIV2_ASSETS_DIR': str(root / 'missing')}):
                    with self.assertRaisesRegex(ValueError, 'not found'):
                        prepare_assets.prepare()
                with patch.dict(os.environ, {'CIV2_ASSETS_URL': 'https://invalid.test/assets.zip'}):
                    with self.assertRaisesRegex(ValueError, 'checksum is required'):
                        prepare_assets.prepare()


if __name__ == '__main__':
    unittest.main()
