# Stack preset compatibility

## Decision

Keep the existing initializers listed below as the default. They preserve the
project's current replay behavior. They are **captured compatibility presets**,
not a claim that the original always reads these constants. This decision changes
no initializer, branch, save format, network behavior or runtime option.

The original executable sometimes reads a local before assigning it. Earlier
calls can leave observable values in that stack storage. Ordinary C++ locals
cannot reproduce those values by being left uninitialized; their behavior would
be undefined. The current explicit initializers make the reconstructed program
deterministic and match the project's recorded scenarios. They do not reconstruct
every original stack history.

In particular, replacing a sampled pointer with zero, a previous value or a
guessed sentinel can change gameplay. A future correction needs a separate
compatibility decision and evidence at the actual consumer. Keep such changes
separate from naming, formatting and type cleanup. There is no corrected mode in
this change.

## Inventory

This inventory describes `FRAME_PRESETS` in the retired [generator](../tools/gen_c.py)
and its current C++ counterparts at base commit
`1ff87e33fbec8b7be8c4bf98c77709e16aad4f31`. Source is edited directly; do not
regenerate it to update a preset. Nine groups initialize fifteen DWORD slots:

| Original entry | Current local / file | Preset | Offset from entry ESP |
| --- | --- | --- | --- |
| `0043d400` | `road_tiles`, [city.cpp](../src/auto/city.cpp) | `0` | `-0x12c` |
| `00560d95` | `attitude_delta`, [diplomacy.cpp](../src/auto/diplomacy.cpp) | `0x6715d7a0` | `-0x34` |
| `00538a29` | `defender_count`, [ai.cpp](../src/auto/ai.cpp) | `0x108` | `-0x9c` |
| `00538a29` | `needed_defenders`, same function | `0x64a8c19c` | `-0x11c` |
| `00538a29` | `cand_score`, same function | `0x3bd` | `-0x70` |
| `00538a29` | `best_score`, same function | `0` | `-0x38` |
| `00538a29` | `goal_match`, same function | `4` | `-0x16c` |
| `004abfe5` | `civ`, [map.cpp](../src/auto/map.cpp) | `0` | `-0x80` |
| `0059062c` | `civ_sees[1..7]`, [unit.cpp](../src/auto/unit.cpp) | `1` for each | `-0x9c, -0x98, -0x94, -0x90, -0x8c, -0x88, -0x84` |

These offsets are from ESP at the original function entry, before its prologue.
For these functions' `PUSH EBP; MOV EBP,ESP` frames, the corresponding EBP offset
is four bytes greater: entry `ESP-0x34` becomes `EBP-0x30`. The probe's multiword
span also advances toward higher addresses: word `k` is at `ESP-OFF+4*k`.
Neither convention refers to the reconstructed C++ frame layout.

The visibility preset does not initialize `civ_sees[0]` or the separate animation
array. The AI's selected city-role local at original `EBP-0x28` is not one of the
five AI presets. Evidence about that role does not justify changing these five.

## What the existing samples establish

The following counts and descriptions come from the generator's retained notes;
this change has not rerun those Wine recordings or recovered their raw logs.

* The city improvement counter was zero in **687 of 713** calls over 50 turns;
  the remaining calls held a stack address. The zero preset is already a choice
  among observed entry values. The counted tile flag is `0x10`.
* The diplomacy preset was observed in the WWII scenario under Wine. The note
  says the large positive value saturates attitude there; that does not prove
  that every value or every caller has the same outcome.
* The five AI values were reported unchanged over **2,367 calls / 15 turns**
  from the same call site. One return address does not establish identical prior
  callees, physical stack storage or writes on other runs.
* The pathfinder local was zero in **3,892 of 4,300** calls and `-1` in **158**.
  The note does not characterize the other **250** values. Zero is a real
  civilization index, not a generic no-owner sentinel.
* The visibility note reports nonzero values. Initializing each entry to `1`
  preserves a Boolean interpretation; it does not reproduce the sampled bytes.

[`hook/stackprobe.c`](../hook/stackprobe.c) samples at function entry. A sample
alone cannot show whether that value survives until a read, is overwritten, or
is on a path that is never consumed. The log includes a return address and three
stack words following it (labelled `args`), but not absolute ESP or a write
history. Those words need not all be formal arguments.

## Independent original-executable evidence

[`stack-compatibility-evidence.json`](stack-compatibility-evidence.json) contains
small historical excerpts contributed from an independent Windows reconstruction
investigation. It records the original executable SHA-256, archived Git revision,
source report paths and hashes, and exact extracted observations. No game assets
are included. These are retained results, not a fresh reproduction in OpenCiv2;
the full harness, logs and independent corrected source are not part of this
patch. Its controlled Windows runs and this project's Wine recordings are
different environments.

The excerpts constrain claims about universality; they do not supply replacement
presets:

* **City counter:** the observed incoming value for city 35 came from
  `005ae3bf PUSH EBP`; the later read was at `0043d788`. This was a saved frame
  value, not a recovered zero initializer. That bounded writer watch detached
  without completing a turn.
* **Diplomacy:** a parent-frame watch recorded `0x00453e93` in the future
  `EBP-0x30` slot, from the return address pushed by an earlier call at
  `00453e8e`. Its last observed write matched the incoming word, and the
  watched run completed a save matching the unobserved control. This differs
  from the Wine preset `0x6715d7a0`; it is not a new universal constant either.
  A separate recorded effect comparison published attitude delta `2268998`
  from the original versus `-3` with an intentional initial-zero correction in
  the independent reconstruction. Attitude clamped to 100 versus 0 and the
  selected unit attacked versus held, despite matching treaty and game RNG.
  Thus zeroing this local can have an observable gameplay effect.
* **AI transport:** two entries contained `0x013fde70` at both `EBP-0x168`
  and `EBP-0x118`, rather than the current presets `4` and `0x64a8c19c`.
  The watch hit its 20,000-write budget. The last observed writer matched the
  next incoming transport-flag word, but **did not** match the quota word.
  The excerpt therefore proves entry variation, not complete quota provenance
  or consumption of either entry value.

The effect excerpt's terms "source" and "authorized" refer to that independent
reconstruction's intentional correction, not an OpenCiv2 policy or approval.
Matching an unobserved control save checks the effect of observation in the
stated run; it does not extend coverage to other inputs.

## Inspecting additional entry samples

Use the existing hook with a scenario that reaches the selected function. For
example, to request the diplomacy word while recording `turns`:

```sh
CIV2_STACKPROBE=560d95:34:4 make ref SCRIPT=harness/scripts/turns.txt
python3 tools/stackprobe_report.py run/ref-turns/hook.log > stack-report.json
python3 -m unittest discover -s tests -p 'test_stackprobe_report.py' -v
```

The address, offset and length are hexadecimal. Use a four-byte length for one
DWORD. Choose at most one probe per function entry per run; the current handler
selects the first matching address. Preserve each run's output before recording
another, and pass multiple retained logs to the report tool to compare them.
Reaching the function is required: a log with no probe records fails the report.

The read-only tool emits JSON with raw input hashes, observed values and counts
for each `(function, offset from entry ESP)`, separated by input log, return
address and the three entry stack words. It accepts the hook's CRLF and trailing
NUL padding. Missing, empty or malformed probe evidence fails without partial
JSON. `observed_single_value` means only one value occurs in the supplied samples;
it never certifies a constant or a consumer read. A log cut off after a complete
record cannot be detected from this format, so validate scenario completion
separately. The regression logs are synthetic format tests, not gameplay data.

## Evidence needed for a future behavior change

Keep the default presets while investigating. For a proposed replacement:

1. Identify the original consumer and paths that skip initialization, the live
   caller inputs, and the prior effects that must still occur.
2. Record original bytes, the last relevant writer and intervening clobbers at
   the same physical address. Distinguish entry samples, read-time observations
   and values deliberately supplied by a fixture. Report incomplete histories.
3. State whether the change reproduces a supported original context or
   intentionally corrects a defect. Do not substitute zero solely because the
   original value is unknown or looks like an address.
4. Preserve independent original baselines. Check the affected path and an edge,
   then existing replays. Screenshots and RNG are useful; also compare the
   affected state and saved continuation when the effect can be visually silent.
5. Decide defaults, replay-baseline handling and mixed-peer compatibility before
   introducing a mode or changing gameplay. Do not silently replace reference
   results to make a correction pass.

This patch validates the report parser and preserves the game source tokens.
It does not add original gameplay coverage or claim that the existing presets
are correct outside their recorded contexts.
