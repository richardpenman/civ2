// Looping sounds in the browser version (make web-test): the replays of
// harness/sound (tests/sound_loops_test.sh runs them on the desktop) run in
// the browser with audio (CIV2_TEST_AUDIO=1; the browser's autoplay policy
// off: cdp.mjs browser()), the game's sounds played through the page's
// WebAudio (host.js).  Each script's `sounds LABEL` logs what the game's
// mixer has and what host.js has scheduled (src/auto/sound.cpp:
// snd_log_state); checked as the desktop's test checks them:
//   noloop    no looping sound playing (the menus' theme MENULOOP, the
//             diplomacy room's drums DRUMxL or any other), and no more
//             queued on the page than the game's buffers hold;
//   drums     the diplomacy room's drums looping;
//   theme     the menus' theme looping;
//   greeting  the drums not started yet.
// Whatever the label, no looping sound twice in the mixer at once (two
// copies of one loop: in a `sounds` line, or in the mixer's log of what it
// mixes, "sound: mixing ID ...", of an ID seen looping).
// The cases: a new game set up to its first turn without the original's
// files (HD graphics, OpenCiv2's own sounds: runs without the game, as in
// CI); with the game ($CIV2_GAME or ~/Civ2) also a new game with Classic
// graphics, a game loaded (Classic, HD), the diplomacy room's talks ended
// with Escape (Classic, HD, without the install), and the AIs coming to the
// player.  Then a new game played as a player plays it (not a replay: the
// real clock, the keys; without the install, and with Classic graphics
// where the game is there): the theme stopped when the menus close and
// not mixed again (the game's log, CIV2_LOG_SOUND), and at the first
// turn's map no more of the game's sound scheduled ahead on the page's
// WebAudio than its buffers hold (the music, a whole track, not counted).
// The game's
// files given to the page as web-parity.mjs gives them (the store filled
// as the page's own import keeps them).  Output in run/web-sound/NAME
// (hook.log); exit status 1 when a case fails.
//
//   tools/toolchain/node/bin/node src/web/test/sound.mjs [NAME...]

import {readFileSync, writeFileSync, mkdirSync, readdirSync, rmSync, existsSync, statSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {ROOT, WEB, serve, browser, tmpDir, clickPlay} from './cdp.mjs';

const OUT = join(ROOT, 'run', 'web-sound');
const GAME = process.env.CIV2_GAME || join(homedir(), 'Civ2');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const log = (s) => console.log(s);
const NOINSTALL = {CIV2_NO_GRAPHICS: '1', CIV2_NO_TEXT: '1', CIV2_NO_RULES: '1', CIV2_NO_SOUND: '1'};
// the game's buffers (8 of some 186 ms in the game, snd_wave_set_buffer_count;
// 12 once a video's sound has played): at most this much queued on the
// page with no loop playing
const QUEUED_MAX_MS = 3000;

// {name, script (harness/sound/NAME.txt), env, needs: [what of the game]}
const CASES = [
    {name: 'newgame-noinstall', script: 'newgame', env: {...NOINSTALL, CIV2_TEST_HD: '1'}, needs: []},
    {name: 'newgame-classic', script: 'newgame', env: {}, needs: ['classic']},
    {name: 'loadgame-classic', script: 'loadgame', env: {}, needs: ['classic', 'save']},
    {name: 'loadgame-hd', script: 'loadgame', env: {CIV2_TEST_HD: '1'}, needs: ['save']},
    {name: 'talk-escape-classic', script: 'talk-escape', env: {}, needs: ['classic', 'save']},
    {name: 'talk-escape-hd', script: 'talk-escape', env: {CIV2_TEST_HD: '1'}, needs: ['save']},
    {name: 'talk-escape-noinstall', script: 'talk-escape', env: {...NOINSTALL, CIV2_TEST_HD: '1'}, needs: ['save']},
    {name: 'talk-ai-classic', script: 'talk-ai', env: {}, needs: ['classic', 'save']},
    // played, not replayed: the page as a player has it (real time, the
    // start screen's click), a new game set up with the keys
    {name: 'play-newgame-noinstall', play: true, env: {...NOINSTALL}, hd: '1', needs: []},
    {name: 'play-newgame-classic', play: true, env: {}, hd: '0', needs: ['classic']},
];
// (harness/sound/talk-early.txt, an emissary sent away while the greeting
// plays, is the desktop's only: it needs the keys taken within the
// greeting's seconds, on the real clock, CIV2_REALTIME, which the
// browser's slower start of the room misses)

// ---- the game's files, as tools/native-run.sh gives them (as web-parity.mjs's) ----

function gameFiles(env) {
    const textNames = new Set(readdirSync(join(ROOT, 'data', 'native', 'text'))
        .filter((n) => /\.TXT$/i.test(n)).map((n) => n.toLowerCase()));
    const files = [];   // [name in the game's folder, path on disk]
    const walk = (dir, rel) => {
        for (const n of readdirSync(dir).sort()) {
            const p = join(dir, n), r = rel ? rel + '/' + n : n, lo = n.toLowerCase();
            const top = !rel;
            if (statSync(p).isDirectory()) {
                if (top && ['kings', 'video', 'music'].includes(lo))
                    continue;
                if (top && env.CIV2_NO_SOUND && lo === 'sound')
                    continue;
                walk(p, r);
                continue;
            }
            if (/\.log$/i.test(n) || (top && (lo.endsWith('.dat') || lo === 'civ.ini')))
                continue;
            if (top && env.CIV2_NO_GRAPHICS && (/\.(gif|dll)$/.test(lo) || lo === 'civ2.exe'))
                continue;
            if (top && env.CIV2_NO_RULES && lo === 'rules.txt')
                continue;
            if (top && env.CIV2_NO_TEXT && textNames.has(lo))
                continue;
            files.push([r, p]);
        }
    };
    if (existsSync(GAME))
        walk(GAME, '');
    files.push(['CIV2.DAT', join(ROOT, 'harness', 'CIV2.DAT')]);
    if (existsSync(join(ROOT, 'harness', 'CIV.INI')))
        files.push(['CIV.INI', join(ROOT, 'harness', 'CIV.INI')]);
    return files;
}

// What of the game's there: the install's pictures, the autosave
function gameHas() {
    const names = existsSync(GAME) ? readdirSync(GAME).map((n) => n.toLowerCase()) : [];
    return {classic: names.includes('civ2.exe'), save: names.includes('ca_auto.sav')};
}

// ---- the check: hook.log's `sounds` as their labels want -> the problems ----

export function checkSounds(text, report = () => {}) {
    const lines = text.split(/\r?\n/), probs = [];
    let label = null, seen = 0;
    probs.push(...loopsTwice(lines));
    for (const l of lines) {
        const m = /^t=(\d+): sounds ?(\S*)/.exec(l);
        if (m) {
            label = [m[2], m[1]];
            continue;
        }
        if (l.startsWith('replay: waitsound done (timed out)'))
            probs.push('a sound that ends still playing after 30 s');
        if (label && l.startsWith('sound: state:')) {
            const [what, t] = label;
            label = null;
            seen++;
            const loops = +/loops (\d+)/.exec(l)[1], queued = +(/queued (\d+) ms/.exec(l) || [0, 0])[1];
            report(`  t=${t} ${what || '-'}: ${l.slice('sound: state:'.length).trim()}`);
            const ids = [...l.matchAll(/ (\d+\([^,)]*),loop/g)].map((x) => x[1]);
            const dup = [...new Set(ids.filter((i) => ids.indexOf(i) !== ids.lastIndexOf(i)))];
            if (dup.length)
                probs.push(`${what || '-'} at t=${t} ms: a loop twice at once (${dup.map((d) => d + ')').join(', ')}): ${l}`);
            if ((what === 'noloop' || what === 'greeting') && loops)
                probs.push(`${what} at t=${t} ms: a sound loops: ${l}`);
            else if (what === 'noloop' && queued > QUEUED_MAX_MS)
                probs.push(`noloop at t=${t} ms: ${queued} ms queued on the page (at most ${QUEUED_MAX_MS}): ${l}`);
            else if (what === 'drums' && !/\(DRUM..,loop/.test(l))
                probs.push(`drums at t=${t} ms: the diplomacy room's drums not looping: ${l}`);
            else if (what === 'theme' && !l.includes('(MENULOOP,loop'))
                probs.push(`theme at t=${t} ms: the menus' theme not looping: ${l}`);
        }
    }
    if (!lines.some((l) => l.startsWith('replay: script finished')))
        probs.push('the script did not finish');
    if (!seen)
        probs.push('no sound state logged (no audio?)');
    return probs;
}

// A looping sound mixed twice at once: in the mixer's log ("sound: mixing
// ID ..."), an ID seen looping (in a `sounds` line, or `also`: the menus'
// theme, 107) twice in one line -> the problems
export function loopsTwice(lines, also = []) {
    const loopIds = new Set(also);
    for (const l of lines)
        if (l.startsWith('sound: state:'))
            for (const m of l.matchAll(/ (\d+)\([^,)]*,loop/g))
                loopIds.add(m[1]);
    const probs = [], twice = new Set();
    for (const l of lines) {
        if (!l.startsWith('sound: mixing'))
            continue;
        const ids = l.split(/\s+/).slice(2);
        for (const i of new Set(ids))
            if (ids.indexOf(i) !== ids.lastIndexOf(i) && loopIds.has(i) && !twice.has(i)) {
                twice.add(i);
                probs.push(`looping sound ${i} mixed twice at once: ${l}`);
            }
    }
    return probs;
}

// ---- the browser ----

async function main() {
    const names = process.argv.slice(2);
    if (!existsSync(join(WEB, 'civ2.wasm')))
        throw new Error('no build/web: make web first');
    const has = gameHas();
    const cases = names.length ? CASES.filter((c) => names.includes(c.name)) : CASES;
    rmSync(OUT, {recursive: true, force: true});
    mkdirSync(OUT, {recursive: true});
    const work = tmpDir();   // (run/web-tmp-*: cdp.mjs)
    let served = new Map();
    const server = await serve((path) => path.startsWith('/__game/') ? served.get(path.slice(8)) : null);
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(work.path, 'profile'), undefined, url);
    const {send} = b;
    b.on((m) => {
        if (m.method === 'Runtime.exceptionThrown') {
            log('  page exception: ' + JSON.stringify(m.params.exceptionDetails).slice(0, 300));
        }
    });
    const evaluate = async (expr) => {
        const r = await send('Runtime.evaluate', {expression: expr, awaitPromise: true, returnByValue: true});
        if (r.exceptionDetails)
            throw new Error('in the page: ' + JSON.stringify(r.exceptionDetails).slice(0, 500));
        return r.result.value;
    };
    const waitFor = async (expr, ms, what) => {
        const end = performance.now() + ms;
        while (performance.now() < end) {
            const v = await evaluate(expr).catch(() => null);
            if (v)
                return v;
            await sleep(250);
        }
        throw new Error('timed out: ' + what);
    };
    const results = [];
    let script = null;
    try {
        await send('Runtime.enable');
        await send('Page.enable');
        await send('Emulation.setDeviceMetricsOverride', {width: 1280, height: 800, deviceScaleFactor: 1, mobile: false});
        for (const c of cases) {
            const lacks = c.needs.filter((n) => !has[n]);
            if (lacks.length) {
                log(`SKIP ${c.name} (the game in ${GAME} lacks: ${lacks.map((n) => n === 'save' ? 'Ca_Auto.SAV' : 'the install\'s pictures').join(', ')})`);
                results.push('SKIP');
                continue;
            }
            const t = performance.now();
            const out = join(OUT, c.name);
            mkdirSync(out, {recursive: true});
            let probs;
            try {
                if (c.play) {
                    log(`${c.name} (a new game played, ${JSON.stringify(c.env)}, ?hd=${c.hd}):`);
                    probs = await playCase(c, out);
                } else {
                    const text = await runCase(c, out);
                    log(`${c.name} (harness/sound/${c.script}.txt, ${JSON.stringify(c.env)}):`);
                    probs = checkSounds(text, log);
                }
            } catch (e) {
                probs = ['the run: ' + e.message];
            }
            const secs = ((performance.now() - t) / 1000).toFixed(0);
            if (probs.length) {
                log(`FAIL ${c.name} (${secs} s; run/web-sound/${c.name}/hook.log)`);
                for (const p of probs)
                    log('    ' + p);
                results.push('FAIL');
            } else {
                log(`PASS ${c.name} (${secs} s)`);
                results.push('PASS');
            }
        }
    } finally {
        await b.close();
        server.close();
        await sleep(300);
        work.remove();
    }
    const n = (k) => results.filter((r) => r === k).length;
    log(`sound.mjs: ${n('PASS')} passed, ${n('FAIL')} failed, ${n('SKIP')} skipped (no looping sound left after the menus or the diplomacy room)`);
    // (page exceptions only logged, as web-parity.mjs: a fetch the page
    // began, cut short by the next case's navigation)
    if (n('FAIL'))
        process.exitCode = 1;

    // One case: the store filled, the page loaded, the replay run with
    // audio -> its hook.log
    // The page's store filled with the case's files (as its own import
    // keeps them), the start screen shown
    async function fillStore(env) {
        const files = gameFiles(env);
        served = new Map(files.map(([, p], i) => [String(i), p]));
        await send('Page.navigate', {url});
        await waitFor('window.civ2Start', 30000, 'the start screen');
        const kept = await evaluate(`(async () => {
            const {openStore} = await import('/store.js');
            const store = await openStore();
            await store.clear();
            const names = ${JSON.stringify(files.map(([n]) => n))};
            let batch = [], size = 0;
            for (let i = 0; i < names.length; i++) {
                const r = await fetch('/__game/' + i);
                if (!r.ok)
                    throw new Error('not served: ' + names[i]);
                const data = new Uint8Array(await r.arrayBuffer());
                batch.push([names[i], data, Date.now()]);
                size += data.length;
                if (size > 16 << 20 || i === names.length - 1) {
                    await store.putMany(batch);
                    batch = [];
                    size = 0;
                }
            }
            await store.setMeta('game', {names, at: Date.now()});
            return store.count();
        })()`);
        if (kept !== files.length)
            throw new Error(`${kept} files in the store, not ${files.length}`);
    }

    async function runCase(c, out) {
        const env = c.env;
        await fillStore(env);
        const vars = {CIV2_SCRIPT: '/replay/script.txt', CIV2_SHOT_DIR: '/replay/out', CIV2_SEED: '7',
                      CIV2_RAND_LOG: '/replay/out/rand.log', CIV2_HOOK_LOG: '/replay/out/hook.log',
                      CIV2_WINDIR: '/tmp/g', CIV2_WINE_TEXT: '1', CIV2_TEST_AUDIO: '1'};
        for (const [k, v] of Object.entries(env))
            if (!/^CIV2_NO_\w+$/.test(k))
                vars[k] = v;
        if (script)
            await send('Page.removeScriptToEvaluateOnNewDocument', {identifier: script});
        ({identifier: script} = await send('Page.addScriptToEvaluateOnNewDocument', {source: `
            window.civ2Test = async (fs, env, args) => {
                const r = await fetch('/__test/sound/${c.script}.txt');
                fs.put('/replay/script.txt', new Uint8Array(await r.arrayBuffer()));
                for (const d of [...fs.dirs])
                    if (d === '/civ2' || d.startsWith('/civ2/'))
                        fs.mkdirs('/tmp/g' + d.slice(5));
                for (const [p, f] of [...fs.files])
                    if (p.startsWith('/civ2/'))
                        fs.files.set('/tmp/g' + p.slice(5), {...f});
                args[1] = '/tmp/g';
                fs.mkdirs('/replay/out');
                Object.assign(env, ${JSON.stringify(vars)});
            };`}));
        await send('Page.navigate', {url: url + '?hd=' + (env.CIV2_TEST_HD || '0')});
        await clickPlay(send);
        await waitFor('/ended|stopped/.test(window.civ2Status || "") && window.civ2Status', 1800000, 'the replay');
        const st = await evaluate('window.civ2Status');
        const text = await evaluate(`(() => { const d = window.civ2Fs.get('/replay/out/hook.log');
            return d ? new TextDecoder('latin1').decode(d) : ''; })()`);
        writeFileSync(join(out, 'hook.log'), text);
        await send('Page.navigate', {url: 'about:blank'});
        if (/stopped/.test(st))
            throw new Error(st);
        return text;
    }

    // A new game played as a player plays it (not a replay: the real
    // clock, the start screen's click, the keys), the game's log of what
    // it mixes (CIV2_LOG_SOUND: snd_log_mixing) kept in the page's file
    // system, and the page's WebAudio sources watched: once the first
    // turn's map is shown, the menus' theme (MENULOOP, 107) stopped and
    // never mixed again, and no more scheduled ahead on the page than the
    // game's buffers hold -> the problems
    async function playCase(c, out) {
        await fillStore(c.env);
        if (script)
            await send('Page.removeScriptToEvaluateOnNewDocument', {identifier: script});
        ({identifier: script} = await send('Page.addScriptToEvaluateOnNewDocument', {source: `
            window.civ2Test = async (fs, env) => {
                fs.mkdirs('/play');
                Object.assign(env, {CIV2_LOG_SOUND: '1', CIV2_HOOK_LOG: '/play/hook.log'});
            };
            (() => {
                const P = window.civ2AudioProbe = {sources: []};
                const start = AudioBufferSourceNode.prototype.start;
                AudioBufferSourceNode.prototype.start = function (when = 0, ...rest) {
                    const at = Math.max(when, this.context.currentTime);
                    P.sources.push({ctx: this.context, end: at + (this.buffer ? this.buffer.duration : 0), node: this});
                    this.addEventListener('ended', () => { P.sources = P.sources.filter((s) => s.node !== this); });
                    return start.call(this, when, ...rest);
                };
                // (one stopped: not to be heard, whether it has begun or not)
                const stop = AudioBufferSourceNode.prototype.stop;
                AudioBufferSourceNode.prototype.stop = function (...a) {
                    P.sources = P.sources.filter((s) => s.node !== this);
                    return stop.apply(this, a);
                };
                // (seconds scheduled ahead of now on the page's contexts, of
                // the game's sound: host_audio_queue's buffers, each a
                // fraction of a second; not the music, host_music_play's
                // whole track)
                P.ahead = () => Math.max(0, ...P.sources.filter((s) => s.node.buffer && s.node.buffer.duration < 2)
                    .map((s) => s.end - s.ctx.currentTime));
            })();`}));
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
                    return;
            }
        };
        const key = async (code, vk) => {
            for (const type of ['rawKeyDown', 'keyUp'])
                await send('Input.dispatchKeyEvent', {type, code, key: code, windowsVirtualKeyCode: vk});
        };
        const enter = async () => {
            await key('Enter', 13);
            await settle(1500, 20000);
        };
        const gameLog = () => evaluate(`(() => { const d = window.civ2Fs && window.civ2Fs.get('/play/hook.log');
            return d ? new TextDecoder('latin1').decode(d) : ''; })()`);
        await send('Page.navigate', {url: url + '?hd=' + c.hd});
        await clickPlay(send);
        await waitFor('/Starting/.test(window.civ2Status || "")', 300000, 'the start');
        await waitFor('window.civ2Timing && window.civ2Timing.frame', 60000, 'the first frame');
        await settle(3000, 60000);
        await evaluate('document.getElementById("screen").focus()');
        const menu = await gameLog();
        const aheadMenu = await evaluate('civ2AudioProbe.ahead()');
        // Begin a New Game, a small world, the rest as offered; the world
        // made; its announcements
        await enter();
        await key('ArrowUp', 38);
        for (let i = 0; i < 9; i++)
            await enter();
        const aheadSetup = await evaluate('civ2AudioProbe.ahead()');
        await settle(5000, 900000);
        const aheadWorld = await evaluate('civ2AudioProbe.ahead()');
        log(`  scheduled ahead on the page: ${aheadMenu.toFixed(2)} s at the main menu, ${aheadSetup.toFixed(2)} s once set up, ` +
            `${aheadWorld.toFixed(2)} s once the world is made`);
        for (let i = 0; i < 3; i++) {
            await key('Enter', 13);
            await sleep(3000);
        }
        await sleep(5000);
        const ahead = await evaluate('civ2AudioProbe.ahead()');
        const text = await gameLog();
        writeFileSync(join(out, 'hook.log'), text);
        const r = await send('Page.captureScreenshot', {format: 'png'});
        writeFileSync(join(out, 'map.png'), Buffer.from(r.data, 'base64'));
        await send('Page.navigate', {url: 'about:blank'});
        // (what is mixed, logged when it changes: "sound: mixing 107 106")
        const mixing = text.split('\n').filter((l) => l.startsWith('sound: mixing'));
        const theme = (l) => / 107( |$)/.test(l);
        const stopAt = mixing.findIndex((l, i) => i > 0 && theme(mixing[i - 1]) && !theme(l));
        log(`  the theme at the main menu's first frames: ${menu.split('\n').some(theme) ? 'mixed' : 'not yet mixed'}; ` +
            `when they close: ${stopAt < 0 ? '(no change)' : mixing.slice(stopAt - 1, stopAt + 2).join(' / ')}; ` +
            `at the map: ${mixing.slice(-1)[0] || '-'}; ${ahead.toFixed(2)} s scheduled ahead on the page`);
        const probs = [];
        // (the theme mixed in the menus, then not: stopped; OpenCiv2's own
        // may come a little after the main menu's first frame)
        if (!mixing.some(theme))
            probs.push('the menus\' theme (107) never mixed (no sound?)');
        else if (stopAt < 0)
            probs.push('the menus\' theme (107) not stopped when the menus close');
        else if (mixing.slice(stopAt).some(theme))
            probs.push('the menus\' theme (107) mixed again after it stopped: ' + mixing.slice(stopAt).find(theme));
        probs.push(...loopsTwice(mixing, ['107']));
        if (ahead * 1000 > QUEUED_MAX_MS)
            probs.push(`${Math.round(ahead * 1000)} ms scheduled ahead on the page at the map (at most ${QUEUED_MAX_MS})`);
        return probs;
    }
}

main().catch((e) => {
    console.error(e);
    process.exit(1);
});
