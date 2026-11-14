// The opening window's choices (src/platform/launcher.c; docs/EXTRAS.md "The
// opening window") on the set-up page: read from and written to CIV.INI's
// [OpenCiv2] section, which the browser keeps with the game's files, as the
// opening window reads and writes them, so that the game reads them as
// natively.  The lists and their texts are the opening window's own
// (tools/build_web.py takes them from launcher.c into choices.json).

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

// ---- the choices (launcher.c: Choices, read_choices, write_choices) ----

export function readChoices(spec, ini) {
    const get = (k) => {
        const v = iniGet(ini, SECTION, k);
        return v === null ? '' : v;
    };
    const getInt = (k, def) => {
        const v = get(k);
        return v ? parseInt(v, 10) || 0 : def;
    };
    const c = {};
    c.classic = get('Mode').toLowerCase() === 'classic';
    c.hd = get('Graphics').toLowerCase() === 'hd';
    c.density = getInt('GraphicsDensity', 0);
    if (c.density < 0 || c.density > 4)
        c.density = 0;
    c.rule = spec.rules.map((r) => getInt(r.key, 0) !== 0);
    c.extra = spec.extras.map((x) => getInt(x.key, 1) !== 0);
    let year = getInt('RetirementYear', 0);
    if (year <= 0)
        year = spec.limits[0].original;
    c.limit = spec.limits.map((l, i) => {
        if (i === 0)
            return String(year);
        if (i === 1)
            return String(getInt('RetirementYear', 0) > 0 ? getInt('RetirementWarningYear', year - 20) : l.original);
        const n = getInt(l.key, 0);
        return String(n > 0 ? n : l.original);
    });
    c.colour = spec.colours.map((x, i) => (i < spec.colours.length - 1 ? String(getInt(x.key, x.def)) : get(x.key)));
    return c;
}

// What changed, as [key, value] to write
export function changedChoices(spec, c, old) {
    const out = [];
    const put = (k, v) => out.push([k, String(v)]);
    const atoi = (s) => parseInt(s, 10) || 0;
    if (c.classic !== old.classic)
        put('Mode', c.classic ? 'Classic' : 'Standard');
    if (c.hd !== old.hd)
        put('Graphics', c.hd ? 'HD' : 'Standard');
    if (c.density !== old.density)
        put('GraphicsDensity', c.density);
    spec.rules.forEach((r, i) => c.rule[i] !== old.rule[i] && put(r.key, c.rule[i] ? 1 : 0));
    spec.extras.forEach((x, i) => c.extra[i] !== old.extra[i] && put(x.key, c.extra[i] ? 1 : 0));
    const [retire, warnL] = spec.limits;
    if (c.limit[0] !== old.limit[0] || c.limit[1] !== old.limit[1]) {
        const year = atoi(c.limit[0]);
        let warn = atoi(c.limit[1]);
        if (c.limit[1] === old.limit[1] && atoi(old.limit[1]) === atoi(old.limit[0]) - 20)
            warn = year - 20;       // a warning 20 years before moves with the year
        if (year === retire.original && warn === warnL.original)
            put('RetirementYear', 0);
        else {
            put('RetirementYear', year < 1 ? 1 : year > 9999 ? 9999 : year);
            put('RetirementWarningYear', warn < 0 ? 0 : warn);
        }
    }
    spec.limits.forEach((l, i) => {
        if (i >= 2 && c.limit[i] !== old.limit[i]) {
            const v = atoi(c.limit[i]);
            put(l.key, v <= l.original ? 0 : v > l.hi ? l.hi : v);
        }
    });
    spec.colours.forEach((x, i) => {
        if (c.colour[i] !== old.colour[i])
            put(x.key, i === spec.colours.length - 1 ? c.colour[i] : atoi(c.colour[i]));
    });
    return out;
}

export function writeChoices(spec, ini, c, old) {
    for (const [k, v] of changedChoices(spec, c, old))
        ini = iniSet(ini, SECTION, k, v);
    return ini;
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
