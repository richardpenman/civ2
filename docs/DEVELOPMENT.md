# Developing OpenCiv2

How the reconstruction is organized and checked. Building and playing: [README.md](../README.md);
progress: [STATUS.md](STATUS.md).

## Layout

| Path | What |
| --- | --- |
| `src/auto/` | The game: decompiled once, now C++ edited directly, a file per subject (`city.cpp`, `unit.cpp`, `ai.cpp`, `diplomacy.cpp`, `map.cpp`, `window.cpp` …). |
| `src/game/` | Functions the decompiler could not translate, reconstructed by hand. |
| `src/data/` | The game's static data, laid out exactly as in the original: defined in `gamedata.cpp` (address order), declared by subject in `state/` (`game.h`, `city.h`, `network.h` ..., the texts in `strings.h`); records listed in `re/state_records.txt`. |
| `src/include/` | Types recovered from the binary: units, cities, civilizations, map, the game's classes. |
| `src/platform/` | The Windows API the game uses (windows, GDI, controls, files, sound, video) implemented in portable C. |
| `src/host/` | What the platform layer needs from the machine: window, input and audio over SDL2; the portable build's runtime. |
| `src/native/` | The C runtime pieces the native builds need. |
| `src/third_party/` | Bundled third-party decoders (Indeo 4 video, Ogg Vorbis). |
| `data/` | Fonts as the original renders them, and the original executable's data sections. |
| `re/` | Analysis results: function list, calling conventions, data layout, Ghidra's decompilation. |
| `harness/` | The replay scripts and the comparison with the original (`run.py`, `compare.py`, `native_suite.sh`). |
| `tests/` | The unit tests (`make test`: recorded calls of the game's functions, `tests/unit`) and small standalone tests (data layout, network records, CD music reading). |
| `hook/` | An `XDaemon.dll` proxy that makes the original deterministic under Wine. |
| `tools/` | Build, analysis and refactoring scripts. |
| `docs/` | Notes on the reconstruction and its verification; progress in `STATUS.md`. |

## Approach

1. **Inventory**: Ghidra decompiles all ~4,900 functions; names and types for every function,
   global, class and field were recovered from their use.
2. **Translation**: the decompiled code was turned into C that compiles natively, then edited
   by hand: names, types, struct access, real locals, data as ordinary variables. It is now C++
   (`docs/hand-reconstruction.md`).
3. **Platform**: the game keeps calling the Windows API; `src/platform` implements the part it
   uses on top of SDL2, so the same game code runs everywhere.
4. **Determinism**: under Wine, the `hook/` proxy DLL pins the original's random seed, gives it a
   virtual clock and replays recorded input. The native builds do the same, so a recorded
   scenario plays identically in both.
5. **Differential harness**: each scenario in `harness/scripts` (new game, turns, cities,
   diplomacy, editors, 50 turns of play …) is recorded from the original once; the native builds
   replay it and must produce the same screenshots and the same sequence of `rand()` calls.
   Changes to the code are also checked function by function against the previous version's
   machine code (`tools/cxx_compare.py`), which covers the code no scenario reaches.

## Tooling

Development runs in Docker on Linux; `tools/dk CMD` runs a command in the dev container with
the repository at `/work` and the game at `/civ2`.

| Tool | Notes |
| --- | --- |
| Docker images (`make images`) | Wine (runs the original), MinGW, g++/gcc with 32-bit support, SDL2, Python; Ghidra for the analysis. |
| wasi-sdk + wabt (`tools/fetch-toolchain.sh`) | The portable build: the game compiles to a wasm32 module (keeping the original's 32-bit data model), which wasm2c turns into C for any machine. |
| `hook/` (`make hook`) | The proxy DLL that makes the original replayable. |

```sh
make native          # build/civ2-linux32: gcc -m32, the same code, easier to debug
make port            # build/OpenCiv2 in the container
make ref SCRIPT=harness/scripts/turns.txt    # record the original on a scenario
make native-suite    # replay every scenario on a native build and compare (EXE=OpenCiv2, OpenCiv2.exe under Wine)
make test            # the unit tests: recorded cases of the game's functions, in seconds (no game files needed),
                     # then the source probes (tests/*_test.cpp: memory-error fixes, g++ -m32)
make test-extras     # the additions' tests: harness/extras replayed and compared with our recorded output
make unit-record     # record those cases again from the replays (needs the game and run/ref-*)
```

The replay checker requires a nonempty, matching set of PNG screenshots (including
dimensions and pixels) and nonempty `rand.log` traces on both sides. A seed-only
trace is valid; missing or empty logs are not evidence of matching random calls.
Where the recording has a `calls.log` it must match too: the sounds the game plays
(`snd_play_effect`: effect, mode, loop) and the videos it looks for (each `.avi` path
`file_resolve_path` is given), in order, which the reference cannot play (no audio
device, no video files or Indeo codec). The hook logs them from breakpoints on the
original's two functions (`CIV2_CALL_LOG`, hook/stackprobe.c), the native builds
from the same functions (src/platform/replay.c).
Both `native_suite.sh` and `diff_suite.sh` fail a scenario if its runner or BMP
conversion fails, then continue with the remaining scenarios and return failure
for the suite. A failed run is not subsequently reported as `PASS` or `SAME`.

Checker regression tests use synthetic images and a stub game runner. They need
Python 3, Pillow and a POSIX `sh` for the suite tests, but no game assets, native
build or Wine. Without `sh`, only the suite tests are skipped.

```sh
python3 tests/test_replay_harness.py -v
# Or in the development container:
tools/dk python3 tests/test_replay_harness.py -v
```

## Tests of the additions

`make test-extras` checks OpenCiv2's additions to the original (docs/EXTRAS.md: the interface
additions and the rule changes), which the original game has no recording of.  The scripts in
`harness/extras` replay them on the native build (`harness/extras_suite.py`) and are compared with
our own output, recorded once after checking its screenshots by eye: `harness/extras/expected/NAME.txt`
holds a hash of each screenshot's pixels and of the `rand()` trace, `NAME.rand.gz` the trace itself
(about 200 KB in all; no image or text of the game's).

* A script says how it is run in its first comment lines: `# test: CIV2_TEST_EXTRAS=1
  CIV2_TEST_FULL=640x480` (the variables of the run), and `# test LABEL: ...` for a further run of
  it, named `NAME-LABEL`.  `CIV2_TEST_EXTRAS` turns additions on by number (`1`: all the interface
  additions; `17,42`: those rule changes), `CIV2_TEST_SETTINGS` sets `CIV.INI` values
  (`RetirementYear=1902,Gamma=150`), `CIV2_TEST_SUPPRESS` the popups shown on the map, and
  `CIV2_TEST_FULL=WxH` shows the whole game window on a WxH desktop (the interface scripts: 640x480)
  instead of the reference desktop of the replays (the rule changes' scripts).
* Each run's screenshots (PNG) and logs are in `run/extras-NAME`.  On a failure the suite names
  the screenshots that differ and the first `rand()` call that does, and writes
  `run/report-extras-NAME/index.html`: the differences from the blessed screenshots where this
  machine has them (`run/extras-blessed`, kept by the last bless), else this run's screenshots.
* An intended change of what a script shows: run `make test-extras` (or `NAMES="x-prod x-wheel"`),
  look at `run/extras-NAME/*.png`, then `make extras-bless NAMES=...`.  Blessing runs each script
  twice and refuses one whose two runs differ (`--twice` checks that without blessing).  A new
  addition gets a script and is blessed the same way.
* The replays' virtual clock (as the original's under the harness) lets 10 ms pass at each empty
  check of the message queue, and the map's drawing checks once per square, so redrawing a large
  map takes seconds of the script's time (docs/STATUS.md): wait long enough (about 12 s) after
  anything that redraws the map before a screenshot or a click.
* The tests need the game's files, like the replays, so they do not run in the release workflow
  (which has no game files and must not get them); run them locally before pushing changes to the
  additions.

`tools/native-run.sh` stops a run that hangs after `TIMEOUT` seconds (default 600; the suites:
900): the game ignores SIGTERM, so it is killed `TIMEOUT_KILL` seconds (default 10) later.

## Unit tests

`make test` checks the game's logic in seconds (2.5 s for the cases, a few more to build):
10,481 calls of 836 of the game's functions (combat odds and strengths, movement and paths,
city food, shields, trade, happiness and
production choice, the AI's assessments and unit orders, research, diplomacy, map generation steps
...), each recorded from a replay of the reference scenarios, called again with the same arguments
and state, and compared with what it did then: its return value, every byte of the game's state it
changed and its `rand()` calls.  A change to the logic that any recorded call sees fails the test,
naming the function, the call and the first byte that differs (`cities+0x1a2`).

* **Recording** (`tests/unit_record.c`, `make unit-record`).  The recording build
  (`build_native.py --target linux32rec`) compiles the game and the platform layer with
  `-finstrument-functions`: every function calls the recorder on entry and exit.  While a replay runs
  (`tools/unit_record.sh`: the scenarios in `UNIT_SCRIPTS`, 35 of the 41, each run checked against
  the original's recording as by the suite), the recorder takes some calls of each game function (the 1st, 2nd, 3rd, 4th, 6th,
  9th ...; at most 6 per scenario and 16 in all): the argument words (by the signatures
  `tools/unit_sigs.py` gets from the debugging information), the state at entry (the game's data,
  `src/data/gamedata.cpp`, and the map's blocks), the return value (the function's return address is
  pointed at a trampoline that catches it) and the state at exit.  A call that reaches the platform
  layer (windows, drawing, files, the clock: anything but the `rand()` trace), allocates or frees
  memory, or changes state outside the game's data (`tools/unit_sigs.py --statics`) is not taken.
  Arguments that point into the caller's stack are recorded with the stack they point into (and what
  pointers there point to); a pointer anywhere else but the game's data and the map is not taken.
  The `rand()` generator's seed is game data, so a case replays its random numbers exactly.
* **Running** (`tests/unit_run.c`, `make test`).  The runner (`--target linux32test`,
  `build/civ2-unittest`) is the game with its initializers run and no window: for each case it sets the
  game's data and the map as they were, calls the function, and compares (crashes and hangs are
  failures, not aborts).  Both builds put the game's data at a fixed address (0x30000000), so
  pointers into it mean the same in the recording and in later builds; both are instrumented, as gcc
  passes a static function's arguments in registers unless its address is taken.  After recording,
  the runner keeps only the cases it reproduces, twice in each of three processes
  (`civ2-unittest --bless`): calls that read memory outside what is recorded (the heap: the recorder
  moves its heap away so that such pointers point to nothing) drop out there.
* **Data** (`tests/unit/*.gz`, 2.6 MB).  A case's state is a delta from the previous case's,
  compressed.  No text from the game's files is recorded: where the game's data holds text (every
  `char` array and text buffer, `tools/unit_textmask.py` -> `tests/unit/textmask.tsv`) the cases have
  what the game's initializers leave there, and on the stack what looks like text is zeroed; the
  runner compares the same way.  So the tests need no game files and run anywhere the 32-bit Linux
  build does (the release workflow runs them).  `tools/unit_coverage.py` lists the functions with
  cases by file (`tests/unit/cases.tsv`).
* **When the code changes**.  Renamed functions are skipped (named in the report); a change of the
  game data's size or layout needs a new recording (`make unit-record`, about an hour), as does a
  function whose signature changes.  A failing case after an intended change of behaviour: check it
  against the replays (which compare with the original), then record again.
