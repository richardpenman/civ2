// The page: the player's game files (chosen once, kept in IndexedDB), then
// the game (host.js) on the canvas.

import {MemFS} from './wasi.js';
import {runGame} from './host.js';
import {openStore} from './store.js';
import {unzip} from './zip.js';
import {exportSaves, importSaves} from './saves.js';
import {findDisc} from './setup_run.js';
import {packFrom, keepPack, loadPack, download, siteArt} from './hdpack.js';
import {readChoices, writeChoices, hdWanted} from './options.js';

const GAME = '/civ2', DATA = '/data';
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
        return 'These files are neither an installed Civilization II Multiplayer Gold Edition (its folder has ' +
               'Civ2Art.dll and RULES.TXT in it), nor its CD (DATA1.CAB and CIV2), nor a disc image of the CD ' +
               '(a .cue with its .bin).';
    }
    const pre = root ? root + '/' : '';
    return {files: files.filter((f) => f.name.startsWith(pre) && !/\.log$/i.test(f.name))
                        .map((f) => ({...f, name: f.name.slice(pre.length)}))};
}

async function importFiles(store, files) {
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

// The game set up from its CD or a disc image (disc: findDisc's), as the
// desktop version's set-up does (setup_worker.js runs it): into the store
// as its files come.
function setupFromDisc(store, files, disc) {
    return new Promise((resolve) => {
        const worker = new Worker('setup_worker.js', {type: 'module'});
        const stop = $('stop'), names = new Set(), messages = [];
        let queue = store.clear(), batch = [], size = 0, ended = false;
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
            flush();
            queue.then(async () => {
                if (ok && typeof findGame([...names].map((name) => ({name}))) !== 'string') {
                    await store.setMeta('game', {names: [...names], at: Date.now()});
                    status(text);
                    resolve(true);
                } else {
                    await store.clear();
                    status(ok ? 'The set-up did not make the game (no Civ2Art.dll, RULES.TXT).' : text);
                    resolve(false);
                }
            }, (e) => {
                status('Could not keep the files: ' + e.message);
                resolve(false);
            });
        };
        worker.onmessage = (e) => {
            const m = e.data;
            if (m.file !== undefined) {
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
                end(m.done === 0, m.done === 0 ? `The game is set up from the ${disc.kind === 'cd' ? 'CD' : 'disc image'}: ` +
                    `${names.size} files kept in this browser` + (disc.kind === 'cd' ? ' (without the music: the ' +
                    'browser cannot read the CD\'s music tracks; a disc image of the CD has them).' : '.')
                    : 'The set-up failed: ' + (messages.join(' ') || 'code ' + m.done));
            } else if (m.error) {
                end(false, 'The set-up failed: ' + m.error);
            }
        };
        worker.onerror = (e) => end(false, 'The set-up failed: ' + (e.message || 'its worker stopped'));
        stop.hidden = false;
        stop.onclick = () => end(false, 'The set-up was stopped; choose the files again to start it again.');
        status(`Setting the game up from its ${disc.kind === 'cd' ? 'CD' : 'disc image'} ...`);
        worker.postMessage({files: files.map((f) => ({name: f.name, file: f.file || new Blob([f.data])})),
                            source: disc.source});
    });
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
        p.textContent = `Folder: ${[...top].join(', ')} (${files.length} files)`;
    else if (files.some((f) => /\.cue$/i.test(f.name)))
        p.textContent = 'Disc image: ' + files.map((f) => f.name).join(' + ');
    else if (files.length === 1)
        p.textContent = files[0].name + (files[0].file ? ` (${Math.round(files[0].file.size / 1e6)} MB)` : '');
    else
        p.textContent = files.map((f) => f.name).join(', ');
    p.hidden = false;
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
    $('relay-text').textContent = `Network games go through a relay, and none answered at ${relay}: run ` +
        `tools/webrelay.py on ${host}, or give its address here (or open the page with ?relay=ws://HOST:PORT).`;
    const input = $('relay-address');
    input.value = relay;
    $('relay-use').onclick = () => {
        const v = input.value.trim();
        if (v && !/^wss?:\/\/./.test(v)) {
            $('relay-text').textContent = 'The relay\'s address starts with ws:// (or wss://).';
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

// ---- the opening window's choices (options.js), the HD graphics' art (hdpack.js) ----

let pinP = null, specP = null;
const pin = () => pinP || (pinP = fetch('hd-pack.json').then((r) => r.json()));
const choicesSpec = () => specP || (specP = fetch('choices.json').then((r) => r.json()));
const latin = new TextDecoder('windows-1252');
const MB = (n) => Math.round(n / 1e6);

// CIV.INI as the game has it in its folder (any case), or where it will write it
function iniOf(entries) {
    const e = entries.find(([k]) => /^civ\.ini$/i.test(k));
    return {name: e ? e[0] : 'CIV.INI', text: e ? latin.decode(e[1].data) : ''};
}

async function artInfo(store) {
    const [info, p] = await Promise.all([store.hdGet('info'), pin()]);
    return info && info.sha256 === p.pack_sha256 ? info : null;
}

const opt = {store: null, spec: null, ini: null, was: null, now: null, art: null, fetching: null, message: '',
             built: false};

function optionsRender() {
    const {now, spec} = opt;
    if (!now)
        return;
    for (const r of document.querySelectorAll('input[name=mode]'))
        r.checked = +r.value === (now.classic ? 1 : 0);
    $('opt-sharp').checked = now.sharp;
    $('opt-hd').checked = now.hd;
    // (the art from this site only, where it has it: make web HD=1; kept, it stays usable)
    $('opt-hd').disabled = now.classic || !!opt.fetching || (!opt.site && !opt.art);
    $('opt-rules').disabled = $('opt-ui').disabled = now.classic;
    spec.rules.forEach((x, i) => { $('rule-' + i).checked = now.rule[i]; });
    spec.extras.forEach((x, i) => { $('extra-' + i).checked = now.extra[i]; });
    spec.limits.forEach((x, i) => { $('limit-' + i).value = now.limit[i]; });
    spec.colours.forEach((x, i) => { $('colour-' + i).value = now.colour[i]; });
    // the art
    const f = opt.fetching, status = $('art-status');
    $('art-stop').hidden = !f;
    $('art-bar').hidden = !f;
    $('art-remove').hidden = !!f || !opt.art;
    status.style.color = '';
    if (f) {
        const doing = {download: 'Downloading the art', check: 'Checking it',
                       keep: 'Keeping it in this browser'}[f.phase] || 'Getting the art';
        status.textContent = f.total ? `${doing}: ${MB(f.done)} of ${MB(f.total)} MB` : doing + ' ...';
        $('art-bar').firstElementChild.style.width = (f.total ? 100 * Math.min(1, f.done / f.total) : 0) + '%';
    } else if (opt.art) {
        status.textContent = `The art (openciv2-assets ${opt.art.tag}, ${MB(opt.art.size)} MB) is kept in this browser.`;
    } else if (opt.message) {
        status.style.color = '#e07a6a';
        status.textContent = opt.message + ' HD graphics stay off.';
    } else if (opt.site) {
        status.textContent = `HD graphics need the art: ${MB(opt.site.size)} MB, downloaded once from this site when ticked.`;
    } else {
        status.textContent = 'This site was built without the HD art (make web HD=1).';
    }
}

async function optionsSave() {
    const {store, spec, now, was} = opt;
    opt.ini.text = writeChoices(spec, opt.ini.text, now, was);
    opt.was = structuredClone(now);
    await store.putMany([[opt.ini.name, Uint8Array.from(opt.ini.text, (c) => c.charCodeAt(0) & 0xff), Date.now()]]);
}

function optionsChanged() {
    optionsRender();
    optionsSave().catch((e) => status('Could not keep the choices: ' + e.message));
}

// The art downloaded from the site, checked and kept; HD graphics on once
// it is, off if it failed
async function getArt() {
    const ac = new AbortController(), signal = ac.signal;
    const progress = (phase) => (done, total) => {
        opt.fetching = {phase, done, total};
        optionsRender();
    };
    opt.fetching = {phase: 'download', done: 0, total: 0};
    opt.message = '';
    $('art-stop').onclick = () => ac.abort();
    optionsRender();
    try {
        const p = await pin();
        const b = await download(opt.site.size, {signal, progress: progress('download')});
        const pack = await packFrom(b, p, {signal, progress: (phase, d, t) => progress(phase)(d, t)});
        if (navigator.storage && navigator.storage.estimate) {
            const {usage, quota} = await navigator.storage.estimate();
            if (quota && quota - usage < pack.length)
                throw new Error(`The browser's storage has no room for it (${MB(quota - usage)} MB free).`);
        }
        if (navigator.storage && navigator.storage.persist)
            navigator.storage.persist().catch(() => {});
        opt.art = await keepPack(opt.store, pack, p, {signal, progress: progress('keep')});
        opt.now.hd = !opt.now.classic;
    } catch (e) {
        if (e.name !== 'AbortError')
            console.error(e);
        opt.message = e.name === 'AbortError' ? 'Stopped.' : e.message.replace(/^./, (c) => c.toUpperCase()) +
            (/[.)]$/.test(e.message) ? '' : '.');
        opt.now.hd = false;
        // (a pack only partly kept: none)
        opt.art = await artInfo(opt.store).catch(() => null);
        if (!opt.art)
            await opt.store.hdClear().catch(() => {});
    }
    opt.fetching = null;
    optionsChanged();
    status(opt.art ? 'The HD graphics\' art is kept in this browser.' : 'HD graphics: ' + opt.message);
}

async function showOptions(store) {
    const name = (await store.keys()).find((k) => /^civ\.ini$/i.test(k));
    opt.store = store;
    opt.spec = await choicesSpec();
    opt.site = await siteArt();
    opt.ini = iniOf(name ? [[name, await store.get(name)]] : []);
    opt.was = readChoices(opt.spec, opt.ini.text);
    opt.now = structuredClone(opt.was);
    opt.art = await artInfo(store);
    const {spec} = opt;
    if (!opt.built) {
        opt.built = true;
        $('note-standard').textContent = spec.standard;
        $('note-classic').textContent = spec.classic;
        $('note-hd').textContent = spec.hd;
        $('note-sharp').textContent = spec.sharp;
        $('note-rules').textContent = spec.rulesIntro;
        $('note-limits').textContent = spec.limitsNote;
        $('note-ui').textContent = spec.uiIntro;
        const check = (id, label, note, set) => {
            const l = document.createElement('label'), c = document.createElement('input');
            l.className = 'choice';
            c.type = 'checkbox';
            c.id = id;
            c.onchange = () => {
                set(c.checked);
                optionsChanged();
            };
            l.append(c, ' ' + label);
            if (note) {
                const n = document.createElement('span');
                n.className = 'note';
                n.textContent = note;
                l.append(n);
            }
            return l;
        };
        const field = (id, label, size, set, wide) => {
            const l = document.createElement('label'), c = document.createElement('input');
            c.type = 'text';
            c.id = id;
            c.maxLength = size - 1;
            c.spellcheck = false;
            if (wide)
                c.style.width = '100%';
            c.onchange = () => {
                set(c.value.trim());
                optionsChanged();
            };
            l.append(label + ' ', c);
            if (wide)
                l.style.display = 'block';
            return l;
        };
        spec.rules.forEach((r, i) => $('rules').append(check('rule-' + i, r.label, r.note, (v) => { opt.now.rule[i] = v; })));
        spec.extras.forEach((x, i) => $('extras').append(check('extra-' + i, x.label, null, (v) => { opt.now.extra[i] = v; })));
        spec.limits.forEach((x, i) => $('limits').append(field('limit-' + i, x.label, 16, (v) => { opt.now.limit[i] = v; })));
        const last = spec.colours.length - 1;
        spec.colours.forEach((x, i) => {
            const l = field('colour-' + i, x.label, i === last ? 256 : 16, (v) => { opt.now.colour[i] = v; }, i === last);
            $('colours').append(l);
            if (i === last) {
                const n = document.createElement('span');
                n.className = 'note';
                n.textContent = spec.suppressNote;
                $('colours').append(n);
            }
        });
        for (const r of document.querySelectorAll('input[name=mode]'))
            r.onchange = () => {
                opt.now.classic = +r.value === 1;
                if (!opt.now.sharpSet)
                    opt.now.sharp = !opt.now.classic;   // (by default: not in Classic)
                optionsChanged();
            };
        $('opt-sharp').onchange = () => {
            opt.now.sharp = $('opt-sharp').checked;
            opt.now.sharpSet = true;
            optionsChanged();
        };
        $('opt-hd').onchange = () => {
            opt.now.hd = $('opt-hd').checked;
            optionsChanged();
            // (the art, where it is not here: downloaded, as the opening window does)
            if (opt.now.hd && !opt.art && !opt.fetching && opt.site)
                getArt();
        };
        $('art-remove').onclick = async () => {
            await store.hdClear();
            opt.art = null;
            opt.now.hd = false;
            optionsChanged();
        };
    }
    $('options').hidden = false;
    optionsRender();
}

// What a start needs from the site, fetched at once, alongside the stored
// files: the module (compiled as it downloads), the fonts, the art's pin
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
                 fonts: mark('fonts', Promise.all([fetchGz('data/fonts_win.bin.gz'), fetchGz('data/fonts_win_aa.bin.gz')])),
                 pin: pin()};
        // (a failure is met where it is awaited)
        for (const p of Object.values(siteP))
            p.catch(() => {});
    }
    return siteP;
}

async function play(store) {
    // (the game files and saved games kept for good: without this the browser
    // may clear them when disk space runs low, Safari after a week unused)
    if (navigator.storage && navigator.storage.persist)
        navigator.storage.persist().catch(() => {});
    $('setup').hidden = true;
    $('game').hidden = false;
    $('loading').hidden = false;
    status('Loading the game files ...');
    const t0 = performance.now();
    const site = fromSite();
    const fs = new MemFS();
    // CIV.INI first (a moment): the HD graphics' art, when wanted, read
    // alongside the other files
    const query = new URLSearchParams(location.search), args = ['civ2', GAME];
    const iniName = (await store.keys()).find((k) => /^civ\.ini$/i.test(k));
    const ini = iniOf(iniName ? [[iniName, await store.get(iniName)]] : []).text;
    const hd = hdWanted(ini, query.get('hd')) >= 0;
    const packP = hd ? mark('hd', site.pin.then((p) => loadPack(store, p, timing))).catch((e) => {
        console.error(e);
        return null;
    }) : null;
    const [entries, [fonts, fontsAa]] = await Promise.all([mark('files', store.all()), site.fonts]);
    for (const [k, v] of entries)
        fs.put(GAME + '/' + k, v.data, v.mtime);
    fs.mkdirs(GAME);
    fs.put(DATA + '/fonts_win.bin', fonts);
    fs.put(DATA + '/fonts_win_aa.bin', fontsAa);
    // HD graphics (?hd=N as --hd=N, else CIV.INI's choice): the art the
    // game draws, where hd.c looks for it (CIV2_DATA/hd)
    if (query.has('hd'))
        args.push('--hd=' + (query.get('hd') || '1'));
    // (sharp text and interface: ?sharp=0 off, as --sharp=0; else CIV.INI's)
    if (query.has('sharp'))
        args.push('--sharp=' + (query.get('sharp') || '1'));
    window.civ2Hd = null;
    if (hd) {
        const t = performance.now();
        status('Loading the HD graphics\' art ...');
        const pack = await packP;
        if (pack)
            fs.put(DATA + '/hd/atlas-hd.pack', pack);
        window.civ2Hd = {packBytes: pack ? pack.length : 0, loadMs: Math.round(performance.now() - t)};
        if (!pack)
            console.warn('HD graphics: the art is not in this browser (the ?setup page gets it)');
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
        code = await runGame({wasm: site.wasm, fs,
                              onFrame: () => {
                                  timing.frame = Math.round(performance.now());
                                  $('loading').hidden = true;
                              }, canvas, args, env, messageBox,
                              stats: window.civ2Stats, relay: relayAddress,
                              onRelayUnreachable: relayUnreachable});
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
    $('loading').hidden = true;
    $('setup').hidden = false;
    const have = await store.count() > 0;
    $('have').hidden = !have;
    $('options').hidden = true;
    if (have)
        await showOptions(store);
    $('play').onclick = () => play(store);
    $('forget').onclick = async () => {
        if (confirm('Remove the game files, saved games included, from this browser?')) {
            await store.clear();
            showSetup(store);
        }
    };
    if (!ended)
        status('');
    window.civ2Setup = true;
}

async function main() {
    if (!window.WebAssembly || !window.indexedDB || !window.DecompressionStream) {
        status('This browser lacks what the game needs (WebAssembly, IndexedDB, DecompressionStream).');
        return;
    }
    // (a later visit: what the game needs from the site, at once)
    const setup = new URLSearchParams(location.search).has('setup');
    if (!setup)
        fromSite();
    const store = await mark('store', openStore(() =>
        status('Waiting for another OpenCiv2 tab (from before an update) to close: close it, or reload it.')));
    savesControls(store);
    // (the relay's dialog keeps the keys from the game)
    const rd = $('relay-dialog');
    rd.addEventListener('keydown', (e) => e.stopPropagation());
    rd.addEventListener('keyup', (e) => e.stopPropagation());
    rd.addEventListener('close', () => $('screen').focus());
    window.civ2Relay = {address: relayAddress, unreachable: relayUnreachable};     // (the tests)
    const chosen = async (files) => {
        try {
            if (await importFiles(store, files))
                await showSetup(store, true);
        } catch (e) {
            console.error(e);
            status('Could not copy the files: ' + e.message);
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
            status('Could not read ' + (zip ? zip.name : 'the files') + ': ' + e.message);
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
    if (!setup && await store.count() > 0)
        return play(store);
    showSetup(store);
}

main();
