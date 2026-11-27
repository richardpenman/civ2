// The browser version's start without the original game (docs/BROWSER.md),
// as the desktop game starts without its files (docs/NO-INSTALL.md): build/web
// served, opened in Chrome's headless shell with a fresh profile (nothing in
// its storage) and no copy of the game given: the game starts at once from an
// empty game folder, the HD graphics' art and OpenCiv2's own texts and sounds
// fetched on the page's progress bar, then the regular main menu in HD;
// OpenCiv2's own music, sound effects and wonder movies all fetched; the
// game's folder noted as one without the original's files (store.js's meta
// 'game': no names), none of the original's files in it; the saves' dialog
// over the game leads to the set-up page (?setup: the original's files, the
// saves, Remove, the language), whose Play is there without a copy; a later
// visit starts at once from what was kept (no bar); then, where a copy of the
// game is at hand, a few of its files chosen on the set-up page: added to the
// game, what the game wrote kept; at the later visit, a menu's click heard
// at once (its sound mixed in ahead of the menu's queued theme).
// Needs no copy of the game.  Screenshots in run/web-start (look at them).
//
//   tools/toolchain/node/bin/node src/web/test/start.mjs [GAME_DIR]

import {writeFileSync, readFileSync, mkdirSync, rmSync, existsSync, copyFileSync} from 'node:fs';
import {join, basename} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, zipFolder, serve, browser, tmpDir} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-start');
const GAME = process.argv[2] || process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);

async function main() {
    if (!existsSync(join(WEB, 'civ2.wasm')) || !existsSync(join(WEB, 'hd', 'art.bin')))
        throw new Error('no build/web (or its hd/art.bin): make web first');
    if (existsSync(join(WEB, 'game', 'index.json')))
        throw new Error('build/web has a copy of the game (make web GAME=1): this test is of the site without it');
    rmSync(OUT, {recursive: true, force: true});
    mkdirSync(OUT, {recursive: true});
    const work = tmpDir();
    const server = await serve();
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(work.path, 'profile'), undefined, url);
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
    const waitFor = async (expr, ms, what) => {
        const end = performance.now() + ms;
        while (performance.now() < end) {
            const v = await evaluate(expr);
            if (v)
                return v;
            await sleep(100);
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
    // the main menu: its backdrop fills the screen (its right edge not black)
    const menuShown = () => evaluate('(() => { const c = document.getElementById("screen");' +
        ' const d = c.getContext("2d").getImageData(c.width - 4, c.height >> 1, 1, 1).data; return d[0] + d[1] + d[2] > 0; })()');
    const storeKeys = () => evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
            const tx = r.result.transaction(["files", "meta"]), k = tx.objectStore("files").getAllKeys(),
                  m = tx.objectStore("meta").get("game");
            tx.oncomplete = () => { r.result.close(); resolve({keys: k.result, game: m.result}); }; }; })`);
    const storeIni = () => evaluate(`new Promise((resolve) => { const r = indexedDB.open("OpenCiv2"); r.onsuccess = () => {
            const q = r.result.transaction("files").objectStore("files").get("CIV.INI");
            q.onsuccess = () => { r.result.close(); resolve(q.result ? new TextDecoder("latin1").decode(q.result.data) : null); }; }; })`);
    const site = (dir) => JSON.parse(readFileSync(join(WEB, 'data', dir, 'index.json'), 'utf8'));
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        // the first visit: no set-up page, the bar at once
        const t0 = performance.now();
        await send('Page.navigate', {url});
        await waitFor('window.civ2FirstStart && !document.getElementById("install").hidden', 10000, 'the progress bar');
        log('the bar: ' + JSON.stringify(await evaluate('window.civ2FirstStart')) + ', title: ' +
            await evaluate('document.getElementById("install-title").textContent'));
        if (!await evaluate('document.getElementById("setup").hidden'))
            throw new Error('the set-up page shown at a first visit');
        await waitFor('window.civ2FirstStart && parseInt(window.civ2FirstStart.pct) >= 20 || /Starting/.test(window.civ2Status || "")',
                      120000, 'the art on its way');
        await shot('01-bar');
        await waitFor('/Starting/.test(window.civ2Status || "")', 300000, 'the start');
        const bar = await evaluate('window.civ2FirstStart');
        log(`started in ${Math.round(performance.now() - t0)} ms: ${await evaluate('window.civ2Status')}; the bar's last: ${JSON.stringify(bar)}`);
        if (bar.moves < 5 || !/MB/.test(bar.step))
            throw new Error('the bar did not follow the downloads: ' + JSON.stringify(bar));
        const hd = await evaluate('window.civ2Hd') || {};
        log('the art: ' + JSON.stringify(hd));
        if (!(hd.files > 100))
            throw new Error('the art not loaded (HD graphics: the game has none of the original\'s pictures)');
        // the game's folder: empty but for what the game writes (and the
        // pictures and sounds it makes, OpenCiv2Pictures/, OpenCiv2Sounds/: not kept)
        const files = await evaluate('[...window.civ2Fs.files.keys()].filter((k) => k.startsWith("/civ2/") && !/^\\/civ2\\/OpenCiv2(Pictures|Sounds)\\//.test(k))');
        log('the game\'s folder: ' + (files.join(' ') || '(empty)'));
        if (files.some((k) => /\.(gif|dll|exe|wav|avi)$|\/rules\.txt$/i.test(k)))
            throw new Error('original files in the game\'s folder: ' + files.join(' '));
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'an HD frame');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', 60000, 'the first frame');
        await settle(3000, 60000);
        if (!await menuShown())
            throw new Error('no main menu');
        await shot('02-menu');
        // OpenCiv2's own music, sound effects, wonder movies, texts: all of them
        await waitFor('window.civ2OwnMusic && window.civ2OwnWonders', 120000, 'the music and wonder movies');
        const got = await evaluate('({music: window.civ2OwnMusic, sounds: window.civ2OwnSounds, wonders: window.civ2OwnWonders,' +
            ' texts: [...window.civ2Fs.files.keys()].filter((k) => k.startsWith("/data/text/")).length})');
        const want = {music: site('music'), sounds: site('sound'), wonders: site('wonders'), texts: site('text')};
        log(`OpenCiv2's own: music ${got.music.length}/${want.music.length}, sound effects ${(got.sounds || []).length}/` +
            `${want.sounds.length}, wonder movies ${got.wonders.length}/${want.wonders.length}, texts ${got.texts}/${want.texts.length}`);
        if (!want.music.length || !want.sounds.length || got.music.length !== want.music.length ||
            (got.sounds || []).length !== want.sounds.length || got.wonders.length !== want.wonders.length ||
            got.texts !== want.texts.length)
            throw new Error('OpenCiv2\'s own files not all fetched');
        // kept: the game noted as one without the original's files
        await evaluate('window.civ2Flush()');
        const meta = await storeKeys();
        log('kept: ' + JSON.stringify(meta));
        if (meta.keys.some((k) => /^OpenCiv2(Pictures|Sounds)\//i.test(k)))
            throw new Error('the pictures made from the art kept: ' + meta.keys.join(' '));
        if (!meta.game || meta.game.names.length)
            throw new Error('the game not noted as one of OpenCiv2\'s own files');
        // the saves' dialog over the game: Export, Import, and the set-up page
        await evaluate('document.getElementById("saves-open").click()');
        await sleep(300);
        await shot('03-saves');
        if (!await evaluate('document.getElementById("saves-dialog").open && !!document.getElementById("setup-open").offsetParent'))
            throw new Error('no way to the set-up page from the saves\' dialog');
        await evaluate('document.getElementById("setup-open").click()');
        await waitFor('window.civ2Setup && /[?&]setup\\b/.test(location.search)', 30000, 'the set-up page');
        const page = await evaluate(`({intro: document.querySelector('[data-t="web.intro"]').textContent,
            own: !document.getElementById('have-own').hidden, game: !document.getElementById('have-game').hidden,
            play: !!document.getElementById('play').offsetParent, choose: !!document.getElementById('choose').offsetParent,
            exp: !!document.querySelector('#have .export-saves').offsetParent, lang: !!document.getElementById('language').offsetParent})`);
        log('the set-up page: ' + JSON.stringify(page).slice(0, 400));
        await shot('04-setup');
        if (!page.own || page.game || !page.play || !page.choose || !page.exp || !page.lang ||
            /It plays with your own/.test(page.intro) || !/no copy of the original game is needed/.test(page.intro))
            throw new Error('the set-up page is not as it should be without a copy');
        // a later visit: at once, no bar
        const t1 = performance.now();
        await send('Page.navigate', {url});
        await waitFor('/Starting/.test(window.civ2Status || "")', 60000, 'the later start');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', 60000, 'the later start\'s first frame');
        log(`a later start: its first frame in ${Math.round(performance.now() - t1)} ms; ${JSON.stringify(await evaluate('window.civ2Timing'))}`);
        if (await evaluate('!!window.civ2FirstStart'))
            throw new Error('the bar shown at a later visit');
        if (!((await evaluate('window.civ2Hd') || {}).files > 100))
            throw new Error('the art not loaded at a later start');
        await settle(3000, 60000);
        if (!await menuShown())
            throw new Error('no main menu at the later start');
        await shot('05-later');
        // a menu's click heard at once: its sound (MENUOK) mixed into what
        // of the output is not yet heard, though the main menu's looping
        // theme keeps half a second queued (snd_wave_mix_queued): the main
        // menu's OK, then the world size's (at 1280x800); the audio buffers
        // scheduled after each click: one that differs from what was
        // scheduled for that time before (the click's sound in it), to start
        // at once when scheduled (at most 50 ms of the theme ahead of it;
        // before, 550 ms and more, and nothing scheduled again) and within
        // 500 ms of the click (the game's own time to it: some 20-100 ms on
        // an idle machine, more on a busy one)
        await evaluate(`(() => {
            const P = window.civ2SoundProbe = {clicks: [], starts: [], tl: []};
            window.addEventListener('pointerdown', () => P.clicks.push(performance.now()), true);
            const start = AudioBufferSourceNode.prototype.start;
            AudioBufferSourceNode.prototype.start = function (when = 0, ...rest) {
                const b = this.buffer, c = this.context, at = Math.max(when, c.currentTime);
                let diff = 0;
                if (b) {
                    const d = b.getChannelData(0), sr = b.sampleRate;
                    for (const o of P.tl) {
                        const lo = Math.max(at, o.at), hi = Math.min(at + b.duration, o.at + o.d.length / sr);
                        for (let x = lo; x < hi; x += 1 / sr) {
                            const v = Math.abs((d[Math.round((x - at) * sr)] || 0) - (o.d[Math.round((x - o.at) * sr)] || 0));
                            if (v > diff)
                                diff = v;
                        }
                    }
                    P.tl.push({at, d: d.slice()});
                    if (P.tl.length > 32)
                        P.tl.shift();
                }
                P.starts.push({t: performance.now(), ahead: (at - c.currentTime) * 1000, diff, state: c.state});
                return start.call(this, when, ...rest);
            };
        })()`);
        await sleep(1500);
        for (const [i, [x, y]] of [[963, 739], [642, 739]].entries()) {
            await evaluate('window.civ2SoundProbe.clicks = []; window.civ2SoundProbe.starts = []');
            for (const type of ['mousePressed', 'mouseReleased'])
                await send('Input.dispatchMouseEvent', {type, x, y, button: 'left', clickCount: 1});
            await sleep(1500);
            const p = await evaluate('({clicks: civ2SoundProbe.clicks, starts: civ2SoundProbe.starts.map(({t, ahead, diff, state}) => ({t, ahead, diff, state}))})');
            const heard = p.starts.find((b) => b.diff > 0.01);
            const ms = heard && p.clicks.length ? Math.round(heard.t - p.clicks[0] + Math.max(0, heard.ahead)) : -1;
            log(`a menu click's sound (${i ? 'the world size\'s' : 'the main menu\'s'} OK): ` +
                (ms < 0 ? 'none heard' : `starts ${ms} ms after the click (scheduled ${Math.round(heard.t - p.clicks[0])} ms after it,` +
                 ` ${Math.round(Math.max(0, heard.ahead))} ms ahead)`) + `; ${p.starts.length} buffers`);
            await shot(`05-later-click${i + 1}`);
            if (ms < 0 || heard.ahead > 50 || ms > 500)
                throw new Error('a menu click\'s sound not heard at once');
        }
        // a copy of the original chosen later on the set-up page (a few of
        // its files, from GAME_DIR, else ~/Civ2, if there): added to the
        // game, its CIV.INI and saved games kept, the page then saying the
        // game files are here
        const src = ['Civ2Art.dll', 'RULES.TXT', 'TERRAIN1.GIF'].map((n) => join(GAME, n));
        if (src.every((f) => existsSync(f))) {
            const dir = join(work.path, 'Civ2'), zip = join(work.path, 'Civ2.zip');
            mkdirSync(dir);
            for (const f of src)
                copyFileSync(f, join(dir, basename(f)));
            writeFileSync(join(dir, 'CIV.INI'), '[OpenCiv2]\r\nGraphics=Classic\r\n');
            zipFolder(dir, zip);
            await evaluate('window.civ2Flush()');
            const ini0 = await storeIni();
            await send('Page.navigate', {url: url + '?setup'});
            await waitFor('window.civ2Setup', 30000, 'the set-up page');
            const {result} = await send('Runtime.evaluate', {expression: 'document.getElementById("files")'});
            await send('DOM.setFileInputFiles', {objectId: result.objectId, files: [zip]});
            await waitFor('/added|kept in this browser|not|Could/.test(window.civ2Status) && window.civ2Status', 60000, 'the copy');
            const said = await evaluate('window.civ2Status');
            await sleep(300);
            const after = await evaluate(`({game: !document.getElementById('have-game').hidden,
                own: !document.getElementById('have-own').hidden})`);
            const kept = await storeKeys();
            log(`a copy chosen later: ${said} ${JSON.stringify(after)}; kept: ${kept.keys.join(' ')}; names ${JSON.stringify(kept.game.names)}`);
            await shot('06-copy-added');
            if (!/files of the original game added/.test(said) || !after.game || after.own || !kept.keys.includes('TERRAIN1.GIF') ||
                !kept.game.names.includes('Civ2Art.dll') || kept.game.names.includes('CIV.INI') || await storeIni() !== ini0)
                throw new Error('the copy not added to the game, its files kept');
        } else {
            log('a copy chosen later: skipped (no ' + src[0] + ')');
        }
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
