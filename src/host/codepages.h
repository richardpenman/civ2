/*
 * The code pages a translation can be written in (docs/TRANSLATING.md): the
 * game's text is single bytes, drawn with the glyphs of Windows-1252
 * (data/native/fonts_win*.bin); a translation whose file says
 * "@@codepage" iso-8859-3 (Latin-3: Esperanto, Maltese) has the bytes that
 * differ drawn as their Unicode characters (src/platform/text.c: letters
 * with marks put together from the captured glyphs, its table marks[]),
 * typed and shown by the host as those characters.  Per code page the Unicode character of bytes 0x80 to
 * 0xff (0: none, as Windows-1252's holes).  The game's Latin-3 keeps
 * Windows-1252's 0x80-0x9f (Latin-3 has control codes there) and its
 * characters where Latin-3 has none.
 *
 * Adding one (cp1250 for Polish or Czech): a table here, and its letters
 * in src/platform/text.c's marks[] (tests/lang_test.py checks that every
 * one is there; docs/TRANSLATING.md).  (Letters no Latin letter makes, as
 * Cyrillic, need a font file: UTF-8, as Chinese.)
 */
#ifndef CIV2_CODEPAGES_H
#define CIV2_CODEPAGES_H

static const unsigned short codepage_1252[128] = {
    0x20ac, 0x0000, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
    0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017d, 0x0000,
    0x0000, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
    0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x0000, 0x017e, 0x0178,
    0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
    0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
    0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
    0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
    0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
    0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
    0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
    0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
    0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
    0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
    0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

static const unsigned short codepage_latin3[128] = {
    0x20ac, 0x0000, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
    0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017d, 0x0000,
    0x0000, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
    0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x0000, 0x017e, 0x0178,
    0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4, 0x00a5, 0x0124, 0x00a7,
    0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad, 0x00ae, 0x017b,
    0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7,
    0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd, 0x00be, 0x017c,
    0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x010a, 0x0108, 0x00c7,
    0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
    0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7,
    0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df,
    0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x010b, 0x0109, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
    0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7,
    0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9,
};

/* A code page's table by its name ("windows-1252", "iso-8859-3", any
   case), 0 if none such. */
static inline const unsigned short *codepage_table(const char *name)
{
    static const struct { const char *name; const unsigned short *table; } pages[] = {
        {"windows-1252", codepage_1252}, {"cp1252", codepage_1252},
        {"iso-8859-3", codepage_latin3}, {"latin-3", codepage_latin3},
    };
    unsigned i, k;
    for (i = 0; i < sizeof pages / sizeof pages[0]; i++) {
        for (k = 0; name[k] && pages[i].name[k] &&
                    (name[k] | (name[k] >= 'A' && name[k] <= 'Z' ? 0x20 : 0)) == pages[i].name[k]; k++)
            ;
        if (!name[k] && !pages[i].name[k])
            return pages[i].table;
    }
    return 0;
}

/* "utf-8" (any case, or "utf8"): the game's text in UTF-8 (a translation
   in Chinese ...: src/include/utf8.h), no table. */
static inline int codepage_is_utf8(const char *name)
{
    const char *u = "utf-8";
    unsigned k = 0, i = 0;
    for (; name[k]; k++) {
        char c = (char)(name[k] | (name[k] >= 'A' && name[k] <= 'Z' ? 0x20 : 0));
        if (c == '-' && u[i] != '-')
            return 0;
        if (u[i] == '-' && c != '-')
            i++;
        if (c != u[i++])
            return 0;
    }
    return !u[i];
}

/* A language code as the system or the player gives it, as the name of a
   translation: Chinese in traditional characters (zh_TW, zh-HK, zh_MO,
   zh-Hant ...) is zh-Hant; others as they are (code: "pt-BR", '_' as '-',
   without ".UTF-8"; into out). */
static inline void language_alias(const char *code, char *out, unsigned size)
{
    unsigned i;
    char low[32];
    if (!size)
        return;
    for (i = 0; code[i] && code[i] != '.' && code[i] != '@' && i + 1 < sizeof low; i++)
        low[i] = (char)(code[i] == '_' ? '-' : code[i] | (code[i] >= 'A' && code[i] <= 'Z' ? 0x20 : 0));
    low[i] = 0;
    if (low[0] == 'z' && low[1] == 'h' && low[2] == '-' &&
        ((low[3] == 't' && low[4] == 'w') || (low[3] == 'h' && low[4] == 'k') ||
         (low[3] == 'm' && low[4] == 'o') || (low[3] == 'h' && low[4] == 'a' && low[5] == 'n' && low[6] == 't')) &&
        (!low[5] || low[5] == '-' || (low[4] == 'a' && (!low[7] || low[7] == '-')))) {
        const char *z = "zh-Hant";
        for (i = 0; z[i] && i + 1 < size; i++)
            out[i] = z[i];
        out[i] = 0;
        return;
    }
    for (i = 0; code[i] && code[i] != '.' && code[i] != '@' && i + 1 < size; i++)
        out[i] = code[i] == '_' ? '-' : code[i];
    out[i] = 0;
}

/* A byte of a code page as Unicode (the bytes below 0x80: ASCII). */
static inline unsigned codepage_to_unicode(const unsigned short *table, unsigned char b)
{
    return b < 0x80 ? b : table[b - 0x80];
}

/* A Unicode character as a byte of a code page; 0 if it has none. */
static inline unsigned char codepage_from_unicode(const unsigned short *table, unsigned u)
{
    unsigned i;
    if (u < 0x80)
        return (unsigned char)u;
    for (i = 0; i < 128; i++)
        if (table[i] == u)
            return (unsigned char)(0x80 + i);
    return 0;
}

#endif
