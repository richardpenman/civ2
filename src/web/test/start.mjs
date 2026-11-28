// The browser version's start without the original game (docs/BROWSER.md),
// as the desktop game starts without its files (docs/NO-INSTALL.md): build/web
// served, opened in Chrome's headless shell with a fresh profile (nothing in
// its storage), the browser's own autoplay policy (sound only after a click
// or a key), and no copy of the game given: the start screen first (Click to
// play, Export saves, Import saves; nothing else of a set-up: looked at at a
// desktop's width and a phone's, no sideways scrolling), its button clicked
// with the mouse: the sound started by that click (the page's audio context
// running at the main menu, its theme queued, before any other click), the
// HD graphics' art and OpenCiv2's own texts and sounds fetched (from the
// start screen on, its line under the button showing how far, the site
// slowed down) on the page's progress bar, then the regular main menu in HD; OpenCiv2's own music, sound
// effects and wonder movies all fetched; the game's folder noted as one
// without the original's files (store.js's meta 'game': no names), none of
// the original's files in it; a later visit (an old link's ?setup: the start
// screen) started with Enter once its preload is done, its first frame
// within LATER_MS, at once from what was kept and fetched (no bar); a
// menu's click heard at once (its sound mixed in ahead of the menu's queued
// theme).  (The original's files added over the game, its Settings' Classic
// graphics: hd.mjs; the saves exported and imported: smoke.mjs.)
// Needs no copy of the game.  Screenshots in run/web-start (look at them).
//
//   tools/toolchain/node/bin/node src/web/test/start.mjs

import {writeFileSync, readFileSync, mkdirSync, rmSync, existsSync} from 'node:fs';
import {join} from 'node:path';
import {ROOT, WEB, serve, browser, tmpDir} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-start');
// a later visit's start, the start screen having fetched what it waits for:
// its first frame at most this long after the key (some 1.6-2.7 s here, the
// game's own start; before the preload some 2.2-3 s from this machine's
// server, 18-21 s at 50 Mbit/s)
const LATER_MS = 6000;
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
    const b = await browser(join(work.path, 'profile'), undefined, url, true);
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
    const key = async (code, vk, k = code) => {
        for (const type of ['rawKeyDown', 'keyUp'])
            await send('Input.dispatchKeyEvent', {type, code, key: k, windowsVirtualKeyCode: vk});
    };
    // the start screen as shown: its controls laid out, no sideways scrolling
    const startScreen = () => evaluate(`(() => { const on = (id) => { const e = document.getElementById(id);
            return !!e && e.getClientRects().length > 0; };
        return {start: !!window.civ2Start, play: on('play'), exp: on('export-saves'), imp: on('import-label'),
                add: on('add'), choose: on('choose'), language: !!document.getElementById('language'),
                saves: !!document.getElementById('saves-open'), backdrop: getComputedStyle(document.getElementById('front')).backgroundImage,
                wide: document.documentElement.scrollWidth > innerWidth,
                text: document.getElementById('play').textContent, focus: document.activeElement && document.activeElement.id}; })()`);
    const startOk = (st) => st.start && st.play && st.exp && st.imp && !st.add && !st.choose && !st.language && !st.saves &&
        !st.wide && st.text === 'Click to play' && /title\.webp/.test(st.backdrop);
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
        await send('Network.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        // the first visit: the start screen, at a desktop's width and a
        // phone's, the site slow (some 4 MB/s) until the click's bar has
        // moved: what a start waits for fetched while it is shown, its
        // progress under Click to play
        await send('Network.emulateNetworkConditions', {offline: false, latency: 20, downloadThroughput: 4e6, uploadThroughput: 4e6});
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 10000, 'the start screen');
        const pre = await waitFor('(() => { const p = document.getElementById("preload"); return window.civ2Preload &&' +
                                  ' !window.civ2Preload.ready && getComputedStyle(p).visibility === "visible" &&' +
                                  ' /%$/.test(document.getElementById("preload-pct").textContent) &&' +
                                  ' document.getElementById("preload-pct").textContent; })()', 10000, 'the preload\'s progress');
        log('the start screen\'s preload: ' + pre);
        await evaluate('document.fonts.ready.then(() => new Promise((r) => { const i = new Image(); i.onload = i.onerror = r; i.src = "hd/title.webp"; }))');
        let st = await startScreen();
        log('the start screen: ' + JSON.stringify(st));
        await shot('00-start');
        if (!startOk(st) || st.focus !== 'play')
            throw new Error('the start screen is not as it should be');
        await send('Emulation.setDeviceMetricsOverride', {width: 390, height: 844, deviceScaleFactor: 1, mobile: true});
        await sleep(300);
        st = await startScreen();
        log('the start screen on a phone: ' + JSON.stringify(st));
        await shot('00-start-phone');
        if (!startOk(st))
            throw new Error('the start screen is not as it should be on a phone');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        await sleep(300);
        // (the page's audio contexts and what is played on them, from here)
        await evaluate(`(() => {
            const P = window.civ2AudioProbe = {contexts: [], started: 0};
            const AC = window.AudioContext;
            window.AudioContext = class extends AC { constructor(...a) { super(...a); P.contexts.push(this); } };
            const start = AudioBufferSourceNode.prototype.start;
            AudioBufferSourceNode.prototype.start = function (...a) { P.started++; return start.apply(this, a); };
        })()`);
        // Click to play, with the mouse: the bar at once
        const at = await evaluate('(() => { const r = document.getElementById("play").getBoundingClientRect();' +
                                  ' return [r.x + r.width / 2, r.y + r.height / 2]; })()');
        const t0 = performance.now();
        for (const type of ['mousePressed', 'mouseReleased'])
            await send('Input.dispatchMouseEvent', {type, x: at[0], y: at[1], button: 'left', clickCount: 1});
        await waitFor('window.civ2FirstStart && !document.getElementById("install").hidden', 10000, 'the progress bar');
        log('the bar: ' + JSON.stringify(await evaluate('window.civ2FirstStart')) + ', title: ' +
            await evaluate('document.getElementById("install-title").textContent'));
        await waitFor('window.civ2FirstStart && parseInt(window.civ2FirstStart.pct) >= 20 || /Starting/.test(window.civ2Status || "")',
                      120000, 'the art on its way');
        await shot('01-bar');
        await send('Network.emulateNetworkConditions', {offline: false, latency: 0, downloadThroughput: -1, uploadThroughput: -1});
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
        // the sound, from the start screen's click: running at the main
        // menu, its theme queued, no other click yet
        const sound = await evaluate('({contexts: civ2AudioProbe.contexts.map((c) => c.state), started: civ2AudioProbe.started})');
        log('the sound at the main menu: ' + JSON.stringify(sound));
        if (sound.contexts.length !== 1 || sound.contexts[0] !== 'running' || !sound.started)
            throw new Error('the sound not playing at the main menu after the start screen\'s click');
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
        // a later visit (an old link's ?setup: the start screen), started
        // with Enter: at once, no bar
        await send('Page.navigate', {url: url + '?setup'});
        await waitFor('window.civ2Start', 10000, 'the start screen (?setup)');
        st = await startScreen();
        await shot('04-start-later');
        if (!startOk(st) || !await evaluate('document.getElementById("forget").getClientRects().length > 0'))
            throw new Error('?setup does not open the start screen (with Remove from this browser): ' + JSON.stringify(st));
        // (what it waits for fetched meanwhile: its line gone, then the
        // first frame within LATER_MS of the key, the art not waited for)
        await waitFor('window.civ2Preload && window.civ2Preload.ready', 60000, 'the later visit\'s preload');
        if (await evaluate('getComputedStyle(document.getElementById("preload")).visibility') !== 'hidden')
            throw new Error('the preload\'s line still shown once all has come');
        await evaluate('window.addEventListener("keydown", () => window.civ2KeyAt = Math.round(performance.now()), true)');
        await key('Enter', 13);
        await waitFor('/Starting/.test(window.civ2Status || "")', 60000, 'the later start');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', 60000, 'the later start\'s first frame');
        const later = await evaluate('({timing: window.civ2Timing, key: window.civ2KeyAt, preload: window.civ2Preload})');
        const firstMs = later.timing.frame - later.key;
        log(`a later start: its first frame ${firstMs} ms after the key; ${JSON.stringify(later)}`);
        if (!(later.timing.hd < later.key) || firstMs > LATER_MS)
            throw new Error(`the later start waited: its first frame ${firstMs} ms after the key (at most ${LATER_MS}),` +
                            ` the art ${later.timing.hd < later.key ? 'there before it' : 'fetched after it'}`);
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
