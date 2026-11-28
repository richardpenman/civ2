// The browser version against the desktop's (make web-parity): replays of
// the desktop's harness run in the browser version, their screenshots and
// rand() traces compared with the desktop's own expected results, to catch
// what goes wrong only in the browser (a dialog drawn grey on grey, the
// city view's shading pixelated ...).
//
//   tools/toolchain/node/bin/node src/web/test/web-parity.mjs [--list] [NAME...]
//
// The cases (CASES below, or those NAMEd):
//  - harness/extras scripts as make test-extras runs them (their "# test:"
//    lines' variables), compared with harness/extras/expected/NAME.txt
//    (each screenshot's hash, the rand() trace's hash) and NAME.rand.gz (the
//    trace itself), as harness/extras_suite.py compares the desktop's;
//  - games of the native suite (harness/scripts), compared with the
//    original's recording run/ref-NAME as make native-suite compares the
//    desktop's (skipped where this machine has no recording).
// The cases without the original's pictures (CIV2_NO_GRAPHICS=1) are
// compared instead with the desktop game's replay, build/OpenCiv2's, made
// here in Docker (tools/dk; below, x87: the expected results come from the
// 32-bit test build, whose x87 arithmetic makes the pictures drawn from the
// HD art otherwise than SSE's, the desktop game's and the browser's).
//
// The game's files: those of $CIV2_GAME (default ~/Civ2) as
// tools/native-run.sh gives them to the desktop's replay (the harness's
// CIV2.DAT and CIV.INI; KINGS, Video and Music left out unless
// CIV2_KEEP_MEDIA=1; CIV2_NO_GRAPHICS, NO_DAT, NO_TEXT, NO_RULES, NO_SOUND
// leave out what they do there), put into the page's store (IndexedDB) as
// its own import keeps them, fresh for each case; then the page loaded, its
// Click to play, the replay's script, seed and variables given to the game
// (main.js: window.civ2Test, as smoke.mjs --replay does), HD graphics by
// ?hd=N where the case has CIV2_TEST_HD=N (else ?hd=0, which a replay
// ignores: the page then need not fetch the art).
//
// Each case: PASS or FAIL with what differs; on a mismatch
// run/web-parity/NAME/ has, for each screenshot that differs, SHOT-web.png
// (the browser's), SHOT-desktop.png (the desktop's, where this machine has
// it: run/extras-blessed/NAME, kept by make extras-bless, run/ref-NAME or
// build/OpenCiv2's run in run/web-parity-desktop/NAME) and SHOT-diff.png
// (the pixels that differ, white), and trace.txt with the first differing
// rand() call; the browser's whole output (BMP screenshots, rand.log,
// hook.log) is in run/web-parity/NAME/out.  Exit status 1 when a case
// fails.
//
// What the runner does differently from the desktop's harness, and why
// (nothing compared more loosely):
//  - the game's *.log files are not copied in (the page's own import leaves
//    them out too);
//  - the game's folder: the page keeps it at /civ2, the desktop's replay
//    at /tmp/g, which the game's file dialog shows (Z:\tmp\g): the files
//    are copied to /tmp/g in the page's file system and the game started
//    there (civ2Test's args);
//  - the browser draws into a page 1280x800 (the replay's own desktop size
//    is set by the game: CIV2_TEST_FULL, or the replay's default);
//  - the screenshots are the game's logical frames (replay.c's `shot`),
//    not the page's canvas, as on the desktop.

import {readFileSync, writeFileSync, mkdirSync, readdirSync, rmSync, existsSync, statSync, copyFileSync} from 'node:fs';
import {join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {homedir} from 'node:os';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {gunzipSync, inflateSync, deflateSync} from 'node:zlib';
import {ROOT, WEB, serve, browser, tmpDir, clickPlay} from './cdp.mjs';

// The representative set (some 7 minutes): with the original's files and
// without (no install: the 64-bit desktop's replay made too, below),
// Classic and HD graphics, sharp text, translations (Latin-1, Greek,
// Chinese), the real file dialog, Settings, the city view in HD, two
// games of the native suite.
const CASES = [
    'x-basic', 'x-b20-buttons', 'x-b20-buttons-classic', 'x-options', 'x-options-sharp',
    'x-sharp', 'x-own-text', 'x-lang-de', 'x-lang-el-sharp', 'x-lang-zh',
    'x-no-install', 'x-no-install-hd', 'x-no-install2-hd', 'x-no-install-options',
    'suite:newgame', 'suite:citywindow', 'suite:talks',
];

const OUT = join(ROOT, 'run', 'web-parity');
const GAME = process.env.CIV2_GAME || join(homedir(), 'Civ2');
const EXTRAS = join(ROOT, 'harness', 'extras');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(s);

// ---- the cases ----

// {name, script (relative to harness/), env, expected: 'extras' | 'suite'}
// for every test run of harness/extras (as extras_suite.py's variants)
function extrasCases() {
    const out = [];
    for (const f of readdirSync(EXTRAS).filter((n) => n.endsWith('.txt')).sort()) {
        const base = f.slice(0, -4);
        for (const line of readFileSync(join(EXTRAS, f), 'latin1').split('\n')) {
            if (!line.startsWith('#'))
                break;
            const s = line.slice(1).trim(), i = s.indexOf(':');
            if (i < 0)
                continue;
            const words = s.slice(0, i).trim().split(/\s+/);
            if (words[0] !== 'test' || words.length > 2)
                continue;
            const env = Object.fromEntries(s.slice(i + 1).trim().split(/\s+/).filter(Boolean)
                .map((w) => [w.slice(0, w.indexOf('=')), w.slice(w.indexOf('=') + 1)]));
            out.push({name: base + (words[1] ? '-' + words[1] : ''), script: 'extras/' + f, env, expected: 'extras'});
        }
    }
    return out;
}

function findCase(name, extras) {
    const m = /^suite:(.+)$/.exec(name);
    if (m) {
        if (!existsSync(join(ROOT, 'harness', 'scripts', m[1] + '.txt')))
            throw new Error('no such script: harness/scripts/' + m[1] + '.txt');
        return {name: m[1], script: 'scripts/' + m[1] + '.txt', env: {}, expected: 'suite'};
    }
    const c = extras.find((x) => x.name === name);
    if (!c)
        throw new Error('no such case: ' + name + ' (harness/extras NAME or NAME-LABEL, or suite:NAME)');
    return c;
}

// ---- the game's files, as tools/native-run.sh gives them ----

function gameFiles(env) {
    const textNames = new Set(readdirSync(join(ROOT, 'data', 'native', 'text'))
        .filter((n) => /\.TXT$/i.test(n)).map((n) => n.toLowerCase()));
    const files = [];   // [name in the game's folder, path on disk]
    const walk = (dir, rel) => {
        for (const n of readdirSync(dir).sort()) {
            const p = join(dir, n), r = rel ? rel + '/' + n : n, lo = n.toLowerCase();
            const top = !rel;
            if (statSync(p).isDirectory()) {
                if (top && !env.CIV2_KEEP_MEDIA && ['kings', 'video', 'music'].includes(lo))
                    continue;
                if (top && env.CIV2_NO_SOUND && lo === 'sound')
                    continue;
                walk(p, r);
                continue;
            }
            if (/\.log$/i.test(n))
                continue;           // (as the page's import: main.js findGame)
            if (top && lo.endsWith('.dat'))
                continue;           // (the harness's CIV2.DAT instead, below)
            if (top && env.CIV2_NO_GRAPHICS && (/\.(gif|dll)$/.test(lo) || lo === 'civ2.exe'))
                continue;
            if (top && env.CIV2_NO_RULES && lo === 'rules.txt')
                continue;
            if (top && env.CIV2_NO_TEXT && textNames.has(lo))
                continue;
            if (top && lo === 'civ.ini')
                continue;           // (the harness's)
            files.push([r, p]);
        }
    };
    walk(GAME, '');
    // (CIV2_NO_DAT: none, as on a fresh install; else the harness's CIV2.DAT)
    if (!env.CIV2_NO_DAT)
        files.push(['CIV2.DAT', join(ROOT, 'harness', 'CIV2.DAT')]);
    if (existsSync(join(ROOT, 'harness', 'CIV.INI')))
        files.push(['CIV.INI', join(ROOT, 'harness', 'CIV.INI')]);
    return files;
}

// ---- images: BMP and PNG decoded to RGB, PNG written ----

export function bmpRgb(b) {
    if (b.toString('latin1', 0, 2) !== 'BM')
        throw new Error('not a BMP');
    const off = b.readUInt32LE(10), w = b.readInt32LE(18), h0 = b.readInt32LE(22), bpp = b.readUInt16LE(28);
    const h = Math.abs(h0), stride = Math.ceil(w * bpp / 32) * 4, rgb = Buffer.alloc(w * h * 3);
    const ncol = b.readUInt32LE(46) || (bpp <= 8 ? 1 << bpp : 0), pal = 14 + b.readUInt32LE(14);
    for (let y = 0; y < h; y++) {
        const row = off + (h0 > 0 ? h - 1 - y : y) * stride;
        for (let x = 0; x < w; x++) {
            const o = (y * w + x) * 3;
            if (bpp === 24 || bpp === 32) {
                const p = row + x * bpp / 8;
                rgb[o] = b[p + 2]; rgb[o + 1] = b[p + 1]; rgb[o + 2] = b[p];
            } else if (bpp === 8) {
                const i = b[row + x];
                if (i >= ncol)
                    throw new Error('BMP colour out of the palette');
                rgb[o] = b[pal + i * 4 + 2]; rgb[o + 1] = b[pal + i * 4 + 1]; rgb[o + 2] = b[pal + i * 4];
            } else
                throw new Error('BMP of ' + bpp + ' bits');
        }
    }
    return {w, h, rgb};
}

export function pngRgb(b) {
    let p = 8, w = 0, h = 0, depth = 0, type = 0, plte = null, trns = null;
    const idat = [];
    while (p < b.length) {
        const len = b.readUInt32BE(p), kind = b.toString('latin1', p + 4, p + 8), d = b.subarray(p + 8, p + 8 + len);
        if (kind === 'IHDR') {
            w = d.readUInt32BE(0); h = d.readUInt32BE(4); depth = d[8]; type = d[9];
            if (d[12])
                throw new Error('interlaced PNG');
        } else if (kind === 'PLTE')
            plte = d;
        else if (kind === 'tRNS')
            trns = d;
        else if (kind === 'IDAT')
            idat.push(d);
        p += 12 + len;
    }
    void trns;
    const ch = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}[type], bpp = Math.max(1, ch * depth / 8);
    const stride = Math.ceil(w * ch * depth / 8), raw = inflateSync(Buffer.concat(idat));
    const cur = Buffer.alloc(stride), prev = Buffer.alloc(stride), rgb = Buffer.alloc(w * h * 3);
    for (let y = 0; y < h; y++) {
        const f = raw[y * (stride + 1)], line = raw.subarray(y * (stride + 1) + 1, (y + 1) * (stride + 1));
        for (let i = 0; i < stride; i++) {
            const a = i >= bpp ? cur[i - bpp] : 0, up = prev[i], c = i >= bpp ? prev[i - bpp] : 0;
            let v = line[i];
            if (f === 1) v += a;
            else if (f === 2) v += up;
            else if (f === 3) v += (a + up) >> 1;
            else if (f === 4) {
                const pp = a + up - c, pa = Math.abs(pp - a), pb = Math.abs(pp - up), pc = Math.abs(pp - c);
                v += pa <= pb && pa <= pc ? a : pb <= pc ? up : c;
            }
            cur[i] = v & 255;
        }
        for (let x = 0; x < w; x++) {
            const o = (y * w + x) * 3;
            const sample = (k) => depth === 8 ? cur[x * ch + k] : depth === 16 ? cur[(x * ch + k) * 2] :
                (cur[(x * depth) >> 3] >> (8 - depth - ((x * depth) & 7))) & ((1 << depth) - 1);
            if (type === 3) {
                const i = sample(0);
                rgb[o] = plte[i * 3]; rgb[o + 1] = plte[i * 3 + 1]; rgb[o + 2] = plte[i * 3 + 2];
            } else if (type === 0 || type === 4) {
                const g = depth < 8 ? sample(0) * 255 / ((1 << depth) - 1) : sample(0);
                rgb[o] = rgb[o + 1] = rgb[o + 2] = g;
            } else {
                rgb[o] = sample(0); rgb[o + 1] = sample(1); rgb[o + 2] = sample(2);
            }
        }
        cur.copy(prev);
    }
    return {w, h, rgb};
}

const CRC = new Int32Array(256).map((_, n) => {
    let c = n;
    for (let k = 0; k < 8; k++)
        c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    return c;
});
function crc32(b) {
    let c = -1;
    for (let i = 0; i < b.length; i++)
        c = CRC[(c ^ b[i]) & 0xff] ^ (c >>> 8);
    return (c ^ -1) >>> 0;
}
export function pngOf({w, h, rgb}) {
    const chunk = (kind, d) => {
        const out = Buffer.alloc(12 + d.length);
        out.writeUInt32BE(d.length, 0);
        out.write(kind, 4, 'latin1');
        d.copy(out, 8);
        out.writeUInt32BE(crc32(out.subarray(4, 8 + d.length)), 8 + d.length);
        return out;
    };
    const ihdr = Buffer.alloc(13);
    ihdr.writeUInt32BE(w, 0); ihdr.writeUInt32BE(h, 4); ihdr[8] = 8; ihdr[9] = 2;
    const raw = Buffer.alloc(h * (w * 3 + 1));
    for (let y = 0; y < h; y++)
        rgb.copy(raw, y * (w * 3 + 1) + 1, y * w * 3, (y + 1) * w * 3);
    return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr),
                          chunk('IDAT', deflateSync(raw)), chunk('IEND', Buffer.alloc(0))]);
}

// 'WxH sha256' of the pixels as RGB, as harness/extras_suite.py's result()
const shotHash = ({w, h, rgb}) => `${w}x${h} ${createHash('sha256').update(rgb).digest('hex')}`;

// ---- rand() traces, as harness/compare.py's rand_trace ----

let funcs = null;
function functionOf(addr) {
    if (!funcs) {
        const table = (lines) => lines.sort((a, b) => a[0] - b[0]);
        const tsv = readFileSync(join(ROOT, 're', 'functions.tsv'), 'latin1').split('\n').slice(1)
            .filter(Boolean).map((l) => l.split('\t')).map((r) => [parseInt(r[0], 16), r[2]]);
        const mp = join(process.env.CIV2_BUILD_DIR || join(ROOT, 'build', 'recon'), 'recon.map');
        const recon = existsSync(mp) ? readFileSync(mp, 'latin1').split('\n').filter(Boolean).map((l) => {
            const [a, n0] = l.split(/\s+/);
            const n = n0.replace(/^_+/, '').split('@')[0];
            return [parseInt(a, 16), n.endsWith('__impl') ? n.slice(0, -6) : n];
        }) : [];
        funcs = [table(tsv), table(recon)];
    }
    const t = addr >= 0x800000 ? funcs[1] : funcs[0];
    let lo = 0, hi = t.length - 1, k = -1;
    while (lo <= hi) {
        const m = (lo + hi) >> 1;
        if (t[m][0] <= addr) { k = m; lo = m + 1; } else hi = m - 1;
    }
    return k >= 0 ? t[k][1] : '?';
}

export function randTrace(text) {
    const out = [];
    for (const line of text.replace(/\0+$/, '').split(/\r\n|[\n\r\v\f\x1c\x1d\x1e\x85\u2028\u2029]/)) {
        const parts = line.trim().split(/\s+/).filter(Boolean);
        if (!parts.length)
            continue;
        if (parts.length >= 2 && parts[0] !== 'srand')
            out.push(`${/^[0-9a-f]{8}$/.test(parts[0]) ? functionOf(parseInt(parts[0], 16)) : parts[0]} ${parts[1]}`);
        else
            out.push(line);
    }
    return out;
}
const traceHash = (t) => createHash('sha256').update(t.join('\n')).digest('hex');

// ---- what is expected: the desktop's ----

// The cases without the original's pictures (CIV2_NO_GRAPHICS=1), whose
// expected results the 32-bit test build's x87 arithmetic makes
// (harness/extras/expected comes from build/civ2-linux32): the pictures
// made from the HD art (synth.c's dithering in float, hd.c's scaling in
// double) come out otherwise in a build with SSE arithmetic, as the
// browser's module and the desktop game (build/OpenCiv2, x86-64) are.
// These are compared with build/OpenCiv2's replay of the same script
// instead, made here (tools/native-run.sh in the civ2-dev container:
// tools/dk, Docker; skipped without it).
const x87 = (c) => c.expected === 'extras' && c.env.CIV2_NO_GRAPHICS;

function desktopRun(c) {
    const out = join(ROOT, 'run', 'web-parity-desktop', c.name);
    rmSync(out, {recursive: true, force: true});
    mkdirSync(out, {recursive: true});
    if (!existsSync(join(ROOT, 'build', 'OpenCiv2')))
        return {skip: 'no build/OpenCiv2 (make port) to compare with'};
    const env = {...process.env, EXE: 'OpenCiv2', TIMEOUT: '600'};
    for (const k of Object.keys(env))
        if (/^CIV2_/.test(k))
            delete env[k];
    // (the case's variables set inside the container: tools/dk passes on
    // only some of them)
    const r = spawnSync('sh', [join(ROOT, 'tools', 'dk'), 'env', ...Object.entries(c.env).map(([k, v]) => k + '=' + v),
                               'sh', 'tools/native-run.sh', '/work/harness/' + c.script,
                               '/work/run/web-parity-desktop/' + c.name], {env, encoding: 'latin1'});
    writeFileSync(out + '.log', (r.stdout || '') + (r.stderr || ''));
    if (r.status !== 0 || !existsSync(join(out, 'rand.log')))
        return {skip: `build/OpenCiv2's replay not made (Docker: tools/dk; see run/web-parity-desktop/${c.name}.log)`};
    return out;
}

function expected(c) {
    if (x87(c)) {
        const ref = desktopRun(c);
        if (ref.skip)
            return ref;
        return {...runResult(ref), images: ref, against: 'build/OpenCiv2'};
    }
    if (c.expected === 'extras') {
        const p = join(EXTRAS, 'expected', c.name + '.txt');
        if (!existsSync(p))
            return {skip: 'nothing expected (harness/extras/expected/' + c.name + '.txt)'};
        const shots = {};
        let rand = null;
        for (const line of readFileSync(p, 'latin1').split('\n')) {
            const w = line.trim().split(/\s+/);
            if (!w[0] || w[0].startsWith('#'))
                continue;
            if (w[0] === 'rand')
                rand = [parseInt(w[1], 10), w[2]];
            else
                shots[w[0]] = w.slice(1).join(' ');
        }
        const tp = join(EXTRAS, 'expected', c.name + '.rand.gz');
        const trace = existsSync(tp) ? gunzipSync(readFileSync(tp)).toString('latin1').split('\n').filter((l) => l !== '') : null;
        const blessed = join(ROOT, 'run', 'extras-blessed', c.name);
        return {shots, rand, trace, images: existsSync(blessed) ? blessed : null,
                against: 'harness/extras/expected'};
    }
    const ref = join(ROOT, 'run', 'ref-' + c.name);
    if (!existsSync(join(ref, 'rand.log')))
        return {skip: 'no recording of the original (run/ref-' + c.name + ': make ref SCRIPT=' + c.name + ')'};
    return {...runResult(ref), images: ref, against: 'run/ref-' + c.name};
}

// {shots, rand, trace} of a run's folder (PNG screenshots, rand.log)
function runResult(ref) {
    const shots = {};
    for (const f of readdirSync(ref).filter((n) => n.endsWith('.png')).sort())
        shots[f.slice(0, -4)] = shotHash(pngRgb(readFileSync(join(ref, f))));
    const trace = randTrace(readFileSync(join(ref, 'rand.log'), 'latin1'));
    return {shots, rand: [trace.length, traceHash(trace)], trace};
}

// ---- the browser ----

async function main() {
    const argv = process.argv.slice(2);
    const extras = extrasCases();
    if (argv[0] === '--list') {
        for (const c of extras)
            log(c.name + (CASES.includes(c.name) ? ' *' : ''));
        return;
    }
    const cases = (argv.length ? argv : CASES).map((n) => findCase(n, extras));
    if (!existsSync(join(WEB, 'civ2.wasm')))
        throw new Error('no build/web: make web first');
    if (!existsSync(GAME))
        throw new Error('no game files in ' + GAME + ' (CIV2_GAME)');
    mkdirSync(OUT, {recursive: true});
    const work = tmpDir();   // (run/web-tmp-*: cdp.mjs)
    let served = new Map();
    const server = await serve((path) => path.startsWith('/__game/') ? served.get(path.slice(8)) : null);
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(work.path, 'profile'), undefined, url);
    const {send} = b;
    const errors = [];
    b.on((m) => {
        if (m.method === 'Runtime.exceptionThrown') {
            errors.push(m.params.exceptionDetails);
            log('  page exception: ' + JSON.stringify(m.params.exceptionDetails).slice(0, 300));
        }
    });
    const evaluate = async (expr) => {
        const r = await send('Runtime.evaluate', {expression: expr, awaitPromise: true, returnByValue: true});
        if (r.exceptionDetails)
            throw new Error('in the page: ' + JSON.stringify(r.exceptionDetails).slice(0, 500));
        return r.result.value;
    };
    const waitFor = async (expr, ms, what) => {
        const end = performance.now() + ms;
        while (performance.now() < end) {
            const v = await evaluate(expr).catch(() => null);
            if (v)
                return v;
            await sleep(250);
        }
        throw new Error('timed out: ' + what);
    };
    const results = [];
    const t0 = performance.now();
    let script = null;
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        for (const c of cases) {
            const t = performance.now();
            const exp = expected(c);
            const dir = join(OUT, c.name);
            rmSync(dir, {recursive: true, force: true});
            if (exp.skip) {
                log(`SKIP ${c.name} (${exp.skip})`);
                results.push([c.name, 'SKIP']);
                continue;
            }
            const out = join(dir, 'out');
            mkdirSync(out, {recursive: true});
            let probs;
            try {
                probs = await runCase(c, exp, out);
            } catch (e) {
                probs = ['the run: ' + e.message];
            }
            const secs = ((performance.now() - t) / 1000).toFixed(0);
            if (probs.length) {
                log(`FAIL ${c.name} (${secs} s, against ${exp.against}; see run/web-parity/${c.name})`);
                for (const p of probs)
                    log('    ' + p);
                writeFileSync(join(dir, 'problems.txt'), probs.join('\n') + '\n');
                results.push([c.name, 'FAIL']);
            } else {
                log(`PASS ${c.name} (${secs} s, against ${exp.against})`);
                rmSync(dir, {recursive: true, force: true});
                results.push([c.name, 'PASS']);
            }
        }
    } finally {
        await b.close();
        server.close();
        await sleep(300);
        work.remove();
    }
    const n = (k) => results.filter((r) => r[1] === k).length;
    log(`web-parity: ${n('PASS')} passed, ${n('FAIL')} failed, ${n('SKIP')} skipped in ` +
        `${((performance.now() - t0) / 60000).toFixed(1)} min`);
    if (n('FAIL'))
        process.exitCode = 1;

    // One case: the store filled with the game's files, the page loaded,
    // the replay run, its output compared -> the problems ([] when none)
    async function runCase(c, exp, out) {
        const env = c.env;
        // the game's files, served at /__game/N, put into the page's store
        // (IndexedDB, as its import keeps them; store.js) instead of what
        // the last case left there
        const files = gameFiles(env);
        served = new Map(files.map(([, p], i) => [String(i), p]));
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        const kept = await evaluate(`(async () => {
            const {openStore} = await import('/store.js');
            const store = await openStore();
            await store.clear();
            const names = ${JSON.stringify(files.map(([n]) => n))};
            let batch = [], size = 0;
            for (let i = 0; i < names.length; i++) {
                const r = await fetch('/__game/' + i);
                if (!r.ok)
                    throw new Error('not served: ' + names[i]);
                const data = new Uint8Array(await r.arrayBuffer());
                batch.push([names[i], data, Date.now()]);
                size += data.length;
                if (size > 16 << 20 || i === names.length - 1) {
                    await store.putMany(batch);
                    batch = [];
                    size = 0;
                }
            }
            await store.setMeta('game', {names, at: Date.now()});
            return store.count();
        })()`);
        if (kept !== files.length)
            throw new Error(`${kept} files in the store, not ${files.length}`);
        // the replay's files and variables, as tools/native-run.sh's
        const vars = {CIV2_SCRIPT: '/replay/script.txt', CIV2_SHOT_DIR: '/replay/out', CIV2_SEED: '7',
                      CIV2_RAND_LOG: '/replay/out/rand.log', CIV2_HOOK_LOG: '/replay/out/hook.log',
                      CIV2_CALL_LOG: '/replay/out/calls.log', CIV2_WINDIR: '/tmp/g'};
        if (!env.CIV2_PLAY_TEXT)
            vars.CIV2_WINE_TEXT = '1';
        for (const [k, v] of Object.entries(env))
            if (!/^CIV2_(NO_\w+|KEEP_MEDIA)$/.test(k))
                vars[k] = v;
        if (script)
            await send('Page.removeScriptToEvaluateOnNewDocument', {identifier: script});
        ({identifier: script} = await send('Page.addScriptToEvaluateOnNewDocument', {source: `
            window.civ2Test = async (fs, env, args) => {
                const r = await fetch('/__test/${c.script}');
                fs.put('/replay/script.txt', new Uint8Array(await r.arrayBuffer()));
                // (the game's folder where tools/native-run.sh has it, /tmp/g,
                // which the file dialog shows: its files there too)
                for (const d of [...fs.dirs])
                    if (d === '/civ2' || d.startsWith('/civ2/'))
                        fs.mkdirs('/tmp/g' + d.slice(5));
                for (const [p, f] of [...fs.files])
                    if (p.startsWith('/civ2/'))
                        fs.files.set('/tmp/g' + p.slice(5), {...f});
                args[1] = '/tmp/g';
                fs.mkdirs('/replay/out');
                Object.assign(env, ${JSON.stringify(vars)});
            };`}));
        const hd = env.CIV2_TEST_HD || '0';
        await send('Page.navigate', {url: url + '?hd=' + hd});
        await clickPlay(send);
        await waitFor('/ended|stopped/.test(window.civ2Status || "") && window.civ2Status', 1800000, 'the replay');
        const st = await evaluate('window.civ2Status');
        // its output, out of the page's file system
        const names = await evaluate('window.civ2Fs.list("/replay/out").filter((e) => !e[1]).map((e) => e[0])');
        for (const n of names) {
            const b64 = await evaluate(`(() => { const d = window.civ2Fs.get('/replay/out/${n}'); let s = '';
                for (let i = 0; i < d.length; i += 32768) s += String.fromCharCode(...d.subarray(i, i + 32768));
                return btoa(s); })()`);
            writeFileSync(join(out, n), Buffer.from(b64, 'base64'));
        }
        // the page's start screen again (the game ended): its memory freed
        await send('Page.navigate', {url: 'about:blank'});
        return compare(c, exp, out, st);
    }
}

// ---- the comparison (as extras_suite.py's differences) ----

function compare(c, exp, out, status) {
    const probs = [];
    const dir = join(out, '..');
    if (/stopped/.test(status))
        probs.push('the game stopped: ' + status);
    const shots = {}, images = {};
    for (const f of readdirSync(out).filter((n) => /\.bmp$/i.test(n)).sort()) {
        const name = f.slice(0, -4);
        try {
            images[name] = bmpRgb(readFileSync(join(out, f)));
            shots[name] = shotHash(images[name]);
        } catch (e) {
            probs.push(`${name}: not read (${e.message})`);
        }
    }
    if (!Object.keys(shots).length)
        probs.push('no screenshots');
    for (const s of [...new Set([...Object.keys(shots), ...Object.keys(exp.shots)])].sort()) {
        if (!(s in shots))
            probs.push(`${s}: MISSING`);
        else if (!(s in exp.shots))
            probs.push(`${s}: not expected`);
        else if (shots[s] !== exp.shots[s]) {
            const [a, e] = [shots[s].split(' ')[0], exp.shots[s].split(' ')[0]];
            writeFileSync(join(dir, s + '-web.png'), pngOf(images[s]));
            const desk = exp.images && join(exp.images, s + '.png');
            let note = '';
            if (desk && existsSync(desk)) {
                copyFileSync(desk, join(dir, s + '-desktop.png'));
                const d = pngRgb(readFileSync(desk));
                if (shotHash(d) !== exp.shots[s])
                    note = ' (the desktop\'s picture on this machine is not the expected one: blessed since?)';
                else if (d.w === images[s].w && d.h === images[s].h) {
                    let n = 0, x0 = 1e9, y0 = 1e9, x1 = -1, y1 = -1;
                    const diff = Buffer.alloc(d.rgb.length);
                    for (let i = 0; i < d.rgb.length; i += 3)
                        if (d.rgb[i] !== images[s].rgb[i] || d.rgb[i + 1] !== images[s].rgb[i + 1] || d.rgb[i + 2] !== images[s].rgb[i + 2]) {
                            n++;
                            diff[i] = diff[i + 1] = diff[i + 2] = 255;
                            const x = (i / 3) % d.w, y = Math.floor(i / 3 / d.w);
                            x0 = Math.min(x0, x); x1 = Math.max(x1, x); y0 = Math.min(y0, y); y1 = Math.max(y1, y);
                        }
                    writeFileSync(join(dir, s + '-diff.png'), pngOf({w: d.w, h: d.h, rgb: diff}));
                    note = ` (${n} pixels, in ${x0},${y0} to ${x1},${y1})`;
                }
            } else
                note = ' (no picture of the desktop\'s on this machine: only its hash)';
            probs.push(`${s}: DIFFERS` + (a !== e ? ` (size ${a}, expected ${e})` : '') + note);
        }
    }
    const tp = join(out, 'rand.log');
    const trace = existsSync(tp) ? randTrace(readFileSync(tp, 'latin1')) : [];
    if (!trace.length)
        probs.push('rand.log: MISSING or EMPTY');
    else if (exp.rand && (trace.length !== exp.rand[0] || traceHash(trace) !== exp.rand[1])) {
        let line;
        if (exp.trace) {
            let i = 0;
            while (i < trace.length && i < exp.trace.length && trace[i] === exp.trace[i])
                i++;
            line = `rand trace: DIVERGES at call ${i}: ${trace[i] ?? '<end>'}, expected ${exp.trace[i] ?? '<end>'}`;
        } else
            line = `rand trace: DIFFERS (${trace.length} calls, expected ${exp.rand[0]})`;
        probs.push(line);
        writeFileSync(join(dir, 'trace.txt'), line + '\n');
    }
    return probs;
}

// (run, not imported)
if (process.argv[1] && fileURLToPath(import.meta.url) === resolve(process.argv[1]))
    main().catch((e) => {
        console.error(e);
        process.exit(1);
    });
