#!/usr/bin/env python3
"""Why a client's save differs from the host's (nettest --save-end): replay
the host's state diffs as the client received them (its XD log), once in
the order they arrived and once in the order the game's message queue takes
them, and say which differing bytes of the saves each order explains.

The game's queue (NetMsgQueue::enqueue, 0051438f; net_msg_is_priority) keeps
message types 0x2a..0x66 apart and NetSession::process_messages takes them
first: NM_ALPHA_DIFFERENCE_ENGINE (0x5c) is one of them, NM_DIFFERENCE_ENGINE
(0x15) is not.  A diff and a later alpha diff waiting together are applied
the other way round, and the earlier diff writes back the older values.
Messages are counted as waiting together when they arrived (the XD log's
receive callbacks) at most GAP ms apart (default 3).

usage: diffreplay.py RUN_DIR [GAP]     (RUN_DIR: run/xd/cap/NAME, with
       xdlog-client.txt, end-host.net and end-client.net)
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import savecmp  # noqa: E402
import xdmsgs  # noqa: E402

DIFF, ALPHA = 0x15, 0x5c
SECTIONS = ((3, 'civs'), (5, 'units'))      # (diff engine section, savecmp section): the same layout


def rle_decode(b):
    """diffeng_rle_decode: [u16 n < 0x8000][byte] runs, [u16 0x8000 + n][n bytes] literals."""
    out, q = bytearray(), 0
    while q + 2 <= len(b):
        c = struct.unpack('<H', b[q:q + 2])[0]
        if c < 0x8000:
            out += bytes([b[q + 2]]) * c
            q += 3
        else:
            out += b[q + 2:q + 2 + c - 0x8000]
            q += 2 + c - 0x8000
    return bytes(out)


def records(d, size):
    """A diff message's records: (section, offset, bytes)."""
    if len(d) < size:
        return []
    n, p, out = struct.unpack('<i', d[16:20])[0], 0x2c, []
    for _ in range(n):
        sid, off, sz = struct.unpack('<III', d[p:p + 12])
        body = d[p + 12:p + 12 + sz]
        p += 12 + sz
        out.append((sid & 0x7fff, off, rle_decode(body) if sid & 0x8000 else body))
    return out


def received(path):
    """The client's received game messages: (ms, type, seq, data, size)."""
    out = []
    for m in xdmsgs.parse(path):
        d = m['data']
        if m['chan'] == 'tcp' and m['dir'] == 'rx' and len(d) >= 16 and d[:4] == b'\x60\x66\x60\x66':
            typ, size, seq = struct.unpack('<iII', d[4:16])
            out.append((m['t'], typ, seq, d, size))
    return out


def apply(order):
    """Each byte of the compared sections: (value, seq of the message that wrote it last)."""
    st = {sid: {} for sid, _ in SECTIONS}
    for t, typ, seq, d, size in order:
        if typ in (DIFF, ALPHA):
            for sid, off, body in records(d, size):
                if sid in st:
                    for i, b in enumerate(body):
                        st[sid][off + i] = (b, seq)
    return st


def main():
    run = sys.argv[1]
    gap = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    msgs = received(os.path.join(run, 'xdlog-client.txt'))
    bursts, cur, last = [], [], None
    for x in msgs:
        if last is not None and x[0] - last > gap:
            bursts.append(cur)
            cur = []
        cur.append(x)
        last = x[0]
    bursts.append(cur)
    arrival = apply([x for b in bursts for x in b])
    queue = apply([x for b in bursts for x in sorted(b, key=lambda x: 0 if 0x2a <= x[1] <= 0x66 else 1)])
    host = savecmp.sections(os.path.join(run, 'end-host.net'))
    client = savecmp.sections(os.path.join(run, 'end-client.net'))
    unexplained = 0
    for sid, name in SECTIONS:
        h, c = host[name], client[name]
        size = savecmp.CIV if name == 'civs' else 0x20
        explained, other = [], []
        for i in range(min(len(h), len(c))):
            if h[i] == c[i]:
                continue
            a, q = arrival[sid].get(i), queue[sid].get(i)
            if a and q and a != q and a[0] == h[i] and q[0] == c[i]:
                explained.append((i, a[1], q[1]))
            else:
                other.append(i)
        unexplained += len(other)
        print('%s: %d bytes differ' % (name, len(explained) + len(other)))
        runs = []                               # (first, last, seq in arrival order, seq in the queue's)
        for i, sa, sq in explained:
            if runs and runs[-1][1] + 1 >= i - 3 and runs[-1][2:] == (sa, sq) and runs[-1][0] // size == i // size:
                runs[-1] = (runs[-1][0], i, sa, sq)
            else:
                runs.append((i, i, sa, sq))
        for i, j, sa, sq in runs:
            print('  %s %d +0x%x..+0x%x: host %s (message seq %d, the last to write it in arrival order), '
                  'client %s (seq %d, the last in the queue\'s order)' % (
                      'civ' if name == 'civs' else 'unit', i // size, i % size, j % size,
                      h[i:j + 1].hex()[:24], sa, c[i:j + 1].hex()[:24], sq))
        for i in other:
            print('  %s %d +0x%x: host %02x, client %02x: not explained by the queue' % (
                'civ' if name == 'civs' else 'unit', i // size, i % size, h[i], c[i]))
    print('all explained by the queue\'s order' if not unexplained else '%d bytes not explained' % unexplained)


if __name__ == '__main__':
    main()
