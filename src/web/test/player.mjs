// A browser player for the network tests (tools/xdnet/webnet.py): the
// browser version in Chrome's headless shell, the game's files copied in as
// the smoke test does, the game started with a relay (tools/webrelay.py)
// and the replay harness's CIV2.DAT and CIV.INI (as tools/xdnet/
// start_ours.sh gives a native game), its XDaemon log (CIV2_XDLOG) and the
// platform's log copied to OUT/xdlog-ROLE.txt and OUT/ours-ROLE.log as they
// grow.  Driven by JSON lines on stdin, each answered on stdout:
//   {"cmd": "key", "key": "ctrl+n"}      (xdotool's names: Return, Down, KP_6, a ...)
//   {"cmd": "type", "text": "Bob"}
//   {"cmd": "click", "x": 79, "y": 508}
//   {"cmd": "shot", "path": "/abs/x.png"}
//   {"cmd": "stable", "quiet": 2000, "tol": 600, "most": 30000}   (the screen unchanged for quiet ms,
//                                     but for tol pixels)
//   {"cmd": "status"} -> {"alive": bool, "status": text}
//   {"cmd": "quit"}
//   tools/toolchain/node/bin/node src/web/test/player.mjs OUT ROLE RELAY [GAME_DIR]
import {writeFileSync, appendFileSync, mkdirSync, mkdtempSync, rmSync} from 'node:fs';
import {join} from 'node:path';
import {homedir} from 'node:os';
import {createInterface} from 'node:readline';
import {ROOT, zipFolder, serve, browser} from './cdp.mjs';

const [OUT, ROLE, RELAY, GAME = process.env.CIV2_GAME || join(homedir(), 'Civ2')] = process.argv.slice(2);
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const say = (o) => process.stdout.write(JSON.stringify(o) + '\n');
const note = (s) => process.stderr.write(`player ${ROLE}: ${s}\n`);

const KEYS = {Return: ['Enter', 13, '\r'], Escape: ['Escape', 27], Down: ['ArrowDown', 40], Up: ['ArrowUp', 38],
              Left: ['ArrowLeft', 37], Right: ['ArrowRight', 39], Tab: ['Tab', 9], BackSpace: ['Backspace', 8],
              space: ['Space', 32, ' '], Home: ['Home', 36], End: ['End', 35]};

async function main() {
    mkdirSync(OUT, {recursive: true});
    const tmp = mkdtempSync(join(ROOT, 'run', 'web-player-'));
    const zip = join(tmp, 'Civ2.zip');
    zipFolder(GAME, zip);
    const server = await serve();
    const url = `http://127.0.0.1:${server.address().port}/`;
    const b = await browser(join(tmp, 'profile'), [1024, 768]);
    const {send} = b;
    b.on((m) => {
        if (m.method === 'Runtime.exceptionThrown')
            note('page exception: ' + JSON.stringify(m.params.exceptionDetails).slice(0, 300));
    });
    const evaluate = async (expr) => (await send('Runtime.evaluate', {expression: expr, awaitPromise: true,
                                                                       returnByValue: true})).result.value;
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
    const keyEvent = async (code, vk, key, modifiers = 0, text) => {
        await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code, key, windowsVirtualKeyCode: vk, modifiers});
        if (text)
            await send('Input.dispatchKeyEvent', {type: 'char', text, key, modifiers});
        await send('Input.dispatchKeyEvent', {type: 'keyUp', code, key, windowsVirtualKeyCode: vk, modifiers});
    };
    const charKey = (ch) => {
        if (/[a-z]/i.test(ch))
            return ['Key' + ch.toUpperCase(), ch.toUpperCase().charCodeAt(0), ch];
        if (/[0-9]/.test(ch))
            return ['Digit' + ch, ch.charCodeAt(0), ch];
        return {'.': ['Period', 190, '.'], ' ': ['Space', 32, ' '], '_': ['Minus', 189, '_'], '-': ['Minus', 189, '-']}[ch];
    };
    const key = async (name) => {
        const parts = name.split('+'), k = parts.pop();
        let mods = 0;
        for (const p of parts) {
            const [code, vk, bit] = p === 'ctrl' ? ['ControlLeft', 17, 2] : p === 'shift' ? ['ShiftLeft', 16, 8] : ['AltLeft', 18, 1];
            mods |= bit;
            await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code, key: p, windowsVirtualKeyCode: vk, modifiers: mods});
        }
        const m = /^KP_(\d)$/.exec(k);
        if (m)
            await keyEvent('Numpad' + m[1], 0x60 + +m[1], m[1], mods);
        else if (KEYS[k])
            await keyEvent(KEYS[k][0], KEYS[k][1], KEYS[k][0], mods);
        else if (k.length === 1)
            await keyEvent(...charKey(k), mods);
        else
            throw new Error('a key not known here: ' + name);
        for (const p of parts.reverse()) {
            const [code, vk] = p === 'ctrl' ? ['ControlLeft', 17] : p === 'shift' ? ['ShiftLeft', 16] : ['AltLeft', 18];
            await send('Input.dispatchKeyEvent', {type: 'keyUp', code, key: p, windowsVirtualKeyCode: vk});
        }
    };
    const type = async (text) => {
        for (const ch of text) {
            const shifted = /[A-Z_]/.test(ch);
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'rawKeyDown', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16, modifiers: 8});
            await keyEvent(...charKey(ch), shifted ? 8 : 0);
            if (shifted)
                await send('Input.dispatchKeyEvent', {type: 'keyUp', code: 'ShiftLeft', key: 'Shift', windowsVirtualKeyCode: 16});
            await sleep(30);
        }
    };
    // the screen's pixels that changed since the last look (every 7th): a
    // blinking unit is not a change (as nettest.py's wait_stable allows 600)
    const changed = () => evaluate('(() => { const c = document.getElementById("screen"); if (!c || !c.width) return -1;' +
        ' const d = new Uint32Array(c.getContext("2d").getImageData(0, 0, c.width, c.height).data.buffer);' +
        ' const p = window.__shot, n = Math.ceil(d.length / 7), s = new Uint32Array(n); let k = 0;' +
        ' for (let i = 0, j = 0; i < d.length; i += 7, j++) { s[j] = d[i]; if (!p || p.length !== n || p[j] !== d[i]) k++; }' +
        ' window.__shot = s; return k; })()');
    const stable = async (quiet, tol, most) => {
        const start = performance.now();
        await changed();
        let since = start;
        while (performance.now() - start < most) {
            await sleep(250);
            if (await changed() > tol)
                since = performance.now();
            else if (performance.now() - since >= quiet)
                return true;
        }
        return false;
    };

    await send('Runtime.enable');
    await send('Page.enable');
    await send('Emulation.setDeviceMetricsOverride', {width: 1024, height: 768, deviceScaleFactor: 1, mobile: false});
    await send('Page.navigate', {url: url + '?setup'});
    await waitFor('window.civ2Status', 10000, 'the page');
    const {result} = await send('Runtime.evaluate', {expression: 'document.getElementById("zip")'});
    await send('DOM.setFileInputFiles', {objectId: result.objectId, files: [zip]});
    await waitFor('/kept in this browser/.test(window.civ2Status)', 300000, 'the copy');
    await send('Page.addScriptToEvaluateOnNewDocument', {source: `
        window.civ2Test = async (fs, env) => {
            const get = async (u) => new Uint8Array(await (await fetch(u)).arrayBuffer());
            fs.put('/civ2/CIV2.DAT', await get('/__test/CIV2.DAT'));
            fs.put('/civ2/CIV.INI', await get('/__test/CIV.INI'));
            fs.mkdirs('/out');
            Object.assign(env, {CIV2_XDLOG: '/out/xdlog.txt', CIV2_HOOK_LOG: '/out/hook.log', CIV2_WINDIR: '/civ2'});
        };`});
    await send('Page.navigate', {url: url + '?relay=' + encodeURIComponent(RELAY)});
    await waitFor('window.civ2Status && /Starting/.test(window.civ2Status)', 60000, 'the start');
    // the logs, copied out as they grow
    const logs = [['/out/xdlog.txt', join(OUT, `xdlog-${ROLE}.txt`)], ['/out/hook.log', join(OUT, `ours-${ROLE}.log`)]];
    const copied = logs.map(() => 0);
    logs.forEach(([, to]) => writeFileSync(to, ''));
    let busy = false;
    const sync = async () => {
        if (busy)
            return;
        busy = true;
        try {
            for (let i = 0; i < logs.length; i++) {
                const b64 = await evaluate(`(() => { const d = window.civ2Fs && window.civ2Fs.get('${logs[i][0]}');
                    if (!d || d.length <= ${copied[i]}) return ''; const p = d.subarray(${copied[i]}); let s = '';
                    for (let k = 0; k < p.length; k += 32768) s += String.fromCharCode(...p.subarray(k, k + 32768));
                    return btoa(s); })()`);
                if (b64) {
                    const data = Buffer.from(b64, 'base64');
                    appendFileSync(logs[i][1], data);
                    copied[i] += data.length;
                }
            }
        } finally {
            busy = false;
        }
    };
    const timer = setInterval(() => sync().catch(() => {}), 500);
    say({ready: true});
    const lines = createInterface({input: process.stdin});
    try {
        for await (const line of lines) {
            const c = JSON.parse(line);
            try {
                if (c.cmd === 'key')
                    await key(c.key);
                else if (c.cmd === 'type')
                    await type(c.text);
                else if (c.cmd === 'click') {
                    for (const t of ['mousePressed', 'mouseReleased'])
                        await send('Input.dispatchMouseEvent', {type: t, x: c.x, y: c.y, button: 'left', clickCount: 1});
                } else if (c.cmd === 'shot') {
                    const r = await send('Page.captureScreenshot', {format: 'png'});
                    writeFileSync(c.path, Buffer.from(r.data, 'base64'));
                } else if (c.cmd === 'stable') {
                    say({ok: await stable(c.quiet || 2000, Math.ceil((c.tol ?? 600) / 7), c.most || 30000)});
                    continue;
                } else if (c.cmd === 'status') {
                    await sync();
                    const st = await evaluate('window.civ2Status');
                    say({ok: true, alive: !/ended|stopped/.test(st || ''), status: st});
                    continue;
                } else if (c.cmd === 'quit') {
                    await sync();
                    say({ok: true});
                    break;
                }
                say({ok: true});
            } catch (e) {
                say({ok: false, error: e.message});
            }
        }
    } finally {
        clearInterval(timer);
        await b.close();
        server.close();
        rmSync(tmp, {recursive: true, force: true, maxRetries: 10, retryDelay: 200});
    }
}

main().then(() => process.exit(0), (e) => {
    note(e.stack || e.message);
    say({ready: false, error: e.message});
    process.exit(1);
});
