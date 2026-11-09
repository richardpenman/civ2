# Algorithm specifications

This is the working roadmap for describing the entire classic Civilization II
Multiplayer Gold Edition game in enough detail for a cleanroom implementation.
Each specification must explain its behavior without requiring the reader to
inspect source code. The collection covers simulation, AI, scenarios, persistence,
multiplayer, and the algorithms behind the original player interface.

The backlog contains **65 specification entries: 1 written and 64 planned**.
The IDs below are stable references for tracking work and dependencies. Planned
filenames are shown as code; links lead to documents that already exist. Update
this index when a specification is added, split, or reviewed.

## Classic compatibility target

The target is **Civilization II Multiplayer Gold Edition 5.4.0f, patch 3**, with
the original game rules and supported custom rules and scenarios. Preserve its
observable arithmetic, order of operations, random draws, selection tie breaks,
state transitions, file formats, and implementation quirks. Difficulty, government,
human versus AI control, topology, and game mode are inputs wherever they affect
behavior.

Use OpenCiv2's classic paths as the starting evidence, together with original
program observations and the independent reconstruction in
`/home/peter/project/civ2`. Pin the source revision and original executable
version in each document. Source names and function boundaries are navigation
aids; the document boundaries below follow behavior.

`CIV2_CLASSIC=1` is useful for comparisons, but it is not proof of original
behavior in every state. [BUGS.md](../BUGS.md) records both selectable fixes and
changes present in every mode, including replacements for reads of uninitialized
locals. Each specification must identify relevant differences and distinguish an
original behavior from a reconstruction assumption. If a result depends on
unrecovered state, describe the required input or bounded observation rather than
inventing a deterministic original result.

Optional OpenCiv2 interface additions, rule changes, and improved behavior are
outside this roadmap. [EXTRAS.md](../EXTRAS.md) and the bug-fix notes are useful
for finding branches to separate from classic behavior; their improvements are
not normative requirements for these specifications.

“Entire game” includes observable UI and media behavior and the supported
multiplayer paths. It does not require reproducing compiler scaffolding, allocator
internals, every Win32 wrapper, platform packaging, or third-party codec internals.
Specify the externally visible contracts those services must satisfy. Record
unsupported or unreachable paths explicitly: for example, the presence of PBEM
code does not establish a playable PBEM mode in this MGE release
([status notes](../STATUS.md)).

## Status and completion

| State | Meaning |
| --- | --- |
| Planned | Scope reserved; no specification exists yet. |
| Draft | A document exists with identified unfinished sections or unresolved contracts. |
| Written | The scoped algorithm is described, with source traceability and explicit limits. Independent conformance is not implied. |
| Checked | The document has also been reviewed against its evidence and exercised by independent conformance cases; record exactly what those cases establish. |

The existing map-generation document is **Written**. It pins an earlier OpenCiv2
revision and includes arithmetic vectors; it has not been promoted here to a
claim of complete original-program conformance. Its classic-target review is part
of the first work batch below.

Every document should contain:

1. The compatibility target, scope, entry conditions, inputs, outputs, and
   observable side effects, including calls to other specified algorithms.
2. State and data definitions, units, coordinate systems, widths, signedness,
   rounding, overflow, sentinels, and rule-table dependencies.
3. Ordered pseudocode with constants, branch conditions, loop and scan order,
   tie breaking, random-stream selection, and exact random-call consumption.
4. Relevant mode and rule variations, boundary conditions, failure or cancellation
   behavior, and original quirks. Keep unresolved behavior explicit and local.
5. Small worked examples and conformance cases with complete initial state and
   expected state changes, return values, random state, and external effects.
6. A source-traceability appendix with revisions, function/address references,
   original observations, and the validation limits. Source access must be optional
   for the implementer reading the algorithm itself.

## Document backlog

### Foundations and game orchestration

Start with [game.cpp](../../src/auto/game.cpp),
[civ2types.h](../../src/include/civ2types.h),
[state declarations](../../src/data/state),
[rules.cpp](../../src/game/rules.cpp), [civ.cpp](../../src/auto/civ.cpp),
[random.cpp](../../src/game/random.cpp), [crt.cpp](../../src/native/crt.cpp), and
[retvals.cpp](../../src/game/retvals.cpp). The latter contains substantial save
loading logic; the main `src/auto` file is not always the complete implementation.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| F01 | Planned | `determinism-and-arithmetic.md` | Gameplay and secondary random generators, seeding, clock inputs, range helpers, integer and floating-point conventions, and replay input contracts. Individual algorithms own their draw order. |
| F02 | Planned | `game-state-and-invariants.md` | Game, civilization, city, unit, map, and scenario state; IDs, lifetimes, flags, sentinels, ownership, list invariants, cached versus authoritative values, and mutation contracts. File encoding belongs to F06. |
| F03 | Planned | `rules-and-configuration.md` | RULES.TXT parsing, section order, defaults, conversions, validation, cross-references, technology prerequisites, unit and terrain definitions, cosmic parameters, and classic game-option effects. Include custom-rule input limits. |
| F04 | Planned | `turn-sequencing.md` | Single-player session loop; global, civilization, city, and unit phase order; year progression, turn counters, expiry/reset timing, event hooks, autosave, and end checks. Calls the mechanics below without duplicating their formulas. |
| F05 | Planned | `game-setup-and-starting-positions.md` | New and premade worlds, tribe/leader selection, world parameter resolution, civilization allocation, starting advances and units, start-site search and relaxation, balancing, accelerated starts, and new-game resets. W02 owns terrain generation. |
| F06 | Planned | `save-load-and-file-formats.md` | Save, scenario, and map layouts; versions, sizes, byte order, record validation, strings, event storage, state restoration, mode adaptation, derived-state rebuilds, saved settings/window state, and partial-load failure behavior. |

Establish F01–F03 first. F04 can initially define the phase contract with named
calls into future documents; its final review follows the city, civilization,
event, and victory specifications. F05 uses W02. F06 uses F02–F03 and later
receives cross-checks from scenario and session work.

### World and environment

Start with [map.cpp](../../src/auto/map.cpp), [game.cpp](../../src/auto/game.cpp),
[unit.cpp](../../src/auto/unit.cpp), and
[retvals.cpp](../../src/game/retvals.cpp), including `unit_enter_hut` and
`city_process_pollution`.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| W01 | Planned | `map-geometry-and-spatial-state.md` | Doubled coordinates, parity aliasing, wrapping, distances, neighborhood tables, tile access, ownership and region queries, land/water labeling, border bits, resource/shield/hut predicates, and spatial cache invalidation. |
| W02 | Written | [Map generation](map-generation.md) | Land creation, terrain, rainfall, age operations, rivers, polar edits, region labeling, initial coarse connectivity, fertility, resources, and huts; ordered random draws and phase checks. |
| W03 | Planned | `visibility-and-exploration.md` | Per-civilization known terrain, tile and unit visibility, revelation radii and exceptions, contact discovery hooks, map sharing, city investigation/reveal, and what movement or ownership changes refresh. |
| W04 | Planned | `terrain-work-and-improvements.md` | Settler/engineer orders, eligibility, work accumulation and completion, irrigation sources, mining, transformation, roads, railways, fortresses, airbases, pollution cleanup, and pillaging. AI chooses jobs in A03. |
| W05 | Planned | `pollution-and-global-warming.md` | Pollution generation from city state, probability and tile selection, global accumulation and warming thresholds, terrain changes, recalculation/revelation, and cleanup interaction. Nuclear placement calls into this state. |
| W06 | Planned | `huts-and-barbarians.md` | Hut outcome selection and consumption, gold/advances/mercenaries/nomads/cities, uprising and sea-raid generation, placement constraints, barbarian leaders and ransom, and barbarian-specific behavior. Ordinary combat is U06. |

W01 depends on F01–F03. W03 uses W01 and unit identity from U01. W04 uses U01
and F03. W05 uses city outputs from C03 and C05. W06 uses F04, U01, and C01.
W02 already embeds several shared helpers: keep it self-contained until W01 and
U03 exist, then reconcile cross-references without silently changing its contract.

### Units and combat

Start with [unit.cpp](../../src/auto/unit.cpp), [map.cpp](../../src/auto/map.cpp),
[civ.cpp](../../src/auto/civ.cpp), and the unit/map state declarations. Examine
human command callers and AI callers of the same operation.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| U01 | Planned | `unit-lifecycle-and-stacks.md` | Unit allocation, creation/deletion, stack links and ordering, relocation, cargo ownership, home-city links, off-map units, visibility flags, and bookkeeping. Includes cascading removal and rehoming primitives. |
| U02 | Planned | `movement-and-zones-of-control.md` | Domain and terrain legality, movement allowance and costs, roads/railways/rivers, zones of control, occupied tiles, treaty restrictions and expulsion, entry dispatch, failed moves, and post-move effects. |
| U03 | Planned | `pathfinding-and-goto.md` | Local flood search, cost grids, caching, coarse connectivity and waypoints, directional tie breaks, fallback movement, route continuation/replanning, and differences between route estimates and actual move execution. |
| U04 | Planned | `unit-orders-and-activation.md` | Active-unit selection and cycling, wait/sleep/sentry/fortify/disband/home/goto/automation commands, order persistence, wake-up rules, turn resets, healing, and interrupted or exhausted orders. Delegates job and move execution. |
| U05 | Planned | `transport-and-special-movement.md` | Boarding, cargo gathering, unloading and landing, transport loss, air fuel and basing, trireme loss, airlift, paradrop, and exceptional movement checks and costs. Combat and diplomacy consequences are cross-references. |
| U06 | Planned | `combat-and-veterancy.md` | Attack eligibility, best-defender selection, strength modifiers, hit probability, hit points/firepower, damage loops, special unit interactions, retreat or survival where supported, stack casualties, promotion, and victory aftermath dispatch. |
| U07 | Planned | `nuclear-attacks.md` | Delivery and interception checks, blast footprint, unit and city damage, pollution, diplomatic effects, and the exact ordering of cascading changes. Spy delivery enters through V06; ordinary battles remain U06. |

Write U01 before U02 and U04; U02 before U03 and U05; and U06 before U07.
All use F01–F03 and W01. U06 hands city transfer to C09, and U07 hands pollution
state changes to W05. Describe those call boundaries even before the destination
document is written.

### Cities and trade

Start with [city.cpp](../../src/auto/city.cpp),
[civ.cpp](../../src/auto/civ.cpp), [unit.cpp](../../src/auto/unit.cpp), and
[retvals.cpp](../../src/game/retvals.cpp), including `city_recalc`. Separate
recalculation from the operations that commit a city's turn.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| C01 | Planned | `city-lifecycle-and-ownership.md` | Founding eligibility, city allocation and names, initial state, radius claims, lookup, deletion, ownership-transfer primitives, and trade/unit/map bookkeeping. Capture policy belongs to C09. |
| C02 | Planned | `city-tiles-and-worker-assignment.md` | Tile yields, specials and improvements, government modifiers, usable and blocked squares, shared radii, manual/default worker assignment, specialist allocation, and deterministic assignment tie breaks. |
| C03 | Planned | `city-economy-and-corruption.md` | Food/shield/trade totals, capital links, corruption and waste, tax/science/luxury splits, rounding and modifier order, route contributions, and the complete city-stat recalculation pipeline. |
| C04 | Planned | `city-happiness-and-disorder.md` | Initial citizen moods, difficulty and empire-size effects, specialists, martial law, luxury and building/wonder effects, military unhappiness, celebration and disorder thresholds, and stage-by-stage clamps. |
| C05 | Planned | `city-growth-and-support.md` | Food boxes, granaries, growth limits, starvation, celebration growth, settler food costs, government-dependent unit support, shortage handling, and unsupported-unit removal. Specifies when each change becomes visible. |
| C06 | Planned | `city-production-and-purchasing.md` | Build eligibility, production categories, shield accumulation, item switching and penalties, rush-buy prices, completion, repeat/default orders, capitalization, selling improvements, wonder races, and unit-placement failures. |
| C07 | Planned | `trade-and-commodities.md` | Commodity supply/demand and refresh timing, caravan/freight selection and delivery, revenue and research bonuses, route insertion/replacement, recurring yields, food routes, distance and road/sea connections, and ownership effects. |
| C08 | Planned | `buildings-and-wonders.md` | Improvement/wonder eligibility, acquisition, obsolescence, maintenance, uniqueness and effect scope. Provides an effect-to-consumer catalog linking each bonus to the document that owns its formula. |
| C09 | Planned | `city-capture-and-revolt.md` | Combat capture and incited transfer, population/building losses, plunder, palace relocation, wonder control, rehoming, partisans, destruction, civilization-elimination/civil-war checks, and event/notification order. |

Write C01–C02 before the C03–C06 calculation/turn sequence. C07 feeds C03;
C08 supplies an effect catalog that must be checked against all consumers.
C09 follows U06 and C01 and shares transfer primitives with V01 and V06.
F04 owns the order in which the complete city processes run.

### Civilizations and game outcomes

Start with [civ.cpp](../../src/auto/civ.cpp),
[diplomacy.cpp](../../src/auto/diplomacy.cpp),
[spaceship.cpp](../../src/auto/spaceship.cpp),
[unit.cpp](../../src/auto/unit.cpp), [game.cpp](../../src/auto/game.cpp), and
[reports.cpp](../../src/auto/reports.cpp).

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| V01 | Planned | `civilization-lifecycle-and-civil-war.md` | Civilization creation/reset, elimination and revival where supported, leader assignment, survival checks, civil-war eligibility and partition, transfers, and destroyed-civilization records. F05 owns initial setup orchestration. |
| V02 | Planned | `governments-and-tax-rates.md` | Government availability, revolution/anarchy and transitions, rate caps and balancing, civilization treasury/upkeep settlement, and government effects on other systems. City-level consumption is owned by C03–C05. |
| V03 | Planned | `research-and-technology.md` | Advance availability, prerequisite and research-goal traversal, cost calculation, beaker accumulation and completion, technology grants/theft/exchange, first-discovery effects, obsolescence and upgrades, and research-selection rules. |
| V04 | Planned | `diplomacy-and-treaties.md` | Contact, attitude and reputation state, peace/ceasefire/alliance/war transitions, expiry, treaty violations, senate constraints, shared information, expulsion, and alliance obligations. |
| V05 | Planned | `diplomatic-negotiation.md` | Audience/session flow, offers and demands, gifts, tribute, technology exchange, map sharing, joint wars, acceptance/refusal effects, patience, and human/AI or human/human branches. A08 owns AI valuations and policy. |
| V06 | Planned | `espionage-and-bribery.md` | Diplomat/spy mission eligibility and menus, embassy/investigation, technology theft, sabotage, poisoning, nuclear planting, bribery and incitement prices, success/capture/escape, veteran effects, and incidents. |
| V07 | Planned | `spaceship.md` | Part groups and dependencies, construction limits, assembly, mass/thrust/life-support/energy/success calculations, launch, travel/arrival, loss, and race state. Production belongs to C06; victory checks to V08. |
| V08 | Planned | `scoring-victory-and-retirement.md` | Score and rating components, scenario objectives, conquest and space outcomes, retirement warnings and year rules, endgame ordering, continuation, and Hall of Fame qualification/storage rules. |

V02–V03 use F03 and city outputs. V04 precedes V05–V06; C09 and U07 receive
the consequences of particular espionage missions. V07 precedes V08. F04 must
ultimately agree with every per-turn and end-of-game hook in this group.

### Artificial intelligence

Start with [ai.cpp](../../src/auto/ai.cpp), the `city_choose_production` family in
[city.cpp](../../src/auto/city.cpp), [diplomacy.cpp](../../src/auto/diplomacy.cpp),
the civilization goal routines in [civ.cpp](../../src/auto/civ.cpp), and
`ai_choose_government` in [bigframes.cpp](../../src/game/bigframes.cpp).
AI descriptions must include the actual scores, priorities, scan order, random
choices, and fallbacks, not just strategic intent.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| A01 | Planned | `ai-strategy-and-assessment.md` | Civilization assessment, continent policies, power/threat estimates, role counts and quotas, goals and standing goals, priorities, leader/difficulty parameters, and per-turn orchestration. |
| A02 | Planned | `ai-city-production.md` | City surveys, defense needs, improvement/wonder/unit scoring, special-unit and spaceship choices, redundant sales, rush buying, and final production tie breaks and fallbacks. Uses C06 eligibility. |
| A03 | Planned | `ai-settlement-and-terrain-work.md` | City-site evaluation, expansion goals, settler/engineer job selection, road links, irrigation/mining/transformation priorities, fortress placement, travel and reassignment, including human automation. |
| A04 | Planned | `ai-ground-tactics.md` | Land-unit target and move scoring, exploration, attack/defense, garrisons, pillage, partisan behavior, healing/retreat, trespass response, persistent goto, and exhausted/no-action outcomes. |
| A05 | Planned | `ai-naval-and-transport.md` | Ship goals, embarkation and cargo assignment, port and landing selection, overseas raids, coastal defense, unloading, and coordination with carried units and land goals. |
| A06 | Planned | `ai-air-and-nuclear.md` | Air basing, target selection and valuation, city raids and strikes, range/fuel constraints, nuclear targeting, and coordination with movement/combat rules. |
| A07 | Planned | `ai-special-unit-missions.md` | Caravan routes and deliveries, diplomat/spy target and mission selection, paratrooper objectives, and the specialized dispatch within unit-order selection. Barbarian-specific policy is W06. |
| A08 | Planned | `ai-research-government-and-diplomacy.md` | Technology valuation and selection, government choice, diplomatic stance and security assessments, offer/demand pricing, alliance and war decisions, aid, relation reviews, and spaceship-race influence. |

A01 is the common prerequisite. Draft each specialized AI document after the
corresponding world, unit, city, or diplomacy mechanics. Keep the orchestration
and shared scoring terms in A01 and give each specialized decision one owner.

### Scenarios and editors

Start with `ScenarioEvents` and scenario/cheat entry points in
[game.cpp](../../src/auto/game.cpp), [editor.cpp](../../src/auto/editor.cpp), the
editors in [civ.cpp](../../src/auto/civ.cpp), [city.cpp](../../src/auto/city.cpp),
[unit.cpp](../../src/auto/unit.cpp), [map.cpp](../../src/auto/map.cpp), and the
sprite/sound editors in [graphics.cpp](../../src/auto/graphics.cpp) and
[sound.cpp](../../src/auto/sound.cpp).

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| S01 | Planned | `scenario-loading-and-parameters.md` | Scenario selection, parameter overrides, date/calendar and objective settings, human-civilization selection, asset/rule overrides, scenario reset/setup, and the scenario-loaded event boundary. |
| S02 | Planned | `event-language-and-execution.md` | Event-file grammar and parsing, storage and references, all supported triggers and actions, matching order, random-turn checks, one-shot/persistent behavior, action ordering, event hooks, and save/reload continuation. |
| S03 | Planned | `world-and-scenario-editors.md` | Classic cheat/editor commands that mutate map, cities, units, civilizations, money, visibility, victory, and scenario state; validation, cancellation, derived-state repairs, and scenario/event writing. |
| S04 | Planned | `rules-and-asset-editors.md` | Terrain/unit/building/technology/tribe/cosmic rule edits, references and rename propagation, sprite and sound edits, preview/apply/cancel behavior, and text/asset round trips. Parsing semantics remain F03. |

F03 and F06 precede this group. S02 must be cross-checked at its F04, U06, C09,
V01, V03, and V05 call sites. S03–S04 use existing mutation and parser contracts;
they must specify editor exceptions rather than assuming normal-play validation.

### Multiplayer and local sessions

Start with [network.cpp](../../src/auto/network.cpp), the host/client loops in
[game.cpp](../../src/auto/game.cpp), [bigframes.cpp](../../src/game/bigframes.cpp),
and [xdaemon.c](../../src/platform/xdaemon.c). Use the existing
[XDaemon protocol document](../xdaemon-protocol.md) as evidence for N05;
reconcile and link or relocate that material when N05 is written.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| N01 | Planned | `multiplayer-session-and-lobby.md` | Discovery/join/host setup, player slots and civilization assignment, passwords, scenario/world negotiation, startup waits, disconnects, lost-player handling, and host handoff. |
| N02 | Planned | `network-commands-and-state-synchronization.md` | Game message schema and dispatch, validation, authority, command routing, busy/lock rules, batching, state sections, checksums/diffs, resynchronization, and join/load state transfer. |
| N03 | Planned | `multiplayer-turn-sequencing.md` | Host/client phase state machines, AI versus local/remote human turns, production/movement barriers, simultaneous movement options, timers/timeouts, completion signals, and RNG ownership. Uses F04's simulation phases. |
| N04 | Planned | `hotseat-and-player-handoff.md` | Local multiplayer player masks, passwords, turn transitions, concealed views and knowledge, per-player cursor/window state, save/load adaptation, and end/session transitions. Record unreachable e-mail paths separately. |
| N05 | Planned | `transport-and-wire-protocol.md` | Classic XDaemon TCP framing, UDP discovery, IDs/routing, handshake, buffers, callback ordering, errors and shutdown. Inventory IPX/SPX, modem, serial, and lobby transport contracts, distinguishing original support, reconstruction support, and evidence gaps. |

N05 supplies transport contracts; N01–N02 supply session and command contracts;
N03 joins them to F04. N04 depends on F04, F06, and W03 and can be written
independently of networking. Preserve the distinction between local hotseat and
network multiplayer throughout all mode-dependent algorithms.

### Presentation and player interaction

Start with [mapview.cpp](../../src/auto/mapview.cpp),
[graphics.cpp](../../src/auto/graphics.cpp), [text.cpp](../../src/auto/text.cpp),
[window.cpp](../../src/auto/window.cpp), [controls.cpp](../../src/auto/controls.cpp),
[city.cpp](../../src/auto/city.cpp), [reports.cpp](../../src/auto/reports.cpp),
[pedia.cpp](../../src/auto/pedia.cpp), [sound.cpp](../../src/auto/sound.cpp), and
[throneroom.cpp](../../src/game/throneroom.cpp). Consult the platform adapters
for observable timing, rasterization, and input behavior where needed.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| P01 | Planned | `map-projection-and-interaction.md` | Tile/screen transforms, zoom, clipping, wrapping views, centering and scrolling, hit testing, cursor movement, selection, drag behavior, and multiwindow view coordinates. |
| P02 | Planned | `map-rendering-and-minimap.md` | Terrain/coast/river/road composition, resources, cities, units and stack selection, fog and explored edges, labels, blink/animation layers, minimap colors and viewport rectangles, and redraw order. |
| P03 | Planned | `graphics-and-asset-processing.md` | Original resource lookup and asset overrides, sheet extraction and hotspot metadata, palettes and transparency, sprite scaling, clipping and blitting, surface composition, and image-format behavior visible to the game. |
| P04 | Planned | `text-and-dialogs.md` | Label/text-section parsing, variable substitution, numeric/year/name formatting, wrapping and measurement, dialog layout, lists and controls, modal results, keyboard defaults, and cancellation semantics. |
| P05 | Planned | `input-and-command-dispatch.md` | Original menus, keyboard/mouse dispatch, view/move modes, active-unit and city command routing, enable/disable conditions, modal input gating, queued input, and timer/message ordering relevant to behavior. |
| P06 | Planned | `city-interface.md` | City resource and citizen displays, worker/specialist interactions, production and purchase flows, supported/present units, city cycling, city-view composition, refresh timing, and close/delete/reentrancy behavior. |
| P07 | Planned | `advisors-reports-and-history.md` | Advisor aggregates and lists, demographics, power/history graphs, top cities, intelligence and casualty reports, score/endgame displays, sorting/ties, visibility filters, and refresh behavior. Simulation formulas remain with their owners. |
| P08 | Planned | `civilopedia-and-technology-tree.md` | Entry construction from rules/text, lookup and navigation, hyperlinks, prerequisite/tree layout, history/description views, and classic rule-derived statistics. |
| P09 | Planned | `council-and-throne-room.md` | Advisor-opinion and clip selection, presentation sequencing, throne-room qualification and piece upgrades, layout, interaction, and persistent display state. |
| P10 | Planned | `audio-video-and-animation.md` | Effect and music selection, spatial positioning, mixing/resampling contracts, stream scheduling, video ranges and callbacks, map/combat/nuclear animations, skip/cancel behavior, and timing. Separate gameplay effects from presentation completion. |

P03–P04 define shared presentation primitives. P01 uses W01; P02 adds W03;
P05 routes into U04 and city/civilization commands. P06–P09 consume the relevant
mechanics and shared presentation contracts. P10 must identify any random draws
or blocking callbacks that affect replay order rather than treating all media
as behaviorally inert.

### Compatibility and whole-game conformance

These two supporting documents define how the algorithm collection is used and
checked. They do not replace the subsystem specifications.

| ID | State | Document | Scope |
| --- | --- | --- | --- |
| X01 | Planned | `classic-compatibility-contract.md` | Exact target build, supported inputs and game modes, observable equivalence, original quirks versus reconstruction assumptions, unavoidable deviations, and a cross-reference to each affected algorithm. Use BUGS.md to locate differences. |
| X02 | Planned | `whole-game-conformance.md` | Fixture/state format, random and external-input capture, phase snapshots, integration scenarios, expected outputs, source/function coverage ownership, and explicit validation gaps across the complete game. |

Define X01 and the fixture contract of X02 early. Expand X02 as specifications
gain conformance cases; complete its coverage audit after all document groups
have been reconciled.

## Writing order and first work batch

Work through this sequence, taking one bounded specification at a time. A document
can name a downstream contract before that document exists, but its assumptions
must be tracked and reconciled when the downstream specification is written.

| Stage | Deliverable | Main prerequisite |
| --- | --- | --- |
| 1 | Classic target, arithmetic/state/rules contracts, map geometry, a turn-phase outline, and a map-generation compatibility review. | Existing specification, classic source paths, and original evidence. |
| 2 | Setup, save/load, visibility, unit lifecycle, movement/orders/pathfinding, terrain work, and basic city lifecycle/yields. | Stage 1 contracts. |
| 3 | Complete city economy and turns, trade, government/research, combat/capture, diplomacy/espionage, environment/barbarians, spaceship and victory. | Stage 2 state and mutation contracts. |
| 4 | Strategic AI and each specialized decision family, with exact scores and choices. | Corresponding mechanics from stages 2–3. |
| 5 | Scenarios/events/editors, transport, multiplayer session/sync/turns, and hotseat. | Persistence, phase order, and affected mutation contracts. |
| 6 | Complete classic presentation and interaction, then reconcile whole-game conformance and remaining coverage. | Shared presentation primitives and simulation outputs. |

Scenario hooks, network authority, and presentation side effects should be noted
when a mechanic is first documented; their full documents need not wait to begin
until stage 5 or 6. P03–P04 and N05 can proceed independently once their input
contracts are established.

The next concrete work items are:

- [ ] Write X01, `classic-compatibility-contract.md`, and establish the X02
  fixture format and evidence conventions.
- [ ] Review W02 against the classic target; record its source revision, covered
  cases, and any differences from current classic paths without changing its
  pinned behavior silently.
- [ ] Write F01, `determinism-and-arithmetic.md`.
- [ ] Write F02, `game-state-and-invariants.md`.
- [ ] Write F03, `rules-and-configuration.md`.
- [ ] Write W01, `map-geometry-and-spatial-state.md`.
- [ ] Draft F04, `turn-sequencing.md`, with an explicit ordered call graph and
  unresolved downstream contracts.

## Ownership and coverage rules

Each formula or state transition has one primary owner. Other documents specify
its call conditions, inputs, outputs, and ordering, then link to it. In particular:

- F01 owns random-generator definitions; every consuming algorithm owns its
  exact draw sequence, including discarded draws.
- F04 and N03 own phase ordering; city/civilization/unit documents own the work
  performed in each phase.
- W01 owns shared geometry and coordinate predicates; U03 owns path search and
  coarse movement connectivity; W02 owns generation's orchestration and snapshots.
- C03 owns recalculated city totals, C04 moods, C05 growth/support, C06 production,
  and C07 route/commodity formulas. C08 catalogs effects and links to their consumers.
- U06 owns combat resolution; C09 owns capture consequences; V01 owns elimination
  and civil war. V06 invokes these contracts for espionage outcomes.
- V04–V06 own diplomatic/mission state transitions; A08 and A07 own AI choices
  and valuations that select them. S02 owns event matching and action ordering,
  while action effects reuse their normal mutation contracts or state an exception.
- F06 owns persistent byte formats; N02 owns game message/state-diff formats;
  N05 owns transport framing. UI documents own presentation and input translation,
  and link to the mechanics an accepted command invokes.

Use [prototypes](../../src/auto/protos.h),
[function names](../../re/function_names.tsv), source entry points and their
callers, plus [state declarations](../../src/data/state), to audit coverage.
Include hand-reconstructed functions in `src/game`; the `src/auto` file list
alone is insufficient. Source navigation may evolve, so preserve original entry
addresses and the inspected revision alongside current names.

Maintain a function-family-to-document ownership table in X02 as work proceeds.
For every reachable classic behavior, assign an owner or an explicit disposition
as a platform contract, delegated library operation, or unsupported/unreachable
path. A source file appearing in the navigation notes above is not evidence that
all of its behavior has been specified.

## Validation plan

Each algorithm needs focused cases that check outputs and side effects at its
boundary, followed by integration cases checking the order in which systems
interact. Reuse the [recorded unit-case inventory](../../tests/unit/cases.tsv),
[replay scenarios](../../harness/scripts), and the
[development testing notes](../DEVELOPMENT.md) as evidence and starting inputs.
Recorded coverage and successful replays have finite scope; do not promote them
to exhaustive conformance claims.

The whole-game case set should include:

- New and premade worlds, cylindrical and flat topology, odd custom heights,
  starting-position search, accelerated starts, and a second game in one process.
- A complete human and AI turn including growth, production, support, happiness,
  trade, research, diplomacy, pollution, and order execution.
- Land/sea/air movement, cargo, failed orders, combat and stack losses, city
  capture, partisans, bribery/incitement, civilization elimination and civil war.
- Technology and wonder transitions, government changes, global warming,
  hut/barbarian outcomes, spaceship arrival, conquest, and retirement.
- Save/reload at meaningful phase boundaries, custom rules and scenario events,
  editor changes surviving serialization, and rejected invalid inputs.
- Hotseat concealment and handoff; network join, authoritative commands,
  synchronization, turn barriers, timeouts, disconnects, and host transfer.
- Original map/city/advisor/pedia interactions, modal cancellation, redraws,
  media selection/completion, and presentation's random/timing side effects.

Within each applicable case family, vary difficulty, government, human/AI control,
game mode, custom-rule inputs, and relevant limits. Compare state mutations,
random-call trace and final state, ordered notifications/commands, serialized
bytes, and presentation outputs appropriate to the contract. Use an independent
implementation or original-program observations for expected behavior; a check
that only repeats the specification's pseudocode is not sufficient evidence.
