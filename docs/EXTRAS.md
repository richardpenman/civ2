# Interface additions

OpenCiv2 adds to the original game's interface the features of
[CIV2UIA](https://github.com/FoxAhead/Civ2-UI-Additions), the "Civilization II User Interface
Additions" launcher by FoxAhead, which patches the original executable. They are written anew
for OpenCiv2's code from CIV2UIA's description of them; no code of CIV2UIA's is used.
Thanks to FoxAhead and the CIV2UIA contributors for the ideas and for documenting them.

This file covers the additions to the interface, which change only what the player sees and
does and are on by default, and, under [Rule changes](#rule-changes), the changes to the
game's rules (with the limits CIV2UIA takes from
[civ2patch](https://github.com/vinceho/civ2patch)), which are off by default. CIV2UIA's bug
fixes are in [BUGS.md](BUGS.md).

## On and off

The additions are on by default. They are off:

- with `CIV2_CLASSIC=1` (the original game: its look, its bugs, and none of these);
- in replays and unit tests, which are compared with the original game.

They change only what this player sees and how the interface is used, never what the game
computes, so they stay on in network games (a production changed from the City Status
advisor, or specialists changed with the wheel, are the same orders as through the city
window).

Each one can be switched off in [OpenCiv2 Options](#openciv2-options) (Interface additions), in
the game's main menu, or in the `[OpenCiv2]` section of `CIV.INI` (the file the game keeps its
own settings in, in its Windows directory, where OpenCiv2 Options writes them too): its key `=0`.
A change made in OpenCiv2 Options takes effect at once; one made in `CIV.INI` the next time the
game starts.

| Key | What |
| --- | --- |
| `MouseWheel` | Use the mouse wheel and middle button. The wheel scrolls whatever list or scroll bar is under the pointer (advisors, dialogs, the city window, the Civilopedia), changes the specialist under the pointer in the city window, and moves the tax rate bars. Ctrl+wheel zooms the map and Ctrl+middle click returns to normal zoom. Middle click centres the map on that spot; dragging with the middle button pans it. |
| `UnitListScroll` | When you click a stack of more than 9 units, the list of units to activate shows all of them and scrolls, instead of stopping at 9. |
| `UnitListKeys` | In that scrolling unit list, the arrow keys, Page Up/Down and the mouse wheel move through the list. Needs `UnitListScroll`. |
| `WorkCounter` | Your settlers and engineers show on the map how much terrain work they have done so far. |
| `GameTurn` | The status panel shows the turn number after the year. |
| `ResearchNumbers` | The Science Advisor shows the research points collected so far, the cost of the current advance, and the points gained per turn. |
| `ZeroUpkeep` | The Trade Advisor also lists buildings that cost nothing to maintain. |
| `SupportScroll` | The city window shows how many units the city supports, with a scroll bar when they don't all fit. |
| `SupportSort` | The city window's supported units are sorted: settlers and engineers first, then attack units (land, air, then sea; strongest first), then the rest. |
| `AttitudeColors` | The Attitude Advisor shows celebrating cities in yellow, and an icon after a city's name when it has a food surplus or shortage. |
| `CityAttitude` | In the city window, the text under the citizens is coloured as in the Attitude Advisor: yellow when the city celebrates, red when it is in disorder. |
| `RadioHotkeys` | Options in dialogs get a keyboard shortcut: a letter of the option's name (underlined only with `HotkeyUnderlines`). |
| `Quickinfo` | Hold Ctrl with the pointer over a map square to see details of the city and units there. |
| `ProductionCancel` | The city's Change Production dialog gets a Cancel button; Escape cancels too. |
| `ProductionCost` | The Change Production list shows what each item costs in shields, and each building's upkeep. |
| `CityStatus` | The City Status advisor shows how many cities you have and each city's production as a picture. Click a column heading (size, name, food, shields, trade, production) to sort by it; click it again for the game's usual order. Click a city's production to change it. |
| `MassSpecialists` | In the city window, Shift+wheel over a specialist changes all specialists at once, and Shift+click on one makes all the others the same type. |
| `SuppressPopups` | Simple messages that only have an OK button can be shown for a few seconds across the top of the map instead of waiting for a click. List which ones by their GAME.TXT section names in `SuppressList=`, separated by commas, for example `SuppressList=HIDDENTERRAIN,CITYSIZE`. The message that another civilization has discovered an advance isn't in GAME.TXT; list it as `ADVANCENEWS`. The list is empty by default. |
| `ColorCorrection` | Adjusts the colours of the whole screen: `Gamma=` (a percentage, 100 means no change), `Brightness=` and `Contrast=` (-100 to 100, 0 means no change). These and `SuppressList=` are also in OpenCiv2 Options (Colours and messages). |
| `ForeignGold` | The Foreign Minister shows each civilization's gold with a coin icon. |
| `ArrangeWindowsSL` | The View menu gets Arrange Windows S and Arrange Windows L, which arrange the windows with a narrower or wider side panel than Arrange Windows. |
| `PathLines` | Hold Shift with the pointer over the map to see the path the active unit would take there, based on the map as you know it (units and cities you can't see aren't counted). With a city selected (View Pieces, the cursor on it), hold Shift over the same city to see lines to the cities it has trade routes with, or over another city to see the way a road or railroad must take to give their trade routes the road bonus (the game looks for it along this way only, up to the other city or one of your cities on the way): yellow if the road is already there and gives the bonus, white if not. |
| `SentryHalftone` | Units on sentry, and aircraft at a base, are drawn in grey shades so you can still tell what they are, instead of as a flat grey shape. |
| `DefaultZoom1` | A new game (or a save without a saved window layout) starts with the map at normal 1:1 zoom, whatever the screen size. |
| `TurnsLeft` | The city window counts the turns: on the Food Storage title's line, the turns until the city grows ("9 turns"; "Needs Aqueduct" when it cannot grow; "No surplus"; "Famine: N" when it will shrink in N turns), and on the production's title line the turns until the item is completed ("No production" when no shields are left over). They are counted as the game's turn does it: a food box filled this turn grows the city at the next; shields are added before the item is completed. Where the production's name leaves no room on its line, the count is left out. OpenCiv2's own (not one of CIV2UIA's). |
| `HotkeyUnderlines` | Off by default: the hotkey letters `RadioHotkeys` gives options are underlined, as in the original's Windows dialogs. Off, the letters still work, only the line under them is not drawn. |

## File dialogs

The Open and Save dialogs can list files newest first (the Sort button), and they open in the folder of the last file loaded or saved. Both are remembered in CIV.INI under `[OpenCiv2]` as `FileSort=` and `FileFolder=`.

## Screen scale

The game opens in a window, maximized to fill the screen. To play full screen instead, tick Full
screen in OpenCiv2 Options (Graphics and screen), or set `FullScreen=1` in `CIV.INI`; it takes
effect the next time the game starts.

On a high-resolution screen the game is drawn larger, in whole steps, so it stays sharp: each
of the game's pixels becomes 2×2, 3×3 or 4×4 of the screen's. On Windows the step follows the
display's Scale in Settings > Display, as the original is enlarged there, to the nearest whole
step: 100% 1 (the original's look), 150% and 200% 2, 300% 3. Elsewhere it is the step that
brings the game nearest the original's 1024×768 (in area), leaving it at least 800×560: a
MacBook Air's window 3 (the game at 980×594), a 4K screen 3 (1280×720), 2560×1440 2
(1280×720), 1920×1080 1. `Scale=` in `CIV.INI` sets the step instead (0: by the screen's size); it takes
effect the next time the game starts. The game is never made smaller than 640×480.

Text is drawn with fonts that have the widths of Windows' Times New Roman, Arial and Courier
New (the Liberation fonts, `data/fonts_win.bin`), so it is laid out as the original's on
Windows.

## OpenCiv2 Options

The game's main menu has one more choice after View Credits, *OpenCiv2 Options*, where the
player chooses how to play; the choices are kept in `CIV.INI`'s `[OpenCiv2]` section (only
those changed are written).  It is the game's own dialogs, as the Game menu's options: a list of
what can be chosen, each opening a dialog of its own (OK writes the choices, Cancel leaves them):

- **Mode**: *Standard* (the original's bugs fixed, [BUGS.md](BUGS.md), and these additions) or
  *Classic* (the original game: its look, its bugs, none of these; `Mode=Classic`, as
  `CIV2_CLASSIC=1`).  In Classic the other choices but the graphics are not offered.
- **Graphics and screen**: [sharp text and interface](#sharp-text-and-interface) (`SharpText=`,
  on by default in Standard, off in Classic); [HD graphics](#hd-graphics) (`Graphics=HD`, at the
  screen's own density), the art downloaded when they are ticked where it is not installed (234 MB,
  with its progress and a Stop button; see [HD-GRAPHICS.md](HD-GRAPHICS.md)); full screen
  (`FullScreen=`).
- **Rule changes** and **Limits**: the [rule changes](#rule-changes) and the limits.
- **Interface additions**: the additions above.
- **Colours and messages**: the colour correction (`Gamma=`, `Brightness=`, `Contrast=`) and
  the messages shown on the map (`SuppressList=`).

The interface additions, the rule changes and the limits take effect at once (from the game
started or loaded next).  The mode, the graphics and full screen take effect when the game
starts again: the dialog offers *Restart now*, which starts the program again with the same
arguments but `--hd` and `--sharp` (in the browser version: the page is loaded again), or
*Later*.  `CIV2_CLASSIC=1`, `--hd` / `CIV2_HD` and `--sharp` / `CIV2_SHARP` win over the
choices made there, for the run they are set in (the dialog says so).

It is in the main menu in Standard and in Classic mode alike (the only thing Classic shows that
the original does not), and in the browser version (where HD graphics are offered only where
the site has their art, [BROWSER.md](BROWSER.md#hd-graphics), and full screen is the browser's).
It is not in replays and unit tests, which compare the main menu with the original's
(`CIV2_TEST_OPTIONS=1` shows it in a replay, where nothing is written to `CIV.INI`, downloaded
or restarted: `harness/extras/x-options.txt`).  Its texts are English only
(`src/game/options.cpp`).

These choices were made before in an opening window shown before the game started (and, before
that, in Game > OpenCiv2 Options); OpenCiv2 Options in the main menu replaces both.

## Sharp text and interface

On a high-resolution screen (a [screen scale](#screen-scale) of 2 or more) the game is shown at
the screen's full resolution: text is drawn from glyphs of the same fonts as many times as tall,
and what the platform draws itself (the menu bar and its menus, window captions and frames,
buttons, check boxes and radio buttons, scroll bars, edit boxes, list boxes, combo boxes, the
file dialog) is drawn at that resolution: its curves and diagonals (radio
buttons, check marks, the scroll bars' arrows, the combo box's arrow, the close box) drawn smooth,
its straight lines and edges as many screen pixels wide as the game's pixel is.  The game's own
pictures (terrain, units, cities, portraits, backgrounds, the icons of its dialogs) stay the
original bitmaps, enlarged as before.  It is on by default in Standard mode, off in Classic, and
can be switched in OpenCiv2 Options (Graphics and screen) or with `SharpText=1` / `=0` in
`[OpenCiv2]`; `--sharp` (or `--sharp=0`) on the command line, or `CIV2_SHARP=1` / `=0`, wins for
that run (`--sharp=N`, N from 2 to 4, sets the density, for tests).  Where the screen scale is 1
(a 1920x1080 screen, or Windows' display scale at 100%) it has nothing to do and changes nothing.  It is drawn by the HD graphics'
renderer without their art ([HD-GRAPHICS.md](HD-GRAPHICS.md#the-planes-sharp-text-and-interface));
with HD graphics on as well, the two combine.  It changes only what is shown: the game's own pixels,
which its code reads back, and its random numbers are the same with it or without.  Off in replays
and unit tests (`CIV2_TEST_SHARP=N` turns it on at density N in a replay:
`harness/extras/x-sharp.txt`, and OpenCiv2 Options' `x-options-sharp`).  In the browser version:
in OpenCiv2 Options as natively, or `?sharp=0` in the address ([BROWSER.md](BROWSER.md#hd-graphics)).

## HD graphics

`Graphics=HD` in the same `[OpenCiv2]` section (HD graphics in OpenCiv2 Options; or `--hd` on
the command line, or `CIV2_HD=1`) draws the map (and wherever the same pictures appear) with
The replacement art (openciv2-assets), once installed (by OpenCiv2 Options, or `make hd`), at the screen's
full resolution (`GraphicsDensity=`).  Off by default, with `CIV2_CLASSIC=1`, in replays and
unit tests; it changes only what is shown.  See [HD-GRAPHICS.md](HD-GRAPHICS.md).

## Rule changes

CIV2UIA's items that change the game's rules, and the limits it took from civ2patch, are off
by default: each is switched on in OpenCiv2 Options (Rule changes, Limits) or in the
same `[OpenCiv2]` section of `CIV.INI`, the item by its key `=1`, a limit by its value (`=0` or
no key: the original's). They are off with
`CIV2_CLASSIC=1`, in replays and unit tests, and in network games (as the state fixes of
[BUGS.md](BUGS.md): every player, the original game included, must compute the same game);
they stay on in hot-seat games. The changes for units apply to the human
players' units only: the computer players play by the original rules.

| Key | What |
| --- | --- |
| `WorkerHandover` | When a settler or engineer joins another already doing the same terrain work on its square, the newcomer takes over the work done so far and the first one is freed to go elsewhere. Only one worker keeps the order on each square. |
| `GotoNoZocStop` | A unit on Go To keeps going when it comes next to a foreign unit, instead of stopping (in the original, only caravans and freight do). Zones of control still apply: it stops when one blocks its way or it runs out of moves. A unit sent at an enemy will attack whatever is in its way. |
| `UnitRotation` | When you activate a unit, the other units on its square come next before any elsewhere. After that, the next unit is the one nearest to where you last activated a unit, rather than to where that unit has since moved. |
| `MassMove` | Shift+right click on a square sends the active unit there along with every unit of the same type on its square that can move and has no orders (not fortified, on sentry or working). Each goes as far as it can this turn, as with Go To. |
| `StackSentry` | `U` normally wakes the land units on sentry in a stack. If none is on sentry, `U` instead puts all the stack's land units that have no orders on sentry (on a ship: loads them). |
| `RetirementYear=` | Sets the year the game ends with retirement (1 to 9999; the original is 2020). `RetirementWarningYear=` sets the year of the warning (0 for none; by default 20 years earlier). Each happens on the first turn on or after that year. The game's own messages still say "in 20 years". |
| `PopulationLimit=` | Raises the highest population a civilization can reach, in units of 10,000 people (32000 to 3275800; the original is 32000, which is 320 million). The Hall of Fame still records at most 320 million. |
| `GoldLimit=` | Raises the most gold a treasury can hold (30000 to 1000000; the original is 30000). |
| `MapTilesLimit=` | Raises the largest custom world, as width × height in squares (10000 to 32767; the original is 10000). Each side is still 20 to 250 squares. The dialog's text still says 10,000. |
