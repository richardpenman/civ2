# Game state and invariants

**ID:** F02. **Status:** Written. **Target:** Civilization II Multiplayer Gold
Edition **5.4.0f, patch 3**, under the [X01 contract](X01-classic-compatibility-contract.md).

This specification defines the logical state shared by the game algorithms:
identity, record lifetime, references, ordered collections, ownership, remembered
information, cached results and mutation boundaries. It also specifies the
common slot-selection and state-access rules that constrain those algorithms.
It does not prescribe an object model, memory layout or database schema.

Evidence is pinned to OpenCiv2
`786b4a8572d7ed5d34f47566c026b5d49327d0cc` and the independent reconstruction in
`/home/peter/project/civ2` at
`4cfdd2a4b43e1a99983cc6af3e993a52b8ad8752`. Addresses, original encodings,
source links and validation limits are in the appendices.

## 1. Scope and entry conditions

The input to a state operation is the state at its entry boundary, including the
current game mode, rules, control context and any retained scratch values it
reads. Its output is a return value, ordered state changes and any explicitly
delegated effects. A snapshot of the live cities and units alone is insufficient
to resume every operation.

The ordinary domain consists of allocated worlds, supported rule tables,
civilizations numbered 0 through 7, and references to existing records or the
specific absence markers below. Civilization 0 is the barbarian participant;
1 through 7 are the normal civilization slots. A slot's current tribe, its human
control and its existence in the game are separate properties.

The list invariants below describe coherent state between completed operations.
They are not assertions that hold after each store, during loading, during
network updates or while a unit is temporarily removed from the map. A diagnostic
may report a broken invariant; it must not silently repair state before an
operation that would have read that state unchanged.

F02 owns the state contract, not all transitions that use it. Full unit creation,
deletion and cargo operations belong to U01/U05; city founding, destruction and
transfer belong to C01/C09. Those operations must honor the identity and collection
rules here, while also specifying their policy, callbacks and side effects.
F06 owns serialized formats, legacy conversions and partial-load failures;
N02 owns network authority and state publication. These boundaries matter because
a record update is not necessarily a complete game command.

Arithmetic uses [F01's numeric model](F01-determinism-and-arithmetic.md#1-numeric-model-and-notation).
In particular, a wrapping counter keeps its specified modulus even if stored in
an unlimited-precision integer. The isolated slot-selection, list and information
access operations below consume **no random draws or clock queries**. Their
callers may do so; this is not a zero-draw claim for creation, recalculation or
event execution as a whole.

## 2. State layers and lifetimes

| Layer | Required logical state | Lifetime rule |
| --- | --- | --- |
| Process | Both random streams; next unit and city lifetime tokens; loaded rule/leader tables and their mutable usage/name cursors; preferences and external service state. | A new game resets only what its setup procedure writes. Process launch, starting a game and loading a game are distinct boundaries. |
| Session | Single-player, hotseat or network mode; local and authoritative participants; current input/view civilization; selected and moving unit; resume/turn-loop state; queued commands, modal state and operation-busy indicators. | May change without changing the world. A save does not necessarily capture or restore all of it. |
| World | Calendar and turn; civilization records; city and unit slot pools; map and remembered layers; rules/options, wonders, research history, scenario/event state and historical records. | Mutated by game operations, setup, loading, editors and synchronization. |
| Derived state retained between calls | Civilization censuses and strategic summaries, city economic totals, region/connectivity information, demand lists and their dirty flags. | Rebuilt or incrementally updated at particular boundaries; “derivable” does not mean always current. |
| Operation workspace | Current city calculation, path-search context and grids, diplomacy evaluation, combat/movement state, generation scratch and presentation selections. | Shared by successive calls in the original. Preserve any value read before the next explicit overwrite. |

A cleanroom implementation may split these layers differently. It must preserve
their observable retention and sharing. For example, a second classic game can
retain a human tribe's city-name cursor. The new-game reset clears the unit and
city scan bounds but does not reset their lifetime-token generators. It also
initially sets the selected unit slot to 0, even though the unit scan bound is 0;
selection validity must be tested when the selection is used.
Records outside the current scan bound can retain earlier data; they are not
enumerated merely because their old tokens remain nonzero.

The following are distinct and must not be collapsed into one `currentPlayer`:

- The civilization whose simulation turn is running.
- The configured local player civilization and the current input/view context.
- The saved/resume civilization used by turn continuation.
- The set of civilizations currently treated as human by the running operation.
- The underlying hotseat roster or network player assignments.
- A cheat/view-as civilization and whole-map revelation setting.

Hotseat handoffs and view/cheat paths can temporarily replace the effective human
set. Autoplay is separate from membership of that set. Network clients route
many mutations to the host and wait for replies; a busy indicator is operation
state, not evidence that the local record has already changed.

## 3. Slots, lifetime tokens and absence

### Identity domains

| Identity | Meaning |
| --- | --- |
| Civilization slot | Stable numeric position 0–7. The tribe occupying it may be reset or replaced. Alive, human and present-at-start sets are independent. |
| Tribe definition | One of 21 leader/tribe definitions. Several session decisions use this definition rather than the civilization slot; do not confuse the two. |
| Unit slot | Stable index into the unit pool. Ordinary allocation can extend the pool through slot 2047. Links, selected units and cargo references use slots. |
| City slot | Stable index into the city pool. Ordinary creation extends through slot 254. Home-city and trade-route references use slots. |
| Lifetime token | A separate value assigned when a city/unit becomes a new object. Zero marks a free record. A reused slot receives a new token. |
| Rule index | Position in a rule table, such as unit type, advance, improvement or terrain. It is not a runtime-object token. |
| Region label | A spatial classification used by map and civilization tables. Land and water classifications are contextual; a number alone does not identify a city, tile or civilization. |

The unit and city pools each retain an **exclusive scan bound**, named `unitEnd`
and `cityEnd` here. Most whole-pool scans visit slots `[0, end)` and skip records
whose token is zero. These bounds are not the number of live records, and are
not always one more than the highest live slot. Interior holes remain in place.
Compacting the pool would change references, scan order and tie breaks.

There are separate unit and city token sequences. Both start at 1 at process
initialization. On assignment, use the current token and then increment modulo
`2^32`. There is no recovered collision search or skip-zero rule. Normal fixtures
should avoid token exhaustion; if testing wrap, a zero assignment has the same
free-record meaning as any other zero. Do not promise everlasting uniqueness.
Numeric sign alone does not indicate liveness: a nonzero token remains occupied.

For the current save format, loading replaces every nonzero unit token in
ascending slot order with a fresh unit token, then does the same for cities with
the separate city sequence. Holes retain zero. Saved token values therefore
identify occupancy during import, not persistent identity across loads. The
process's current sequence positions are inputs to this conversion; older-format
occupancy conversion is a separate F06 contract.

Use a pair `(slot, expectedToken)` where an operation actually validates an old
selection against a lifetime. A bare slot reference does not acquire such a
check automatically. In particular, stack links, home cities and route partners
are slot references, not token references.

### Slot selection

These procedures specify the selection/reservation part of creation. They do not
initialize the record, assign its token, update civilization counts or emit the
creation command's limit notice. Run them at the authoritative creation boundary
with coherent counts and no intervening callback.

```text
reserveUnitSlot(owner):
    s = 0
    while s < unitEnd and unit[s].token != 0:
        s += 1
    if s == unitEnd:
        if unitEnd >= 2048:
            return NoSlot
        if civilization[owner].militaryCount > 1948:
            return NoSlot
        if owner is not in effectiveHumanSet and unitEnd >= 2040:
            return NoSlot
        unitEnd += 1
    return s

reserveCitySlot():
    s = 0
    while s < cityEnd and city[s].token != 0:
        s += 1
    if s == cityEnd:
        if cityEnd >= 255:
            return NoSlot
        cityEnd += 1
    return s
```

All three unit limits apply **only when extending the bound**. An interior free
slot is reused even if the civilization's recorded military count exceeds the
threshold or the nonhuman extension limit has been reached. `militaryCount` is
the recorded value at this point, not a census recomputed for the allocation.
The comparison permits extension at 1948; it rejects at 1949.

Deletion marks the selected record free after the operation's required cleanup.
Its pool-bound effect is only:

```text
record[s].token = 0
if s == end - 1:
    end -= 1
```

It does not continue scanning backward over earlier holes. It does not clear the
rest of the record. Negative deletion arguments and already-free records are
no-ops in the ordinary deletion entry points; this is not a guarantee of bounds
checking for arbitrary positive indices.

### Contextual absence markers

Use named states internally. Their original numeric encodings are in Appendix A.

| Field/context | Distinct states |
| --- | --- |
| Record occupancy | Free versus a nonzero lifetime token. Slot 0 is a valid object slot. |
| Selected unit; stack predecessor/successor; ordinary failed city/unit lookup | Absent versus a slot. Stack traversal treats a negative link as terminal; ordinary writers use the standard absent value. |
| Unit home city | No home versus city slot 0–254. High city slots remain positive identities even where the original field is declared signed. |
| Wonder location | Never built; destroyed; or a city slot. Destruction is not equivalent to never built. Effective wonder activity also depends on obsolescence and scenario rules. |
| Research target | No current advance versus a rule-table advance; Future Technology has separate repeated-progress state. |
| Rule prerequisite | No prerequisite versus unavailable/never allowed versus a technology. Do not use runtime-object absence semantics for these. |
| Tile occupant owner | No owner versus a participant. Separate from radius claimant and from whether a unit/city/airbase is present. |
| Radius claimant | No normal civilization claim, represented by 0, versus a normal civilization slot. This is not the tile occupant's barbarian identity. |
| Contact turn | Unset versus a signed turn value. Some cooldown updates place it in the future. |
| Event selector | Particular participant/type/name versus the trigger/action's wildcard. A wildcard is not a missing object. |

## 4. Global and civilization state

The global world state contains these independently retained values:

| Group | Contents |
| --- | --- |
| Calendar and phase | Turn, year, first spaceship-launch turn, turn civilization and continuation state. Keep turn and year separately; a scenario or launch can alter their relationship. |
| Configuration | Difficulty, barbarian activity, custom game rules, player-interface options, disabled-message choices and multiplayer options such as doubled movement/production. |
| Session progress | Won/continued/scored state, cheat history, scenario and saved-as-scenario states, map-size classification, AI coordination state, one-shot tutorial markers and transient turn/UI flags. A tutorial marker can be set even with its dialog disabled. |
| Population of slots | Alive, human and present-at-start civilization sets; recorded opponent count; unit and city scan bounds. |
| Environment | Pollution count, pollution accumulated for warming, warming pressure and number of warming events. |
| Research history | Number of defined advances, first discoverer and known-by set for each advance. These supplement each civilization's own known-advance set. |
| Unique achievements | Location/state of each of 28 wonders; first launch, historical-report timing, power ranks and inverse rank ordering. |
| History | Power graph and destroyed-civilization records, including turn, killer, portrait and name. A destroyed civilization's history survives reuse of its former civilization slot. |

Each civilization record retains the following:

| Group | Contents and interpretation |
| --- | --- |
| Identity and presentation | Tribe definition, optional leader/tribe/adjective/title overrides, city style, portrait/gender, throne-room pieces and upgrade count. Empty override strings request the tribe default. |
| Economy and government | Treasury, government, tax and science rates in tenths, revolution/government-change state. Luxury is the remaining share; rate validation belongs to the rate-changing operation. |
| Research | Current target, accumulated beakers, known advances, ordinary/future advance counts, research-cost adjustment and source of each advance. The source history is not recoverable from the known set alone. |
| Diplomacy | Directed relationship flags, attitudes, contact/cooldown turns, reputation credits and aggression counters for each other civilization; reputation, request counters, random diplomacy threshold and pending diplomacy state. |
| City/army summaries | Recorded city and military/ship counts, total unit count, city-size sum, attack/strength totals, population milestone, active/lost/in-production counts by unit type, and wonder counts by group. |
| Spatial strategy | Per-region military, attack, city, size and trade-unit totals; presence/threat flags and policy; best shield surplus and government scores. |
| Goals | 48 ordered current goals and 16 standing goals. A goal has coordinates, role, priority and a free marker. Negative priority has expiry meaning, not the same meaning as a free goal. |
| Spaceship | Existence, launch, arrival, launch-time propulsion and victory state; launch/arrival years, population and six part quantities. These are civilization state even before an arrival result is resolved. |

Diplomatic state is **directed**. Store both `relationship[a][b]` and
`relationship[b][a]`, including their independent incident/history flags and
timers. Do not enforce symmetry by construction. Contact, embassy, ceasefire,
peace, alliance, war, vendetta, hostility and recent incidents are separate facts;
the current treaty is not a single enum that can replace the full state.

Civilization liveness is not equivalent to `cityCount > 0`: a civilization can
survive with settlers, and elimination/revival has its own transition. Similarly,
human control does not imply that a civilization is currently taking input.

Military count uses the unit rule's role classification, not “all units” or
“attack strength greater than zero.” Ordinary creation increments it for roles
below the settlement role; deletion decrements it only if nonzero and the removed
unit qualifies. Active counts per unit type wrap modulo 256; deletion leaves a
zero active count at zero rather than decrementing it. Consequently, these counts
need not equal an unlimited-precision live census after wrap or partial updates.
The military count and other signed totals also retain their arithmetic domains.

The shared numeric domains are behavioral requirements, independent of storage:

| State | Retained numeric domain |
| --- | --- |
| Treasury | Signed cyclic `-2^31..2^31-1`. |
| Turn/year, research progress/target, military/city/ship counts, city-size sum and attack total | Signed cyclic `-32768..32767`; the meaning of negative values remains field-specific. |
| Military strength and per-region military/attack totals | Modulo 65536. |
| Advance/future-tech counts, rates, reputation, unit-type counts, region city/size/trade-unit counts | Modulo 256 at storage; algorithms may impose smaller ordinary ranges or read a signed interpretation. |
| Lost units by type | Saturates at 255 in the loss-recording operation, rather than wrapping like the active count. |
| Goal role/priority, directed reputation credits and request count | Signed cyclic `-128..127`; free/expiry markers retain their distinct meanings. |

These domains do not authorize a universal clamp, saturation or validation pass.
In particular, the military-role predicate uses the signed interpretation of the
rule's role value; malformed/custom rule behavior beyond the declared F03 input
domain is not established by the ordinary role names.

AI assessment clears and rebuilds many of these summaries in a particular order,
and also changes goals, flags and sometimes units. It is not a pure “repair all
counts” function that may be called at arbitrary times. A cached total remains
an input until the specified producer replaces it.

## 5. Unit state, positions and stacks

### Unit record

| Property | Meaning |
| --- | --- |
| Identity and ownership | Slot, lifetime token, unit-type rule and owner civilization. Home-city affiliation is separate from ownership. |
| Position | On-map coordinates or an off-map staging position. Off-map does not mean free or destroyed. |
| Stack membership | Ordered predecessor/successor links. A singleton has neither; that alone does not establish whether it is currently placed. |
| Movement/combat | Movement spent, damage taken, last direction, veteran/moved/paradropped state. Damage is loss, not remaining hit points. |
| Orders | None, fortifying, fortified, sleeping, terrain-work kind, goto, or the separate AI idle/goto states. Fortifying and fortified are different states. |
| Task payload | Destination fields, work/commodity/AI counter, auxiliary timer and AI task code. Their meaning depends on orders, unit role and the current operation. |
| Visibility | Set of civilizations recorded as seeing the unit, independent of tile exploration and city discovery. |
| Other flags | Waiting, automation, aboard, border intrusion/warning, retreat, goal reached, garrison/colonist assignment, military unhappiness/support charges, spy mission and temporary scratch marker. |

Movement spent, damage, task counter, auxiliary timer and direction values have
256 possible states; writes that overflow wrap as described by F01. Do not assume
that damage is less than maximum hit points or that movement spent is less than
the current allowance. Queries clamp remaining amounts where their algorithms
require it. In simplified combat, the remaining-hit-points query itself clears
stored damage: a function named as a query is not necessarily read-only.

A sleeping unit can use the destination's first component as a **carrier unit
slot**. The same field is a coordinate for goto orders. Model the meanings
separately if convenient, while preserving value retention when the discriminator
changes. Setting sleep clears that field to absent only if the unit was not
already sleeping. Removing a carrier wakes every sleeping record in the scan
range whose stored carrier slot matches it; that scan does not first filter out
free records. It does not itself delete those cargo records.

Temporary removal places a unit at an owner-specific off-map location. Other
operations use different staging locations for cargo or ordering. These locations
can participate in ordinary coordinate-based stack lookup. They must preserve
their equality/grouping behavior; treating every off-map unit as having one
common `null` position would merge groups that the original keeps separate.
Appendix A maps the inspected staging positions to reference coordinates.

### Ordered stack contract

A coherent placed stack is a finite, noncyclic ordered sequence of live units at
the same exact coordinates, with reciprocal predecessor/successor links and
terminal endpoints. Normal linking searches by owner as well as coordinates.
The specification does not infer that arbitrary mixed-owner or imported stacks
will be normalized. Unit and city coordinates are stored explicitly; the map's
tile aliasing does not make two different coordinate pairs equal in record scans.

For coherent input, the common operations are:

```text
firstInStack(s):
    if s is absent: return s
    follow predecessors from s until the terminal link
    return that slot

firstForOwnerAt(owner, position):
    scan unit slots from 0 through unitEnd - 1
    take the first live record with that owner and exact position
    return firstInStack(found), or Absent if none matched

detach(s):
    remember whether s has a predecessor and successor
    if there is a predecessor: predecessor.next = s.next
    if there is a successor: successor.previous = s.previous
    clear both links on s
    if neither neighbor existed and the old position is in bounds:
        clear the tile's unit-present state
    place s at its owner's detached staging position

attachAtFront(s, position):
    head = firstForOwnerAt(s.owner, position)
    s.position = position
    s.previous = Absent
    s.next = head
    if head exists:
        head.previous = s
    else if position is in bounds:
        set tile.unitPresent
        set tile.occupantOwner = s.owner

relocate(s, position):
    detach(s)
    attachAtFront(s, position)
```

`attachAtFront` requires a detached/new record not already in the destination
chain. It does not detach the record itself. Relocating a member to its current
position moves it to the front. Moving an entire stack by visiting its old
head-to-tail order and inserting each member at the destination front reverses
the transferred order, ahead of any existing destination chain. Iteration must
capture the old successor before relocating the current member.

Detaching the last unit clears the unit-present marker but **leaves the tile's
occupant owner unchanged**. Attaching to a nonempty same-owner chain also leaves
the tile owner unchanged. This is why occupant owner and unit presence must be
independent state. These primitives do not reveal terrain, spend movement, update
home affiliation or enforce movement/capture legality.

Original stack accessors invoke a validator that can repair damaged links.
The pseudocode above specifies their coherent-input result. Cyclic, dead-linked,
nonreciprocal and cross-location input is not covered by a generic repair rule
here; the original validator has different directional tests and observable
repair order. Its complete recovery contract belongs with U01. New validation
must not be mistaken for recovered original behavior.

### Retained free-slot state

Deleting a unit changes its token, links and staging position, but retains such
fields as damage, orders and destinations unless the deletion path explicitly
writes them. Creation also initializes fields in sequence rather than clearing
the entire record first.

In simultaneous network movement, creation assigns the new type, owner and token,
then computes movement spent from the movement allowance **before clearing the
old damage**. The reused slot's damage can therefore affect the new unit's initial
spent movement. In local play and nonsimultaneous network play, that initial
spent movement is zero. Record reuse must preserve this input even if the new
implementation represents vacant slots separately from live objects.

## 6. City state and dependent references

| Property | Meaning |
| --- | --- |
| Identity/location | Slot, lifetime token, exact coordinates and name. |
| Ownership/history | Current owner, founding civilization and capture-age counter. Transfer does not automatically change the founder. |
| Citizens | Size, worked-square selections, specialist count, individual specialist kinds, happy and unhappy totals. |
| Stocks and production | Food and shield boxes; selected unit, improvement, wonder or special production item; installed improvements. |
| Economy | Base trade, remembered science/tax/trade and food/shield totals; these are written by city processing/recalculation, not intrinsic immutable properties. |
| Trade | Three supply and three demand entries; an ordered list of at most three routes, each with partner city slot and commodity/food kind. |
| Knowledge | Persistent set of civilizations that know the city and its separately remembered size for each civilization. |
| Status | Disorder and its duration states, celebration, improvement sold/airlift used this turn, theft/investigation, completed-wonder/caravan history, objective status and automation choices. |
| Cached/local policy | Coastal/hydroelectric/ocean-access properties, commodity dirty flag, defender lost, nearby settler, ferry-port/freshness, specialist retention and terrain-work goals. |

Size uses the signed cyclic domain `-128..127`. Classic growth beyond 127 can
produce -128; do not add an implicit size cap or use positivity as the record's
liveness test. Food/shield stocks and science/tax/trade fields use signed cyclic
`-32768..32767`; stored food/shield/happy/unhappy totals wrap modulo 256. A rule
or editor's input restrictions are distinct from these storage-visible domains.

Worked squares are an ordered neighborhood's positions 0–20, with the center at
20. Store the chosen set separately from the specialist count, which has a
64-value cyclic domain. There are 16 explicitly retained specialist kinds:
unused, entertainer, tax collector and scientist. For a nonnegative specialist
index of 16 or greater, the kind query returns entertainer and the setter does
nothing. Counting stored kinds scans only the first 16 positions. Do not extend
that count to every implicit entertainer in a large city.

Installed improvements and the selected production item are independent. Wonders
also have global location/history state; an ordinary building-membership set
cannot replace it. A city can retain the same production choice while its
eligibility changes through advances, rules or ownership.

Supply/demand entries distinguish the commodity identity from its signed
availability/usage marker. Negating commodity zero still yields zero, so a
separate internal flag must not introduce a distinction the original lacks.
Route goods use a separate food-route state. Routes are ordered and need not
form a symmetric graph: deleting one entry shifts later entries left, decrements
the count and marks demand dirty, leaving the now-unused trailing payload intact.
Do not infer validity or reciprocal routes from a nonempty trailing field.

City deletion is not just freeing the record. Its callers must account for
supported units, inbound route references, wonder locations, radius/fertility
updates and open city views. The observed deletion path processes supported units
before freeing the city; removes matching inbound routes from other live cities;
and changes wonders located there to **destroyed**. Rehoming versus destruction
of supported units is a policy decision of the city lifecycle, not an invariant
that all such units disappear or become homeless.

### City knowledge update

For a valid city and civilization 0–7:

```text
revealCity(city, civilization):
    add civilization to city.knownBy
    city.rememberedSize[civilization] = city.size
```

A negative civilization argument causes no change. This primitive does not
reveal the tile, update its remembered improvements, or refresh the unit visibility
set. A later city-size change does not automatically change every observer's
remembered size.

## 7. World and spatial state

The world has a width in doubled horizontal coordinates, a row count, flat or
wrapping topology, a resource-pattern seed and coarse-grid dimensions. A row has
`width/2` tile records. Normal positions satisfy equal horizontal/vertical parity;
the tile accessor nevertheless aliases the two horizontal coordinates in a pair.
Bounds checking and wrapping are distinct operations. In particular, record
lookup does not silently wrap a supplied coordinate or correct its parity.

Each tile retains:

- Base terrain, river and resource suppression state, plus any preserved flags
  whose interpretation is not established here.
- Unit presence; city/fortress/airbase state; irrigation/mine/farmland state;
  road, railroad and pollution state.
- Occupant owner, independently retained radius claimant and fertility state.
- Region classification and exploration/revelation set.

There is also a separate remembered-improvements layer for each normal
civilization, a region summary/border table and four coarse work/connectivity
grids. These are not interchangeable with the live tiles. A change to terrain or
improvements does not automatically refresh memories, region classifications,
connectivity, fertility or city totals.

Some original feature combinations have compound meanings. City without the
fortress component denotes a city; city plus fortress components denotes an
airbase. Irrigation plus mine components denotes farmland. A logical enum may
represent these combinations, but it must reproduce the individual feature
tests and partial updates. Clearing one component can change the compound kind.
Railroad and road are separately stored; do not derive one merely from the other
when accepting or comparing state.

| Query | State it actually reads |
| --- | --- |
| Tile owner | The retained occupant owner, with an out-of-bounds/absent result. It does not first require a city or unit. |
| Unit owner at tile | Unit-present state, then occupant owner. |
| City owner at tile | In-bounds city kind, excluding airbase, then occupant owner. |
| Airbase owner at tile | Airbase combination, then occupant owner. |
| City-or-unit owner | City owner if present, otherwise unit owner. |
| City at coordinates | First requires an in-bounds tile reporting a city owner; then scans live city slots ascending for an exact coordinate match. It does not compare the record owner with the tile owner. |
| First unit at coordinates | Normally scans live unit slots ascending for an exact match, then returns that unit's stack head. During unit-move animation, a missing tile unit owner suppresses the scan. |

The city query's tile gate and the unit query's animation gate are observable.
A spatial index can accelerate scans only if it preserves these conditions,
the selected matching slot and the stack ordering.

Updating one civilization's remembered tile copies the **current improvements**
into that civilization's remembered layer. It is not a full copy of the terrain
record, and it does not itself add the civilization to the tile's explored set.
The update helper does nothing for civilization 0. Tile exploration, remembered
improvements, city discovery/remembered size and unit visibility remain four
distinct channels of knowledge.

Out-of-bounds tile access uses a shared dummy tile in the original; it is not a
fresh record returned per call. Do not assume every accessor has the same
out-of-bounds result: owner explicitly returns absent, while terrain and feature
accessors can read the dummy. Operations that can mutate/read that dummy require
its state as an input. Arbitrary out-of-range remembered-layer access has no
corresponding safe default here.

## 8. Scenario, event and rule-associated state

Scenario state includes enabled options, name, technology-cost parameter,
calendar increment/start, turn limit, objective civilization and four outcome
thresholds. Starting positions are indexed by tribe definition, not civilization
slot. The game's scenario-active status, saved-as-scenario status, scenario
options and whether event data is loaded are separate values.

Events form an ordered list. Each record contains a trigger kind and its matching
parameters, action choices and payloads, and one-shot execution state. Relevant
payloads include participant/type selectors and names, up to 20 text lines,
movement rectangles and destination, creation locations/home-city selection,
money, diplomacy, technology, terrain and media choices. Event text must remain
owned and valid for the event's lifetime; a temporary parser buffer is not its
storage contract.

Allocation appends a cleared event after the previous tail and increments the
count: no trigger or actions, absent text references, zero numeric payloads and
terminal list links. Failure to allocate returns no event without appending
one or incrementing the count. Reset removes the whole list and resets its count
and string-pool ownership. A cleanroom implementation can use an ordered array
and owned strings instead of linked records and a pool.

One-shot state is retained on the event; an already-used one-shot event stays in
the list. Its next action execution is skipped. The used marker is set **after**
the action sequence returns, not before its first callback. The list's position,
trigger and payload are not erased by that marker. Full action order and nested
event behavior belong to S02; F02 does not introduce transaction rollback if an
action fails or reenters the game.

Rule definitions include 62 unit types, 100 advance slots, 67 improvement/special
production entries, 28 wonder obsolescence entries, 11 base terrain kinds with two
special-resource variants each, 7 governments, 21 tribes and 16 commodities.
Their numeric positions and order can affect algorithms. Resolved text may be
stored as strings internally; the original string-pool ordinals are not gameplay
object IDs. Keep rule content separate from mutable world state such as a tribe's
usage and next-city-name counters, even when the original stores them together.

Rule/edit/load operations may replace definitions that existing records refer
to. They must preserve or explicitly update those references; copying rule
values into every unit at creation would lose later rule changes. Exact parsing,
allowed custom values and rename propagation are F03/S04 contracts.

## 9. Cache and mutation contracts

### The shared city workspace

City calculation uses a shared workspace containing tile availability/yields,
worker and specialist counts, support, capital links, route contributions,
corruption/waste, tax/science/luxury, food surplus, pollution and staged citizen
moods. It also has a remembered calculated-city slot and turn/UI mode flags.
The city record stores only some of the resulting totals.

The recalculation entry has this dispatch:

```text
cityCalculation(city, full):
    if city is Absent:
        return 0
    if not full:
        return workspace.happyCount - workspace.unhappyCount
    workspaceCity = city
    perform the full city-calculation pipeline
    return its happiness result
```

The non-full path does **not** compare `city` with `workspaceCity`, select that
city's stored values or refresh anything. It can therefore return the previous
city's result. An absent city returns zero even when the workspace holds a
different value. The full pipeline can change assignments, flags and record
totals, so caching it as a pure function would change behavior. C02–C04 own the
pipeline's formulas and ordering; this dispatch defines its shared-state boundary.

### Mutation responsibilities

| Mutation | State that must be accounted for by its owning operation |
| --- | --- |
| Create/reuse a unit | Slot reservation, token sequence, civilization counters, ordered field initialization including retained damage, home selection, stack placement, visibility and notifications. |
| Remove a unit | Counts with their zero guards, unlinking, occupancy marker, record token/bound, sleeping carrier references, city views and possible civilization-elimination dispatch. Other unit-loss/cargo effects belong to the caller. |
| Relocate/reorder a unit | Both affected chains, exact coordinates, source unit-presence marker and destination owner when the destination chain was empty. A move command additionally owns costs, reveals and entry effects. |
| Change unit ownership | Old/new civilization bookkeeping, stack/occupant ownership and applicable home/cargo relationships. Assigning only the owner is not a complete transfer. |
| Found/remove/transfer a city | City and civilization identity/count state, tile kind/owner, radius/fertility, dependent home/route/wonder references, production bookkeeping and city-view lifetime. |
| Change city production or workers | Persistent selection/assignments, production counters or saved switch inputs where applicable, dirty flags and recalculation at the caller's specified boundary. |
| Change terrain/features | Preserve untouched tile properties; update memories/regions/connectivity/city data only where the specific operation calls for it. |
| Grant an advance or change a wonder | Local knowledge/location, global knowledge/history, source attribution, effective effects and required downstream updates. A Boolean grant alone is insufficient. |
| Load or synchronize | Restore the specified sections in order; perform only the named fixups/rebuilds. Intermediate state need not satisfy all stable-state invariants. |
| Return from a callback | Respect values deliberately captured before the callback and reread fields that the original operation reads afterward. Revalidate identity only where that operation does so. |

Logical setters preserve unrelated flags and properties. A fresh object may
initialize a flag set; changing one property of an existing object must not
replace the entire set or clear an unrecognized preserved value. Unknown fields
and bits are opaque state until their producer/consumer is established, not
permission to invent a meaning or zero them during a round trip.

Useful diagnostics for a declared coherent boundary are: slot bounds in their
ordinary domains; each enumerated live record has a valid owner/type; each
followed stack link targets a live record and is reciprocal at the same position;
no stack cycle; referenced home cities and active route partners exist; ordinary
city placement agrees with the tile's city kind; and route counts do not exceed
three. Scope each check to the operation being tested. Do not assert that every
cache equals a fresh census, every selected slot is live, every live unit is
on-map, every known size is current, or every diplomatic relation is symmetric.

## 10. Worked conformance cases

The following are operation-level cases, not complete game snapshots. Unless
otherwise stated, use local single-player mode, coherent ordinary records,
no callbacks, no observation/redraw/network effects and unchanged RNG states.
Any field not mentioned is retained unchanged. Slot-reservation cases end before
the remainder of creation and before any limit notice.

| Case and complete relevant input | Expected result/state |
| --- | --- |
| Unit reservation: `unitEnd=2040`, all scanned tokens nonzero except slot 7; owner is nonhuman, military count 2000. | Slot 7, bound still 2040. Neither extension limit is consulted. |
| Same input with no hole. | `NoSlot`, no state change. |
| Unit reservation: `unitEnd=2039`, no holes, nonhuman owner, military count 1948. | Slot 2039, bound 2040. At military count 1949 instead: `NoSlot`, bound 2039. |
| City reservation: `cityEnd=255`, all tokens nonzero. Then repeat with slot 0 free. | First attempt fails; second returns slot 0 with bound still 255. |
| Pool finalization: `end=5`, tokens `[11,0,13,0,15]`, finalize removal of slot 4. | Tokens `[11,0,13,0,0]`, bound 4, live count 2. The preceding hole is not trimmed. |
| Current-format token import: unit tokens `[50,0,90]`, next unit token 100; city tokens `[7,9]`, next city token 200. | Unit tokens `[100,0,101]`, next 102; city tokens `[200,201]`, next 202. Slot references unchanged. |
| Decode a home-city field containing the reference representation for city 200. | Home is city slot 200, not absent and not a negative city index. |
| Stack `[2,5,1]` at position A, same owner, all live; destination B has stack `[9]`. Move A's members in old head-to-tail order to B using relocation. | B becomes `[1,5,2,9]`; A's unit-present state clears; A's retained owner is unchanged. |
| Single live unit 3 is the only member at A. Detach it. | Token unchanged; both links absent; owner-specific staging position; A's unit-present state clear and occupant owner unchanged. |
| City 4 at `(6,4)` is live, owner 2; matching tile is an airbase with occupant owner 2. | City lookup returns absent. Changing tile kind to city permits lookup to return 4. Record presence alone did not suffice. |
| City 4 has size 6, known set `{1}`, remembered sizes 3 for civ 1 and 0 for civ 2. Reveal it to civ 2. | Known set `{1,2}`; remembered sizes remain 3 for civ 1 and become 6 for civ 2. Tile and unit knowledge unchanged. |
| Shared workspace is for city 4, happy 5 and unhappy 2. Query city 9 with `full=false`. | Return 3; workspace and stored city values unchanged. Query absent city instead: return 0. |
| Specialist slot 16 queried with any first-16 contents; then assigned scientist. | Query gives entertainer; setter changes nothing. Counting stored scientists still considers only slots 0–15. |
| City-size write increments 127 in the classic signed cyclic domain. | Stored size -128; token unchanged. No automatic deletion or cap follows from the write alone. |

Also exercise the unit-reuse ordering with a simultaneous network host: choose
an existing hole whose retained damage is nonzero, a new land unit type with
maximum hit points 20 and base allowance 6, road movement scale 3, normal combat,
no movement bonuses and retained damage 10. Creation's allowance is 3 when read;
initial spent movement becomes 3, then damage becomes zero. With the same inputs
in nonsimultaneous mode, spent movement is zero. This case stops after those
initialization writes, before home selection, placement or reveal callbacks.

## Appendix A: Original representation for validation

This mapping is for interpreting reference memory and fixtures. It does not
require matching internal layouts. Persistent byte ordering and versioned file
records will be specified by F06. Preserve extra backing only when a case reaches
it; ordinary allocated-object domains do not expand merely because adjacent
memory exists.

### Records and identifiers

| Logical state | Reference representation |
| --- | --- |
| Unit record | 32 bytes, ordinary array base `006560f0`; `x,y` at `00,02`, flags `04`, type/owner `06,07`, spent/visibility/damage/direction `08..0b`, AI task/counter/timer/orders `0c..0f`, home `10`, destination `12,14`, previous/next `16,18`, token `1a`. `11` and `1e..1f` are not ordinary initialized gameplay properties. |
| City record | 88 bytes, base `0064f340`; position `00,02`, flags `04`, owner/size/founder/capture age `08..0b`, known mask `0c`, remembered sizes `0d..14`, specialists `16`, food/shield/base-trade `1a,1c,1e`, name `20`, worked/count word `30`, improvement set `34..38`, production `39`, route count `3a`, supplies/demands `3b,3e`, route goods/partners `41,44`, science/tax/trade `4a,4c,4e`, food/shield/happy/unhappy totals `50..53`, token `54`. |
| Civilization | 1428-byte record at `0064c6a0`, eight records. Directed treaties at `20`, attitudes `40`, known advances `58`, military/city counts `66,68`, advance sources `74`, active/lost/production unit counts `d8,116,154`, region tables `192..3d1`, spaceship `400..413`, current/standing goals `414,534`. |
| Global game block | 330 bytes at `00655ae8`; unit/city scan bounds are signed 16-bit fields at `2e,30`, not population counts. Civilization name/style overrides are a separate eight-record block. |
| Tokens | 32-bit values, zero free; next-unit/next-city values at `00627fd8,00627fdc`, initially 1. Reference signed views do not alter the nonzero occupancy test. |
| Home city | `ff` is no home; otherwise decode as unsigned city slot. The signed field declaration must not turn `80..fe` into absence. |
| Other markers | Ordinary absent unit/city/stack link and no research: `-1`; never-built wonder `-1`, destroyed wonder `-2`; unavailable advance prerequisite `-2`, no prerequisite `-1`; advance source `ff` means no recorded source/initial. |
| Production | Nonnegative value selects a unit type; negative value selects the corresponding improvement-table entry by negation. Wonders occupy entries 39–66, separate from wonder indices 0–27. |
| Specialists/workers | Each of 16 kinds uses two bits: 0 unused, 1 entertainer, 2 tax collector, 3 scientist. Worked positions are bits 0–20; specialist count is bits 26–31 of a separate word. Preserve the other bits where an operation does. |
| Trade | Supply/demand are signed interpretations of stored bytes; negative values mark used/suppressed commodities. Route goods `-1` is food; ordinary commodity IDs are 0–15. Route partners are signed 16-bit city slots. |
| Orders | None `-1`; fortifying/fortified/sleep `1/2/3`; fortress/road/irrigate/mine/transform/clean/airbase `4..10`; goto `11`; AI idle `16`; AI goto `27`. The rules' no-orders label at 12 is not an ordinary assigned order. |
| Detached staging | Both coordinates `-100*(owner+1)` after unlink. Cargo gathering stages a lifted carrier at `-200*(owner+1)`; ship-to-front sorting uses `-300*(owner+1)`, both in each coordinate. Preserve other staging values at their owning operations. |

Ordinary unit creation stops at 2048 slots, but the original has special backing
around the pool and its validator admits additional indices. The independent
reconstruction models 2050 backed unit positions, whereas OpenCiv2's ordinary
array has 2048 and a separate dummy representation. Likewise, city backing has
256 records but the extension test stops at 255. This document specifies normal
creation, not the validity of every possible serialized count or special-record
access. Do not use either reconstructed array declaration as a proof of original
import limits.

### Tile and scenario mapping

| Logical state | Reference representation |
| --- | --- |
| Tile | Six bytes: terrain/flags, improvements, radius, region, explored set, owner/fertility. Base terrain is the low nibble of byte 0, river `80`, suppressed resource `40`. |
| Tile features | Byte 1: unit `01`, city component `02`, irrigation `04`, mine `08`, road `10`, railroad `20`, fortress component `40`, pollution `80`; airbase tests `(features & 42)==42`, city tests `(features & 42)==02`, farmland uses `0c`. |
| Tile ownership | Radius civilization in byte 2's high three bits; preserve its low five bits. Byte 5's high nibble is occupant owner, `f` absent; low nibble is fertility including its refresh marker. |
| Tile addressing | In bounds, tile index `(width/2)*y + floor(x/2)`. Width/height/area and coarse dimensions are in the 14-byte map header. Out-of-bounds tile access returns shared storage at `006d1188`. |
| Memories | Seven per-civilization arrays of one improvement byte per tile; no corresponding ordinary barbarian layer. |
| Sets and flags | A civilization set uses bit `1<<civ`. This encoding does not make the distinct known, explored, human, alive and visible sets interchangeable. |
| Scenario | Header at `0064bc60`: options, 80-byte name, technology parameter, year increment/start, turn limit, objective civilization and four thresholds. |
| Events | Original event is 452 bytes; trigger/actions begin at `00,04`; next/previous links at `1bc,1c0`. Text references belong to the event pool. One-shot choice is action bit `40`, already-used marker `2000`. Serialized pointer replacement is a file-format concern. |

### Flag interpretation

The following maps the named state used above to reference masks. Masks are
hexadecimal. Additional preserved bits are not assigned invented semantics.

| Record | Logical flags and masks |
| --- | --- |
| Unit, temporary/mission | Scratch `0001`, spy on mission `0002`, border intruder `0004`, AI retreating `0008`, paradropped `0010`, border warned `0020`. |
| Unit, movement/AI | Moved `0040`, goal reached `0080`, garrison `0100`, colonist `0200`. |
| Unit, support/control | Aggressive/unhappiness `0400`, costs shield `0800`, aboard `1000`, veteran `2000`, waiting `4000`, automated `8000`. |
| City, status | Disorder `1`, celebrating `2`, improvement sold `4`, technology stolen `8`, autobuild `10`, defender lost `20`, settler near `40`, coastal `80`. |
| City, history/cache | Wonder built `100`, ferry port `200`, fresh port `400`, hydro possible `800`, had caravan `1000`, second disorder turn `2000`, mood balance/on edge `4000`, specialist held `8000`. |
| City, further state | Airlifted `10000`, trade dirty `20000`, unused SAM-scoring input `40000`, terrain-work goal `80000`, long disorder `100000`, ocean access `200000`, investigated `400000`, units-only automation `1000000`, improvements-only automation `2000000`, objective `4000000`, coastal-fortress-used `8000000`. |
| Civilization | Revolution `1`, at-war summary `2`, senate active `4`, government changed `8`, Eiffel applied `10`, pending free advance `20`, traded technology `80`, has allied `100`, female leader `200`; `40` is only cleared in the inspected code. |
| Relationship, low flags | Contact `1`, ceasefire `2`, peace `4`, alliance `8`, vendetta `10`, hostile intent `20`, broken treaty `40`, embassy `80`, nuclear threat `100`, fighting `200`, recent contact `400`, aggressor `800`, sneak attack `1000`, war `2000`, changed this turn `4000`. |
| Relationship, history | Incident `10000`, nuked `20000`, tribute `40000`, extended aggressor `80000`, mercenary `100000`, war this turn `200000`, city captured `400000`, met this turn `800000`, spy chat `1000000`. |
| Spaceship | Exists `1`, launched `2`, arrived `4`, fusion at launch `8`, first-arrival victory `10`. |

## Appendix B: Source traceability and validation

The evidence here is recovered-operation/source inspection and reconstruction
agreement, not a new observation of the original executable. The logical model
does not grant unspecified reads a zero default.

| Evidence at the pinned OpenCiv2 revision | Contract established here |
| --- | --- |
| [Record definitions](../../src/include/civ2types.h), [state declarations](../../src/data/state), [named flags](../../src/include/enums.h) | State inventory and reference mapping; comments are checked against writers/readers for behavioral claims. |
| [Initial data](../../src/data/gamedata.cpp), `next_ids` at `00627fd8`; [game setup](../../src/auto/game.cpp), `game_init_new` `004aa9c0` | Initial token values, separate reset boundaries and retained name/token state. |
| [Unit operations](../../src/auto/unit.cpp), `unit_create` `005b3d06`, `unit_delete` `005b4391` | First-hole selection, extension gates, ordered initialization, retained free records, counter guards, one-step bound reduction and carrier-reference wakeup. |
| [Unit stack operations](../../src/auto/unit.cpp), validator `005b2590`, first/last/next `005b2d39/005b2cc3/005b2c82`, owner lookup `005b2daf`, sleep `005b2f50`, unlink/link/relocate `005b319e/005b345f/005b36df`, stack relocation `005b3ae0`, ship sorting `005b3b78` | Coherent list behavior, ordering, staging, preserved tile owner and explicit malformed-graph limit. |
| [Unit movement/cargo/loss](../../src/auto/unit.cpp), maximum HP/remaining HP/movement `005b29aa/005b29d7/005b2a39`, `unit_gather_cargo`, combat loss `0057e9f9` | Damage-sensitive creation input, simplified-combat query mutation, cargo staging and saturating loss count. |
| [City operations](../../src/auto/city.cpp), reveal `0043cc00`, lookup `0043cf76`, creation `0043f8b0`, route removal `00440325`, deletion `004413d1` | City identity, tile-gated lookup, discovery memory, dependent-reference cleanup and route ordering. |
| [City specialist helpers](../../src/auto/city.cpp), setter/getter/count `004e7549/004e75a6/004e75ea` | First-16 storage, implicit entertainer query and bounded count behavior. |
| [City recalculation entry](../../src/game/retvals.cpp), `city_recalc` `004eb4ed`; [city workspace](../../src/data/state/city.h) | Shared workspace and non-full dispatch; no invented cache-key validation. |
| [Map accessors](../../src/auto/map.cpp), unit lookup `005b2e69`, tile/known lookup `005b8931/005b898b`, owner `005b8a1d`, memory update `005b8b1a`, owner queries `005b8ca6..005b8da4`, setters `005b94fc..005b9d81` | Tile-versus-record lookup, compound features, independent ownership/knowledge and preservation by setters. |
| [Civilization state](../../src/auto/civ.cpp), creation `004a7ce9`, advance predicate `004bd9f0`; [AI census](../../src/auto/ai.cpp), `ai_civ_turn_assess` `0053184d` | Directed/retained state, reset distinctions and non-pure census boundaries. |
| [Loading](../../src/auto/game.cpp), `save_read_units_cities` `004732a6` | Fresh tokens after current-format import, ordered by slot; format/failure details remain outside F02. |
| [Scenario/event operations](../../src/auto/game.cpp), parameters reset `00484cc0`, event reset/allocate `004fa5d9/004fa617`, actions `004fc3ae` | Separate scenario state, event order/lifetime and post-action one-shot marker. |
| [Network selections](../../src/auto/network.cpp) | Examples of retaining lifetime tokens to reject changed selections; not a universal token check on slot references. |

Supplementary source at the pinned `/home/peter/project/civ2` revision:
`src/unit_creation.cpp`, `src/unit_deletion.cpp`, `src/city_creation.cpp`,
`src/city_record_access.hpp`, `src/record_limits.hpp` and
`analysis/reference/bugfix-log.md`. These corroborate ordinary slot selection,
retained damage before simultaneous-movement initialization, deletion ordering,
and the distinction between ordinary capacity and backing. That reconstruction
also contains deliberate malformed-input checks and an unused-read repair;
their presence does not make them original invariants.

Relevant deviations are localized as follows:

- [B2](../BUGS.md#b2-a-new-game-in-the-same-session-keeps-the-human-players-place-in-the-city-name-list):
  the optional new-game name-cursor reset is excluded here.
- [B16](../BUGS.md#b16-a-city-grows-past-127-to-a-negative-size): the optional
  city-growth cap is excluded; the signed size transition remains observable.
- [X01's backing-state contract](X01-classic-compatibility-contract.md#3-original-behavior-and-reconstruction-choices)
  applies to unrecovered locals, invalid indices and reads outside the logical
  records. They require supplied evidence/input or an explicit bounded claim,
  not an invented cleanroom value.
- The save-record validation and bounds checks in current OpenCiv2 are always-on
  repairs. F02's ordinary state domain must not be read as a claim that the
  original loader rejected every state outside it.

Validation for this document passed **130 assertions** against source-extracted
operation bodies or bounded fragments: reservation gates, hole reuse, one-step
bound reduction, carrier-reference wakeup, token import, home decoding, coherent
stack relocation, city lookup/revelation, specialist access, city-workspace
dispatch, movement initialization and route removal. The harness used the
recovered record definitions; stack validation was a no-op for coherent inputs,
and full city-calculation children were stand-ins to check dispatch only. It did
not run complete creation/deletion, prompts or network commands. A separate
compile-time check verified **68 record-size/offset mappings**, and relative
documentation links, anchors and roadmap counts were checked.

The existing [unit-case inventory](../../tests/unit/cases.tsv) records cases for
unit creation, stack operations, city revelation and recalculation; those counts
are evidence inventory, not a claim that this change reran those recordings.
Full lifecycle effects, damaged-stack recovery, original-program execution,
callback interleavings, event reentrancy and network/save integration require
their owning specifications and conformance cases.
