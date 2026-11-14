// HD graphics in the browser (docs/HD-GRAPHICS.md, docs/BROWSER.md): the
// openciv2-assets art pack, as the desktop version's opening window gets it.
// The release pinned in data/hd-pack.json (copied to the site as
// hd-pack.json) is checked as tools/fetch_hd_pack.py checks it: the archive's
// SHA-256, then data/atlas-hd.pack's against the archive's package.json (or,
// given alone, against the pin's pack_sha256).  The pack is kept in
// IndexedDB (store.js: the 'hd' store) in chunks of CHUNK bytes, its
// description under 'info' written last; the game gets, at start, a pack of
// the pictures it draws (prunePack), in its files in memory where hd.c looks
// for it (/data/hd/atlas-hd.pack).

export const CHUNK = 4 << 20;
// The pictures src/platform/hd.c draws, by their keys' first part
// ("crop/file/SHEET/x,y,w,h", "tint/crop/...", "dither/T/Q", "fog/Q",
// "file/SHEET"): the rest of the pack (the other windows' pictures, which it
// does not draw yet) is left out of the game's memory.
export const KEEP = ['crop/', 'tint/', 'dither/', 'fog/', 'file/'];
const HEAD = 12 + 768, ENTRY = 180;

export async function sha256(bytes) {
    const h = new Uint8Array(await crypto.subtle.digest('SHA-256', bytes));
    return Array.from(h, (b) => b.toString(16).padStart(2, '0')).join('');
}

const stopped = (signal) => {
    if (signal && signal.aborted)
        throw new DOMException('stopped', 'AbortError');
};

async function bytesOf(blob, from, to) {
    return new Uint8Array(await blob.slice(from, to).arrayBuffer());
}

// A .zip's members (a Blob, read in parts: the archive is never in memory
// for this): name -> {method, csize, size, local}
export async function zipDir(blob) {
    const tailFrom = Math.max(0, blob.size - 22 - 65535);
    const tail = await bytesOf(blob, tailFrom, blob.size), v = new DataView(tail.buffer);
    let end = -1;
    for (let i = tail.length - 22; i >= 0; i--)
        if (v.getUint32(i, true) === 0x06054b50) {
            end = i;
            break;
        }
    if (end < 0)
        throw new Error('not a zip archive');
    const count = v.getUint16(end + 10, true), cdSize = v.getUint32(end + 12, true), cdAt = v.getUint32(end + 16, true);
    const cd = await bytesOf(blob, cdAt, cdAt + cdSize), c = new DataView(cd.buffer), dec = new TextDecoder();
    const out = new Map();
    for (let k = 0, p = 0; k < count; k++) {
        if (c.getUint32(p, true) !== 0x02014b50)
            throw new Error('damaged zip archive');
        const nlen = c.getUint16(p + 28, true);
        out.set(dec.decode(cd.subarray(p + 46, p + 46 + nlen)), {
            method: c.getUint16(p + 10, true), csize: c.getUint32(p + 20, true), size: c.getUint32(p + 24, true),
            local: c.getUint32(p + 42, true)});
        p += 46 + nlen + c.getUint16(p + 30, true) + c.getUint16(p + 32, true);
    }
    return out;
}

// A member's bytes, inflated as they come into one array of its size;
// progress(done, total)
export async function zipMember(blob, m, {progress = () => {}, signal} = {}) {
    const h = await bytesOf(blob, m.local, m.local + 30), v = new DataView(h.buffer);
    if (v.getUint32(0, true) !== 0x04034b50)
        throw new Error('damaged zip archive');
    const start = m.local + 30 + v.getUint16(26, true) + v.getUint16(28, true);
    const raw = blob.slice(start, start + m.csize);
    if (m.method === 0)
        return new Uint8Array(await raw.arrayBuffer());
    if (m.method !== 8)
        throw new Error('compressed in a way this page cannot read (method ' + m.method + ')');
    const out = new Uint8Array(m.size), reader = raw.stream().pipeThrough(new DecompressionStream('deflate-raw')).getReader();
    let at = 0;
    for (;;) {
        stopped(signal);
        const {done, value} = await reader.read();
        if (done)
            break;
        if (at + value.length > out.length)
            throw new Error('damaged zip archive');
        out.set(value, at);
        at += value.length;
        progress(at, out.length);
    }
    if (at !== out.length)
        throw new Error('damaged zip archive');
    return out;
}

// The pinned art from what the player chose or the page downloaded (a Blob:
// the release's .zip, or atlas-hd.pack alone), checked -> the pack's bytes.
// progress(phase, done, total): phase 'check' or 'unpack'.
export async function packFrom(blob, pin, {progress = () => {}, signal} = {}) {
    const magic = new TextDecoder().decode(await bytesOf(blob, 0, 8));
    if (magic.startsWith('PK\x03\x04')) {
        progress('check', 0, blob.size);
        if (blob.size !== pin.size || await sha256(await blob.arrayBuffer()) !== pin.sha256)
            throw new Error(`this archive is not the art's release ${pin.tag} (its SHA-256 differs)`);
        stopped(signal);
        const dir = await zipDir(blob), want = dir.get('package.json'), member = dir.get('data/atlas-hd.pack');
        const manifest = want && JSON.parse(new TextDecoder().decode(await zipMember(blob, want)));
        if (!manifest || manifest.kind !== 'openciv2-assets' || manifest.schema !== 1 || !member ||
            !manifest.files || !manifest.files['data/atlas-hd.pack'])
            throw new Error('not an openciv2-assets runtime pack (schema 1)');
        const listed = manifest.files['data/atlas-hd.pack'];
        const pack = await zipMember(blob, member, {progress: (d, t) => progress('unpack', d, t), signal});
        progress('check', pack.length, pack.length);
        if (pack.length !== listed.size || await sha256(pack) !== listed.sha256)
            throw new Error('data/atlas-hd.pack does not match its package.json');
        return pack;
    }
    if (magic === 'C2ATLAS1') {
        progress('check', 0, blob.size);
        const pack = new Uint8Array(await blob.arrayBuffer());
        if (pack.length !== pin.pack_size || await sha256(pack) !== pin.pack_sha256)
            throw new Error(`this atlas-hd.pack is not the one of the art's release ${pin.tag}`);
        return pack;
    }
    throw new Error('neither the art\'s .zip nor atlas-hd.pack');
}

// The pack kept in the store (store.hdClear, hdPutMany([[key, value]])):
// its chunks, then its description.  progress(done, total)
export async function keepPack(store, pack, pin, {progress = () => {}, signal} = {}) {
    await store.hdClear();
    const n = Math.ceil(pack.length / CHUNK);
    for (let i = 0; i < n; i += 4) {
        stopped(signal);
        const batch = [];
        for (let k = i; k < Math.min(n, i + 4); k++)
            batch.push([k, pack.slice(k * CHUNK, Math.min(pack.length, (k + 1) * CHUNK))]);
        await store.hdPutMany(batch);
        progress(Math.min(pack.length, (i + 4) * CHUNK), pack.length);
    }
    const info = {tag: pin.tag, size: pack.length, chunk: CHUNK, chunks: n, sha256: pin.pack_sha256, at: Date.now()};
    await store.hdPutMany([['info', info]]);
    return info;
}

// The entries of a pack's index (head: its first bytes, the index whole)
export function packIndex(head) {
    const v = new DataView(head.buffer, head.byteOffset, head.byteLength), dec = new TextDecoder();
    if (dec.decode(head.subarray(0, 8)) !== 'C2ATLAS1')
        throw new Error('not an HD art pack');
    const n = v.getUint32(8, true), out = [];
    if (head.length < HEAD + n * ENTRY)
        throw new Error('the pack\'s index is cut short');
    for (let i = 0; i < n; i++) {
        const at = HEAD + i * ENTRY, key = head.subarray(at, at + 160);
        const z = key.indexOf(0);
        out.push({key: dec.decode(key.subarray(0, z < 0 ? 160 : z)), at,
                  off: v.getUint32(at + 168, true), len: v.getUint32(at + 172, true)});
    }
    return out;
}

// The pictures of KEEP as a pack of their own (the same header, palette and
// entries in their order, each with its PNG; an entry's indexed copy, which
// hd.c does not read, left out).  read(chunk) -> Promise<Uint8Array>: the
// stored pack's chunk of CHUNK bytes.
export async function prunePack(read, chunk = CHUNK, keep = KEEP) {
    const cache = new Map();
    const get = async (k) => {
        if (!cache.has(k))
            cache.set(k, await read(k));
        return cache.get(k);
    };
    const bytes = async (off, len) => {
        const out = new Uint8Array(len);
        for (let at = 0; at < len;) {
            const k = Math.floor((off + at) / chunk), c = await get(k), from = off + at - k * chunk;
            const part = c.subarray(from, Math.min(c.length, from + len - at));
            if (!part.length)
                throw new Error('the stored pack is cut short');
            out.set(part, at);
            at += part.length;
        }
        return out;
    };
    const first = await bytes(0, 12);
    const count = new DataView(first.buffer).getUint32(8, true);
    const head = await bytes(0, HEAD + count * ENTRY);
    const kept = packIndex(head).filter((e) => keep.some((p) => e.key.startsWith(p)));
    const index = HEAD + kept.length * ENTRY;
    const out = new Uint8Array(index + kept.reduce((n, e) => n + e.len, 0)), v = new DataView(out.buffer);
    out.set(head.subarray(0, HEAD));
    v.setUint32(8, kept.length, true);
    let at = index;
    // (the PNGs read in the stored pack's order: each chunk once)
    const order = kept.map((e, i) => [e, i]).sort((a, b) => a[0].off - b[0].off);
    const where = new Array(kept.length);
    for (const [e, i] of order) {
        out.set(await bytes(e.off, e.len), at);
        where[i] = at;
        at += e.len;
        if (cache.size > 2)
            for (const k of [...cache.keys()])
                if (k < Math.floor(e.off / chunk))
                    cache.delete(k);
    }
    kept.forEach((e, i) => {
        const p = HEAD + i * ENTRY;
        out.set(head.subarray(e.at, e.at + ENTRY), p);
        v.setUint32(p + 168, where[i], true);
        v.setUint32(p + 176, 0, true);
    });
    return out;
}

// The stored art for the game, or null (none, or not the pinned release's)
export async function loadPack(store, pin) {
    const info = await store.hdGet('info');
    if (!info || (pin && info.sha256 !== pin.pack_sha256))
        return null;
    return prunePack((k) => store.hdGet(k), info.chunk);
}

// The art downloaded: from the site itself where it has it (make web HD=1:
// hd/atlas-hd.pack), else the release on GitHub (which browsers may refuse
// to a page of another site: then the player downloads it and chooses it).
// -> a Blob; progress(done, total)
export async function download(pin, {progress = () => {}, signal} = {}) {
    let r = null;
    try {
        r = await fetch('hd/atlas-hd.pack', {signal});
        if (!r.ok)
            r = null;
    } catch (e) {
        if (e.name === 'AbortError')
            throw e;
    }
    if (!r) {
        try {
            r = await fetch(pin.url, {signal});
        } catch (e) {
            if (e.name === 'AbortError')
                throw e;
            const err = new Error('the browser does not let this page download the art from GitHub');
            err.cors = true;
            throw err;
        }
        if (!r.ok)
            throw new Error('the download failed (' + r.status + ')');
    }
    const total = +r.headers.get('Content-Length') || pin.size, parts = [], reader = r.body.getReader();
    let done = 0;
    for (;;) {
        const {done: end, value} = await reader.read();
        if (end)
            break;
        parts.push(value);
        done += value.length;
        progress(done, total);
    }
    return new Blob(parts);
}
