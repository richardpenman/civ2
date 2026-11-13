// The part of WASI (preview 1) the game's module imports (as src/host/wasi.c
// gives it over POSIX), over a file system in memory: the player's game files
// under /civ2, the program's own (the fonts) under /data.  Files the game
// writes are reported (onchange) so that the page keeps them (IndexedDB).

const W = {SUCCESS: 0, BADF: 8, EXIST: 20, INVAL: 28, IO: 29, ISDIR: 31, MFILE: 33, NOENT: 44, NOTDIR: 54,
           NOTEMPTY: 55, NOSYS: 52};
const FILETYPE = {DIR: 3, FILE: 4, CHAR: 2};

export class ExitStatus extends Error {
    constructor(code) {
        super('exit ' + code);
        this.code = code;
    }
}

// A path made absolute and plain: no ".", "..", empty or trailing parts.
export function normalize(path) {
    const out = [];
    for (const p of path.split('/')) {
        if (p === '' || p === '.')
            continue;
        if (p === '..')
            out.pop();
        else
            out.push(p);
    }
    return '/' + out.join('/');
}

function parent(path) {
    const i = path.lastIndexOf('/');
    return i <= 0 ? '/' : path.slice(0, i);
}

// Files: path -> {data: Uint8Array (capacity), size, mtime}; directories: a set.
export class MemFS {
    constructor() {
        this.files = new Map();
        this.dirs = new Set(['/']);
        this.onchange = null;       // (path, removed) after a file is written, renamed or removed
    }
    mkdirs(path) {
        path = normalize(path);
        while (path !== '/' && !this.dirs.has(path)) {
            this.dirs.add(path);
            path = parent(path);
        }
    }
    put(path, bytes, mtime = Date.now()) {
        path = normalize(path);
        this.mkdirs(parent(path));
        this.files.set(path, {data: bytes, size: bytes.length, mtime});
    }
    get(path) {
        const f = this.files.get(normalize(path));
        return f ? f.data.subarray(0, f.size) : null;
    }
    // Entries of a directory: [name, isDir].
    list(dir) {
        const pre = dir === '/' ? '/' : dir + '/', out = [];
        for (const d of this.dirs)
            if (d !== dir && d.startsWith(pre) && !d.slice(pre.length).includes('/'))
                out.push([d.slice(pre.length), true]);
        for (const f of this.files.keys())
            if (f.startsWith(pre) && !f.slice(pre.length).includes('/'))
                out.push([f.slice(pre.length), false]);
        return out;
    }
    changed(path, removed = false) {
        if (this.onchange)
            this.onchange(path, removed);
    }
}

// The WASI imports over fs; env: {NAME: value}; args: strings.  memory()
// returns the module's memory (known once it is instantiated).
export function makeWasi(fs, args, env, memory, log = (fd, s) => (fd === 2 ? console.warn : console.log)(s)) {
    const enc = new TextEncoder(), dec = new TextDecoder();
    const envs = Object.entries(env).map(([k, v]) => k + '=' + v);
    // descriptors: 0-2 the console, 3 the root ("/"), then files and directories
    const fds = [{con: 0}, {con: 1}, {con: 2}, {dir: '/'}];
    const lines = ['', '', ''];
    const dv = () => new DataView(memory().buffer);
    const u8 = () => new Uint8Array(memory().buffer);
    const str = (p, n) => dec.decode(u8().subarray(p, p + n));
    const fdNew = (o) => {
        for (let i = 4; i < 1024; i++)
            if (!fds[i]) {
                fds[i] = o;
                return i;
            }
        return -1;
    };
    const resolve = (dirfd, p, n) => {
        const d = fds[dirfd];
        if (!d || d.dir === undefined)
            return null;
        return normalize(d.dir + '/' + str(p, n));
    };
    const putStat = (buf, isDir, size, mtime) => {
        const v = dv();
        for (let i = 0; i < 64; i += 4)
            v.setUint32(buf + i, 0, true);
        v.setUint8(buf + 16, isDir ? FILETYPE.DIR : FILETYPE.FILE);
        v.setBigUint64(buf + 24, 1n, true);
        v.setBigUint64(buf + 32, BigInt(size), true);
        const ns = BigInt(Math.floor(mtime)) * 1000000n;
        v.setBigUint64(buf + 40, ns, true);
        v.setBigUint64(buf + 48, ns, true);
        v.setBigUint64(buf + 56, ns, true);
    };
    const ensure = (f, n) => {
        if (f.data.length < n) {
            const d = new Uint8Array(Math.max(n, f.data.length * 2, 4096));
            d.set(f.data.subarray(0, f.size));
            f.data = d;
        }
    };
    const iovs = (ptr, n) => {
        const v = dv(), out = [];
        for (let i = 0; i < n; i++)
            out.push([v.getUint32(ptr + 8 * i, true), v.getUint32(ptr + 8 * i + 4, true)]);
        return out;
    };
    const wasi = {
        args_sizes_get(argc, size) {
            dv().setUint32(argc, args.length, true);
            dv().setUint32(size, args.reduce((n, a) => n + enc.encode(a).length + 1, 0), true);
            return 0;
        },
        args_get(argv, buf) {
            args.forEach((a, i) => {
                const b = enc.encode(a + '\0');
                dv().setUint32(argv + 4 * i, buf, true);
                u8().set(b, buf);
                buf += b.length;
            });
            return 0;
        },
        environ_sizes_get(count, size) {
            dv().setUint32(count, envs.length, true);
            dv().setUint32(size, envs.reduce((n, a) => n + enc.encode(a).length + 1, 0), true);
            return 0;
        },
        environ_get(envp, buf) {
            envs.forEach((a, i) => {
                const b = enc.encode(a + '\0');
                dv().setUint32(envp + 4 * i, buf, true);
                u8().set(b, buf);
                buf += b.length;
            });
            return 0;
        },
        clock_time_get(id, precision, out) {
            const ns = id === 0 ? BigInt(Date.now()) * 1000000n
                                : BigInt(Math.round((performance.timeOrigin + performance.now()) * 1000)) * 1000n;
            dv().setBigUint64(out, ns, true);
            return 0;
        },
        proc_exit(code) {
            throw new ExitStatus(code);
        },
        fd_prestat_get(fd, buf) {
            if (fd !== 3)
                return W.BADF;
            dv().setUint32(buf, 0, true);
            dv().setUint32(buf + 4, 1, true);
            return 0;
        },
        fd_prestat_dir_name(fd, path, len) {
            if (fd !== 3)
                return W.BADF;
            if (len)
                u8()[path] = 47;    // "/"
            return 0;
        },
        fd_fdstat_get(fd, buf) {
            const f = fds[fd];
            if (!f)
                return W.BADF;
            const v = dv();
            for (let i = 0; i < 24; i += 4)
                v.setUint32(buf + i, 0, true);
            v.setUint8(buf, f.dir !== undefined ? FILETYPE.DIR : f.con !== undefined ? FILETYPE.CHAR : FILETYPE.FILE);
            v.setBigUint64(buf + 8, 0xffffffffffffffffn, true);
            v.setBigUint64(buf + 16, 0xffffffffffffffffn, true);
            return 0;
        },
        fd_fdstat_set_flags(fd) {
            return fds[fd] ? 0 : W.BADF;
        },
        fd_filestat_get(fd, buf) {
            const f = fds[fd];
            if (!f)
                return W.BADF;
            if (f.dir !== undefined)
                putStat(buf, true, 0, Date.now());
            else if (f.con !== undefined)
                putStat(buf, false, 0, Date.now());
            else
                putStat(buf, false, f.file.size, f.file.mtime);
            return 0;
        },
        path_filestat_get(dirfd, flags, path, len, buf) {
            const p = resolve(dirfd, path, len);
            if (p === null)
                return W.BADF;
            if (fs.dirs.has(p)) {
                putStat(buf, true, 0, Date.now());
                return 0;
            }
            const f = fs.files.get(p);
            if (!f)
                return W.NOENT;
            putStat(buf, false, f.size, f.mtime);
            return 0;
        },
        path_open(dirfd, dirflags, path, len, oflags, rights, inheriting, fdflags, out) {
            const p = resolve(dirfd, path, len);
            if (p === null)
                return W.BADF;
            if (p === '/dev/null') {
                const fd = fdNew({con: 0});
                if (fd < 0)
                    return W.MFILE;
                dv().setUint32(out, fd, true);
                return 0;
            }
            const creat = oflags & 1, directory = oflags & 2, excl = oflags & 4, trunc = oflags & 8;
            if (directory || (!creat && fs.dirs.has(p))) {
                if (!fs.dirs.has(p))
                    return fs.files.has(p) ? W.NOTDIR : W.NOENT;
                const fd = fdNew({dir: p});
                if (fd < 0)
                    return W.MFILE;
                dv().setUint32(out, fd, true);
                return 0;
            }
            if (fs.dirs.has(p))
                return W.ISDIR;
            let file = fs.files.get(p);
            if (file && creat && excl)
                return W.EXIST;
            if (!file) {
                if (!creat)
                    return W.NOENT;
                if (!fs.dirs.has(parent(p)))
                    return W.NOENT;
                file = {data: new Uint8Array(0), size: 0, mtime: Date.now()};
                fs.files.set(p, file);
                fs.changed(p);
            }
            const writing = (rights & (1n << 6n)) !== 0n;
            if (trunc && file.size) {
                file.size = 0;
                file.mtime = Date.now();
                fs.changed(p);
            }
            const fd = fdNew({path: p, file, pos: 0, append: (fdflags & 1) !== 0, writing, dirty: false});
            if (fd < 0)
                return W.MFILE;
            dv().setUint32(out, fd, true);
            return 0;
        },
        fd_close(fd) {
            const f = fds[fd];
            if (!f)
                return W.BADF;
            if (fd < 4)
                return 0;
            if (f.dirty)
                fs.changed(f.path);
            fds[fd] = undefined;
            return 0;
        },
        fd_read(fd, iov, n, done) {
            const f = fds[fd];
            if (!f || f.dir !== undefined)
                return W.BADF;
            let total = 0;
            if (f.file) {
                const mem = u8();
                for (const [p, l] of iovs(iov, n)) {
                    const k = Math.max(0, Math.min(l, f.file.size - f.pos));
                    mem.set(f.file.data.subarray(f.pos, f.pos + k), p);
                    f.pos += k;
                    total += k;
                    if (k < l)
                        break;
                }
            }
            dv().setUint32(done, total, true);
            return 0;
        },
        fd_write(fd, iov, n, done) {
            const f = fds[fd];
            if (!f || f.dir !== undefined)
                return W.BADF;
            let total = 0;
            const mem = u8();
            for (const [p, l] of iovs(iov, n)) {
                if (f.con !== undefined) {
                    if (f.con === 0) {          // (/dev/null)
                        total += l;
                        continue;
                    }
                    lines[f.con] += dec.decode(mem.subarray(p, p + l));
                    let i;
                    while ((i = lines[f.con].indexOf('\n')) >= 0) {
                        log(f.con, lines[f.con].slice(0, i));
                        lines[f.con] = lines[f.con].slice(i + 1);
                    }
                } else {
                    const file = f.file;
                    if (f.append)
                        f.pos = file.size;
                    ensure(file, f.pos + l);
                    if (f.pos > file.size)
                        file.data.fill(0, file.size, f.pos);
                    file.data.set(mem.subarray(p, p + l), f.pos);
                    f.pos += l;
                    if (f.pos > file.size)
                        file.size = f.pos;
                    file.mtime = Date.now();
                    f.dirty = true;
                    fs.changed(f.path);
                }
                total += l;
            }
            dv().setUint32(done, total, true);
            return 0;
        },
        fd_seek(fd, offset, whence, out) {
            const f = fds[fd];
            if (!f || !f.file)
                return f ? 70 /* SPIPE */ : W.BADF;
            const base = whence === 0 ? 0 : whence === 1 ? f.pos : f.file.size;
            const pos = base + Number(BigInt.asIntN(64, offset));
            if (pos < 0)
                return W.INVAL;
            f.pos = pos;
            dv().setBigUint64(out, BigInt(pos), true);
            return 0;
        },
        // Entries from position cookie: 24-byte headers, each followed by
        // its name; one that does not fit is cut short (the reader retries).
        fd_readdir(fd, buf, len, cookie, used) {
            const f = fds[fd];
            if (!f || f.dir === undefined)
                return W.BADF;
            const entries = [['.', true], ['..', true], ...fs.list(f.dir)];
            const mem = u8();
            let pos = 0;
            for (let k = Number(cookie); k < entries.length && pos < len; k++) {
                const name = enc.encode(entries[k][0]);
                const hdr = new DataView(new ArrayBuffer(24));
                hdr.setBigUint64(0, BigInt(k + 1), true);
                hdr.setBigUint64(8, BigInt(k + 1), true);
                hdr.setUint32(16, name.length, true);
                hdr.setUint8(20, entries[k][1] ? FILETYPE.DIR : FILETYPE.FILE);
                let m = Math.min(24, len - pos);
                mem.set(new Uint8Array(hdr.buffer, 0, m), buf + pos);
                pos += m;
                m = Math.min(name.length, len - pos);
                mem.set(name.subarray(0, m), buf + pos);
                pos += m;
            }
            dv().setUint32(used, pos, true);
            return 0;
        },
        path_create_directory(dirfd, path, len) {
            const p = resolve(dirfd, path, len);
            if (p === null)
                return W.BADF;
            if (fs.dirs.has(p) || fs.files.has(p))
                return W.EXIST;
            if (!fs.dirs.has(parent(p)))
                return W.NOENT;
            fs.dirs.add(p);
            return 0;
        },
        path_remove_directory(dirfd, path, len) {
            const p = resolve(dirfd, path, len);
            if (p === null)
                return W.BADF;
            if (!fs.dirs.has(p))
                return fs.files.has(p) ? W.NOTDIR : W.NOENT;
            if (fs.list(p).length)
                return W.NOTEMPTY;
            fs.dirs.delete(p);
            return 0;
        },
        path_unlink_file(dirfd, path, len) {
            const p = resolve(dirfd, path, len);
            if (p === null)
                return W.BADF;
            if (fs.dirs.has(p))
                return W.ISDIR;
            if (!fs.files.delete(p))
                return W.NOENT;
            fs.changed(p, true);
            return 0;
        },
        path_rename(fd, old, oldLen, fd2, nu, nuLen) {
            const a = resolve(fd, old, oldLen), b = resolve(fd2, nu, nuLen);
            if (a === null || b === null)
                return W.BADF;
            const f = fs.files.get(a);
            if (!f)
                return fs.dirs.has(a) ? W.NOSYS : W.NOENT;
            if (!fs.dirs.has(parent(b)))
                return W.NOENT;
            fs.files.delete(a);
            fs.files.set(b, f);
            fs.changed(a, true);
            fs.changed(b);
            return 0;
        },
    };
    return wasi;
}
