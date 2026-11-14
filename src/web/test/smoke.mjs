// The browser version's smoke test: build/web served on a local port, opened
// in Chrome's headless shell (driven over the DevTools protocol, no other
// dependency), given the player's game files as a .zip through the page's
// own file input (after a set-up from a made-up disc image: disc.mjs), then the game: the main menu, a new game with the default
// choices, its first turn, a save; the saves exported and imported again
// (from the set-up page, and while playing), the saved game loaded.  Screenshots in run/web-smoke (the game's
// images: never committed).
//
//   tools/toolchain/node/bin/node src/web/test/smoke.mjs [--replay NAME] [GAME_DIR]
// (GAME_DIR: $CIV2_GAME or ~/Civ2; tools/fetch-toolchain.sh browser fetches
// node and the browser; make web-test runs it)
//
// --replay NAME: instead, the replay harness/scripts/NAME.txt as the native
// suite runs it (tools/native-run.sh: virtual clock, the harness's CIV2.DAT
// and CIV.INI, seed 7), its screenshots, rand() trace and logs written to
// run/web-NAME, to compare with the original's recording (run/ref-NAME):
//   tools/dk python3 harness/compare.py run/ref-NAME run/web-NAME
// (after converting the .bmp screenshots to .png; make web-replay does both).
// The run's time measures the module's speed (no real waiting in a replay).

import {readFileSync, writeFileSync, mkdirSync, readdirSync, mkdtempSync, rmSync, existsSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, zipFolder, serve, browser} from './cdp.mjs';
import {unzip} from '../zip.js';
import {sampleDisc, makeImage, tone} from './disc.mjs';

const OUT = join(ROOT, 'run', 'web-smoke');
const argv = process.argv.slice(2);
const REPLAY = argv[0] === '--replay' ? argv.splice(0, 2)[1] : null;
const GAME = argv[0] || process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);

// The replay: its files and settings given to the page before the game
// starts (main.js: window.civ2Test), its output taken from the page's file
// system once the game has ended.
async function replay(send, evaluate, waitFor) {
    const out = join(ROOT, 'run', 'web-' + REPLAY);
    rmSync(out, {recursive: true, force: true});
    mkdirSync(out, {recursive: true});
    await send('Page.addScriptToEvaluateOnNewDocument', {source: `
        window.civ2Test = async (fs, env) => {
            const get = async (u) => new Uint8Array(await (await fetch(u)).arrayBuffer());
            fs.put('/replay/script.txt', await get('/__test/${REPLAY}.txt'));
            fs.put('/civ2/CIV2.DAT', await get('/__test/CIV2.DAT'));
            const ini = await fetch('/__test/CIV.INI');
            if (ini.ok)
                fs.put('/civ2/CIV.INI', new Uint8Array(await ini.arrayBuffer()));
            fs.mkdirs('/replay/out');
            Object.assign(env, {CIV2_SCRIPT: '/replay/script.txt', CIV2_SHOT_DIR: '/replay/out', CIV2_SEED: '7',
                                CIV2_RAND_LOG: '/replay/out/rand.log', CIV2_HOOK_LOG: '/replay/out/hook.log',
                                CIV2_CALL_LOG: '/replay/out/calls.log', CIV2_WINDIR: '/civ2', CIV2_WINE_TEXT: '1'});
        };`});
    const t = performance.now();
    await send('Page.navigate', {url: (await evaluate('location.origin')) + '/'});
    await waitFor('/ended|stopped/.test(window.civ2Status || "") && window.civ2Status', 3600000, 'the replay');
    const wall = performance.now() - t;
    const st = await evaluate('window.civ2Status');
    const stats = await evaluate('window.civ2Stats');
    const names = await evaluate('window.civ2Fs.list("/replay/out").filter((e) => !e[1]).map((e) => e[0])');
    for (const n of names) {
        const b64 = await evaluate(`(() => { const d = window.civ2Fs.get('/replay/out/${n}'); let s = '';
            for (let i = 0; i < d.length; i += 32768) s += String.fromCharCode(...d.subarray(i, i + 32768));
            return btoa(s); })()`);
        writeFileSync(join(out, n), Buffer.from(b64, 'base64'));
    }
    log(`replay ${REPLAY}: ${st} in ${(wall / 1000).toFixed(1)} s (in the game's code ${(stats.busyMs / 1000).toFixed(1)} s, ` +
        `${stats.resumes} waits); ${names.length} files in ${out}`);
    if (/stopped/.test(st))
        throw new Error(st);
}

async function main() {
    if (!existsSync(join(WEB, 'civ2.wasm')))
        throw new Error('no build/web: make web first');
    mkdirSync(OUT, {recursive: true});
    mkdirSync(join(ROOT, 'run'), {recursive: true});
    const tmp = mkdtempSync(join(ROOT, 'run', 'web-tmp-'));   // (not /tmp: often a small tmpfs)
    const zip = join(tmp, 'Civ2.zip');
    log(`zipping ${GAME}: ${zipFolder(GAME, zip)} files`);
    const server = await serve();
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(tmp, 'profile'));
    const errors = [];
    b.on((m) => {
        if (m.method === 'Runtime.consoleAPICalled')
            console.log('  page:', m.params.args.map((a) => a.value ?? a.description).join(' '));
        if (m.method === 'Runtime.exceptionThrown') {
            errors.push(m.params.exceptionDetails);
            console.log('  page exception:', JSON.stringify(m.params.exceptionDetails).slice(0, 500));
        }
    });
    const {send} = b;
    const evaluate = async (expr) => (await send('Runtime.evaluate', {expression: expr, awaitPromise: true,
                                                                       returnByValue: true})).result.value;
    const shot = async (name) => {
        const r = await send('Page.captureScreenshot', {format: 'png'});
        writeFileSync(join(OUT, name + '.png'), Buffer.from(r.data, 'base64'));
        log('shot ' + name);
    };
    const key = async (code, vk, key = code) => {
        for (const type of ['rawKeyDown', 'keyUp'])
            await send('Input.dispatchKeyEvent', {type, code, key, windowsVirtualKeyCode: vk});
    };
    const type = async (text) => {
        for (const ch of text) {
            const shifted = ch === '_' || /[A-Z]/.test(ch);
            const code = /[a-z]/i.test(ch) ? 'Key' + ch.toUpperCase() : /[0-9]/.test(ch) ? 'Digit' + ch :
                ch === '.' ? 'Period' : ch === '_' ? 'Minus' : null;
            const vk = /[a-z0-9]/i.test(ch) ? ch.toUpperCase().charCodeAt(0) : ch === '.' ? 190 : 189;
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16, modifiers: 8});
            await key(code, vk, ch);
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'keyUp', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16});
        }
    };
    const click = async (x, y, count = 1) => {
        for (let i = 1; i <= count; i++)
            for (const type of ['mousePressed', 'mouseReleased'])
                await send('Input.dispatchMouseEvent', {type, x, y, button: 'left', clickCount: i});
    };
    const waitFor = async (expr, ms, what) => {
        const end = performance.now() + ms;
        while (performance.now() < end) {
            const v = await evaluate(expr);
            if (v)
                return v;
            await sleep(250);
        }
        throw new Error('timed out: ' + what);
    };
    const frame = () => evaluate('(() => { const c = document.getElementById("screen"); if (!c || !c.width) return 0;' +
        ' const d = new Uint32Array(c.getContext("2d").getImageData(0, 0, c.width, c.height).data.buffer);' +
        ' let h = 0; for (let i = 0; i < d.length; i += 7) h = (h * 31 + d[i]) | 0; return h; })()');
    // ms until the picture stopped changing for quiet ms (-1: not within most)
    const settle = async (quiet, most) => {
        const start = performance.now();
        let last = await frame(), since = start;
        while (performance.now() - start < most) {
            await sleep(250);
            const f = await frame();
            if (f !== last) {
                last = f;
                since = performance.now();
            } else if (performance.now() - since >= quiet)
                return Math.round(since - start);
        }
        return -1;
    };
    // the menu's backdrop fills the screen; the first run's question is on black
    const menuShown = () => evaluate('(() => { const c = document.getElementById("screen");' +
        ' const d = c.getContext("2d").getImageData(c.width - 4, c.height >> 1, 1, 1).data; return d[0] + d[1] + d[2] > 0; })()');
    const savedGames = () => evaluate('new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {' +
        ' const q = r.result.transaction("files").objectStore("files").getAllKeys();' +
        ' q.onsuccess = () => resolve(q.result.filter((k) => /\\.sav$/i.test(k))); }; })');
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        await send('Page.navigate', {url: url + '?setup'});
        await waitFor('window.civ2Status', 10000, 'the page');
        if (!REPLAY) {
            // the set-up from a disc image, a made-up one (disc.mjs: the
            // cabinet, the videos, two audio tracks), chosen as the player
            // chooses one (.cue and .bin): its files in IndexedDB, its music
            // as the browser decodes it
            const disc = sampleDisc(), image = makeImage(disc.iso, [tone(3, 440), tone(2, 660)], 'Civ2.bin');
            writeFileSync(join(tmp, 'Civ2.cue'), image.cue);
            writeFileSync(join(tmp, 'Civ2.bin'), image.bin);
            const {result: input} = await send('Runtime.evaluate', {expression: 'document.getElementById("image")'});
            await send('DOM.setFileInputFiles', {objectId: input.objectId, files: [join(tmp, 'Civ2.cue'), join(tmp, 'Civ2.bin')]});
            await waitFor('/set up from|failed|stopped|neither/.test(window.civ2Status) && window.civ2Status', 120000, 'the set-up');
            const said = await evaluate('window.civ2Status');
            log('disc image: ' + said);
            if (!/set up from/.test(said))
                throw new Error(said);
            const kept = await evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
                const s = r.result.transaction("files").objectStore("files");
                const got = (q) => new Promise((ok) => { q.onsuccess = () => ok(q.result); });
                Promise.all([got(s.getAllKeys()), got(s.getAll())]).then(async ([keys, values]) => {
                    const k = {result: keys}, q = {result: values}, out = {};
                    for (let i = 0; i < k.result.length; i++) {
                        const d = q.result[i].data;
                        out[k.result[i]] = /\.ogg$/.test(k.result[i])
                            ? (await new OfflineAudioContext(2, 44100, 44100).decodeAudioData(d.slice().buffer)).duration
                            : Array.from(d.subarray(0, 4096)).reduce((h, b) => (h * 31 + b) | 0, d.length);
                    }
                    r.result.close();
                    resolve(out);
                }); }; })`);
            const hash = (d) => Array.from(d.subarray(0, 4096)).reduce((h, b) => (h * 31 + b) | 0, d.length);
            for (const [k, v] of Object.entries(disc.expect))
                if (kept[k] !== hash(v))
                    throw new Error('set up from the disc image, wrong: ' + k);
            log('  music decoded: ' + ['Music/Track02.ogg', 'Music/Track03.ogg'].map((k) => k + ' ' + kept[k] + ' s').join(', '));
            if (Math.abs(kept['Music/Track02.ogg'] - 3) > 0.05 || Math.abs(kept['Music/Track03.ogg'] - 2) > 0.05)
                throw new Error('the music is not as the disc image has it');
            rmSync(join(tmp, 'Civ2.bin'));
        }
        const {result} = await send('Runtime.evaluate', {expression: 'document.getElementById("zip")'});
        await send('DOM.setFileInputFiles', {objectId: result.objectId, files: [zip]});
        const t0 = performance.now();
        await waitFor('/kept in this browser/.test(window.civ2Status) || /not|Could/.test(window.civ2Status) && window.civ2Status',
                      300000, 'the copy');
        const copied = await evaluate('window.civ2Status');
        log(`files copied into IndexedDB in ${Math.round(performance.now() - t0)} ms: ${copied}`);
        if (!/kept in this browser/.test(copied))
            throw new Error(copied);
        if (REPLAY)
            return await replay(send, evaluate, waitFor);
        // a later visit: straight into the game
        const t1 = performance.now();
        await send('Page.navigate', {url});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start');
        log('started: ' + await evaluate('window.civ2Status'));
        await waitFor('(() => { const c = document.getElementById("screen"); if (!c || !c.width) return false;' +
                      ' const d = c.getContext("2d").getImageData(0, 0, c.width, c.height).data;' +
                      ' for (let i = 0; i < d.length; i += 4097) if (d[i] || d[i + 1] || d[i + 2]) return true; return false; })()',
                      60000, 'the first picture');
        log(`first picture ${Math.round(performance.now() - t1)} ms after loading the page`);
        // the first screens: a question on the first run (the heralds), then the menu
        log(`settled after ${await settle(3000, 60000)} ms`);
        await shot('00-start');
        if (!await menuShown()) {
            await key('Enter', 13);
            await settle(3000, 30000);
        }
        await shot('01-menu');
        // a new game, the default choices (as harness/scripts/newgame.txt)
        for (let i = 2; i <= 10; i++) {
            await key('Enter', 13);
            await settle(1500, 20000);
            await shot(String(i).padStart(2, '0') + '-setup');
        }
        await key('Enter', 13);     // the city style: the world is built
        const t2 = performance.now();
        await sleep(2000);
        await shot('11-world');
        const built = await settle(3000, 600000);
        log(`world built in about ${built < 0 ? '(not settled)' : built + 2000} ms ` +
            `(${JSON.stringify(await evaluate('window.civ2Stats'))})`);
        await shot('12-intro');
        await key('Enter', 13);
        await settle(2000, 10000);
        await shot('13-tutorial');
        await key('Enter', 13);
        await settle(2000, 15000);
        await shot('14-map');
        await key('Enter', 13);     // the tutorial's second word
        await settle(2000, 10000);
        // saved (Ctrl+S, the file dialog's default name), kept in IndexedDB
        const before = await savedGames();
        await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code: 'ControlLeft', key: 'Control', windowsVirtualKeyCode: 17, modifiers: 2});
        await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code: 'KeyS', key: 's', windowsVirtualKeyCode: 83, modifiers: 2});
        await send('Input.dispatchKeyEvent', {type: 'keyUp', code: 'KeyS', key: 's', windowsVirtualKeyCode: 83, modifiers: 2});
        await send('Input.dispatchKeyEvent', {type: 'keyUp', code: 'ControlLeft', key: 'Control', windowsVirtualKeyCode: 17});
        await settle(1500, 10000);
        await shot('15-save');
        await key('Enter', 13);
        await settle(2000, 15000);
        await shot('16-saved');
        await sleep(1500);          // (the page keeps what is written a second later)
        const after = await savedGames();
        const added = after.filter((k) => !before.includes(k));
        log('saved games in IndexedDB: ' + after.join(' ') + (added.length ? ' (new: ' + added.join(' ') + ')' : ''));
        if (!added.length)
            throw new Error('the saved game is not in IndexedDB');
        // the saves exported from the set-up page (a download: the player's
        // files only), the saved game removed, imported again from the .zip
        const downloads = join(tmp, 'downloads');
        mkdirSync(downloads, {recursive: true});
        await send('Browser.setDownloadBehavior', {behavior: 'allow', downloadPath: downloads}).catch(
            () => send('Page.setDownloadBehavior', {behavior: 'allow', downloadPath: downloads}));
        await send('Page.navigate', {url: url + '?setup'});
        await waitFor('/Ready/.test(window.civ2Status || "")', 30000, 'the set-up page');
        await evaluate('document.querySelector("#have .export-saves").click()');
        const t3 = performance.now();
        let exported = null;
        while (!exported && performance.now() - t3 < 30000) {
            await sleep(250);
            exported = readdirSync(downloads).find((f) => /^OpenCiv2-saves-.*\.zip$/.test(f));
        }
        if (!exported)
            throw new Error('no saves exported: ' + await evaluate('window.civ2Status'));
        const zipped = join(downloads, exported);
        const zb = readFileSync(zipped);
        const inZip = (await unzip(zb.buffer.slice(zb.byteOffset, zb.byteOffset + zb.length))).map((f) => f.name);
        log(`exported ${exported}: ${inZip.join(' ')}`);
        if (!inZip.includes(added[0]) || inZip.some((n) => !/\.(sav|scn|mp)$|^(civ\.ini|civ2\.dat|hallfame\.dat)$/i.test(n)))
            throw new Error('the export is not the player\'s files');
        const removeSave = (k) => evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
            const t = r.result.transaction("files", "readwrite"); t.objectStore("files").delete(${JSON.stringify(k)});
            t.oncomplete = () => { r.result.close(); resolve(true); }; }; })`);
        const importFile = async (path) => {
            const {result: input} = await send('Runtime.evaluate', {expression: 'document.getElementById("import-saves")'});
            await send('DOM.setFileInputFiles', {objectId: input.objectId, files: [path]});
        };
        await removeSave(added[0]);
        if ((await savedGames()).includes(added[0]))
            throw new Error('the saved game was not removed');
        await importFile(zipped);
        await waitFor('/Imported|Could not|Nothing/.test(window.civ2Status) && window.civ2Status', 30000, 'the import');
        log('set-up page: ' + await evaluate('window.civ2Status'));
        if (!(await savedGames()).includes(added[0]))
            throw new Error('the saved game was not imported');
        // a later visit again (the saved game removed once more): imported
        // while the game runs (into its files too), then loaded from the menu
        await removeSave(added[0]);
        await send('Page.navigate', {url});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the second start');
        await settle(3000, 60000);
        await shot('17-again');
        if (await evaluate(`!!window.civ2Fs.get('/civ2/${added[0]}')`))
            throw new Error('the saved game is still there');
        await importFile(zipped);
        await waitFor('/Imported|Could not|Nothing/.test(document.getElementById("saves-status").textContent)', 30000,
                      'the import while playing');
        if (!await evaluate(`!!window.civ2Fs.get('/civ2/${added[0]}')`) || !(await savedGames()).includes(added[0]))
            throw new Error('the saved game imported while playing is not in the game\'s files');
        log('while playing: ' + await evaluate('document.getElementById("saves-status").textContent'));
        await evaluate('document.getElementById("screen").focus()');
        await key('KeyL', 76, 'l');     // Load a Game
        await key('Enter', 13);
        await settle(2000, 15000);
        await shot('18-load');
        // the saved game's name typed into the file dialog, then Enter
        await type(added[0]);
        await settle(1000, 5000);
        await key('Enter', 13);
        await settle(3000, 30000);
        await shot('19-loaded');
        await key('Enter', 13);
        await settle(3000, 20000);
        await shot('20-map');
        // the mouse: the game's own menu bar
        await click(20, 10);
        await settle(1000, 5000);
        await shot('21-menu');
        await key('Escape', 27);
        await settle(1000, 5000);
        // a smaller window: the desktop follows it
        await send('Emulation.setDeviceMetricsOverride', {width: 1024, height: 720, deviceScaleFactor: 1, mobile: false});
        await settle(3000, 20000);
        await shot('22-resized');
        const size = await evaluate('[document.getElementById("screen").width, document.getElementById("screen").height]');
        log('desktop after resizing: ' + size.join('x'));
        if (size[0] !== 1024 || size[1] !== 720)
            throw new Error('the desktop did not follow the window');
        const st = await evaluate('window.civ2Status');
        if (errors.length || /stopped/.test(st))
            throw new Error('the game stopped: ' + st);
        log('ok: ' + OUT);
    } finally {
        await b.close();
        server.close();
        await sleep(500);
        rmSync(tmp, {recursive: true, force: true, maxRetries: 10, retryDelay: 200});
    }
}

main().catch((e) => {
    console.error(e);
    process.exit(1);
});
