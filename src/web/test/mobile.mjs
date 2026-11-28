// The browser version on a phone (docs/BROWSER.md; host.js: smallScreen,
// smallDesktop, the touch handling; main.js: fullScreen; index.html: the
// game area's touch-action): build/web served, opened in Chrome's headless
// shell with a fresh profile and no copy of the game (OpenCiv2's own files,
// as start.mjs), the phone emulated over the DevTools protocol (a 844x390
// or 390x844 CSS px page at a device pixel ratio of 3, mobile, a touch
// screen: (pointer: coarse)), its fingers by Input.dispatchTouchEvent;
// what the game made of them read from its log (CIV2_TRACE_MSG: each
// message it took, msg 0201 the left button pressed, 0204 the right, 0200 a
// move, wp=1 with the left button held; the windows it made, with their
// titles and places; CIV2_TRACE_PT: their moves).  Checked:
//   - on a desktop (1280x800, no touch): the desktop as before (1280x800),
//     no full screen asked for, a press held still the left button's (no
//     long press);
//   - Click to play on a phone asks for full screen, then a sideways lock
//     (both stubbed: a headless browser shows neither), and starts though
//     they fail (full screen refused: no lock asked for; the lock refused);
//   - the start screen can be zoomed (a pinch);
//   - held sideways (844x390): a 999x480 desktop at density 2 (not 1000
//     wide: the city window would grow), its canvas the page's full height,
//     centred, no scrollbars, nothing off the page;
//   - the main menu: a tap selects an item (Settings, then OK: its
//     dialog), a long press the right button's click (no left), a press
//     that moves a drag (the left button held through the moves, no right
//     click though held still afterwards), a second finger ignored;
//   - in the game: neither a pinch nor a double tap zooms the page, a
//     swipe neither scrolls it nor reloads it (pull to refresh);
//   - turned upright (390x844) under the world size's dialog: a 640x1385
//     desktop, the canvas the page's full width, the dialog (placed for 999
//     wide) moved onto it, its choice and OK tapped there; turned back:
//     999x480 again, the game still answering;
//   - a new game (a small world, the rest as offered), a city founded: its window inside the 999x480 desktop,
//     its Exit button tapped (the window closed), the city tapped on the
//     map (its window opened again); a long press on the map the right
//     button's;
//   - the menu bar: the Game menu open, the Kingdom menu opened instead by
//     the mouse moved onto its title (no button), by Right, by a tap on
//     its title (its Tax Rate then chosen: its dialog).
// Screenshots in run/web-mobile (look at them).  Needs no copy of the game.
//
//   tools/toolchain/node/bin/node src/web/test/mobile.mjs

import {writeFileSync, mkdirSync, rmSync, existsSync} from 'node:fs';
import {join} from 'node:path';
import {ROOT, WEB, serve, browser, tmpDir, clickPlay} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-mobile');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(`[${(performance.now() / 1000).toFixed(1)}s] ${s}`);
const LANDSCAPE = {width: 844, height: 390, deviceScaleFactor: 3, mobile: true,
                   screenOrientation: {type: 'landscapePrimary', angle: 90}};
const PORTRAIT = {width: 390, height: 844, deviceScaleFactor: 3, mobile: true,
                  screenOrientation: {type: 'portraitPrimary', angle: 0}};
const DESKTOP = {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false};

// (before the page's own script: full screen and the screen's lock
// recorded in window.civ2Calls, not done; ?stub=fs: full screen refused,
// else the lock refused)
const STUBS = `
    window.civ2Calls = [];
    const refuse = (what) => Promise.reject(new DOMException(what + ' refused (test)', 'NotAllowedError'));
    Element.prototype.requestFullscreen = function (o) {
        civ2Calls.push('fullscreen');
        return /stub=fs/.test(location.search) ? refuse('full screen') : Promise.resolve();
    };
    if (window.ScreenOrientation)
        ScreenOrientation.prototype.lock = function (o) {
            civ2Calls.push('lock ' + o);
            return refuse('the lock');
        };
    window.civ2Marker = Math.random();
    window.civ2Test = async (fs, env) => {
        fs.mkdirs('/out');
        Object.assign(env, {CIV2_HOOK_LOG: '/out/hook.log', CIV2_TRACE_MSG: '1', CIV2_TRACE_PT: '-1,-1'});
    };`;

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
    const check = (ok, what) => {
        if (!ok)
            throw new Error(what);
        log('ok: ' + what);
    };
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
            await sleep(200);
        }
        throw new Error('timed out: ' + what);
    };
    // the screen quiet for `quiet` ms (no row of the frame changed:
    // civ2Stats.hdRows, host.js), at most `most`
    const rows = () => evaluate('(window.civ2Stats || {}).hdRows || 0');
    const settle = async (quiet = 1500, most = 30000) => {
        const start = performance.now();
        let last = await rows(), since = start;
        while (performance.now() - start < most) {
            await sleep(200);
            const f = await rows();
            if (f !== last) {
                last = f;
                since = performance.now();
            } else if (performance.now() - since >= quiet)
                return;
        }
    };
    const key = async (code, vk, k = code) => {
        for (const type of ['rawKeyDown', 'keyUp'])
            await send('Input.dispatchKeyEvent', {type, code, key: k, windowsVirtualKeyCode: vk});
    };
    // the game's log, from `from` on (its length: mark())
    const hookAll = () => evaluate('(() => { const d = window.civ2Fs && window.civ2Fs.get("/out/hook.log");' +
                                   ' return d ? new TextDecoder("latin1").decode(d) : ""; })()');
    const mark = async () => (await hookAll()).length;
    const since = async (from) => (await hookAll()).slice(from);
    // the messages the game took: [msg, window, wp] (WM_TIMER, the sound's
    // and painting left out)
    const msgs = (text) => [...text.matchAll(/^msg ([0-9a-f]{4}) w(\d+) wp=([0-9a-f]+)/gm)]
        .map((m) => [m[1], +m[2], parseInt(m[3], 16)]).filter(([m]) => !['03bd', '03bb', '000f', '0014'].includes(m));
    const buttons = (text) => msgs(text).filter(([m]) => /^020[1-5]$/.test(m));
    // the windows the game made after `from`: {title, x, y, w, h, index (wN), parent}
    const windows = (text) => {
        const out = [];
        const re = /^CreateWindowEx\(\d+, (\w+), "([^"]*)", [0-9a-f]+, (-?\d+), (-?\d+), (-?\d+), (-?\d+), parent ([0-9a-f]+)\)\ncreate w(\d+)/gm;
        for (const m of text.matchAll(re))
            out.push({cls: m[1], title: m[2], x: +m[3], y: +m[4], w: +m[5], h: +m[6], parent: m[7], index: +m[8]});
        return out;
    };
    // a window shown by the game, the last made of that title: its handle,
    // its place on the desktop (the last move's, else where it was made),
    // its controls' places (in its client area, inside a 3 px frame)
    const dialog = (text, title) => {
        const i = text.lastIndexOf(`CreateWindowEx(0, MSWindowClass, "${title}", `);
        if (i < 0)
            return null;
        const made = /^CreateWindowEx\(\d+, \w+, "[^"]*", [0-9a-f]+, (-?\d+), (-?\d+), (\d+), (\d+)/.exec(text.slice(i));
        const kid = /CreateWindowEx\(4, \w+, "[^"]*", [0-9a-f]+, -?\d+, -?\d+, \d+, \d+, parent ([0-9a-f]+)\)/.exec(text.slice(i));
        const handle = kid[1];
        const kids = [...text.slice(i).matchAll(new RegExp(`^CreateWindowEx\\(4, \\w+, "[^"]*", [0-9a-f]+, (-?\\d+), (-?\\d+), (\\d+), (\\d+), parent ${handle}\\)`, 'gm'))]
            .map((m) => m.slice(1).map(Number));
        const g = [...text.slice(i).matchAll(new RegExp(`^geom: \\w+\\(${handle}, .*-> window (-?\\d+),(-?\\d+),(-?\\d+),(-?\\d+)`, 'gm'))].pop();
        const [x, y, w, h] = made.slice(1).map(Number);
        const rect = g ? g.slice(1).map(Number) : [x, y, x + w, y + h];
        return {handle, rect, kids: kids.map(([kx, ky, kw, kh]) => ({x: rect[0] + 3 + kx, y: rect[1] + 3 + ky, w: kw, h: kh}))};
    };
    // the canvas: its pixels, its place on the page (CSS px)
    const canvas = () => evaluate(`(() => { const c = document.getElementById("screen"), r = c.getBoundingClientRect();
        return {w: c.width, h: c.height, left: r.left, top: r.top, right: r.right, bottom: r.bottom}; })()`);
    // a point of the desktop (desktop px) -> the page's (CSS px)
    let desk = [0, 0];
    const at = async (x, y) => {
        const c = await canvas();
        return [c.left + (x + 0.5) * (c.right - c.left) / desk[0], c.top + (y + 0.5) * (c.bottom - c.top) / desk[1]];
    };
    // fingers: touch(type, [[x, y] ...]) (CSS px; each point its own id)
    const touch = (type, pts, ids = pts.map((_, i) => i)) =>
        send('Input.dispatchTouchEvent', {type, touchPoints: pts.map(([x, y], i) => ({x, y, id: ids[i], radiusX: 4, radiusY: 4, force: 1}))});
    const tap = async (x, y, hold = 60) => {
        await touch('touchStart', [[x, y]]);
        await sleep(hold);
        await touch('touchEnd', []);
    };
    const tapDesk = async (x, y) => tap(...await at(x, y));
    const viewport = () => evaluate(`({scale: visualViewport.scale, x: scrollX, y: scrollY, ox: visualViewport.offsetLeft,
        oy: visualViewport.offsetTop, sw: document.documentElement.scrollWidth, sh: document.documentElement.scrollHeight,
        iw: innerWidth, ih: innerHeight, marker: window.civ2Marker})`);
    // the page as shown: its size, no scrollbars, every element shown on it
    const layout = () => evaluate(`(() => {
        const off = [];
        for (const e of document.querySelectorAll('body *')) {
            if (!e.getClientRects().length || getComputedStyle(e).visibility === 'hidden')
                continue;
            const r = e.getBoundingClientRect();
            if (r.width && r.height && (r.left < -0.5 || r.top < -0.5 || r.right > innerWidth + 0.5 || r.bottom > innerHeight + 0.5))
                off.push((e.id || e.tagName) + ' ' + [r.left, r.top, r.right, r.bottom].map(Math.round).join(','));
        }
        const d = document.documentElement;
        return {iw: innerWidth, ih: innerHeight, sw: d.scrollWidth, sh: d.scrollHeight, off,
                bars: innerWidth - d.clientWidth + innerHeight - d.clientHeight};
    })()`);
    const desktopOf = (c) => [c.w / 2, c.h / 2];        // (density 2 on a phone at DPR 3)
    const checkLayout = async (name, want) => {
        const c = await canvas(), l = await layout();
        log(`${name}: canvas ${JSON.stringify(c)}, page ${JSON.stringify(l)}`);
        check(c.w === want[0] * 2 && c.h === want[1] * 2, `${name}: a ${want.join('x')} desktop at density 2 (canvas ${c.w}x${c.h})`);
        // (the desktop's shape kept: the page's full height sideways, its full width upright)
        const full = want[0] / want[1] > l.iw / l.ih ? [c.left, c.right, l.iw] : [c.top, c.bottom, l.ih];
        check(Math.abs(full[0]) < 1 && Math.abs(full[1] - full[2]) < 1 &&
              Math.abs(c.left + c.right - l.iw) < 1 && Math.abs(c.top + c.bottom - l.ih) < 1,
              `${name}: the canvas fills the page (centred)`);
        check(l.sw <= l.iw && l.sh <= l.ih && !l.bars, `${name}: no scrollbars`);
        check(!l.off.length, `${name}: nothing off the page ${l.off.join('; ')}`);
    };
    const running = async () => !/ended|stopped/.test(await evaluate('window.civ2Status || ""'));
    // the main menu: its backdrop fills the screen (its right edge not black)
    const MENU_SHOWN = '(() => { const c = document.getElementById("screen"); if (!c.width) return false;' +
        ' const d = c.getContext("2d").getImageData(c.width - 4, c.height >> 1, 1, 1).data; return d[0] + d[1] + d[2] > 0; })()';
    const toMenu = async () => {
        await waitFor('/Starting/.test(window.civ2Status || "")', 300000, 'the start');
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 120000, 'an HD frame');
        await waitFor(MENU_SHOWN, 180000, 'the main menu');
        await settle(2000, 60000);
    };
    // the main menu's dialog (the last one made): its items and buttons
    // (the 9 items, OK, Cancel: wN), their places on the desktop
    const mainMenu = async () => {
        const all = windows(await hookAll());
        const i = all.map((w) => w.title).lastIndexOf('OpenCiv2');
        if (i < 0)
            throw new Error('no main menu in the game\'s log');
        const d = all[i], kids = all.slice(i + 1, i + 12);
        // (the client area: 3 px of frame; the controls' places in it)
        return {dialog: d, items: kids.slice(0, 9).map((k) => ({w: k.index, x: d.x + 3 + k.x + k.w / 2, y: d.y + 3 + k.y + k.h / 2})),
                ok: {w: kids[9].index, x: d.x + 3 + kids[9].x + kids[9].w / 2, y: d.y + 3 + kids[9].y + kids[9].h / 2}};
    };

    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Page.addScriptToEvaluateOnNewDocument', {source: STUBS});

        // ---- a desktop: as before ----
        await send('Emulation.setDeviceMetricsOverride', DESKTOP);
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        check(!await evaluate('matchMedia("(pointer: coarse)").matches'), 'the desktop: no touch screen');
        await clickPlay(send);
        await toMenu();
        let c = await canvas();
        log('the desktop\'s canvas: ' + JSON.stringify(c));
        check(c.w === 1280 && c.h === 800 && c.right - c.left === 1280 && c.bottom - c.top === 800,
              `the desktop: a 1280x800 desktop as before (canvas ${c.w}x${c.h})`);
        check(!(await evaluate('civ2Calls')).length, 'the desktop: no full screen asked for');
        desk = [1280, 800];
        let menu = await mainMenu(), from = await mark();
        {
            const [x, y] = await at(menu.items[2].x, menu.items[2].y);
            await send('Input.dispatchMouseEvent', {type: 'mousePressed', x, y, button: 'left', clickCount: 1});
            await sleep(900);
            await send('Input.dispatchMouseEvent', {type: 'mouseReleased', x, y, button: 'left', clickCount: 1});
            await settle(1000, 10000);
            const got = buttons(await since(from));
            log('a mouse button held 0.9 s: ' + JSON.stringify(got));
            check(got.some(([m, w]) => m === '0201' && w === menu.items[2].w) && got.some(([m]) => m === '0202') &&
                  !got.some(([m]) => m === '0204'), 'the desktop: the mouse held still is the left button\'s, no long press');
        }
        await shot('00-desktop');

        // ---- a phone: Click to play's full screen refused ----
        await send('Emulation.setTouchEmulationEnabled', {enabled: true, maxTouchPoints: 5});
        await send('Emulation.setDeviceMetricsOverride', LANDSCAPE);
        await send('Page.navigate', {url: url + '?stub=fs'});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        check(await evaluate('matchMedia("(pointer: coarse)").matches'), 'the phone: a touch screen');
        const playAt = () => evaluate('(() => { const r = document.getElementById("play").getBoundingClientRect();' +
                                      ' return [r.x + r.width / 2, r.y + r.height / 2]; })()');
        await tap(...await playAt());
        await waitFor('(window.civ2Stats || {}).hdFrames > 0', 300000, 'the game\'s first frame (full screen refused)');
        await sleep(500);
        let calls = await evaluate('civ2Calls');
        log('full screen refused: ' + JSON.stringify(calls));
        check(JSON.stringify(calls) === '["fullscreen"]' && !errors.length && await running(),
              'full screen asked for at the tap; refused: no lock asked for, the game started, nothing thrown');

        // ---- a phone held sideways (the lock refused) ----
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        // the start screen: a pinch zooms it
        const pinch = async (x, y, grow) => {
            await touch('touchStart', [[x - 20, y]]);
            await touch('touchStart', [[x - 20, y], [x + 20, y]]);
            for (let i = 1; i <= 15; i++) {
                await touch('touchMove', [[x - 20 - grow * i, y], [x + 20 + grow * i, y]]);
                await sleep(16);
            }
            await touch('touchMove', [[x - 20 - grow * 15, y]]);
            await touch('touchEnd', []);
            await sleep(500);
        };
        await pinch(420, 300, 6);
        let v = await viewport();
        log('the start screen pinched: ' + JSON.stringify(v));
        check(v.scale > 1.2, 'the start screen zooms (a pinch)');
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        await tap(...await playAt());
        await toMenu();
        calls = await evaluate('civ2Calls');
        log('the lock refused: ' + JSON.stringify(calls));
        check(JSON.stringify(calls) === '["fullscreen","lock landscape"]' && !errors.length,
              'full screen, then the sideways lock asked for at the tap; the lock refused: the game started, nothing thrown');
        desk = [999, 480];
        await checkLayout('sideways', desk);
        await shot('01-menu');

        // ---- the main menu by touch ----
        menu = await mainMenu();
        // a tap: Settings, then OK: its dialog
        from = await mark();
        await tapDesk(menu.items[8].x, menu.items[8].y);
        await settle(800, 10000);
        let got = buttons(await since(from));
        log('Settings tapped: ' + JSON.stringify(got));
        check(JSON.stringify(got) === JSON.stringify([['0201', menu.items[8].w, 1], ['0202', menu.items[8].w, 0]]),
              'a tap: the left button\'s click on the item');
        await tapDesk(menu.ok.x, menu.ok.y);
        await settle(1500, 20000);
        check(windows(await since(from)).some((w) => w.title === 'Settings'), 'the main menu\'s Settings chosen by taps (its dialog)');
        await shot('02-settings');
        await key('Escape', 27);
        await settle(1500, 20000);
        menu = await mainMenu();
        // a long press: the right button's click, no left
        from = await mark();
        {
            const [x, y] = await at(menu.items[2].x, menu.items[2].y);
            await touch('touchStart', [[x, y]]);
            await sleep(1000);
            const held = buttons(await since(from));
            await touch('touchEnd', []);
            await settle(800, 10000);
            got = buttons(await since(from));
            log('a long press: ' + JSON.stringify(got));
            check(held.some(([m]) => m === '0204') && got.some(([m, w]) => m === '0204' && w === menu.items[2].w) &&
                  got.some(([m]) => m === '0205') && !got.some(([m]) => m === '0201'),
                  'a long press: the right button\'s click while held, no left');
        }
        // a second finger: ignored
        from = await mark();
        {
            const p = await at(menu.items[3].x, menu.items[3].y), q = await at(menu.items[4].x, menu.items[4].y);
            await touch('touchStart', [p]);
            await touch('touchStart', [p, q]);
            await sleep(80);
            await touch('touchMove', [p]);
            await touch('touchEnd', []);
            await settle(800, 10000);
            got = buttons(await since(from));
            log('two fingers: ' + JSON.stringify(got));
            check(JSON.stringify(got) === JSON.stringify([['0201', menu.items[3].w, 1], ['0202', menu.items[3].w, 0]]),
                  'a second finger ignored: one tap, the first finger\'s');
        }
        // a press that moves: a drag (the left button held), not a right
        // click though held still afterwards
        from = await mark();
        {
            const p = await at(menu.items[1].x, menu.items[1].y), q = await at(menu.items[2].x, menu.items[2].y);
            await touch('touchStart', [p]);
            for (let i = 1; i <= 8; i++) {
                await touch('touchMove', [[p[0], p[1] + (q[1] - p[1]) * i / 8]]);
                await sleep(20);
            }
            await sleep(900);
            await touch('touchEnd', []);
            await settle(800, 10000);
            const m = msgs(await since(from)).filter(([k]) => /^020[0-5]$/.test(k));
            log('a drag: ' + JSON.stringify(m));
            const down = m.findIndex(([k]) => k === '0201'), up = m.findIndex(([k]) => k === '0202');
            check(down >= 0 && m[down][1] === menu.items[1].w && up > down &&
                  m.slice(down + 1, up).some(([k, , wp]) => k === '0200' && wp & 1) && !m.some(([k]) => k === '0204'),
                  'a press that moves: a drag (pressed on the first item, moved with the left button held, released), no right click');
        }
        // the page's gestures over the game: no zoom, no scroll, no reload
        // (on the main menu's picture, away from its dialog)
        {
            const [x, y] = await at(200, 300);
            await pinch(x, y, 6);
            v = await viewport();
            log('pinched over the game: ' + JSON.stringify(v));
            check(v.scale === 1, 'a pinch over the game does not zoom the page');
            await send('Input.synthesizeTapGesture', {x, y, tapCount: 2, gestureSourceType: 'touch'});
            await sleep(800);
            v = await viewport();
            check(v.scale === 1, 'a double tap over the game does not zoom the page');
            const marker = v.marker;
            for (const [y0, y1] of [[20, 380], [380, 20]]) {
                await touch('touchStart', [[x, y0]]);
                for (let i = 1; i <= 12; i++) {
                    await touch('touchMove', [[x, y0 + (y1 - y0) * i / 12]]);
                    await sleep(16);
                }
                await touch('touchEnd', []);
                await sleep(800);
            }
            v = await viewport();
            log('swiped over the game: ' + JSON.stringify(v));
            check(v.scale === 1 && v.x === 0 && v.y === 0 && !v.ox && !v.oy && v.marker === marker && await running(),
                  'swipes over the game neither scroll the page nor reload it');
        }
        await settle(1500, 20000);
        // Start a New Game, OK
        menu = await mainMenu();
        from = await mark();
        await tapDesk(menu.items[0].x, menu.items[0].y);
        await settle(800, 10000);
        await tapDesk(menu.ok.x, menu.ok.y);
        await settle(1500, 20000);
        check(windows(await since(from)).some((w) => w.title === 'Choose Size of World'),
              'Start a New Game chosen by taps (the world\'s size asked)');

        // ---- turned upright and back ----
        // (the world's size asked: its dialog, 999 wide, then on a desktop
        // 640 wide: moved onto it, its buttons tapped there)
        let size = dialog(await hookAll(), 'Choose Size of World');
        log('the world size\'s dialog sideways: ' + JSON.stringify(size.rect));
        await send('Emulation.setDeviceMetricsOverride', PORTRAIT);
        await waitFor('document.getElementById("screen").height === 2770', 30000, 'the upright desktop');
        await settle(1500, 20000);
        desk = [640, 1385];
        await checkLayout('upright', desk);
        await shot('03-upright');
        size = dialog(await hookAll(), 'Choose Size of World');
        log('the world size\'s dialog upright: ' + JSON.stringify(size));
        const inside = (b) => b.x >= 0 && b.y >= 0 && b.x + b.w <= desk[0] && b.y + b.h <= desk[1];
        check(size.rect[2] <= desk[0] && size.kids.every(inside), 'upright: the dialog shown then moved onto the narrower desktop');
        from = await mark();
        await tapDesk(size.kids[0].x + 40, size.kids[0].y + size.kids[0].h / 2);   // Small
        await settle(800, 10000);
        await tapDesk(size.kids[3].x + size.kids[3].w / 2, size.kids[3].y + size.kids[3].h / 2);   // OK
        await settle(1500, 20000);
        check(windows(await since(from)).some((w) => w.title === 'Choose Difficulty Level'),
              'upright: the dialog\'s choice and OK tapped (the next question asked)');
        await send('Emulation.setDeviceMetricsOverride', LANDSCAPE);
        await waitFor('document.getElementById("screen").width === 1998', 30000, 'the sideways desktop again');
        await settle(1500, 20000);
        desk = [999, 480];
        await checkLayout('sideways again', desk);
        check(await running(), 'turned: the game still running');

        // ---- a new game: the rest as offered ----
        from = await mark();
        for (let i = 0; i < 9; i++) {
            await key('Enter', 13);
            await settle(1500, 30000);
        }
        check(windows(await since(from)).some((w) => w.title === 'Choose Your City Style'), 'turned: the game still answering (the new game\'s questions)');
        // (the world made; its first messages: Enter while each brings
        // another; the settlers then waiting for orders)
        await settle(5000, 900000);
        const dismiss = async () => {
            for (let i = 0; i < 15; i++) {
                const before = windows(await hookAll()).length;
                await key('Enter', 13);
                await settle(2000, 30000);
                if (windows(await hookAll()).length === before)
                    return;
            }
        };
        await dismiss();
        await shot('04-map');
        // the active unit's square: where the canvas changes as it blinks
        const blink = await evaluate(`new Promise((resolve) => {
            const c = document.getElementById("screen"), g = c.getContext("2d");
            const first = g.getImageData(0, 0, c.width, c.height).data;
            let x0 = 1e9, y0 = 1e9, x1 = -1, y1 = -1, n = 0;
            const look = () => {
                const d = g.getImageData(0, 0, c.width, c.height).data;
                for (let y = 0; y < c.height; y += 2)
                    for (let x = 0; x < c.width; x += 2) {
                        const i = (y * c.width + x) * 4;
                        if (d[i] !== first[i] || d[i + 1] !== first[i + 1] || d[i + 2] !== first[i + 2]) {
                            x0 = Math.min(x0, x); x1 = Math.max(x1, x); y0 = Math.min(y0, y); y1 = Math.max(y1, y);
                        }
                    }
                if (++n < 12)
                    setTimeout(look, 100);
                else
                    resolve([x0 / 2, y0 / 2, x1 / 2, y1 / 2].map(Math.round));
            };
            setTimeout(look, 100);
        })`);
        log('the active unit blinks in ' + JSON.stringify(blink));
        check(blink[2] > blink[0] && blink[2] - blink[0] < 120 && blink[3] - blink[1] < 120,
              'the settlers found on the map (blinking)');
        const square = [(blink[0] + blink[2]) / 2, (blink[1] + blink[3]) / 2];

        // ---- a city: its window, its Exit button, tapped on the map ----
        // the city window: the window with a scroll bar and nine buttons
        // (its handle, its place on the desktop, its buttons' places)
        const cityWindow = (text) => {
            const kids = new Map();
            for (const m of text.matchAll(/^CreateWindowEx\(\d+, (\w+), "[^"]*", [0-9a-f]+, (-?\d+), (-?\d+), (\d+), (\d+), parent ([0-9a-f]+)\)/gm)) {
                if (!kids.has(m[6]))
                    kids.set(m[6], []);
                kids.get(m[6]).push({cls: m[1], x: +m[2], y: +m[3], w: +m[4], h: +m[5]});
            }
            const found = [...kids].filter(([, k]) => k.some((c) => c.cls === 'MSScrollBarClass') &&
                                                      k.filter((c) => c.cls === 'MSControlClass').length >= 9);
            if (!found.length)
                return null;
            const [handle, k] = found[found.length - 1];
            const g = [...text.matchAll(new RegExp(`^geom: \\w+\\(${handle}, .*-> window (-?\\d+),(-?\\d+),(-?\\d+),(-?\\d+) client (\\d+)x(\\d+)`, 'gm'))].pop();
            if (!g)
                return null;
            const [l, t, r, bt, cw, ch] = g.slice(1).map(Number);
            const fx = (r - l - cw) / 2, fy = (bt - t - ch) / 2;
            // (the buttons in the client area; the frame round it)
            const buttons = k.filter((c) => c.cls === 'MSControlClass').map((c) =>
                ({x: l + fx + c.x, y: t + fy + c.y, w: c.w, h: c.h}));
            // Exit: the bottom row's last button
            const exit = buttons.reduce((a, c) => (c.y > a.y || (c.y === a.y && c.x > a.x) ? c : a));
            return {handle, rect: [l, t, r, bt], client: [cw, ch], buttons, exit};
        };
        const shown = (text, handle, how) => new RegExp(`^ShowWindow\\(${handle}, ${how}\\)`, 'm').test(text);
        from = await mark();
        await key('KeyB', 66, 'b');
        let city = null;
        for (let i = 0; i < 6 && !city; i++) {
            await settle(2000, 30000);
            const t = await since(from), w = cityWindow(await hookAll());
            if (w && shown(t, w.handle, 5) && !new RegExp(`ShowWindow\\(${w.handle}, 5\\)[^]*ShowWindow\\(${w.handle}, 0\\)`).test(t))
                city = w;
            else
                await key('Enter', 13);     // (the city's name; the new city's picture)
        }
        check(city, 'a city founded: its window shown');
        log('the city window: ' + JSON.stringify(city));
        await shot('05-city');
        check(city.buttons.every(inside), `sideways: the city window's buttons all on the ${desk.join('x')} desktop`);
        // (its frame's last row may be off it, as on the original's 640x480)
        check(city.rect[0] >= 0 && city.rect[1] >= 0 && city.rect[2] <= desk[0] && city.rect[3] <= desk[1] + 1,
              `sideways: the city window on the desktop (${city.rect.join(',')})`);
        from = await mark();
        await tapDesk(city.exit.x + city.exit.w / 2, city.exit.y + city.exit.h / 2);
        await settle(1500, 20000);
        check(shown(await since(from), city.handle, 0), 'the city window\'s Exit button tapped: the window closed');
        await dismiss();
        await shot('06-after-city');
        // the city tapped on the map: its window again
        from = await mark();
        await tapDesk(...square);
        await settle(2000, 20000);
        let t = await since(from);
        let again = cityWindow(await hookAll());
        check(again && shown(t, again.handle, 5), 'the city tapped on the map: its window opened');
        await shot('07-city-tapped');
        await tapDesk(again.exit.x + again.exit.w / 2, again.exit.y + again.exit.h / 2);
        await settle(1500, 20000);
        // a long press on the map: the right button's click
        from = await mark();
        {
            const [x, y] = await at(square[0] - 150, square[1]);
            await touch('touchStart', [[x, y]]);
            await sleep(1000);
            await touch('touchEnd', []);
            await settle(1500, 20000);
            got = buttons(await since(from));
            log('a long press on the map: ' + JSON.stringify(got));
            check(got.some(([m]) => m === '0204') && got.some(([m]) => m === '0205') && !got.some(([m]) => m === '0201'),
                  'a long press on the map: the right button\'s click, no left');
        }
        await shot('08-long-press-map');

        // ---- the menu bar: from one menu to the next (menu.c: track) ----
        // the Game menu opened, then the next title's (Kingdom) by `how`: its
        // first item (Tax Rate) then chosen by keys, its dialog shown
        const GAME_TITLE = [15, 8], KINGDOM_TITLE = [75, 8];
        const mouse = async (type, x, y) => send('Input.dispatchMouseEvent',
            {type, x, y, button: type === 'mouseMoved' ? 'none' : 'left', clickCount: type === 'mouseMoved' ? 0 : 1});
        const nextMenu = async (what, open, how) => {
            from = await mark();
            await open();
            await settle(800, 10000);
            await how();
            await settle(800, 10000);
            await shot('09-menu-' + what.split(' ')[0]);
            await key('ArrowDown', 40);
            await key('Enter', 13);
            await settle(1500, 20000);
            await shot('09-menu-' + what.split(' ')[0] + '-chosen');
            // (the Tax Rate dialog: its three scroll bars, the rates')
            const made = windows(await since(from)).map((w) => w.cls);
            log(`${what}: windows made ${JSON.stringify(made)}`);
            check(made.filter((c) => c === 'MSScrollBarClass').length === 3,
                  `${what}: the Kingdom menu opened from the Game menu (Tax Rate chosen in it: its dialog)`);
            await key('Enter', 13);     // (its OK)
            await settle(1500, 20000);
        };
        const clickDesk = async (x, y) => {
            const p = await at(x, y);
            await mouse('mouseMoved', ...p);
            await mouse('mousePressed', ...p);
            await mouse('mouseReleased', ...p);
        };
        await nextMenu('mouse moved onto the next title (no button)', () => clickDesk(...GAME_TITLE),
                       async () => mouse('mouseMoved', ...await at(...KINGDOM_TITLE)));
        await nextMenu('Right arrow', () => clickDesk(...GAME_TITLE), () => key('ArrowRight', 39));
        await nextMenu('next title tapped', () => tapDesk(...GAME_TITLE), () => tapDesk(...KINGDOM_TITLE));
        check(await running() && !errors.length, 'the game still running, nothing thrown');
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
