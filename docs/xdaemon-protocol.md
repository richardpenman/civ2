# XDaemon.dll: the network protocol of Civilization II Multiplayer Gold

The original game does all its networking through MicroProse's XDaemon.dll
(version 3.5.0). To let OpenCiv2 play against an unmodified civ2.exe, our
replacement has to put exactly the same bytes on the wire. This document
describes that wire protocol (TCP/IP only; IPX/SPX, modem and serial are
out of scope), how the game drives the library, and what the game sends
through it. It also proposes how we implement it.

Evidence tags:

- **[static ADDR]**: from the decompilation of XDaemon.dll. `ADDR` is the
  function's address in the DLL (image base 0x10000000).
- **[observed FILE]**: seen when two original games played each other under
  Wine. `FILE` is under `run/xd/cap/` (untracked).

All the hex dumps below come from our own captures.

## 1. Summary

| | |
|---|---|
| Transports | **TCP port 4993** ("secure": reliable, ordered, star topology through the host) and **UDP port 4994** ("broadcast": LAN-wide datagrams to 255.255.255.255, unreliable). |
| Byte order | Little-endian (raw x86 structs). Only sockaddr fields are in network order. |
| TCP framing | Records `[u32 total][u32 tag][u32 len][len bytes][padding]`, where `total = (len + 12)` rounded up to a multiple of 64. The padding is junk. |
| tag | `src_id << 24 \| dst_id << 16 \| flags16`. The ids are connection ids: host 0, clients 1..6, 0xFF means all. Control tags: `0xFA000000` (your id), `0xFC000000` (ping request), `0xFD000000` (ping reply). |
| UDP framing | `[i32 header][payload, at most 512 bytes]`. The header is XD_SendBroadcastData's 3rd argument; the game always passes 0. |
| Handshake | The client connects to the host's port 4993. The host accepts, sleeps 500 ms, then sends one record with tag 0xFA000000 whose 2-byte payload is the client's new id (u16). Nothing else is exchanged. |
| Routing | Clients only ever talk to the host. The host relays a client's message to dst 1..6, or to "all" (0xFF). |
| Keepalive | None in XDaemon. The game sends its own NM_HEART_BEAT every 60 s. |
| Disconnect | Graceful TCP close (shutdown, then drain, then closesocket). The peer's FD_CLOSE becomes the ConnectionLost(id) callback. |
| Async model | One thread. WSAAsyncSelect sends socket events as messages to hidden windows. Callbacks run inside the game's own message pump. |
| Version | XD_GetXDaemonVersion returns 3.5.0. The `version` argument of Init (3) selects secure and broadcast together. |

## 2. Evidence and how it was produced

### 2.1 Static

- Ghidra project: `run/xd/gproj` (project `xd`, separate from re/gproj).
  The DLL was imported from `~/Civ2/XDaemon.dll` (copied to `run/xd/bin/`).
- `run/xd/export/decomp.c` is the full export. `decomp_named.c` and `d2.c`
  are the same with the WSOCK32 imports, which are imported by ordinal,
  renamed to `ws_<name>`.
- `run/xd/export/extra_named.c` holds the functions Ghidra had not found:
  the window procedures and callbacks reached only through pointers. They
  were created with `tools/xdnet/MakeFuncs.java`.
- Imports: WSOCK32 by ordinal (accept, bind, closesocket, connect,
  getpeername, getsockname, getsockopt, htons, inet_addr, inet_ntoa, listen,
  recv, recvfrom, send, sendto, setsockopt, shutdown, socket,
  gethostbyname, gethostname, WSAAsyncSelect, WSAGetLastError, WSAStartup).
  There is no `select` and no `ioctlsocket`. DPLAYX ordinals 1, 2 and 4
  and ole32 are used only for the lobby.

### 2.2 Dynamic

- `run/xd/logproxy/`: our logging proxy. It sits between the replay hook
  DLL and the original library: XDaemon.dll (hook/) loads
  XDaemon_real.dll (the log proxy), which loads XDaemon_orig.dll (the
  original). The proxy logs every XD_* call and its payload bytes, wraps
  the game's callbacks, and works around a Wine incompatibility (§9.1).
- `tools/xdnet/sniff.py`: an AF_PACKET sniffer that writes a pcap.
  `xdparse.py` reassembles the TCP streams into records and names the
  game messages. `pcapdump.py` prints one line per packet.
- Captures:
  - `lan2/`: LAN game. The host (Alice, 172.17.0.2) and the client (Bob,
    172.17.0.3) run in two containers. The run covers discovery by UDP
    broadcast, the join, nation choice, world generation, one turn each
    (each founds a city), then the client quits.
    Files: `capture.pcap`, `records.txt`, `xdlog-host.txt`,
    `xdlog-client.txt`, `shots/`.
  - `inet1/`: "Internet game" over **127.0.0.1**. Both games share one
    network namespace. The run covers the join by typed IP address and
    game start, then the **host quits and the client becomes the new
    host**. Same files.
  - `lan1/` and `lan1b/`: failed first attempts, kept because they show
    the Wine SO_RCVBUF problem (§9.1).

## 3. Sockets and ports

**Init.** `XD_InitializeSocketsTCP(version, port, port2, is_server,
max_players, cb)` [static 10001050]. The game calls it as
`(3, 4993, 4994, is_server, 7, NULL)` [observed xdlog-*.txt].

- **version** selects the parts to create: 1 = secure only, 2 = broadcast
  only, 3 = both. Any other value fails with -9.
- **Broadcast part** (an internal "QuickSocket", [static 100042d0]):
  - A UDP socket with SO_BROADCAST and SO_REUSEADDR, bound to
    `INADDR_ANY:port2` (4994). It has a 0x204-byte buffer (payload at most
    0x200 bytes).
  - It is registered with `WSAAsyncSelect(FD_READ)` to a hidden window
    ("QSocketWndClass").
  - Its send address is `255.255.255.255:port2`.
- **Secure part** (a "stream socket" object, 0x1EC bytes,
  [static 10004d90]):
  - A TCP socket. The host binds it to `INADDR_ANY:port` (4993). A
    client's socket is not bound (ephemeral source port).
  - It gets a receive buffer whose size comes from
    `getsockopt(SO_RCVBUF)`, stored in a **16-bit** field (§9.1).
  - Each stream socket gets its own hidden window ("StreamWindowClass",
    title "XDSControlWindow N").
  - No TCP_NODELAY and no SO_LINGER are set.
- **Host only**:
  - It allocates a connection table of `max_players` entries (7), at most
    0xF8.
  - It puts the listening socket in entry 0, so the host's own id is 0
    and clients get 1..6 [static 10005f60, 100060a0].
- **`XD_ActivateServer`** [static 10001830 → 10005160]:
  `WSAAsyncSelect(FD_ACCEPT|FD_CLOSE)` and `listen(backlog 5)`.
- **`XD_StopConnections`**: `WSAAsyncSelect(listen socket, 0)`. New
  connections are no longer accepted; the listener stays open.
- **Accepted sockets** get `WSAAsyncSelect(FD_READ|FD_WRITE|FD_CLOSE)`.
- **A client's connecting socket** gets
  `WSAAsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE)` before its
  non-blocking `connect()` [static 100051d0].

Observed ports: the host listens on 4993/tcp. The client's ports were
59972 and 55206. Both sides send from and receive on 4994/udp
[observed lan2, inet1].

## 4. UDP: XD_SendBroadcastData and the broadcast receive callback

**Sending** [static 10002570]:

- The datagram is `[i32 flags][payload]`, sent to 255.255.255.255:4994.
- `flags` is the 3rd argument of XD_SendBroadcastData.
- A payload longer than 512 bytes is silently truncated to 512.
- The function returns sendto()'s result: payload + 4 bytes.

**Receiving** [static 100047d0]:

- On FD_READ, `recvfrom` reads one datagram, then the library calls
  `broadcast_cb(payload, (u16)(n-4), header)`.
- Every socket bound to 4994 receives every datagram, the sender's own
  included. The game filters them itself [observed: `CB BroadcastReceive`
  right after the host's own send in xdlog-host.txt].
- A 6-byte datagram is compared against "XDPING", meant to trigger a
  version-string reply. The comparison reads 6 bytes starting at offset 4
  of a 6-byte datagram, so it can never match. Ignore it.

**Unused by the game**: XD_SendUDPBroadcastData (to a given IP),
XD_SendDirectBroadcastData, and XD_EnableServerLocator. The last one sends
a 4-byte datagram 0xFFAABBCC every 500 ms [static 10002ed0, 10003210].

Observed traffic [observed lan2/records.txt]:

- On creating the game, the host broadcasts its game description once (412
  bytes).
- After that it answers each client query (120 bytes, sent every 250 ms
  while the client's game list is open) with the description again.
- In Internet mode the host still broadcasts once [observed inet1].

```
client query (UDP payload; the first 4 bytes are the XDaemon header):
00000000 60666066 00000000 74000000 00000000 3137322e31372e302e33...
   hdr    magic    type 0    size 116  seq 0  "172.17.0.3"
```

## 5. TCP: the record stream

### 5.1 Framing [static 10005490 (send), 10005ac0 (receive)] [observed]

```
+0  u32 total     whole record, header included, rounded up to a multiple of 64
+4  u32 tag       src<<24 | dst<<16 | flags (flags = 4th arg of XD_SendSecureData, the game passes 0)
+8  u32 len       payload length
+12 len bytes     payload
    total-12-len  padding: junk (stale bytes of XDaemon's static send buffer)
```

- `total = (len + 12 + 63) & ~63`. For example: len 2 → 64; len 100 → 128;
  len 308 → 320 (no padding); len 3002 → 3072. All 264 records in both
  captures follow this [observed records.txt].
- The receiver consumes `total` bytes and passes only `len` bytes up.
- The padding is not zeroed. Here it holds pieces of earlier messages
  [observed]. We should write zeros; receivers must ignore it.
- Records are a plain byte stream. A TCP segment may carry several records
  or part of one.
- Large records are sent in `send()` calls of at most 0x2000 bytes. The
  rest is queued (§8).
- Size limit: `XD_SetOversizedMessageCB(limit, cb)` (the game passes
  50000).
  - Sending a payload of `len >= limit-12` fails with -1.
  - A received record with `total > limit-12` calls `cb` and is dropped.
    The receiver resets its state but does not skip the record's bytes,
    so the stream desynchronises after that. Never send such a record.

Example, the first record a client sends (NM_PLAYER_LIST_NODE, 100 bytes)
[observed lan2]:

```
80000000 00000001 64000000 | 60666066 2f000000 64000000 15000000 01000000 "172.17.0.3"...
total 128  tag: src1 dst0  len 100 | game message: magic, type 47, size 100, seq 21, ...
```

### 5.2 Connection establishment [static 10001940, 100051d0, 10002820, 10002a20] [observed]

1. Client: `XD_OpenConnection("a.b.c.d", timeout_ms)`.
   - The address goes through `inet_addr` only: dotted quads, no DNS.
   - The library makes a non-blocking `connect()` to port 4993, starts a
     window timer (id 0x1F5) of `timeout_ms`, and returns 0 at once.
   - The game passes 15000 for LAN and 60000 for Internet
     ("TCPIP Timeout" / "INTERNET Timeout" in CIV.INI) [observed
     xdlog-client.txt].
2. Host: on FD_ACCEPT, it `accept()`s and takes the first free table entry
   (1..6) as the client's id.
   - It **sleeps 500 ms**, then sends one record with tag `0xFA000000`
     and a 2-byte payload: the id as u16.
   - It then calls `NewClientConnection(count, id)`, where count is the
     number of used table entries, the listener included. Observed:
     `(2, 1)`.
   - If the table is full, the accepted socket is closed.
3. Client: on a record with tag exactly `0xFA000000`, while it has no id
   and the timer has not expired, it takes the id and kills the timer.
   - It then calls `ClientConnectionToServer(id)`.
   - If the timer fires first, it closes the socket, makes a fresh one,
     and calls `ClientConnectionToServer(-1)`.

```
SYN / SYN+ACK                       (t = 0)
host -> client  (t = +0.505 s):
40000000 000000fa 02000000 0100 0000...0000      total 64, tag FA000000, len 2, id 1, 50 bytes zero padding
client -> host  (game: NM_PLAYER_LIST_NODE, see §10)
```

[observed lan2 at 149.48 / 149.99 s; inet1 at 0.000 / 0.498 s]

### 5.3 Secure sends and routing [static 10001f50, 10002a20]

`XD_SendSecureData(to, data, len, flags)` builds the tag with
`src = my id`, `dst = to`.

- **On a client**, every record goes to the host connection, whatever
  `to` is. Sending to its own id returns -9 and calls the
  OnInvalidSendToSelf callback.
- **On the host**:
  - `to` = 1..6: sent on that client's connection with tag
    `0x00dd0000 | flags`.
  - `to` = 0xFF: one copy per client, each with its own dst (tag
    `0x00dd0000`, never 0x00FF0000) [observed lan2: `to=255` calls give
    `tag=00010000`]. The return value is `len` when all succeed.
- **The host's handling of a received record** depends on the tag:
  - dst = 0: delivered locally as `secure_cb(src, payload, len, tag)`.
  - dst = 0xFF: sent to every client except the sender, with the tag
    rewritten to `src=0 (host), dst=client`, then delivered locally with
    the real src. Other clients therefore see a relayed broadcast as
    coming from the host (id 0) [static only; our captures have one
    client].
  - dst = another client: forwarded unchanged (src kept) and not
    delivered locally.
- **A client** delivers every received record (except the control tags)
  to its callback, with `src` = the top byte of the tag.
- **The callback's 4th argument** is the whole 32-bit tag, but its
  prototype declares a `short`, so the game sees only `flags`.
- **Return values**:
  - 2: sent at once.
  - 1: queued.
  - -2: out of memory.
  - -3 / -4: send error. -4 also closes the connection.
  - -5: not initialised.
  - -9: bad target.
  - -13: no such connection.
  - The game only checks for `< 0`.

### 5.4 Ping (not used by the game) [static 100026e0, 10002a20]

- `XD_SendPingRequest(id, size, cb)` sends a record with tag `0xFC000000`
  and a payload of `size` bytes (at least 0x20; 0x40 if smaller). The
  payload's first u16 is the sender's id.
- The receiver echoes the payload back with tag `0xFD000000`. A host
  sends the echo to the connection named by that u16.
- The requester calls `cb(elapsed_ms, size/0x2000)`.

We must answer 0xFC with 0xFD, and never deliver 0xFA, 0xFC or 0xFD
records to the game.

### 5.5 Flushing, queueing, and the thread model [static 10002240, 10005490, 10005740]

- **Sending.** If a stream's queue is empty and the record fits
  (total ≤ 0x2000), it is `send()`-ed from a static buffer. Whatever does
  not go out (WSAEWOULDBLOCK or a partial send) is queued.
- **The queue** is drained by FD_WRITE. XDaemon also posts a fake FD_WRITE
  to its own window after each queue append.
- **`XD_FlushSendBuffer(ms)`**:
  - It loops `PeekMessage(NULL, PM_REMOVE)`/`DispatchMessage`, which
    dispatches **all** of the thread's messages, the game's included.
  - It stops when every stream's queue is empty or `ms` has passed, and
    returns the number of items left (0 = flushed).
  - While it runs, `XD_InFlushSendBuffer()` returns 1. The game uses this
    to avoid re-entering its own code.
- **Threads.** XDaemon has no threads for TCP/UDP. The only thread is the
  debug "port noise" generator.
- **Callbacks** (receive, accept, close, connect timeout) all run from
  window procedures. They therefore run inside whatever message loop the
  game is pumping, on the game's thread.

### 5.6 Disconnect and loss [static 10001d80, 10001cb0, 100053a0, 10002e00] [observed]

- **Closing a socket** [static 100053a0]: `WSAAsyncSelect(0)`,
  `shutdown(SD_SEND)`, `recv` until 0 or error, `closesocket`. On the wire
  this is a normal FIN exchange [observed lan2 413.18 s, inet1 312.33 s].
- **`XD_CloseConnection`**:
  - On a client: closes the server stream and immediately creates a fresh
    unconnected socket, so XD_OpenConnection can be called again. This is
    where the extra `getsockopt` comes from in xdlog-client.txt.
  - On the host: closes every client connection.
- **`XD_ServerCloseConnection(id)`** (the host kicks a player): closes that
  client's connection.
- **FD_CLOSE, or a fatal send error**: the connection is closed, then
  `ConnectionLost(id)` is called. A client receives id 0 when it loses the
  host [observed: client `CB ConnectionLost id=0` in inet1; host
  `CB ConnectionLost id=1` in lan2].
- **`XD_ShutdownSockets` / `XD_ResetLibrary`**: close everything.
- **XDaemon sends no keepalive.** A dead peer is noticed only through TCP
  errors, or by the game's own heartbeat (§10).

## 6. Other API functions the game uses

| Function | Behaviour |
|---|---|
| `XD_GetCurrentProtoAddr(adapter, 0)` | Returns the local IP as text: `inet_ntoa` of entry #adapter (the game passes 0) of `gethostbyname(gethostname())`. Our runs gave "172.17.0.2" and "172.17.0.3" [observed]. The game puts this string in its messages (player slots, datagram addresses, the host's address for migration), so it has to be the address other players can reach. |
| `XD_GetXDaemonVersion` | Returns 3, 5, 0. |
| `XD_LaunchedByLobby(hwnd, info)` | DirectPlay lobby (IDirectPlayLobby GetConnectionSettings). Returned -3 ("not launched by a lobby") in all our runs [observed]. |
| `XD_LobbySendMessage` | DirectPlay lobby SendLobbyMessage. Both lobby functions can stay stubs: -3, and -2 "no lobby" [static 10007d10, 10007fb0]. |
| `XD_InitializeSocketsTCP` | Fails with -4 if already initialised and -11 if `max_players` > 0xF8 [static 10001050]. |
| The other entry points | Return -1 when not initialised. |
| Debug log | XDaemon can append its own log to a file. The switch is internal and its file is not configured by the game. |

## 7. Callback prototypes (corrections to src/include/xdaemon.h)

The C++-mangled export names give the real prototypes, all `__cdecl`:

```
SetBroadcastReceive          (void (*)(void *data, unsigned short len, long header))
SetSecureReceive             (void (*)(unsigned short src, void *data, unsigned long len, short flags))
SetNewClientConnection       (void (*)(unsigned short connected_count, unsigned short new_id))
SetOnClientConnectionToServer(void (*)(short my_id_or_minus_1))
SetOnConnectionLost          (void (*)(unsigned short id))
SetOversizedMessageCB        (unsigned long limit, void (*)(unsigned long size))
InitializeSocketsTCP         (int, int, int, int, unsigned, void (*)(void *, unsigned short, long))
OpenConnection               (void *address, unsigned long timeout_ms)
SendSecureData               (unsigned short to, void *, unsigned long, short flags)
SendBroadcastData            (void *, unsigned long, long header)
```

xdaemon.h's `XD_NewClientCB(int client_info, unsigned conn_id)` corresponds
to `(count, id)`. The game's handler only logs it.

## 8. Limits

| What | Limit |
|---|---|
| Players | At most 7 (host 0 and clients 1..6), as the game passes. XDaemon allows ids up to 0xF7. |
| UDP payload | At most 512 bytes. The game's largest is 408. |
| TCP payload | Less than `limit - 12` (49988 with the game's 50000). The largest observed was 3004 (NM_JOIN_GAMESTATE, small map). Larger maps produce bigger messages; NM_MAP_PACKETS is used for the map. |
| send() chunk | 0x2000 bytes. On the receiving side, recv() fills the buffer (8192 on Windows 9x). |

## 9. Compatibility notes and quirks

### 9.1 SO_RCVBUF truncation (Wine, and modern Windows)

XDaemon allocates its receive buffer with the size from
`getsockopt(SO_RCVBUF)` and keeps that size in a **u16**.

- Under Wine on Linux the value is 131072, which truncates to 0. Every
  `recv(len = 0)` then returns 0 and the client spins on FD_READ forever.
  The host's id record is never read, and the join fails after the timeout
  [observed lan1b/wine-client-head.log: about 180 000 `WS2_recv_base` calls
  in seconds; lan1: the client times out with `ClientConnectionToServer
  id=-1`].
- Windows 95/98 report 8192. Modern Windows may report 64K or more, so the
  original game may well have the same problem on current Windows unless
  the system is tuned.
- Our proxy clamps the value to 8192 (`getsockopt(...) = 131072 -> 8192`
  in xdlog-*.txt). This changes nothing on the wire.
- For interop testing against the original under Wine, the original side
  needs this fix (`run/xd/logproxy`).

### 9.2 Other quirks to tolerate

- The padding bytes are junk, and the game header's `seq` is per sender
  (§10). Neither is checked.
- The host blocks its UI for 500 ms on every accept.
- The game process allows only one instance per Windows session (a named
  mutex, "Civilization II Once Only"). Two originals on one machine
  therefore need two Wine prefixes, that is, two wineservers. We used two
  containers. Our own build should not keep that restriction.
- Every instance binds UDP 4994 with SO_REUSEADDR, so several games on one
  machine all receive the LAN broadcasts.
- When two players share an IP address, as in the 127.0.0.1 test, the
  game's datagram filter (§10.2) cannot tell them apart. Internet mode
  does not use UDP for anything that matters, so it still works.

## 10. What the game sends through XDaemon

### 10.1 When the game calls XD_* (src/auto/network*.cpp)

- **Startup** (`net_session_ctor`): Set{Broadcast,Secure}Receive,
  SetOnClientConnectionToServer, SetNewClientConnection,
  SetOnConnectionLost, SetOversizedMessageCB(50000), then
  `XD_ResetLibrary`, `XD_LaunchedByLobby` and `XD_GetXDaemonVersion`
  [observed xdlog: startup lines].
- **Hosting and joining** (`net_init_transport`): ResetLibrary, then
  `InitializeSocketsTCP(3, 4993, 4994, is_server, 7, NULL)`. A host then
  calls ActivateServer. Both then call `GetCurrentProtoAddr(0, 0)` for
  their own address.
- **Browsing games** (`net_gamelist_*`):
  - The client broadcasts a query (NM_DATAGRAM with inner NM_GET_SERVERS)
    every 250 ms.
  - The host answers with a broadcast NM_SERVER_INFO.
  - Joining: `XD_OpenConnection(host_ip, timeout*1000)`, then the game
    waits for the connected callback, after which the client sends
    NM_PLAYER_LIST_NODE.
- **Host lobby** (`lobby_host_wait_players`):
  - "Reject Player" calls `XD_ServerCloseConnection(id)`.
  - Start and cancel call `XD_StopConnections`.
  - Lobby launches call `XD_LobbySendMessage(1, 2 or 3)`.
- **Sending** (`net_session_send_to` / `net_session_send_to_all`):
  - The game sets `msg->seq = net_send_sequence++`, then calls
    `XD_SendSecureData(player, msg, msg->size, 0)` or
    `XD_SendBroadcastData(msg, msg->size, 0)`.
  - `player` 0xFF means everyone.
  - Many game actions then call `XD_FlushSendBuffer(5000)`; game state
    transfers use 60000 and the host's lobby uses 180000.
- **Receiving** (`net_on_secure_receive`, `net_on_broadcast_receive`):
  - The callbacks accept a message only if it starts with the magic
    0x66606660, then queue it (`net_msgq_enqueue`).
  - `net_process_messages` handles the queue later, from the game's idle
    loops.
- **Leaving** (`net_session_shutdown`): `XD_FlushSendBuffer(60000)`,
  `XD_CloseConnection` (on a client), `XD_ShutdownSockets`, then
  `XD_ResetLibrary`.
- **Host migration** (`net_host_quit_handoff`, `net_client_transfer_server`):
  1. The quitting host sends NM_SERVER_QUIT_REQUEST and waits for
     NM_SERVER_QUIT_ACK.
  2. It sends NM_SERVER_QUIT, a state diff, NM_START_CIVILIZATION and
     NM_PLAYER_LIST (with the new host in slot 0), then closes.
  3. The new host (the first remaining player) calls `XD_CloseConnection`,
     `XD_ShutdownSockets` and `XD_ResetLibrary`, then
     `InitializeSocketsTCP(..., server = 1)` and ActivateServer on the
     same ports.
  4. The other clients `XD_OpenConnection` to the new host's address (from
     the player list).

  [observed inet1: client xdlog `ConnectionLost id=0` →
  `InitializeSocketsTCP(... server=1 ...)`; screenshot
  `inet1/shots/c14.png` "You are the new hosting player"]

### 10.2 Game message format (inside the XDaemon payload)

Every game message starts with a 16-byte header (`NetMsg` in
src/include/civ2classes.h):

```
+0  u32 magic 0x66606660 (bytes 60 66 60 66)
+4  i32 type   (index into the NM_* name table, str_nm_datagram)
+8  u32 size   (whole message, header included == XDaemon payload len)
+12 i32 seq    (sender's running counter, shared by both send paths; only logged)
```

- **Messages from `net_send_message`** (0x2C bytes): the header, then
  `args[6]`, then `payload_len`, then the payload
  (`net_msg_build_packet`).
- **Datagrams** (UDP, type 0 NM_DATAGRAM, `NetNamedMsg`):
  - +0x10: sender address (32 bytes).
  - +0x30: target address. When empty, every receiver accepts the
    message; otherwise only that address does.
  - +0x50: excluded address. A receiver with this address drops the
    message (the sender's own echo).
  - +0x70: inner type: 1 = NM_GET_SERVERS (116 bytes) or
    2 = NM_SERVER_INFO (408 bytes = 0x74 + `NetGameInfo` 0x124).
  - NetGameInfo holds the host IP, the version string ("5.4.0f Multiplayer
    26-March-99"), the game name, the host name, options and map size.
    Some of its fields are uninitialised pointers.
- **The struct definitions** are already in civ2classes.h / civ2types.h:
  NetPlayerNodeMsg 0x64, NetPlayerListMsg 0x280, NetPlayerInfoMsg 0x118,
  NetServerAnswerMsg 0x134, NetWhoseTurnMsg 0x14, ParleyOfferMsg, and
  others.

### 10.3 Message types seen on the wire [observed lan2 + inet1]

S>C is host to client, C>S client to host, C>all a client's dst 0xFF send.

| Type | Name | Direction | Sizes |
|---:|---|---|---|
| 0 | NM_DATAGRAM (UDP) | broadcast | 116 (query), 408 (server info) |
| 4 | NM_PLAYER_LIST | S>C | 640 |
| 6 | NM_SERVER_PATH | S>C | 540 |
| 9 | NM_JOIN_GAMESTATE | S>C | 3002, 3004 |
| 10 | NM_MAP_STRUCT | S>C | 158, 166 |
| 14/15/16 | NM_SERVER_QUIT_REQUEST / _QUIT / _QUIT_ACK | S>C, S>C, C>S | 16 |
| 18/19 | NM_SERVER_QUERY / NM_SERVER_ANSWER | C>S / S>C | 16 / 308 |
| 21 | NM_DIFFERENCE_ENGINE (state diffs) | S>C, C>all | 60..1039 |
| 22 | NM_START_CIVILIZATION | S>C | 16 |
| 23..36 | NM_COSMIC, CIVILIZE, IMPROVE, ENDWONDER, UNITS, TERRAIN, GOVT, GOVT_TITLE, CARAVAN, ORDERS, DIFFICULTY, ATTITUDES (the rules tables) | S>C | 72..1644 |
| 37, 38, 40, 42 | NM_EVENTSLOADED, NATION_ACCEPT, GAME_GOING, LOCK_DATA | S>C | 44 / 16 |
| 43/44 | NM_HEART_BEAT / _ACK | both ways | 16 |
| 45 | NM_QUIT_REQUEST | C>S | 44 |
| 46/47/48 | NM_PLAYER_INFO / PLAYER_LIST_NODE / LOCK_NATION | C>S | 280 / 100 / 44 |
| 51 | NM_MULTIOPTIONS_CHANGE | S>C | 44 |
| 52/53 | NM_KILL_OR_QUIT / _ACK | C>S / S>C | 44 / 16 |
| 55/56, 59/60 | NM_DELETE_UNIT / UNIT_DELETED, CREATE_CITY / CITY_CREATED | C>S / S>C | 44 / 76, 44 / 132 |
| 92 | NM_ALPHA_DIFFERENCE_ENGINE | S>C | 60..188 |
| 105 | NM_START_WHOSE_TURN | S>C | 20 |
| 106 | NM_POPUP | S>C, C>all | 73, 85 |
| 114/116/117 | NM_DRAW_SQUARE / DRAW_MAP / DRAW_AREA | S>C | 44 / 16 / 44 |
| 135, 144, 148, 151 | NM_CITYWIN_CHECK_UNIT, FEATURE_SET, OWNER_SET, FEAT_LOC_SET | S>C (148/151 both ways) | 44 |
| 155, 156, 161, 162 | NM_SET_CIV, READY_TO_START, POLITICAL_ATTITUDE, TURN_FINISHED | S>C ×3, C>S | 44 / 16 / 44 / 44 |

**Join sequence** [observed lan2, game level]:

1. The client sends NM_PLAYER_LIST_NODE.
2. The host presses Start. The host sends PLAYER_LIST, GAME_GOING,
   SERVER_PATH and JOIN_GAMESTATE, then LOCK_DATA.
3. The client sends LOCK_NATION (its tribe choice). The host answers
   NATION_ACCEPT and LOCK_DATA.
4. The client sends PLAYER_INFO. The host sends GAME_GOING and
   PLAYER_LIST.
5. After world generation the host sends: about 40 DIFFERENCE_ENGINE
   blocks (most about 1 KB), MAP_STRUCT, EVENTSLOADED, START_CIVILIZATION,
   GAME_GOING, MULTIOPTIONS_CHANGE, the rules tables (COSMIC … ATTITUDES),
   READY_TO_START, ALPHA_DIFFERENCE_ENGINE, and START_WHOSE_TURN.
6. The client sends NM_HEART_BEAT every 60 s; the host acks it. The host
   also pings the player whose turn starts.

In Internet mode the client first sends NM_SERVER_QUERY and the host
answers NM_SERVER_ANSWER [observed inet1].

## 11. Running two originals against each other (test reference)

Everything runs in the `civ2-dev` image, with one container per game:
the second instance needs its own Wine prefix (§9.2). Scripts are in
`tools/xdnet/`. The game files come from `~/Civ2`, mounted read-only;
start1.sh copies them to /tmp/g in the container. It then puts hook/'s
XDaemon.dll in front of the log proxy (XDaemon_real.dll), which loads the
original (XDaemon_orig.dll).

```sh
cd /home/labaron/code/civ2
# build the log proxy (once)
mkdir -p run/xd/logproxy
docker run --rm -v "$PWD":/work -w /work/tools/xdnet civ2-dev sh -c \
  'i686-w64-mingw32-gcc -O1 -shared -o /work/run/xd/logproxy/XDaemon.dll xdlog.c xdlog.def -static-libgcc -Wl,--enable-stdcall-fixup'

# LAN test: two containers on the docker bridge (172.17.0.x), separate Wine prefixes
for c in xdnet:civ2-wine2 xdnet2:civ2-wine; do
  docker run -d --name ${c%%:*} --hostname ${c%%:*} -v ${c##*:}:/wine -v "$PWD":/work \
    -v "$HOME/Civ2":/civ2:ro -w /work civ2-dev sleep infinity
done
#   (Internet/127.0.0.1 test instead: run xdnet2 with --network container:xdnet and use DISP=8 for it)
R=lan3; mkdir -p run/xd/cap/$R
docker run -d --name xdsniff --network container:xdnet --user root -v "$PWD":/work civ2-dev \
  python3 -I /work/tools/xdnet/sniff.py /work/run/xd/cap/$R/capture.pcap
docker exec xdnet  sh /work/tools/xdnet/start1.sh $R host
docker exec xdnet2 sh /work/tools/xdnet/start1.sh $R client
docker exec xdnet  sh /work/tools/xdnet/host_lan.sh $R Alice TestGame   # ends at "Available Players"
docker exec xdnet2 sh /work/tools/xdnet/join_lan.sh $R Bob              # ends joined, waiting
# then by hand: k.sh SHOT keys... (xdotool on display :7, then a screenshot)
#   host: Return (Start Game), Return (profile); client: Return (profile)
#   each: Return x4 (gender, tribe, name, city style); world generation ~60 s
docker exec xdnet sh /work/tools/xdnet/k.sh /work/run/xd/cap/$R/shots/x.png Return
# analyse
python3 -I tools/xdnet/xdparse.py run/xd/cap/$R/capture.pcap > run/xd/cap/$R/records.txt
# clean up
docker stop xdsniff; docker rm -f xdsniff xdnet xdnet2
```

Notes:

- Logs go to `run/xd/cap/$R/xdlog-{host,client}.txt` (XD calls, payload
  hex, callbacks).
- With `CLAMP_RCVBUF=0` the proxy leaves the SO_RCVBUF value alone, which
  reproduces the original's failure under Wine.
- Without the hook DLL in front, the original's EDIT boxes crash under
  Wine (hook/compat.c), so the hook stays in the chain. With
  `CIV2_HOOKS=none` and no `CIV2_SCRIPT` it does not replay or alter
  timing.
- Some dialogs show black under xdotool driving (difficulty, aggression).
  Keys still work.

### 11.1 The automated two-game test (tools/xdnet/nettest.py)

`tools/xdnet/nettest.py` does the above by itself: two `civ2-dev`
containers (`xdh` hosting, `xdc` joining; the originals get their own Wine
volumes, civ2-wine2 and civ2-wine), each game on its own Xvfb, driven with
xdotool from the docker host:

```sh
cd /home/labaron/code/civ2
tools/xdnet/nettest.py ours-ours                          # host-client: ours or orig
tools/xdnet/nettest.py ours-orig --exe civ2-linux32asan   # our side under ASan
tools/xdnet/nettest.py orig-ours --inet                   # Internet mode, 127.0.0.1
tools/xdnet/nettest.py ours-ours --diplo --load --turns 2 # chat, parley, save + load
tools/xdnet/nettest.py ours-orig --turns 50 --save-end    # a long game; both saves must agree
tools/xdnet/nettest.py ours-ours --exe civ2-linux32cov --migrate   # coverage (netcov.py), host migration
```

- Steps: start menu, host a LAN (or Internet) game with 3 civilizations,
  join it, start, the default civilizations, N turns (1: move the settler;
  2: found a city, close the city window; then: sentry, end the turn), quit
  (client first, then host). `--chat`: Chat with Kings both ways
  (the host ticks "All Humans Can Chat From Start"); `--diplo`: the parley
  (below); `--save`/`--load`: the host saves in its turn 2, then after
  quitting loads that game ("Load a Saved Multiplayer Game"), the client
  joins again and N more turns are played; `--rejoin`: the host opens the
  game ("Open Game"), after the turns the client quits, starts again and
  joins the game in progress ("Hot Join"), one more turn each; `--third
  ours|orig`: a third player (container `xdc2`) joins after the client, so
  the host relays between two clients. `--keep` leaves the containers
  (drive them by hand with `docker exec -e DISPLAY=:7 xdh xdotool ...`),
  `--stop-after lobby|join|civ|turns` stops there, `--gdb host|client`
  runs our game under gdb (a backtrace on a crash or a fatal error),
  `--exe-host`/`--exe-client` pick our build per side (two ASan games need
  about 700 MB), `--sniff` also captures the packets (records.txt).
- Waits are on conditions: the XD logs (a message type sent or received,
  NM_START_WHOSE_TURN for a civ, the client's NM_PLAYER_INFO), the screen
  having stopped changing, and which dialogs are open (the original: its
  top-level windows' names, xdotool; ours: CreateWindowEx/ShowWindow in
  its log). Each dialog gets Return (a city window Escape); a turn is
  played once its "It is our turn" dialog has gone.
- Checks, in `run/xd/cap/NAME/result.txt` (exit status 0 when all pass):
  every logged game message well formed (magic, size, and the difference
  engine's records and RLE streams: `tools/xdnet/xdmsgs.py LOG --check`);
  what each side sent over TCP the other received, in order and byte for
  byte (the host's last messages after the client left excepted); both
  reached turn N (NM_START_WHOSE_TURN for civ 0); the expected message
  types in order (client: PLAYER_LIST_NODE, LOCK_NATION, PLAYER_INFO,
  CREATE_CITY, TURN_FINISHED, QUIT_REQUEST; host: PLAYER_LIST ...
  START_WHOSE_TURN, CITY_CREATED), NM_CHAT and the parley messages when
  asked for; no crash (both alive until they quit, no ASan report, no
  Wine "Unhandled", no fatal error in our log). With three players the
  byte-for-byte stream check becomes: every message a game received was
  sent by another, and every message a client sent to everyone reached
  both others.
- A run takes 8 to 15 minutes (world building is in real time); the
  logs, `msgs-*.txt` (`xdmsgs.py` output), and a screenshot at each step
  (`shots/`) stay in the run's directory. Our XD log (CIV2_XDLOG) has the
  payloads in full, as the proxy's.
- The original under Wine opens a 640x480 window; on a 1024x768 screen
  it uses the large layouts, so its parley and chat windows came out half
  off it.  The test moves and sizes it as our client area (0,19 1024x749):
  its dialogs and windows then come where ours do and one set of clicks
  drives both.  (Its dialogs often show black under Xvfb, its own
  painting under Wine; the clicks and keys still work.)
- More options: `--diplo` drives the whole parley (below); `--extras` the
  advisors F1..F12, Multiplayer Options (Ctrl+Y) and a timer change
  (Ctrl+T, the others vote); `--features kill,positions,production,movement`
  ticks those multiplayer features, `--timer N` picks the turn timer (0
  unlimited, 1 30 s ...); `--simultaneous` simultaneous moves (CIV.INI
  `Simultaneous=1`, the extra feature ticked; turns start with
  NM_HUMAN_MOVEMENT); `--migrate` the host leaves first and the client
  becomes the host (NM_SERVER_QUIT_REQUEST/_ACK, then it opens its sockets
  as a server) and plays a turn alone; `--kick` (with `--third`) the host
  rejects the third player in Available Players
  (XD_ServerCloseConnection; it sees "You were REJECTED"); `--lobby` (with
  `--third --kick`) the lobby as players come and go first: Reject Player
  with no one listed (nothing happens), the third player (Carol) cancels
  its wait (NM_CLIENT_CANCELED_START: the host takes it off the list) and
  joins again (ours from the network menu; the original starts again: its
  game list, after a join it cancelled, never asks for games, no
  XD_SendBroadcastData, so it lists none), then it is rejected; `--save-end`
  both players save where the game stands and `tools/xdnet/savecmp.py`
  compares the two saves section by section (game, civs, map, units,
  cities; what each civ has seen and the saving player's own header
  fields apart); `--exe civ2-linux32cov` runs the coverage build with its
  gcov data in run/xd/cov/RUN-ROLE (`tools/xdnet/netcov.py` merges them
  and lists the network functions never run; `--classes`: with the
  network classes' members, LobbyList:: ...); `--exe OpenCiv2.exe` runs
  the Windows build under Wine (its Winsock code).
- `--diplo`: Chat with Kings both ways (with the chat macros Ctrl+F1..F3,
  Clear, the Mute and Anonymous boxes); an audience left unanswered (the
  requester gives up after 15 s, the request closes itself after 20 s and
  answers 0, Go away); an audience granted; then, each side offering in
  turn: a gift of 10 gold declined (No Thanks) and accepted, the reveal and
  conceal buttons, a cease-fire countered (Make Counter Offer), peace
  (picked by double-click), maps bartered for maps, a proposal to share
  maps, a threat (5 gold or else), an alliance, a gift of maps, the treaty
  cancelled; Check Intelligence, a key typed, the small layout, Escape
  ends the negotiations; last an audience granted too late (after the
  requester gave up): the other's parley window opens alone, a new request
  is answered 2 (busy) at once, End Negotiations closes it.  Checked: each
  offer, reply and execution on the wire both ways, the replies' values.
  Knowledge, units, cities and war declarations need advances, units and
  other contacts: a longer game.

## 12. Proposed design for OpenCiv2's implementation

The goal: an XD_* implementation that speaks this protocol byte for byte,
in every build including wasm32, which has no sockets.

**1. Host layer: raw non-blocking sockets.** A new `src/host/host_net.c`
implements them with BSD sockets or Winsock 2, exported like the rest of
host.h (`HOST_API`, wasm imports in the portable build). Plain C ints and
buffers; handles are small ints.

```c
int  host_net_local_addr(char *buf, int size);              /* "a.b.c.d" (first non-loopback IPv4) */
int  host_net_udp_open(int port);                           /* SO_BROADCAST|SO_REUSEADDR, bound */
int  host_net_udp_send(int s, uint32_t ip, int port, const void *p, int n);   /* ip 0xFFFFFFFF = broadcast */
int  host_net_udp_recv(int s, void *p, int n, uint32_t *from_ip);            /* -1 EWOULDBLOCK */
int  host_net_tcp_listen(int port);
int  host_net_tcp_accept(int s, uint32_t *peer_ip);          /* -1 none pending */
int  host_net_tcp_connect(uint32_t ip, int port);           /* non-blocking */
int  host_net_tcp_state(int s);                              /* connecting / connected / failed / closed */
int  host_net_tcp_send(int s, const void *p, int n);        /* bytes taken, -1 would block, -2 dead */
int  host_net_tcp_recv(int s, void *p, int n);              /* bytes, 0 closed, -1 would block */
void host_net_close(int s);                                  /* shutdown(SD_SEND) + close */
int  host_net_wait(int timeout_ms);                          /* optional: wake the idle loop on I/O */
```

- In the portable build, port_main.c forwards these to host_net.c,
  translating memory offsets as it does for the other imports.
- Browser builds would need a WebSocket relay. That is out of scope; the
  functions would just fail there.

**2. Platform layer: the XDaemon state machine.** A new
`src/platform/xdaemon.c` replaces the stubs in misc.c and keeps
xdaemon.h's API, with the prototypes corrected as in §7. It holds:

- The connection table (host 0, clients 1..6) and per-stream receive
  reassembly: the 12-byte header, `total` rounded to 64, and dropping
  padding and control tags.
- Per-stream send queues.
- The 0xFA id handshake, including the 500 ms delay before sending the
  id. That delay can be a non-blocking timer, since nobody can observe the
  difference. Also the connect timeout and the relay rules of §5.3.
- 0xFC → 0xFD ping replies, and the UDP header word.
- Padding written as zeros.

**3. Where the polling happens.** XDaemon's callbacks run when the game
pumps messages. We get the same effect by calling `xd_poll()` (non-blocking
accept, recv, send and timers) from the platform's message retrieval:

- Calling it in `PeekMessage`/`GetMessage` in `src/platform/message.c`, and
  from `XD_FlushSendBuffer`, reproduces exactly when the game sees its
  callbacks.
- `XD_FlushSendBuffer` must also keep dispatching the game's own window
  messages while it waits, as the original does.
- `GetMessage` waits on `host_net_wait` together with input, so a
  networked game doesn't spin.
- Replays stay deterministic: the replay harness never initialises the
  network.

**4. Testing.**

- Unit-test the framing and the relay with recorded streams: replay
  `run/xd/cap/*/capture.pcap` records against our parser and check the
  records come out identical (ignoring padding).
- Then use §11 with one side replaced by our build. Our build needs no
  Wine prefix trick, and it can run in the same container as an original
  as long as the IPs differ, or in Internet mode over 127.0.0.1.

## 13. Gaps and confidence

High confidence, both static and observed:

- Ports, framing, the 64-byte rounding, the tag layout, the id handshake.
- Host and client routing for a single client.
- UDP format and discovery.
- Disconnect behaviour and the API call sequence.

Static only:

- How the host relays a client's dst=0xFF and dst=client messages to
  *other* clients, including the rewrite to src = 0.  At the game level it
  works both ways round (§11.1 `--third`: an original host relaying
  between two of ours, ours between an original and ours: every client's
  messages to everyone reached both others).  The record tags of a
  3-player capture (`--sniff`, `tools/xdnet/relaytags.py records.txt`)
  agree, the original hosting and ours: a client's dst=0xFF record goes
  to each other client with tag src=0 dst=that client; one to another
  client is forwarded with its src kept; dst=0 is not forwarded.
- The oversize path, ping (unused) and the XDPING oddity.
- Behaviour when the connection table is full.

Not examined:

- The DirectPlay lobby path (we keep stubs).
- IPX, modem and serial (dropped).
- Very large maps' messages, e.g. NM_MAP_PACKETS sizes. Our games used the
  small 40×50 map.
- Simultaneous-move games and chat/parley traffic (game level only; no
  effect on the transport).
