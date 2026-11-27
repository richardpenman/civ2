/*
 * (port) Text in UTF-8: a translation whose file says "@@codepage utf-8"
 * (Chinese: data/native/lang/zh-Hant.txt, docs/TRANSLATING.md) has the
 * game's byte strings in UTF-8.  Then a character is one to four bytes,
 * and what handles text by byte -- drawing and measuring (src/platform/
 * text.c), word wrapping (DrawText, the edit control, the game's dialogs),
 * the caret, cutting a text to fit -- goes by whole characters, and lines
 * may break next to Chinese characters, which have no spaces between them.
 * In any other code page (plat_text_utf8 0) every byte is a character and
 * these helpers change nothing: the text is handled exactly as before.
 *
 * Line breaking, kept simple: a line may break at a space, and before or
 * after a wide character (CJK ideographs, kana, hangul, full-width forms
 * and punctuation), but not before a closing mark (，。、；：？！」』）...
 * and ASCII ,.;:!?)]}%) nor after an opening one (「『（《 ... ([{), nor
 * next to '_' (the game's texts' space that does not break: "_._._.").
 * UTF-8's continuation bytes are 0x80-0xbf, so the game's own codes
 * (ASCII: '%', '@', '^', '|', '\\' ...) are never inside a character. *
 * One escape, in any code page: U8_ESC (0x1a, the byte that ended a DOS
 * text file, so no text of the game has it) followed by one character in
 * UTF-8 is that character.  Only the Language list of the Settings
 * uses it (src/game/lang.cpp), so that each language's own name (Ελληνικά,
 * 繁體中文) shows in a Windows-1252 or Latin-3 interface; src/platform/
 * text.c draws and measures it, these helpers count it as one character.
 */
#ifndef CIV2_UTF8_H
#define CIV2_UTF8_H
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/* 1 when the game's text is UTF-8 (src/platform/text.c sets it from the
   translation's code page) */
extern int plat_text_utf8;
#ifdef __cplusplus
}
#endif

#define U8_BAD 0xffffffffu
#define U8_ESC 0x1a

/* The character at s[0..n) (n < 0: up to the NUL) and its length in *len;
   U8_BAD (with *len 1) for a byte that starts no whole character: a
   continuation byte, an overlong or broken sequence, one cut short. */
static inline unsigned u8_decode(const char *str, int n, int *len)
{
    const unsigned char *s = (const unsigned char *)str;
    unsigned c = s[0], cp, k, need;
    *len = 1;
    if (c < 0x80)
        return c;
    if (c >= 0xc2 && c <= 0xdf) { need = 1; cp = c & 0x1f; }
    else if (c >= 0xe0 && c <= 0xef) { need = 2; cp = c & 0x0f; }
    else if (c >= 0xf0 && c <= 0xf4) { need = 3; cp = c & 0x07; }
    else return U8_BAD;
    for (k = 1; k <= need; k++) {
        if ((n >= 0 && (int)k >= n) || (s[k] & 0xc0) != 0x80)
            return U8_BAD;
        cp = cp << 6 | (s[k] & 0x3f);
    }
    if ((need == 2 && cp < 0x800) || (need == 3 && (cp < 0x10000 || cp > 0x10ffff)) ||
        (cp >= 0xd800 && cp <= 0xdfff))
        return U8_BAD;
    *len = (int)need + 1;
    return cp;
}

/* The length of an escaped character (U8_ESC and a UTF-8 character) at s,
   of s[0..n) (n < 0: up to the NUL); 0 if s has none. */
static inline int u8_escaped_len(const char *s, int n)
{
    int len;
    if (s[0] != U8_ESC || n == 1 || n == 0 || !s[1] || u8_decode(s + 1, n < 0 ? -1 : n - 1, &len) == U8_BAD)
        return 0;
    return len + 1;
}

/* The length of the character at s[i] (1 if not UTF-8 text). */
static inline int u8_len_at(const char *s, int n, int i)
{
    int len = 1;
    if (s[i] == U8_ESC && (len = u8_escaped_len(s + i, n < 0 ? -1 : n - i)) != 0)
        return len;
    len = 1;
    if (plat_text_utf8 && (unsigned char)s[i] >= 0x80)
        u8_decode(s + i, n < 0 ? -1 : n - i, &len);
    return len;
}

/* The start of an escaped character that ends at s[i], or -1. */
static inline int u8_escaped_before(const char *s, int i)
{
    int k;
    for (k = i - 2; k >= 0 && k >= i - 5; k--)
        if (s[k] == U8_ESC && u8_escaped_len(s + k, i - k) == i - k)
            return k;
    return -1;
}

/* The start of the character before s[i] (i - 1 if not UTF-8 text). */
static inline int u8_prev(const char *s, int i)
{
    int k = i - 1, len;
    if (i >= 2 && (unsigned char)s[i - 1] >= 0x80 && (len = u8_escaped_before(s, i)) >= 0)
        return len;
    if (!plat_text_utf8 || i <= 0)
        return i > 0 ? i - 1 : 0;
    while (k > 0 && k > i - 4 && ((unsigned char)s[k] & 0xc0) == 0x80)
        k--;
    u8_decode(s + k, i - k, &len);
    return k + len == i ? k : i - 1;
}

/* The longest start of s[0..n) that ends on a whole character (n if not
   UTF-8 text): where a text is cut to fit, no character is cut in two. */
static inline int u8_clip(const char *s, int n)
{
    int k, len;
    for (k = n - 1; k >= 0 && k >= n - 4; k--)
        if (s[k] == U8_ESC) {
            /* (an escaped character cut short: before it) */
            if (u8_escaped_len(s + k, -1) > n - k)
                return k;
            break;
        }
    if (!plat_text_utf8 || n <= 0)
        return n;
    k = n - 1;
    while (k > 0 && k > n - 4 && ((unsigned char)s[k] & 0xc0) == 0x80)
        k--;
    if ((unsigned char)s[k] < 0xc0)
        return n;               /* ASCII, or a stray continuation byte: as it is */
    u8_decode(s + k, n - k, &len);
    return k + len <= n && len > 1 ? n : k;
}

/* A wide character: one a line may break before or after. */
static inline int u8_wide(unsigned cp)
{
    return (cp >= 0x1100 && cp <= 0x115f) || (cp >= 0x2e80 && cp <= 0xa4cf) ||
           (cp >= 0xac00 && cp <= 0xd7a3) || (cp >= 0xf900 && cp <= 0xfaff) ||
           (cp >= 0xfe30 && cp <= 0xfe4f) || (cp >= 0xff00 && cp <= 0xff60) ||
           (cp >= 0xffe0 && cp <= 0xffe6) || (cp >= 0x20000 && cp <= 0x3fffd);
}

/* A character a line does not start with (closing marks). */
static inline int u8_no_start(unsigned cp)
{
    static const unsigned marks[] = {0x3001, 0x3002, 0xff0c, 0xff0e, 0xff1b, 0xff1a, 0xff1f, 0xff01, 0x300d,
                                     0x300f, 0xff09, 0x3009, 0x300b, 0x3011, 0x3015, 0x30fc, 0x2019, 0x201d,
                                     0x2026, 0x30fb, 0xff5e, 0x00bb};
    unsigned i;
    if (cp < 0x80)
        return cp && strchr(",.;:!?)]}%_", (int)cp) != NULL;
    for (i = 0; i < sizeof marks / sizeof marks[0]; i++)
        if (marks[i] == cp)
            return 1;
    return 0;
}

/* A character a line does not end with (opening marks). */
static inline int u8_no_end(unsigned cp)
{
    static const unsigned marks[] = {0x300c, 0x300e, 0xff08, 0x3008, 0x300a, 0x3010, 0x3014, 0x2018, 0x201c,
                                     0x00ab};
    unsigned i;
    if (cp < 0x80)
        return cp && strchr("([{_", (int)cp) != NULL;
    for (i = 0; i < sizeof marks / sizeof marks[0]; i++)
        if (marks[i] == cp)
            return 1;
    return 0;
}

/* Whether a line may break between the characters a and b (not counting
   spaces, which the callers break at themselves). */
static inline int u8_break_between(unsigned a, unsigned b)
{
    if (a == U8_BAD || b == U8_BAD)
        return 0;
    return (u8_wide(a) || u8_wide(b)) && !u8_no_start(b) && !u8_no_end(a);
}

/* The character before s + i (U8_BAD if none). */
static inline unsigned u8_before(const char *s, int i)
{
    int k = u8_prev(s, i), len;
    return i > 0 ? u8_decode(s + k, i - k, &len) : U8_BAD;
}

/* The end of the word that starts at s (a NUL-terminated text): the next
   space or NUL, or (UTF-8 text) the first place before it where a line may
   break (next to a wide character). */
static inline const char *u8_word_end(const char *s)
{
    const char *p = s;
    unsigned prev = U8_BAD;
    if (!plat_text_utf8) {
        p = strchr(s, ' ');
        return p ? p : s + strlen(s);
    }
    while (*p && *p != ' ') {
        int len;
        unsigned c = u8_decode(p, -1, &len);
        if (p > s && u8_break_between(prev, c))
            break;
        prev = c;
        p += len;
    }
    return p;
}

#endif
