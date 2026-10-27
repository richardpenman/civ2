#!/usr/bin/env python3
"""Compare two harness runs screenshot-by-screenshot.

    compare.py RUN_A RUN_B [--report DIR]

Exit status is 0 when both runs contain the same nonempty set of screenshots,
every screenshot is pixel-identical, and both nonempty rand.log traces match.
"""
import argparse, os, re, sys
from PIL import Image, ImageChops


ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
_funcs = None


def _function_tables():
    """Original function starts, plus reconstructed __impl starts -> original name."""
    global _funcs
    if _funcs is None:
        import bisect, csv
        orig = sorted((int(r['entry'], 16), r['name']) for r in
                      csv.DictReader(open(os.path.join(ROOT, 're', 'functions.tsv')), delimiter='\t'))
        recon = []
        mp = os.path.join(os.environ.get('CIV2_BUILD_DIR') or os.path.join(ROOT, 'build', 'recon'), 'recon.map')
        if os.path.exists(mp):
            for line in open(mp):
                a, n = line.split()
                n = n.lstrip('_').split('@')[0]
                recon.append((int(a, 16), n[:-6] if n.endswith('__impl') else n))
        recon.sort()
        _funcs = (orig, recon)
    return _funcs


def function_of(addr):
    import bisect
    orig, recon = _function_tables()
    table = recon if addr >= 0x800000 else orig
    i = bisect.bisect_right(table, (addr, '\uffff')) - 1
    return table[i][1] if i >= 0 else '?'


def rand_trace(run):
    """rand() calls as (calling function, value): comparable across builds."""
    p = os.path.join(run, 'rand.log')
    if not os.path.exists(p):
        return None
    out = []
    for line in open(p, errors='replace').read().rstrip('\0').splitlines():
        parts = line.split()
        if not parts:
            continue
        if len(parts) >= 2 and parts[0] != 'srand':
            # Hook runtime: call address; native builds: the function's name.
            caller = function_of(int(parts[0], 16)) if re.match(r'^[0-9a-f]{8}$', parts[0]) else parts[0]
            out.append('%s %s' % (caller, parts[1]))
        else:
            out.append(line)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('a'); ap.add_argument('b')
    ap.add_argument('--report')
    args = ap.parse_args()
    shots_a = {f for f in os.listdir(args.a) if f.endswith('.png')}
    shots_b = {f for f in os.listdir(args.b) if f.endswith('.png')}
    ok = True
    rows = []
    problems = []
    for run, shots in ((args.a, shots_a), (args.b, shots_b)):
        if not shots:
            problems.append(f'{run}: no screenshots')
    for f in sorted(shots_a | shots_b):
        if f not in shots_a:
            print(f'{f}: MISSING in {args.a}'); ok = False; rows.append((f, 'missing', None)); continue
        pb = os.path.join(args.b, f)
        if f not in shots_b:
            print(f'{f}: MISSING in {args.b}'); ok = False; rows.append((f, 'missing', None)); continue
        ia = Image.open(os.path.join(args.a, f)).convert('RGB')
        ib = Image.open(pb).convert('RGB')
        if ia.size != ib.size:
            status = f'size mismatch: {ia.size} vs {ib.size}'
            print(f'{f}: {status}'); ok = False; rows.append((f, status, None)); continue
        diff = ImageChops.difference(ia, ib)
        bbox = diff.getbbox()
        if bbox is None:
            print(f'{f}: identical'); rows.append((f, 'identical', None))
        else:
            n = sum(1 for p in diff.getdata() if p != (0, 0, 0))
            print(f'{f}: DIFFERS in {n} pixels, bbox {bbox}'); ok = False
            rows.append((f, f'{n} px differ, bbox {bbox}', diff))
    ta, tb = rand_trace(args.a), rand_trace(args.b)
    for run, trace in ((args.a, ta), (args.b, tb)):
        if trace is None:
            problems.append(f'{run}/rand.log: MISSING')
        elif not trace:
            problems.append(f'{run}/rand.log: EMPTY')
    if ta and tb:
        if ta == tb:
            print(f'rand trace: identical ({len(ta)} calls)')
        else:
            i = next((i for i, (x, y) in enumerate(zip(ta, tb)) if x != y), min(len(ta), len(tb)))
            print(f'rand trace: DIVERGES at call {i}: {ta[i] if i < len(ta) else "<end>"} vs {tb[i] if i < len(tb) else "<end>"}')
            ok = False
    if problems:
        ok = False
        for problem in problems:
            print(problem)
    if args.report:
        os.makedirs(args.report, exist_ok=True)
        html = ['<!doctype html><meta charset=utf-8><title>Civ2 comparison</title>',
                '<style>body{font:14px sans-serif;background:#222;color:#eee} img{width:32%;border:1px solid #555}</style>',
                f'<h1>{args.a} vs {args.b}</h1>',
                '<p>' + ('PASS' if ok else 'FAIL') + '</p>']
        html.extend(f'<p>{problem}</p>' for problem in problems)
        for f, status, diff in rows:
            stem = f[:-4]
            html.append(f'<h3>{f}: {status}</h3>')
            a_rel = os.path.relpath(os.path.join(args.a, f), args.report)
            b_rel = os.path.relpath(os.path.join(args.b, f), args.report)
            html.append(f'<img src="{a_rel}"><img src="{b_rel}">')
            if diff is not None:
                d = diff.point(lambda v: 255 if v else 0)
                d.save(os.path.join(args.report, f'{stem}-diff.png'))
                html.append(f'<img src="{stem}-diff.png">')
        open(os.path.join(args.report, 'index.html'), 'w').write('\n'.join(html))
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
