#!/usr/bin/env python3
"""Regression tests of OpenCiv2's additions (docs/EXTRAS.md): make test-extras.

    extras_suite.py [--media] [--bless] [--twice] [NAME...]

The scripts in harness/extras replay the interface additions and rule
changes, which the original game does not have, so there is no recording of
the original to compare with.  Each is compared instead with our own output,
recorded once and checked by eye (--bless): harness/extras/expected/NAME.txt
holds a hash of each screenshot's pixels and of the rand() trace, and
NAME.rand.gz the trace itself (where a run diverges).  No image or text of the
game is kept.

A script says how it is run in lines "# test: VAR=VALUE ..." (the variables
of a test run: CIV2_TEST_EXTRAS, CIV2_TEST_SETTINGS, CIV2_TEST_FULL ...), or
"# test LABEL: ..." for further runs of it, named NAME-LABEL.

Runs inside the civ2-dev container (tools/dk), on build/$EXE (default
civ2-linux32).  Each run's screenshots (PNG) and logs are in run/extras-NAME.
On a mismatch, run/report-extras-NAME/index.html shows the screenshots that
differ; when this machine has the blessed run's screenshots
(run/extras-blessed/NAME, kept by --bless), with their differences.

--bless runs each script twice, requires both runs to be identical (the test
is deterministic), and writes the expected results.  --twice checks the same
without blessing.

--media (make test-media, media-bless): the scripts of harness/media
instead, on a copy of the game with the CD's videos and music (KINGS, Video,
Music: CIV2_KEEP_MEDIA=1, the music on the virtual clock: CIV2_TEST_MUSIC=1),
expected in harness/media/expected, their runs in run/media-NAME; calls.log
(the sounds, videos and music the game asks for) is compared too
(NAME.calls).  Skipped, with a message, where the game folder lacks those
folders; the HD runs (CIV2_HD_PACK) where there is no pack.
"""
import argparse, glob, gzip, hashlib, html, os, shutil, subprocess, sys
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'harness'))
import compare  # noqa: E402  (rand_trace: the trace as compare.py reads it)

EXTRAS = os.path.join(ROOT, 'harness', 'extras')
EXPECTED = os.path.join(EXTRAS, 'expected')
RUN = os.path.join(ROOT, 'run')
KIND = 'extras'                 # 'media' with --media
MEDIA_DIRS = ('KINGS', 'Video', 'Music')


def missing_media():
    """The CD's folders the game folder lacks (case as found)."""
    game = os.environ.get('CIV2_GAME', '/civ2')
    have = {n.lower() for n in os.listdir(game)} if os.path.isdir(game) else set()
    return [d for d in MEDIA_DIRS if d.lower() not in have]


def variants(names):
    """(name, script, {variable: value}) for each test run of the scripts."""
    out = []
    for script in sorted(glob.glob(os.path.join(EXTRAS, '*.txt'))):
        base = os.path.basename(script)[:-4]
        found = False
        for line in open(script):
            if not line.startswith('#'):
                break
            head, sep, rest = line[1:].strip().partition(':')
            words = head.split()
            if not sep or not words or words[0] != 'test' or len(words) > 2:
                continue
            found = True
            name = base + ('-' + words[1] if len(words) == 2 else '')
            env = dict(w.split('=', 1) for w in rest.split())
            if not names or name in names or base in names:
                out.append((name, script, env))
        if not found:
            sys.exit(f'{script}: no "# test:" line')
    return out


def run(script, env, out):
    """Run a script; its screenshots as PNG (the BMPs removed) and its log."""
    shutil.rmtree(out, ignore_errors=True)
    e = dict(os.environ)
    for k in list(e):
        if k.startswith('CIV2_TEST_') or k == 'CIV2_CLASSIC':
            del e[k]
    e.pop('CIV2_KEEP_MEDIA', None)
    if KIND == 'media':
        e.update(CIV2_KEEP_MEDIA='1', CIV2_TEST_MUSIC='1')
    e.update(env)
    e.setdefault('EXE', 'civ2-linux32')
    e.setdefault('TIMEOUT', '600')
    with open(out + '.log', 'w') as log:
        status = subprocess.call(['sh', os.path.join(ROOT, 'tools', 'native-run.sh'), script, out],
                                 env=e, stdout=log, stderr=subprocess.STDOUT)
    for f in glob.glob(os.path.join(out, '*.bmp')):
        Image.open(f).convert('RGB').save(f[:-4] + '.png')
        os.remove(f)
    return status


def calls(out):
    """calls.log's lines (the media tests), else None."""
    p = os.path.join(out, 'calls.log')
    if KIND != 'media':
        return None
    return open(p).read().splitlines() if os.path.exists(p) else []


def result(out):
    """{shot: 'WxH sha256'} and the rand trace of a run."""
    shots = {}
    for f in sorted(glob.glob(os.path.join(out, '*.png'))):
        im = Image.open(f).convert('RGB')
        shots[os.path.basename(f)[:-4]] = '%dx%d %s' % (im.size[0], im.size[1],
                                                       hashlib.sha256(im.tobytes()).hexdigest())
    return shots, compare.rand_trace(out) or []


def trace_hash(trace):
    return hashlib.sha256('\n'.join(trace).encode()).hexdigest()


def load_expected(name):
    p = os.path.join(EXPECTED, name + '.txt')
    if not os.path.exists(p):
        return None
    shots, rand = {}, None
    for line in open(p):
        w = line.split()
        if not w or w[0].startswith('#'):
            continue
        if w[0] == 'rand':
            rand = (int(w[1]), w[2])
        else:
            shots[w[0]] = ' '.join(w[1:])
    trace = None
    tp = os.path.join(EXPECTED, name + '.rand.gz')
    if os.path.exists(tp):
        trace = gzip.decompress(open(tp, 'rb').read()).decode().splitlines()
    return shots, rand, trace


def load_calls(name):
    p = os.path.join(EXPECTED, name + '.calls')
    return open(p).read().splitlines() if os.path.exists(p) else None


def calls_differences(got, exp):
    if exp is None:
        return ['calls.log: nothing expected (%s.calls)' % name_of_expected()]
    if got == exp:
        return []
    i = next((i for i, (x, y) in enumerate(zip(got, exp)) if x != y), min(len(got), len(exp)))
    return ['calls.log: DIFFERS at line %d: %s, expected %s' % (
        i + 1, got[i] if i < len(got) else '<end>', exp[i] if i < len(exp) else '<end>')]


def name_of_expected():
    return os.path.relpath(EXPECTED, ROOT)


def save_expected(name, env, shots, trace, call_lines=None):
    os.makedirs(EXPECTED, exist_ok=True)
    with open(os.path.join(EXPECTED, name + '.txt'), 'w') as f:
        f.write('# harness/extras_suite.py %s--bless: %s\n' % ('--media ' if KIND == 'media' else '', ' '.join('%s=%s' % kv for kv in sorted(env.items()))))
        for s, v in sorted(shots.items()):
            f.write('%s %s\n' % (s, v))
        f.write('rand %d %s\n' % (len(trace), trace_hash(trace)))
    data = ('\n'.join(trace) + '\n').encode() if trace else b''
    with open(os.path.join(EXPECTED, name + '.rand.gz'), 'wb') as f:
        f.write(gzip.compress(data, 9, mtime=0))
    if call_lines is not None:
        with open(os.path.join(EXPECTED, name + '.calls'), 'w') as f:
            f.write(''.join(l + '\n' for l in call_lines))


def differences(shots, trace, exp_shots, exp_rand, exp_trace):
    """What differs from the expected results, as lines; [] when nothing."""
    probs = []
    if not shots:
        probs.append('no screenshots')
    for s in sorted(set(shots) | set(exp_shots)):
        if s not in shots:
            probs.append(f'{s}: MISSING')
        elif s not in exp_shots:
            probs.append(f'{s}: not expected')
        elif shots[s] != exp_shots[s]:
            a, b = shots[s].split()[0], exp_shots[s].split()[0]
            probs.append(f'{s}: DIFFERS' + (f' (size {a}, expected {b})' if a != b else ''))
    if not trace:
        probs.append('rand.log: MISSING or EMPTY')
    elif exp_rand and (len(trace), trace_hash(trace)) != exp_rand:
        if exp_trace is not None:
            i = next((i for i, (x, y) in enumerate(zip(trace, exp_trace)) if x != y), min(len(trace), len(exp_trace)))
            probs.append('rand trace: DIVERGES at call %d: %s, expected %s' % (
                i, trace[i] if i < len(trace) else '<end>', exp_trace[i] if i < len(exp_trace) else '<end>'))
        else:
            probs.append('rand trace: DIFFERS (%d calls, expected %d)' % (len(trace), exp_rand[0]))
    return probs


def report(name, out, probs):
    """run/report-extras-NAME: the screenshots that differ (with the blessed
    ones and their difference, where this machine has them)."""
    rep = os.path.join(RUN, 'report-%s-%s' % (KIND, name))
    shutil.rmtree(rep, ignore_errors=True)
    blessed = os.path.join(RUN, KIND + '-blessed', name)
    if os.path.isdir(blessed):
        subprocess.call([sys.executable, os.path.join(ROOT, 'harness', 'compare.py'), blessed, out, '--report', rep],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if os.path.exists(os.path.join(rep, 'index.html')):
            return rep
    os.makedirs(rep, exist_ok=True)
    page = ['<!doctype html><meta charset=utf-8><title>Extras test %s</title>' % html.escape(name),
            '<style>body{font:14px sans-serif;background:#222;color:#eee} img{max-width:48%;border:1px solid #555}</style>',
            '<h1>%s: FAIL</h1>' % html.escape(name),
            '<p>No blessed screenshots on this machine (run/%s-blessed): these are this run\'s.</p>' % KIND]
    for p in probs:
        page.append('<h3>%s</h3>' % html.escape(p))
        shot = p.split(':')[0]
        if os.path.exists(os.path.join(out, shot + '.png')):
            shutil.copy(os.path.join(out, shot + '.png'), rep)
            page.append('<img src="%s.png">' % html.escape(shot))
    open(os.path.join(rep, 'index.html'), 'w').write('\n'.join(page))
    return rep


def main():
    global KIND, EXTRAS, EXPECTED
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--media', action='store_true', help='the media tests (harness/media; the CD\'s videos and music)')
    ap.add_argument('--bless', action='store_true', help='record the expected results (after checking the screenshots)')
    ap.add_argument('--twice', action='store_true', help='run each script twice and require identical runs')
    ap.add_argument('names', nargs='*', help='scripts (NAME or NAME-LABEL; default all)')
    a = ap.parse_args()
    if a.media:
        KIND = 'media'
        EXTRAS = os.path.join(ROOT, 'harness', 'media')
        EXPECTED = os.path.join(EXTRAS, 'expected')
        lack = missing_media()
        if lack:
            print('SKIP: the media tests need the CD\'s %s in the game folder (%s lacks %s; '
                  'docs/DEVELOPMENT.md, "Media tests")' % ('/'.join(MEDIA_DIRS), os.environ.get('CIV2_GAME', '/civ2'),
                                                           ', '.join(lack)), flush=True)
            sys.exit(0)
    os.makedirs(RUN, exist_ok=True)
    tests = variants(set(a.names))
    if not tests:
        sys.exit('no such script')
    print('testing build/%s' % os.environ.get('EXE', 'civ2-linux32'), flush=True)
    fail = 0
    for name, script, env in tests:
        pack = env.get('CIV2_HD_PACK')
        if pack and not os.path.exists(pack):
            print(f'SKIP {name} (no HD pack: {pack}; make hd)', flush=True)
            continue
        out = os.path.join(RUN, '%s-%s' % (KIND, name))
        status = run(script, env, out)
        if status:
            print(f'FAIL {name} (runner exited {status}; see {os.path.relpath(out, ROOT)}.log)', flush=True)
            fail = 1
            continue
        shots, trace = result(out)
        call_lines = calls(out)
        probs = []
        if a.bless or a.twice:
            out2 = out + '-2'
            status = run(script, env, out2)
            shots2, trace2 = result(out2)
            if status:
                probs.append(f'second run: runner exited {status}')
            elif (shots2, trace2, calls(out2)) != (shots, trace, call_lines):
                probs.append('not deterministic: the second run (%s) differs:' % os.path.relpath(out2, ROOT))
                probs += ['  ' + p for p in differences(shots2, trace2, shots, (len(trace), trace_hash(trace)), trace)]
                if call_lines is not None:
                    probs += ['  ' + p for p in calls_differences(calls(out2), call_lines)]
            shutil.rmtree(out2, ignore_errors=True)
            os.remove(out2 + '.log')
        if a.bless:
            if not probs and (not shots or not trace):
                probs.append('no screenshots or no rand trace')
            if probs:
                print(f'FAIL {name} (not blessed)', flush=True)
                for p in probs:
                    print('    ' + p)
                fail = 1
                continue
            save_expected(name, env, shots, trace, call_lines)
            blessed = os.path.join(RUN, KIND + '-blessed', name)
            shutil.rmtree(blessed, ignore_errors=True)
            shutil.copytree(out, blessed)
            print(f'BLESSED {name} ({len(shots)} screenshots, {len(trace)} rand calls)', flush=True)
            continue
        exp = load_expected(name)
        if exp is None:
            probs.append(f'nothing expected ({name_of_expected()}/{name}.txt): --bless')
        else:
            probs += differences(shots, trace, *exp)
            if call_lines is not None:
                probs += calls_differences(call_lines, load_calls(name))
        if probs:
            rep = report(name, out, probs)
            print(f'FAIL {name} (see {os.path.relpath(rep, ROOT)}/index.html)', flush=True)
            for p in probs:
                print('    ' + p)
            fail = 1
        else:
            print(f'PASS {name} ({len(shots)} screenshots, {len(trace)} rand calls)', flush=True)
    sys.exit(fail)


if __name__ == '__main__':
    main()
