#!/usr/bin/env python3
"""A relay for the browser version's network games (README "Browser version").

Browsers cannot open TCP connections; the game's network play is TCP (port
4993: the host listens, the other players connect to it).  This relay
bridges WebSockets to TCP, so that a browser's game (src/web/host.js,
host_net_*) can

  * join a game hosted by anyone (the desktop OpenCiv2, the original game,
    another browser through a relay): ws://RELAY/connect?host=A.B.C.D&port=4993
    -> the relay opens TCP to A.B.C.D:4993; then binary messages are the
    connection's bytes, both ways; a text message {"connected": true} says
    it is open; closing either side closes the other (the WebSocket closed
    with code 1000 when the peer has closed, else 4000-4999 and a reason);
  * host a game: ws://RELAY/listen?port=4993 -> the relay listens on TCP
    port 4993 (--listen-host) for the browser: {"listening": 4993, "addr":
    "a.b.c.d"} (the address the others join at), then for each player who
    connects {"accept": ID, "ip": "a.b.c.d"}, their bytes as binary messages
    of [ID: 4 bytes little-endian][bytes] both ways, {"closed": ID} when they
    have closed; from the browser {"close": ID} closes one, {"unlisten":
    true} stops listening (the connections stay);
  * ask the address the others reach it at: ws://RELAY/info -> {"addr": ...}.

The game's local network discovery (UDP broadcasts, port 4994) cannot pass
through: in the browser, network games are the game's "Internet" games, by
address.

Security (it should not be an open proxy): the WebSocket side listens on
127.0.0.1 unless --host says otherwise; connections are made only to the
game's port (4993; --ports) and only to addresses on this machine and the
private networks (127/8, 10/8, 172.16/12, 192.168/16, 169.254/16; --allow
CIDR adds others, --allow any allows all); games are hosted for browsers on
those ports only, listening on 127.0.0.1 unless --listen-host says
otherwise; --origin limits the pages that may use it (the Origin header:
e.g. --origin http://localhost:8000; default any page); at most --max
connections at a time.  Plain WebSocket (ws://): put it behind a TLS proxy
for wss:// when the page is served over https.

Usage:
  tools/webrelay.py [--host 127.0.0.1] [--port 8765] [--listen-host 127.0.0.1]
                    [--ports 4993] [--allow CIDR ...] [--origin URL ...]
                    [--public-addr A.B.C.D] [--max 32] [-v]
e.g. on the machine of a LAN party, so that its browsers and other machines
can play: tools/webrelay.py --host 0.0.0.0 --listen-host 0.0.0.0, the pages
opened with ?relay=ws://THAT-MACHINE:8765.

Python 3.8+, the standard library only.
"""
import argparse
import asyncio
import base64
import hashlib
import ipaddress
import json
import socket
import struct
import sys
import urllib.parse

GUID = b'258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
PRIVATE = ['127.0.0.0/8', '10.0.0.0/8', '172.16.0.0/12', '192.168.0.0/16', '169.254.0.0/16']
MAX_MESSAGE = 1 << 20

opts = None
active = 0              # TCP connections open (connected and accepted)


def log(*a):
    if opts.verbose:
        print('webrelay:', *a, file=sys.stderr, flush=True)


# ---- WebSocket (RFC 6455), the server's side ----

class Closed(Exception):
    pass


class WebSocket:
    def __init__(self, reader, writer):
        self.r, self.w = reader, writer
        self.closed = False
        self.lock = asyncio.Lock()

    async def _frame(self, opcode, data):
        if self.closed:
            raise Closed()
        n = len(data)
        head = bytes([0x80 | opcode])
        if n < 126:
            head += bytes([n])
        elif n < 65536:
            head += bytes([126]) + struct.pack('>H', n)
        else:
            head += bytes([127]) + struct.pack('>Q', n)
        async with self.lock:
            self.w.write(head + data)
            await self.w.drain()

    async def send(self, data):
        await self._frame(2, bytes(data))

    async def send_json(self, obj):
        await self._frame(1, json.dumps(obj).encode())

    async def close(self, code=1000, reason=''):
        if self.closed:
            return
        try:
            await self._frame(8, struct.pack('>H', code) + reason.encode()[:120])
        except (Closed, ConnectionError):
            pass
        self.closed = True
        try:
            self.w.close()
        except Exception:
            pass

    async def recv(self):
        """The next message: (is_text, bytes); Closed when it is over."""
        parts, kind = [], None
        while True:
            try:
                b0, b1 = await self.r.readexactly(2)
                n = b1 & 0x7f
                if n == 126:
                    n = struct.unpack('>H', await self.r.readexactly(2))[0]
                elif n == 127:
                    n = struct.unpack('>Q', await self.r.readexactly(8))[0]
                if n > MAX_MESSAGE or not b1 & 0x80:      # (a client's frames are masked)
                    raise Closed()
                mask = await self.r.readexactly(4)
                data = await self.r.readexactly(n)
            except (asyncio.IncompleteReadError, ConnectionError):
                self.closed = True
                raise Closed()
            if n:
                data = (int.from_bytes(data, 'little') ^
                        int.from_bytes((mask * (n // 4 + 1))[:n], 'little')).to_bytes(n, 'little')
            op = b0 & 0x0f
            if op == 8:
                await self.close()
                raise Closed()
            if op == 9:
                await self._frame(10, bytes(data))
                continue
            if op == 10:
                continue
            if op in (1, 2):
                kind, parts = op, [data]
            elif op == 0 and kind:
                parts.append(data)
            else:
                raise Closed()
            if sum(len(p) for p in parts) > MAX_MESSAGE:
                raise Closed()
            if b0 & 0x80:
                return kind == 1, b''.join(parts)


async def handshake(reader, writer):
    """The HTTP upgrade: (path, query) of an accepted WebSocket, else None (answered)."""
    try:
        head = await asyncio.wait_for(reader.readuntil(b'\r\n\r\n'), 10)
    except Exception:
        return None
    lines = head.decode('latin-1').split('\r\n')
    try:
        method, target, _ = lines[0].split(' ', 2)
    except ValueError:
        return None
    hdr = {}
    for line in lines[1:]:
        if ':' in line:
            k, v = line.split(':', 1)
            hdr[k.strip().lower()] = v.strip()
    url = urllib.parse.urlsplit(target)
    query = {k: v[0] for k, v in urllib.parse.parse_qs(url.query).items()}

    def answer(status, text):
        body = text.encode()
        writer.write(('HTTP/1.1 %s\r\nContent-Type: text/plain\r\nContent-Length: %d\r\nConnection: close\r\n\r\n' %
                      (status, len(body))).encode() + body)

    if hdr.get('upgrade', '').lower() != 'websocket' or 'sec-websocket-key' not in hdr:
        answer('200 OK', 'OpenCiv2 relay for the browser version\'s network games (tools/webrelay.py).\n')
        return None
    if opts.origin and hdr.get('origin', '') not in opts.origin:
        log('origin refused:', hdr.get('origin'))
        answer('403 Forbidden', 'This page may not use this relay (--origin).\n')
        return None
    accept = base64.b64encode(hashlib.sha1(hdr['sec-websocket-key'].encode() + GUID).digest()).decode()
    writer.write(('HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n'
                  'Sec-WebSocket-Accept: %s\r\n\r\n' % accept).encode())
    await writer.drain()
    return url.path, query


# ---- the policy ----

def allowed(ip):
    try:
        a = ipaddress.ip_address(ip)
    except ValueError:
        return False
    return a.version == 4 and (opts.allow_any or any(a in n for n in opts.allow))


def public_addr():
    if opts.public_addr:
        return opts.public_addr
    if opts.listen_host not in ('0.0.0.0', ''):
        return opts.listen_host
    try:        # (the address the default route leaves from: a UDP "connect" sends nothing)
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 53))
        a = s.getsockname()[0]
        s.close()
        return a
    except OSError:
        return '127.0.0.1'


def nodelay(writer):
    sock = writer.get_extra_info('socket')
    if sock is not None:
        try:
            sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        except OSError:
            pass


# ---- connect: one WebSocket, one TCP connection ----

async def do_connect(ws, query):
    global active
    host, port = query.get('host', ''), query.get('port', '')
    if not port.isdigit() or int(port) not in opts.ports:
        return await ws.close(4001, 'port not allowed')
    if not allowed(host):
        return await ws.close(4001, 'address not allowed')
    if active >= opts.max:
        return await ws.close(4003, 'too many connections')
    try:
        reader, writer = await asyncio.wait_for(asyncio.open_connection(host, int(port)), 15)
    except Exception as e:
        log('connect %s:%s failed: %s' % (host, port, e))
        return await ws.close(4002, 'could not connect')
    nodelay(writer)
    active += 1
    log('connected to %s:%s' % (host, port))
    try:
        await ws.send_json({'connected': True})

        async def tcp_to_ws():
            while True:
                data = await reader.read(65536)
                if not data:
                    break
                await ws.send(data)
            await ws.close(1000, 'closed')

        async def ws_to_tcp():
            try:
                while True:
                    text, data = await ws.recv()
                    if not text:
                        writer.write(data)
                        await writer.drain()
            except Closed:
                pass

        up, down = asyncio.ensure_future(tcp_to_ws()), asyncio.ensure_future(ws_to_tcp())
        done, _ = await asyncio.wait([up, down], return_when=asyncio.FIRST_COMPLETED)
        if down in done:            # the browser closed: no more sends (FIN), then the end
            try:
                writer.write_eof()
                await asyncio.wait_for(up, 5)
            except Exception:
                pass
        up.cancel()
        down.cancel()
    except (Closed, ConnectionError):
        pass
    finally:
        active -= 1
        writer.close()
        await ws.close(4000, 'connection lost')
        log('closed %s:%s' % (host, port))


# ---- listen: one WebSocket for a listening port and its connections ----

async def do_listen(ws, query):
    global active
    port = query.get('port', '')
    if not port.isdigit() or int(port) not in opts.ports:
        return await ws.close(4001, 'port not allowed')
    children, ids = {}, [0]

    async def child(reader, writer):
        global active
        if active >= opts.max or ws.closed:
            writer.close()
            return
        nodelay(writer)
        active += 1
        ids[0] += 1
        cid = ids[0]
        children[cid] = writer
        peer = writer.get_extra_info('peername') or ('0.0.0.0', 0)
        log('accepted %s as %d on %s' % (peer[0], cid, port))
        try:
            await ws.send_json({'accept': cid, 'ip': peer[0]})
            while True:
                data = await reader.read(65536)
                if not data:
                    break
                await ws.send(struct.pack('<I', cid) + data)
            await ws.send_json({'closed': cid})
        except (Closed, ConnectionError):
            pass
        finally:
            active -= 1
            children.pop(cid, None)
            writer.close()

    try:
        server = await asyncio.start_server(child, opts.listen_host, int(port), reuse_address=True)
    except OSError as e:
        log('listen on %s failed: %s' % (port, e))
        return await ws.close(4002, 'could not listen')
    log('listening on %s:%s' % (opts.listen_host, port))
    try:
        await ws.send_json({'listening': int(port), 'addr': public_addr()})
        while True:
            text, data = await ws.recv()
            if not text:
                if len(data) >= 4:
                    w = children.get(struct.unpack('<I', data[:4])[0])
                    if w:
                        w.write(data[4:])
                        await w.drain()
                continue
            msg = json.loads(data)
            if 'close' in msg:
                w = children.pop(msg['close'], None)
                if w:
                    w.write_eof()
                    w.close()
            elif msg.get('unlisten') and server:
                server.close()
                server = None
    except (Closed, ConnectionError, ValueError):
        pass
    finally:
        if server:
            server.close()
        for w in list(children.values()):
            w.close()
        await ws.close(4000, 'over')
        log('stopped listening on %s' % port)


async def client(reader, writer):
    try:
        got = await handshake(reader, writer)
        if not got:
            await writer.drain()
            writer.close()
            return
        path, query = got
        ws = WebSocket(reader, writer)
        if path == '/connect':
            await do_connect(ws, query)
        elif path == '/listen':
            await do_listen(ws, query)
        elif path == '/info':
            await ws.send_json({'addr': public_addr(), 'ports': sorted(opts.ports)})
            await ws.close()
        else:
            await ws.close(4004, 'unknown')
    except Exception as e:      # (one client's trouble is not the relay's)
        log('error:', repr(e))
        writer.close()


def main(argv=None):
    global opts
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--host', default='127.0.0.1', help='the WebSocket side\'s address (default 127.0.0.1)')
    ap.add_argument('--port', type=int, default=8765, help='the WebSocket side\'s port (default 8765)')
    ap.add_argument('--listen-host', default='127.0.0.1',
                    help='where games hosted by browsers listen (default 127.0.0.1; 0.0.0.0: every interface)')
    ap.add_argument('--ports', default='4993', help='the game ports allowed, comma-separated (default 4993)')
    ap.add_argument('--allow', action='append', default=[],
                    help='a network (CIDR) the browsers may connect to besides this machine\'s and the private '
                         'ones; "any": all')
    ap.add_argument('--origin', action='append', default=[], help='a page origin allowed (default: any)')
    ap.add_argument('--public-addr', help='the address the others join a browser\'s game at (default: guessed)')
    ap.add_argument('--max', type=int, default=32, help='connections at most (default 32)')
    ap.add_argument('-v', '--verbose', action='store_true')
    opts = ap.parse_args(argv)
    opts.ports = {int(p) for p in opts.ports.split(',') if p.strip()}
    opts.allow_any = 'any' in opts.allow
    opts.allow = [ipaddress.ip_network(n, strict=False) for n in PRIVATE + [a for a in opts.allow if a != 'any']]

    async def serve():
        server = await asyncio.start_server(client, opts.host, opts.port)
        print('webrelay: ws://%s:%d (game ports %s; joining %s; hosting on %s)' % (
            opts.host, opts.port, ','.join(map(str, sorted(opts.ports))),
            'any address' if opts.allow_any else 'this machine and private networks' +
            ''.join(' and ' + str(n) for n in opts.allow[len(PRIVATE):]), opts.listen_host), flush=True)
        async with server:
            await server.serve_forever()
    try:
        asyncio.run(serve())
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
