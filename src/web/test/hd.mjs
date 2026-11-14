// The browser version's HD graphics (docs/BROWSER.md): build/web in Chrome's
// headless shell (as smoke.mjs), the game's files given as a .zip, then in
// the game's OpenCiv2 Options HD graphics not offered while the site has no
// art; then the site with the art (as make web HD=1: data/native/hd/
// atlas-hd.pack, make hd, served at hd/), HD graphics ticked there and the
// game restarted: the page loaded again, the art downloaded from the site,
// checked, kept in IndexedDB, HD graphics on in CIV.INI; then the game at a
// device pixel ratio of 2 (density Auto: 2), the menu and a saved game's map
// drawn in HD (the canvas twice the desktop's size, and not the desktop's
// pixels enlarged), the first screen's text sharp; then a later visit's
// start (said at once, its first frame within START_MS); then
// ?hd=0: sharp text and interface (on by default) without the art; then
// ?hd=0&sharp=0: the original graphics.  Screenshots in
// run/web-hd (look at them).  Without the art (or the game) it says so and
// passes.
//
//   tools/toolchain/node/bin/node src/web/test/hd.mjs [GAME_DIR]

import {writeFileSync, mkdirSync, mkdtempSync, rmSync, existsSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, zipFolder, serve, browser} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-hd');
const GAME = process.argv[2] || process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
// (a later visit's start, to the game's first frame: some 3 s here)
const START_MS = 15000;
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);

const artFile = () => [join(ROOT, 'data', 'native', 'hd', 'atlas-hd.pack')].find((f) => existsSync(f));

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
    let siteArt = null;
    const server = await serve({hd: () => siteArt});
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
    // (box: that part of it only, [left, top, right, bottom] as fractions:
    // the middle, where the first screen's dialog is, its text drawn sharp
    // rather than enlarged)
    // (the first screen: the heralds' question the first time, in the
    // middle, else the main menu, at the bottom right)
    const MIDDLE = [0.3, 0.35, 0.7, 0.65], MENU = [0.68, 0.53, 0.94, 0.95];
    const firstScreen = async () => Math.max((await canvasState(MIDDLE)).mixed, (await canvasState(MENU)).mixed);
    const canvasState = (box = [0, 0, 1, 1]) => evaluate(`(() => { const c = document.getElementById("screen");
        const r = c.getBoundingClientRect(), d = c.getContext("2d").getImageData(0, 0, c.width, c.height).data;
        const [x0, y0, x1, y1] = ${JSON.stringify(box)}.map((f, i) => Math.floor(f * (i & 1 ? c.height : c.width) / 2) * 2);
        let blocks = 0, mixed = 0;
        for (let y = y0; y + 1 < y1; y += 2)
            for (let x = x0; x + 1 < x1; x += 2) {
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
        await waitFor('window.civ2Setup', 10000, 'the page');
        const input = async (id, files) => {
            const {result} = await send('Runtime.evaluate', {expression: `document.getElementById("${id}")`});
            await send('DOM.setFileInputFiles', {objectId: result.objectId, files});
        };
        await input('files', [zip]);
        await waitFor('/kept in this browser/.test(window.civ2Status) || /not|Could/.test(window.civ2Status) && window.civ2Status',
                      300000, 'the copy');
        log('game: ' + await evaluate('window.civ2Status'));
        // the game's OpenCiv2 Options (the main menu's last choice) where
        // the site has no art: HD graphics not offered there (look at the shot)
        const toMenu = async () => {
            await settle(3000, 60000);
            if (!await evaluate('(() => { const c = document.getElementById("screen");' +
                                ' const d = c.getContext("2d").getImageData(c.width - 8, c.height >> 1, 1, 1).data; return d[0] + d[1] + d[2] > 0; })()')) {
                await key('Enter', 13);     // (the heralds' question, the first time)
                await settle(3000, 30000);
            }
            await evaluate('document.getElementById("screen").focus()');
        };
        const toGraphics = async () => {
            await key('KeyO', 79, 'o');     // OpenCiv2 Options (its hotkey)
            await key('Enter', 13);
            await settle(1000, 10000);
            await key('ArrowDown', 40);     // Graphics and screen
            await key('Enter', 13);
            await settle(1000, 10000);
        };
        await evaluate('document.getElementById("play").click()');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start');
        await toMenu();
        await toGraphics();
        await shot('00-graphics-no-art');
        await key('Escape', 27);
        await settle(1000, 10000);
        await key('Escape', 27);
        await settle(1000, 10000);
        // the site with the art: HD graphics ticked there (Tab, Space), OK,
        // Restart now: the page loaded again, the art downloaded from the
        // site, checked, kept in this browser, the game in HD
        siteArt = art;
        await send('Page.navigate', {url: url + '?setup'});
        await waitFor('window.civ2Setup', 10000, 'the page again');
        await evaluate('document.getElementById("play").click()');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start again');
        await toMenu();
        await toGraphics();
        await key('Tab', 9);
        await key('Space', 32, ' ');
        await settle(500, 5000);
        await shot('00-graphics');
        await key('Enter', 13);
        await settle(1000, 10000);
        await shot('00-restart');
        await evaluate('window.civ2Before = 1');
        const t0 = performance.now();
        await key('Enter', 13);             // Restart now
        await waitFor('!window.civ2Before && window.civ2Status', 60000, 'the page loaded again');
        log('restarted: ' + await evaluate('window.civ2Status'));
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 600000, 'the art');
        log(`art from ${art} (${Math.round(performance.now() - t0)} ms): ${JSON.stringify(await evaluate('window.civ2Hd'))}`);
        if (!(await evaluate('window.civ2Hd') || {}).packBytes)
            throw new Error('the art not downloaded and kept at the restart');
        const ini = await evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
            const s = r.result.transaction("files").objectStore("files"), k = s.getAllKeys();
            k.onsuccess = () => { const name = k.result.find((n) => /^civ\\.ini$/i.test(n));
                const q = s.get(name); q.onsuccess = () => { r.result.close();
                    resolve(new TextDecoder("latin1").decode(q.result.data)); }; }; }; })`);
        const section = ini.slice(ini.indexOf('[OpenCiv2]')).split(/\r?\n\r?\n/)[0];
        log('CIV.INI: ' + section.replace(/\r?\n/g, ' '));
        if (!/^Graphics=HD$/m.test(section))
            throw new Error('CIV.INI does not have the choice');
        const usage = await evaluate('navigator.storage.estimate()');
        log(`browser storage used: ${Math.round(usage.usage / 1e6)} MB of ${Math.round(usage.quota / 1e6)} MB`);
        // played: HD graphics at the density Auto (the device's pixels: 2)
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'an HD frame');
        await settle(3000, 60000);
        let st = await canvasState();
        log('menu: ' + JSON.stringify(st));
        await shot('01-menu');
        if (st.w !== st.cssW * 2 || st.h !== st.cssH * 2)
            throw new Error('the canvas is not at the density 2');
        if (st.rendering === 'auto')
            throw new Error('the canvas is smoothed where it is shown at its own pixels');
        // the text sharp from the first screen (a dialog, before a game)
        const first = await firstScreen();
        log('the first screen\'s text: ' + first.toFixed(3));
        if (first < 0.05)
            throw new Error('the text is not sharp on the first screen');
        await loadMap();
        st = await canvasState();
        const stats = await evaluate('window.civ2Stats');
        log('map: ' + JSON.stringify(st));
        log(`HD frames: ${stats.hdFrames}, ${(stats.hdMs / stats.hdFrames).toFixed(1)} ms each to show ` +
            `(${st.w}x${st.h}); the module's memory ${Math.round(stats.memoryBytes / 1e6)} MB`);
        await shot('02-map');
        if (st.mixed < 0.05)
            throw new Error('the map is not drawn in HD (the desktop\'s pixels enlarged)');
        // a later visit (the page opened again, the art kept, its pruned
        // copy made by the start above; the site without it now: the art
        // kept still used): what it does said at once, the pruned art read
        // alone, the game's first frame within START_MS
        siteArt = null;
        await send('Page.navigate', {url});
        const opened = performance.now();
        const said0 = await waitFor('(() => { const l = document.getElementById("loading");' +
                                    ' return l && !l.hidden && l.textContent || window.civ2Timing && window.civ2Timing.frame; })()',
                                    1000, 'a word from the page at once');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', START_MS, `the first frame within ${START_MS} ms`);
        const timing = await evaluate('window.civ2Timing');
        log(`a later start: "${said0}" ${Math.round(performance.now() - opened)} ms; ${JSON.stringify(timing)}`);
        if (!(await evaluate('window.civ2Hd') || {}).packBytes)
            throw new Error('the art kept not used where the site has none');
        if (!(timing.packChunks <= 20))
            throw new Error('the whole pack read at a start, not its pruned copy');
        await settle(2000, 30000);
        // ?hd=0: the original graphics, as natively --hd=0, with sharp
        // text and interface (on by default): the planes without the art
        await send('Page.navigate', {url: url + '?hd=0'});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start without HD');
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'a sharp frame');
        await settle(3000, 60000);
        st = await canvasState();
        const text = await firstScreen();
        log('sharp: ' + JSON.stringify(st) + ', the first screen\'s text: ' + text.toFixed(3));
        await shot('03-sharp-menu');
        if (st.w !== st.cssW * 2 || text < 0.05)
            throw new Error('the text is not sharp (SharpText, by default)');
        await loadMap();
        const sharp = await evaluate('window.civ2Stats');
        log(`sharp frames: ${sharp.hdFrames}, ${(sharp.hdMs / sharp.hdFrames).toFixed(1)} ms each to show; ` +
            `the module's memory ${Math.round(sharp.memoryBytes / 1e6)} MB`);
        await shot('04-sharp-map');
        // ?hd=0&sharp=0: the original's look, the desktop's pixels enlarged
        await send('Page.navigate', {url: url + '?hd=0&sharp=0'});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start without HD');
        await settle(3000, 60000);
        await loadMap();
        st = await canvasState();
        log('?hd=0&sharp=0: ' + JSON.stringify(st) + `; the module's memory ${Math.round((await evaluate('window.civ2Stats')).memoryBytes / 1e6)} MB`);
        await shot('05-hd-off');
        // (the desktop at the device's 2 pixels each: its canvas half the shown size)
        if (st.w !== st.cssW || (await evaluate('window.civ2Stats')).hdFrames)
            throw new Error('?hd=0&sharp=0 is not the original graphics');
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
