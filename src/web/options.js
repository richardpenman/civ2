// The game's OpenCiv2 Options (src/game/options.cpp; docs/EXTRAS.md
// "OpenCiv2 Options") as the page reads them: CIV.INI's [OpenCiv2] section,
// which the game writes and the browser keeps with the game's files.  What
// the page needs to know before the game starts: whether it wants HD
// graphics (their art, which the page gets: main.js).

const SECTION = 'OpenCiv2';

// ---- CIV.INI (Windows' profile files: sections and keys in any case) ----

export function iniGet(text, section, key) {
    let inSection = false;
    for (const raw of text.split(/\r?\n/)) {
        const line = raw.trim();
        const m = /^\[(.*)\]$/.exec(line);
        if (m) {
            inSection = m[1].trim().toLowerCase() === section.toLowerCase();
            continue;
        }
        const eq = line.indexOf('=');
        if (inSection && eq > 0 && line.slice(0, eq).trim().toLowerCase() === key.toLowerCase())
            return line.slice(eq + 1).trim();
    }
    return null;
}

export function iniSet(text, section, key, value) {
    const nl = /\r\n/.test(text) || !text ? '\r\n' : '\n';
    const lines = text ? text.split(/\r?\n/) : [];
    if (lines.length && lines[lines.length - 1] === '')
        lines.pop();
    let start = -1, end = lines.length;
    for (let i = 0; i < lines.length; i++) {
        const m = /^\s*\[(.*)\]\s*$/.exec(lines[i]);
        if (!m)
            continue;
        if (start >= 0) {
            end = i;
            break;
        }
        if (m[1].trim().toLowerCase() === section.toLowerCase())
            start = i;
    }
    const entry = key + '=' + value;
    if (start < 0) {
        if (lines.length && lines[lines.length - 1].trim() !== '')
            lines.push('');
        lines.push('[' + section + ']', entry);
    } else {
        let at = -1, last = start;
        for (let i = start + 1; i < end; i++) {
            const eq = lines[i].indexOf('=');
            if (eq > 0 && lines[i].slice(0, eq).trim().toLowerCase() === key.toLowerCase())
                at = i;
            if (lines[i].trim() !== '')
                last = i;
        }
        if (at >= 0)
            lines[at] = entry;
        else
            lines.splice(last + 1, 0, entry);
    }
    return lines.join(nl) + nl;
}

// Whether the game will want HD graphics: ?hd=N (as --hd=N) wins, else
// CIV.INI's choice; never in Classic mode, as natively.  -> the density
// wanted (0: auto), or -1 off
export function hdWanted(ini, query) {
    if ((iniGet(ini, SECTION, 'Mode') || '').toLowerCase() === 'classic')
        return -1;
    if (query !== null && query !== undefined) {
        const n = query === '' ? 1 : parseInt(query, 10) || (/^on$/i.test(query) ? 1 : 0);
        return n >= 1 ? (n > 1 ? Math.min(n, 4) : 0) : -1;
    }
    if ((iniGet(ini, SECTION, 'Graphics') || '').toLowerCase() !== 'hd')
        return -1;
    return parseInt(iniGet(ini, SECTION, 'GraphicsDensity') || '0', 10) || 0;
}
