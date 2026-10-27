# Status

## Coverage (re/manifest.tsv)

| status      | functions | meaning |
|-------------|-----------|---------|
| auto        | 4040 | game functions, translated from Ghidra's output by `tools/gen_c.py`; since October 2026 the source, edited directly, in files by subject (src/auto/city.cpp, unit.cpp, map.cpp ...) |
| game        |  114 | reconstructed by hand (src/game/*.cpp) |
| ehfunclet   |  462 | C++ unwind funclets; inlined into their callers, never called on their own |
| crt         |  295 | statically linked MSVC 4.2 debug C runtime: the native builds use their own C library (src/native/crt.cpp for what differs) |
| asm         |    2 | `ASMGRAF` section: 16-bit leftovers from the Win16 version, unreachable |

About 221,000 lines of C++ (the game) and C (the platform layer).  Every
function that can execute has source.

## Builds

The native builds below.  Until step 5 of the clean-up (removing the
emulation of the original's memory layout) there was also a Windows build
grafting the compiled functions onto the original `civ2.exe`
(tools/build_recon.py; `make bisect` swapped single functions back to the
original's): retired, available at the git tag `pre-step5`.  It and the
standalone executable written from it were pixel-identical to the original
with the identical `rand()` sequence on these scenarios:

| scenario  | what it does | rand() calls |
|-----------|--------------|-------------:|
| menu      | boot to the main menu | 772 |
| newgame   | all new-game dialogs, world generation, first screens | 13 280 |
| loadgame  | load `Ca_Auto.SAV` | 1 039 |
| turns     | loaded game, 8 turns of AI play | 1 109 |
| advisors  | every advisor / report screen (F1–F9, F11) | 1 055 |
| long50    | new game, found a city, 50 turns of play | 14 057 |

## Native builds (no Windows, no Wine)

See `README.md`.  The Windows API is implemented over SDL2 (`src/platform`,
`src/host`); checked with `harness/native_suite.sh` against the same Wine
recordings:

| build | how |
|-------|-----|
| `build/civ2-linux32` | gcc -m32 |
| `build/OpenCiv2` (64-bit, portable to macOS and Windows) | wasm32 → wasm2c → cc |

All 36 scenarios of `harness/scripts` give pixel-identical screenshots and
the identical `rand()` trace.  The last few pixels came from Wine details
now matched: a combo box's text clipped to its field (a glyph's overhang
shows), a multiline edit's text clipped to its format rectangle and drawn
in Uniscribe items each on its own background box, LineTo's Bresenham
rounding per octant, and a list box drawing its focus rectangle at once
on losing focus (tools/probe: edititems, uspprobe, lineprobe); a window
that grows invalidates only its new strips, as Wine's server keeps the old
client pixels (menuview-arrange); a CS_OWNDC window has one DC, which
BeginPaint clips to the update region (conquest-spy: the blinking unit under
the spy dialog).  `harness/diff_suite.sh` plays a 30-turn autoplay and two
random-click scripts on both builds: identical (3 392 `rand()` calls).

Some paths read locals before assignment in the original. The reconstructed
functions use explicit captured compatibility presets, identified in comments,
to preserve the current replay behavior. `CIV2_STACKPROBE` samples the original
stack at function entry; these samples alone do not establish what a later
consumer reads or a constant valid for every caller. The
[stack compatibility policy](stack-compatibility.md) inventories the unchanged
presets and records their evidence and limits.

Matching Wine's exact message semantics also matters: window positioning,
painting and exposure, activation and focus, key state, and posted messages
before input. `harness/msgdiff.sh` and
`harness/clockdiff.py` compare a native run with the Wine reference
message by message and clock event by clock event.

## Harness notes

* Replay is deterministic (virtual clock, synchronous timers, in-process
  input, scripted file dialogs).  Two runs of the original are identical,
  which is what makes the comparison meaningful.
* `hook/compat.c` works around a crash of the *original* game under wine
  (EDIT control extra-bytes assumption) that made founding a city impossible.
* Waits in scripts are virtual milliseconds at 10 ms per idle message-loop
  iteration; world generation needs ~600 s of virtual time.

## Known limitations / next steps

* **Names and types.**  Everything the game code (src/auto, but for the
  MSVC runtime) refers to has a name: every function, global and class
  (no FUN_, DAT_ or C_ placeholders left), every parameter and local
  (no param_N, iVarN, pCVarN or local_XX: about 14,500 named by
  tools/name_vars.py from proposals per slice of functions, src/game
  included), every goto label (no LAB_ADDR), every class and struct field it
  accesses by name (tools/field_uses.py dossiers, tools/apply_field_names.py),
  and Ghidra's undefinedN types are explicit integers (i8, i16, i32, i64).
  Each rename was checked by building: the Windows build stayed byte for byte
  the same.  Evidence for names: re/function_names.tsv, re/global_names.tsv,
  re/type_names.tsv, re/field_names.tsv; names live in re/symbols.tsv and
  those maps, tools/rename.py applies them.  Layouts: the accesses by
  offset (`this->pad_48 + 0x10`, the object read as an array of cells
  `this[0x567].field_0`, and past a class's declared end) became fields at
  those offsets (tools/layout_fields.py: the pad_ gaps split, classes
  extended, ClassExt structs where a class must keep its size; every field
  checked with offsetof), named from their uses (re/layout_names.tsv).
  None is left: no field_XX, pad_ or cell access in the game code; the gaps
  no code touches stay pad_ in the headers.  The machine code is the same
  but for register choice and addressing modes in six functions (an index
  through a field now, `base + 4 + 4*i` for `base + 4*(i + 1)`).
* **No emulation of the original's memory layout** (step 5).  Locals are
  ordinary locals (tools/unframe.py), callbacks and data are referred to by
  name (no ADDR), and the static data is 4,989 ordinary variables with C
  initializers in original address order (src/data/gamedata.cpp, made by
  tools/unglobal.py; strings named from their text).  Where the code relies
  on data lying together, it is one object: Win32 structs and arrays the
  decompiler split into locals, and 115 packed structs of globals
  (`pedia_window_block.pedia_wonder_entry_count`) -- a window object and the
  fields the code also names as globals, tables split into columns.  Found
  with AddressSanitizer over every replay (build_native.py --target
  linux32asan; learned extents in re/data_extents.tsv), by checking that
  every initial value and every data pointer matches the original image, and
  by the replays: every replay runs clean under AddressSanitizer.  Code no
  replay runs may still reach from one variable into the next, so the data
  keeps the original's layout exactly: one section, in address order, each
  variable at its original alignment, the bytes nothing names kept
  (GAMEDATA() in gamedata.c; -fno-toplevel-reorder for gcc, no forward
  references in static initializers for clang).  Every build checks this
  when it starts (gamedata_check_layout(), generated by build_native.py from
  the address comments) and native_suite.sh fails a run where it differs.  A
  DIB's pixels get a few bytes of slack as on Windows (the GIF decoder
  writes one byte past the last row).
* **C++** (step 6, in progress).  The game (src/auto, src/game, src/data,
  src/native) compiles as strict C++: g++ for the linux32 builds, the wasm
  toolchain's clang++ for the portable ones; the platform layer stays C
  (extern "C" at the boundary).  The conversion was checked function by
  function: the machine code of each one is identical to that of the same
  source compiled with g++ -fpermissive (C's integer/pointer conversions;
  CIV2_CXX=permissive, tools/cxx_compare.py), which passes the suite as the C
  build did.  Many conversions are still explicit casts where headers give
  the wrong type (`iptr` for pointers, `void *` for strings); the classes,
  typed callbacks and the rest of the modernization come next.  Types and
  fields are recovered per subsystem: raw offset accesses
  (`*(T *)(p + 0x34)`) went from over 5,100 to 1,100, now struct members
  (network sessions and messages, native windows, dialog lists, scenario
  events, Civilopedia lists, ...).  These rewrites are checked by
  construction instead of by identical machine code: every member's offset
  and type are static_asserts (tools/cxx_layout.py, tools/cxx_fields.py,
  tools/cxx_members.py), uses keep their integer meaning (arithmetic stays on
  the old integer type, never pointer arithmetic), the compiler rejects any
  implicit pointer/integer conversion, and the suite runs on every batch.
  Two bugs of
  the C version surfaced and are fixed: the parley window chose its
  right-hand pane's callbacks by adding the distance between two functions in
  the original executable to a function's address (a garbage pointer in any
  other build; the eight handlers it meant are restored), and unit moves
  compared the address of human_civ, not its value, so
  unit_move_by_local_human was never set.
* **Classes** (step 6).  The decompiler's partial views of each class
  (structs sized by how far one function reads) are folded into one struct
  per real class, with C++ inheritance for bases: Dialog (ScenarioEvents
  on it); the controls on Control (Button, Checkbox, Scrollbar,
  EditControl, ComboBox, ListBox, Hotspot, Hyperlink, RadioGroup,
  SpriteButton); the windows on WndBase -> ModalMenuWindow (Wnd, the city
  view, throne room, spaceship, diplomacy and movie windows); the surfaces
  on DibSurface -> SurfaceWindow -> FrameWindow (the map, city, editor,
  picture, Civilopedia, council, parley windows ...).  Sizes are the
  constructors' allocations.  Each fold was checked by layout (no member
  moved) and access by access (every member access compared function by
  function before and after), plus the suite.  Methods are still free
  functions taking the object; member functions and typed callbacks are
  next.
* **Network play** (multiplayer over TCP/IP).  The original's networking
  library, XDaemon.dll, is replaced by src/platform/xdaemon.c over sockets
  in the host layer (src/host/host_net.c), speaking its wire protocol
  (docs/xdaemon-protocol.md: TCP 4993 records, UDP 4994 LAN discovery, the
  id handshake, host relaying).  `tools/xdnet/nettest.py` plays two (or
  three) games against each other in containers and checks the logs
  (docs/xdaemon-protocol.md §11.1).  Passing, our side under ASan: ours vs
  ours, ours vs the original and the original vs ours, on a LAN and in
  Internet mode (127.0.0.1): join, several turns (a unit moved, cities
  founded), quit; ours vs ours: chat, the parley (audience, peace treaty
  offered and accepted); against the original: chat to it, its audience
  granted, a treaty offered to it; save, then load ("Load a Saved
  Multiplayer Game") with the original joining; a player leaving and
  joining the game in progress; three players with either one hosting
  (the host relaying between an original and ours).  The game's network
  code needed: net_session_init (the join dialog) written; locals the
  original's frame kept adjacent (net_send_message's buffers,
  net_process_messages' arguments, four RLE streams, the parley's radio
  labels: garbage treaty names) made structs or arrays; a game-list text
  buffer 21 bytes instead of 256; the difference engine's RLE decoder
  reading a run's byte as a 16-bit word and its encoder reading past its
  input; a size from the original's absolute address; a 5-entry read of a
  4-entry label array.  In normal play WM_TIMER is Windows' (one pending
  per timer, no backlog: the unit blink, which retrieves messages while it
  redraws, nested without end in a slow build) and presenting the screen
  takes at most a third of the time.  Not done: the original's parley
  window driven by the test (its 640x480 layout), barter, a game of many
  turns.
* Coverage of the comparison: the scripts execute 2,520 of the 3,529
  compiled game functions (71%, 58% of lines; `tools/coverage_report.py`
  with the linux32cov build, per function in re/coverage.tsv).  Never
  executed: multiplayer (parley, net, lobby), sound and video (the
  reference has neither an audio device nor the Indeo codec), council,
  atexit destructors.  Untested paths may still differ.  Add a script under `harness/scripts/`, record it with
  `make ref SCRIPT=...` and compare with `make native-suite`.
