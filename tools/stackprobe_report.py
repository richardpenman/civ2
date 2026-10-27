#!/usr/bin/env python3
"""Summarize CIV2_STACKPROBE entry samples without inferring consumer values.

Usage: python3 tools/stackprobe_report.py RUN/hook.log [OTHER_RUN/hook.log ...]
The JSON report goes to stdout; input logs are never modified.
"""
import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re


HEX = r'[0-9a-fA-F]{1,8}'
RECORD = re.compile(
    r'stackprobe (' + HEX + r') #([1-9][0-9]*) from (' + HEX + r') args '
    r'(' + HEX + r') (' + HEX + r') (' + HEX + r') local_(' + HEX + r'):'
    r'((?: [0-9a-fA-F]{8})+)')


def parse_records(raw, name):
    records = []
    # The hook's memory-mapped log can have unused NUL bytes at the end.
    for line_number, line in enumerate(raw.rstrip(b'\0').decode('utf-8', errors='replace').splitlines(), 1):
        line = line.strip()
        if not re.match(r'stackprobe(?:\s|$)', line):
            continue
        match = RECORD.fullmatch(line)
        if not match:
            raise ValueError('%s:%d: malformed or truncated stackprobe record' % (name, line_number))
        address, call, caller, arg1, arg2, arg3, offset, values = match.groups()
        records.append({
            'function': int(address, 16), 'call': int(call), 'line': line_number,
            'return_address': int(caller, 16),
            'entry_stack_words': tuple(int(word, 16) for word in (arg1, arg2, arg3)),
            'start_offset': -int(offset, 16),
            'values': tuple(int(word, 16) for word in values.split()),
        })
    if not records:
        raise ValueError('%s: no stackprobe records' % name)
    return records


def value_counts(values):
    return [{'value': '0x%08x' % value, 'samples': count}
            for value, count in sorted(Counter(values).items())]


def summarize(inputs):
    files = []
    slots = defaultdict(list)
    for log_index, (name, raw) in enumerate(inputs):
        records = parse_records(raw, name)
        files.append({'path': name, 'sha256': hashlib.sha256(raw).hexdigest(),
                      'bytes': len(raw), 'probe_records': len(records)})
        for record in records:
            # The probe prints DWORDs at entry ESP - OFF + 4*k, not OFF + 4*k.
            for index, value in enumerate(record['values']):
                slot = (record['function'], record['start_offset'] + 4 * index)
                slots[slot].append((log_index, record, value))
    rows = []
    for (function, offset), samples in sorted(slots.items()):
        contexts = defaultdict(list)
        for log_index, record, value in samples:
            context = (log_index, record['return_address'], record['entry_stack_words'])
            contexts[context].append((record, value))
        values = [value for _, _, value in samples]
        rows.append({
            'function': '0x%08x' % function, 'offset_from_entry_esp': offset,
            'samples': len(samples), 'distinct_values': len(set(values)),
            'observed_single_value': len(set(values)) == 1,
            'values': value_counts(values),
            'contexts': [
                {'log_index': log_index, 'return_address': '0x%08x' % caller,
                 'entry_stack_words': ['0x%08x' % word for word in words],
                 'first_line': min(record['line'] for record, _ in observations),
                 'last_line': max(record['line'] for record, _ in observations),
                 'samples': len(observations),
                 'values': value_counts([value for _, value in observations])}
                for (log_index, caller, words), observations in sorted(contexts.items())
            ],
        })
    return {'evidence_kind': 'function-entry samples',
            'consumer_reads_observed': False,
            'limits': 'Entry samples do not establish reads before assignment, last writers, '
                      'absolute slot identity across calls, or universal compatibility constants.',
            'files': files, 'slots': rows}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('logs', type=Path, nargs='+')
    args = parser.parse_args()
    try:
        if len({path.resolve() for path in args.logs}) != len(args.logs):
            raise ValueError('the same input log was supplied more than once')
        report = summarize([(str(path), path.read_bytes()) for path in args.logs])
    except (OSError, ValueError) as error:
        parser.error(str(error))
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
