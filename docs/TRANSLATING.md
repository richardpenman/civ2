# Translating OpenCiv2

OpenCiv2 has two kinds of text:

- **The game's own texts** (`GAME.TXT`, `RULES.TXT`, `LABELS.TXT`, `DESCRIBE.TXT`, `CITY.TXT`,
  `MENU.TXT`, the advisors', the scenarios' ...) come from your copy of Civilization II, as the
  original reads them. A localized Civilization II (German, French, Spanish ...) gives them in
  its language as it is. A translation may carry them too (keyed: below): the German,
  Latin, Greek and Esperanto ones that come with OpenCiv2 translate the whole game. Where your copy of the game has no
  such file, OpenCiv2 uses its own English of it (below).
- **OpenCiv2's own texts**: what it adds — Settings, the interface additions' few words,
  the file dialog, its message boxes, the set-up's messages and progress, the HD graphics'
  download, the browser version's page. These are translated here. English is built in;
  `data/native/lang/` has the translations that come with OpenCiv2 (first drafts:
  machine-drafted, need review by a native speaker): German (`de.txt`, Deutsch) and
  Latin (`la.txt`, Latina: classical, with Vicipaedia's words for modern things), in
  Windows-1252, Greek (`el.txt`, Ελληνικά, monotonic) and Traditional Chinese (`zh-Hant.txt`,
  繁體中文, Taiwan usage), in UTF-8, and Esperanto (`eo.txt`); all of them translate the game's
  texts too.

The game names itself OpenCiv2: where the game's texts (your copy's, OpenCiv2's own English or a
translation's) name the game being played as "Civilization II", "Civilization II Multiplayer
Gold", "Civ II", "Civ2" (or a translation's "文明帝國II"), the game shows "OpenCiv2" instead, as
it reads the line (`own_name`, `src/game/lang.cpp`). A translation keeps the original's name, as
the original's text has it. The original product keeps its name: a line naming a CD, the
original game or credits, GAME.TXT's sections about its CD, its installation and whose credits,
and the credits themselves. Not in the original game (`CIV2_CLASSIC=1`), nor in replays and the
unit tests, which compare with the original (as the interface additions: `plat_own_name`).

## OpenCiv2's own English of the game's texts

`data/native/text/` holds OpenCiv2's own English of the game's text files: `GAME.TXT`,
`LABELS.TXT`, `MENU.TXT`, `CITY.TXT`, `PEDIA.TXT`, `DESCRIBE.TXT`, `ADVICE.TXT`, `HELP.TXT`,
`TUTORIAL.TXT`, `COUNCIL0-2.TXT`, `HERALD.TXT`, `MAPMENU.TXT`, `DEBUG.TXT` and the credits
(OpenCiv2's). They are in the game's format (sections, `@title=`, `@options` ..., Windows-1252)
and are read only where neither a scenario's folder nor the game's folder has the file: your
copy's own file always wins, and a translation's keys apply on top of either. (`RULES.TXT` is
built in: `src/game/rules_default.cpp`.)

They were written for OpenCiv2, not copied from the original: short functional strings (menu
items, buttons, labels, names) directly, everything longer (the Civilopedia, the advisors and
the High Council, help, the tutorial, the dialogs' messages) clean-room, by writers who saw
only a description of each text's structure, purpose and facts. Their structure matches the
original's line for line where the game reads it (`tests/text_test.py`, `make test`; the
structure expected is `tests/text_skeleton.txt`); the Civilopedia's articles are shorter.

A translator may start from them: `CIV2_EXPORT_TEXT=xx.txt` run without the game's text
files (as `CIV2_NO_TEXT=1` does in the tests, below) writes them keyed, and a translation made
from them may be added here. `CIV2_NO_TEXT=1` (`tools/native-run.sh`) runs a test's game copy
without its text files.

## Choosing a language

- In the game: *Settings > Language* (main menu): *Automatic* (the system's language),
  *English*, or one of the translations in `lang/`. It is kept in `CIV.INI` (`[OpenCiv2]
  Language=eo`; `Auto` or none: automatic) and takes effect when the game starts again.
- `CIV2_LANG=<code>` in the environment wins over that choice (and is the only one test runs
  read).
- In the browser version: the address's `?lang=CODE`, for the start screen and the game (without
  it the language chosen in Settings, else English); the game's *Automatic* is English
  ([BROWSER.md](BROWSER.md#language)).

A code is a file's name: `eo` reads `lang/eo.txt`; a system's `pt_BR` reads `lang/pt-BR.txt`,
else `lang/pt.txt`. Chinese in traditional characters (a system's or browser's `zh_TW`, `zh-HK`,
`zh_MO`, `zh-Hant...`) reads `lang/zh-Hant.txt` (`src/host/codepages.h`: `language_alias`);
other Chinese (`zh_CN`, `zh-Hans`) is not taken for it. The file is looked for in the game's folder (`lang/` beside its files: a
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
  itself, as the Language choice shows it in every interface language (letters the
  interface's code page lacks, such as Greek or Chinese in Windows-1252, are drawn from the
  font files of `data/native/fonts`).
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
game: keep it to yourself (it is not added here); a translation made from it may be. Made
from a folder without the game's text files, it is OpenCiv2's own English (above), which is
free to share and a good base for a translation.

### The game's texts

Their keys say which file and which part of it they replace; a key the translation lacks keeps
the game's English. What to keep as it is:

- Every text has the same lines as the English: a popup's blank line ends its text and starts
  its options; `@width=`, `@options`, `@listbox`, `@columns=` ... and `;` comments stay as they
  are, but the text of `@title=` and `@button=`. Plain lines are joined and wrapped; a line
  starting `^` (`^^` centred) is drawn as it is, so it must fit the dialog's width.
- `%STRING0`, `%NUMBER0` ... (the game fills them in by number) and `%s`, `%d` (in order);
  `{`, `}`, `|` and the text after it (a menu's shortcut), a menu's `&` (its letter).
- `advice.@@<name>`: ADVICE.TXT's advice to build a unit or improvement (the city window's
  advisors), found by its English name as in the file (`advice.@@Temple`), whatever the
  name is in the translation; a block that serves several (`@@Coastal Fortress` and
  `@@SAM Missile Battery`) is keyed by the first, its other `@@` names (written `\@@...`)
  are its first lines and stay as they are.
- `label.0` and `label.1` (B.C., A.D.) may hold `%d`: then each is the whole date's form, the
  year at `%d` (`西元前%d年`, `%d n. Chr.`); without it the label goes before (A.D.) or after
  (B.C.) the year, as in English.
- `menu.*`, `label.<n>` are MENU.TXT's and LABELS.TXT's lines; `rules.*` the names of
  RULES.TXT (the advances, units, improvements ..., one per key: the rules' numbers stay in the
  game's RULES.TXT); `describe.<GROUP>.<entry>` the Civilopedia's descriptions, a paragraph a
  line; `council0.*` ... the High Council's lines (`t3, 262,357, ` and the like before the
  words stay: they choose the advisor and the video's frames).
- The scenarios' own files are not translated (a scenario's texts are its own).
- `tests/text_keys_test.py` (`make test`) checks that the export keys every section and block
  of the game's text files (OpenCiv2's own English of them, and your copy's).
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

Then the bytes where that code page differs from Windows-1252 are drawn as their characters,
the host shows the text (message boxes, the menu bar, the set-up) as those characters, and
characters typed on a keyboard layout that has them (names of cities, of saved games) are put
in as those bytes; others are dropped. Code pages so far (`src/host/codepages.h`):

| Code page | Languages | Glyphs |
|-----------|-----------|--------|
| `windows-1252` (default) | Western European | `fonts_win.bin`, `fonts_win_aa.bin` |
| `iso-8859-3` (Latin-3) | Esperanto (ĉ ĝ ĥ ĵ ŝ ŭ), Maltese | Windows-1252's, put together (below) |
| `utf-8` | Chinese (`zh-Hant`), Greek (`el`), any other | Windows-1252's; the others from `data/native/fonts` (below) |

Write the file in its code page's bytes (e.g. `iconv -f utf-8 -t iso-8859-3`); the browser page
reads it in the same code page and shows the letters as Unicode.

**Letters with marks** (Latin-3's ĉ ĝ ĥ ĵ ŝ ŭ, ċ ġ ż İ ı ş ğ ħ ...) are put together when first
drawn from the captured glyphs of Windows-1252 (`src/platform/text.c`, its table `marks[]`), so
that they are hinted as the letters beside them, at every size and style, 1-bit and
antialiased: the base letter as captured (ĵ and ı without their dot), the mark taken from a
captured letter that has it (the circumflex of `â` and `Â`, the cedilla of `ç` and `Ç`, the dot
of `i`), placed as Liberation's composite glyphs place it (moved by half the difference of the
two letters' advances); the marks no Windows-1252 letter has (the breve of ŭ and ğ, the bar of ħ)
are drawn in the circumflex's box with its stroke. A letter takes its base letter's advance, so
text is laid out as with the glyphs Wine draws for it (they match those within a pixel or two).
The same letters in a UTF-8 translation are put together alike.

**Adding a code page** (`cp1250` for Polish or Czech):

1. its table in `src/host/codepages.h` (the Unicode character of bytes 0x80-0xFF) and its
   names in `codepage_table`;
2. each of its letters Windows-1252 lacks in `marks[]` of `src/platform/text.c` (base letter and
   mark; a new kind of mark, as the caron's or the ogonek's, needs its few lines there):
   `make test` (`tests/lang_test.py`) fails while a byte has no glyph;
3. nothing else: no files. (A script with letters of its own, as Cyrillic or Greek, would be
   written in UTF-8 instead, with a font file for its letters as Chinese has: below.)

The game's text is still single bytes: names saved under one code page (a city named in
Esperanto, a saved game's leader) show as other characters under another (in English, `ĉ` saved
as byte 0xE6 shows as `æ`), and the game's own texts from a Windows-1252 copy of the game show
their accented letters wrongly under another code page. A translation of the game's texts for
another code page is written in it too.

### UTF-8: Chinese and other languages beyond one byte a letter

A translation whose `@@codepage` is `utf-8` has the game's text in UTF-8: a character is one to
four bytes. UTF-8 rather than Big5 (or GBK) because a Big5 character's second byte can be
`@`, `\`, `^`, `|`, `{`, `%` ..., which the game's texts use as codes; UTF-8's are 0x80-0xBF
and never one of them. Then:

- **Drawing.** Characters up to U+00FF and Windows-1252's others are drawn with the game's
  glyphs (`fonts_win*.bin`), letters with marks put together from them (above); the others are
  drawn when first needed from the font files in `data/native/fonts` (below), on the font's
  baseline. A character they lack is drawn as a box; bytes that are no character (a sequence cut
  short) are not drawn and take no room.
- **Line breaking.** Chinese has no spaces between words: a line may break at a space, or before
  or after any wide character (CJK ideographs, kana, hangul, full-width forms), but never inside
  a character, not before a closing mark (`，。、；：？！」』）》〉】` and `,.;:!?)]}%`), not after
  an opening one (`「『（《〈【` and `([{`), and not next to `_` (the game's texts' space that
  does not break, as in `_._._.`). So it is in the game's dialogs (`Dialog::draw_wrapped_text`),
  DrawText (wrapped labels, the advisors' and the status panels' boxes) and the edit control
  (the Civilopedia's descriptions). In the game's texts the lines of a paragraph are joined: with
  a space between Latin words, with none where the line before ends or the next starts with a
  wide character, so a Chinese paragraph may be split over lines as the English one is.
- **Typing.** Characters typed (the system's input method, a keyboard layout's letters) go in
  whole: the host's text input hands them on one by one (all of an input method's text), the
  edit control's caret, Backspace, Delete, arrows, selection and clicks go by character, and a
  name cut to its room (a city's 15 bytes and the like) loses whole characters. The input
  method's composition (the text being composed, before it is chosen) is shown in the system's
  own window where it has one, not in the game's field; the browser version takes characters
  typed on a keyboard layout, but no input method (its canvas is not a text field).
- **Case.** The game changes the case of ASCII letters only (`toupper`, `strupr`, `CharUpper`:
  its C library's "C" locale), so UTF-8's bytes are never changed.
- **Menus' letters.** A menu's or button's `&` letter is a key on the keyboard: keep an ASCII
  one, after the label in parentheses, as Chinese Windows does: `遊戲(&G)`, `檢視(&V)`,
  `Παιχνίδι(&G)`. (The menu bar of a 640-pixel window has room for about 90 letters.)
- **The host** shows the text as it is (its message boxes, menu bar, set-up: UTF-8 already).
- **Saved games and names** are bytes: a city named in Chinese shows as other characters under
  another code page (in English, as Windows-1252 letters) and the other way round.

**The fonts** (`data/native/fonts`), a serif and a sans for each family of scripts: for Chinese
Noto Serif TC (for Times New Roman from 20 pixels up) and Noto Sans TC (Arial, System, Courier,
and Times below 20 pixels, where a serif's strokes are not legible with few pixels); for Greek
(and Cyrillic) Noto Serif (Times New Roman at every size, as the Latin letters beside it) and
Noto Sans (the others). SIL Open Font License 1.1 (`THIRD-PARTY.txt`,
`data/native/fonts/OFL.txt`), regular weight only, cut down to the characters the UTF-8
translations in `data/native/lang` use (and, with Chinese, common full-width punctuation),
without hinting or layout tables. `src/platform/text.c` draws them with stb_truetype
(`src/third_party`), Chinese at the font's pixel height, Greek with its capitals as high as the
captured `H` (so that both alphabets look one size), and keeps each glyph it has drawn:

- on the game's 8-bit surfaces (and where Liberation draws the Latin letters of that size
  without antialiasing) as 1-bit glyphs: a pixel is set where it is more than about half
  covered, or where it is the darker pixel across a thin stroke, so that strokes neither vanish
  nor thicken; elsewhere antialiased (coverage 0..16, a little more contrast than the outline's
  own, as the hinted glyphs beside them have), blended as Wine blends;
- placed, of the shifts by quarters of a pixel, where the fewest pixels are partly covered (up
  to 32 pixels: no hinting, but Chinese strokes are mostly level and upright, and this puts most
  of them on whole pixels);
- bold made by thickening (1-bit: a pixel wider, but never closing a one-pixel gap;
  antialiased: half a pixel), italics upright (Chinese has none); HD's larger text is drawn at
  its own size.

After adding characters to a translation run `tools/dk python3 tools/lang_fonts.py`: it fetches
the fonts (Noto Serif TC 2.003, Noto Sans TC 2.004, Noto Serif and Noto Sans 2.015 unhinted)
and fontTools 4.55.0, each pinned by its SHA-256, into `run/lang-fonts`, and writes the files
again (the same bytes for the same characters; a family no translation needs has none). `make
test` fails while a character of a UTF-8 translation is in neither Windows-1252, `marks[]` nor
both font files of its family.

Size: Greek's 68 characters take 17 KB (both files); Chinese about 230 bytes (sans) and 340
bytes (serif) a character: 1.3 MB for the 2,289 characters of `zh-Hant.txt`, the whole game
(the browser version's files are gzipped: some 15% less).

## Testing a translation

- `make test` (`tests/lang_test.py`) checks that `en.txt` is `src/host/texts.h`, that every
  translation in `data/native/lang` has every key of the template and no other, each with the
  template's placeholders in order, and that every character of its code page is Windows-1252's
  or put together (`marks[]`); a UTF-8 one, that it is valid UTF-8 and that the font files have
  every other character it uses. `tests/utf8_text_test.cpp`
  tests the UTF-8 handling itself (cutting to fit, break rules, the edit control's wrapping and
  caret), `tests/dialog_wrap_utf8_test.cpp` the game's dialogs' wrapping beside a picture (an
  advisor's: a line of Chinese kept inside the window).
- `CIV2_LANG=<code>` runs the game in it; `harness/extras/x-lang-eo.txt` (`make test-extras`)
  shows Settings, its Language page, the file dialog and a city renamed with Esperanto's
  letters typed, in Esperanto (and with sharp text), and the game's texts in it: the menus, the
  city window, an advisor, the Civilopedia; `x-lang-de.txt` and `x-lang-la.txt` the same in German and Latin.
- `harness/extras/x-lang-el.txt` does the same in Greek (its letters from the font files).
- `harness/extras/x-lang-zh.txt` does the same in Traditional Chinese: the main menu, Options
  and its Language page, the file dialog with Chinese typed and taken back character by
  character, the map's status panel, a city renamed in Chinese and its window, the menu bar, the
  Civilopedia's page of an advance and its description (wrapped), an envoy's greeting (lines
  joined and wrapped), the advisors, the tax rates and the date in Chinese (`西元前2400年`); and
  with sharp text.
- These tests measure text as the game does in play (`CIV2_PLAY_TEXT=1`): the other tests
  measure it as Wine did in the original's recordings (`CIV2_WINE_TEXT=1`,
  `tools/native-run.sh`), some letters narrower than they are drawn, so that a word drawn in a
  box of its measured width (the science advisor's advances) lost its last letter there.
- The browser page: `?lang=<code>`; `src/web/test/smoke.mjs` looks at the page in Esperanto and
  in Chinese (`?lang=zh-TW`: the page's fonts are the system's that have its characters), in
  English without `?lang`, and starts the game in Esperanto (`?lang=eo`).

## Adding a translation to OpenCiv2

A translation of OpenCiv2's own texts is welcome: `data/native/lang/<code>.txt`, its header
saying who wrote it, or *machine-drafted, needs review by a native speaker* until a speaker has
reviewed it. It may translate the game's texts too (as above): only the translation, never
the game's English.
