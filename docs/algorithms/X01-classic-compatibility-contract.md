# Classic compatibility contract

**ID:** X01. **Status:** Written. **Target:** Civilization II Multiplayer Gold
Edition **5.4.0f, patch 3**, the 32-bit Windows game.

This contract defines what the [algorithm specifications](README.md) mean by
classic behavior. An implementation should be possible using the specifications
and their declared inputs, without consulting the reconstructed source.

The inspected OpenCiv2 revision is
`ce18e5e52e19870cf1598651eee7c1a97d517519`. Supplementary evidence comes from
`/home/peter/project/civ2` at
`4cfdd2a4b43e1a99983cc6af3e993a52b8ad8752`. These are evidence revisions, not
alternative game versions. Earlier specifications retain their own explicitly
pinned contracts until reviewed; in particular, [W02](W02-map-generation.md) pins
OpenCiv2 `1ff87e33fbec8b7be8c4bf98c77709e16aad4f31`.

## 1. The behavior being reproduced

For a declared operation, supply the same entry state and ordered external
inputs, execute to the same boundary, and compare the specified observations.
Equivalence includes intermediate effects that can influence another operation,
even if the final visible screen happens to match.

| Observation | Required agreement |
| --- | --- |
| Simulation state | Exact field values, flags, IDs, ownership, list/stack order, counters, caches that affect later decisions, and mutations of shared scratch state where specified. |
| Decisions | Exact eligibility, scores, tie breaks, scan order, branches, and selected outcomes. |
| Randomness | Stream selected, seed changes, draw order/count, discarded draws, returned values, and full final states. F01 defines the primitives. |
| Interaction | Accepted commands, rejected commands, cancellation, modal transitions, active player/unit, visibility, notifications, and their order. |
| Persistence | Specified save/map/scenario bytes and their load effects. Equality of a few displayed statistics is insufficient. F06 will identify padding or fields that may be excluded. |
| Multiplayer | Authority, ordered messages, payload/state differences, turn barriers, and synchronization effects at the declared protocol boundary. |
| Presentation | Specified text, palette/pixels/layout, selection and sequencing of sounds/videos, and their completion or cancellation effects, given matching assets and display inputs. |
| Failure | Defined return values, partial mutations, notifications and cleanup for a documented failure. A successful-path case establishes nothing about failure handling. |

Do not infer freedom to reorder work from matching return values. A redraw can
consume random state; a message pump can dispatch a callback; a failed attempt
can consume a draw or change a cache. Each owning document must state these
effects or constrain its boundary so they cannot occur. F01 gives the shared
[determinism contract](F01-determinism-and-arithmetic.md).

No universal numeric tolerance applies. Integer state and file/protocol fields
are exact. Floating operations follow the specified precision and rounding;
any permitted tolerance for a particular output must be justified locally.
Pixel equivalence requires the same assets, fonts, palette and viewport. Media
request equivalence alone does not establish decoded audio/video equivalence.

## 2. Inputs and supported domains

A case must declare the game mode, difficulty, active and human civilizations,
government, topology, rule tables/options, scenario settings, and relevant
session history. Only list an input as irrelevant when the scoped operation
cannot read it or be affected by it through a callback.

Original rule and scenario customization is part of classic behavior. Optional
OpenCiv2 additions are outside this contract. Each specification must give the
accepted custom-input domain; defaults do not establish the behavior of all
custom tables. Distinguish caller validation from a helper's actual behavior.

| Input or mode | Disposition |
| --- | --- |
| Single player; new, custom and premade worlds | In scope, including setup and a second game in the same process. New-game setup does not imply every process global resets. |
| Saved games, scenarios and classic editors | In scope for the formats and domains defined by F03/F06/S01–S04, including load-time mode adaptation. |
| Hotseat | In scope. It is a local multiplayer mode, with its own concealment, active-player and handoff behavior; do not treat it as a network game. |
| Network and Internet | In scope. N01–N05 must state transport and synchronization boundaries and distinguish observations from untested paths. |
| Dial-up, serial, IPX and DirectPlay-related code | Original provider-dependent paths require a disposition in N05. Their absence from the current reconstructed transport and replay coverage is an implementation/evidence gap, not proof they were absent from the original. |
| Play by e-mail (PBEM) | Not a reachable playable mode in this MGE release. Do not add it to the classic feature set merely because mode-2 branches remain in the executable. |
| Other Civilization II releases and unofficial patches | Separate targets; do not merge their rules or fixes into MGE 5.4.0f patch 3 without an explicit variant contract. |
| Malformed files/packets, allocation failure, missing assets and I/O failure | Include only the failure domains actually specified. Successful ordinary-play equivalence is not a claim to reproduce arbitrary corruption or host failure. |

The PBEM exclusion is specific: the original Multiplayer Game menu offers Hot
Seat, Network, Internet and Dial-Up. The recovered PBEM setup entry at
`0x005ae580` has only an unused incremental-link thunk as a reference. Loading
an `.eml` through normal Load a Game clears multiplayer mode and proceeds as
single player. That reachable load behavior belongs to F06; the unreachable
email prompts do not become a required session mode.

State at an operation boundary is often a better input than a complete process
history. For example, W02 accepts resolved world parameters, the primary RNG
state at generator entry, and a clock-derived resource seed input. A test that
starts at the main menu must additionally reproduce the earlier setup draws,
clock queries, UI actions and callbacks. These are different test boundaries.

## 3. Original behavior and reconstruction choices

Use the following evidence categories explicitly:

1. **Original operation:** recovered instructions/data establish the operation,
   widths, branch conditions or constants. Record the address and executable
   identity; decompiler types and variable names alone are not definitive.
2. **Original observation:** a recorded execution establishes an outcome for
   its captured inputs, environment and history. Record that bounded domain.
3. **Reconstruction agreement:** source and/or tests from OpenCiv2 and the
   independent reconstruction agree. This supports a contract but does not
   replace missing original evidence.
4. **Assumption or deliberate deviation:** a chosen value, missing platform
   behavior, safety repair or unresolved interpretation. State it at the affected
   procedure and describe the observable consequence or unknown.

When evidence conflicts, first compare builds, modes, entry state and external
inputs. Prefer original instructions for a recovered primitive and controlled
original observations for its exercised behavior. If they still disagree,
record an unresolved contract instead of silently choosing a convenient result.
An observed stale stack value is evidence of that execution, not a universal
constant. Lack of a replay mismatch does not establish an unexercised branch.

### Classic switches in OpenCiv2

At the pinned revision, a **nonempty** `CIV2_CLASSIC` disables selectable bug
fixes and extras; the string `0` is also nonempty. The usual invocation is
`CIV2_CLASSIC=1`. Unit tests disable both. Replays disable both unless their
respective test override is enabled, and an explicit classic setting still
takes precedence over those overrides.

The precise conditions, using `present(x)` to mean a nonempty environment value,
are:

```text
classic = present(CIV2_CLASSIC)
replay = not unitTest and present(CIV2_SCRIPT)
fixes = not classic and not unitTest
        and (not replay or present(CIV2_TEST_FIXES))
extras = not classic and not unitTest
         and (not replay or present(CIV2_TEST_EXTRAS))
UI_fix_enabled = fixes
state_fix_enabled = fixes and game.multiplayer < 3
```

Consequently, disabling network state fixes does not by itself select the whole
classic presentation. Nor does selecting classic undo repairs present in every
mode. Capture the effective switches in each fixture rather than relying on a
label such as “default run.”

### Known differences that later documents must account for

[BUGS.md](../BUGS.md) is the detailed evidence ledger. The following routing
table identifies important exceptions to using current classic source as an
unqualified original oracle. It is not a substitute for auditing each family's
other selectable fixes and stale-local presets.

| Evidence | Difference or unresolved input | Owning specifications |
| --- | --- | --- |
| B1 | Commodity road count starts at zero in every OpenCiv2 mode. The original starts from a stale local; zero was common in Wine recordings, but other values affected demand. | C07; F02 for any explicitly captured backing state. |
| B3, B13, B19 and other preset locals | Classic uses recorded/preset values for otherwise uninitialized reads. Do not generalize those values to all original histories or hosts. | A08 for B3's attitude change; A05 for B13/B19's transport choices; F02 for entry-state requirements. |
| B21 | Wonder scoring bounds an index that originally could read adjacent locals. This can change AI production scores. | A02. |
| B22 | Ordinary hotseat movement retains a reconstructed layout of the original adjacent locals, but paradrop visibility with no active human uses an always-on repair. | U02/U05, N04, P02. |
| B26 | Cancelling a custom portrait preserves the existing portrait, replacing an original stale result. | F05, P04. |
| B27 | Disband with no active unit returns, replacing a possible stale-coordinate city-disband prompt in cheat mode. | U04, S03, P05. |
| B29–B31 | Council speech clearing, famine redraw location and production-waste drawing use initialized values instead of stale locals. | P09; C05/P06; P06, respectively. |
| B8 and memory-error ledger | Event string lifetimes, buffer bounds, file/packet validation and similar repairs are always on. Equivalent successful output does not establish equivalent corrupt-input or failure behavior. | S02/S03, F06, N02/N05, and the relevant presentation/file owner. |
| F01 range arithmetic | Original endpoint subtraction wraps at 32 bits. The hand-written C++ expression does not make signed-overflow semantics portable for extreme endpoints. | F01; any consumer accepting those endpoints. |

For a stale or out-of-bounds original read, a specification may describe exact
behavior conditional on explicitly supplied backing bytes. Alternatively, it may
limit its equivalence claim to recorded cases, or specify a deliberate safe
replacement. It must say which. Do not require arbitrary memory corruption as a
portable implementation technique or claim a replacement is exact original
behavior. A snapshot of valid game records need not contain those extra inputs.

Deterministic quirks with recovered inputs remain part of the target. For
example, a second game in the same classic process can retain the human tribe's
city-name position (B2). “Start a new game” is not permission to reset all state.

## 4. Platform boundary

An implementation may use different storage layouts, classes, rendering APIs,
allocators, networking libraries and codecs if the specified observations agree.
Original addresses identify evidence; they are not required runtime addresses.
Little-endian file fields, integer widths, ordering, timing inputs, and aliases
that affect behavior must still be represented explicitly.

Model platform calls by their inputs, results and side effects. At minimum,
specify relevant clock returns, message/timer callbacks, file contents and
errors, input events, transport deliveries, and media completion. Record ordering
where callbacks can interleave with an algorithm. Do not infer real-time
scheduling from an ordinary sequential procedure.

The deterministic Wine/OpenCiv2 replay harness is a **test environment**. Its
virtual clock and injected seed are not intrinsic rules of Civilization II.
F01 describes their effect on random and arithmetic inputs. P05 and N01–N05
will own the remaining event and transport scheduling contracts. Runtime
packaging, compiler scaffolding and third-party codec internals are outside the
algorithm collection; their externally visible effects are not automatically
outside it.

## 5. Evidence required for a compatibility claim

Every case accompanying a specification must identify:

1. **Target and evidence:** game version, original binary hash when used, source
   revisions, fixture revision, and how expected results were obtained.
2. **Boundary and domain:** called operation, start/stop points, applicable modes
   and input limits, plus any excluded callbacks or failures.
3. **Entry state:** all read state and relevant backing bytes; both RNG states,
   resource-pattern seed where applicable, process/session versus new-game
   initialization, rules and asset identities.
4. **External inputs:** ordered commands, clock returns or named clock model,
   file/packet inputs, callback schedule, environmental options and failures.
5. **Expected observations:** return value, mutations, ordered effects, random
   draws/final states and appropriate serialized or presentation output.
6. **Comparison and limits:** normalization/masks with reasons, what was actually
   compared, disagreements, assumptions and unexercised branches.

These are required information categories, not a completed fixture schema.
X02 still owns the machine-readable fixture format, executable case collection,
coverage matrix and whole-game integration tests.

For example, a W02 boundary case can provide resolved dimensions/parameters,
entry primary state, the one resource-seed clock value, terrain rules, required
map/cache state, and a no-mutating-callback condition. It can then compare tile
bytes, derived metadata and final primary state. It does not establish setup,
starting positions, an AI turn or network generation equivalence. A main-menu
replay has a wider boundary and correspondingly more inputs.

**Written** means the scoped behavior and its limits have been described.
**Checked** additionally requires review and independent conformance cases with
their results recorded. Neither label implies complete game equivalence. A
document with unresolved required steps should be **Draft**, or explicitly
narrow its supported domain; missing behavior must not be hidden by a status.

## 6. Traceability and validation limits

| Evidence at the pinned OpenCiv2 revision | Use here |
| --- | --- |
| [Classic and test configuration](../../src/platform/main.c), [fix predicates](../../src/include/auto_common.h) | Exact mode-switch conditions. |
| [Original bug ledger](../BUGS.md), [extras](../EXTRAS.md) | Selectable changes, always-on repairs, stale-state assumptions and optional behavior. |
| [Status and coverage](../STATUS.md) | PBEM reachability, provider gaps and finite replay coverage. |
| [Replay comparison](../../harness/compare.py), [determinism hook](../../hook/determinism.c), [native replay](../../src/platform/replay.c) | Scope of screenshot, primary-random and media-request observations. |
| [W02](W02-map-generation.md), [F01](F01-determinism-and-arithmetic.md) | Existing scoped algorithm and shared arithmetic contracts. |

This document was checked against those source conditions and evidence notes.
It introduces no claim that an original executable or whole-game conformance
suite was run for this documentation change. The known-difference table is a
starting audit list; each later owner must record its own exact behavior and
evidence before claiming compatibility.
