// The page: the player's game files (chosen once, kept in IndexedDB), then
// the game (host.js) on the canvas.

import {MemFS} from './wasi.js';
import {runGame} from './host.js';
import {openStore} from './store.js';
import {unzip} from './zip.js';
import {exportSaves, importSaves} from './saves.js';
import {findDisc} from './setup_run.js';
import {hdWanted, hasOriginalPictures, iniGet, iniSet} from './options.js';
import {t, languages, pageLanguage, urlLanguage, withLanguage, useLanguage, langFile, parseLang, match} from './i18n.js';

const GAME = '/civ2', DATA = '/data';
let pageLang = 'en';        // the page's language (i18n.js: ?lang=CODE, else English)
const $ = (id) => document.getElementById(id);
// (on the set-up page, and over the screen until the game's first frame)
const status = (s) => {
    $('status').textContent = s;
    $('loading').textContent = s;
    window.civ2Status = s;
};
// a start's steps, in ms since the page was opened (the tests read it)
const timing = window.civ2Timing = {};
const mark = (name, p) => p.then((v) => {
    timing[name] = Math.round(performance.now());
    return v;
});

// The installed game among the files chosen: the folder with Civ2Art.dll and
// RULES.TXT (any case), as the native set-up checks (src/host/setup.c).
// files: [{name: "a/b/c", data|file}] -> {root, files} or an error text
function findGame(files) {
    const dirs = new Map();
    for (const f of files) {
        const i = f.name.lastIndexOf('/');
        const dir = i < 0 ? '' : f.name.slice(0, i), base = f.name.slice(i + 1).toLowerCase();
        if (!dirs.has(dir))
            dirs.set(dir, new Set());
        dirs.get(dir).add(base);
    }
    let root = null;
    for (const [dir, names] of dirs)
        if (names.has('civ2art.dll') && names.has('rules.txt') && (root === null || dir.length < root.length))
            root = dir;
    if (root === null) {
        return t('web.not_game');
    }
    const pre = root ? root + '/' : '';
    return {files: files.filter((f) => f.name.startsWith(pre) && !/\.log$/i.test(f.name))
                        .map((f) => ({...f, name: f.name.slice(pre.length)}))};
}

async function importFiles(store, files, progress = null) {
    const found = findGame(files);
    if (typeof found === 'string') {
        const disc = findDisc(files);
        if (disc)
            return setupFromDisc(store, files, disc);
        status(found);
        return false;
    }
    await store.clear();
    let batch = [], size = 0, n = 0;
    for (const f of found.files) {
        const data = f.data || new Uint8Array(await f.file.arrayBuffer());
        batch.push([f.name, data, f.file ? f.file.lastModified : Date.now()]);
        size += data.length;
        n++;
        if (size > 16 << 20) {
            status(t('web.copying', n, found.files.length));
            if (progress)
                progress(n, found.files.length);
            await store.putMany(batch);
            batch = [];
            size = 0;
        }
    }
    await store.putMany(batch);
    await store.setMeta('game', {names: found.files.map((f) => f.name), at: Date.now()});
    status(t('web.kept', n));
    return true;
}

// The game set up from its CD or a disc image (disc: findDisc's), as the
// desktop version's set-up does (setup_worker.js runs it): into the store
// as its files come; or (into: an array) its files into that, the store
// left as it is (the original's files added to a game: addChosen).
function setupFromDisc(store, files, disc, into = null) {
    return new Promise((resolve) => {
        const worker = new Worker('setup_worker.js', {type: 'module'});
        const stop = $('stop'), names = new Set(), messages = [];
        let queue = into ? Promise.resolve() : store.clear(), batch = [], size = 0, ended = false;
        const flush = () => {
            const b = batch;
            batch = [];
            size = 0;
            if (b.length)
                queue = queue.then(() => store.putMany(b));
        };
        const end = (ok, text) => {
            if (ended)
                return;
            ended = true;
            worker.terminate();
            stop.hidden = true;
            if (into) {
                status(ok ? '' : text);
                resolve(ok);
                return;
            }
            flush();
            queue.then(async () => {
                if (ok && typeof findGame([...names].map((name) => ({name}))) !== 'string') {
                    await store.setMeta('game', {names: [...names], at: Date.now()});
                    status(text);
                    resolve(true);
                } else {
                    await store.clear();
                    status(ok ? t('web.setup_no_game') : text);
                    resolve(false);
                }
            }, (e) => {
                status(t('web.keep_failed', e.message));
                resolve(false);
            });
        };
        worker.onmessage = (e) => {
            const m = e.data;
            if (m.file !== undefined && into) {
                into.push({name: m.file, data: m.data});
            } else if (m.file !== undefined) {
                names.add(m.file);
                batch.push([m.file, m.data, Date.now()]);
                size += m.data.length;
                if (size > 16 << 20)
                    flush();
            } else if (m.progress) {
                status(`${m.progress[0]}: ${m.progress[1].replace(/^\/(game|src)\//, '')}`);
            } else if (m.message) {
                messages.push(m.message[0]);
                console.warn('set-up:', m.message[0]);
            } else if (m.done !== undefined) {
                end(m.done === 0, m.done === 0 ? t(disc.kind === 'cd' ? 'web.set_up_cd' : 'web.set_up_image', names.size)
                    : t('web.setup_failed', messages.join(' ') || 'code ' + m.done));
            } else if (m.error) {
                end(false, t('web.setup_failed', m.error));
            }
        };
        worker.onerror = (e) => end(false, t('web.setup_failed', e.message || 'its worker stopped'));
        stop.hidden = false;
        stop.onclick = () => end(false, t('web.setup_stopped'));
        status(t(disc.kind === 'cd' ? 'web.setting_up_cd' : 'web.setting_up_image'));
        langFile(pageLang).then((lang) =>
            worker.postMessage({files: files.map((f) => ({name: f.name, file: f.file || new Blob([f.data])})),
                                source: disc.source, lang}));
    });
}

// ---- the original's files, later: Settings' Classic graphics ----

// While the set-up page is shown for them (host_add_original: addOriginal):
// {store, fs, resolve}; the files chosen go to addChosen then
let adding = null;

// The files of the player's copy ([{name, data|file}], under the game's
// folder) the running game lacks (any case), put into its files and kept in
// the store, -> how many
async function mergeFiles(store, fs, files) {
    const have = new Set([...fs.files.keys()].filter((k) => k.startsWith(GAME + '/'))
        .map((k) => k.slice(GAME.length + 1).toUpperCase()));
    const added = [];
    let batch = [], size = 0;
    for (const f of files) {
        if (have.has(f.name.toUpperCase()) || /\.log$/i.test(f.name))
            continue;
        const data = f.data || new Uint8Array(await f.file.arrayBuffer());
        fs.put(GAME + '/' + f.name, data);
        batch.push([f.name, data, Date.now()]);
        added.push(f.name);
        size += data.length;
        if (size > 16 << 20) {
            status(t('web.copying', added.length, files.length));
            await store.putMany(batch);
            batch = [];
            size = 0;
        }
    }
    await store.putMany(batch);
    const meta = await store.getMeta('game');
    if (meta && meta.names)
        await store.setMeta('game', {...meta, names: [...meta.names, ...added]});
    return added.length;
}

// The player's choice on the set-up page while adding: an installed game's
// folder or a .zip of it, its CD or a disc image (set up into memory, as
// at a first visit); the game's own files kept.  Done when the game's
// files then have the original's pictures (hasOriginalPictures).
async function addChosen(files) {
    const {store, fs} = adding;
    try {
        let found = findGame(files);
        if (typeof found === 'string') {
            const disc = findDisc(files);
            if (!disc) {
                status(found);
                return;
            }
            const made = [];
            if (!await setupFromDisc(store, files, disc, made))
                return;
            found = {files: made};
        }
        const n = await mergeFiles(store, fs, found.files);
        const names = [...fs.files.keys()].filter((k) => k.startsWith(GAME + '/')).map((k) => k.slice(GAME.length + 1));
        if (!hasOriginalPictures(names)) {
            status(t('web.add_no_pictures'));
            return;
        }
        status(t('web.added', n));
        endAdding(1);
    } catch (e) {
        console.error(e);
        status(t('web.copy_failed', e.message));
    }
}

// host_add_original (the game's Settings, Classic graphics ticked
// without the original's files): the set-up page's choice shown over the
// game, -> 1 the game's files have the original's pictures now, 0 not
// (Back to the game)
function addOriginal(store, fs) {
    return new Promise((resolve) => {
        adding = {store, fs, resolve};
        $('game').hidden = true;
        $('setup').hidden = false;
        $('have').hidden = true;
        $('chosen').hidden = true;
        $('add-note').hidden = false;
        $('add-back').hidden = false;
        $('add-back').onclick = () => endAdding(0);
        status('');
        window.civ2Adding = true;       // (the tests)
    });
}

function endAdding(r) {
    const a = adding;
    adding = null;
    window.civ2Adding = false;
    $('add-note').hidden = true;
    $('add-back').hidden = true;
    $('setup').hidden = true;
    $('game').hidden = false;
    $('screen').focus();
    a.resolve(r);
}

// a dropped folder's files, through its entries
async function entryFiles(entry, prefix, out) {
    if (entry.isFile) {
        const file = await new Promise((resolve, reject) => entry.file(resolve, reject));
        out.push({name: prefix + entry.name, file});
    } else if (entry.isDirectory) {
        const reader = entry.createReader();
        for (;;) {
            const batch = await new Promise((resolve, reject) => reader.readEntries(resolve, reject));
            if (!batch.length)
                break;
            for (const e of batch)
                await entryFiles(e, prefix + entry.name + '/', out);
        }
    }
    return out;
}

// what was chosen, said under the button: "Folder: Civ2 (412 files)",
// "Disc image: CIV2.cue + CIV2.bin", "Civ2.zip (25 MB)"
function showChosen(files) {
    const p = $('chosen');
    const top = new Set(files.map((f) => f.name.split('/')[0]));
    if (files.some((f) => f.name.includes('/')))
        p.textContent = t('web.chosen_folder', [...top].join(', '), files.length);
    else if (files.some((f) => /\.cue$/i.test(f.name)))
        p.textContent = t('web.chosen_image', files.map((f) => f.name).join(' + '));
    else if (files.length === 1)
        p.textContent = files[0].name + (files[0].file ? ` (${Math.round(files[0].file.size / 1e6)} MB)` : '');
    else
        p.textContent = files.map((f) => f.name).join(', ');
    p.hidden = false;
}

async function zipFiles(file) {
    status(t('web.reading', file.name));
    return (await unzip(await file.arrayBuffer())).map((f) => ({name: f.name, data: f.data}));
}

async function fetchGz(url) {
    const r = await fetch(url);
    if (!r.ok)
        throw new Error(url + ': ' + r.status);
    return new Uint8Array(await new Response(r.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
}

function messageBox(icon, title, text, labels, ids, enter, escape) {
    return new Promise((resolve) => {
        const d = $('dialog');
        $('dialog-title').textContent = title;
        $('dialog-text').textContent = text;
        const buttons = $('dialog-buttons');
        buttons.textContent = '';
        const close = (i) => {
            d.close();
            resolve(i < 0 ? -1 : ids[i]);
        };
        labels.forEach((l, i) => {
            const b = document.createElement('button');
            b.textContent = l.replace(/&/g, '');
            b.onclick = () => close(i);
            buttons.append(b);
            if (i === enter)
                setTimeout(() => b.focus(), 0);
        });
        d.oncancel = (e) => {
            e.preventDefault();
            if (escape >= 0)
                close(escape);
        };
        d.showModal();
    });
}

// The network games' relay (tools/webrelay.py): ?relay=ws://HOST:PORT, else
// the address the player gave (kept in this browser), else port 8765 of
// the site's own host
const relayDefault = () => `${location.protocol === 'https:' ? 'wss' : 'ws'}://${location.hostname || '127.0.0.1'}:8765`;
function relayAddress() {
    const q = new URLSearchParams(location.search).get('relay');
    if (q !== null)
        return q;
    try {
        return localStorage.getItem('civ2.relay') || relayDefault();
    } catch (e) {
        return relayDefault();
    }
}

// A network game's connection could not reach the relay: the player told
// how to run it, and asked for its address (kept; the next connection uses it)
function relayUnreachable(relay) {
    const d = $('relay-dialog');
    if (d.open)
        return;
    let host = relay;
    try {
        host = new URL(relay).hostname || relay;
    } catch (e) { /* not an address */ }
    $('relay-text').textContent = t('web.relay_none', relay, host);
    const input = $('relay-address');
    input.value = relay;
    $('relay-use').onclick = () => {
        const v = input.value.trim();
        if (v && !/^wss?:\/\/./.test(v)) {
            $('relay-text').textContent = t('web.relay_bad');
            return;
        }
        try {
            if (!v || v === relayDefault())
                localStorage.removeItem('civ2.relay');
            else
                localStorage.setItem('civ2.relay', v);
        } catch (e) { /* not kept */ }
        d.close();
    };
    $('relay-close').onclick = () => d.close();
    d.showModal();
    input.focus();
}

// ---- the player's saved games: exported, imported (saves.js) ----

let running = null;         // the game's files while it runs (MemFS)

function savesStatus(s) {
    $('saves-status').textContent = s;
    status(s);
}

async function exportClicked(store) {
    try {
        if (running)
            await window.civ2Flush();
        const {data, names} = await exportSaves(store);
        if (!names.length) {
            savesStatus(t('web.no_saves'));
            return;
        }
        const a = document.createElement('a');
        const d = new Date(), pad = (n) => String(n).padStart(2, '0');
        a.download = `OpenCiv2-saves-${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())}.zip`;
        a.href = URL.createObjectURL(new Blob([data], {type: 'application/zip'}));
        document.body.append(a);
        a.click();
        a.remove();
        setTimeout(() => URL.revokeObjectURL(a.href), 60000);
        savesStatus(names.length > 1 ? t('web.exported', names.length, names.join(', ')) : t('web.exported_one', names[0]));
    } catch (e) {
        console.error(e);
        savesStatus(t('web.export_failed', e.message));
    }
}

async function importChosen(store, list) {
    try {
        const files = [];
        for (const f of list)
            files.push({name: f.name, data: new Uint8Array(await f.arrayBuffer())});
        const names = await importSaves(store, files, running, GAME);
        savesStatus(names.length > 1 ? t('web.imported', names.length, names.join(', '))
            : names.length ? t('web.imported_one', names[0]) : t('web.nothing_to_import'));
    } catch (e) {
        console.error(e);
        savesStatus(t('web.import_failed', e.message));
    }
}

function savesControls(store) {
    for (const b of document.querySelectorAll('.export-saves'))
        b.onclick = () => exportClicked(store);
    $('import-saves').onchange = async (e) => {
        const list = [...e.target.files];
        e.target.value = '';
        if (list.length)
            await importChosen(store, list);
    };
    // while playing: a small button over the screen's corner, its dialog
    // keeping the keys from the game
    const d = $('saves-dialog');
    $('saves-open').onclick = () => {
        $('saves-status').textContent = '';
        d.showModal();
    };
    $('saves-close').onclick = () => d.close();
    d.addEventListener('keydown', (e) => e.stopPropagation());
    d.addEventListener('keyup', (e) => e.stopPropagation());
    d.addEventListener('close', () => $('screen').focus());
}

// ---- the HD graphics' art, as CIV.INI asks for it (options.js) ----

// The art (data/native/hd: hd/pictures.txt and its pictures), OpenCiv2's
// own heralds (data/native/heralds: heralds/...) and HD pictures
// (data/native/hd-extra: hd-extra/...), on the site as one file, hd/art.bin,
// its files one after another as hd/index.json lists them ([path, size]
// each) -> [[path, bytes]], put where hd.c looks for them (CIV2_DATA/hd,
// CIV2_DATA/heralds, CIV2_DATA/hd-extra)
async function fetchArt() {
    const index = await fetch('hd/index.json').then((r) => {
        if (!r.ok)
            throw new Error('hd/index.json: ' + r.status);
        return r.json();
    });
    const r = await fetch('hd/art.bin');
    if (!r.ok)
        throw new Error('hd/art.bin: ' + r.status);
    const bytes = new Uint8Array(await r.arrayBuffer());
    let at = 0;
    const files = index.map(([path, size]) => [path, bytes.subarray(at, (at += size))]);
    if (at !== bytes.length)
        throw new Error('hd/art.bin is not as hd/index.json says');
    return files;
}

const latin = new TextDecoder('windows-1252');

// CIV.INI as the game has it in its folder (any case), or where it will write it
function iniOf(entries) {
    const e = entries.find(([k]) => /^civ\.ini$/i.test(k));
    return {name: e ? e[0] : 'CIV.INI', text: e ? latin.decode(e[1].data) : ''};
}

// CIV.INI's bytes with [OpenCiv2] key=value (its other bytes as they are)
function iniBytesSet(bytes, key, value) {
    let s = '';
    for (const b of bytes)
        s += String.fromCharCode(b);
    return Uint8Array.from(iniSet(s, 'OpenCiv2', key, value), (c) => c.charCodeAt(0));
}
const iniLanguage = (bytes) => (iniGet(latin.decode(bytes || new Uint8Array(0)), 'OpenCiv2', 'Language') || '').trim();

// What a start needs from the site, fetched at once, alongside the stored
// files: the module (compiled as it downloads), the fonts
let siteP = null;
function fromSite() {
    if (!siteP) {
        const compile = async () => {
            try {
                return await WebAssembly.compileStreaming(fetch('civ2.wasm'));
            } catch (e) {
                // (a server that does not say application/wasm)
                const r = await fetch('civ2.wasm');
                if (!r.ok)
                    throw new Error('civ2.wasm: ' + r.status);
                return WebAssembly.compile(await r.arrayBuffer());
            }
        };
        siteP = {wasm: mark('wasm', compile()),
                 fonts: mark('fonts', Promise.all([fetchGz('data/fonts_win.bin.gz'), fetchGz('data/fonts_win_aa.bin.gz')]))};
        // (a failure is met where it is awaited)
        for (const p of Object.values(siteP))
            p.catch(() => {});
    }
    return siteP;
}

// ---- the game from the site itself (make web GAME=1: game/index.json) ----

// The site's copy of the game, or null: {core: {size, files}, files: [[path, size]]}
let siteGameP = null;
const siteGame = () => siteGameP || (siteGameP = fetch('game/index.json', {cache: 'no-cache'})
    .then((r) => (r.ok ? r.json() : null)).catch(() => null));

// A first visit to such a site: its copy kept in this browser, without
// asking (the set-up page's choice is not shown: ?setup shows it), on one
// progress bar: game/core.zip and the files beside it (game/files: the
// music, the animated advisors), downloaded, then unpacked and kept.  On
// later visits, those files the kept game lacks (a site that has added
// them since): so that the game never waits on the site while it plays.
const installProgress = (step, frac) => {
    const pct = Math.max(0, Math.min(100, Math.floor(frac * 100)));
    $('install-step').textContent = step;
    $('install-fill').style.width = `calc((100% - 6px) * ${pct / 100})`;
    $('install-pct').textContent = pct + '%';
    $('install-bar').setAttribute('aria-valuenow', pct);
    window.civ2Status = step;
};
const MB1 = (n) => (n / 1e6).toFixed(1);
// (of the bar: the download, then the files unpacked and kept)
const DOWNLOAD = 0.9;
async function installFromSite(store, g, first) {
    const have = first ? new Set() : new Set((await store.keys()).map((k) => k.toUpperCase()));
    const extra = (g.files || []).filter(([path]) => !have.has(path.toUpperCase()));
    if (!first && !extra.length)
        return;
    $('loading').hidden = true;
    $('install').hidden = false;
    // ("the first visit only": not on a later one's top-up)
    $('install').querySelector('[data-t="web.install_note"]').hidden = !first;
    const total = (first ? g.core.size : 0) + extra.reduce((n, [, size]) => n + size, 0) || 1;
    let done = 0;
    const shown = () => installProgress(t('web.install_download', MB1(done), MB1(total)), DOWNLOAD * done / total);
    shown();
    const get = async (url) => {
        const r = await fetch(url);
        if (!r.ok)
            throw new Error(url + ': ' + r.status);
        const parts = [], reader = r.body.getReader();
        for (;;) {
            const {done: end, value} = await reader.read();
            if (end)
                break;
            parts.push(value);
            done += value.length;
            shown();
        }
        return new Uint8Array(await new Blob(parts).arrayBuffer());
    };
    const core = first ? get('game/core.zip') : null;
    // (the others a few at a time, beside it)
    const got = new Array(extra.length);
    let next = 0;
    const worker = async () => {
        while (next < extra.length) {
            const k = next++;
            got[k] = {name: extra[k][0], data: await get('game/files/' + extra[k][0].split('/').map(encodeURIComponent).join('/'))};
        }
    };
    await Promise.all([core, ...[0, 1, 2].map(worker)]);
    const keep = (n, of) => installProgress(t('web.install_keep', n, of), DOWNLOAD + (1 - DOWNLOAD) * n / of);
    if (!first) {
        keep(0, got.length);
        await store.putMany(got.map((f) => [f.name, f.data, Date.now()]));
        const meta = await store.getMeta('game');
        if (meta && meta.names)
            await store.setMeta('game', {...meta, names: [...meta.names, ...got.map((f) => f.name)]});
        keep(got.length, got.length);
        return;
    }
    let k = 0;
    const count = (g.core.files || 1) + got.length;
    const files = (await unzip((await core).buffer, () => {
        k++;
        if (k % 8 === 0)
            installProgress(t('web.install_unpack', k, count), DOWNLOAD + (1 - DOWNLOAD) * 0.5 * k / count);
    })).map((f) => ({name: f.name, data: f.data})).concat(got);
    if (!await importFiles(store, files, (n, of) => keep(n, of)))
        throw new Error(window.civ2Status);
    keep(files.length, files.length);
}

async function play(store) {
    // (the game files and saved games kept for good: without this the browser
    // may clear them when disk space runs low, Safari after a week unused)
    if (navigator.storage && navigator.storage.persist)
        navigator.storage.persist().catch(() => {});
    $('setup').hidden = true;
    $('install').hidden = true;
    $('game').hidden = false;
    $('loading').hidden = false;
    status(t('web.loading_files'));
    const t0 = performance.now();
    const site = fromSite();
    const fs = new MemFS();
    // CIV.INI first (a moment): the HD graphics' art, when wanted, read
    // alongside the other files
    const query = new URLSearchParams(location.search), args = ['civ2', GAME];
    const keys = await store.keys();
    const iniName = keys.find((k) => /^civ\.ini$/i.test(k));
    const ini = iniOf(iniName ? [[iniName, await store.get(iniName)]] : []).text;
    // (always where the game's files lack the original's pictures: the game
    // makes them from the art, docs/NO-INSTALL.md)
    const hd = hdWanted(ini, query.get('hd'), hasOriginalPictures(keys)) >= 0;
    const artP = hd ? mark('hd', fetchArt()).catch((e) => {
        console.error(e);
        return null;
    }) : null;
    // (the HD art of former versions, downloaded and kept in this browser:
    // the art is the site's now, its room freed)
    store.hdClear().catch(() => {});
    const [entries, [fonts, fontsAa]] = await Promise.all([mark('files', store.all()), site.fonts]);
    for (const [k, v] of entries)
        fs.put(GAME + '/' + k, v.data, v.mtime);
    fs.mkdirs(GAME);
    // ?lang=CODE: the game's language too, as its Settings have it
    // (CIV.INI [OpenCiv2] Language=, in the game's files; kept in this
    // browser only when the game changes it: the language the player had
    // before stays theirs, for a visit without ?lang)
    const iniPath = GAME + '/' + (iniName || 'CIV.INI');
    let urlLang = null, gameIni = ini;
    if (urlLanguage()) {
        urlLang = {code: pageLang, before: iniGet(ini, 'OpenCiv2', 'Language') || 'Auto'};
        fs.put(iniPath, iniBytesSet(fs.get(iniPath) || new Uint8Array(0), 'Language', pageLang));
        gameIni = latin.decode(fs.get(iniPath));
    }
    // OpenCiv2's English of the game's text files the player's game lacks
    // (data/text: the game reads them where it has none of its own)
    try {
        const have = new Set(entries.map(([k]) => k.toUpperCase()));
        const names = await fetch('data/text/index.json').then((r) => (r.ok ? r.json() : []));
        const missing = names.filter((n) => !have.has(n));
        const files = await Promise.all(missing.map((n) => fetchGz('data/text/' + n + '.gz')));
        missing.forEach((n, i) => fs.put(DATA + '/text/' + n, files[i]));
    } catch (e) {
        console.warn('OpenCiv2 texts:', e);
    }
    // OpenCiv2's own sound effects for those the player's game lacks
    // (data/sound: sound.c plays one where the game has no Sound/NAME.WAV)
    try {
        const have = new Set(entries.map(([k]) => k.toUpperCase()));
        const names = await fetch('data/sound/index.json').then((r) => (r.ok ? r.json() : []));
        const missing = names.filter((n) => !have.has('SOUND/' + n.slice(0, -4).toUpperCase() + '.WAV'));
        const files = await Promise.all(missing.map((n) => fetch('data/sound/' + n).then((r) => {
            if (!r.ok)
                throw new Error(n + ': ' + r.status);
            return r.arrayBuffer();
        })));
        missing.forEach((n, i) => fs.put(DATA + '/sound/' + n, new Uint8Array(files[i])));
    } catch (e) {
        console.warn('OpenCiv2 sounds:', e);
    }
    fs.put(DATA + '/fonts_win.bin', fonts);
    fs.put(DATA + '/fonts_win_aa.bin', fontsAa);
    // the translations (the game's Language choice lists them; Auto is
    // English), and, whatever the language, the font files of characters
    // beyond Windows-1252 (text.c draws them: a UTF-8 translation's, and
    // each language's own name in the Language list; other code pages'
    // letters it makes from the fonts above)
    const gameLang = await gameLanguage(gameIni);
    for (const [code, bytes] of gameLang.files)
        fs.put(DATA + '/lang/' + code + '.txt', bytes);
    try {
        const names = await fetch('data/fonts/index.json').then((r) => (r.ok ? r.json() : []));
        const files = await Promise.all(names.map((n) => fetchGz('data/fonts/' + n + '.gz')));
        names.forEach((n, i) => fs.put(DATA + '/fonts/' + n, files[i]));
    } catch (e) {
        console.warn('no font files', e);
    }
    // HD graphics (?hd=N as --hd=N, else CIV.INI's choice): the art the
    // game draws, where hd.c looks for it (CIV2_DATA/hd)
    if (query.has('hd'))
        args.push('--hd=' + (query.get('hd') || '1'));
    // (sharp text and interface: ?sharp=0 off, as --sharp=0; else CIV.INI's)
    if (query.has('sharp'))
        args.push('--sharp=' + (query.get('sharp') || '1'));
    window.civ2Hd = null;
    if (hd) {
        const since = performance.now();
        status(t('web.loading_hd'));
        const art = await artP;
        for (const [path, bytes] of art || [])
            fs.put(DATA + '/' + path, bytes);
        window.civ2Hd = {files: art ? art.length : 0, bytes: art ? art.reduce((n, [, b]) => n + b.length, 0) : 0,
                         loadMs: Math.round(performance.now() - since)};
        if (!art)
            console.warn('HD graphics: the site\'s art could not be loaded');
    }
    // what the game writes, kept: at once when a file is closed, renamed or
    // removed (a saved game is safe as soon as the game has written it),
    // else a second after the last write to a file still open
    const dirty = new Map();
    let timer = 0;
    const flush = async () => {
        timer = 0;
        const now = [...dirty];
        dirty.clear();
        const put = [];
        for (const [path, removed] of now) {
            const rel = path.slice(GAME.length + 1);
            const data = fs.get(path);
            if (removed || !data)
                await store.remove(rel);
            else if (urlLang && path.toLowerCase() === iniPath.toLowerCase() && iniLanguage(data) === urlLang.code)
                put.push([rel, iniBytesSet(data, 'Language', urlLang.before), Date.now()]);
            else
                put.push([rel, data.slice(), Date.now()]);
        }
        if (put.length)
            await store.putMany(put);
    };
    fs.onchange = (path, removed, partial) => {
        if (!path.startsWith(GAME + '/') || /\.log$/i.test(path))
            return;
        dirty.set(path, removed);
        if (!partial) {
            clearTimeout(timer);
            flush();
        } else if (!timer) {
            timer = setTimeout(flush, 1000);
        }
    };
    window.civ2Fs = fs;
    running = fs;
    window.civ2Flush = () => (timer ? (clearTimeout(timer), flush()) : Promise.resolve());
    window.addEventListener('pagehide', () => window.civ2Flush());
    status(t('web.starting', Math.round(performance.now() - t0)));
    const canvas = $('screen');
    canvas.focus();
    // (the platform's log: ?log, in the console)
    // (Auto, the system's language: English)
    const env = {CIV2_DATA: DATA, CIV2_SYSTEM_LANG: 'en'};
    if (!new URLSearchParams(location.search).has('log'))
        env.CIV2_HOOK_LOG = '/dev/null';
    // (tests, src/web/test/smoke.mjs: files and settings of a replay)
    if (window.civ2Test)
        await window.civ2Test(fs, env);
    // OpenCiv2's own music (data/music, some 26 MB): with HD graphics all
    // its tracks (sound.c plays data/music/trackNN.ogg as track NN first,
    // the game's Music/TrackNN only for a track it lacks), with Classic
    // graphics those the player's game lacks (it plays the game's
    // Music/TrackNN.ogg, else .wav, else ours); fetched while the game starts
    // and plays, not waited for; put in all at once when all have come, so
    // that the game, which counts the tracks when it opens the CD drive,
    // finds all of them or none (until then it plays the game's own, or as
    // without music); window.civ2OwnMusic: the tracks put in (tests)
    (async () => {
        const have = new Set([...fs.files.keys()].map((k) => k.toUpperCase()));
        const names = await fetch('data/music/index.json').then((r) => (r.ok ? r.json() : []));
        const wanted = hd ? names : names.filter((n) => {
            const track = (GAME + '/MUSIC/' + n.slice(0, -4)).toUpperCase();
            return !have.has(track + '.OGG') && !have.has(track + '.WAV');
        });
        const files = await Promise.all(wanted.map((n) => fetch('data/music/' + n).then((r) => {
            if (!r.ok)
                throw new Error(n + ': ' + r.status);
            return r.arrayBuffer();
        })));
        wanted.forEach((n, i) => fs.put(DATA + '/music/' + n, new Uint8Array(files[i])));
        window.civ2OwnMusic = wanted;
    })().catch((e) => console.warn('OpenCiv2 music:', e));
    let code;
    try {
        window.civ2Stats = {};
        code = await runGame({wasm: site.wasm, fs,
                              onFrame: () => {
                                  timing.frame = Math.round(performance.now());
                                  $('loading').hidden = true;
                              }, canvas, args, env, messageBox, codepage: gameLang.codepage,
                              stats: window.civ2Stats, relay: relayAddress,
                              onRelayUnreachable: relayUnreachable,
                              addOriginal: () => addOriginal(store, fs),
                              // (the game's Settings' restart: the page loaded again, into the
                              // game, without what would override them; ?lang= the game's language now,
                              // where the game changed it)
                              restart: async () => {
                                  await window.civ2Flush();
                                  const u = new URL(location.href);
                                  for (const k of ['setup', 'hd', 'sharp'])
                                      u.searchParams.delete(k);
                                  const now = iniLanguage(fs.get(iniPath));
                                  if (!urlLang || now !== urlLang.code)
                                      withLanguage(u, now);
                                  location.replace(u.href);
                              }});
        status(code ? t('web.ended_code', code) : t('web.ended'));
    } catch (e) {
        console.error(e);
        status(t('web.stopped', e.message));
    }
    await window.civ2Flush();
    running = null;
    $('game').hidden = true;
    showSetup(store, true);
}

// The game's language (as src/game/lang.cpp chooses it: CIV.INI's
// Language=, ?lang='s where the URL has it; Auto: English) and the site's translation files
// -> {files: [[code, bytes]], codepage: the game's language's}
async function gameLanguage(ini) {
    const list = await languages();
    const files = (await Promise.all(list.map(async (l) => [l.code, await langFile(l.code)])))
        .filter(([, b]) => b);
    const set = (iniGet(ini, 'OpenCiv2', 'Language') || '').trim();
    const code = set && !/^auto$/i.test(set) ? (/^en\b/i.test(set) ? 'en' : match(set, list)) : 'en';
    const own = files.find(([c]) => c === code);
    return {files, codepage: own ? parseLang(own[1]).codepage : 'windows-1252'};
}

// The page's Language choice: English, the site's translations; the
// URL's ?lang= (kept by a reload or a bookmark), the page loaded again in it
async function languageChoice() {
    const list = await languages();
    const sel = $('language');
    $('language-row').hidden = !list.length;
    sel.textContent = '';
    const add = (value, label) => {
        const o = document.createElement('option');
        o.value = value;
        o.textContent = label;
        sel.append(o);
    };
    add('en', 'English');
    for (const l of list)
        add(l.code, l.name);
    sel.value = pageLang;
    sel.onchange = () => {
        history.replaceState(null, '', withLanguage(new URL(location.href), sel.value).href);
        location.reload();
    };
}

async function showSetup(store, ended = false) {
    $('loading').hidden = true;
    $('setup').hidden = false;
    const have = await store.count() > 0;
    $('have').hidden = !have;
    $('play').onclick = () => play(store);
    $('forget').onclick = async () => {
        if (confirm(t('web.forget_confirm'))) {
            await store.clear();
            await store.hdClear();
            showSetup(store);
        }
    };
    if (!ended)
        status('');
    window.civ2Setup = true;
}

async function main() {
    pageLang = await pageLanguage();
    await useLanguage(pageLang);
    await languageChoice();
    if (!window.WebAssembly || !window.indexedDB || !window.DecompressionStream) {
        status(t('web.no_browser'));
        return;
    }
    // (a later visit: what the game needs from the site, at once)
    const setup = new URLSearchParams(location.search).has('setup');
    if (!setup)
        fromSite();
    const store = await mark('store', openStore(() =>
        status(t('web.other_tab'))));
    savesControls(store);
    // (the relay's dialog keeps the keys from the game)
    const rd = $('relay-dialog');
    rd.addEventListener('keydown', (e) => e.stopPropagation());
    rd.addEventListener('keyup', (e) => e.stopPropagation());
    rd.addEventListener('close', () => $('screen').focus());
    window.civ2Relay = {address: relayAddress, unreachable: relayUnreachable};     // (the tests)
    const chosen = async (files) => {
        if (adding)
            return addChosen(files);
        try {
            if (await importFiles(store, files))
                await showSetup(store, true);
        } catch (e) {
            console.error(e);
            status(t('web.copy_failed', e.message));
        }
    };
    // the one button: a folder (the installed game or the CD) or files (a
    // .zip of the folder, or a disc image's .cue and .bin), from its menu
    const menu = $('choose-menu'), choose = $('choose');
    const showMenu = (on) => {
        menu.hidden = !on;
        choose.setAttribute('aria-expanded', on);
    };
    choose.onclick = (e) => {
        e.stopPropagation();
        showMenu(menu.hidden);
        if (!menu.hidden)
            $('choose-folder').focus();
    };
    document.addEventListener('click', () => showMenu(false));
    menu.onkeydown = (e) => {
        if (e.key === 'Escape') {
            showMenu(false);
            choose.focus();
        }
    };
    $('choose-folder').onclick = () => $('folder').click();
    $('choose-files').onclick = () => $('files').click();
    $('folder').onchange = (e) => {
        const files = [...e.target.files].map((f) => ({name: f.webkitRelativePath || f.name, file: f}));
        e.target.value = '';
        if (files.length) {
            showChosen(files);
            chosen(files);
        }
    };
    $('files').onchange = async (e) => {
        const files = [...e.target.files];
        e.target.value = '';
        if (files.length)
            chosenFiles(files);
    };
    // files (not a folder): a .zip alone, or a disc image's .cue and .bin
    const chosenFiles = async (files) => {
        const zip = files.length === 1 && /\.zip$/i.test(files[0].name) ? files[0] : null;
        showChosen(files.map((f) => ({name: f.name, file: f})));
        try {
            chosen(zip ? await zipFiles(zip) : files.map((f) => ({name: f.name, file: f})));
        } catch (e) {
            status(t('web.read_failed', zip ? zip.name : t('web.the_files'), e.message));
        }
    };
    const drop = $('drop');
    drop.ondragover = (e) => {
        e.preventDefault();
        drop.classList.add('over');
    };
    drop.ondragleave = () => drop.classList.remove('over');
    drop.ondrop = async (e) => {
        e.preventDefault();
        drop.classList.remove('over');
        const items = [...e.dataTransfer.items].map((i) => i.webkitGetAsEntry && i.webkitGetAsEntry()).filter(Boolean);
        if (items.every((it) => it.isFile))
            return chosenFiles([...e.dataTransfer.files]);
        const files = [];
        for (const it of items)
            await entryFiles(it, '', files);
        showChosen(files);
        chosen(files);
    };
    // later visits: straight into the game (?setup: this page first)
    if (!setup && await store.count() > 0) {
        // (the site's music and animated advisors this browser does not
        // have yet: kept now, not read from the site while the game plays)
        const g = await siteGame();
        if (g)
            await installFromSite(store, g, false).catch((e) => console.error(e));
        return play(store);
    }
    // a first visit to a site with the game (make web GAME=1): its copy
    // kept, then the game
    const g = setup ? null : await siteGame();
    if (g) {
        try {
            await installFromSite(store, g, true);
            return play(store);
        } catch (e) {
            console.error(e);
            $('install').hidden = true;
            status(t('web.copy_failed', e.message));
        }
    }
    showSetup(store);
}

main();
