// The files of a .zip archive (stored or deflated; no ZIP64, no encryption),
// read with the browser's own DecompressionStream.

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
