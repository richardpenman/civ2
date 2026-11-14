// .zip archives (stored or deflated; no ZIP64, no encryption): read with
// the browser's own DecompressionStream, written with its CompressionStream.

async function inflateRaw(bytes) {
    const s = new Blob([bytes]).stream().pipeThrough(new DecompressionStream('deflate-raw'));
    return new Uint8Array(await new Response(s).arrayBuffer());
}

// buf: ArrayBuffer -> [{name, data: Uint8Array}] (files only); onfile(name) for progress
export async function unzip(buf, onfile = () => {}) {
    const v = new DataView(buf), u8 = new Uint8Array(buf);
    let end = -1;
    for (let i = buf.byteLength - 22; i >= Math.max(0, buf.byteLength - 22 - 65535); i--)
        if (v.getUint32(i, true) === 0x06054b50) {
            end = i;
            break;
        }
    if (end < 0)
        throw new Error('not a zip archive');
    const count = v.getUint16(end + 10, true);
    let p = v.getUint32(end + 16, true);
    const dec = new TextDecoder(), out = [];
    for (let k = 0; k < count; k++) {
        if (v.getUint32(p, true) !== 0x02014b50)
            throw new Error('damaged zip archive');
        const method = v.getUint16(p + 10, true), csize = v.getUint32(p + 20, true);
        const nlen = v.getUint16(p + 28, true), xlen = v.getUint16(p + 30, true), clen = v.getUint16(p + 32, true);
        const local = v.getUint32(p + 42, true);
        const name = dec.decode(u8.subarray(p + 46, p + 46 + nlen)).replace(/\\/g, '/');
        p += 46 + nlen + xlen + clen;
        if (name.endsWith('/'))
            continue;
        const start = local + 30 + v.getUint16(local + 26, true) + v.getUint16(local + 28, true);
        const raw = u8.subarray(start, start + csize);
        onfile(name);
        if (method === 0)
            out.push({name, data: raw.slice()});
        else if (method === 8)
            out.push({name, data: await inflateRaw(raw)});
        else
            throw new Error(name + ': compressed in a way this page cannot read (method ' + method + ')');
    }
    return out;
}

// ---- writing ----

const CRC = new Int32Array(256).map((_, n) => {
    let c = n;
    for (let k = 0; k < 8; k++)
        c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    return c;
});

export function crc32(b) {
    let c = -1;
    for (let i = 0; i < b.length; i++)
        c = CRC[(c ^ b[i]) & 0xff] ^ (c >>> 8);
    return (c ^ -1) >>> 0;
}

async function deflateRaw(bytes) {
    const s = new Blob([bytes]).stream().pipeThrough(new CompressionStream('deflate-raw'));
    return new Uint8Array(await new Response(s).arrayBuffer());
}

// a time (ms) as MS-DOS's date and time (local, 2 s steps, 1980 on)
function dosTime(ms) {
    const d = new Date(ms);
    if (!(d.getFullYear() >= 1980))
        return [0, 0x21];
    return [d.getHours() << 11 | d.getMinutes() << 5 | d.getSeconds() >> 1,
            (d.getFullYear() - 1980) << 9 | (d.getMonth() + 1) << 5 | d.getDate()];
}

// files: [{name, data: Uint8Array, mtime (ms, optional)}] -> the archive's
// bytes; each file deflated where that makes it smaller (deflate: false:
// all stored)
export async function zip(files, {deflate = true} = {}) {
    const enc = new TextEncoder(), parts = [], central = [];
    let at = 0;
    for (const f of files) {
        const name = enc.encode(f.name), crc = crc32(f.data);
        let body = f.data, method = 0;
        if (deflate && f.data.length > 64) {
            const d = await deflateRaw(f.data);
            if (d.length < f.data.length) {
                body = d;
                method = 8;
            }
        }
        const [time, date] = dosTime(f.mtime ?? Date.now());
        const utf8 = /[^\x20-\x7e]/.test(f.name) ? 0x800 : 0;
        const h = new DataView(new ArrayBuffer(30));
        h.setUint32(0, 0x04034b50, true);
        h.setUint16(4, 20, true);
        h.setUint16(6, utf8, true);
        h.setUint16(8, method, true);
        h.setUint16(10, time, true);
        h.setUint16(12, date, true);
        h.setUint32(14, crc, true);
        h.setUint32(18, body.length, true);
        h.setUint32(22, f.data.length, true);
        h.setUint16(26, name.length, true);
        const c = new DataView(new ArrayBuffer(46));
        c.setUint32(0, 0x02014b50, true);
        c.setUint16(4, 20, true);
        c.setUint16(6, 20, true);
        c.setUint16(8, utf8, true);
        c.setUint16(10, method, true);
        c.setUint16(12, time, true);
        c.setUint16(14, date, true);
        c.setUint32(16, crc, true);
        c.setUint32(20, body.length, true);
        c.setUint32(24, f.data.length, true);
        c.setUint16(28, name.length, true);
        c.setUint32(42, at, true);
        parts.push(new Uint8Array(h.buffer), name, body);
        central.push(new Uint8Array(c.buffer), name);
        at += 30 + name.length + body.length;
    }
    const size = central.reduce((n, p) => n + p.length, 0);
    const e = new DataView(new ArrayBuffer(22));
    e.setUint32(0, 0x06054b50, true);
    e.setUint16(8, files.length, true);
    e.setUint16(10, files.length, true);
    e.setUint32(12, size, true);
    e.setUint32(16, at, true);
    const out = new Uint8Array(at + size + 22);
    let p = 0;
    for (const part of [...parts, ...central, new Uint8Array(e.buffer)]) {
        out.set(part, p);
        p += part.length;
    }
    return out;
}
