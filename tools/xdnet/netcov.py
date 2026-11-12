#!/usr/bin/env python3
"""Coverage of the game's network code by network test runs.

nettest.py runs the coverage build (--exe civ2-linux32cov) with its gcov
data redirected to run/xd/cov/RUN-ROLE/ (GCOV_PREFIX), so the replays' data
in build/native/linux32cov stays as it is.  This merges the directories
given (all of run/xd/cov/ by default) and prints, for the network functions
(net_*, parley_*, lobby_*, chat_*, mp_*, xd*), how many ran and the ones that
never did; --classes also counts the network classes' member functions
(LobbyList::, NetSession::, ParleyWindow::, NetGamelistView::, Net*Msg::).

usage (in the container, from the project root):
  python3 tools/xdnet/netcov.py [--classes] [--tsv OUT] [DIR...]
"""
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
OBJ = os.path.join(ROOT, 'build', 'native', 'linux32cov')
SOURCES = sorted(os.path.relpath(f, ROOT) for f in glob.glob(os.path.join(ROOT, 'src/auto/*.cpp'))) + \
    ['src/platform/xdaemon.c', 'src/host/host_net.c']
NET = re.compile(r'^(net_|parley_|lobby_|chat_|mp_|XD_|xd_|host_net_)')
CLASSES = re.compile(r'^(LobbyList|NetSession|ParleyWindow|NetGamelistView|NetMsgQueue|Net\w*Msg|NetMsgHeader)::')


def main():
    args = sys.argv[1:]
    tsv = None
    net = NET
    if args[:1] == ['--classes']:
        net, args = re.compile(NET.pattern + '|' + CLASSES.pattern), args[1:]
    if args[:1] == ['--tsv']:
        tsv, args = args[1], args[2:]
    dirs = args or sorted(glob.glob(os.path.join(ROOT, 'run/xd/cov/*')))
    dirs = [d for d in dirs if glob.glob(os.path.join(d, '*.gcda'))]
    if not dirs:
        sys.exit('no gcov data (run nettest.py with --exe civ2-linux32cov)')
    tmp = tempfile.mkdtemp()
    try:
        merged = os.path.join(tmp, 'm')
        # merge the runs' .gcda (gcov-tool merges two directories at a time)
        shutil.copytree(dirs[0], merged)
        for d in dirs[1:]:
            out = os.path.join(tmp, 'o')
            subprocess.run(['gcov-tool', 'merge', '-o', out, d, merged], check=True,
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            shutil.rmtree(merged)
            os.rename(out, merged)
        rows = []
        for src in SOURCES:
            base = src.replace('/', '_')
            gcda = os.path.join(merged, base + '.gcda')
            if not os.path.exists(gcda):
                continue
            work = os.path.join(tmp, 'w')
            os.makedirs(work, exist_ok=True)
            shutil.copy(gcda, os.path.join(work, base + '.gcda'))
            shutil.copy(os.path.join(OBJ, base + '.gcno'), os.path.join(work, base + '.gcno'))
            out = subprocess.run(['gcov', '-f', '-m', '-n', '-o', os.path.join(work, base + '.o'), src],
                                 cwd=ROOT, capture_output=True, text=True).stdout
            for m in re.finditer(r"Function '([^']+)'\nLines executed:([\d.]+)% of (\d+)", out):
                name = m.group(1).split('(')[0]
                if net.match(name):
                    n = int(m.group(3))
                    rows.append((name, round(float(m.group(2)) * n / 100), n, src))
    finally:
        shutil.rmtree(tmp)
    rows.sort()
    ran = [r for r in rows if r[1]]
    lines = sum(r[2] for r in rows)
    print('network functions executed: %d of %d (%.0f%%); their lines: %.0f%%   (%d runs)' % (
        len(ran), len(rows), 100.0 * len(ran) / max(1, len(rows)), 100.0 * sum(r[1] for r in rows) / max(1, lines),
        len(dirs)))
    print('never executed:', ' '.join(r[0] for r in rows if not r[1]))
    if tsv:
        with open(tsv, 'w') as f:
            f.write('function\texecuted\tlines\tfile\n')
            for r in rows:
                f.write('%s\t%d\t%d\t%s\n' % r)


if __name__ == '__main__':
    main()
