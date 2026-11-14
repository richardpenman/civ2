// The set-up from the game's CD or a disc image (setup_run.js) in a Web
// Worker: the player's files (File objects) read synchronously
// (FileReaderSync) as the set-up asks for them, so that a disc image of
// 700 MB is never in memory whole.  Messages to the page: {progress: [what,
// detail]}, {message: [text, kind]}, {file: path, data} (transferred), then
// {done: exit code} or {error: text}.  Stopped by the page (terminate).

import {runSetup} from './setup_run.js';

self.onmessage = async (e) => {
    const {files, source} = e.data;
    try {
        const module = await WebAssembly.compileStreaming(fetch('setup.wasm')).catch(
            async () => WebAssembly.compile(await (await fetch('setup.wasm')).arrayBuffer()));
        const reader = new FileReaderSync();
        let shown = 0;
        const code = runSetup({
            module,
            source,
            files: files.map((f) => ({name: f.name, size: f.file.size, mtime: f.file.lastModified,
                                      readAt: (pos, n) => new Uint8Array(reader.readAsArrayBuffer(f.file.slice(pos, pos + n)))})),
            onFile: (path, data) => self.postMessage({file: path, data}, [data.buffer]),
            onProgress: (what, detail) => {
                // (the same text again: shown at most every 100 ms)
                const now = performance.now();
                if (now - shown >= 100) {
                    shown = now;
                    self.postMessage({progress: [what, detail]});
                }
                return false;
            },
            onMessage: (text, kind) => self.postMessage({message: [text, kind]}),
            log: (fd, line) => (fd === 2 ? console.warn : console.log)(line),
        });
        self.postMessage({done: code});
    } catch (err) {
        self.postMessage({error: err.message || String(err)});
    }
};
