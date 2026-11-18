// The page's texts in the player's language (docs/TRANSLATING.md): the same
// translation files as the game's (lang/<code>.txt, "@@openciv2.<key>"
// lines, in their code page: "@@codepage", else Windows-1252), the page's
// keys "web.*"; English (texts.js, built by tools/build_web.py from
// src/host/texts.h) where a translation has none.  The language: the one
// chosen on the page (kept in this browser), else the browser's.

import {EN} from './texts.js';

const STORE = 'civ2.lang';
let texts = {};             // the translation's, by key (without "openciv2.")
let current = 'en';

// a translation file's bytes -> {codepage, texts: {key: text}}
export function parseLang(bytes) {
    const ascii = new TextDecoder('windows-1252').decode(bytes);
    const cp = /^@@codepage\r?\n([^\r\n]*)/m.exec(ascii);
    const codepage = cp ? cp[1].trim().toLowerCase() : 'windows-1252';
    let text;
    try {
        text = new TextDecoder(codepage).decode(bytes);
    } catch (e) {
        text = ascii;
    }
    const out = {};
    let key = null, lines = [];
    const end = () => {
        if (key !== null && key.startsWith('openciv2.'))
            out[key.slice(9)] = lines.join('\n');
    };
    for (const line of text.split(/\r?\n/)) {
        if (line.startsWith('@@')) {
            end();
            key = line.slice(2);
            lines = [];
        } else if (key !== null) {
            lines.push(line.startsWith('\\') ? line.slice(1) : line);
        }
    }
    end();
    // (a file's last line ends with a newline: no empty line more)
    for (const k of Object.keys(out))
        out[k] = out[k].replace(/\n$/, '');
    return {codepage, texts: out};
}

// The text of key, its %s and %d filled with args in order.
export function t(key, ...args) {
    let s = key in texts ? texts[key] : key in EN ? EN[key] : key;
    let i = 0;
    return s.replace(/%[sd]/g, () => String(i < args.length ? args[i++] : ''));
}

export const language = () => current;

// The languages the site has (lang/index.json: [{code, name}], English not
// among them); [] if none.
let listP = null;
export function languages() {
    return listP || (listP = fetch('lang/index.json').then((r) => (r.ok ? r.json() : []), () => []).catch(() => []));
}

// The code chosen on the page ('' : the browser's)
export function chosen() {
    try {
        return localStorage.getItem(STORE) || '';
    } catch (e) {
        return '';
    }
}

export function choose(code) {
    try {
        if (code)
            localStorage.setItem(STORE, code);
        else
            localStorage.removeItem(STORE);
    } catch (e) { /* not kept */ }
}

// Among the site's languages, the one a code asks for ("pt-BR": pt-BR,
// else pt); 'en' if none.
export function match(code, list) {
    let c = (code || '').replace('_', '-').toLowerCase();
    // Chinese in traditional characters (zh-TW, zh-HK, zh-MO, zh-Hant...): zh-Hant
    // (as src/host/codepages.h's language_alias)
    if (/^zh-(tw|hk|mo|hant)(-|$)/.test(c))
        c = 'zh-hant';
    const codes = list.map((l) => l.code);
    return codes.find((k) => k.toLowerCase() === c) || codes.find((k) => k.toLowerCase() === c.split('-')[0]) || 'en';
}

// The page's language: chosen, else (Auto) the first of the browser's the
// site has (?lang=CODE wins: the tests)
export async function pageLanguage() {
    const list = await languages();
    const q = new URLSearchParams(location.search).get('lang');
    if (q)
        return match(q, list);
    const c = chosen();
    if (c)
        return c === 'en' ? 'en' : match(c, list);
    for (const l of navigator.languages || [navigator.language]) {
        const m = match(l, list);
        if (m !== 'en' || /^en\b/i.test(l))
            return m;
    }
    return 'en';
}

// A translation file's bytes (null: none, or English)
const fileCache = new Map();
export function langFile(code) {
    if (!code || code === 'en')
        return Promise.resolve(null);
    if (!fileCache.has(code))
        fileCache.set(code, fetch('lang/' + code + '.txt').then((r) => (r.ok ? r.arrayBuffer() : null))
            .then((b) => (b ? new Uint8Array(b) : null), () => null));
    return fileCache.get(code);
}

// Only <em> and <code> of a translation's markup kept (the rest as text)
function safeHtml(s) {
    const esc = s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    return esc.replace(/&lt;(\/?)(em|code)&gt;/g, '<$1$2>');
}

// The page's language in force: its texts read, the page's own texts
// (data-t: the element's text, data-t-node: its first text, data-t-title:
// its title) put in their place
export async function useLanguage(code) {
    current = code || 'en';
    texts = {};
    const bytes = await langFile(current);
    if (bytes)
        texts = parseLang(bytes).texts;
    if (current === 'en')
        return;             // (the page as written)
    document.documentElement.lang = current;
    for (const el of document.querySelectorAll('[data-t]')) {
        const k = el.dataset.t;
        if (!(k in texts))
            continue;
        if (/[<]/.test(EN[k] || ''))
            el.innerHTML = safeHtml(texts[k]);
        else
            el.textContent = texts[k];
    }
    // (data-t-node: the element's first text, before its other contents)
    for (const el of document.querySelectorAll('[data-t-node]')) {
        const node = [...el.childNodes].find((n) => n.nodeType === 3 && n.textContent.trim());
        if (node && el.dataset.tNode in texts)
            node.textContent = texts[el.dataset.tNode] + (/\s$/.test(node.textContent) ? ' ' : '');
    }
    for (const el of document.querySelectorAll('[data-t-title]'))
        if (el.dataset.tTitle in texts)
            el.title = texts[el.dataset.tTitle];
}
