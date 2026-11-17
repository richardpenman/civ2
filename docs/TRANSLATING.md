# Translating OpenCiv2

OpenCiv2 has two kinds of text:

- **The game's own texts** (`GAME.TXT`, `RULES.TXT`, `LABELS.TXT`, `DESCRIBE.TXT`, `CITY.TXT`,
  `MENU.TXT`, the advisors', the scenarios' ...) come from your copy of Civilization II, as the
  original reads them. A localized Civilization II (German, French, Spanish ...) gives them in
  its language as it is. A translation may carry them too (keyed: below): the Esperanto one
  that comes with OpenCiv2 translates the whole game. The English of the game's texts is not
  in this repository (it is in your copy of the game): only translations of them.
- **OpenCiv2's own texts**: what it adds — OpenCiv2 Options, the interface additions' few words,
  the file dialog, its message boxes, the set-up's messages and progress, the HD graphics'
  download, the browser version's page. These are translated here. English is built in;
  `data/native/lang/` has the translations that come with OpenCiv2: Esperanto (`eo.txt`, a first
  draft: machine-drafted, needs review by a native speaker), which translates the game's texts
  too.

## Choosing a language

- In the game: *OpenCiv2 Options > Language* (main menu): *Automatic* (the system's language),
  *English*, or one of the translations in `lang/`. It is kept in `CIV.INI` (`[OpenCiv2]
  Language=eo`; `Auto` or none: automatic) and takes effect when the game starts again.
- `CIV2_LANG=<code>` in the environment wins over that choice (and is the only one test runs
  read).
- In the browser version: the set-up page's *Language* choice (automatic: the browser's
  language), which is also the game's *Automatic* ([BROWSER.md](BROWSER.md#language)).

A code is a file's name: `eo` reads `lang/eo.txt`; a system's `pt_BR` reads `lang/pt-BR.txt`,
else `lang/pt.txt`. The file is looked for in the game's folder (`lang/` beside its files: a
player's own) first, then in OpenCiv2's data directory (`data/lang/` beside the program;
`data/native/lang/` in the source tree; in the macOS app `Contents/Resources/data/lang`; on
Windows the program writes its own into `data\lang` beside it). The Language choice lists the
files in the data directory.

## The file format

A translation is a text file of keyed texts: a line `@@key` starts an entry, the lines after it
up to the next `@@` line are its text (one line for most keys; a text that runs over several
lines keeps them). Lines before the first key are comments. A text line that starts with `@@`
or `\` is written with a `\` before it.

```
; OpenCiv2: Esperanto ... (machine-drafted, needs review by a native speaker)
@@codepage
iso-8859-3
@@openciv2.language.name
Esperanto
@@openciv2.options.title
Agordoj de OpenCiv2
@@openciv2.file.not_found
%s
Dosiero ne trovita.
```

- `openciv2.<key>`: OpenCiv2's own texts. `openciv2.language.name` is the language's name in
  itself, as the Language choice shows it.
- Placeholders (`%s`, `%d`) stay, in the same order as in English (they are filled in that
  order).
- Keys the file lacks stay English; a key that is not OpenCiv2's (below) is one of the game's.
- `web.*` keys are the browser page's; they may hold `<em>` and `<code>`, which the page keeps.
- Keep a text about as long as the English: dialogs are as wide as their English (the file
  dialog's labels have little room).

### The template: exporting the texts

`data/native/lang/en.txt` is the template: every one of OpenCiv2's own keys with its English
(made from `src/host/texts.h`, where the English lives). Copy it to `lang/<code>.txt` and
translate the texts. The game writes it too:

```sh
CIV2_EXPORT_TEXT=en.txt CIV2_EXPORT_OWN=1 ./OpenCiv2
```

Without `CIV2_EXPORT_OWN=1`, `CIV2_EXPORT_TEXT` writes every text, the game's too
(`game.<SECTION>`, `menu.*`, `label.<n>`, `rules.*`, `describe.*`, `src/include/lang.h`):
the start of a translation of the game itself. That export is the English of your copy of the
game: keep it to yourself (it is not added here); a translation made from it may be.

### The game's texts

Their keys say which file and which part of it they replace; a key the translation lacks keeps
the game's English. What to keep as it is:

- Every text has the same lines as the English: a popup's blank line ends its text and starts
  its options; `@width=`, `@options`, `@listbox`, `@columns=` ... and `;` comments stay as they
  are, but the text of `@title=` and `@button=`. Plain lines are joined and wrapped; a line
  starting `^` (`^^` centred) is drawn as it is, so it must fit the dialog's width.
- `%STRING0`, `%NUMBER0` ... (the game fills them in by number) and `%s`, `%d` (in order);
  `{`, `}`, `|` and the text after it (a menu's shortcut), a menu's `&` (its letter).
- `menu.*`, `label.<n>` are MENU.TXT's and LABELS.TXT's lines; `rules.*` the names of
  RULES.TXT (the advances, units, improvements ..., one per key: the rules' numbers stay in the
  game's RULES.TXT); `describe.<GROUP>.<entry>` the Civilopedia's descriptions, a paragraph a
  line; `council0.*` ... the High Council's lines (`t3, 262,357, ` and the like before the
  words stay: they choose the advisor and the video's frames).
- The scenarios' own files are not translated (a scenario's texts are its own).
- `tests/lang_test.py` checks a shipped translation's game keys are of those files, once each;
  that each text keeps the English's lines and codes is checked against your copy of the game
  (an export) by whoever translates it.

## Encoding and code pages

The game's text is single bytes, drawn with the glyphs of Windows-1252 (Western European:
English, French, German, Spanish, Italian, Portuguese, Dutch, the Nordic languages ...). A file
is read as Windows-1252 unless its first entry says otherwise:

```
@@codepage
iso-8859-3
```

Then the bytes where that code page differs from Windows-1252 are drawn with glyphs captured
for it, of the same fonts at every size (`data/native/fonts_iso-8859-3.bin` and `_aa.bin`), the
host shows the text (message boxes, the menu bar, the set-up) as those characters, and
characters typed on a keyboard layout that has them (names of cities, of saved games) are put
in as those bytes; others are dropped. Code pages so far (`src/host/codepages.h`):

| Code page | Languages | Glyphs |
|-----------|-----------|--------|
| `windows-1252` (default) | Western European | `fonts_win.bin`, `fonts_win_aa.bin` |
| `iso-8859-3` (Latin-3) | Esperanto (ĉ ĝ ĥ ĵ ŝ ŭ), Maltese | `fonts_iso-8859-3.bin`, `_aa.bin` |

Write the file in its code page's bytes (e.g. `iconv -f utf-8 -t iso-8859-3`); the browser page
reads it in the same code page and shows the letters as Unicode.

Where a code page's glyphs are missing (a build without them), Esperanto can be written in the
**x-system** instead, in Windows-1252: `cx gx hx jx sx ux` for `ĉ ĝ ĥ ĵ ŝ ŭ` (`Cx` ... for the
capitals), as Esperanto does without its letters.

**Adding a code page** (`cp1250` for Polish or Czech, `cp1251` for Cyrillic):

1. its table in `src/host/codepages.h` (the Unicode character of bytes 0x80-0xFF) and its
   names in `codepage_table`;
2. its glyphs, captured under Wine with the same fonts as the others
   (`tools/dk2 sh tools/probe/run.sh fontdump /work/data/native/fonts_<name>.bin <name>` and
   `fontdump_aa ... fonts_<name>_aa.bin <name>`): only the bytes where it differs from
   Windows-1252, for every face, size and style;
3. nothing else: the builds package `data/native/fonts_*.bin` and `lang/*.txt` (the releases,
   the Windows program's resources, the browser site), and `make test` checks the glyph files
   cover every font.

The game's text is still single bytes: names saved under one code page (a city named in
Esperanto, a saved game's leader) show as other characters under another (in English, `ĉ` saved
as byte 0xE6 shows as `æ`), and the game's own texts from a Windows-1252 copy of the game show
their accented letters wrongly under another code page. A translation of the game's texts for
another code page is written in it too.

## Testing a translation

- `make test` (`tests/lang_test.py`) checks that `en.txt` is `src/host/texts.h`, that every
  translation in `data/native/lang` has every key of the template and no other, each with the
  template's placeholders in order, and its code page's glyph files.
- `CIV2_LANG=<code>` runs the game in it; `harness/extras/x-lang-eo.txt` (`make test-extras`)
  shows OpenCiv2 Options, its Language page, the file dialog and a city renamed with Esperanto's
  letters typed, in Esperanto (and with sharp text), and the game's texts in it: the menus, the
  city window, an advisor, the Civilopedia.
- The browser page: `?lang=<code>`; `src/web/test/smoke.mjs` looks at the page in Esperanto.

## Adding a translation to OpenCiv2

A translation of OpenCiv2's own texts is welcome: `data/native/lang/<code>.txt`, its header
saying who wrote it, or *machine-drafted, needs review by a native speaker* until a speaker has
reviewed it. It may translate the game's texts too (as above): only the translation, never
the game's English.
