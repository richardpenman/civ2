/* The text of a message box: SDL's on X11 breaks lines only at '\n', so a
 * long message was one line wider than the screen; there (Linux) it is
 * broken at spaces into lines of at most 72 characters.  Windows' and
 * macOS' boxes wrap their own.  Returns out (or text, unchanged). */
#ifndef BOX_TEXT_H
#define BOX_TEXT_H
#include <string.h>

static const char *box_text(const char *text, char *out, size_t size)
{
#if defined(_WIN32) || defined(__APPLE__)
    (void)out;
    (void)size;
    return text;
#else
    size_t i, n = strlen(text), line = 0, space = 0;
    if (!size)
        return text;
    if (n >= size)
        n = size - 1;
    memcpy(out, text, n);
    out[n] = 0;
    for (i = 0; i < n; i++) {
        if (out[i] == '\n') {
            line = i + 1;
            space = 0;
        } else if (out[i] == ' ') {
            space = i;
        }
        if (i - line >= 72 && space > line) {
            out[space] = '\n';
            line = space + 1;
            space = 0;
        }
    }
    return out;
#endif
}

#endif
