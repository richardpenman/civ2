"""Synthetic log-format regressions; these records are not gameplay evidence."""
import hashlib
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
TOOL = ROOT / 'tools/stackprobe_report.py'
spec = importlib.util.spec_from_file_location('stackprobe_report', TOOL)
reporter = importlib.util.module_from_spec(spec)
spec.loader.exec_module(reporter)


def record(value='6715d7a0', *, call=1, caller='560c5e', words='7 6 0',
           function='560d95', offset='34'):
    return ('stackprobe %s #%d from %s args %s local_%s: %s\n'
            % (function, call, caller, words, offset, value)).encode('ascii')


class StackprobeReportTests(unittest.TestCase):
    def test_repeated_sample_is_not_consumer_proof(self):
        raw = record() + record(call=2)
        result = reporter.summarize([('one.log', raw)])
        self.assertEqual(result['evidence_kind'], 'function-entry samples')
        self.assertFalse(result['consumer_reads_observed'])
        self.assertEqual(result['files'][0]['sha256'], hashlib.sha256(raw).hexdigest())
        self.assertEqual(result['files'][0]['probe_records'], 2)
        slot, = result['slots']
        self.assertEqual(slot['function'], '0x00560d95')
        self.assertEqual(slot['offset_from_entry_esp'], -0x34)
        self.assertTrue(slot['observed_single_value'])
        self.assertEqual(slot['values'], [{'value': '0x6715d7a0', 'samples': 2}])

    def test_variation_preserves_run_caller_and_stack_context(self):
        first = record() + record('00453e93', call=2, words='6 7 0')
        second = record('00000000', caller='401234')
        slot, = reporter.summarize([('first.log', first), ('second.log', second)])['slots']
        self.assertFalse(slot['observed_single_value'])
        self.assertEqual(slot['distinct_values'], 3)
        self.assertEqual(slot['samples'], 3)
        self.assertEqual(len(slot['contexts']), 3)
        self.assertEqual({item['log_index'] for item in slot['contexts']}, {0, 1})
        self.assertEqual({item['return_address'] for item in slot['contexts']},
                         {'0x00560c5e', '0x00401234'})
        self.assertEqual({tuple(item['entry_stack_words']) for item in slot['contexts']},
                         {('0x00000007', '0x00000006', '0x00000000'),
                          ('0x00000006', '0x00000007', '0x00000000')})

    def test_multiple_words_follow_increasing_memory_addresses(self):
        raw = record('00000001 00000002 ffffffff', function='59062c', offset='9c')
        rows = reporter.summarize([('span.log', raw)])['slots']
        self.assertEqual([row['offset_from_entry_esp'] for row in rows], [-0x9c, -0x98, -0x94])
        self.assertEqual([row['values'][0]['value'] for row in rows],
                         ['0x00000001', '0x00000002', '0xffffffff'])

    def test_overlapping_spans_use_same_relative_offset(self):
        raw = record('00000001 00000002', offset='38') + record('00000002')
        rows = reporter.summarize([('overlap.log', raw)])['slots']
        self.assertEqual([row['samples'] for row in rows], [1, 2])
        self.assertEqual(rows[1]['values'], [{'value': '0x00000002', 'samples': 2}])

    def test_functions_at_same_offset_remain_separate(self):
        raw = record() + record(function='538a29')
        self.assertEqual(len(reporter.summarize([('two.log', raw)])['slots']), 2)

    def test_padding_headers_crlf_and_uppercase_hex(self):
        raw = b'civ2 hook runtime loaded\r\nstackprobe: 1 probes\r\n'
        raw += record('ABCDEF01', function='560D95').replace(b'\n', b'\r\n') + b'\0' * 32
        result = reporter.summarize([('padded.log', raw)])
        self.assertEqual(result['files'][0]['bytes'], len(raw))
        slot, = result['slots']
        self.assertEqual(slot['values'][0]['value'], '0xabcdef01')
        self.assertEqual(slot['contexts'][0]['first_line'], 3)

    def test_malformed_or_truncated_record_is_an_error(self):
        for raw in (record(''), record('123'), record('123456789'), record('not-hex!'),
                    record()[:-5], b'stackprobe', b'stackprobe 560d95 #1 from',
                    record().replace(b'stackprobe ', b'stackprobe\t')):
            with self.subTest(raw=raw):
                with self.assertRaisesRegex(ValueError, 'malformed or truncated'):
                    reporter.parse_records(raw, 'bad.log')

    def test_unrelated_or_empty_log_is_not_evidence(self):
        for raw in (b'', b'\0' * 16, b'stackprobe: 0 probes\n', b'srand 7\n'):
            with self.subTest(raw=raw):
                with self.assertRaisesRegex(ValueError, 'no stackprobe records'):
                    reporter.parse_records(raw, 'empty.log')

    def test_cli_reads_multiple_logs_without_modifying_them(self):
        with tempfile.TemporaryDirectory(prefix='stack report ') as directory:
            paths = [Path(directory) / name for name in ('one.log', 'two.log')]
            contents = [record(), record('00453e93')]
            for path, raw in zip(paths, contents):
                path.write_bytes(raw)
            result = subprocess.run([sys.executable, str(TOOL), *map(str, paths)],
                                    capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(json.loads(result.stdout)['slots'][0]['distinct_values'], 2)
            self.assertEqual([path.read_bytes() for path in paths], contents)
            duplicate = subprocess.run([sys.executable, str(TOOL), str(paths[0]), str(paths[0])],
                                       capture_output=True, text=True, timeout=10)
            self.assertEqual(duplicate.returncode, 2)
            self.assertEqual(duplicate.stdout, '')

    def test_cli_rejects_missing_or_invalid_input_without_partial_json(self):
        with tempfile.TemporaryDirectory() as directory:
            bad = Path(directory) / 'bad.log'
            for content in (None, b'no probe records\n', record() + b'stackprobe truncated\n'):
                if content is not None:
                    bad.write_bytes(content)
                result = subprocess.run([sys.executable, str(TOOL), str(bad)],
                                        capture_output=True, text=True, timeout=10)
                self.assertEqual(result.returncode, 2, result.stdout + result.stderr)
                self.assertEqual(result.stdout, '')


if __name__ == '__main__':
    unittest.main()
