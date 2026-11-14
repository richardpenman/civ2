// The player's game files in the browser's IndexedDB: the copy of their
// installed Civilization II made on the first visit, and what the game writes
// to it since (saved games, CIV.INI, CIV2.DAT ...).  Keys are paths relative
// to the game's folder ("SOUND/DIVINE.WAV"), values {data: Uint8Array, mtime}.

// A second store, meta, notes what the copy of the game had ('game':
// {names, at}: its paths, when it was made), so that the player's own files
// can be told from the game's (saves.js).

const DB = 'OpenCiv2', FILES = 'files', META = 'meta';

function req(r) {
    return new Promise((resolve, reject) => {
        r.onsuccess = () => resolve(r.result);
        r.onerror = () => reject(r.error);
    });
}

export async function openStore() {
    const open = indexedDB.open(DB, 2);
    open.onupgradeneeded = (e) => {
        if (e.oldVersion < 1)
            open.result.createObjectStore(FILES);
        if (e.oldVersion < 2)
            open.result.createObjectStore(META);
    };
    const db = await req(open);
    const tx = (mode) => db.transaction(FILES, mode).objectStore(FILES);
    const done = (t) => new Promise((resolve, reject) => {
        t.oncomplete = () => resolve();
        t.onerror = t.onabort = () => reject(t.error);
    });
    return {
        async count() {
            return req(tx('readonly').count());
        },
        async keys() {
            return req(tx('readonly').getAllKeys());
        },
        async all() {
            const s = tx('readonly');
            const [keys, values] = await Promise.all([req(s.getAllKeys()), req(s.getAll())]);
            return keys.map((k, i) => [k, values[i]]);
        },
        // entries: [[path, Uint8Array, mtime]]
        async putMany(entries) {
            const t = db.transaction(FILES, 'readwrite');
            const s = t.objectStore(FILES);
            for (const [k, data, mtime] of entries)
                s.put({data, mtime: mtime || Date.now()}, k);
            return done(t);
        },
        async remove(k) {
            const t = db.transaction(FILES, 'readwrite');
            t.objectStore(FILES).delete(k);
            return done(t);
        },
        async clear() {
            const t = db.transaction([FILES, META], 'readwrite');
            t.objectStore(FILES).clear();
            t.objectStore(META).clear();
            return done(t);
        },
        async getMeta(k) {
            return req(db.transaction(META, 'readonly').objectStore(META).get(k));
        },
        async setMeta(k, v) {
            const t = db.transaction(META, 'readwrite');
            t.objectStore(META).put(v, k);
            return done(t);
        },
    };
}
