// The page: the player's game files (chosen once, kept in IndexedDB), then
// the game (host.js) on the canvas.

import {MemFS} from './wasi.js';
import {runGame} from './host.js';
import {openStore} from './store.js';
import {unzip} from './zip.js';
import {exportSaves, importSaves} from './saves.js';

const GAME = '/civ2', DATA = '/data';
const $ = (id) => document.getElementById(id);
const status = (s) => {
    $('status').textContent = s;
    window.civ2Status = s;
};

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
        for (const names of dirs.values())
            if (names.has('data1.cab'))
                return 'This is the game\'s CD. The browser version needs the game installed: install it ' +
                       'from the CD (or set it up with the desktop version of OpenCiv2) and choose the folder ' +
                       'it is installed in.';
        return 'These files are not an installed Civilization II Multiplayer Gold Edition: its folder has ' +
               'Civ2Art.dll and RULES.TXT in it.';
    }
    const pre = root ? root + '/' : '';
    return {files: files.filter((f) => f.name.startsWith(pre) && !/\.log$/i.test(f.name))
                        .map((f) => ({...f, name: f.name.slice(pre.length)}))};
}

async function importFiles(store, files) {
    const found = findGame(files);
    if (typeof found === 'string') {
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
            status(`Copying the game: ${n} of ${found.files.length} files`);
            await store.putMany(batch);
            batch = [];
            size = 0;
        }
    }
    await store.putMany(batch);
    await store.setMeta('game', {names: found.files.map((f) => f.name), at: Date.now()});
    status(`The game's ${n} files are kept in this browser.`);
    return true;
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

async function zipFiles(file) {
    status('Reading ' + file.name + ' ...');
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
            savesStatus('There are no saved games to export yet.');
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
        savesStatus(`Exported ${names.length} file${names.length > 1 ? 's' : ''}: ${names.join(', ')}.`);
    } catch (e) {
        console.error(e);
        savesStatus('Could not export the saved games: ' + e.message);
    }
}

async function importChosen(store, list) {
    try {
        const files = [];
        for (const f of list)
            files.push({name: f.name, data: new Uint8Array(await f.arrayBuffer())});
        const names = await importSaves(store, files, running, GAME);
        savesStatus(names.length ? `Imported ${names.length} file${names.length > 1 ? 's' : ''}: ${names.join(', ')}.`
                                 : 'Nothing to import there: saved games are .SAV files (or a .zip of them).');
    } catch (e) {
        console.error(e);
        savesStatus('Could not import: ' + e.message);
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

async function play(store) {
    // (the game files and saved games kept for good: without this the browser
    // may clear them when disk space runs low, Safari after a week unused)
    if (navigator.storage && navigator.storage.persist)
        navigator.storage.persist().catch(() => {});
    $('setup').hidden = true;
    $('game').hidden = false;
    status('Loading ...');
    const t0 = performance.now();
    const fs = new MemFS();
    const [entries, fonts, fontsAa] = await Promise.all([store.all(), fetchGz('data/fonts_win.bin.gz'),
                                                         fetchGz('data/fonts_win_aa.bin.gz')]);
    for (const [k, v] of entries)
        fs.put(GAME + '/' + k, v.data, v.mtime);
    fs.mkdirs(GAME);
    fs.put(DATA + '/fonts_win.bin', fonts);
    fs.put(DATA + '/fonts_win_aa.bin', fontsAa);
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
    status(`Starting (files loaded in ${Math.round(performance.now() - t0)} ms) ...`);
    const canvas = $('screen');
    canvas.focus();
    // (the platform's log: ?log, in the console)
    const env = {CIV2_DATA: DATA};
    if (!new URLSearchParams(location.search).has('log'))
        env.CIV2_HOOK_LOG = '/dev/null';
    // (tests, src/web/test/smoke.mjs: files and settings of a replay)
    if (window.civ2Test)
        await window.civ2Test(fs, env);
    let code;
    try {
        window.civ2Stats = {};
        code = await runGame({wasm: fetch('civ2.wasm'), fs, canvas, args: ['civ2', GAME], env, messageBox,
                              stats: window.civ2Stats});
        status('The game has ended' + (code ? ' (' + code + ')' : '') + '.');
    } catch (e) {
        console.error(e);
        status('The game stopped: ' + e.message);
    }
    await window.civ2Flush();
    running = null;
    $('game').hidden = true;
    showSetup(store, true);
}

async function showSetup(store, ended = false) {
    $('setup').hidden = false;
    const have = await store.count() > 0;
    $('have').hidden = !have;
    $('play').onclick = () => play(store);
    $('forget').onclick = async () => {
        if (confirm('Remove the game files, saved games included, from this browser?')) {
            await store.clear();
            showSetup(store);
        }
    };
    if (!ended)
        status(have ? 'Ready.' : 'Choose your installed Civilization II.');
}

async function main() {
    if (!window.WebAssembly || !window.indexedDB || !window.DecompressionStream) {
        status('This browser lacks what the game needs (WebAssembly, IndexedDB, DecompressionStream).');
        return;
    }
    const store = await openStore();
    savesControls(store);
    const chosen = async (files) => {
        try {
            if (await importFiles(store, files))
                await showSetup(store, true);
        } catch (e) {
            console.error(e);
            status('Could not copy the files: ' + e.message);
        }
    };
    $('folder').onchange = (e) => chosen([...e.target.files].map((f) => ({name: f.webkitRelativePath || f.name, file: f})));
    $('zip').onchange = async (e) => e.target.files[0] && chosen(await zipFiles(e.target.files[0]));
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
        const zip = [...e.dataTransfer.files].find((f) => /\.zip$/i.test(f.name));
        if (zip && items.length === 1 && items[0].isFile)
            return chosen(await zipFiles(zip));
        const files = [];
        for (const it of items)
            await entryFiles(it, '', files);
        chosen(files);
    };
    // later visits: straight into the game (?setup: this page first)
    if (await store.count() > 0 && !new URLSearchParams(location.search).has('setup'))
        return play(store);
    showSetup(store);
}

main();
