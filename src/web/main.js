// The page: its start screen (the click that lets the browser play the
// game's sound; the saved games exported and imported), then the game
// (host.js) on the canvas; the player's copy of the original, chosen over
// the game from its Settings, kept in IndexedDB.

import {MemFS} from './wasi.js';
import {runGame, smallScreen} from './host.js';
import {openStore} from './store.js';
import {unzip} from './zip.js';
import {exportSaves, importSaves} from './saves.js';
import {findDisc} from './setup_run.js';
import {hdWanted, hasOriginalPictures, iniGet, iniSet} from './options.js';
import {t, languages, pageLanguage, urlLanguage, withLanguage, useLanguage, langFile, parseLang, match} from './i18n.js';

const GAME = '/civ2', DATA = '/data';
let pageLang = 'en';        // the page's language (i18n.js: ?lang=CODE, else English)
const $ = (id) => document.getElementById(id);
// (on the start screen, its panel choosing the original's files, and over
// the screen until the game's first frame)
const status = (s) => {
    $('status').textContent = s;
    $('add-status').textContent = s;
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
    let found = findGame(files);
    // (a game of OpenCiv2's own files only, played already: the copy added
    // to it, the player's saved games and settings kept, as Settings'
    // Classic graphics adds it: ownGame)
    const own = await ownGame(store) && await store.count() > 0;
    if (typeof found === 'string') {
        const disc = findDisc(files);
        if (disc && own) {
            const made = [];
            if (!await setupFromDisc(store, files, disc, made))
                return false;
            found = {files: made};
        } else if (disc) {
            return setupFromDisc(store, files, disc);
        } else {
            status(found);
            return false;
        }
    }
    if (own) {
        const n = await mergeFiles(store, null, found.files);
        status(t('web.added', n));
        return true;
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

// While the page's panel is shown for them over a running game
// (host_add_original: addOriginal): {store, fs, resolve}; the files chosen
// go to addChosen then
let adding = null;

// A game of OpenCiv2's own files, without a copy of the original (the
// store's meta 'game' {names: []}: play, a start without one), or an
// empty store
async function ownGame(store) {
    const meta = await store.getMeta('game');
    return meta ? !(meta.names || []).length : await store.count() === 0;
}

// The files of the player's copy ([{name, data|file}], under the game's
// folder) the game lacks (any case), put into the running game's files (fs;
// null: not running) and kept in the store, -> how many
async function mergeFiles(store, fs, files) {
    const have = new Set(fs ? [...fs.files.keys()].filter((k) => k.startsWith(GAME + '/'))
        .map((k) => k.slice(GAME.length + 1).toUpperCase()) : (await store.keys()).map((k) => k.toUpperCase()));
    const added = [];
    let batch = [], size = 0;
    for (const f of files) {
        if (have.has(f.name.toUpperCase()) || /\.log$/i.test(f.name))
            continue;
        const data = f.data || new Uint8Array(await f.file.arrayBuffer());
        if (fs)
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
    await store.setMeta('game', meta && meta.names ? {...meta, names: [...meta.names, ...added]}
        : {names: added, at: Date.now()});
    return added.length;
}

// The player's choice on the panel while adding: an installed game's
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

// The front (the backdrop) with one of its panels: 'start' (the start
// screen), 'add' (the original's files chosen), 'install' (a progress bar);
// null: none, the front hidden
function front(panel) {
    $('front').hidden = !panel;
    for (const id of ['start', 'add', 'install'])
        $(id).hidden = id !== panel;
    if (panel)
        $('loading').hidden = true;
}

// host_add_original (the game's Settings, Classic graphics ticked
// without the original's files: Find the original files...): the panel
// choosing them shown over the game, the game's own files kept, -> 1 the
// game's files have the original's pictures now, 0 not (Back to the game)
function addOriginal(store, fs) {
    return new Promise((resolve) => {
        adding = {store, fs, resolve};
        $('game').hidden = true;
        front('add');
        $('chosen').hidden = true;
        $('add-back').onclick = () => endAdding(0);
        status('');
        $('choose').focus();
        window.civ2Adding = true;       // (the tests)
    });
}

function endAdding(r) {
    const a = adding;
    adding = null;
    window.civ2Adding = false;
    front(null);
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

// The start screen's Export saves: a .zip of them downloaded
async function exportClicked(store) {
    try {
        if (running)
            await window.civ2Flush();
        const {data, names} = await exportSaves(store);
        if (!names.length) {
            status(t('web.no_saves'));
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
        status(names.length > 1 ? t('web.exported', names.length, names.join(', ')) : t('web.exported_one', names[0]));
    } catch (e) {
        console.error(e);
        status(t('web.export_failed', e.message));
    }
}

// Its Import saves: the files chosen put into the game's folder -> how many
async function importChosen(store, list) {
    try {
        const files = [];
        for (const f of list)
            files.push({name: f.name, data: new Uint8Array(await f.arrayBuffer())});
        const names = await importSaves(store, files, running, GAME);
        window.civ2Imported = names;    // (the tests)
        status(names.length > 1 ? t('web.imported', names.length, names.join(', '))
            : names.length ? t('web.imported_one', names[0]) : t('web.nothing_to_import'));
        return names.length;
    } catch (e) {
        console.error(e);
        status(t('web.import_failed', e.message));
        return 0;
    }
}

// The sound, started at the start screen's click (or key): the browser
// lets a page's audio play only from a user's gesture, so the context is
// made and resumed in its handler, and the game plays on it (host.js: the
// opening menu's music heard from the start)
let audioCtx = null;
function startAudio() {
    try {
        audioCtx = audioCtx || new AudioContext();
        audioCtx.resume().catch(() => {});
    } catch (e) {
        audioCtx = null;
    }
}

// A phone or tablet (smallScreen: a touch screen under 800x600 CSS px):
// the game full screen from the start screen's click, where the browser
// can (not Safari on an iPhone), a phone's then turned sideways, where the
// browser lets it (Chrome on Android): the game's interface drawn larger
// (host.js: smallDesktop); back with the system's Back or Escape
function fullScreen() {
    const el = document.documentElement;
    if (!smallScreen() || !el.requestFullscreen || document.fullscreenElement)
        return;
    try {
        el.requestFullscreen({navigationUI: 'hide'}).then(() => {
            if (Math.min(screen.width, screen.height) < 600 && screen.orientation && screen.orientation.lock)
                return screen.orientation.lock('landscape');
        }).catch(() => {});
    } catch (e) { /* not here */ }
}

function savesControls(store) {
    $('export-saves').onclick = () => exportClicked(store);
    // (the click that opens the file chooser starts the sound: the saves
    // imported go straight into the game)
    const label = $('import-label');
    label.addEventListener('click', startAudio);
    label.onkeydown = (e) => {
        if (e.key === 'Enter' || e.key === ' ') {
            e.preventDefault();
            startAudio();
            $('import-saves').click();
        }
    };
    $('import-saves').onchange = async (e) => {
        const list = [...e.target.files];
        e.target.value = '';
        if (list.length && await importChosen(store, list) && !running)
            play(store);
    };
}

// ---- the HD graphics' art, as CIV.INI asks for it (options.js) ----

// The art (data/native/hd: hd/pictures.txt and its pictures), OpenCiv2's
// own heralds (data/native/heralds: heralds/...), historical leaders'
// portraits (data/native/portraits: portraits/leader-ID.webp) and HD pictures
// (data/native/hd-extra: hd-extra/...), on the site as one file, hd/art.bin,
// its files one after another as hd/index.json lists them ([path, size]
// each) -> [[path, bytes]], put where hd.c looks for them (CIV2_DATA/hd,
// CIV2_DATA/heralds, CIV2_DATA/portraits, CIV2_DATA/hd-extra)
// (onProgress(done, total): its bytes as they come)
async function fetchArt(onProgress = null) {
    const index = await fetch('hd/index.json').then((r) => {
        if (!r.ok)
            throw new Error('hd/index.json: ' + r.status);
        return r.json();
    });
    const r = await fetch('hd/art.bin');
    if (!r.ok)
        throw new Error('hd/art.bin: ' + r.status);
    let bytes;
    if (onProgress && r.body) {
        const total = index.reduce((n, [, size]) => n + size, 0), parts = [];
        let got = 0;
        for (const reader = r.body.getReader();;) {
            const {done, value} = await reader.read();
            if (done)
                break;
            parts.push(value);
            got += value.length;
            onProgress(got, total);
        }
        bytes = new Uint8Array(await new Blob(parts).arrayBuffer());
    } else {
        bytes = new Uint8Array(await r.arrayBuffer());
    }
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

// ---- what a start waits for, fetched while the start screen is shown ----

// The site's files a start needs, each fetched once (url -> its promise; a
// failure forgotten, to be tried again), so that the start screen's preload
// and the start share them: the click starts the game at once when they
// have all come.  (counted: one of the files the start waits for, in loads)
const fetched = new Map();
function once(url, get, counted = false) {
    let p = fetched.get(url);
    if (!p) {
        if (counted) {
            loads.files.set(url, false);
            loadsMoved();
        }
        p = get(url).then((v) => {
            if (counted) {
                loads.files.set(url, true);
                loads.bytes += v ? v.byteLength || 0 : 0;
                loadsMoved();
            }
            return v;
        });
        fetched.set(url, p);
        p.catch(() => {
            fetched.delete(url);
            loads.files.delete(url);
            loadsMoved();
        });
    }
    return p;
}
const siteJson = (url) => once(url, (u) => fetch(u).then((r) => (r.ok ? r.json() : [])));
const siteGz = (url) => once(url, fetchGz, true);
const siteBytes = (url, counted = true) => once(url, (u) => fetch(u).then((r) => {
    if (!r.ok)
        throw new Error(u.split('/').pop() + ': ' + r.status);
    return r.arrayBuffer();
}).then((b) => new Uint8Array(b)), counted);
// the HD graphics' art (fetchArt), its bytes counted as they come
const siteArt = () => once('hd/art.bin', () => mark('hd', fetchArt((done, total) => {
    loads.bytes += done - loads.artDone;
    loads.artDone = done;
    loads.artTotal = total;
    loadsMoved();
})).catch((e) => {
    loads.bytes -= loads.artDone;
    loads.artDone = loads.artTotal = 0;
    throw e;
}));

// Those downloads as they come (the start screen's line, a first start's
// bar): the art's bytes, the other files the start waits for (url -> come)
const loads = {artDone: 0, artTotal: 0, files: new Map(), bytes: 0, art: false, on: new Set()};
function loadsMoved() {
    for (const f of loads.on)
        f();
}
// (how far, 0 to 1: the art most of it where it is wanted)
function loadsFrac() {
    const files = loads.files.size ? [...loads.files.values()].filter(Boolean).length / loads.files.size : 0;
    return loads.art ? 0.9 * (loads.artTotal ? loads.artDone / loads.artTotal : 0) + 0.1 * files : files;
}

// The game's files kept in this browser (the store's), read once until it
// changes (preloadStore)
let keptP = null;
const keptFiles = (store) => keptP || (keptP = mark('files', store.all()));
const keptChanged = () => {
    keptP = null;
};
// (the store's writes: what was read of it is read again)
function preloadStore(store) {
    for (const k of ['putMany', 'remove', 'clear']) {
        const f = store[k];
        store[k] = (...a) => {
            keptChanged();
            return f.apply(store, a);
        };
    }
    return store;
}

// What a start fetches from the site for a game whose kept files are keys
// (their names): the HD graphics' art where hd, OpenCiv2's texts and sound
// effects the game lacks (with HD graphics all the sound effects), the
// font files -> {art, texts, sounds, fonts}: promises of [[name, bytes]]
// (art: null where it could not be fetched; the others fail), as play puts
// them in the game's files
function siteFiles(keys, hd) {
    const have = new Set(keys.map((k) => k.toUpperCase()));
    loads.art = loads.art || hd;
    const all = (names, get) => Promise.all(names.map((n) => get(n).then((b) => [n, b])));
    const art = hd ? siteArt().catch((e) => {
        console.error(e);
        return null;
    }) : Promise.resolve(null);
    const texts = siteJson('data/text/index.json')
        .then((names) => all(names.filter((n) => !have.has(n)), (n) => siteGz('data/text/' + n + '.gz')));
    const sounds = siteJson('data/sound/index.json').then((names) => all(
        hd ? names : names.filter((n) => !have.has('SOUND/' + n.slice(0, -4).toUpperCase() + '.WAV')),
        (n) => siteBytes('data/sound/' + n)));
    const fonts = siteJson('data/fonts/index.json').then((names) => all(names, (n) => siteGz('data/fonts/' + n + '.gz')));
    for (const p of [texts, sounds, fonts])
        p.catch(() => {});
    return {art, texts, sounds, fonts};
}

// OpenCiv2's own music and wonder movies the game will have (hd: all of
// them; else those its files lack: play), fetched not waited for ->
// {music: promise of [[name, bytes]], wonders: of [[NN, webp, ogg]]}
// (names: the game's files' names, with GAME/)
function siteExtras(names, hd) {
    const have = new Set(names.map((k) => k.toUpperCase()));
    const music = siteJson('data/music/index.json').then((list) => Promise.all(list.filter((n) => {
        const track = (GAME + '/MUSIC/' + n.slice(0, -4)).toUpperCase();
        return hd || (!have.has(track + '.OGG') && !have.has(track + '.WAV'));
    }).map((n) => siteBytes('data/music/' + n, false).then((b) => [n, b]))));
    const wonders = siteJson('data/wonders/index.json').then((list) => list
        .filter((nn) => hd || !have.has((GAME + '/VIDEO/WONDER' + nn + '.AVI').toUpperCase()))
        .map((nn) => Promise.all([siteBytes('data/wonders/wonder' + nn + '.webp', false),
                                  siteBytes('data/wonders/wonder' + nn + '.ogg', false)]).then(([webp, ogg]) => [nn, webp, ogg])));
    music.catch(() => {});
    wonders.catch(() => {});
    return {music, wonders};
}

// What a start would do first, done while the start screen is shown: the
// kept files read, what the game needs from the site fetched (siteFiles,
// the translations), then its music and wonder movies; its progress on the
// start screen, under Click to play, until all a start waits for has come
// (window.civ2Preload: {ready, readyMs}, the tests).  Nothing played, nothing
// written: a start then reads and fetches only what changed since (saves
// imported, files removed).
let preloading = null;
function preload(store) {
    const run = preloading = {};
    const shown = () => {
        if (run !== preloading)
            return;
        const pct = Math.floor(loadsFrac() * 100);
        $('preload-fill').style.width = pct + '%';
        $('preload-pct').textContent = t('web.loading') + ' ' + pct + '%';
        $('preload').setAttribute('aria-valuenow', pct);
    };
    loads.on.add(shown);
    window.civ2Preload = {ready: false};
    $('preload').hidden = false;
    shown();
    (async () => {
        const site = fromSite();
        const keys = await store.keys();
        const iniName = keys.find((k) => /^civ\.ini$/i.test(k));
        const ini = iniOf(iniName ? [[iniName, await store.get(iniName)]] : []).text;
        const hd = hdWanted(ini, new URLSearchParams(location.search).get('hd'), hasOriginalPictures(keys)) >= 0;
        const files = siteFiles(keys, hd);
        shown();
        await Promise.all([site.wasm, site.fonts, keptFiles(store), files.art, files.texts, files.sounds, files.fonts,
                           gameLanguage(ini)]);
        window.civ2Preload = {ready: true, readyMs: Math.round(performance.now())};
        siteExtras(keys.map((k) => GAME + '/' + k), hd);
    })().catch((e) => console.warn('preload:', e)).finally(() => {
        loads.on.delete(shown);
        if (run === preloading)
            $('preload').hidden = true;
    });
}

// ---- the game from the site itself (make web GAME=1: game/index.json) ----

// The site's copy of the game, or null: {core: {size, files}, files: [[path, size]]}
let siteGameP = null;
const siteGame = () => siteGameP || (siteGameP = fetch('game/index.json', {cache: 'no-cache'})
    .then((r) => (r.ok ? r.json() : null)).catch(() => null));

// A first visit to such a site: its copy kept in this browser, without
// asking (before the start screen, whose panel offers another copy), on one
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
    front('install');
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

// A first start without a copy of the original (nothing in the store: the
// game from OpenCiv2's own files, HD graphics), before what it waits for has
// all come (preload): those downloads on the install's progress bar (loads:
// the art by its bytes, most of it, OpenCiv2's texts, sounds and fonts by
// their files) -> {end()}; window.civ2FirstStart (tests): the bar's last
// words and how many times it moved
function firstStartBar() {
    front('install');
    $('install-title').textContent = t('web.starting_title');
    $('install').querySelector('[data-t="web.install_note"]').textContent = t('web.start_note');
    let moves = 0, frac = 0;
    const shown = () => {
        // (never back: more files to come as each kind's list is read)
        frac = Math.max(frac, loadsFrac());
        installProgress(t('web.start_download', MB1(loads.bytes)), frac);
        window.civ2FirstStart = {step: $('install-step').textContent, pct: $('install-pct').textContent, moves: ++moves};
    };
    loads.on.add(shown);
    shown();
    return {
        end() {
            loads.on.delete(shown);
            front(null);
        },
    };
}

async function play(store) {
    // (the game files and saved games kept for good: without this the browser
    // may clear them when disk space runs low, Safari after a week unused)
    if (navigator.storage && navigator.storage.persist)
        navigator.storage.persist().catch(() => {});
    front(null);
    window.civ2Start = false;
    const t0 = performance.now();
    const site = fromSite();
    const fs = new MemFS();
    // (the start screen's preload: what it fetched and read is used, what
    // it has not yet is waited for here; its line no longer shown)
    preloading = null;
    const ready = window.civ2Preload && window.civ2Preload.ready;
    // CIV.INI first (a moment): the HD graphics' art, when wanted, read
    // alongside the other files
    const query = new URLSearchParams(location.search), args = ['civ2', GAME];
    const keys = await store.keys();
    // nothing kept (a first visit; the files removed): the game from an
    // empty folder, OpenCiv2's own files filling it in (the desktop game's
    // start without the original's: docs/NO-INSTALL.md), noted as such
    // (meta 'game': no names), its progress on the bar (unless all it
    // waits for has come already)
    const bar = keys.length || ready ? null : firstStartBar();
    if (!keys.length && !await store.getMeta('game'))
        await store.setMeta('game', {names: [], at: Date.now()});
    if (!bar) {
        $('game').hidden = false;
        $('loading').hidden = false;
    }
    status(t('web.loading_files'));
    const iniName = keys.find((k) => /^civ\.ini$/i.test(k));
    const ini = iniOf(iniName ? [[iniName, await store.get(iniName)]] : []).text;
    // (always where the game's files lack the original's pictures: the game
    // makes them from the art, docs/NO-INSTALL.md)
    const hd = hdWanted(ini, query.get('hd'), hasOriginalPictures(keys)) >= 0;
    const files = siteFiles(keys, hd);
    // (the HD art of former versions, downloaded and kept in this browser:
    // the art is the site's now, its room freed)
    store.hdClear().catch(() => {});
    const [entries, [fonts, fontsAa]] = await Promise.all([keptFiles(store), site.fonts]);
    // (read again at a next start: the game writes into these files)
    keptChanged();
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
        for (const [n, b] of await files.texts)
            fs.put(DATA + '/text/' + n, b);
    } catch (e) {
        console.warn('OpenCiv2 texts:', e);
    }
    // OpenCiv2's own sound effects (data/sound, some 2 MB): with HD graphics
    // all of them (sound.c plays data/sound/name.ogg first, the game's
    // Sound/NAME.WAV only for one it lacks), with Classic graphics those the
    // player's game lacks (it plays the game's Sound/NAME.WAV, else ours);
    // window.civ2OwnSounds: those put in (tests)
    try {
        const sounds = await files.sounds;
        for (const [n, b] of sounds)
            fs.put(DATA + '/sound/' + n, b);
        window.civ2OwnSounds = sounds.map(([n]) => n);
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
        for (const [n, b] of await files.fonts)
            fs.put(DATA + '/fonts/' + n, b);
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
        const art = await files.art;
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
    // (not the pictures and sound effects the game makes from the HD art and
    // OpenCiv2's own sounds where its files lack the original's,
    // OpenCiv2Pictures/ and OpenCiv2Sounds/: made again at each start,
    // synth.c, sound.c)
    fs.onchange = (path, removed, partial) => {
        if (!path.startsWith(GAME + '/') || /\.log$/i.test(path) || /^\/civ2\/OpenCiv2(Pictures|Sounds)\//i.test(path))
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
    if (bar) {
        bar.end();
        $('game').hidden = false;
        $('loading').hidden = false;
    }
    status(t('web.starting', Math.round(performance.now() - t0)));
    const canvas = $('screen');
    canvas.focus();
    // (the platform's log: ?log, in the console)
    // (Auto, the system's language: English)
    const env = {CIV2_DATA: DATA, CIV2_SYSTEM_LANG: 'en'};
    if (!new URLSearchParams(location.search).has('log'))
        env.CIV2_HOOK_LOG = '/dev/null';
    // (tests, src/web/test/smoke.mjs, web-parity.mjs: files and settings of
    // a replay; args: the game's command line, its folder args[1])
    if (window.civ2Test)
        await window.civ2Test(fs, env, args);
    // OpenCiv2's own music (data/music, some 26 MB): with HD graphics all
    // its tracks (sound.c plays data/music/trackNN.ogg as track NN first,
    // the game's Music/TrackNN only for a track it lacks), with Classic
    // graphics those the player's game lacks (it plays the game's
    // Music/TrackNN.ogg, else .wav, else ours); fetched while the game starts
    // and plays (or already at the start screen: preload), not waited for; put in all at once when all have come, so
    // that the game, which counts the tracks when it opens the CD drive,
    // finds all of them or none (until then it plays the game's own, or as
    // without music); window.civ2OwnMusic: the tracks put in (tests)
    const extras = siteExtras([...fs.files.keys()], hd);
    extras.music.then((music) => {
        for (const [n, b] of music)
            fs.put(DATA + '/music/' + n, b);
        window.civ2OwnMusic = music.map(([n]) => n);
    }).catch((e) => console.warn('OpenCiv2 music:', e));
    // OpenCiv2's own wonder movies (data/wonders, some 3 MB each: the
    // picture wonderNN.webp and its sound wonderNN.ogg): with HD graphics
    // all of them (hd.c plays ours first, in the place of the game's
    // Video/WONDERNN.AVI), with Classic graphics those the player's game
    // lacks (the game's own, else ours); fetched while the game starts and
    // plays (or at the start screen), not waited for; each movie put in when both its files have
    // come, the sound first (the game looks for the picture: until it is
    // there it plays the game's own, or none); window.civ2OwnWonders: the
    // movies put in, when all have come (tests)
    extras.wonders.then(async (movies) => {
        const put = [];
        await Promise.all(movies.map((m) => m.then(([nn, webp, ogg]) => {
            fs.put(DATA + '/wonders/wonder' + nn + '.ogg', ogg);
            fs.put(DATA + '/wonders/wonder' + nn + '.webp', webp);
            put.push(nn);
        })));
        window.civ2OwnWonders = put.sort();
    }).catch((e) => console.warn('OpenCiv2 wonder movies:', e));
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
                              audio: audioCtx,
                              // (the game's Settings' restart: the page loaded again, into the
                              // game, without what would override them, not stopping at the
                              // start screen; ?lang= the game's language now, where the game
                              // changed it)
                              restart: async () => {
                                  await window.civ2Flush();
                                  try {
                                      sessionStorage.setItem('civ2.restart', '1');
                                  } catch (e) { /* the start screen again */ }
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
    showStart(store, true);
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

// The start screen: Click to play (Enter or Space too), the saves exported
// and imported, and what this browser keeps removed (the game's own files,
// a copy of the original, the saves and settings)
async function showStart(store, ended = false) {
    front('start');
    $('more-row').hidden = !(await store.count() > 0);
    const go = () => {
        if ($('front').hidden || $('start').hidden || $('play').disabled)
            return;
        $('play').disabled = true;
        startAudio();
        fullScreen();
        play(store);
    };
    $('play').onclick = go;
    // (Enter or Space anywhere on it but its other controls)
    document.onkeydown = (e) => {
        if ((e.key === 'Enter' || e.key === ' ') && !$('front').hidden && !$('start').hidden &&
            (e.target.id === 'play' || !e.target.closest('button, label, select, input, a'))) {
            e.preventDefault();
            go();
        }
    };
    $('play').disabled = false;
    $('forget').onclick = async () => {
        if (confirm(t('web.forget_confirm'))) {
            await store.clear();
            await store.hdClear();
            showStart(store);
        }
    };
    if (!ended)
        status('');
    $('play').focus();
    window.civ2Start = true;    // (the tests)
    // (what a start waits for, fetched now: the click then starts the game at once)
    preload(store);
}

async function main() {
    pageLang = await pageLanguage();
    await useLanguage(pageLang);
    if (!window.WebAssembly || !window.indexedDB || !window.DecompressionStream) {
        front('start');
        $('play').disabled = true;
        $('saves-row').hidden = true;
        $('more-row').hidden = true;
        status(t('web.no_browser'));
        return;
    }
    // (what the game needs from the site, at once, while the start screen waits)
    fromSite();
    const store = preloadStore(await mark('store', openStore(() =>
        status(t('web.other_tab')))));
    // (without ?lang=: the page in the game's language, as its Settings
    // chose it, CIV.INI's Language=; the page has no choice of its own)
    if (!urlLanguage()) {
        const name = (await store.keys()).find((k) => /^civ\.ini$/i.test(k));
        const set = name ? iniLanguage(((await store.get(name)) || {}).data) : '';
        const code = set && !/^(auto$|en\b)/i.test(set) ? match(set, await languages()) : 'en';
        if (code && code !== pageLang) {
            pageLang = code;
            await useLanguage(code);
        }
    }
    savesControls(store);
    // (the relay's dialog keeps the keys from the game)
    const rd = $('relay-dialog');
    rd.addEventListener('keydown', (e) => e.stopPropagation());
    rd.addEventListener('keyup', (e) => e.stopPropagation());
    rd.addEventListener('close', () => $('screen').focus());
    window.civ2Relay = {address: relayAddress, unreachable: relayUnreachable};     // (the tests)
    // (the panel is shown only while adding: addOriginal; its inputs given
    // files before a start, as the tests give a copy of the game, smoke.mjs,
    // hd.mjs, player.mjs: the copy kept, as the site's own copy of the game
    // is, importFiles, then the start screen again)
    const chosen = async (files) => {
        if (adding)
            return addChosen(files);
        try {
            if (await importFiles(store, files))
                await showStart(store, true);
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
    const kept = await store.count() > 0, g = await siteGame();
    // a later visit to a site with the game (make web GAME=1): the site's
    // music and animated advisors this browser does not have yet kept now,
    // not read from the site while the game plays
    if (g && kept)
        await installFromSite(store, g, false).catch((e) => console.error(e));
    // a first visit to it: its copy kept, then the game
    if (g && !kept) {
        try {
            await installFromSite(store, g, true);
        } catch (e) {
            console.error(e);
            status(t('web.copy_failed', e.message));
            return showStart(store, true);
        }
    }
    // the game's Settings' restart: straight into the game again (its
    // sound, without a click now, heard from the next one: host.js)
    let restart = false;
    try {
        restart = sessionStorage.getItem('civ2.restart') === '1';
        sessionStorage.removeItem('civ2.restart');
    } catch (e) { /* the start screen */ }
    if (restart)
        return play(store);
    // (else the start screen; then, at a first visit, the game from
    // OpenCiv2's own files, HD graphics, the original's files added later
    // for Classic graphics; at a later visit, what was kept)
    return showStart(store);
}

main();
