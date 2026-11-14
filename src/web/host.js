// The browser's host for the game's module: the imports src/host/host.h
// describes (as host_sdl.c gives them natively) over a <canvas>, DOM input
// events, WebAudio and an in-page dialog, and the module's run made
// resumable.  The module (tools/build_web.py) is the portable build's
// civ2.wasm passed through Binaryen's asyncify: where the game blocks
// (waiting for input, sleeping, a message box, or busy with only quick looks
// at its input) the module's stack is unwound, the page's event loop runs
// (drawing, input, sound), and the stack is wound back to go on.

import {makeWasi, ExitStatus} from './wasi.js';

const EV = {NONE: 0, QUIT: 1, KEYDOWN: 2, KEYUP: 3, MOTION: 4, BUTTONDOWN: 5, BUTTONUP: 6, EXPOSED: 7, COMMAND: 8,
            WHEEL: 9, RESIZE: 10};
const ASYNC_SIZE = 1 << 20;        // the unwound stack's room
const YIELD_MS = 25;               // the longest the game keeps the page busy without a look at its input

// ---- keys: KeyboardEvent -> Windows virtual-key code (as host_sdl.c's vk_of_key) ----

const VK = {Enter: 0x0D, Escape: 0x1B, Backspace: 0x08, Tab: 0x09, Space: 0x20, ArrowUp: 0x26, ArrowDown: 0x28,
            ArrowLeft: 0x25, ArrowRight: 0x27, Home: 0x24, End: 0x23, PageUp: 0x21, PageDown: 0x22, Insert: 0x2D,
            Delete: 0x2E, ShiftLeft: 0xA0, ShiftRight: 0xA1, ControlLeft: 0xA2, ControlRight: 0xA3, AltLeft: 0xA4,
            AltRight: 0xA5, CapsLock: 0x14, NumpadEnter: 0x0D, NumpadAdd: 0x6B, NumpadSubtract: 0x6D,
            NumpadMultiply: 0x6A, NumpadDivide: 0x6F, Minus: 0xBD, Equal: 0xBB, BracketLeft: 0xDB,
            BracketRight: 0xDD, Semicolon: 0xBA, Quote: 0xDE, Backquote: 0xC0, Backslash: 0xDC, Comma: 0xBC,
            Period: 0xBE, Slash: 0xBF};
// the keypad without Num Lock: what its keys are then (e.key)
const KEYPAD_NAV = {Insert: 0x2D, End: 0x23, ArrowDown: 0x28, PageDown: 0x22, ArrowLeft: 0x25, Clear: 0x0C,
                    ArrowRight: 0x27, Home: 0x24, ArrowUp: 0x26, PageUp: 0x21, Delete: 0x2E};

export function vkOf(e) {
    const c = e.code || '';
    if (/^Numpad[0-9]$/.test(c) || c === 'NumpadDecimal') {
        if (/^[0-9.,]$/.test(e.key))
            return c === 'NumpadDecimal' ? 0x6E : 0x60 + +c.slice(6);
        return KEYPAD_NAV[e.key] || (c === 'Numpad5' ? 0x0C : 0);
    }
    if (e.key && e.key.length === 1 && /[a-z0-9]/i.test(e.key))
        return e.key.toUpperCase().charCodeAt(0);
    if (/^Key[A-Z]$/.test(c))
        return c.charCodeAt(3);
    if (/^Digit[0-9]$/.test(c))
        return c.charCodeAt(5);
    if (/^F([1-9]|1[0-2])$/.test(c))
        return 0x70 + +c.slice(1) - 1;
    return VK[c] || 0;
}

// ---- the mouse wheel: notches, fractions added up (as host_sdl.c) ----

export class Wheel {
    constructor() { this.rest = 0; }
    // positive away from the user; deltaMode 0 pixels (100 a notch), 1 lines (3 a notch), 2 pages
    notches(deltaY, deltaMode) {
        const y = -deltaY / (deltaMode === 1 ? 3 : deltaMode === 2 ? 1 : 100);
        if ((this.rest > 0 && y < 0) || (this.rest < 0 && y > 0))
            this.rest = 0;
        this.rest += y;
        const n = Math.trunc(this.rest) | 0;
        this.rest -= n;
        return n;
    }
}

// host.h's host_desktop_scale
export function desktopScale(w, h, scale) {
    const most = Math.min(Math.floor(w / 640), Math.floor(h / 480));
    if (scale < 1) {
        let best = 0;
        scale = 1;
        for (let k = 1; k <= most && (k === 1 || (Math.floor(w / k) >= 800 && Math.floor(h / k) >= 560)); k++) {
            let r = Math.floor(w / k) * Math.floor(h / k) / (1024 * 768);
            if (r < 1)
                r = 1 / r;
            if (k === 1 || r < best) {
                best = r;
                scale = k;
            }
        }
    }
    if (scale > most)
        scale = most;
    return scale < 1 ? 1 : scale;
}

// ARGB (the game's 0xAARRGGBB words) -> the canvas's RGBA bytes, opaque
export function argbToRgba(src, dst, n) {
    for (let i = 0; i < n; i++) {
        const p = src[i];
        dst[i] = 0xff000000 | (p & 0xff00) | ((p & 0xff) << 16) | ((p >>> 16) & 0xff);
    }
}

// ---- the run ----

// opts: {wasm: ArrayBuffer|Response (or a promise of one), fs: MemFS, canvas, args, env, messageBox(icon, title, text, labels, ids,
// enter, escape) -> Promise<id>, stats: {} (filled in as it runs: busyMs, resumes)}
// -> the game's exit code
export async function runGame(opts) {
    const {canvas, fs} = opts;
    const ctx = canvas.getContext('2d', {alpha: false});
    const latin = new TextDecoder('windows-1252');
    let exports, memory, asyncData = 0;
    const dv = () => new DataView(memory.buffer);
    const cstr = (p) => {
        const m = new Uint8Array(memory.buffer);
        let e = p;
        while (m[e])
            e++;
        return latin.decode(m.subarray(p, e));
    };

    // -- input: events queued for the game; a wait resolved by the next one
    const queue = [];
    let waiter = null;
    const push = (type, vk = 0, x = 0, y = 0, button = 0) => {
        queue.push([type, vk, x, y, button]);
        if (waiter)
            waiter();
    };
    const take = (out) => {
        const e = queue.shift();
        const v = dv();
        e.forEach((x, i) => v.setInt32(out + 4 * i, x, true));
        return 1;
    };

    // -- the desktop on the canvas: w x h desktop pixels, shown at a whole
    //    number of device pixels each where it fits, else shrunk
    let screenW = 640, screenH = 480, follows = false, image = null;
    const dpr = () => window.devicePixelRatio || 1;
    const viewport = () => [Math.floor(window.innerWidth * dpr()), Math.floor(window.innerHeight * dpr())];
    const fit = () => {
        const [vw, vh] = viewport();
        let s = Math.min(vw / screenW, vh / screenH);
        if (s >= 1)
            s = Math.floor(s);
        canvas.style.width = (screenW * s / dpr()) + 'px';
        canvas.style.height = (screenH * s / dpr()) + 'px';
        cursorCache.clear();
        applyCursor();
    };
    const setDesktop = (w, h) => {
        screenW = w;
        screenH = h;
        canvas.width = w;
        canvas.height = h;
        image = ctx.createImageData(w, h);
        fit();
    };
    window.addEventListener('resize', () => {
        fit();
        if (follows) {
            const [vw, vh] = viewport();
            push(EV.RESIZE, 0, Math.max(vw, 640), Math.max(vh, 480));
        }
    });
    document.addEventListener('visibilitychange', () => {
        if (!document.hidden)
            push(EV.EXPOSED);
    });
    const desktopXY = (e) => {
        const r = canvas.getBoundingClientRect();
        return [Math.floor((e.clientX - r.left) * screenW / r.width), Math.floor((e.clientY - r.top) * screenH / r.height)];
    };
    const button = (b) => (b === 0 ? 1 : b === 2 ? 2 : 3);
    canvas.addEventListener('pointermove', (e) => {
        const [x, y] = desktopXY(e);
        push(EV.MOTION, 0, x, y);
    });
    canvas.addEventListener('pointerdown', (e) => {
        canvas.focus();
        canvas.setPointerCapture(e.pointerId);
        resumeAudio();
        const [x, y] = desktopXY(e);
        push(EV.BUTTONDOWN, 0, x, y, button(e.button));
        e.preventDefault();
    });
    canvas.addEventListener('pointerup', (e) => {
        const [x, y] = desktopXY(e);
        push(EV.BUTTONUP, 0, x, y, button(e.button));
        e.preventDefault();
    });
    canvas.addEventListener('contextmenu', (e) => e.preventDefault());
    const wheel = new Wheel();
    canvas.addEventListener('wheel', (e) => {
        const n = wheel.notches(e.deltaY, e.deltaMode);
        if (n)
            push(EV.WHEEL, 0, 0, 0, n);
        e.preventDefault();
    }, {passive: false});
    const held = new Set();
    const key = (e, down) => {
        if (e.metaKey || dialogOpen)
            return;         // the system's (Command) shortcuts; the message box's own keys
        const vk = vkOf(e);
        if (!vk)
            return;
        e.preventDefault();
        if (down)
            held.add(vk);
        else
            held.delete(vk);
        push(down ? EV.KEYDOWN : EV.KEYUP, vk);
    };
    window.addEventListener('keydown', (e) => {
        resumeAudio();
        key(e, true);
    });
    window.addEventListener('keyup', (e) => key(e, false));
    window.addEventListener('blur', () => {
        // keys let go while the page was not looking: up, not stuck down
        for (const vk of held)
            push(EV.KEYUP, vk);
        held.clear();
    });

    // -- cursors: CSS cursors, the game's images scaled as the desktop is
    const SYSTEM_CURSORS = ['default', 'text', 'wait', 'crosshair', 'nwse-resize', 'nesw-resize', 'ew-resize',
                            'ns-resize', 'move', 'not-allowed', 'pointer'];
    const cursors = [null];
    const cursorCache = new Map();
    let cursorNow = 0, cursorShown = true;
    const cursorSlot = (c) => {
        let i = cursors.indexOf(undefined, 1);
        if (i < 0)
            i = cursors.length;
        cursors[i] = c;
        return i;
    };
    const cursorCss = (c) => {
        if (c.css)
            return c.css;
        const r = canvas.getBoundingClientRect();
        const k = Math.max(1, Math.round(r.width / screenW));
        const key = c.id + ':' + k;
        if (!cursorCache.has(key)) {
            const s = (c.w * k <= 128 && c.h * k <= 128) ? k : 1;
            const cv = document.createElement('canvas');
            cv.width = c.w * s;
            cv.height = c.h * s;
            const g = cv.getContext('2d');
            const src = document.createElement('canvas');
            src.width = c.w;
            src.height = c.h;
            src.getContext('2d').putImageData(new ImageData(c.rgba, c.w, c.h), 0, 0);
            g.imageSmoothingEnabled = false;
            g.drawImage(src, 0, 0, cv.width, cv.height);
            cursorCache.set(key, `url(${cv.toDataURL()}) ${c.x * s} ${c.y * s}, default`);
        }
        return cursorCache.get(key);
    };
    let cursorIds = 0;
    function applyCursor() {
        const c = cursors[cursorNow];
        canvas.style.cursor = !cursorShown ? 'none' : c ? cursorCss(c) : 'default';
    }

    // -- sound: PCM queued on WebAudio sources, one after the other
    let audio = null;
    const devices = [null];
    function resumeAudio() {
        if (audio && audio.state === 'suspended')
            audio.resume();
    }
    const audioOk = () => audio && audio.state === 'running';
    const music = {source: null, playing: false, token: 0};

    // -- the module's imports
    const host = {
        host_init: () => 1,
        host_quit: () => {},
        host_base_path: (buf, size) => {
            if (size)
                new Uint8Array(memory.buffer)[buf] = 0;
            return 0;
        },
        host_open_window: (w, h) => {
            setDesktop(w, h);
            return 1;
        },
        host_open_maximized_window: (pw, ph, scale) => {
            let [cw, ch] = viewport();
            if (cw < 640 || ch < 480) {
                cw = 1024;
                ch = 768;
            }
            const k = desktopScale(cw, ch, scale);
            setDesktop(Math.floor(cw / k), Math.floor(ch / k));
            follows = true;
            dv().setInt32(pw, screenW, true);
            dv().setInt32(ph, screenH, true);
            return k;
        },
        host_open_native_window: () => 0,
        host_display_size: (pw, ph) => {
            dv().setInt32(pw, Math.floor(screen.width * dpr()), true);
            dv().setInt32(ph, Math.floor(screen.height * dpr()), true);
            return 1;
        },
        host_usable_size: (pw, ph) => {
            const [w, h] = viewport();
            dv().setInt32(pw, w, true);
            dv().setInt32(ph, h, true);
            return 1;
        },
        host_menu_begin: () => {},
        host_menu_add: () => {},
        host_menu_end: () => {},
        // HD graphics (docs/HD-GRAPHICS.md) are not in the browser version:
        // the game never calls this here (no pack); the module imports it.
        host_present_hd: () => {},
        host_present: (pixels, stride) => {
            if (!image)
                return;
            const dst = new Uint32Array(image.data.buffer);
            if (stride === screenW * 4)
                argbToRgba(new Uint32Array(memory.buffer, pixels, screenW * screenH), dst, screenW * screenH);
            else
                for (let y = 0; y < screenH; y++)
                    argbToRgba(new Uint32Array(memory.buffer, pixels + y * stride, screenW),
                               dst.subarray(y * screenW), screenW);
            ctx.putImageData(image, 0, 0);
        },
        host_resize_desktop: (w, h) => {
            if (w !== screenW || h !== screenH)
                setDesktop(w, h);
        },
        host_minimize: () => {},
        host_time_us: () => BigInt(Math.round(performance.now() * 1000)),
        host_cursor_system: (kind) => cursorSlot({css: SYSTEM_CURSORS[kind] || 'default'}),
        host_cursor_image: (argb, w, h, x, y) => {
            const rgba = new Uint8ClampedArray(w * h * 4);
            const src = new Uint32Array(memory.buffer, argb, w * h), dst = new Uint32Array(rgba.buffer);
            for (let i = 0; i < w * h; i++) {
                const p = src[i];
                dst[i] = (p & 0xff00ff00) | ((p & 0xff) << 16) | ((p >>> 16) & 0xff);
            }
            return cursorSlot({id: ++cursorIds, rgba, w, h, x, y});
        },
        host_cursor_set: (c) => {
            if (cursors[c]) {
                cursorNow = c;
                applyCursor();
            }
        },
        host_cursor_free: (c) => {
            if (c > 0 && cursors[c]) {
                cursors[c] = undefined;
                if (cursorNow === c)
                    cursorNow = 0;
            }
        },
        host_cursor_show: (show) => {
            cursorShown = !!show;
            applyCursor();
        },
        host_audio_init: () => {
            try {
                audio = audio || new AudioContext();
                return 1;
            } catch (e) {
                return 0;
            }
        },
        host_audio_open: (freq, ch, bits) => {
            if (!audio)
                return 0;
            return devices.push({freq, ch, bits, sources: new Set(), end: 0}) - 1;
        },
        host_audio_queue: (dev, data, len) => {
            const d = devices[dev];
            // (without a running context nothing would play, nor ever be done)
            if (!d || !audioOk())
                return;
            const bps = d.bits === 8 ? 1 : 2, frames = Math.floor(len / (bps * d.ch));
            if (!frames)
                return;
            const buf = audio.createBuffer(d.ch, frames, d.freq);
            const bytes = new Uint8Array(memory.buffer, data, frames * bps * d.ch);
            const words = bps === 2 ? new Int16Array(bytes.slice().buffer) : null;
            for (let c = 0; c < d.ch; c++) {
                const out = buf.getChannelData(c);
                for (let i = 0; i < frames; i++)
                    out[i] = bps === 2 ? words[i * d.ch + c] / 32768 : (bytes[i * d.ch + c] - 128) / 128;
            }
            const src = audio.createBufferSource();
            src.buffer = buf;
            src.connect(audio.destination);
            const at = Math.max(audio.currentTime, d.end);
            src.start(at);
            d.end = at + buf.duration;
            d.sources.add(src);
            src.onended = () => d.sources.delete(src);
        },
        host_audio_queued: (dev) => {
            const d = devices[dev];
            if (!d || !audioOk())
                return 0;
            const left = d.end - audio.currentTime;
            return left > 0 ? Math.ceil(left * d.freq) * d.ch * (d.bits === 8 ? 1 : 2) : 0;
        },
        host_audio_clear: (dev) => {
            const d = devices[dev];
            if (!d)
                return;
            for (const s of d.sources)
                try { s.stop(); } catch (e) { /* not started */ }
            d.sources.clear();
            d.end = 0;
        },
        host_audio_close: (dev) => {
            host.host_audio_clear(dev);
            devices[dev] = null;
        },
        host_music_play: (file, len) => {
            host.host_music_stop();
            if (!audio)
                return 0;
            const token = ++music.token;
            const bytes = new Uint8Array(memory.buffer, file, len).slice();
            music.playing = true;
            audio.decodeAudioData(bytes.buffer).then((buf) => {
                if (token !== music.token)
                    return;
                const src = audio.createBufferSource();
                src.buffer = buf;
                src.connect(audio.destination);
                src.onended = () => {
                    if (token === music.token)
                        music.playing = false;
                };
                src.start();
                music.source = src;
            }, () => {
                if (token === music.token)
                    music.playing = false;
            });
            return 1;
        },
        host_music_stop: () => {
            music.token++;
            music.playing = false;
            if (music.source)
                try { music.source.stop(); } catch (e) { /* not started */ }
            music.source = null;
        },
        host_music_playing: () => (music.playing ? 1 : 0),
        // network play: not in the browser
        host_net_local_addr: () => -1,
        host_net_udp_open: () => -1,
        host_net_udp_send: () => -1,
        host_net_udp_recv: () => -1,
        host_net_tcp_open: () => -1,
        host_net_tcp_listen: () => -1,
        host_net_tcp_accept: () => -1,
        host_net_tcp_connect: () => -1,
        host_net_tcp_state: () => -1,
        host_net_tcp_send: () => -2,
        host_net_tcp_recv: () => -2,
        host_net_close: () => {},
    };

    // -- the imports that may block: a promise unwinds the module's stack,
    //    its value is what the import returns once the stack is wound back
    let lastYield = performance.now();
    let pending = null, resumeValue = 0, dialogOpen = false;
    // (a message to itself: the page's turn at once, where setTimeout would
    // wait 4 ms once timeouts nest, as these do)
    const channel = new MessageChannel(), soonQueue = [];
    channel.port1.onmessage = () => soonQueue.shift()();
    const yieldNow = () => new Promise((resolve) => {
        soonQueue.push(resolve);
        channel.port2.postMessage(0);
    });
    const sleepMs = async (ms) => {
        if (ms > 4)
            return new Promise((resolve) => setTimeout(resolve, ms));
        const end = performance.now() + ms;
        do
            await yieldNow();
        while (performance.now() < end);
    };
    const blocking = {
        host_poll_event: (out) => {
            if (queue.length)
                return take(out);
            if (performance.now() - lastYield < YIELD_MS)
                return 0;
            return yieldNow().then(() => () => (queue.length ? take(out) : 0));
        },
        host_wait_event: (out, timeout) => {
            if (queue.length)
                return take(out);
            if (timeout === 0)
                return blocking.host_poll_event(out);
            return new Promise((resolve) => {
                const t = timeout > 0 ? setTimeout(() => {
                    waiter = null;
                    resolve();
                }, timeout) : 0;
                waiter = () => {
                    clearTimeout(t);
                    waiter = null;
                    resolve();
                };
            }).then(() => () => (queue.length ? take(out) : 0));
        },
        host_delay_ms: (ms) => sleepMs(ms).then(() => () => 0),
        host_message_box: (icon, title, text, n, labels, ids, enter, escape) => {
            const l = [], id = [];
            for (let i = 0; i < Math.min(n, 8); i++) {
                l.push(cstr(dv().getUint32(labels + 4 * i, true)));
                id.push(dv().getInt32(ids + 4 * i, true));
            }
            dialogOpen = true;
            return opts.messageBox(icon, cstr(title), cstr(text), l, id, enter, escape).then((r) => () => {
                dialogOpen = false;
                return r;
            });
        },
    };
    for (const [name, fn] of Object.entries(blocking)) {
        host[name] = (...args) => {
            if (exports.asyncify_get_state() === 2) {      // winding back: the waited-for result
                exports.asyncify_stop_rewind();
                return resumeValue();
            }
            const r = fn(...args);
            if (!(r instanceof Promise))
                return r;
            const v = dv();
            v.setUint32(asyncData, asyncData + 8, true);
            v.setUint32(asyncData + 4, asyncData + ASYNC_SIZE, true);
            exports.asyncify_start_unwind(asyncData);
            pending = r;
            return 0;
        };
    }

    const wasi = makeWasi(fs, opts.args, opts.env, () => memory);
    const imports = {host, wasi_snapshot_preview1: wasi};
    const wasm = await opts.wasm;
    const {instance} = wasm instanceof Response
        ? await WebAssembly.instantiateStreaming(wasm.clone(), imports).catch(
            async () => WebAssembly.instantiate(await wasm.arrayBuffer(), imports))
        : await WebAssembly.instantiate(wasm, imports);
    exports = instance.exports;
    memory = exports.memory;
    asyncData = exports.malloc(ASYNC_SIZE);
    if (!asyncData)
        throw new Error('no memory for the module');

    // -- the run: _start, and again after each wait, until the game exits
    let code = 0;
    const stats = opts.stats || {};
    stats.busyMs = 0;           // time in the game's code
    stats.resumes = 0;          // its stack wound back after a wait
    for (;;) {
        const t = performance.now();
        try {
            exports._start();
        } catch (e) {
            if (e instanceof ExitStatus) {
                code = e.code;
                break;
            }
            throw e;
        } finally {
            stats.busyMs += performance.now() - t;
        }
        if (exports.asyncify_get_state() !== 1)
            break;              // returned from main
        exports.asyncify_stop_unwind();
        stats.resumes++;
        resumeValue = await pending;
        lastYield = performance.now();
        exports.asyncify_start_rewind(asyncData);
    }
    host.host_music_stop();
    return code;
}
