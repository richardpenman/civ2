# Determinism and arithmetic

**ID:** F01. **Status:** Written. **Target:** Civilization II Multiplayer Gold
Edition **5.4.0f, patch 3**, under the [X01 contract](classic-compatibility-contract.md).

This specification defines numeric operations, the two shared random streams,
seed changes, clock-derived inputs and the information required for deterministic
execution. Individual algorithms own their precise call order. [W02](map-generation.md)
uses these shared definitions; its `R` and `S` correspond to `primaryNext` and
`S` below.

Evidence is pinned to OpenCiv2
`ce18e5e52e19870cf1598651eee7c1a97d517519` and the independent reconstruction in
`/home/peter/project/civ2` at
`4cfdd2a4b43e1a99983cc6af3e993a52b8ad8752`. Original entry addresses and validation
limits appear at the end. These procedures do not require the original memory
layout or source-language types.

## 1. Numeric model and notation

Use the following explicit numeric types. An unsuffixed scalar in the algorithms
is a signed 32-bit integer unless its owner specifies another type. Coordinates,
record fields, bitmasks and constants must retain their individually declared
width and signedness; do not infer them from this default.

| Type | Meaning |
| --- | --- |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers of exactly 8, 16, 32 or 64 bits. |
| `i8`, `i16`, `i32`, `i64` | Two's-complement signed integers of those widths. |
| `binary32`, `binary64` | IEEE binary floating formats with 24- and 53-bit significands respectively, including the implicit leading bit. |

For mathematical integer `x`, define:

```text
uN(x) = x modulo 2^N, in [0, 2^N - 1]
iN(x) = uN(x)                    if uN(x) < 2^(N-1)
        uN(x) - 2^N             otherwise
```

These conversions truncate bits, without saturation. For example,
`u8(260)=4`, `i8(255)=-1`, `i16(32768)=-32768` and
`i32(0xffffffff)=-1`. Widening a signed value preserves its mathematical value;
widening an unsigned value zero-extends it. Thus reading the same byte as `i8`
and as `u8` can produce different arithmetic and comparisons.

Original scalar `char`, `short`, `int`, `long` and pointers occupy 8, 16, 32, 32
and 32 bits respectively, with plain `char` signed in this build. A modern
64-bit host's `long`, pointer or native integer size must not redefine game
arithmetic. Multi-byte memory/file values are little-endian where the relevant
record format exposes their bytes. Record layouts and pointer replacements
belong to F02/F06 rather than this specification.

### Integer operations

- A recovered 32-bit add, subtract or low-word multiply keeps its low 32 bits;
  reinterpret the result as signed where the next operation requires it. Preserve
  the width at each intermediate step, not just at the final assignment.
- Narrow stores retain the low field-width bits. Subsequent reads sign-extend or
  zero-extend according to that field's type. Byte arithmetic is often performed
  after integer promotion; it is not necessarily an 8-bit operation at every step.
- Signed division truncates toward zero: `sdiv(-7,3)=-2`. Signed remainder is
  `a - sdiv(a,b)*b`, so `srem(-7,3)=-1`. This differs from floor division and
  from a language's always-nonnegative modulo operator.
- Unsigned division uses nonnegative values and rounds down. Unsigned remainder
  is nonnegative. Do not exchange signed and unsigned comparisons or divisions.
- Signed arithmetic right shift fills with the sign bit: `asr32(-7,1)=-4`.
  Unsigned right shift fills with zero. Left shift discards high bits.
- An x86 32-bit variable shift uses the low five count bits, hence shifting by
  32 uses count zero. This is the rule for the helper below; an owning algorithm
  must identify any operation with a different width.
- Do not reassociate expressions, move a cast across an operation, substitute
  shift for signed division on negative values, or replace integer arithmetic
  with floating arithmetic without proving the same intermediate results.

Signed division by zero and `INT_MIN / -1` do not have ordinary 32-bit results
on the original x86 instruction; they fault. A caller must exclude these inputs
or specify its failure behavior. Do not turn them into zero, saturation or wrap
as a supposed classic result. Original carry-dependent, multiword or exceptional
operations need their own explicit contract; this is not permission to assume
every decompiled C expression defines portable overflow behavior.

### Shared clamp and shift helpers

All arguments here are `i32`; comparisons are signed. The two clamp forms agree
for `lo <= hi`, but **do not swap reversed bounds**:

```text
clampAB(value, lo, hi):            # int_clamp and int_clamp_b
    return min(max(value, lo), hi)

clampC(value, lo, hi):             # int_clamp_c
    if value > hi: return hi
    return max(value, lo)

shiftSigned(value, shift):
    if shift == 0: return value
    if shift > 0:
        return i32(u32(value) << (u32(shift) & 31))
    count = u32(-mathematical_integer(shift)) & 31
    return asr32(value, count)
```

For reversed bounds `(lo,hi)=(10,0)`, `clampAB(-5,10,0)=0`, while
`clampC(-5,10,0)=10`; both return zero for `value=5`.
`shiftSigned(-7,-1)=-4`, `shiftSigned(0x40000000,1)=-2147483648`, and
`shiftSigned(-7,-32)=-7`. The count calculation also handles `shift=INT_MIN`
without a host-language signed negation overflow.

### Floating point

The original startup selects **53-bit x87 precision**. For the finite,
ordinary-magnitude computations below, use round-to-nearest, ties-to-even,
with a separately rounded result for each floating operation. A stored `double`
is binary64. Do not use binary32, fused multiply-add, decimal arithmetic or
fast-math reassociation in the random-range procedure.

An x87 register still has the extended exponent range when its precision control
selects a 53-bit significand. Therefore “use double” is sufficient for the bounded
random calculations below, not a proof of equivalence for every possible
floating expression elsewhere in the game. An owner involving underflow,
overflow, special values, library math or a changed FPU control word must specify
those details separately. If a test deliberately changes the control word,
capture it; the original random-range operations use the incoming precision and
rounding, except that the final integer conversion temporarily selects truncation.

Floating-to-integer conversion in the range helper truncates toward zero. The
original conversion produces a signed 64-bit integer, then uses its low 32 bits.
It restores the prior rounding control. All products possible in the range
procedure below fit that conversion domain.

## 2. State and primitive boundaries

| State | Width | Fresh executable value | Meaning |
| --- | --- | --- | --- |
| `S` | `u32` | `0x00000001` | Primary Microsoft CRT random state. |
| `N` | `u32` | `0x00bc6840` | Secondary Numerical Recipes linear-congruential state. |
| `P` | `u16`, ordinarily `1..32767` when newly generated | Supplied with map state | Resource/shield/hut pattern input; not a third random stream. |

Fresh executable values precede startup seeding. They are not defaults to insert
when loading a fixture or starting another game in an existing process. A
fixture entering after startup must supply the actual states at its boundary.
F06 owns whether and how particular save/load operations restore or retain them;
do not assume that loading a save restores all process-level determinism inputs.

Both generators are shared mutable state. A primitive is described as one
sequential call with no intervening game callback; synchronization and callback
placement around calls are the caller's responsibility. No primary operation
implicitly advances `N`, and no secondary operation implicitly advances `S`.
Setting `P` itself advances neither; later operations can use `P` to reseed `N`.

The source comment describing the secondary generator as cosmetic is incomplete:
its range helper is also called by AI, city production and diplomacy. Keeping a
separate generator for each subsystem would change classic outcomes.

## 3. Primary generator

```text
seedPrimary(seed):                # seed is u32; no random draw
    S = seed

primaryNext():
    S = u32(214013 * S + 2531011)
    return (S >> 16) & 32767      # i32 in [0,32767], after updating S
```

`seedPrimary(0)` really sets zero. Seeding replaces state without advancing it
and does not reset the secondary generator. The observable result of a draw
includes its returned 15-bit value and the entire updated 32-bit state; the
returned value alone cannot be used to reconstruct the next state.

Many callers use `primaryNext() % n` for a positive `n`. Preserve the modulo
bias and the fact that this consumes a draw even for `n=1`. It is not rejection
sampling. Callers must specify their handling of zero or negative bounds.

W02's [map-local `pick` helper](map-generation.md#random-draws) has a guard that
can skip the draw. Use that helper only where W02 calls it. Do not introduce its
guard into an unconditional primary draw or the secondary inclusive range.

### Primary vectors

Starting from `S=1`, successive calls return:

| Call | Return | State after call (hex) |
| --- | --- | --- |
| 1 | 41 | `0029e2c0` |
| 2 | 18467 | `c823f683` |
| 3 | 6334 | `18be873a` |
| 4 | 26500 | `e7847115` |
| 5 | 19169 | `cae1df84` |
| 6 | 15724 | `3d6c1037` |
| 7 | 11478 | `2cd60a1e` |
| 8 | 29358 | `72ae7869` |
| 9 | 26962 | `69521988` |
| 10 | 24464 | `df90722b` |

Starting from `S=1`, W02's `pick(1)` returns zero and leaves `S=1`.
Starting separately from `S=1`, `primaryNext()%1` returns zero and leaves
`S=0x0029e2c0`. Identical return values do not establish identical execution.

## 4. Secondary generator and inclusive range

Define `round53(x)` as nearest-even rounding to a 53-bit significand. All values
in this section are in binary64's normal range or exactly zero, so binary64
arithmetic at the stated boundaries implements it. `trunc(x)` is mathematical
truncation toward zero. Multiplication/addition in the state update are integer
operations, with the final low 32 bits retained.

```text
exchangeSecondarySeed(seed):     # input is a 32-bit bit pattern
    if u32(seed) == 0:
        return 0                 # N is unchanged; no draw
    previous = N
    N = u32(seed)
    return i32(previous)

secondaryUnit():
    N = u32(1664525 * N + 1013904223)
    return round53(integer_value(N) / 4294967295)
```

The numerator is **unsigned**, even when bit 31 is set. The denominator is
`2^32-1`, not `2^32`. The floating result is in the closed interval `[0,1]`;
both endpoints are possible. A zero seed request is a no-op, whereas an actual
zero state can occur through the recurrence or be explicitly supplied in a
boundary fixture. A fixture with `N=0` must not attempt to establish that state
by calling `exchangeSecondarySeed(0)`.

The inclusive integer helper takes signed 32-bit endpoints in either order:

```text
secondaryRange(lo, hi):
    if lo == hi:
        discard secondaryUnit()             # exactly one draw even here
        return lo
    if hi < lo: swap lo, hi                  # signed comparison

    difference = i32(u32(hi) - u32(lo))
    adjustment = binary64_bits(0x3feffffde7210be9)
    span = round53(integer_value(difference) + adjustment)
    # span is stored as binary64 before the draw
    fraction = secondaryUnit()
    product = round53(fraction * span)
    integral = trunc(product)               # signed 64-bit result
    return i32(u32(integral) + u32(lo))
```

`adjustment` is the executable's binary64 encoding of decimal `0.999999`
(`0x1.ffffde7210be9p-1` in hexadecimal floating notation). Preserve this exact
constant and the addition/division/multiplication rounding points. The normal
result lies in `[min(lo,hi),max(lo,hi)]` when their mathematical difference is
at most `2147483647`. It is not a modulo reduction and is not an exactly uniform
integer sampler. The adjustment and rounded multiplication are part of the
behavior, including at the endpoints.

For a wider signed-endpoint interval, the difference wraps to a negative `i32`.
The result then need not lie between the requested endpoints. This is original
operation behavior, not an instruction to widen, clamp or reject the difference.
The pinned OpenCiv2 C++ uses `(hi-lo)` directly, which does not explicitly define
portable signed overflow. The independent reconstruction preserves the original
wrapping subtraction, binary64 span store, x87 quotient/multiply and truncating
conversion. A clean implementation should use the explicit operations above.

Every call consumes exactly one secondary draw, including equal bounds. Swapping
endpoints and seed exchange consume none. There is no loop, rejection or special
case that skips the draw for a one-value range.

### Secondary vectors

Each row is an independent call with the given **entry state**, under the normal
53-bit, nearest-even environment:

| Entry `N` (hex) | `lo` | `hi` | Return | Exit `N` (hex) |
| --- | --- | --- | --- | --- |
| `00000001` | 0 | 2 | 0 | `3c88596c` |
| `00000001` | 2 | 0 | 0 | `3c88596c` |
| `00000001` | 5 | 5 | 5 | `3c88596c` |
| `00000001` | -4 | 7 | -2 | `3c88596c` |
| `00bc6840` | 0 | 100 | 52 | `85c9be9f` |
| `00000000` | -4 | 7 | -2 | `3c6ef35f` |
| `ffffffff` | 0 | 100 | 23 | `3c558d52` |
| `25d60fe5` | -4 | 7 | -4 | `00000000` |
| `26f5b720` | -4 | 7 | 7 | `ffffffff` |
| `00000001` | -2147483648 | 2147483647 | -2147483648 | `3c88596c` |
| `00000001` | -2147483648 | 1 | 1639699275 | `3c88596c` |

The last row intentionally returns outside the supplied interval. The two rows
ending in zero and `ffffffff` also check `secondaryUnit()` results of exactly
zero and one. Separately, starting at `N=0x00bc6840`, exchanging seed zero
returns zero and leaves the state unchanged; exchanging seed one then returns
`0x00bc6840` and leaves `N=1`.

## 5. Seeding, process history and the map pattern

The recovered startup sequence includes these distinct actions:

1. System initialization seeds `S` with the low 32 bits of the wall-clock
   seconds result returned by `_time`.
2. Later application startup requests a 4 ms timer period, calls the game's
   tick helper, and seeds `S` from that result.
3. It makes a **second** tick-helper call and passes that result to
   `exchangeSecondarySeed`, ignoring the returned previous state. A zero result
   leaves the existing `N` in place; it does not replace it with zero.

Do not merge the clock reads or assume the first seed was never observed. A
boundary after startup can capture the resulting states directly; a process
replay must preserve the earlier sequence and any intervening work.

City-view building layout subsequently calls:

```text
exchangeSecondarySeed(u32(city.x * 256 + city.y + P))
```

It does not save and restore the previous state. Its layout then consumes
secondary ranges, including deliberate `secondaryRange(0,0)` draws. Thus viewing
a city can affect the secondary state seen by later AI/diplomacy. P06 owns the
layout's full draw order; it must not be replaced by an isolated visual RNG.
This is also why a game seed plus a list of turn-ending commands is insufficient
to reproduce every decision.

### Game ticks

Let `milliseconds` be the `u32` result of a single `GetTickCount` call. The game
tick helper computes:

```text
gameTicks():
    milliseconds = external GetTickCount()
    return u32(6 * milliseconds) / 100       # unsigned integer division
```

The multiplication wraps **before** division. This is not unbounded
`floor(milliseconds*0.06)`. Its output is in `0..42949672`, so the subsequent
signed return wrapper preserves its value. Do not replace existing busy-wait
comparisons with wrap-aware elapsed-time formulas unless their owner specifies
that behavior; the original helper's discontinuities are observable inputs.

Creating a new map-pattern seed is a separate operation:

```text
mapPatternSeedFromTicks(ticks):
    result = ticks & 32767
    if result == 0: result = 1
    return result

newMapPatternSeed():
    P = mapPatternSeedFromTicks(gameTicks())
```

There is one tick query and no random draw. For W02, its input named `ticks` is
this **already converted game-tick value**, not raw milliseconds. A retained map
keeps its existing `P` unless a caller explicitly requests reseeding; the world
loader/editor's reseeding decisions belong to F05/S03. Coordinate predicates
using `P` belong to W01 and remain embedded in W02 for now.

| Raw milliseconds | Game ticks | New `P` |
| --- | --- | --- |
| 0 | 0 | 1 |
| 16 | 0 | 1 |
| 17 | 1 | 1 |
| 1000 | 60 | 60 |
| 715827882 | 42949672 | 23592 |
| 715827883 | 0 | 1 |
| 4294967295 | 42949672 | 23592 |

The first multiplication overflow occurs after about 8.28 days of the raw
millisecond count, before that counter itself wraps at 32 bits.

Given already converted game ticks `0,32768,32769,65535`,
`mapPatternSeedFromTicks` returns `1,1,1,32767`, respectively.

## 6. Deterministic execution and replay inputs

A deterministic boundary requires all state that the operation reads, both
stream states if reachable, every seed mutation, and ordered external inputs.
Provide the map seed separately where read. Include rules, caches, prior-session
state, callback behavior and any original backing bytes required by X01's
stale-state cases. Neither a save file nor a single `seed` field proves these
inputs are complete.

For every random consumer, the owning algorithm must specify:

1. Which primitive is called, with its arguments and signedness.
2. Its order relative to scans, mutations, callbacks and other random calls.
3. Whether rejected candidates, equal bounds, failed actions or cancelled
   dialogs still draw, and when retry loops stop.
4. What reseeds the stream, and whether any prior state is restored.

Preserve the original order explicitly when a source expression contains more
than one side-effecting operand; host-language evaluation order is not a
substitute for recovered execution order. Do not add draws to pick among tied
scores when a deterministic scan selects the first or last one.

A useful trace record contains stream ID, primitive/arguments, entry and exit
state, return value, caller/phase, and ordered clock/seed events. Compare state
at phase boundaries as well as draw values. For a subsystem that promises no
random effects, verify both streams remain unchanged.

### Existing harness conventions

These conventions explain existing reference recordings. They are environment
inputs for a replay, not the game's native clock algorithm:

| Mechanism | Pinned harness behavior |
| --- | --- |
| `CIV2_SEED` | A nonempty value parsed with base autodetection replaces the seed on **every** primary `srand` call. It does not directly set `N` or `P`. Record the effective `u32` value; avoid relying on host parsing of out-of-range text. |
| Virtual clock | Starts at zero elapsed microseconds with a millisecond baseline of `1000000`. Game `GetTickCount`/`timeGetTime` queries advance elapsed time by 50 microseconds, then return `u32(1000000 + floor(elapsed_us/1000))`. These queries do not dispatch timers in virtual mode. |
| Idle and sleep | A virtual idle step adds 10000 microseconds, dispatches due timers, then runs due script actions. `Sleep(ms)` adds `ms*1000` microseconds and dispatches due timers. |
| Keyboard polling | During replay, `GetAsyncKeyState` adds 50 microseconds, dispatches due timers and runs due script actions before returning key state. It is a scheduling point, unlike a virtual tick query. |
| Wall-clock seconds | During replay, `plat_time` returns `922449600 + floor(elapsed_us/1000000)` without advancing time. This is an epoch-seconds input, not a promise that every calendar API implements an unrestricted calendar. |
| Real-time option | A nonempty `CIV2_REALTIME` disables the virtual tick/idle clock path. Do not claim deterministic scheduling from the script alone in this mode. |
| `CIV2_RAND_LOG` | Captures primary seeding and primary returned values with caller identity. It does **not** trace the secondary stream. |
| `CIV2_CALL_LOG` | Records selected sound and video requests during replay. It does not establish complete media decoding or completion equivalence. |

The full event-loop/timer/input ordering belongs to P05 and X02. Reimplementing
only the clock rows above is insufficient to reproduce arbitrary scripts; either
supply an ordered event/clock transcript or use the pinned harness with its
message queue, timers and script interpreter. Declare which model a fixture uses.

For example, with zero initial virtual elapsed time and no other activity, the
first tick query returns `1000000` milliseconds and `gameTicks()` returns
`60000`. After twenty 50-microsecond queries the millisecond return is `1000001`;
the game-tick result is still `60000`. These query counts are part of the input
history even though many consecutive returns are equal.

## 7. Source traceability and validation

Original addresses below identify routines/data in the target executable; an
implementer does not need to use those addresses.

| Contract | Evidence at the pinned revisions |
| --- | --- |
| Widths and floating precision | [Type definitions](../../tools/ghidra.h), [32-bit long compatibility](../../src/include/auto_common.h); `__setdefaultprecision` at `0x005fe240` in [decompilation](../../re/decomp.c), setting precision through `__controlfp(0x10000,0x30000)`. |
| Clamp/shift variants | [support.cpp](../../src/auto/support.cpp): `int_clamp_c` `0x00511350`, `int_clamp_b` `0x005a1a44`, `int_clamp` `0x005adfa0`, `int_shift_signed` `0x005ae3ec`. |
| Primary RNG | [crt.cpp](../../src/native/crt.cpp): original `srand` `0x005f2260`, `rand` `0x005f2280`; state at `0x00639e50`. [Determinism hook](../../hook/determinism.c) records these original calls. |
| Secondary RNG | [support.cpp](../../src/auto/support.cpp), [random.cpp](../../src/game/random.cpp): seed exchange `0x0059a6f0`, unit draw `0x0059a733`, range `0x0059a791`; state at `0x00635094`. |
| Initial states and adjustment | [Executable image layout](../../data/image/layout.json), [.data](../../data/image/data.bin), [.rdata](../../data/image/rdata.bin), [state declarations](../../src/data/gamedata.cpp); binary64 adjustment at `0x0061d700`. |
| Startup and ticks | [game.cpp](../../src/auto/game.cpp): system initialization `0x005dbbd6`, application startup `0x0041b4c0`, tick wrapper `0x00421bb0`, tick calculation `0x005d41e0`. |
| Map-pattern seed | [map.cpp](../../src/auto/map.cpp): `map_new_seed` `0x005b85fe`; `MapHeader.seed` in [civ2types.h](../../src/include/civ2types.h). |
| City-view reseed | [city.cpp](../../src/auto/city.cpp): `CityViewWindow::layout_buildings` `0x00455314`. Secondary gameplay callers also occur in [ai.cpp](../../src/auto/ai.cpp), [civ.cpp](../../src/auto/civ.cpp) and [diplomacy.cpp](../../src/auto/diplomacy.cpp). |
| Replay environment | [replay.c](../../src/platform/replay.c), [message.c](../../src/platform/message.c), [original replay hook](../../hook/replay.c), [comparison script](../../harness/compare.py). |

Supplementary evidence under `/home/peter/project/civ2` is
`src/random_range.cpp` (explicit wrapped difference and x87 operations),
`src/runtime_startup.cpp` (53-bit precision), `src/x86_arithmetic.hpp` (signed
division instruction semantics), and `tests/random_range.py` with its probe and
original-code oracle. The latter defines 72 seed/endpoint edge combinations and
512 generated combinations, comparing returned integer and final secondary
state. It is a useful independent conformance source; it was **not rerun against
the original executable** for this documentation change.

For this document, 39 helper/vector checks compared the documented results with
extracted OpenCiv2 helpers compiled with explicit wrapping arithmetic. Primary
and secondary vector tables were also independently recomputed. The initial seed
words and adjustment bits were checked directly in the stored executable data sections.
That validates the tables and source transcription, not complete original-game
conformance. Existing [unit-case inventory](../../tests/unit/cases.tsv) includes
the secondary routines, but a zero primary-random-call count there does not mean
the secondary routine consumed no randomness.

Remaining integration work belongs to X02 and consuming algorithms: complete
capture of both streams, callback/draw ordering, save/load and second-session
state, city-view-to-AI interactions, and mode-specific scheduling. This document
does not establish all those call sequences simply by defining their primitives.
