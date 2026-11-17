/*
 * The host's texts, translated (docs/TRANSLATING.md): OpenCiv2's own texts
 * the host shows itself (the set-up's messages and progress, the macOS menu
 * bar's Full Screen), from the same table as the game's (texts.h) and the
 * same translation files (lang/<code>.txt), as UTF-8 for the system's
 * dialogs; and the game's text (bytes in the translation's code page,
 * codepages.h) as UTF-8 for the host's message boxes and menu bar.
 */
#ifndef CIV2_HOST_LANG_H
#define CIV2_HOST_LANG_H
#include <stddef.h>

/* The text of key (texts.h's, without "openciv2."), as UTF-8: the
   translation's, else the English; key itself if there is none. */
const char *host_text(const char *key);

/* The translation: the language chosen as the game chooses it (CIV2_LANG,
   else game_dir's CIV.INI [OpenCiv2] Language=, else, Auto, the system's:
   CIV2_SYSTEM_LANG), its file in game_dir's lang/ or the data directory's
   (game_dir may be NULL: not known yet).  Again with another game_dir:
   read again. */
void host_lang_init(const char *game_dir);

/* A translation file read (path), in place of the one chosen. */
void host_lang_load(const char *path);

/* The game's text (in the translation's code page) as UTF-8, into out. */
const char *host_lang_utf8(const char *text, char *out, size_t size);

/* The user's preferred language ("de-DE"), as the system says it
   (host_sdl.c); 0 if it does not. */
int host_system_language(char *out, int size);

#endif
