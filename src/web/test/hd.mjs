// The browser version's HD graphics (docs/BROWSER.md): build/web in Chrome's
// headless shell (as smoke.mjs), the game's files given as a .zip, a first
// start in HD (the default: the art loaded), then in the game's Settings
// Classic graphics ticked and the game restarted (no art, Graphics=Classic
// in CIV.INI), then unticked again and the game restarted:
// the page loaded again, the art (the site's hd/art.bin: data/native/hd and,
// where present, the local data/native/hd-extra) put into the game's files,
// HD graphics on in CIV.INI; then the game at a
// device pixel ratio of 2 (density Auto: 2), the menu and a saved game's map
// drawn in HD (the canvas twice the desktop's size, and not the desktop's
// pixels enlarged), the first screen's text sharp, the Domestic Advisor on
// the art's background (a WebP); then a later visit's
// start (said at once, its first frame within START_MS); then
// ?hd=0: sharp text and interface (on by default) without the art; then
// ?hd=0&sharp=0: the original graphics; then the game without the
// original's pictures (dropped from its files as it starts): Classic graphics
// ticked in Settings, Find the original files...: the set-up page
// over the game, the .zip chosen there again, the files the game lacked
// added, the restart offered, and after it Classic graphics (no art
// loaded, Graphics=Classic in CIV.INI).  OpenCiv2's own music, sound
// effects and wonder movies: all of them fetched with HD graphics, with
// Classic graphics only those the game lacks.  Screenshots in
// run/web-hd (look at them).  Without the game it says so and passes.
//
//   tools/toolchain/node/bin/node src/web/test/hd.mjs [GAME_DIR]

import {writeFileSync, readFileSync, mkdirSync, rmSync, existsSync, readdirSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, zipFolder, serve, browser, tmpDir} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-hd');
const GAME = process.argv[2] || process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
// (a later visit's start, to the game's first frame: some 3 s here)
const START_MS = 15000;
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);

async function main() {
    if (!existsSync(join(WEB, 'civ2.wasm')))
        throw new Error('no build/web: make web first');
    if (!existsSync(join(WEB, 'hd', 'art.bin')))
        throw new Error('no build/web/hd/art.bin: make web first');
    if (!existsSync(join(GAME, 'Ca_Auto.SAV'))) {
        console.log('hd.mjs: skipped (no ' + join(GAME, 'Ca_Auto.SAV') + ')');
        return;
    }
    rmSync(OUT, {recursive: true, force: true});
    mkdirSync(OUT, {recursive: true});
    const work = tmpDir(), tmp = work.path;   // (run/web-tmp-*: cdp.mjs)
    const zip = join(tmp, 'Civ2.zip');
    log(`zipping ${GAME}: ${zipFolder(GAME, zip)} files`);
    const server = await serve();
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(tmp, 'profile'), undefined, url);
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
        if (!await evaluate('navigator.storage.persisted()'))
            throw new Error('the page\'s storage is not persistent (cdp.mjs: durableStorage)');
        const input = async (id, files) => {
            const {result} = await send('Runtime.evaluate', {expression: `document.getElementById("${id}")`});
            await send('DOM.setFileInputFiles', {objectId: result.objectId, files});
        };
        await input('files', [zip]);
        await waitFor('/kept in this browser/.test(window.civ2Status) || /not|Could/.test(window.civ2Status) && window.civ2Status',
                      300000, 'the copy');
        log('game: ' + await evaluate('window.civ2Status'));
        // the game's Settings (the main menu's last choice)
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
            await key('KeyE', 69, 'e');     // Settings (its hotkey: X24 gives it its first free letter)
            await key('Enter', 13);
            await settle(1000, 10000);
            await key('ArrowDown', 40);     // Graphics and screen
            await key('Enter', 13);
            await settle(1000, 10000);
        };
        // a first start: HD graphics by default (no Graphics= in CIV.INI:
        // HD_BY_DEFAULT), the art from the site; its Graphics and screen
        // page (Classic graphics unticked: look at the shot)
        await evaluate('document.getElementById("play").click()');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 600000, 'the start');
        if (!((await evaluate('window.civ2Hd') || {}).files > 100))
            throw new Error('the art not loaded at a first start (HD graphics by default)');
        await toMenu();
        await toGraphics();
        await shot('00-graphics-first');
        // Classic graphics ticked there (the first check box: Space), OK,
        // Restart now: the page loaded again, the original's look, no art
        await key('Space', 32, ' ');
        await settle(500, 5000);
        await shot('00-graphics-classic');
        await key('Enter', 13);
        await settle(1000, 10000);
        await evaluate('window.civ2Before = 1');
        await key('Enter', 13);             // Restart now
        await waitFor('!window.civ2Before && window.civ2Status', 60000, 'the page loaded again (Classic graphics)');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start with Classic graphics');
        await toMenu();
        await shot('00-classic');
        if (await evaluate('window.civ2Hd'))
            throw new Error('the art loaded with Classic graphics chosen');
        if (!/Graphics=Classic/.test(await evaluate('new TextDecoder("latin1").decode(window.civ2Fs.get(' +
                                                     '[...window.civ2Fs.files.keys()].find((k) => /^\\/civ2\\/civ\\.ini$/i.test(k))))')))
            throw new Error('CIV.INI does not have Graphics=Classic');
        // Classic graphics unticked again, OK, Restart now: the page loaded
        // again, the art from the site, the game in HD
        await toGraphics();
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
        const hdArt = await evaluate('window.civ2Hd') || {};
        log(`the art (${Math.round(performance.now() - t0)} ms): ${JSON.stringify(hdArt)}`);
        if (!(hdArt.files > 100))
            throw new Error('the art not loaded from the site at the restart');
        if (!await evaluate('!!window.civ2Fs.get("/data/hd/pictures.txt")'))
            throw new Error("the art's pictures' list not in the game's files");
        // (data/native/heralds, made by make own-assets, and data/native/hd-extra,
        // the local staging folder, neither in the repository: whatever of them
        // the site has must be in the game's files)
        const extra = JSON.parse(readFileSync(join(WEB, 'hd', 'index.json'), 'utf8'))
            .map((e) => e[0]).filter((n) => n.startsWith('hd-extra/') || n.startsWith('heralds/'));
        log(`OpenCiv2's own pictures and heralds on the site: ${extra.length ? extra.join(' ') : 'none (no data/native/heralds nor hd-extra)'}`);
        for (const n of extra)
            if (!await evaluate(`!!window.civ2Fs.get(${JSON.stringify('/data/' + n)})`))
                throw new Error(`${n} (data/native/heralds, hd-extra) not in the game's files`);
        // OpenCiv2's own music (the site's data/music: make own-assets): with
        // HD graphics all its tracks fetched, the game's own Music/TrackNN or
        // not (sound.c plays ours first)
        const siteMusic = JSON.parse(readFileSync(join(WEB, 'data', 'music', 'index.json'), 'utf8'));
        const gameMusic = existsSync(join(GAME, 'Music')) || existsSync(join(GAME, 'MUSIC'));
        await waitFor('window.civ2OwnMusic', 120000, 'the music looked at');
        const own = await evaluate('window.civ2OwnMusic');
        log(`OpenCiv2's own music with HD graphics: ${siteMusic.length} tracks on the site, ${own.length} fetched (the game ${gameMusic ? 'has' : 'has no'} Music)`);
        if (own.length !== siteMusic.length)
            throw new Error('OpenCiv2\'s own music not all fetched with HD graphics');
        // OpenCiv2's own sound effects (the site's data/sound: make
        // own-assets): with HD graphics all of them, the game's own
        // Sound/NAME.WAV or not (sound.c plays ours first)
        const siteSounds = JSON.parse(readFileSync(join(WEB, 'data', 'sound', 'index.json'), 'utf8'));
        const soundDir = ['Sound', 'SOUND', 'sound'].map((d) => join(GAME, d)).find((d) => existsSync(d));
        const gameSounds = new Set(soundDir ? readdirSync(soundDir).map((n) => n.toLowerCase()) : []);
        const soundsLacking = siteSounds.filter((n) => !gameSounds.has(n.slice(0, -4) + '.wav'));
        const ownSounds = await evaluate('window.civ2OwnSounds');
        log(`OpenCiv2's own sound effects with HD graphics: ${siteSounds.length} on the site, ${(ownSounds || []).length} fetched (the game lacks ${soundsLacking.length})`);
        if (!siteSounds.length || !ownSounds || ownSounds.length !== siteSounds.length)
            throw new Error('OpenCiv2\'s own sound effects not all fetched with HD graphics');
        // OpenCiv2's own wonder movies (the site's data/wonders: make
        // own-assets): with HD graphics all of them, the game's Video or not
        const siteWonders = JSON.parse(readFileSync(join(WEB, 'data', 'wonders', 'index.json'), 'utf8'));
        const videoDir = ['Video', 'VIDEO', 'video'].map((d) => join(GAME, d)).find((d) => existsSync(d));
        const gameVideos = new Set(videoDir ? readdirSync(videoDir).map((n) => n.toLowerCase()) : []);
        const wondersLacking = siteWonders.filter((nn) => !gameVideos.has('wonder' + nn + '.avi'));
        await waitFor('window.civ2OwnWonders', 120000, 'the wonder movies looked at');
        const ownWonders = await evaluate('window.civ2OwnWonders');
        log(`OpenCiv2's own wonder movies with HD graphics: ${siteWonders.length} on the site, ${ownWonders.length} fetched (the game lacks ${wondersLacking.length})`);
        if (!siteWonders.length || ownWonders.length !== siteWonders.length)
            throw new Error('OpenCiv2\'s own wonder movies not all fetched with HD graphics');
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
        // the Domestic Advisor (F1): the pack's background (its painting of a
        // steward's table, WebP), decoded by the module (src/platform/hd_image.c)
        // (look at it: a steward's table, not the original's picture)
        await key('F1', 112);
        await settle(2000, 15000);
        await shot('02-advisor');
        await key('Escape', 27);
        await settle(1000, 10000);
        // a later visit (the page opened again): what it does said at once,
        // the game's first frame within START_MS
        await send('Page.navigate', {url});
        const opened = performance.now();
        const said0 = await waitFor('(() => { const l = document.getElementById("loading");' +
                                    ' return l && !l.hidden && l.textContent || window.civ2Timing && window.civ2Timing.frame; })()',
                                    1000, 'a word from the page at once');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', START_MS, `the first frame within ${START_MS} ms`);
        const timing = await evaluate('window.civ2Timing');
        log(`a later start: "${said0}" ${Math.round(performance.now() - opened)} ms; ${JSON.stringify(timing)}`);
        if (!((await evaluate('window.civ2Hd') || {}).files > 100))
            throw new Error('the art not loaded at a later start');
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
        // Classic graphics without the original's pictures (docs/BROWSER.md
        // "How to play"): the game's files without TERRAIN1.GIF and Tiles.dll
        // as it starts (the store keeps them): HD graphics, whatever is chosen;
        // and without its music (Music/): OpenCiv2's own fetched, all its tracks
        const {identifier} = await send('Page.addScriptToEvaluateOnNewDocument', {source: `
            window.civ2Test = async (fs) => {
                for (const k of [...fs.files.keys()])
                    if (/^\\/civ2\\/(terrain1\\.gif|tiles\\.dll|music\\/.*)$/i.test(k))
                        fs.files.delete(k);
            };`});
        await send('Page.navigate', {url});
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start without the pictures');
        await waitFor('window.civ2OwnMusic', 120000, 'OpenCiv2\'s own music');
        const ownNow = await evaluate('window.civ2OwnMusic');
        const ownSizes = await evaluate(`window.civ2OwnMusic.map((n) => (window.civ2Fs.get('/data/music/' + n) || []).length)`);
        log(`without the game's Music: OpenCiv2's own fetched, ${ownNow.join(' ')} (${(ownSizes.reduce((a, b) => a + b, 0) / 1e6).toFixed(1)} MB)`);
        if (ownNow.length !== siteMusic.length || ownSizes.some((n) => !n))
            throw new Error('OpenCiv2\'s own music not all in the game\'s files without its Music');
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'an HD frame (forced)');
        await toMenu();
        await toGraphics();
        await shot('06-no-pictures-graphics');
        await key('Space', 32, ' ');        // Classic graphics ticked
        await key('Enter', 13);             // OK: the original's files are needed
        await settle(1000, 10000);
        await shot('06-find-original');
        await key('Enter', 13);             // Find the original files...
        await waitFor('window.civ2Adding', 10000, 'the set-up page over the game');
        await shot('07-add-page');
        await input('files', [zip]);
        await waitFor('window.civ2Adding === false', 300000, 'the files added');
        log('added: ' + await evaluate('window.civ2Status'));
        if (!await evaluate('[...window.civ2Fs.files.keys()].some((k) => /\\/terrain1\\.gif$/i.test(k))'))
            throw new Error('TERRAIN1.GIF not added to the game\'s files');
        await settle(1500, 10000);
        await shot('08-restart');
        await send('Page.removeScriptToEvaluateOnNewDocument', {identifier});
        await evaluate('window.civ2Before = 1');
        await key('Enter', 13);             // Restart now
        await waitFor('!window.civ2Before && window.civ2Status', 60000, 'the page loaded again');
        await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start with Classic graphics');
        await settle(3000, 60000);
        await shot('09-classic');
        if (await evaluate('window.civ2Hd'))
            throw new Error('the art loaded with Classic graphics');
        // (Classic graphics: OpenCiv2's music only for the tracks the game
        // lacks, none where it has its Music)
        await waitFor('window.civ2OwnMusic', 120000, 'the music looked at (Classic graphics)');
        const ownClassic = await evaluate('window.civ2OwnMusic');
        log(`OpenCiv2's own music with Classic graphics: ${ownClassic.length} fetched (the game ${gameMusic ? 'has' : 'has no'} Music)`);
        if (gameMusic ? ownClassic.length : ownClassic.length !== siteMusic.length)
            throw new Error(`OpenCiv2's own music with Classic graphics: ${ownClassic.length} fetched`);
        // (and its sound effects only for those the game lacks)
        const ownSoundsClassic = await evaluate('window.civ2OwnSounds');
        log(`OpenCiv2's own sound effects with Classic graphics: ${(ownSoundsClassic || []).length} fetched (the game lacks ${soundsLacking.length})`);
        if (!ownSoundsClassic || ownSoundsClassic.length !== soundsLacking.length)
            throw new Error(`OpenCiv2's own sound effects with Classic graphics: ${(ownSoundsClassic || []).length} fetched, not ${soundsLacking.length}`);
        // (and its wonder movies only for those the game lacks)
        await waitFor('window.civ2OwnWonders', 120000, 'the wonder movies looked at (Classic graphics)');
        const ownWondersClassic = await evaluate('window.civ2OwnWonders');
        log(`OpenCiv2's own wonder movies with Classic graphics: ${ownWondersClassic.length} fetched (the game lacks ${wondersLacking.length})`);
        if (ownWondersClassic.length !== wondersLacking.length)
            throw new Error(`OpenCiv2's own wonder movies with Classic graphics: ${ownWondersClassic.length} fetched, not ${wondersLacking.length}`);
        if (!/Graphics=Classic/.test(await evaluate('new TextDecoder("latin1").decode(window.civ2Fs.get(' +
                                                     '[...window.civ2Fs.files.keys()].find((k) => /^\\/civ2\\/civ\\.ini$/i.test(k))))')))
            throw new Error('CIV.INI does not have Graphics=Classic');
        if (errors.length)
            throw new Error('the page threw');
        log('ok: ' + OUT);
    } finally {
        await b.close();
        server.close();
        await sleep(500);
        work.remove();
    }
}

main().catch((e) => {
    console.error(e);
    process.exit(1);
});
