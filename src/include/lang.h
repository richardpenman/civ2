/*
 * (port) The game's text by key, not by its place in the game's text files.
 *
 * The program's structure (which menu item runs which command, ...) is in
 * the code; its text is looked up here by key ("menu.title.1",
 * "menu.1f1", ...).  The English text is built from the game's own files
 * (src/game/lang.cpp knows their layouts and versions); a translation is a
 * file of the same keys (lang/<code>.txt in the data directory, chosen with
 * CIV2_LANG=<code>) whose texts replace the English ones.  CIV2_EXPORT_TEXT=
 * <path> writes the English text as such a file, the start of a translation.
 *
 * Text file format: a line "@@key" starts an entry, the following lines up
 * to the next "@@" line are its text (one line for most keys); lines before
 * the first key are comments.  A text line starting with "@@" or "\" is
 * written with a "\" before it.
 *
 * The popups and other texts of the game's text files are keyed by file and
 * section ("game.MAINMENU": GAME.TXT's @MAINMENU): the section's lines, as
 * they are in the file (its @title=, @width=, @options lines too: a
 * translation keeps the lines that are not text, and the order of option
 * lines, which decides what each does).  A translated section is read in
 * place of the file's lines by text_read_line, whatever reads it.
 *
 * OpenCiv2's own texts (its interface additions, docs/EXTRAS.md) are keyed
 * "openciv2.<key>": their English is in the table of src/game/extras.cpp
 * (texts[]), which the export writes with the rest.  "openciv2.menu.arrange_s"
 * and "_l", when not translated, are the translated Arrange Windows with " S"
 * and " L".  (The opening window, src/platform/launcher.c, shown before the
 * game's texts are read, is in English.)
 */
#ifndef CIV2_LANG_H
#define CIV2_LANG_H

/* The text of key: its translation if there is one, else the English text;
   "" if neither has the key. */
const char *lang_text(const char *key);

/* Whether key has a text (a translation's or the English). */
int lang_has(const char *key);

/* Whether the translation has key. */
int lang_translated(const char *key);

/* The English menu texts, from MENU.TXT (the scenario's first, as the game
   reads its text files); called before the menus are built. */
void lang_load_menus(void);

/* The labels (LABELS.TXT, label_table.string_ids[n]: key "label.<n>"), once
   labels_load has read them: their English texts recorded, a translation's
   put in their place. */
void lang_add_labels(int count);

/* The translation of the string pool's string_id (a label), or 0. */
char *lang_pool_text(int string_id);

/* The English names of the rules (RULES.TXT; their keys: rules.h), once. */
void lang_load_rules(void);

/* The Civilopedia's description (DESCRIBE.TXT) of entry (its @@ line's
   name) in the group whose index pedia_load_description looked for
   ("@ADVANCE_INDEX", "CONCEPT_DESCRIPTIONS", ...): its translation (key
   "describe.ADVANCE.Advanced Flight": the paragraphs' lines), or 0. */
const char *lang_description(const char *group, const char *entry);

/* text_open_section found filename's section (filename 0: in the file
   open), in_scenario if in the scenario's copy of the file (which keeps its
   own text): its translation, if there is one, is what text_read_line reads
   next, up to the file's next section. */
void lang_section_opened(const char *filename, const char *section, int in_scenario);

/* The next line of the translated section text_read_line reads (into buf, as
   fgets would, without the newline): 1, or 0 when there is none (no
   translated section open, or its lines all read). */
int lang_section_line(char *buf);

/* text_close_file: no section open. */
void lang_section_closed(void);

/* (port) The main menu bar: one row per menu title, item or separator, in
   order.  cmd: the command id (0 a separator, MENU_TITLE the menu's title);
   key: its text's key; room: the bytes its text has room for (the Orders
   items whose label the game changes, as for Build Road / Railroad). */
#define MENU_TITLE (-1)
typedef struct MenuRow {
    int menu;
    int cmd;
    const char *key;
    int room;
} MenuRow;
extern const MenuRow menu_rows[];
extern const int menu_row_count;

#endif
