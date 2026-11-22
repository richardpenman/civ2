#!/usr/bin/env python3
"""Remove old outputs from run/ (make clean-run).

    clean_run.py [--days N] [--dry-run]

Removes each entry of run/ (a folder or a file) in which nothing has changed
for N days (default 3): the newest modification time of anything inside it is
older.  Kept whatever the age:
  * an entry with a file named .keep anywhere inside it (touch run/NAME/.keep
    to keep a run's outputs);
  * run/ref-* (the original's recordings, made by make ref, which needs the
    original under Wine) and run/extras-blessed (what make extras-bless kept);
  * a browser test's work folder (run/web-tmp-*, run/web-player-*) whose test
    is still running (src/web/test/cdp.mjs: its .pid).
Prints what it removes (with --dry-run: would remove) and how much it frees.
"""
import argparse, os, shutil, stat, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RUN = os.path.join(ROOT, 'run')


def protected(name):
    return name.startswith('ref-') or name == 'extras-blessed'


def scan(path):
    """(bytes on disk, newest mtime, has .keep) of a file or a folder's tree."""
    st = os.lstat(path)
    size, newest, keep = st.st_blocks * 512, st.st_mtime, os.path.basename(path) == '.keep'
    if stat.S_ISDIR(st.st_mode):
        for d, dirs, files in os.walk(path, onerror=lambda e: None):
            for n in dirs + files:
                try:
                    s = os.lstat(os.path.join(d, n))
                except OSError:
                    continue
                size += s.st_blocks * 512
                newest = max(newest, s.st_mtime)
                keep = keep or n == '.keep'
    return size, newest, keep


def test_running(path):
    try:
        pid = int(open(os.path.join(path, '.pid')).read())
        os.kill(pid, 0)
        return True
    except (OSError, ValueError):
        return False


def human(n):
    for unit in ('B', 'KB', 'MB', 'GB'):
        if n < 1024 or unit == 'GB':
            return '%.1f %s' % (n, unit) if unit != 'B' else '%d B' % n
        n /= 1024


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--days', type=float, default=3)
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    if not os.path.isdir(RUN):
        print('no run/: nothing to do')
        return
    cutoff = time.time() - a.days * 86400
    freed = kept_size = 0
    removed, kept = [], {'.keep': 0, 'recordings': 0, 'newer': 0, 'running': 0}
    failed = []
    for name in sorted(os.listdir(RUN)):
        p = os.path.join(RUN, name)
        size, newest, keep = scan(p)
        why = ('recordings' if protected(name) else '.keep' if keep else
               'newer' if newest >= cutoff else
               'running' if name.startswith(('web-tmp-', 'web-player-')) and test_running(p) else None)
        if why:
            kept[why] += 1
            kept_size += size
            continue
        if not a.dry_run:
            try:
                if os.path.isdir(p) and not os.path.islink(p):
                    shutil.rmtree(p)
                else:
                    os.remove(p)
            except OSError as e:
                failed.append('%s: %s' % (name, e))
                continue
        removed.append((size, name))
        freed += size
    verb = 'would remove' if a.dry_run else 'removed'
    for size, name in sorted(removed, reverse=True)[:25]:
        print('  %9s  run/%s' % (human(size), name))
    if len(removed) > 25:
        print('  ... and %d more' % (len(removed) - 25))
    for f in failed:
        print('  not removed: ' + f)
    print('%s %d entries of run/ unchanged for %g days: %s %s' % (
        verb, len(removed), a.days, human(freed), 'would be freed' if a.dry_run else 'freed'))
    print('kept %d entries (%s): %d with .keep, %d recordings (ref-*, extras-blessed), %d newer, %d running tests'
          % (sum(kept.values()), human(kept_size), kept['.keep'], kept['recordings'], kept['newer'], kept['running']))
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
