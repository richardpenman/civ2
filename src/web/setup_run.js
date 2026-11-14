// The set-up from the game's CD or a disc image of it (src/web/setup/
// setup_web.c: src/host/setup.c and setup_disc.c as their own module,
// setup.wasm), run synchronously over WASI: the player's files under /src,
// read where they are as the set-up asks for them, the game set up into
// /game, each of its files handed over (onFile) as soon as it is written,
// and then dropped here (the game, its videos and its music do not stay in
// memory twice).  Runs in a Web Worker (setup_worker.js) or under node (the
// tests).

import {MemFS, makeWasi, ExitStatus} from './wasi.js';

export const SRC = '/src', GAME = '/game';

// A source file's reader with a cache of BLOCK bytes, so that small reads
// (stdio's) do not each reach the file: read(pos, n) from readAt(pos, n).
const BLOCK = 1 << 20;
export function cachedReader(size, readAt) {
    let at = -1, block = null;
    return (pos, n) => {
        if (n > BLOCK / 2)
            return readAt(pos, n);
        const out = new Uint8Array(n);
        for (let done = 0; done < n;) {
            const p = pos + done;
            if (!block || p < at || p >= at + block.length) {
                at = p - (p % BLOCK);
                block = readAt(at, Math.min(BLOCK, size - at));
                if (!block.length)
                    throw new Error('read past the end');
            }
            const k = Math.min(n - done, at + block.length - p);
            out.set(block.subarray(p - at, p - at + k), done);
            done += k;
        }
        return out;
    };
}

// opts: {module: WebAssembly.Module, files: [{name ("a/b.cue": under /src),
// size, readAt(pos, n) -> Uint8Array}], source (a .cue's or the CD's folder's
// name among them), onFile(path, data), onProgress(what, detail) -> true to
// stop, onMessage(text, kind: 0 info, 1 warning, 2 error), log(fd, line)}
// -> 0: set up; else not (said through onMessage)
export function runSetup(opts) {
    const fs = new MemFS();
    for (const f of opts.files)
        fs.putSource(SRC + '/' + f.name, f.size, cachedReader(f.size, f.readAt), f.mtime || Date.now());
    fs.mkdirs(GAME);
    const handed = (path) => path.startsWith(GAME + '/') && !/\.part$/i.test(path) &&
        !path.slice(GAME.length + 1).startsWith('OpenCiv2-cabinet/');
    fs.onchange = (path, removed, partial) => {
        if (partial || removed || !handed(path))
            return;
        const f = fs.files.get(path);
        if (!f || f.read)
            return;
        opts.onFile(path.slice(GAME.length + 1), f.data.slice(0, f.size));
        if (f.size)
            f.read = () => {
                throw new Error('read after it was handed over');
            };
    };
    let memory = null;
    const dec = new TextDecoder('windows-1252');
    const cstr = (p) => {
        const m = new Uint8Array(memory.buffer);
        let e = p;
        while (m[e])
            e++;
        return dec.decode(m.subarray(p, e));
    };
    const source = SRC + '/' + opts.source.replace(/\/$/, '');
    const wasi = makeWasi(fs, ['setup', source, GAME], {}, () => memory, opts.log);
    const setup = {
        progress: (what, detail) => (opts.onProgress(cstr(what), cstr(detail)) ? 1 : 0),
        message: (text, kind) => opts.onMessage(cstr(text), kind),
    };
    const instance = new WebAssembly.Instance(opts.module, {wasi_snapshot_preview1: wasi, setup});
    memory = instance.exports.memory;
    try {
        instance.exports._start();
    } catch (e) {
        if (e instanceof ExitStatus)
            return e.code;
        throw e;
    }
    return 0;
}

// Among the player's chosen files ([{name}]), the game's CD or a disc image
// of it: the .cue's name, or the folder with DATA1.CAB and CIV2 in it ('' at
// the top); null: neither.
export function findDisc(files) {
    const names = files.map((f) => f.name.replace(/\\/g, '/'));
    const cue = names.filter((n) => /\.cue$/i.test(n)).sort((a, b) => a.length - b.length)[0];
    if (cue)
        return {kind: 'image', source: cue};
    let best = null;
    for (const n of names) {
        const m = /^(.*\/)?data1\.cab$/i.exec(n);
        if (!m)
            continue;
        const dir = m[1] || '';
        if (names.some((k) => k.toLowerCase().startsWith((dir + 'civ2/').toLowerCase())) &&
            (best === null || dir.length < best.length))
            best = dir;
    }
    return best === null ? null : {kind: 'cd', source: best.replace(/\/$/, '')};
}
