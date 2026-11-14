// The browser version's HD graphics (docs/BROWSER.md): build/web in Chrome's
// headless shell (as smoke.mjs), the game's files given as a .zip, then on
// the set-up page the art chosen as the player chooses it once downloaded
// (the release's .zip from the cache of tools/fetch_hd_pack.py, else
// data/native/hd/atlas-hd.pack: make hd; nothing downloaded here): checked,
// kept in IndexedDB, HD graphics switched on in CIV.INI; then the game at a
// device pixel ratio of 2 (density Auto: 2), the menu and a saved game's map
// drawn in HD (the canvas twice the desktop's size, and not the desktop's
// pixels enlarged); then ?hd=0: the original graphics.  Screenshots in
// run/web-hd (look at them).  Without the art (or the game) it says so and
// passes.
//
//   tools/toolchain/node/bin/node src/web/test/hd.mjs [GAME_DIR]

import {writeFileSync, mkdirSync, mkdtempSync, rmSync, existsSync, readFileSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, zipFolder, serve, browser} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-hd');
const GAME = process.argv[2] || process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);

function artFile() {
    const pin = JSON.parse(readFileSync(join(ROOT, 'data', 'hd-pack.json')));
    const cache = process.env.CIV2_DEPENDENCY_CACHE ||
        join(process.env.XDG_CACHE_HOME || join(homedir(), '.cache'), 'openciv2');
    const candidates = [join(cache, pin.sha256 + '.zip'), join(ROOT, 'data', 'native', 'hd', 'atlas-hd.pack')];
    return candidates.find((f) => existsSync(f));
}

async function main() {
    if (!existsSync(join(WEB, 'civ2.wasm')))
        throw new Error('no build/web: make web first');
    const art = artFile();
    if (!art || !existsSync(join(GAME, 'Ca_Auto.SAV'))) {
        console.log('hd.mjs: skipped (' + (art ? 'no ' + join(GAME, 'Ca_Auto.SAV') : 'no art: make hd') + ')');
        return;
    }
    rmSync(OUT, {recursive: true, force: true});
    mkdirSync(OUT, {recursive: true});
    const tmp = mkdtempSync(join(ROOT, 'run', 'web-tmp-'));
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
    const key = async (code, vk, k = code) => {
        for (const type of ['rawKeyDown', 'keyUp'])
            await send('Input.dispatchKeyEvent', {type, code, key: k, windowsVirtualKeyCode: vk});
    };
    const type = async (text) => {
        for (const ch of text) {
            const shifted = ch === '_' || /[A-Z]/.test(ch);
            const code = /[a-z]/i.test(ch) ? 'Key' + ch.toUpperCase() : /[0-9]/.test(ch) ? 'Digit' + ch :
                ch === '.' ? 'Period' : 'Minus';
            const vk = /[a-z0-9]/i.test(ch) ? ch.toUpperCase().charCodeAt(0) : ch === '.' ? 190 : 189;
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16, modifiers: 8});
            await key(code, vk, ch);
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'keyUp', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16});
        }
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
    // the canvas's size against the desktop's (its CSS size in device
    // pixels: the device pixel ratio 2), and the share of its 2x2 blocks
    // that are not one colour (the desktop's pixels enlarged twice: none)
    const canvasState = () => evaluate(`(() => { const c = document.getElementById("screen");
        const r = c.getBoundingClientRect(), d = c.getContext("2d").getImageData(0, 0, c.width, c.height).data;
        let blocks = 0, mixed = 0;
        for (let y = 0; y + 1 < c.height; y += 2)
            for (let x = 0; x + 1 < c.width; x += 2) {
                const a = (y * c.width + x) * 4, b = a + 4, e = a + c.width * 4, f = e + 4;
                blocks++;
                for (let k = 0; k < 3; k++)
                    if (d[a + k] !== d[b + k] || d[a + k] !== d[e + k] || d[a + k] !== d[f + k]) {
                        mixed++;
                        break;
                    }
            }
        return {w: c.width, h: c.height, cssW: r.width, cssH: r.height, dpr: devicePixelRatio,
                rendering: c.style.imageRendering, mixed: mixed / blocks}; })()`);
    // a saved game's map (harness/hd/hd-map.txt's)
    const loadMap = async () => {
        if (!await evaluate('(() => { const c = document.getElementById("screen");' +
                            ' const d = c.getContext("2d").getImageData(c.width - 8, c.height >> 1, 1, 1).data; return d[0] + d[1] + d[2] > 0; })()')) {
            await key('Enter', 13);
            await settle(3000, 30000);
        }
        await evaluate('document.getElementById("screen").focus()');
        await key('KeyL', 76, 'l');
        await key('Enter', 13);
        await settle(2000, 15000);
        await type('Ca_Auto.SAV');
        await settle(1000, 5000);
        await key('Enter', 13);
        await settle(3000, 60000);
        await key('Enter', 13);
        await settle(4000, 15000);      // (the active unit blinks)
    };
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 2, mobile: false});
        await send('Page.navigate', {url: url + '?setup'});
        await waitFor('window.civ2Status', 10000, 'the page');
        const input = async (id, files) => {
            const {result} = await send('Runtime.evaluate', {expression: `document.getElementById("${id}")`});
            await send('DOM.setFileInputFiles', {objectId: result.objectId, files});
        };
        await input('zip', [zip]);
        await waitFor('/kept in this browser/.test(window.civ2Status) || /not|Could/.test(window.civ2Status) && window.civ2Status',
                      300000, 'the copy');
        log('game: ' + await evaluate('window.civ2Status'));
        // the set-up page: the opening window's choices, HD graphics off
        await waitFor('!document.getElementById("options").hidden', 10000, 'the choices');
        if (await evaluate('document.getElementById("opt-hd").checked'))
            throw new Error('HD graphics on before the art');
        log('art: ' + await evaluate('document.getElementById("art-status").textContent'));
        // the art chosen (the release's .zip, or the pack alone): checked, kept
        const t0 = performance.now();
        await input('art-file', [art]);
        await waitFor('/kept in this browser|stay off/.test(document.getElementById("art-status").textContent) && ' +
                      'document.getElementById("art-bar").hidden', 600000, 'the art');
        const said = await evaluate('document.getElementById("art-status").textContent');
        log(`art from ${art}: ${said} (${Math.round(performance.now() - t0)} ms)`);
        if (!/kept in this browser/.test(said))
            throw new Error(said);
        if (!await evaluate('document.getElementById("opt-hd").checked'))
            throw new Error('HD graphics not on once the art is here');
        // the rules page's choice too, kept in CIV.INI as the opening window writes them
        await evaluate('document.getElementById("rule-0").click()');
        await sleep(500);
        const ini = await evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
            const s = r.result.transaction("files").objectStore("files"), k = s.getAllKeys();
            k.onsuccess = () => { const name = k.result.find((n) => /^civ\\.ini$/i.test(n));
                const q = s.get(name); q.onsuccess = () => { r.result.close();
                    resolve(new TextDecoder("latin1").decode(q.result.data)); }; }; }; })`);
        const section = ini.slice(ini.indexOf('[OpenCiv2]')).split(/\r?\n\r?\n/)[0];
        log('CIV.INI: ' + section.replace(/\r?\n/g, ' '));
        if (!/^Graphics=HD$/m.test(section) || !/^GotoNoZocStop=1$/m.test(section))
            throw new Error('CIV.INI does not have the choices');
        await evaluate('document.getElementById("options").scrollIntoView()');
        await shot('00-setup');
        await evaluate('document.getElementById("opt-rules").scrollIntoView()');
        await shot('00-setup-rules');
        const usage = await evaluate('navigator.storage.estimate()');
        log(`browser storage used: ${Math.round(usage.usage / 1e6)} MB of ${Math.round(usage.quota / 1e6)} MB`);
        // played: HD graphics at the density Auto (the device's pixels: 2)
        await evaluate('document.getElementById("play").click()');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start');
        log('started: ' + await evaluate('window.civ2Status') + ' ' + JSON.stringify(await evaluate('window.civ2Hd')));
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'an HD frame');
        await settle(3000, 60000);
        let st = await canvasState();
        log('menu: ' + JSON.stringify(st));
        await shot('01-menu');
        if (st.w !== st.cssW * 2 || st.h !== st.cssH * 2)
            throw new Error('the canvas is not at the density 2');
        if (st.rendering === 'auto')
            throw new Error('the canvas is smoothed where it is shown at its own pixels');
        await loadMap();
        st = await canvasState();
        const stats = await evaluate('window.civ2Stats');
        log('map: ' + JSON.stringify(st));
        log(`HD frames: ${stats.hdFrames}, ${(stats.hdMs / stats.hdFrames).toFixed(1)} ms each to show ` +
            `(${st.w}x${st.h}); the module's memory ${Math.round(stats.memoryBytes / 1e6)} MB`);
        await shot('02-map');
        if (st.mixed < 0.05)
            throw new Error('the map is not drawn in HD (the desktop\'s pixels enlarged)');
        // ?hd=0: the original graphics, as natively --hd=0
        await send('Page.navigate', {url: url + '?hd=0'});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start without HD');
        await settle(3000, 60000);
        await loadMap();
        st = await canvasState();
        log('?hd=0: ' + JSON.stringify(st) + `; the module's memory ${Math.round((await evaluate('window.civ2Stats')).memoryBytes / 1e6)} MB`);
        await shot('03-hd-off');
        // (the desktop at the device's 2 pixels each: its canvas half the shown size)
        if (st.w !== st.cssW || (await evaluate('window.civ2Stats')).hdFrames)
            throw new Error('?hd=0 is not the original graphics');
        if (errors.length)
            throw new Error('the page threw');
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
