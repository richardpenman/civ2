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

// HD graphics where CIV.INI's Graphics= says nothing: the mirror of
// src/platform/options.c's HD_BY_DEFAULT (the one place natively)
export const HD_BY_DEFAULT = false;

// The original's pictures the game folder must have for their look
// (src/platform/synth.c: synth_missing); without any of them the game makes
// them from the HD art and runs with HD graphics, whatever is chosen
// (docs/NO-INSTALL.md)
export const ORIGINAL_PICTURES = ['TERRAIN1.GIF', 'TERRAIN2.GIF', 'UNITS.GIF', 'CITIES.GIF', 'ICONS.GIF',
    'PEOPLE.GIF', 'EDITORPT.GIF', 'CITY.GIF', 'SCREDITS.GIF', 'EDITORAS.GIF', 'EDITORSA.GIF', 'EDITORSQ.GIF',
    'CIV2.EXE', 'CIV2ART.DLL', 'INTRO.DLL', 'CV.DLL', 'MK.DLL', 'PV.DLL', 'SS.DLL', 'TILES.DLL', 'WONDER.DLL'];
export function hasOriginalPictures(names) {
    const have = new Set(names.map((n) => n.toUpperCase()));
    return ORIGINAL_PICTURES.every((n) => have.has(n));
}

// Whether the game will want HD graphics: ?hd=N (as --hd=N) wins, else
// CIV.INI's choice (Graphics=HD; Classic graphics: Graphics=Classic, or an
// earlier version's Mode=Classic, which the game turns into that at its
// start); always where the
// game folder lacks the original's pictures (pictures false), as natively
// (the density as asked, else auto).  -> the density wanted (0: auto), or
// -1 off
export function hdWanted(ini, query, pictures = true) {
    if (!pictures) {
        const n = hdWanted(iniSet(ini, SECTION, 'Mode', 'Standard'), query);
        return n >= 0 ? n : parseInt(iniGet(ini, SECTION, 'GraphicsDensity') || '0', 10) || 0;
    }
    if ((iniGet(ini, SECTION, 'Mode') || '').toLowerCase() === 'classic')
        return -1;
    if (query !== null && query !== undefined) {
        const n = query === '' ? 1 : parseInt(query, 10) || (/^on$/i.test(query) ? 1 : 0);
        return n >= 1 ? (n > 1 ? Math.min(n, 4) : 0) : -1;
    }
    const graphics = (iniGet(ini, SECTION, 'Graphics') || '').toLowerCase();
    if (graphics ? graphics !== 'hd' : !HD_BY_DEFAULT)
        return -1;
    return parseInt(iniGet(ini, SECTION, 'GraphicsDensity') || '0', 10) || 0;
}
