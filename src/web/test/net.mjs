// The browser's network play (src/web/net.js) through the relay
// (tools/webrelay.py), under node: its WebSocket, real TCP on this machine.
// Joining: a TCP server stands for a game's host; hosting: TCP clients
// stand for the other players; and what the relay refuses.
//   tools/toolchain/node/bin/node src/web/test/net.mjs
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer, connect} from 'node:net';
import {join, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {makeNet, ipNumber, ipText} from '../net.js';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..', '..');
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
async function until(f, ms = 5000, what = 'a condition') {
    const end = Date.now() + ms;
    for (;;) {
        const v = f();
        if (v !== false && v !== -1 && v !== undefined && v !== null)
            return v;
        if (Date.now() > end)
            throw new Error('timed out: ' + what);
        await sleep(10);
    }
}
const freePort = () => new Promise((resolve) => {
    const s = createServer().listen(0, '127.0.0.1', () => {
        const p = s.address().port;
        s.close(() => resolve(p));
    });
});

assert.equal(ipText(0x7f000001), '127.0.0.1');
assert.equal(ipNumber('192.168.1.20'), 0xc0a80114);
assert.equal(ipNumber('host'), 0);

// no relay: no network play (as without the network)
const none = makeNet('');
assert.equal(none.tcpConnect(none.tcpOpen(0), 0x7f000001, 4993), -1);
assert.equal(none.tcpListen(none.tcpOpen(4993)), -1);

const wsPort = await freePort(), gamePort = await freePort(), hostPort = await freePort();
const relay = spawn('python3', [join(ROOT, 'tools', 'webrelay.py'), '--port', String(wsPort),
                                '--ports', `${gamePort},${hostPort}`, '-v'], {stdio: ['ignore', 'pipe', 'inherit']});
await new Promise((resolve, reject) => {
    relay.stdout.on('data', (d) => /webrelay: ws:/.test(d) && resolve());
    relay.on('exit', (c) => reject(new Error('the relay stopped: ' + c)));
});
try {
    const net = makeNet(`ws://127.0.0.1:${wsPort}`);
    await until(() => net.localAddr() === '127.0.0.1' || null, 3000, 'the relay\'s address');

    // joining: a host on gamePort that echoes, upper-cased, and closes on "bye"
    const server = createServer((c) => {
        c.on('data', (d) => {
            c.write(d.toString().toUpperCase());
            if (/bye/.test(d))
                c.end();
        });
    }).listen(gamePort, '127.0.0.1');
    let h = net.tcpOpen(0);
    assert.equal(net.tcpConnect(h, 0x7f000001, gamePort), 0);
    assert.equal(net.tcpState(h), 0);                       // connecting
    assert.equal(net.tcpSend(h, new Uint8Array([1])), -1);  // (would block)
    assert.equal(net.tcpRecv(h, 10), -1);
    await until(() => net.tcpState(h) === 1 || null, 5000, 'connected');
    assert.equal(net.tcpSend(h, new TextEncoder().encode('hello')), 5);
    let got = await until(() => net.tcpRecv(h, 3), 5000, 'the echo');
    assert.equal(new TextDecoder().decode(got), 'HEL');      // (cut to what was asked)
    got = await until(() => net.tcpRecv(h, 100));
    assert.equal(new TextDecoder().decode(got), 'LO');
    net.tcpSend(h, new TextEncoder().encode('bye'));
    got = await until(() => net.tcpRecv(h, 100));
    assert.equal(new TextDecoder().decode(got), 'BYE');
    assert.equal(await until(() => (net.tcpRecv(h, 100) === 0 ? 0 : null)), 0);     // the host closed
    assert.equal(net.tcpSend(h, new Uint8Array([1])), -2);
    net.close(h);
    server.close();

    // refused: a port not allowed, an address not allowed, nobody listening
    for (const [ip, port] of [[0x7f000001, 22], [ipNumber('8.8.8.8'), gamePort], [0x7f000001, gamePort]]) {
        h = net.tcpOpen(0);
        assert.equal(net.tcpConnect(h, ip, port), 0);
        await until(() => net.tcpState(h) === -1 || null, 20000, `refused ${ipText(ip)}:${port}`);
        assert.equal(net.tcpRecv(h, 1), -2);
        net.close(h);
    }
    const bad = net.tcpOpen(23);
    assert.equal(net.tcpListen(bad), 0);                   // (said later: never accepts)
    await sleep(300);
    assert.equal(net.tcpAccept(bad), -1);
    net.close(bad);

    // hosting: the relay listens on hostPort; two players connect
    const l = net.tcpOpen(hostPort);
    assert.equal(net.tcpListen(l), 0);
    assert.equal(net.tcpAccept(l), -1);
    await sleep(500);                   // (the relay listening)
    const players = [];
    for (let i = 0; i < 2; i++) {
        const c = connect(hostPort, '127.0.0.1');
        await new Promise((r, j) => (c.once('connect', r), c.once('error', j)));
        players.push(c);
    }
    const [a, ipA] = await until(() => net.tcpAccept(l), 5000, 'the first player');
    const [b] = await until(() => net.tcpAccept(l), 5000, 'the second player');
    assert.equal(ipA, 0x7f000001);
    assert.equal(net.tcpState(a), 1);
    const heard = ['', ''];
    players.forEach((p, i) => p.on('data', (d) => (heard[i] += d)));
    net.tcpSend(a, new TextEncoder().encode('to A'));
    net.tcpSend(b, new TextEncoder().encode('to B'));
    await until(() => (heard[0] === 'to A' && heard[1] === 'to B') || null, 5000, 'the players hearing');
    players[1].write('from B');
    got = await until(() => net.tcpRecv(b, 100), 5000, 'B\'s bytes');
    assert.equal(new TextDecoder().decode(got), 'from B');
    assert.equal(net.tcpRecv(a, 100), -1);
    // the listener closed: the players stay; a player leaves; one is closed
    net.close(l);
    players[1].end();
    assert.equal(await until(() => (net.tcpRecv(b, 100) === 0 ? 0 : null)), 0);
    net.close(b);
    const aClosed = new Promise((r) => players[0].on('end', r));
    players[0].write('still here');
    got = await until(() => net.tcpRecv(a, 100), 5000, 'A after the listener closed');
    assert.equal(new TextDecoder().decode(got), 'still here');
    net.close(a);
    await aClosed;
    players.forEach((p) => p.destroy());
    console.log('src/web network tests passed');
} finally {
    relay.kill();
}
