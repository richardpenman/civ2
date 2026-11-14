// The player's own files among the game's (saved games, the scenarios and
// maps they made, CIV.INI, CIV2.DAT, HALLFAME.DAT): exported as a .zip, and
// imported (.SAV files, or such a .zip) into the browser's storage, and into
// the running game's files so that it can load them at once.

import {zip, unzip} from './zip.js';

const SETTINGS = new Set(['civ.ini', 'civ2.dat', 'hallfame.dat']);
const base = (name) => name.slice(name.lastIndexOf('/') + 1).toLowerCase();
const dirOf = (name) => (name.includes('/') ? name.slice(0, name.lastIndexOf('/')) : '');

// a file of the kinds the player makes (any folder for the scenarios'
// .SCN and .MP; CIV.INI ... in the game's own folder)
export function savesKind(name) {
    const b = base(name);
    if (/\.sav$/i.test(b))
        return 'sav';
    if (/\.(scn|mp)$/i.test(b))
        return 'scn';
    return !name.includes('/') && SETTINGS.has(b) ? 'settings' : null;
}

// The player's files among the store's entries ([[path, {data, mtime}]]):
// paths.  game: what the copy of the game had ({names: [path...], at: ms},
// store.js's meta), so that the scenarios it came with stay out unless the
// player changed them; without it (a copy made before it was noted) a
// scenario is the player's if written well after Civ2Art.dll was copied.
export function playerFiles(entries, game) {
    const shipped = game ? new Set(game.names.map((n) => n.toLowerCase())) : null;
    let since = game ? game.at : Infinity;
    if (!game)
        for (const [k, v] of entries)
            if (k.toLowerCase() === 'civ2art.dll')
                since = v.mtime + 10 * 60 * 1000;
    return entries.filter(([k, v]) => {
        const kind = savesKind(k);
        if (kind === 'scn')
            return shipped ? !shipped.has(k.toLowerCase()) || v.mtime > since : v.mtime > since;
        return kind !== null;
    }).map(([k]) => k);
}

// The player's files as a .zip: {data: Uint8Array, names}
export async function exportSaves(store) {
    const entries = await store.all();
    const keys = new Set(playerFiles(entries, await store.getMeta('game')));
    const files = entries.filter(([k]) => keys.has(k)).map(([k, v]) => ({name: k, data: v.data, mtime: v.mtime}));
    return {data: await zip(files), names: files.map((f) => f.name)};
}

// Where an imported file goes in the game's folder (existing: the store's
// paths): its path, less the folders in front of it the game's folder does
// not have ("Civ2/A.SAV", "saves/A.SAV" -> "A.SAV"; "Scenario/X.SCN" kept
// where the game has a Scenario folder), in the case of a file or folder
// already there.
export function importPath(name, existing) {
    const lower = new Map(existing.map((k) => [k.toLowerCase(), k]));
    const dirs = new Map();
    for (const k of existing)
        for (let d = dirOf(k); d; d = dirOf(d))
            dirs.set(d.toLowerCase(), d);
    const parts = name.replace(/\\/g, '/').split('/').filter((p) => p && p !== '.' && p !== '..');
    for (let i = 0; i < parts.length; i++) {
        const rest = parts.slice(i).join('/'), dir = dirOf(rest);
        if (!dir || dirs.has(dir.toLowerCase())) {
            const known = lower.get(rest.toLowerCase());
            if (known)
                return known;
            return dir ? dirs.get(dir.toLowerCase()) + rest.slice(dir.length) : rest;
        }
    }
    return parts[parts.length - 1] || '';
}

// files: [{name, data}] chosen by the player (.SAV ..., .zip archives of
// them) -> [[path, data]] to put in the game's folder (other files left out)
export async function savesToImport(files, existing) {
    const out = new Map();
    const add = (name, data) => {
        const p = importPath(name, existing);
        if (savesKind(p))
            out.set(p, data);
    };
    for (const f of files) {
        if (/\.zip$/i.test(f.name))
            for (const g of await unzip(f.data.buffer.slice(f.data.byteOffset, f.data.byteOffset + f.data.length)))
                add(g.name, g.data);
        else
            add(f.name, f.data);
    }
    return [...out];
}

// Imported: into the store, and the running game's files (fs, dir: its
// folder there) if one runs.  -> the paths
export async function importSaves(store, files, fs = null, dir = '') {
    const existing = await store.keys();
    const put = await savesToImport(files, existing);
    const now = Date.now();
    await store.putMany(put.map(([k, data]) => [k, data, now]));
    if (fs)
        for (const [k, data] of put)
            fs.put(dir + '/' + k, data.slice(), now);
    return put.map(([k]) => k);
}
