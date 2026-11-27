// A unit's move in the browser version (docs/BROWSER.md): build/web served,
// opened in Chrome's headless shell with a fresh profile and no copy of the
// game (HD graphics, OpenCiv2's own files, as start.mjs), at a device pixel
// ratio of 2 (HD at density 2, as on a high-DPI screen: a frame of
// 2560x1600), a new game set up with the defaults but a small world, the
// world made, and the first units moved with the keypad: each move timed
// from the key to the slide's first frame, the slide (the frames the game
// presents, civ2Stats: host.js), and from the key to its last frame (the
// unit drawn where it went).  The original slides a unit in some 240 ms
// (map_animate_unit_move, src/auto/map.cpp: tile height / 2 + 1 frames,
// 256 ms / frames between them: at the default zoom 17 frames 15 ms apart);
// here a frame at density 2 used to cost more than that (the whole HD frame
// composed, converted and put on the canvas each time, whatever changed),
// and a slide took up to twice as long.  Fails where the
// slides' frames are on average more than FRAME_MS apart or a move takes
// more than MOVE_MS from its key to its last frame (the medians of the
// moves that slid).  Screenshots in run/web-perf (look at them).
//
//   tools/toolchain/node/bin/node src/web/test/perf.mjs
//
// PERF_SAVE=FILE: that saved game opened rather than a new game made
// (quicker: a copy of the original's Ca_Auto.SAV, say); PERF_DPR=N the
// device pixel ratio (default 2); PERF_PROFILE=1: the moves' CPU profile,
// its functions by self time (named with a civ2.wasm made with wasm-opt -g:
// CIV2_WEB=DIR, a copy of build/web with it).

import {writeFileSync, readFileSync, mkdirSync, rmSync, existsSync} from 'node:fs';
import {join} from 'node:path';
import {ROOT, WEB, serve, browser, tmpDir, clickPlay} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-perf');
const SAVE = process.env.PERF_SAVE, DPR = +(process.env.PERF_DPR || 2), PROFILE = !!process.env.PERF_PROFILE;
// (the original: 15 ms apart, 240 ms; here now some 16 to 19 ms and the
// key to the last frame 260 to 380 ms, before 30 ms and 440 to 600 ms; a
// busy machine slower)
const FRAME_MS = 25, MOVE_MS = 500;
const MOVES = 4;            // moves that slid, wanted
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);
const median = (a) => [...a].sort((x, y) => x - y)[a.length >> 1];

async function main() {
    if (!existsSync(join(WEB, 'civ2.wasm')) || !existsSync(join(WEB, 'hd', 'art.bin')))
        throw new Error('no build/web (or its hd/art.bin): make web first');
    rmSync(OUT, {recursive: true, force: true});
    mkdirSync(OUT, {recursive: true});
    const work = tmpDir();
    const server = await serve();
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(work.path, 'profile'), undefined, url);
    const errors = [];
    b.on((m) => {
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
    // the screen quiet for `quiet` ms (no row of the frame changed: host.js'
    // civ2Stats.hdRows), at most `most`
    const rows = () => evaluate('(window.civ2Stats || {}).hdRows || 0');
    const settle = async (quiet, most) => {
        const start = performance.now();
        let last = await rows(), since = start;
        while (performance.now() - start < most) {
            await sleep(200);
            const f = await rows();
            if (f !== last) {
                last = f;
                since = performance.now();
            } else if (performance.now() - since >= quiet)
                return Math.round(since - start);
        }
        return -1;
    };
    const key = async (code, vk, k = code) => {
        for (const type of ['rawKeyDown', 'keyUp'])
            await send('Input.dispatchKeyEvent', {type, code, key: k, windowsVirtualKeyCode: vk});
    };
    const enter = async (quiet = 1500, most = 20000) => {
        await key('Enter', 13);
        await settle(quiet, most);
    };
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: DPR, mobile: false});
        await send('Page.navigate', {url});
        await clickPlay(send);
        await waitFor('/Starting/.test(window.civ2Status || "")', 300000, 'the start');
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 60000, 'an HD frame');
        await settle(3000, 60000);
        await evaluate('document.getElementById("screen").focus()');
        // each frame host.js presents that changed something, timed
        // (civ2Stats.hdFrames' setter: the rows, the time in host.js)
        await evaluate(`(() => {
            const s = window.civ2Stats, log = window.civ2FrameLog = [];
            let n = s.hdFrames || 0, rows = s.hdRows || 0;
            Object.defineProperty(s, 'hdFrames', {get: () => n, set: (v) => {
                n = v;
                if (s.hdRows !== rows)
                    log.push([performance.now(), s.hdRows - rows]);
                rows = s.hdRows;
            }});
            window.civ2KeyLog = [];
            addEventListener('keydown', () => window.civ2KeyLog.push(performance.now()), true);
        })()`);
        const canvas = await evaluate('[document.getElementById("screen").width, document.getElementById("screen").height]');
        log(`the frame: ${canvas.join('x')}`);
        await shot('01-menu');
        if (SAVE) {
            await evaluate(`window.civ2Fs.put('/civ2/PERF.SAV', new Uint8Array(${JSON.stringify([...readFileSync(SAVE)])}))`);
            for (let i = 0; i < 4; i++)     // Open a Game
                await key('ArrowDown', 40);
            await key('Enter', 13);
            await settle(1500, 15000);
            for (const ch of 'PERF.SAV')
                await key(ch === '.' ? 'Period' : 'Key' + ch, ch === '.' ? 190 : ch.charCodeAt(0), ch);
            await sleep(500);
            await key('Enter', 13);
            await settle(2000, 60000);
            await key('Enter', 13);
            await sleep(4000);
        } else {
            // Begin a New Game, a small world (made sooner), the rest as
            // offered, until the world is being made
            await enter();
            await key('ArrowUp', 38);
            await shot('02-worldsize');
            for (let i = 0; i < 9; i++)
                await enter();
            await shot('03-setup-done');
            const t = performance.now();
            await settle(5000, 900000);
            log(`the world made in ${Math.round(performance.now() - t - 5000)} ms`);
            await shot('04-world');
            for (let i = 0; i < 3; i++) {
                await key('Enter', 13);
                await sleep(3000);
            }
        }
        await shot('05-map');
        if (PROFILE) {
            await send('Profiler.enable');
            await send('Profiler.setSamplingInterval', {interval: 200});
            await send('Profiler.start');
        }
        // the moves: the keypad's 7, 8, 9, 6, 3, 2, 1, 4 in turn until MOVES
        // of them slid (a move into the sea does not; the active unit's
        // blink is a frame every 150 ms or so)
        const DIRS = [['Numpad7', 0x24, 'Home'], ['Numpad8', 0x26, 'ArrowUp'], ['Numpad9', 0x21, 'PageUp'],
                      ['Numpad6', 0x27, 'ArrowRight'], ['Numpad3', 0x22, 'PageDown'], ['Numpad2', 0x28, 'ArrowDown'],
                      ['Numpad1', 0x23, 'End'], ['Numpad4', 0x25, 'ArrowLeft']];
        const slid = [];
        let stayed = 0;
        for (let i = 0; i < 32 && slid.length < MOVES; i++) {
            const [code, vk, k] = DIRS[i % DIRS.length];
            await sleep(1000);
            await evaluate('window.civ2FrameLog.length = 0; window.civ2KeyLog.length = 0');
            await key(code, vk, k);
            await sleep(2000);
            const f = await evaluate(`(() => {
                const t0 = window.civ2KeyLog[0];
                return window.civ2FrameLog.filter((e) => e[0] >= t0).map((e) => [e[0] - t0, e[1]]);
            })()`);
            // the slide: the frames less than 60 ms apart from the first
            let end = 0;
            while (end + 1 < f.length && f[end + 1][0] - f[end][0] < 60)
                end++;
            const n = f.length ? end + 1 : 0, gaps = [];
            for (let j = 1; j < n; j++)
                gaps.push(f[j][0] - f[j - 1][0]);
            const m = {key: code, frames: n, first: f.length ? Math.round(f[0][0]) : -1,
                       last: n ? Math.round(f[end][0]) : -1, slide: n ? Math.round(f[end][0] - f[0][0]) : 0,
                       gap: gaps.length ? Math.round(gaps.reduce((a, x) => a + x, 0) / gaps.length * 10) / 10 : 0,
                       maxGap: gaps.length ? Math.round(Math.max(...gaps)) : 0,
                       rows: n ? Math.round(f.slice(0, n).reduce((a, e) => a + e[1], 0) / n) : 0};
            log(`${code}: ${n >= 10 ? 'slid' : 'stayed'} ${JSON.stringify(m)}`);
            if (n >= 10) {
                slid.push(m);
                stayed = 0;
                await shot('06-move-' + slid.length);
            } else if (++stayed % 4 === 0) {
                // (the units' moves used up: the turn ended, Enter at its
                // end, a new turn's moves)
                await key('Enter', 13);
                await sleep(6000);
            }
        }
        if (PROFILE) {
            const {profile} = await send('Profiler.stop');
            const self = new Map(), dt = new Map();
            profile.samples.forEach((id, i) => dt.set(id, (dt.get(id) || 0) + (profile.timeDeltas[i] || 0)));
            for (const n of profile.nodes) {
                const name = (n.callFrame.functionName || '(anon)') + ' ' + n.callFrame.url.split('/').pop();
                self.set(name, (self.get(name) || 0) + (dt.get(n.id) || 0));
            }
            const total = [...self.values()].reduce((a, x) => a + x, 0);
            log('profile, self time (ms):');
            for (const [name, us] of [...self].sort((a, x) => x[1] - a[1]).slice(0, 25))
                console.log(`  ${(us / 1000).toFixed(1).padStart(8)} ${(100 * us / total).toFixed(1).padStart(5)}%  ${name}`);
        }
        if (!slid.length)
            throw new Error('no move slid: the unit stayed (look at the shots)');
        const gap = median(slid.map((m) => m.gap)), move = median(slid.map((m) => m.last));
        log(`${slid.length} moves slid: the key to the first frame ${median(slid.map((m) => m.first))} ms, the slide ` +
            `${median(slid.map((m) => m.slide))} ms in ${median(slid.map((m) => m.frames))} frames ${gap} ms apart, ` +
            `the key to the last frame ${move} ms (medians)`);
        if (gap > FRAME_MS)
            throw new Error(`the slide's frames ${gap} ms apart (at most ${FRAME_MS}; the original's 15)`);
        if (move > MOVE_MS)
            throw new Error(`a move took ${move} ms from its key to its last frame (at most ${MOVE_MS}; the original's slide 240)`);
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
