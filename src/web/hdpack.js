// HD graphics in the browser (docs/HD-GRAPHICS.md, docs/BROWSER.md): the
// openciv2-assets art pack, from the site itself, where it was built with it
// (make web HD=1: hd/atlas-hd.pack, hd/pack.json saying so), checked against
// the release pinned in data/hd-pack.json (copied to the site as
// hd-pack.json: its pack_sha256).  The pack is kept in IndexedDB (store.js:
// the 'hd' store) in chunks of CHUNK bytes, its description under 'info'
// written last; the game gets, at start, a pack of the pictures it draws
// (prunePack), kept with it, in its files in memory where hd.c looks for it
// (/data/hd/atlas-hd.pack).

export const CHUNK = 4 << 20;
// The pictures src/platform/hd.c draws, by their keys' first part
// ("crop/file/SHEET/x,y,w,h", "crop/resource/MODULE/gifs/ID/x,y,w,h",
// "tint/crop/...", "dither/T/Q", "fog/Q", "file/SHEET"; the resource
// pictures it paints whole, hd.c's WHOLE; the throne room's, which
// src/game/hd_graphics.cpp composes: "palace/" but its "decor-", and the
// empty hall, "resource/pv.dll/gifs/100"): the rest of the pack (the pictures made for other
// renderers) is left out of the game's memory.
export const KEEP = ['crop/', 'tint/', 'dither/', 'fog/', 'file/', 'palace/', 'advance/', 'shield',
    'resource/pv.dll/gifs/100',
    'resource/mk.dll/gifs/20', 'resource/tiles.dll/gifs/90',
    'resource/intro.dll/', 'resource/cv.dll/gifs/340', 'resource/cv.dll/gifs/341',
    'resource/cv.dll/gifs/345', 'resource/cv.dll/gifs/346', 'resource/cv.dll/gifs/350',
    'resource/cv.dll/gifs/351',
    'resource/ss.dll/gifs/498', 'resource/ss.dll/gifs/499'];
// Of those, the throne room's decorations (not drawn), the crops hd.c does
// not draw: its NOT_CROPPED (the pack's
// leaders' portraits, not the tribes' leaders), and of the city view's
// houses (cv.dll 310, src/game/hd_graphics.cpp) all but the industrial
// era's rows (y 231 and 512).
const CV = 'crop/resource/cv.dll/gifs/310/';
export const DROP = ['palace/decor-', 'crop/resource/mk.dll/gifs/210/', 'crop/resource/mk.dll/gifs/211/',
    // its NOT_CROPPED_KEYS: tiles.dll 86's information seal, tutorial owl, podium
    'crop/resource/tiles.dll/gifs/86/398,134,', 'crop/resource/tiles.dll/gifs/86/134,398,',
    'crop/resource/tiles.dll/gifs/86/2,398,', 'tint/crop/resource/tiles.dll/gifs/86/2,398,',
    'crop/resource/mk.dll/gifs/22', 'crop/resource/mk.dll/gifs/23', 'crop/resource/mk.dll/gifs/24',
    'crop/resource/mk.dll/gifs/25', 'crop/resource/mk.dll/gifs/26',
    ...[1, 160, 319, 478].flatMap((x) => [`${CV}${x},1,`, `${CV}${x},116,`]),
    ...[1, 125, 249, 373].flatMap((x) => [`${CV}${x},346,`, `${CV}${x},429,`])];
// A key of a picture hd.c draws.
export const drawn = (key, keep = KEEP) => keep.some((p) => key.startsWith(p)) && !DROP.some((p) => key.startsWith(p));
const HEAD = 12 + 768, ENTRY = 180;

export async function sha256(bytes) {
    const h = new Uint8Array(await crypto.subtle.digest('SHA-256', bytes));
    return Array.from(h, (b) => b.toString(16).padStart(2, '0')).join('');
}

const stopped = (signal) => {
    if (signal && signal.aborted)
        throw new DOMException('stopped', 'AbortError');
};

// The pinned art downloaded from the site (a Blob: atlas-hd.pack), checked
// -> the pack's bytes.  progress(phase, done, total): phase 'check'.
export async function packFrom(blob, pin, {progress = () => {}, signal} = {}) {
    const pack = new Uint8Array(await blob.arrayBuffer());
    stopped(signal);
    if (new TextDecoder().decode(pack.subarray(0, 8)) !== 'C2ATLAS1')
        throw new Error('the site\'s hd/atlas-hd.pack is not an HD art pack');
    progress('check', 0, pack.length);
    if (pack.length !== pin.pack_size || await sha256(pack) !== pin.pack_sha256)
        throw new Error(`the site's hd/atlas-hd.pack is not the one of the art's release ${pin.tag}`);
    return pack;
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
    // (and the pictures the game draws, as a start reads them: loadPack)
    await keepPruned(store, info, await prunePack(async () => pack, pack.length));
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
    const kept = packIndex(head).filter((e) => drawn(e.key, keep));
    const index = HEAD + kept.length * ENTRY;
    // (the PNGs read in the stored pack's order: each chunk once; a PNG the
    // same as one already kept, as the city view's landscapes of one terrain,
    // kept once, its entries pointing at the same bytes)
    const order = kept.map((e, i) => [e, i]).sort((a, b) => a[0].off - b[0].off);
    const where = new Array(kept.length), pngs = [], seen = new Map();
    let at = index;
    for (const [e, i] of order) {
        const png = await bytes(e.off, e.len);
        let h = 2166136261;
        for (let k = 0; k < png.length; k += 61)
            h = Math.imul(h ^ png[k], 16777619);
        const id = png.length + ':' + (h >>> 0), same = (seen.get(id) || []).find(([p]) =>
            p.every((b, k) => b === png[k]));
        if (same)
            where[i] = same[1];
        else {
            seen.set(id, [...(seen.get(id) || []), [png, at]]);
            pngs.push(png);
            where[i] = at;
            at += e.len;
        }
        if (cache.size > 2)
            for (const k of [...cache.keys()])
                if (k < Math.floor(e.off / chunk))
                    cache.delete(k);
    }
    const out = new Uint8Array(at), v = new DataView(out.buffer);
    out.set(head.subarray(0, HEAD));
    v.setUint32(8, kept.length, true);
    at = index;
    for (const png of pngs) {
        out.set(png, at);
        at += png.length;
    }
    kept.forEach((e, i) => {
        const p = HEAD + i * ENTRY;
        out.set(head.subarray(e.at, e.at + ENTRY), p);
        v.setUint32(p + 168, where[i], true);
        v.setUint32(p + 176, 0, true);
    });
    return out;
}

// The stored art for the game, or null (none, or not the pinned release's).
// The pruned pack is kept too (keys 'p0', 'p1' ... and 'pruned', its
// description, written last), the first start after the art is kept making
// it: later starts read it alone (some 110 MB rather than the 250 MB pack), its
// chunks at once.  Each step's time in timing (ms) when given.
export async function loadPack(store, pin, timing = {}) {
    const t0 = performance.now();
    const [info, pruned] = await Promise.all([store.hdGet('info'), store.hdGet('pruned')]);
    if (!info || (pin && info.sha256 !== pin.pack_sha256))
        return null;
    const want = prunedOf(info);
    if (pruned && pruned.sha256 === want.sha256 && pruned.at === want.at && pruned.keep === want.keep) {
        const parts = await Promise.all(Array.from({length: pruned.chunks}, (_, k) => store.hdGet('p' + k)));
        if (parts.every((c) => c)) {
            const out = new Uint8Array(pruned.size);
            let at = 0;
            for (const c of parts) {
                out.set(c.subarray(0, Math.min(c.length, out.length - at)), at);
                at += c.length;
            }
            if (at === pruned.size) {
                timing.packMs = Math.round(performance.now() - t0);
                timing.packChunks = parts.length;
                return out;
            }
        }
    }
    let read = 0;
    const pack = await prunePack((k) => (read++, store.hdGet(k)), info.chunk);
    timing.packMs = Math.round(performance.now() - t0);
    timing.packChunks = read;
    // (kept for the next start, while this one goes on)
    timing.kept = keepPruned(store, info, pack).catch((e) => console.warn('HD graphics: the pruned art not kept:', e));
    return pack;
}

const prunedOf = (info) => ({sha256: info.sha256, at: info.at, keep: KEEP.join(' ')});

// The pruned pack kept (keepPack's info, the pruned pack's bytes)
export async function keepPruned(store, info, pruned) {
    const n = Math.ceil(pruned.length / CHUNK), puts = [];
    for (let k = 0; k < n; k++)
        puts.push(['p' + k, pruned.slice(k * CHUNK, Math.min(pruned.length, (k + 1) * CHUNK))]);
    await store.hdPutMany(puts);
    await store.hdPutMany([['pruned', {...prunedOf(info), size: pruned.length, chunks: n}]]);
}

// Whether the site has the art (make web HD=1): {size} or null
export async function siteArt() {
    try {
        const r = await fetch('hd/pack.json', {cache: 'no-cache'});
        return r.ok ? await r.json() : null;
    } catch (e) {
        return null;
    }
}

// The art downloaded from the site (hd/atlas-hd.pack) -> a Blob;
// progress(done, total)
export async function download(size, {progress = () => {}, signal} = {}) {
    const r = await fetch('hd/atlas-hd.pack', {signal});
    if (!r.ok)
        throw new Error('the download failed (' + r.status + ')');
    const total = +r.headers.get('Content-Length') || size, parts = [], reader = r.body.getReader();
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
