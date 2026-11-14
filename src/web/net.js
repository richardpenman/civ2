// Network play in the browser (host.h's host_net_*, as src/host/host_net.c
// gives them natively): browsers cannot open TCP connections, so each goes
// through a relay (tools/webrelay.py) over a WebSocket: connecting to a
// game, ws://RELAY/connect?host=&port=; hosting one, ws://RELAY/listen?port=
// (the relay listens for the browser and passes each player's connection
// over that WebSocket, its bytes marked with the connection's number).
// The semantics are host_net.c's non-blocking sockets': a connect goes on
// while the game polls its state, a recv with nothing yet is "would block"
// (-1), 0 once the peer has closed, -2 when the connection is lost.  The
// game polls them from its message loop, which gives the page its turns
// (host.js), so the WebSockets' events come in between.  UDP (the local
// network's game announcements, port 4994) cannot pass: its socket opens
// but sends nowhere and receives nothing (a LAN game lists no games; join
// by address, the game's "Internet" games).

export const ipText = (ip) => [ip >>> 24, (ip >>> 16) & 255, (ip >>> 8) & 255, ip & 255].join('.');
export const ipNumber = (s) => {
    const m = /^(\d+)\.(\d+)\.(\d+)\.(\d+)$/.exec(s || '');
    return m ? ((+m[1] << 24) | (+m[2] << 16) | (+m[3] << 8) | +m[4]) >>> 0 : 0;
};

// relay: 'ws://host:port' (or '' / null: no network play); WS: the
// WebSocket class (the tests give node's) -> the calls, on plain values
export function makeNet(relay, WS = globalThis.WebSocket) {
    relay = (relay || '').replace(/\/+$/, '');
    const socks = [];
    let localAddr = '127.0.0.1';
    const add = (s) => {
        let h = socks.indexOf(null);
        if (h < 0)
            h = socks.push(null) - 1;
        socks[h] = s;
        return h;
    };
    const get = (h) => (h >= 0 && h < socks.length ? socks[h] : null);
    const open = (path) => {
        const ws = new WS(relay + path);
        ws.binaryType = 'arraybuffer';
        return ws;
    };
    // (the address the others reach this machine's games at: the relay's)
    if (relay && WS) {
        try {
            const ws = open('/info');
            ws.onmessage = (e) => {
                try {
                    const m = JSON.parse(e.data);
                    if (m.addr)
                        localAddr = m.addr;
                } catch (err) { /* not ours */ }
            };
            ws.onerror = () => {};
        } catch (e) { /* no relay there */ }
    }
    const readable = (s, n) => {
        if (!s.rx.length)
            return null;
        const out = new Uint8Array(Math.min(n, s.rxSize));
        let k = 0;
        while (k < out.length) {
            const c = s.rx[0], take = Math.min(c.length - s.rxOff, out.length - k);
            out.set(c.subarray(s.rxOff, s.rxOff + take), k);
            k += take;
            s.rxOff += take;
            if (s.rxOff === c.length) {
                s.rx.shift();
                s.rxOff = 0;
            }
        }
        s.rxSize -= k;
        return out;
    };
    const received = (s, bytes) => {
        if (bytes.length) {
            s.rx.push(bytes);
            s.rxSize += bytes.length;
        }
    };
    const stream = (extra) => Object.assign({kind: 'tcp', state: 'new', rx: [], rxOff: 0, rxSize: 0, peerClosed: false},
                                            extra);
    const dropListener = (l) => {
        if (l.ws && l.closed && !l.children.size) {
            l.ws.close(1000);
            l.ws = null;
        }
    };

    return {
        get relay() {
            return relay;
        },
        localAddr: () => localAddr,
        udpOpen: () => add({kind: 'udp'}),
        udpSend: (h, n) => (get(h) ? n : -1),
        udpRecv: () => -1,
        tcpOpen: (port) => add(stream({port})),
        // (the relay listens: whether it could is known later; a listener
        // that could not just never accepts)
        tcpListen: (h) => {
            const s = get(h);
            if (!s || s.kind !== 'tcp' || !relay || !s.port)
                return -1;
            s.kind = 'listener';
            s.pending = [];
            s.children = new Map();
            const ws = s.ws = open('/listen?port=' + s.port);
            ws.onmessage = (e) => {
                if (typeof e.data !== 'string') {
                    const d = new Uint8Array(e.data);
                    if (d.length < 4)
                        return;
                    const c = s.children.get(new DataView(d.buffer).getUint32(0, true));
                    if (c)
                        received(c, d.slice(4));
                    return;
                }
                const m = JSON.parse(e.data);
                if (m.listening !== undefined) {
                    s.state = 'listening';
                    if (m.addr)
                        localAddr = m.addr;
                } else if (m.accept !== undefined) {
                    const c = stream({kind: 'child', state: 'open', parent: s, id: m.accept, ip: ipNumber(m.ip)});
                    s.children.set(m.accept, c);
                    if (!s.closed)
                        s.pending.push(c);
                } else if (m.closed !== undefined) {
                    const c = s.children.get(m.closed);
                    if (c) {
                        c.peerClosed = true;
                        s.children.delete(m.closed);
                    }
                }
            };
            ws.onclose = () => {
                s.state = 'closed';
                for (const c of s.children.values())
                    c.state = 'error';
                s.children.clear();
                s.ws = null;
            };
            ws.onerror = () => {};
            return 0;
        },
        // -> [handle, ip] or -1
        tcpAccept: (h) => {
            const s = get(h);
            if (!s || s.kind !== 'listener' || !s.pending.length)
                return -1;
            const c = s.pending.shift();
            return [add(c), c.ip];
        },
        tcpConnect: (h, ip, port) => {
            const s = get(h);
            if (!s || s.kind !== 'tcp' || s.ws || !relay)
                return -1;
            s.state = 'connecting';
            let ws;
            try {
                ws = s.ws = open(`/connect?host=${ipText(ip)}&port=${port}`);
            } catch (e) {
                s.state = 'error';
                return 0;
            }
            ws.onmessage = (e) => {
                if (typeof e.data === 'string') {
                    if (JSON.parse(e.data).connected)
                        s.state = 'open';
                } else {
                    received(s, new Uint8Array(e.data));
                }
            };
            ws.onclose = (e) => {
                if (s.state === 'open' && e.code === 1000)
                    s.peerClosed = true;
                else
                    s.state = 'error';
            };
            ws.onerror = () => {};
            return 0;
        },
        tcpState: (h) => {
            const s = get(h);
            if (!s || s.state === 'error')
                return -1;
            if (s.state === 'connecting')
                return 0;
            return 1;
        },
        // bytes taken, -1 would block, -2 the connection is dead
        tcpSend: (h, bytes) => {
            const s = get(h);
            if (!s)
                return -2;
            if (s.kind === 'child') {
                const ws = s.parent.ws;
                if (s.state !== 'open' || s.peerClosed || !ws || ws.readyState !== 1)
                    return -2;
                const msg = new Uint8Array(4 + bytes.length);
                new DataView(msg.buffer).setUint32(0, s.id, true);
                msg.set(bytes, 4);
                ws.send(msg);
                return bytes.length;
            }
            if (s.state === 'connecting')
                return -1;
            if (s.state !== 'open' || s.peerClosed || !s.ws || s.ws.readyState !== 1)
                return -2;
            s.ws.send(bytes.slice());
            return bytes.length;
        },
        // -> Uint8Array (read), 0 the peer closed, -1 nothing yet, -2 error
        tcpRecv: (h, n) => {
            const s = get(h);
            if (!s || s.kind === 'listener' || s.kind === 'udp')
                return -2;
            const got = readable(s, n);
            if (got)
                return got;
            if (s.peerClosed)
                return 0;
            if (s.state === 'error')
                return -2;
            return -1;
        },
        close: (h) => {
            const s = get(h);
            if (!s)
                return;
            socks[h] = null;
            if (s.kind === 'child') {
                const l = s.parent;
                if (l.children.delete(s.id) && l.ws && l.ws.readyState === 1)
                    l.ws.send(JSON.stringify({close: s.id}));
                dropListener(l);
            } else if (s.kind === 'listener') {
                s.closed = true;
                s.pending = [];
                if (s.ws && s.ws.readyState === 1)
                    s.ws.send(JSON.stringify({unlisten: true}));
                dropListener(s);
            } else if (s.ws) {
                s.ws.onclose = null;
                s.ws.close(1000);
            }
        },
    };
}
