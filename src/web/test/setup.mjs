// The browser version's set-up from the CD or a disc image (build/web/
// setup.wasm, src/web/setup_run.js) under node, on a made-up CD
// (disc.mjs): a disc image (.cue, .bin: the data track's cabinet and
// videos, two audio tracks) and the CD's files as a folder.  Checks the
// game unpacked as the cabinet has it, the videos copied, the music encoded
// (Ogg Vorbis: its pages, its length), the set-up's temporary files gone.
//   tools/toolchain/node/bin/node src/web/test/setup.mjs   (after make web)
import assert from 'node:assert/strict';
import {readFileSync, existsSync} from 'node:fs';
import {join, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runSetup, findDisc} from '../setup_run.js';
import {sampleDisc, makeImage, tone, RAW} from './disc.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..', '..');
const WASM = process.env.SETUP_WASM || join(ROOT, 'build', 'web', 'setup.wasm');
if (!existsSync(WASM))
    throw new Error('no build/web/setup.wasm: make web first');
const module = new WebAssembly.Module(readFileSync(WASM));

const mem = (name, data) => ({name, size: data.length, readAt: (pos, n) => data.subarray(pos, pos + n)});

function run(files, source) {
    const out = new Map(), messages = [], progress = new Set();
    const t = performance.now();
    const code = runSetup({module, files, source,
                           onFile: (path, data) => out.set(path, data),
                           onProgress: (what) => {
                               progress.add(what);
                               return false;
                           },
                           onMessage: (text, kind) => messages.push(kind + ': ' + text),
                           log: (fd, line) => console.log('  setup.wasm:', line)});
    return {code, out, messages, progress, ms: performance.now() - t};
}

// Ogg pages: their count, the last one's granule position (samples), its end flag
function ogg(data) {
    const v = new DataView(data.buffer, data.byteOffset, data.length);
    let p = 0, pages = 0, granule = 0n, eos = false;
    while (p < data.length) {
        assert.equal(new TextDecoder().decode(data.subarray(p, p + 4)), 'OggS');
        granule = v.getBigInt64(p + 6, true);
        eos = (data[p + 5] & 4) !== 0;
        const segs = data[p + 26];
        let len = 27 + segs;
        for (let i = 0; i < segs; i++)
            len += data[p + 27 + i];
        p += len;
        pages++;
    }
    return {pages, samples: Number(granule), eos};
}

const disc = sampleDisc();

// the disc image: two audio tracks (3 s, 2 s) after the data track
const {cue, bin} = makeImage(disc.iso, [tone(3, 440), tone(2, 660)], 'Civ2.bin');
const files = [mem('MGE/Civ2.cue', new TextEncoder().encode(cue)), mem('MGE/Civ2.bin', bin)];
assert.deepEqual(findDisc(files), {kind: 'image', source: 'MGE/Civ2.cue'});
let r = run(files, 'MGE/Civ2.cue');
console.log(`disc image (${(bin.length / RAW) | 0} sectors): ${r.out.size} files in ${Math.round(r.ms)} ms; ` +
            `${[...r.progress].join('; ')}`);
assert.equal(r.code, 0, r.messages.join('\n'));
for (const [k, v] of Object.entries(disc.expect))
    assert.deepEqual(r.out.get(k), v, k);
for (const [k, s] of [['Music/Track02.ogg', 3], ['Music/Track03.ogg', 2]]) {
    const o = ogg(r.out.get(k));
    assert.ok(o.eos && Math.abs(o.samples - s * 44100) <= 588 * 2, `${k}: ${JSON.stringify(o)}`);
}
assert.ok(r.out.has('OpenCiv2-setup.txt'));
assert.deepEqual([...r.out.keys()].filter((k) => /cabinet|\.part$/i.test(k)), []);
assert.deepEqual(r.messages, []);

// the CD's files (a folder, its names in another case): the game and the
// videos, no music
const cd = [mem('CDROM/data1.cab', disc.cab), mem('CDROM/setup.exe', new Uint8Array(2))];
for (const [d, fs] of Object.entries(disc.videos))
    for (const [n, data] of Object.entries(fs))
        cd.push(mem(`CDROM/Civ2/${d.toLowerCase()}/${n}`, data));
assert.deepEqual(findDisc(cd), {kind: 'cd', source: 'CDROM'});
r = run(cd, 'CDROM');
console.log(`CD folder: ${r.out.size} files in ${Math.round(r.ms)} ms`);
assert.equal(r.code, 0, r.messages.join('\n'));
for (const [k, v] of Object.entries(disc.expect)) {
    const got = r.out.get(k) || r.out.get(k.replace(/^(KINGS|VIDEO)/, (d) => d.toLowerCase()));
    assert.deepEqual(got, v, k);
}
assert.ok(![...r.out.keys()].some((k) => k.startsWith('Music/')));

// neither: said, nothing set up
r = run([mem('x/notes.txt', new Uint8Array(3))], 'x');
assert.notEqual(r.code, 0);
assert.ok(r.messages.some((m) => /neither an installed/.test(m)), r.messages.join('\n'));
assert.equal(findDisc([{name: 'a/DATA1.CAB'}]), null);         // (no CIV2 folder)
console.log('src/web set-up tests passed');
