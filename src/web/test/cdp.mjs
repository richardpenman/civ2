// What the browser version's drivers share (smoke.mjs, player.mjs): a
// stored .zip of the game's folder, build/web served on a local port (and
// the replay harness's files at /__test/), Chrome's headless shell over the
// DevTools protocol (no other dependency).

import {createServer} from 'node:http';
import {spawn} from 'node:child_process';
import {readFileSync, writeFileSync, readdirSync, statSync, existsSync} from 'node:fs';
import {join, relative, extname, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';

export const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..', '..');
export const WEB = join(ROOT, 'build', 'web');
const CHROME = process.env.CHROME || join(ROOT, 'tools', 'toolchain', 'chrome-headless-shell', 'chrome-headless-shell');

// ---- a stored (uncompressed) .zip of the game's folder ----

const CRC = new Int32Array(256).map((_, n) => {
    let c = n;
    for (let k = 0; k < 8; k++)
        c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    return c;
});
function crc32(b) {
    let c = -1;
    for (let i = 0; i < b.length; i++)
        c = CRC[(c ^ b[i]) & 0xff] ^ (c >>> 8);
    return (c ^ -1) >>> 0;
}
export function zipFolder(dir, out) {
    const parts = [], central = [];
    let at = 0;
    const walk = (d) => {
        for (const name of readdirSync(d).sort()) {
            const p = join(d, name);
            if (statSync(p).isDirectory())
                walk(p);
            else {
                const data = readFileSync(p), fname = Buffer.from('Civ2/' + relative(dir, p).split('\\').join('/'));
                const crc = crc32(data);
                const h = Buffer.alloc(30);
                h.writeUInt32LE(0x04034b50, 0); h.writeUInt16LE(10, 4); h.writeUInt32LE(crc, 14);
                h.writeUInt32LE(data.length, 18); h.writeUInt32LE(data.length, 22); h.writeUInt16LE(fname.length, 26);
                const c = Buffer.alloc(46);
                c.writeUInt32LE(0x02014b50, 0); c.writeUInt16LE(20, 4); c.writeUInt16LE(10, 6); c.writeUInt32LE(crc, 16);
                c.writeUInt32LE(data.length, 20); c.writeUInt32LE(data.length, 24); c.writeUInt16LE(fname.length, 28);
                c.writeUInt32LE(at, 42);
                parts.push(h, fname, data);
                central.push(c, fname);
                at += 30 + fname.length + data.length;
            }
        }
    };
    walk(dir);
    const cd = Buffer.concat(central), n = central.length / 2;
    const e = Buffer.alloc(22);
    e.writeUInt32LE(0x06054b50, 0); e.writeUInt16LE(n, 8); e.writeUInt16LE(n, 10);
    e.writeUInt32LE(cd.length, 12); e.writeUInt32LE(at, 16);
    writeFileSync(out, Buffer.concat([...parts, cd, e]));
    return n;
}

// ---- the site ----

const TYPES = {'.html': 'text/html', '.js': 'text/javascript', '.wasm': 'application/wasm', '.gz': 'application/gzip'};
export function serve() {
    const server = createServer((req, res) => {
        const path = decodeURIComponent(new URL(req.url, 'http://x').pathname);
        // (a replay's files: /__test/NAME.txt, CIV2.DAT, CIV.INI from harness/)
        const base = path.startsWith('/__test/') ? join(ROOT, 'harness') : WEB;
        const rel = path.startsWith('/__test/') ? (path.endsWith('.txt') ? 'scripts/' : '') + path.slice(8) : path;
        const p = join(base, rel.replace(/\/$/, '/index.html'));
        if (!p.startsWith(base) || !existsSync(p) || statSync(p).isDirectory()) {
            res.writeHead(404).end();
            return;
        }
        res.writeHead(200, {'Content-Type': TYPES[extname(p)] || 'application/octet-stream'});
        res.end(readFileSync(p));
    });
    return new Promise((resolve) => server.listen(0, '127.0.0.1', () => resolve(server)));
}

// ---- the browser, over the DevTools protocol ----

export async function browser(profile, size = [1280, 800]) {
    const proc = spawn(CHROME, ['--headless', '--no-sandbox', '--disable-gpu', '--remote-debugging-port=0',
                                '--user-data-dir=' + profile, '--window-size=' + size.join(','), '--autoplay-policy=no-user-gesture-required',
                                'about:blank'], {stdio: ['ignore', 'ignore', 'pipe']});
    const port = await new Promise((resolve, reject) => {
        let err = '';
        proc.stderr.on('data', (d) => {
            err += d;
            const m = /DevTools listening on ws:\/\/[^:]+:(\d+)\//.exec(err);
            if (m)
                resolve(+m[1]);
        });
        proc.on('exit', () => reject(new Error('the browser stopped: ' + err)));
    });
    const pages = await (await fetch(`http://127.0.0.1:${port}/json/list`)).json();
    const ws = new WebSocket(pages.find((p) => p.type === 'page').webSocketDebuggerUrl);
    await new Promise((r) => ws.addEventListener('open', r));
    let id = 0;
    const waiting = new Map(), listeners = [];
    ws.addEventListener('message', (m) => {
        const msg = JSON.parse(m.data);
        if (msg.id && waiting.has(msg.id)) {
            const [resolve, reject] = waiting.get(msg.id);
            waiting.delete(msg.id);
            msg.error ? reject(new Error(msg.error.message)) : resolve(msg.result);
        } else if (msg.method)
            listeners.forEach((l) => l(msg));
    });
    const send = (method, params = {}) => new Promise((resolve, reject) => {
        waiting.set(++id, [resolve, reject]);
        ws.send(JSON.stringify({id, method, params}));
    });
    const exited = new Promise((r) => proc.on('exit', r));
    return {proc, send, on: (l) => listeners.push(l), close: async () => { ws.close(); proc.kill(); await exited; }};
}
