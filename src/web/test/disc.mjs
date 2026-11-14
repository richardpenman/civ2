// A made-up copy of the game's CD for the set-up's tests (there is no real
// one here): an InstallShield 5 cabinet (DATA1.CAB, its header inside, as
// libunshield reads it: file groups, a directory table, files stored or
// zlib-compressed in chunks), an ISO 9660 file system with it and a CIV2
// folder of videos, and a disc image of that: a .bin of raw 2352-byte
// sectors (the data track MODE1/2352, then audio tracks, 16-bit stereo) with
// its .cue sheet.  As tests/cue_gaps_test.c makes its image, plus the data
// track's contents.

import {deflateRawSync} from 'node:zlib';

const enc = new TextEncoder();

function concat(parts) {
    const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
    let at = 0;
    for (const p of parts) {
        out.set(p, at);
        at += p.length;
    }
    return out;
}

// ---- the cabinet ----

// groups: [{name, files: [{dir, name, data, compress}]}] -> DATA1.CAB
// (version 5, one volume, the header in it)
export function makeCabinet(groups) {
    const files = groups.flatMap((g) => g.files);
    const dirs = [...new Set(files.map((f) => f.dir))];
    // the files' data, after the common and the volume header
    let at = 20 + 40;
    const bodies = files.map((f) => {
        let body;
        if (f.compress) {
            const chunks = [];
            for (let i = 0; i < f.data.length || i === 0; i += 0x8000) {
                const z = deflateRawSync(f.data.subarray(i, i + 0x8000));
                const n = new Uint8Array(2);
                new DataView(n.buffer).setUint16(0, z.length, true);
                chunks.push(n, z);
            }
            body = concat(chunks);
        } else {
            body = f.data;
        }
        const r = {body, offset: at};
        at += body.length;
        return r;
    });
    const D = at;               // the cabinet descriptor
    const desc = new Uint8Array(0x10000);
    const dv = new DataView(desc.buffer);
    let end = 0x276;
    const put = (bytes) => {
        const o = end;
        desc.set(bytes, o);
        end += bytes.length;
        end = (end + 3) & ~3;
        return o;
    };
    const str = (s) => put(enc.encode(s + '\0'));
    // the file table (offsets in it relative to its start T): the
    // directories' names, then the files' descriptors (0x3a bytes)
    const T = end;
    const n = dirs.length + files.length;
    end += 4 * n;
    const table = [];
    for (const d of dirs)
        table.push(str(d) - T);
    files.forEach((f, i) => {
        const name = str(f.name) - T;
        const fd = new Uint8Array(0x3a), v = new DataView(fd.buffer);
        v.setUint32(0, name, true);
        v.setUint32(4, dirs.indexOf(f.dir), true);
        v.setUint16(8, f.compress ? 4 : 0, true);
        v.setUint32(10, f.data.length, true);
        v.setUint32(14, bodies[i].body.length, true);
        v.setUint32(0x26, bodies[i].offset, true);
        table.push(put(fd) - T);
    });
    table.forEach((o, i) => dv.setUint32(T + 4 * i, o, true));
    const tableSize = end - T;
    // the file groups: a list of {name, descriptor, next}, each descriptor
    // {name, 0x48 bytes, first file, last file}
    let first = 0, prev = 0;
    groups.forEach((g, k) => {
        const gd = new Uint8Array(4 + 0x48 + 8), gv = new DataView(gd.buffer);
        gv.setUint32(0, str(g.name), true);
        gv.setUint32(4 + 0x48, first, true);
        gv.setUint32(8 + 0x48, first + g.files.length - 1, true);
        first += g.files.length;
        const desc0 = put(gd);
        const node = new Uint8Array(12), nv = new DataView(node.buffer);
        nv.setUint32(0, desc0 - 0, true);       // (the list's name: unused)
        nv.setUint32(4, desc0, true);
        const o = put(node);
        if (k === 0)
            dv.setUint32(0x3e, o, true);
        else
            dv.setUint32(prev + 8, o, true);
        prev = o;
    });
    dv.setUint32(0x0c, T, true);
    dv.setUint32(0x14, tableSize, true);
    dv.setUint32(0x18, tableSize, true);
    dv.setUint32(0x1c, dirs.length, true);
    dv.setUint32(0x28, files.length, true);
    const descriptor = desc.subarray(0, end);
    const head = new Uint8Array(60), hv = new DataView(head.buffer);
    hv.setUint32(0, 0x28635349, true);          // ISc(
    hv.setUint32(4, 0x01005000, true);          // version 5
    hv.setUint32(12, D, true);
    hv.setUint32(16, descriptor.length, true);
    hv.setUint32(20, 60, true);                 // (volume header: data offset)
    hv.setUint32(28, 0, true);                  // first file
    hv.setUint32(32, files.length - 1, true);   // last file
    return concat([head, ...bodies.map((b) => b.body), descriptor]);
}

// ---- ISO 9660 ----

const SECTOR = 2048;

function bothEndian32(v, o, x) {
    v.setUint32(o, x, true);
    v.setUint32(o + 4, x, false);
}

function record(name, extent, size, isDir) {
    const nb = typeof name === 'string' ? enc.encode(name) : name;
    const len = 33 + nb.length + ((33 + nb.length) & 1);
    const r = new Uint8Array(len), v = new DataView(r.buffer);
    r[0] = len;
    bothEndian32(v, 2, extent);
    bothEndian32(v, 10, size);
    r.set([126, 1, 1, 0, 0, 0, 0], 18);         // 2026-01-01
    r[25] = isDir ? 2 : 0;
    v.setUint16(28, 1, true);
    v.setUint16(30, 1, false);
    r[32] = nb.length;
    r.set(nb, 33);
    return r;
}

// tree: {NAME: Uint8Array | {...}} -> the file system's sectors
export function makeIso(tree) {
    // sectors: 0-15 empty, 16 the volume descriptor, 17 its terminator, then
    // the directories (one sector each), then the files
    const dirs = [], files = [];
    const walk = (t, parent) => {
        const d = {t, parent, sector: 0, entries: []};
        dirs.push(d);
        for (const name of Object.keys(t).sort()) {
            const x = t[name];
            if (x instanceof Uint8Array) {
                const f = {data: x, sector: 0};
                files.push(f);
                d.entries.push({name: name + ';1', f});
            } else {
                d.entries.push({name, d: walk(x, d)});
            }
        }
        return d;
    };
    walk(tree, null);
    let next = 18;
    for (const d of dirs)
        d.sector = next++;
    for (const f of files) {
        f.sector = next;
        next += Math.max(1, Math.ceil(f.data.length / SECTOR));
    }
    const img = new Uint8Array(next * SECTOR);
    for (const d of dirs) {
        const parts = [record(new Uint8Array([0]), d.sector, SECTOR, true),
                       record(new Uint8Array([1]), (d.parent || d).sector, SECTOR, true)];
        for (const e of d.entries)
            parts.push(e.f ? record(e.name, e.f.sector, e.f.data.length, false) : record(e.name, e.d.sector, SECTOR, true));
        const sec = concat(parts);
        if (sec.length > SECTOR)
            throw new Error('a directory too big for this test');
        img.set(sec, d.sector * SECTOR);
    }
    for (const f of files)
        img.set(f.data, f.sector * SECTOR);
    const pvd = img.subarray(16 * SECTOR, 17 * SECTOR), pv = new DataView(pvd.buffer, pvd.byteOffset);
    pvd[0] = 1;
    pvd.set(enc.encode('CD001'), 1);
    pvd[6] = 1;
    pvd.set(enc.encode('CIV2MGE'.padEnd(32)), 40);
    bothEndian32(pv, 80, next);
    pvd.set(record(new Uint8Array([0]), dirs[0].sector, SECTOR, true), 156);
    const term = img.subarray(17 * SECTOR);
    term[0] = 255;
    term.set(enc.encode('CD001'), 1);
    term[6] = 1;
    return img;
}

// ---- the disc image ----

export const RAW = 2352;

// iso: the data track's sectors; tracks: [Int16Array of interleaved stereo
// samples] (each made whole sectors); gap: sectors of silence before each
// (the cue sheet's INDEX 00 to 01) -> {cue: text, bin: Uint8Array,
// starts: [sector of each audio track's INDEX 01]}
export function makeImage(iso, tracks, binName = 'CIV2.BIN', gap = 150) {
    const dataSectors = iso.length / SECTOR;
    const audioSectors = tracks.map((t) => gap + Math.ceil(t.length * 2 / RAW));
    const total = dataSectors + audioSectors.reduce((a, b) => a + b, 0);
    const bin = new Uint8Array(total * RAW);
    for (let s = 0; s < dataSectors; s++) {
        const o = s * RAW;
        bin.set([0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0], o);
        bin[o + 15] = 1;                        // (mode 1; the address and ECC left out)
        bin.set(iso.subarray(s * SECTOR, (s + 1) * SECTOR), o + 16);
    }
    const msf = (s) => [Math.floor(s / 4500), Math.floor(s / 75) % 60, s % 75].map((x) => String(x).padStart(2, '0')).join(':');
    let cue = `FILE "${binName}" BINARY\n  TRACK 01 MODE1/2352\n    INDEX 01 00:00:00\n`;
    let at = dataSectors;
    const starts = [];
    tracks.forEach((t, i) => {
        const start = at + gap;
        bin.set(new Uint8Array(t.buffer, t.byteOffset, t.length * 2), start * RAW);
        cue += `  TRACK ${String(i + 2).padStart(2, '0')} AUDIO\n    INDEX 00 ${msf(at)}\n    INDEX 01 ${msf(start)}\n`;
        starts.push(start);
        at += audioSectors[i];
    });
    return {cue, bin, starts};
}

// seconds of a tone (Hz) as 44.1 kHz 16-bit stereo
export function tone(seconds, hz, amp = 8000) {
    const n = Math.round(seconds * 44100), out = new Int16Array(2 * n);
    for (let i = 0; i < n; i++)
        out[2 * i] = out[2 * i + 1] = Math.round(amp * Math.sin(2 * Math.PI * hz * i / 44100));
    return out;
}

// The made-up game: its cabinet's groups as the real one has them (the
// program's files, "Sound Files" unpacked into Sound), its CD's videos.
export function sampleDisc() {
    const bytes = (s, n = 1) => enc.encode(s.repeat(n));
    const noise = new Uint8Array(70000).map((_, i) => (i * 2654435761) >>> 24);
    const game = [
        {dir: '', name: 'Civ2Art.dll', data: noise, compress: true},
        {dir: '', name: 'RULES.TXT', data: bytes('@COSMIC\r\n', 5000), compress: true},
        {dir: '', name: 'README.TXT', data: bytes('stored, not compressed'), compress: false},
        {dir: 'SCENARIO', name: 'TEST.SCN', data: bytes('scenario', 10), compress: true},
    ];
    const sound = [{dir: '', name: 'DIVINE.WAV', data: bytes('RIFF wave', 300), compress: true}];
    const cab = makeCabinet([{name: 'Program Executable Files', files: game}, {name: 'Sound Files', files: sound}]);
    const videos = {KINGS: {'KING1.AVI': bytes('king video', 3000)}, VIDEO: {'INTRO.AVI': bytes('intro video', 200)}};
    return {cab, videos, iso: makeIso({'DATA1.CAB': cab, CIV2: videos, 'SETUP.EXE': bytes('MZ')}),
            expect: {'Civ2Art.dll': noise, 'RULES.TXT': game[1].data, 'README.TXT': game[2].data,
                     'SCENARIO/TEST.SCN': game[3].data, 'Sound/DIVINE.WAV': sound[0].data,
                     'KINGS/KING1.AVI': videos.KINGS['KING1.AVI'], 'VIDEO/INTRO.AVI': videos.VIDEO['INTRO.AVI']}};
}
