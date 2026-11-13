// The browser host's logic under node (no browser): keys, the wheel, the
// desktop's scale, pixels, the WASI file system, .zip reading.
//   tools/toolchain/node/bin/node src/web/test/unit.mjs
import assert from 'node:assert/strict';
import {vkOf, Wheel, desktopScale, argbToRgba} from '../host.js';
import {MemFS, makeWasi, normalize} from '../wasi.js';
import {unzip} from '../zip.js';
import {deflateRawSync} from 'node:zlib';

// keys
assert.equal(vkOf({code: 'KeyA', key: 'a'}), 0x41);
assert.equal(vkOf({code: 'KeyQ', key: 'a'}), 0x41);          // AZERTY: the letter typed
assert.equal(vkOf({code: 'Digit7', key: '&'}), 0x37);
assert.equal(vkOf({code: 'F12', key: 'F12'}), 0x7B);
assert.equal(vkOf({code: 'Numpad8', key: '8'}), 0x68);       // Num Lock on
assert.equal(vkOf({code: 'Numpad8', key: 'ArrowUp'}), 0x26); // off
assert.equal(vkOf({code: 'Numpad5', key: 'Clear'}), 0x0C);
assert.equal(vkOf({code: 'Minus', key: '_'}), 0xBD);
assert.equal(vkOf({code: 'ShiftLeft', key: 'Shift'}), 0xA0);
assert.equal(vkOf({code: 'IntlRo', key: 'ro'}), 0);

// the wheel: notches away from the user positive, fractions added up, reset on turning back
const w = new Wheel();
assert.equal(w.notches(-100, 0), 1);
assert.equal(w.notches(-40, 0), 0);
assert.equal(w.notches(-60, 0), 1);
assert.equal(w.notches(-50, 0), 0);
assert.equal(w.notches(30, 0), 0);          // turned back: the half notch forgotten
assert.equal(w.notches(70, 0), -1);
assert.equal(w.notches(3, 1), -1);          // lines: 3 a notch

// the desktop's scale (host.h's examples)
assert.equal(desktopScale(1920, 1080, 0), 1);
assert.equal(desktopScale(2560, 1440, 0), 2);
assert.equal(desktopScale(3840, 2160, 0), 3);
assert.equal(desktopScale(2940, 1782, 0), 3);
assert.equal(desktopScale(800, 600, 2), 1);

// pixels: ARGB words -> RGBA bytes, opaque
const dst = new Uint32Array(1);
argbToRgba(new Uint32Array([0x00112233]), dst, 1);
assert.deepEqual([...new Uint8Array(dst.buffer)], [0x11, 0x22, 0x33, 0xff]);

// WASI over the memory file system
assert.equal(normalize('/civ2/./a/../B.TXT'), '/civ2/B.TXT');
const fs = new MemFS(), changed = [];
fs.onchange = (p, removed) => changed.push(p + (removed ? ' -' : ''));
fs.put('/civ2/RULES.TXT', new TextEncoder().encode('hello'));
const mem = new WebAssembly.Memory({initial: 1});
const wasi = makeWasi(fs, ['civ2', '/civ2'], {CIV2_DATA: '/data'}, () => mem);
const v = new DataView(mem.buffer), u8 = new Uint8Array(mem.buffer);
const str = (at, s) => { const b = new TextEncoder().encode(s); u8.set(b, at); return b.length; };
// open, read
let n = str(1000, 'civ2/RULES.TXT');
assert.equal(wasi.path_open(3, 0, 1000, n, 0, 2n, 0n, 0, 2000), 0);
const fd = v.getUint32(2000, true);
v.setUint32(3000, 4000, true); v.setUint32(3004, 3, true);
assert.equal(wasi.fd_read(fd, 3000, 1, 3010), 0);
assert.equal(v.getUint32(3010, true), 3);
assert.equal(new TextDecoder().decode(u8.subarray(4000, 4003)), 'hel');
assert.equal(wasi.fd_seek(fd, 0n, 2, 3020), 0);
assert.equal(v.getBigUint64(3020, true), 5n);
assert.equal(wasi.fd_close(fd), 0);
// create, write, rename, unlink
n = str(1000, 'civ2/GAME.SAV');
assert.equal(wasi.path_open(3, 0, 1000, n, 1 | 8, 64n, 0n, 0, 2000), 0);
const out = v.getUint32(2000, true);
str(4000, 'saved!');
v.setUint32(3000, 4000, true); v.setUint32(3004, 6, true);
assert.equal(wasi.fd_write(out, 3000, 1, 3010), 0);
assert.equal(wasi.fd_close(out), 0);
assert.equal(new TextDecoder().decode(fs.get('/civ2/GAME.SAV')), 'saved!');
n = str(1000, 'civ2/GAME.SAV');
const m = str(1100, 'civ2/OTHER.SAV');
assert.equal(wasi.path_rename(3, 1000, n, 3, 1100, m), 0);
assert.equal(fs.get('/civ2/GAME.SAV'), null);
assert.equal(wasi.path_unlink_file(3, 1100, m), 0);
assert.deepEqual([...new Set(changed)], ['/civ2/GAME.SAV', '/civ2/GAME.SAV -', '/civ2/OTHER.SAV', '/civ2/OTHER.SAV -']);
// missing files, /dev/null
n = str(1000, 'civ2/NONE.TXT');
assert.equal(wasi.path_open(3, 0, 1000, n, 0, 2n, 0n, 0, 2000), 44);
n = str(1000, 'dev/null');
assert.equal(wasi.path_open(3, 0, 1000, n, 1, 64n, 0n, 0, 2000), 0);
assert.equal(wasi.fd_write(v.getUint32(2000, true), 3000, 1, 3010), 0);
assert.equal(v.getUint32(3010, true), 6);
// a directory's entries
n = str(1000, 'civ2');
assert.equal(wasi.path_open(3, 0, 1000, n, 2, 0n, 0n, 0, 2000), 0);
assert.equal(wasi.fd_readdir(v.getUint32(2000, true), 5000, 4096, 0n, 3030), 0);
const names = [];
for (let p = 5000, end = 5000 + v.getUint32(3030, true); p < end;) {
    const len = v.getUint32(p + 16, true);
    names.push(new TextDecoder().decode(u8.subarray(p + 24, p + 24 + len)));
    p += 24 + len;
}
assert.deepEqual(names, ['.', '..', 'RULES.TXT']);
// exit
assert.throws(() => wasi.proc_exit(3), (e) => e.code === 3);

// .zip: stored and deflated
function zip(files) {
    const parts = [], central = [];
    let at = 0;
    for (const [name, data, deflate] of files) {
        const body = deflate ? deflateRawSync(data) : data, fname = Buffer.from(name);
        const h = Buffer.alloc(30);
        h.writeUInt32LE(0x04034b50, 0); h.writeUInt16LE(deflate ? 8 : 0, 8); h.writeUInt32LE(body.length, 18);
        h.writeUInt32LE(data.length, 22); h.writeUInt16LE(fname.length, 26);
        const c = Buffer.alloc(46);
        c.writeUInt32LE(0x02014b50, 0); c.writeUInt16LE(deflate ? 8 : 0, 10); c.writeUInt32LE(body.length, 20);
        c.writeUInt32LE(data.length, 24); c.writeUInt16LE(fname.length, 28); c.writeUInt32LE(at, 42);
        parts.push(h, fname, body);
        central.push(c, fname);
        at += 30 + fname.length + body.length;
    }
    const cd = Buffer.concat(central), e = Buffer.alloc(22);
    e.writeUInt32LE(0x06054b50, 0); e.writeUInt16LE(files.length, 8); e.writeUInt16LE(files.length, 10);
    e.writeUInt32LE(cd.length, 12); e.writeUInt32LE(at, 16);
    const b = Buffer.concat([...parts, cd, e]);
    return b.buffer.slice(b.byteOffset, b.byteOffset + b.length);
}
const text = Buffer.from('RULES '.repeat(100));
const files = await unzip(zip([['Civ2/RULES.TXT', text, true], ['Civ2/Sound\\A.WAV', Buffer.from('wav'), false]]));
assert.deepEqual(files.map((f) => f.name), ['Civ2/RULES.TXT', 'Civ2/Sound/A.WAV']);
assert.equal(Buffer.from(files[0].data).toString(), text.toString());
console.log('src/web unit tests passed');
