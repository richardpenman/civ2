# Map generation algorithm specification

**ID:** W02. **Status:** Written.

This specification describes the map produced by OpenCiv2's `map_generate`,
including the order of its random draws and its observable implementation quirks.
It is intended to let an implementer reproduce the algorithm without reading the
implementation. The normative procedures below use abstract arrays and integer
operations; they do not require the original classes, memory addresses, or UI.

The compatibility target is the implementation at commit
`1ff87e33fbec8b7be8c4bf98c77709e16aad4f31`. Source references and corroborating
material are collected at the end. “Must” means necessary for matching this target,
including where a more conventional algorithm would produce a different result.

Shared numeric operations, random primitives and clock-to-seed conversion are
specified in [F01: Determinism and arithmetic](determinism-and-arithmetic.md).
Those definitions agree with the helpers at this document's pinned revision;
their extraction does not change its generation behavior. The
[classic compatibility contract (X01)](classic-compatibility-contract.md)
defines evidence and equivalence claims; a complete review of this pinned
generation implementation against the original program remains separate work.

## Scope and entry contract

Generation begins with map dimensions and world parameters already selected.
It ends after terrain, rivers, polar terrain, region identifiers, coarse movement
connectivity, fertility, and resource suppression have been computed. Resources,
grassland shields, and huts are coordinate predicates, specified below; there is
no random resource-placement or hut-placement pass.

Civilization selection, starting settler placement, starting technologies,
initial AI activity, and subsequent terrain changes are outside this operation.
In particular, this operation clears start-position hints but does not choose
replacement starting positions. A save made after new-game setup includes more
than the output of this algorithm.

The ordinary compatibility domain is a successful single-player generation with
stable inputs, valid terrain codes, successfully allocated storage, and no UI or
network callback changing game state during generation. The implementation pumps
messages at many points; the abstract algorithm assumes these calls neither
consume gameplay random numbers nor change its inputs. UI rendering, allocator
failures, corrupt saves, and callback-driven mutation are not specified here.

### Inputs

| Symbol | Meaning |
| --- | --- |
| `W` | Width in doubled horizontal coordinates; even. There are `W/2` tiles per row. |
| `H` | Number of rows. |
| `flat` | Whether game-rules bit `0x8000` is set. False means horizontal wrapping; there is never vertical wrapping. |
| `L` | Land mass: `-1`, `0`, or `1`, from lower to higher land target. |
| `F` | Land form: `-1`, `0`, or `1`; controls overlap and narrow versus wide walks. |
| `C` | Climate: `-1`, `0`, or `1`, from drier to wetter. |
| `T` | Temperature: `-1`, `0`, or `1`, from colder to warmer. |
| `A` | Age: `-1`, `0`, or `1`; selects the counts of weathering and voting operations. |
| `keep` | Zero generates a new map. Any nonzero value runs only common finalization on an existing map. |
| `S` | Full state of F01's primary random generator **at entry**. |
| `ticks` | Already converted game ticks used by F01's `mapPatternSeedFromTicks` on a new map; not raw milliseconds. |
| `wideLayout` | Existing `map_layout_wide` setting; nonzero skips distant-ocean resource suppression. It is independent of `F`. |
| `rules[0..21]` | Terrain food, shields, trade, mining target, and irrigation target, used for fertility. |

Ordinary size presets are `(W,H) = (80,50)`, `(100,80)`, and `(150,120)`,
containing 2,000, 4,000, and 9,000 tiles. The custom-size dialog accepts tile-row
width `U` and height `H` with `20 <= U,H <= 250` and
`1000 <= U*H <= 10000`, then passes `W = 2*U`. Height may be odd.
These limits are caller validation, not checks performed by the generator.

For a retained map, supply its tile bytes, resource seed, allocations, and
metadata storage; no new map is allocated. The coarse-connectivity calculation
also uses the existing pathfinding cache and unit-rule definitions described in
its section. A reproducible fixture must specify those inputs too if it compares
the coarse grids.

The random-world setup preceding this operation ordinarily draws parameters in
this order:

```text
L = R() % 3 - 1
F = 0
if R() % 3 == 0: F = L
T = R() % 3 - 1
C = R() % 3 - 1
A = R() % 3 - 1
```

Do not repeat these five draws when `S` is already the state at generator entry.
The custom-world dialog can resolve parameters individually and in a different
order. Supplying resolved parameters and entry state avoids dependence on UI
history.

### Outputs and storage

Let `U = W/2` and `N = U*H`. Tiles are stored by increasing row, then increasing
column. A tile has the following six unsigned bytes, in this order when serialized:

| Byte | Final meaning | Temporary use during generation |
| --- | --- | --- |
| `terrain` | Low nibble is terrain; `0x80` is river; `0x40` suppresses a special resource. | Base terrain and tentative river edits. |
| `improvements` | Improvements; zero on return. | Number of landmass passes covering the tile, modulo 256. |
| `radius` | City-radius state. | Initialized to zero on a new map. |
| `region` | Land or water region ID, interpreted according to terrain. | Reassigned during region labeling. |
| `visible` | Visibility bits. | Initialized to zero on a new map. |
| `owner` | High nibble `15` means no owner; low nibble is fertility. | Land-pass marker, then full terrain backup during rivers. |

The implementation returns no explicit result. Its outputs are mutated tile
storage, map header, 64 region records, coarse grids, random state `S`, and
effective parameters. Other observable map state includes seven zeroed known-map
arrays on new allocation, each with `N` bytes, and an off-map sentinel copied from
tile zero at the end. Only coarse grids 0 and 1 are built here; allocated grids 2
and 3 have no defined generated contents in this specification.

Each region record includes signed 16-bit land count, fertile-tile count, and
water count, plus an eight-byte border bitset. Land IDs and water IDs occupy
separate logical namespaces even though they share each numeric record index.

### Terrain codes

| Code | Terrain | Code | Terrain |
| --- | --- | --- | --- |
| 0 | Desert | 6 | Tundra |
| 1 | Plains | 7 | Glacier |
| 2 | Grassland | 8 | Swamp |
| 3 | Forest | 9 | Jungle |
| 4 | Hills | 10 | Ocean |
| 5 | Mountains | | |

`base(tile)` means `tile.terrain & 15`. `ocean(tile)` means `base(tile) == 10`.
Where a procedure says **full byte**, do not mask terrain flags before comparing.

## Arithmetic and geometry

Use [F01's numeric model](determinism-and-arithmetic.md#1-numeric-model-and-notation)
for widths, signedness, division, shifts, overflow and narrow stores. Ordinary
scalar calculations here use its default `i32` model unless otherwise stated;
most do not overflow within the valid dimension and parameter domain. `clamp`
below means F01's `clampAB` helper.

### Random draws

`R()` means F01's [primaryNext](determinism-and-arithmetic.md#3-primary-generator),
operating on shared state `S`. Map generation does not use the secondary stream.
The map-specific guarded helper is:

```text
pick(n):
    if n <= 1: return 0                 # consumes no draw
    return R() % n
```

Use `pick` only where stated. Other expressions such as `R()%2` call the primary
generator directly, with F01's draw-consumption and modulo semantics.

On a new map, set `P = mapPatternSeedFromTicks(ticks)` using
[F01's map-pattern conversion](determinism-and-arithmetic.md#game-ticks).
A retained map keeps its existing `P`. The generation phases below own the order
of random draws and map edits; F01 owns the shared primitives they invoke.

### Coordinates and access

Canonical tile coordinates satisfy `0 <= y < H`, `0 <= x < W`, and
`x % 2 == y % 2`. The canonical row scan is:

```text
for y = 0 .. H-1:
    for x = (y & 1), (y & 1)+2, ... while x < W:
        visit tile(x,y)
```

However, the accessor itself does **not** validate parity. For any in-bounds
coordinate it returns index `y*U + floor(x/2)`. Thus `(8,y)` and `(9,y)` refer to
the same tile. Land-walk starting coordinates deliberately need not have canonical
parity; preserve this aliasing.

`inBounds(x,y)` checks only the rectangular bounds. An out-of-bounds access
returns a shared mutable six-byte sentinel, not an ocean tile and not an error.
Its entry contents are external state; a fresh process starts with zero bytes.
New map allocation does not reset it. Algorithms below generally check bounds,
but land marking can write its scratch byte after an insufficient horizontal wrap.

```text
wrap(x):
    if not flat:
        if x < 0: x += W
        else if x >= W: x -= W
    return x
```

This corrects at most one circumference. It is not mathematical modulo. Walk
positions are allowed to accumulate outside the normal x interval.

### Ordered neighbor tables

These tables are constants. Their order affects walks, voting, merging, and
random-independent tie breaking.

| Direction `d` | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `DX[d]` | 1 | 2 | 1 | 0 | -1 | -2 | -1 | 0 |
| `DY[d]` | -1 | 0 | 1 | 2 | 1 | 0 | -1 | -2 |

The cardinal walk table is `DX,DY` at indices `1,3,5,7`, in that order.
The diagonal walk and river table `Q` uses indices `0,2,4,6`:
`[(1,-1), (1,1), (-1,1), (-1,-1)]`.

The 21-position city neighborhood `B` is:

```text
index:  0       1      2      3      4       5       6       7
B:     (1,-1) (2,0)  (1,1)  (0,2)  (-1,1) (-2,0)  (-1,-1) (0,-2)
index:  8       9      10      11      12      13      14     15
B:     (2,-2) (2,2)  (-2,2)  (-2,-2) (1,-3)  (3,-1)  (3,1) (1,3)
index:  16      17      18       19      20
B:     (-1,3)  (-3,1)  (-3,-1)  (-1,-3) (0,0)
```

Always wrap the neighbor's x and then test rectangular bounds, unless a procedure
explicitly says otherwise. The last position is the center; “20 neighbors” means
indices 0 through 19 only.

## Overall sequence

```text
if keep == 0:
    clear 21 start-x and 21 start-y hints to -1
    P = mapPatternSeedFromTicks(ticks)  # F01; no random draw
    if flat: C *= 2; T *= 2            # mutate the effective parameters
    allocate and initialize map
    choose land-generation bounds
    create main landmasses
    run preliminary region labeling
    add islets
    repair diagonal land contacts
    convert land-pass counts to base terrain
    run interleaved forward and reverse rainfall passes
    perform age-dependent weathering
    perform age-dependent terrain voting
    attempt rivers

if not flat: add provisional polar glaciers next to land
assign regions and region border bits
build coarse land and sea connectivity
calculate fertility
if not flat:
    overwrite both polar rows with glaciers
    if keep == 0: scatter tundra on four polar rows
clear all improvements
if not wideLayout: suppress isolated ocean resources
copy the first six-byte tile to the off-map sentinel
```

There is no final relabeling, connectivity rebuild, or fertility recalculation
after the polar overwrites and resource suppression. Derived metadata can therefore
describe the earlier terrain. Do not “repair” it in a compatible implementation.

## Land creation

### Initialization and target

Allocate `N` tiles, initializing terrain to 10 and every other byte to zero.
Set header area to `N`, flat field to `0x8000` or zero, and coarse dimensions to
`CW = floor((W+3)/4)`, `CH = floor((H+3)/4)`.

Set `placed = 0`, horizontal generation bounds `[3,W-3]`, and vertical bounds
`[0,H-1]`. Draw `R()%2`. Zero changes the upper bound to `H-5`; one changes the
lower bound to 4. Bounds are inclusive.

`generationBounds(x,y)` checks the vertical bounds, and checks the horizontal
bounds only for a flat world. It does not check normal rectangular bounds.

```text
scale = floor(N / 400)
positiveForm = max(F, 0)
target = trunc(scale * ((8*L + 8 + 8*positiveForm)*5 + 80)*8 / 10)
do:
    addLandmass(islet=false)
while placed < target
```

For `N=4000`, the targets for `L=-1,0,1` are respectively `640,960,1280` when
`F<=0`, and `960,1280,1600` when `F=1`. These are counts of marked tiles summed
over passes, including repeated coverage, not a requested final land area.
The target is quantized by `floor(N/400)`, and the last pass may overshoot it.

### One landmass pass

Clear every tile's `owner` byte to zero. Then select a starting coordinate:

```text
repeat:
    x = 8 + pick(W-16)                # x draw precedes y draw
    y = 4 + pick(H-8)
    if not islet:
        roll = pick(F+2)
        if roll != 0: break           # may start on existing land
    if tile(x,y).improvements == 0: break
```

There is no rejection limit. In particular, do not align `x` with `y` parity.
For `F=-1` the roll consumes no random number. For `F=0` it draws modulo 2;
for `F=1` it draws modulo 3.

For a main pass, run the narrow walk when `F<1`, otherwise the wide walk.
For an islet pass, first draw `r = R()%10`, then run a diagonal walk. Run a second
diagonal walk if `r>=7` and a third if `r>=9`. All start at the original selected
coordinate and share the same scratch markers. Each walk draws its own length.

Finally scan all `N` tiles. For each with nonzero `owner`, increment its
`improvements` byte modulo 256 and increment `placed` once. Multiple visits within
this pass count once; coverage in different passes counts repeatedly.

### Marking and walks

`mark(x,y)` first replaces x with `wrap(x)`. If `generationBounds(x,y)` succeeds,
set `tile(x,y).owner = 1`. `cluster(x,y)` calls `mark` at `(x,y)`, `(x+1,y-1)`,
and `(x+1,y+1)`, in that order.

For a narrow walk, draw `remaining = R()%64`. If the starting tile's
`improvements` is nonzero, halve `remaining` with integer division. Then add one.
For a wide walk, use `R()%48` and the same overlap adjustment.

```text
while remaining != 0 and generationBounds(x,y):
    cluster(x,y)
    if wide:
        if R()%4 == 0: cluster(x+2,y)
        if R()%4 == 0: cluster(x-2,y)
        if R()%4 == 0: cluster(x,y+2)
        if R()%4 == 0: cluster(x,y-2)
    d = 2*(R()%4) + 1
    x += DX[d]                       # do not wrap the walk position
    y += DY[d]
    remaining -= 1
```

All four wide-walk probability tests draw, even if their cluster would be out of
bounds. The direction draw occurs after marking, including on the last step.

A diagonal islet walk sets `remaining = R()%16 + 1`. Its loop uses the same
termination test, calls `mark(x,y)` once, draws `d = 2*(R()%4)`, advances by
`DX[d],DY[d]` without wrapping the walk position, and decrements `remaining`.

### Preliminary regions and islets

Run the region-labeling procedure specified below, then count region records
whose **land count** is nonzero. Let that count be `occupied`. Execute
`max(0,63-occupied)` islet passes.

At this point the new-map terrain bytes are all still ocean: the land walks have
only changed `improvements`. Consequently, in the ordinary new-map path
`occupied` is zero and **63 islet passes are performed**. Do not classify the
scratch coverage as land when doing this preliminary region calculation.

### Repairing diagonal contacts

This pass reads and writes coverage counts, not terrain. It scans and mutates its
loop coordinates as follows; the ordinary loop increments still run after each
body:

```text
y = 1
while y < H-2:
    x = y & 1
    while x < W-2:
        mask = (1 if count(x,y)       != 0 else 0)
             | (2 if count(x+1,y+1) != 0 else 0)
             | (4 if count(x+1,y-1) != 0 else 0)
             | (8 if count(x+2,y)   != 0 else 0)
        if mask == 6 or mask == 9:
            count(x+1,y+1) = 1
            count(x+1,y-1) = 1
            count(x+2,y) = 1
            if x > 1 and y > 1:
                x -= 1; y -= 1
            else if y > 1:
                x += 1; y -= 1
            else if x > 1:
                x -= 2
        x += 2
    y += 1
```

Here `count` means `improvements`. The assignments overwrite higher counts with
one, leave `(x,y)` untouched, and do not update `placed`. The pass neither wraps
its diamond across the seam nor uses a separate output buffer.

## Initial terrain

Set `bandSize = H/12` for cylindrical worlds or `H/8` for flat worlds, using
integer division. If `bandSize<3`, replace it with 2. Scan tiles in storage order.

Coverage zero becomes ocean, coverage two becomes hills, and coverage three
through 255 becomes mountains. Those cases consume no random draws.
For coverage exactly one:

```text
center = floor(H/2) + 8
a = R()%16
negativeSide = center - (y+a+1) <= 0
b = R()%16
latitude = center - (y+b+1)
if negativeSide: latitude = -latitude
latitude += (-6*T if flat else -3*T)
if latitude < 1: latitude = 0
band = latitude / bandSize
```

The first random draw chooses the sign; the second supplies the value to which
that sign is applied. This is not `abs(center-(y+random+1))`.

| Band | Result |
| --- | --- |
| 0 | Desert |
| 1, 2, 3 | Plains |
| 4 | Draw `R()%2`: zero gives plains, one tundra. |
| 5, 6 | Tundra |
| Any other value | Glacier |

Remember that `T` was already doubled on a new flat world: the effective shift
there is twelve times the negated original temperature parameter.

## Rainfall

Use two persistent tile cursors, `forward=0` and `reverse=N-1`. For each outer
iteration `r=0..H-1`, define:

```text
latitude = abs(floor(H/2) - r)
quarterDistance = abs(floor(H/4) - latitude)
cap = 4*C + 4 + quarterDistance
rain = R()%(cap+1) if cap > 0 else 0
```

First process exactly `U` tiles by incrementing `forward`. If the current full
terrain byte is ocean, increment `rain` if `rain<cap`. Otherwise, if rain was
positive at entry to the tile:

1. If `7-2*C > 1`, subtract `R()%(7-2*C)` from rain.
2. Apply the forward transition below, even if the subtraction made rain zero
   or negative.

| Terrain before forward visit | Terrain after visit | Further rain change |
| --- | --- | --- |
| Desert | Plains | 0 |
| Plains | Grassland | 0 |
| Hills | Forest | 0 |
| Mountains | Mountains | -3 |
| Tundra | Forest | 0 |
| Others | Unchanged | 0 |

Next set `rain=0` and process exactly `U` tiles by decrementing `reverse`, using
the **same** outer iteration's `latitude`. Ocean increases rain by one only if
`rain < floor(latitude/2)+C+1`. For a non-ocean tile with positive incoming rain:

1. If `5-2*C > 1`, subtract `R()%(5-2*C)` from rain.
2. Apply the reverse transition regardless of the new rain value.

| Terrain before reverse visit | Terrain after visit | Further rain change |
| --- | --- | --- |
| Desert | Plains | 0 |
| Plains | Grassland | 0 |
| Grassland | Jungle if `latitude < 3*H/10` and `<10`; swamp if only the first condition holds; forest otherwise. | -2 |
| Hills | Forest | 0 |
| Mountains | Mountains | -3 |
| Tundra | Plains | 0 |
| Swamp | Forest | 0 |
| Others | Unchanged | 0 |

`3*H/10` is integer division. During iteration `r`, the forward cursor visits
row `r` and the reverse cursor visits row `H-1-r`. They alternate row by row,
mutating the same array. It is incorrect to finish all forward rows before
starting the reverse pass, or to reset the reverse cursor to row `r`. In the
second half of the traversal each cursor sees changes made by the other cursor.

## Age operations

### Paired tile selection

Both age loops use this selection pattern with a fresh loop index beginning at
zero:

```text
if iteration is even:
    y = pick(H)
    x = 2*pick(U) + (y & 1)
else:
    d = R()%8
    x = wrap(x + DX[d])
    y += DY[d]
```

An odd iteration is one neighbor step from the preceding random tile, not a new
random tile. If it leaves the map, skip its terrain operation; the draws are
still consumed. The next even iteration replaces both coordinates.

### Weathering

Execute `(5*A+10)*160` iterations: 800, 1,600, or 2,400 for ages -1, 0, or 1.
For an in-bounds selected tile, use full terrain byte and
`latitude = abs(floor(H/2)-y)`:

| Before | After |
| --- | --- |
| Desert | Plains |
| Plains | Hills |
| Grassland | Forest |
| Forest | Jungle if `latitude < 3*H/10`, otherwise plains. |
| Hills | Mountains |
| Mountains | Try the lake operation below. |
| Tundra | Hills |
| Glacier | Mountains |
| Swamp | Grassland |
| Jungle | Swamp |
| Ocean | Unchanged |

A lake attempt succeeds only when `2<=x<W-2` and `2<=y<H-2`, and all four
tiles `(x-2,y)`, `(x+2,y)`, `(x,y-2)`, `(x,y+2)` are non-ocean. On success replace
the center's full terrain byte with ocean. There is no diagonal-neighbor test,
no wrapping in this test, and no extra random draw.

### Terrain voting

Execute `(1-A)*800` iterations: 1,600, 800, or zero. Use the same paired selection
pattern. On every even iteration reset `last=-1`.
Skip an out-of-bounds or ocean center. Also skip if its base terrain equals
`last` and `last>=0`. Otherwise save `high = (centerTerrain>2)` and initialize
eleven integer votes to zero.

Apply these initial biases, with all unspecified votes unchanged:

| Condition | Vote changes |
| --- | --- |
| `C==-1` | Desert +1; plains +1; swamp -1. |
| `C==1` | Grassland +1; swamp +1; also jungle +1 if `T>=0`. |
| `T==-1` | Tundra +1; forest +1; desert -1. |
| `T==1` | Plains +1; tundra -1; glacier -1; also grassland -1 and jungle -1 if `C==-1`. |

Climate conditions are mutually exclusive; temperature conditions are mutually
exclusive. Values `-2` and `2` on flat worlds match neither equality test.

Visit directions `d=0..7` in order. For each valid, non-ocean neighbor of base
terrain `t`:

```text
if t > 2 and high and votes[t] != 0: votes[t] += 1
if d is even: votes[t] += 1
votes[t] += 1
```

The first condition observes the running vote count, including negative biases
and previous neighbors; it is not a constant weight per terrain class.
Finally scan terrain codes 0 through 10 with `best=0`. Whenever
`votes[t]>best`, set `best=votes[t]`, overwrite the center's terrain with `t`, and
set `last=t`. Thus the smallest terrain code wins a positive tie. If no score
exceeds zero, neither the tile nor `last` is changed. Updates are immediate.

## Rivers

Set `accepted=0`, `attempts=0`. The outer loop runs at least once and stops after
an attempt when `attempts>=1024` or `accepted>=2*L+2*C+12`. Climate here is the
effective value, doubled for flat worlds.

At the start of each attempt, copy every full terrain byte into its tile's
`owner` byte. This is the attempt's immutable backup. Increment `attempts` and
set `length=0`.

Repeatedly choose `y=pick(H)` then `x=2*pick(U)+(y&1)` until the **full terrain
byte** is neither 5 nor 10. Save this coordinate as `(startX,startY)`, and take
the full terrain byte as `t`. Draw initial direction `d=R()%4`. A previously
river-marked tile is allowed as a start, even if its low nibble is mountains.

Each walk step performs this sequence:

1. Transform local `t` using the table below, comparing its full value.
2. Set `t |= 0x80`, store it to the current tile's terrain, and increment length.
3. Test the four `Q` neighbors in order for an in-bounds ocean tile. Set
   `reachedOcean` if any is found.
4. Always draw `R()%2`, including after finding ocean, and compute
   `d = (d + draw - (length&1)) & 3`.
5. Move to `(wrap(x+Q[d].x), y+Q[d].y)`.
6. If the new coordinate is in bounds, load `t` from that tile's **backup owner
   byte**, not its current terrain.
7. Continue only if no adjacent ocean was found, the new coordinate is in
   bounds, and `(t&0x80)==0`.

| Full `t` before the step | Transformation |
| --- | --- |
| 0 | Draw `r=R()%10`; become grassland if `r>=2` or pre-increment `length>3`, otherwise plains. The draw occurs even when length alone decides. |
| 1 | Draw `R()%10`; become grassland if result >=4, otherwise remain plains. |
| 3 | Draw `R()%10`; become grassland if result >=3, otherwise remain forest. |
| 4 or 5 | Grassland, without a draw. |
| 6 | Plains, without a draw. |
| 7 | Tundra, without a draw. |
| 8 | Draw `R()%10`; become jungle if result >=4, otherwise remain swamp. |
| Anything else | Unchanged, without a terrain-conversion draw. |

After termination, accept if
`(reachedOcean or (t&0x80)!=0) and length >= 5-trunc(attempts/800)`.
The minimum length is five for attempts 1–799 and four for attempts 800–1024.
If the last move left the map, step 6 did not reload `t`: it still contains the
river bit just set. Such an attempt can be accepted without reaching ocean or an
older river. The test is not a proof of a valid river mouth.

Acceptance increments `accepted` and keeps all edits. Then visit the 20 city
neighbors of the saved start. For every valid neighbor whose **base** terrain is
forest and whose `abs(floor(H/2)-y) < 3*H/10`, consume one `R()` and discard its
result. No tile changes in this post-acceptance loop.

Rejection restores all terrain bytes from `owner`. It does not restore random
state. The next attempt replaces the backup regardless of acceptance.

Because continuation reads the backup, crossing a tile already visited during
the current attempt does not detect a self-intersection. Previously accepted
rivers do stop a later attempt. There is no per-walk step limit or retry limit
for choosing a river start; adding either would change compatibility.

## Common finalization

These steps also run when `keep!=0`, which is therefore not a read-only path.

### Provisional poles

On a cylindrical world, for `x=0,2,...,W-2`:

```text
if not ocean(tile(x+1,1)):   tile(x,0).terrain = 7
if not ocean(tile(x,H-2)):   tile(x+1,H-1).terrain = 7
```

Then assign regions, build coarse connectivity, and calculate fertility, in that
order. These three procedures are defined below.

### Final poles

On a cylindrical world, overwrite the full terrain bytes at `(x,0)` and
`(x+1,H-1)` with glacier for every even x. This clears any river or suppression
flags on those tiles. On a new map only, repeat the following `floor(W/8)` times:

```text
tile(2*pick(U),   0).terrain   = 6
tile(2*pick(U)+1, 1).terrain   = 6
tile(2*pick(U)+1, H-1).terrain = 6
tile(2*pick(U),   H-2).terrain = 6
```

Each line draws independently; duplicates are allowed. Tundra can overwrite
ocean and any other terrain. On an odd-height custom map, the stated bottom-row
coordinates still go through the parity-aliasing accessor; do not change them.

### Improvements and resource suppression

Clear all `improvements` bytes. When `wideLayout==0`, scan in storage order.
For a tile whose **full byte equals 10** and whose resource predicate is nonzero,
look for an in-bounds non-ocean tile among its 20 city neighbors. If none exists,
set `terrain |= 0x40`. Use the terrain after final polar edits. River-flagged or
already-suppressed ocean is not reconsidered by the full-byte test.

Finally copy tile zero, all six bytes, to the shared off-map sentinel.
There are no random draws in clearing improvements, suppression, or this copy.

## Region labeling

A conventional connected-components algorithm is not sufficient to reproduce
the numeric IDs and counts. In particular, cylindrical scanning carries the
current run across row boundaries, water IDs have special aggregation rules,
and the overflow bucket's count is not recomputed from assigned tiles.

Clear land and water counts for records 0..63 and clear every tile's region byte.
Initialize `overflow=0`. Process ocean tiles first (`kind=1`), then land
(`kind=0`), independently. Each pass has a zeroed integer `count[0..32767]` and
zeroed temporary `label[0..N-1]` of unsigned 16-bit IDs.

### Provisional labels

Initialize `next=0`, `current=0`, `run=false`. Scan canonical coordinates in row
order. At the start of each row, set `run=false,current=0` **only if flat**.

If the tile's ocean predicate differs from `kind`, set `run=false,current=0`
and continue. Otherwise:

1. When `y!=0`, examine neighbor directions `6,7,0`, in that order. Wrap x and
   ignore out-of-bounds neighbors and zero temporary labels. These are
   `(-1,-1)`, `(0,-2)`, and `(1,-1)`, not all previously scanned neighbors.
2. For a nonzero neighbor label `other`, if `current` is nonzero and differs
   from `other`, merge the larger ID into the smaller. Add the larger count to
   the smaller, zero the larger count, and replace every occurrence of the
   larger label throughout rows 0 through y inclusive with the smaller ID.
   Include the whole current row in this replacement scan. Set `current` to
   the smaller. If no merge is needed, set `current=other`.
3. If `current==0`, and `run` is false, choose `next` by starting at 1 and
   advancing until `count[next]==0`, or until ID 32767 is reached. Set
   `current=next`. The valid size domain cannot exhaust that table.
4. Assign the current tile's temporary label to `current`, increment
   `count[current]`, and set `run=true`.

Cleared counts permit provisional ID reuse. On a cylindrical world, a run at
the end of one row can flow directly into the first matching tile of the next
row. Preserve that behavior even when a generic flood fill would differ.

### Small oceans

On the water pass only, process IDs 0 through 62. Whenever `count[id]<9`, add
that count to `count[63]`, set `count[id]=-1`, and remember that a merge occurred.
Then, if any merge occurred, scan **all** temporary labels: any label whose
count is negative becomes 63. Finally replace negative counts in slots 0..62
with zero.

This includes ID zero, whose count is zero. Thus land tiles, which were unlabeled
in the water pass, temporarily receive water label 63 too. The later land pass
overwrites them. Only provisional water IDs 0..62 undergo this particular
less-than-nine aggregation; higher provisional IDs follow the next rule.

### Publishing labels and overflow

Scan temporary labels in storage order. Ignore zero labels. For a label `id>63`:

```text
if count[id] <= 0:
    id = -count[id]                   # mapping to a previously assigned low ID
else:
    free = first positive index with count[free] == 0
    if free < 64 and (kind == 0 or count[id] > 16):
        count[free] = count[id]
        count[id] = -free
        id = free
    else:
        overflow |= 1 << kind
        id = 63
```

Write the resulting ID to the tile's region byte. On overflow, do not add the
overflowed component's count to slot 63 and do not record a negative remapping
for that component. Copy `count[0..63]`, truncated to signed 16 bits, into the
records' water counts for the first pass or land counts for the second.
The generator ignores the returned overflow flags, but a compatible helper can
return bit 0 for land overflow and bit 1 for water overflow.

### Border bits

After both passes, clear all eight border bytes of every record. Scan land tiles
in canonical order. Walk neighbor directions with this exact skip behavior:

```text
d = 0
while d < 8:
    q = (wrap(x+DX[d]), y+DY[d])
    if inBounds(q) and ocean(tile(q)):
        set bit tile(q).region in record[tile(x,y).region].borders
        d += 2 - (d & 1)
    d += 1
```

Bit `b` means byte `b/8`, mask `1<<(b&7)`. Do not check all eight neighbors after
finding water: the implementation skips two following directions after an even
hit or one after an odd hit. This can omit contacts that an exhaustive adjacency
calculation would include. Only the land record receives the water-border bit.

## Coarse connectivity

Coarse grids contain one byte per `CW*CH` cell in row-major storage. Build land
grid 0, then sea grid 1; clear each before building it. Set the pathfinder's
road-planning mode to one for this operation and restore it to zero afterward.

For each grid, visit cells with `cx` as the **outer** loop and `cy` as the inner
loop. A cell's base point is `(4*cx+1,4*cy+1)`. Its representative is the first
valid tile of the requested domain among `(baseX,baseY)` and
`(wrap(baseX+1),baseY+1)`, in that order. If neither qualifies, skip the cell.

The coarse direction table is
`[(0,-1),(1,-1),(1,0),(1,1),(0,1),(-1,1),(-1,0),(-1,-1)]`.
Check directions 0 through 3 only. For each, form the adjacent base point by
adding four times its offset, wrapping x once, and checking bounds. Find that
cell's representative in the same way. Continue only if the representatives have
the same region ID.

Call the bounded path calculation below between representatives with limit 5.
If its result is strictly between zero and five, set bit `d` in the current
coarse byte and bit `d+4` in the adjacent coarse byte. The adjacent cell index
uses a separate one-step horizontal wrap with circumference `CW`; check both
coarse coordinates before setting the reverse bit.

### Bounded path calculation

For ordinary new maps and standard unit domains, a freshly calculated result is
`1 + minimum number of tile moves`, using all eight tile directions within the
requested land/sea domain. The extra one comes from initializing the goal cost
to one. Therefore only routes of one, two, or three moves create an edge.
No terrain movement costs or road bonuses apply in road-planning mode.

For exact coarse-byte compatibility, also preserve these stateful details of the
shared path helper:

* A query chooses unit type 2 for land and 33 for sea. Its domain, not the query
  flag directly, determines whether ocean is passable (`domain==2`). With
  customized rules, supply those two unit domains. The source and destination
  themselves are allowed even when their terrain differs from that domain.
* Compute `distance=(wrappedAbsDx+absDy)/2`. A zero distance returns zero;
  distance >=23 returns -1, without a flood search.
* Let the cost-grid center be the new goal. Normalize the source's x relative to
  that goal: on a cylinder add W to a negative difference, then subtract W if
  the difference is at least `floor(W/2)`. Add the goal x back.
* The helper retains its previous flood goal and a 48 by 48 integer cost grid.
  Reset the query's result cost to zero. If the goal matches the retained goal
  and the normalized source's grid cost is nonzero, reuse the grid without
  resetting that result cost. Such a query cannot set a coarse edge because its
  returned result is zero or failure. Do not silently treat it as a new search.
* Otherwise clear the grid, remember the new goal, enqueue the goal with cost
  one, and initialize result cost to 5. Flood in FIFO order, expanding neighbors
  in direction order 0..7. Queue coordinates retain unwrapped x; tile lookups
  wrap x. Only admit neighbors with
  `((abs(unwrappedX-goalX)+abs(y-goalY)) & ~1) < 48` and valid map coordinates.
  An unvisited or lower-cost neighbor replaces its grid cost and is enqueued.
* When a dequeued point equals the source after wrapping x, lower result cost
  to that point's cost and do not expand it. Otherwise expand only if its cost
  is at most the current result cost. This still allows storing costs one more
  than the cutoff. Exhaust the queue; it has 2,304 entries, with the original
  also stopping on queue-head wrap or queue overflow. Those limits cannot be
  reached by an ordinary radius-five unit-cost query.
* A fresh ordinary search returns its result cost only if it is less than 5 and
  a valid neighboring grid point descends from the source toward the goal;
  otherwise it returns -1. Neighbors must pass the same domain/bounds/window
  checks, have a nonzero cost, and cost less than the source's grid cost.

Grid indices for an unwrapped point are
`col=(dx+dy)/2+24`, `row=(dx-dy)/2+24`, where `dx=x-goalX`, `dy=y-goalY`;
canonical parity makes these divisions exact. The underlying cost is
`grid[row*48+col]`. When testing descent, form each neighbor from the original
source coordinate, test the unwrapped window distance before normalizing it,
then normalize its x relative to the goal before fetching its cost. Keep that
order at the horizontal seam too.

Two additional path rules matter for unusual retained maps or customized unit
rules. If the selected unit is naval but the source is land, flood admission also
rejects candidate region 63 or a candidate region whose water count is one.
If the selected unit has flag `0x20` and a candidate's terrain has bit `0x40`,
the flood's otherwise unit edge cost gains `4*roadMoveMultiplier`. This addition
is applied even in road-planning mode; descent still uses a unit edge cost.
The helper disables ownership restrictions for these queries. No path operation
draws random numbers or changes map terrain.

For a standalone new-map implementation, initialize the incoming path cache as
empty. When comparing against an already-running game, capture its incoming
cache instead. Coarse cache behavior affects these derived bytes, not terrain,
region labels, fertility, or the random stream.

## Fertility

Clear the fertile-tile count of all 64 region records. Build 22 scores from the
loaded terrain rules. Treat food, shields, and trade as signed eight-bit values:

```text
for i = 0 .. 21:
    t = i % 11
    score[i] = 3*rules[i].food + rules[i].trade
    if t != 2: score[i] += 2*rules[i].shields
    if rules[t].mineTo == -2:
        score[i] += (3 if t == 4 else 1)
    else if rules[t].irrigateTo == -2:
        score[i] += 2
```

The byte representation of the improvement sentinel `-2` is `0xfe`. Mining's
condition takes precedence over irrigation's. Rules 0..10 are ordinary terrain;
11..21 are the first special-resource yield set. The second special-resource
yield set is not used here: both nonzero resource results select `t+11`.

Scan canonical center coordinates. If the center's base terrain is neither
plains nor grassland, set `owner=0xf0` and move on. Otherwise calculate `total=0`
over city-neighborhood indices 0..20. For each valid neighbor:

```text
t = base(neighbor)
value = 2 if t == 2 and grassShield(neighborCoordinate) else 0
if resource(neighborCoordinate) != 0 and index != 20: t += 11
value += score[t]
if neighbor.terrain & 0x80: value += 1

if index < 8:
    value *= 2
else if index == 20:
    value *= 4
    if neighbor.terrain & 0x80: value += value/2
total += value
```

Thus the center's river adds one before multiplication and also increases the
weighted center contribution by 50 percent. An out-of-bounds neighbor contributes
zero; do not substitute the off-map tile. If the grassland center lacks a shield,
subtract 16 from total. Then:

```text
q = clamp((total-120) >> 3, 1, 15)
fertility = (q >> 1) + 8
owner = 0xf0 | fertility
record[center.region].fertileTileCount += 1
```

The shift before clamping is arithmetic. Plains and grassland always receive
fertility in 8..15; all others receive zero. The region counter counts qualifying
tiles, not a sum of fertility scores. This happens before final polar edits and
distant-ocean resource suppression, so those later changes do not update it.

## Coordinate features

These predicates consume no random draws. They use the saved resource seed `P`,
not the gameplay state `S`.

### Grassland shield

For a canonical coordinate, a grassland tile has a shield if:

```text
(( -((x+y) >> 1) - x ) & 2) == 0
```

The helper itself does not check the terrain. Its callers decide whether to use
the result for grassland.

### Special resource

Return zero if the coordinate is out of bounds, terrain bit `0x40` is set, or
base terrain is grassland. Otherwise:

```text
u = (x+y) >> 1
v = x-u
cell = (u & 3) + 4*(v & 3)
pattern = (11*((x+y) >> 3) + 13*(v >> 2) + P) & 15
if cell != pattern: return 0
mask = 1 << ((P >> 4) & 3)
return 2 if (u & mask) == (v & mask) else 1
```

`v` can be negative. Its right shift must be arithmetic. Results 1 and 2 select
the first and second special-resource variants in consumers, although fertility
treats both as selecting the first variant's score.

### Hut

Return false if out of bounds, ocean, or the owner high nibble is not 15.
Otherwise use the same `u`, `v`, and `cell` and return:

```text
cell == ((11*((x+y) >> 3) + 13*(v >> 2) + P + 8) & 31)
```

The right side has five bits whereas `cell` has only four; values 16..31 do not
match. Resource suppression bit `0x40` does not suppress huts. Because fertility
sets the owner high nibble to 15, all generated land is initially eligible for
this coordinate test, including final polar terrain.

## Conformance checks

Use snapshots immediately after the named phases, before civilization setup.
A fixture should record dimensions, topology, the five original parameters,
entry `S`, clock-derived `P`, terrain rules, `wideLayout`, and retained-map or
path-cache inputs if applicable. Record random-call count and exit state in
addition to map bytes: a visually identical map can still desynchronize later
gameplay.

### Small deterministic vectors

Run [F01's primary vectors](determinism-and-arithmetic.md#primary-vectors) and
[tick/pattern-seed vectors](determinism-and-arithmetic.md#game-ticks) for the
shared primitives. The following checks cover map-specific behavior without
needing the game's UI or assets:

| Check | Expected result |
| --- | --- |
| `W=100,H=80` | `U=50,N=4000,CW=25,CH=20` |
| Access `(8,5)` and `(9,5)` for those dimensions | Both index 254. |
| Cylindrical `wrap(-1)`, `wrap(100)`, `wrap(201)` with `W=100` | `99,0,101`; the last is still out of bounds. |
| `grassShield(0,0)`, `(2,0)`, `(1,1)` | `true,true,false` |
| Unsuppressed desert at `(0,0)`, `P=16` | Resource 2; no hut. |
| Unsuppressed desert at `(2,0)`, `P=5` | Resource 2; no hut. |
| Unowned desert at `(0,0)`, `P=24` | Hut present; no resource. |
| Fertility with totals 0, 120, 136, 232, 1000 | `8,8,9,15,15` |

### Phase comparisons

For an independent implementation, compare at least these boundaries:

1. Each landmass pass: scratch markers, byte coverage counts, `placed`, random
   calls, and stream state. Include a wrong-parity start and overlapping passes.
2. Contact repair: coverage bytes and loop traversal; include masks 6 and 9
   near x/y boundaries and a tile whose count was greater than one.
3. Initial terrain and each paired rainfall iteration: full terrain bytes,
   cursors, moisture, and draws. Include even and odd heights and flat wet
   climate, whose reverse moisture subtraction consumes no draw.
4. Weathering and voting: check pair selection, failed lake attempts, ties,
   negative initial votes, running high-terrain bonuses, and no-vote outcomes.
5. Each river attempt: backup, full terrain, accepted count, length, and draws.
   Exercise acceptance, rollback, existing rivers, boundary exits, and the
   minimum-length change at attempt 800. Include discarded forest-neighbor draws.
6. Region labeling: separate IDs from counts. Exercise a seam, row carry,
   components of size 8/9 and 16/17, IDs above 63, and border-direction skipping.
7. Coarse grids: include paths of three and four moves, empty representatives,
   horizontal wrap, and repeated-goal cache hits.
8. Finalization: compare pre-pole and post-pole bytes and metadata separately;
   exercise both `wideLayout` values and both new and retained maps.

A useful generation matrix covers all three size presets, an odd custom height,
both topologies, all three land forms and ages, and dry/cold and wet/warm
extremes. Use more than one entry random state. Exact comparisons should include
the ordered six-byte tile array, header seed/dimensions, region fields, coarse
grids 0 and 1, and final random state; exclude allocator padding and unrelated
region fields that this operation does not initialize.

The vectors above are arithmetic checks, not claimed full-map golden outputs.
The repository's existing replay harness checks wider gameplay behavior against
the original program; it does not by itself establish that a new independent
implementation conforms to every phase specified here.

## Source traceability

These references allow maintainers to audit the specification. An implementer
can use the procedures above and the referenced F01 primitives without inspecting
the source.

| Subject | Primary source and original entry address |
| --- | --- |
| Pipeline, rainfall, age, poles, suppression | [map.cpp](../../src/auto/map.cpp), `map_generate`, `0x00408d33` |
| Fertility | Same file, `mapgen_calc_fertility`, `0x0040897f` |
| Land pass | Same file, `mapgen_add_landmass`, `0x0040a572` |
| Narrow walk and bounds | Same file, `mapgen_land_walk`, `0x0040a763`; `mapgen_in_bounds`, `0x0040a824` |
| Cluster and mark | Same file, `mapgen_mark_tile_cluster`, `0x0040a892`; `mapgen_mark_tile`, `0x0040a8db` |
| Wide walk, islet walk, lake | Same file, `0x0040a92f`, `0x0040aaa4`, `0x0040ab41` |
| Rivers and grass shields | Same file, `mapgen_place_rivers`, `0x0040ac5a`; `tile_grassland_has_shield`, `0x0040bcb0` |
| Region labels and borders | Same file, `map_assign_landmass_ids`, `0x004b32fe`; `landmass_compute_borders`, `0x004b315c` |
| Coarse connectivity and representatives | Same file, `map_build_coarse_connectivity`, `0x0055a980`; `map_coarse_cell_landmass`, `0x0055ac37` |
| Path query and flood | Same file, `map_path_length`, `0x004ad0d1`; `map_path_next_dir`, `0x004abfe5` |
| Path grid indexing | [unit.cpp](../../src/auto/unit.cpp), `path_grid_get_cost`, `0x004ad01e`; `path_grid_set_cost`, `0x004ad076` |
| Tile storage, wrap, allocation, seed | Same file, `tile_at`, `0x005b8931`; `map_wrap_x`, `0x005ae052`; `map_alloc`, `0x005b7fe0`; `map_new_seed`, `0x005b85fe` |
| Resources and huts | Same file, `tile_resource`, `0x005b8ee1`; `tile_has_hut`, `0x005b8ffa` |
| Presets, custom limits, parameter selection | [game.cpp](../../src/auto/game.cpp), `setup_choose_map_size`, `setup_custom_world`, and new-game callers |
| Shared arithmetic, random generator and seed conversion | [F01 traceability](determinism-and-arithmetic.md#7-source-traceability-and-validation) |
| Map and region layouts | [civ2types.h](../../src/include/civ2types.h), `MapHeader`, `Tile`, `Landmass` |
| Ordered neighbor constants | [data.bin](../../data/image/data.bin), addresses `0x0062833c`, `0x00628344`, `0x00628350`, `0x00628360`, `0x00628370`, `0x006283a0`, `0x006283d0`, `0x006283e0`; section mapping in [layout.json](../../data/image/layout.json) |

The independent reconstruction in `/home/peter/project/civ2`, inspected at
commit `4cfdd2a4b43e1a99983cc6af3e993a52b8ad8752`, supplies corroborating
descriptions in `src/map_generation.cpp` and neighbor constants in `src/map.cpp`.
In particular, it makes the two-draw latitude calculation, attempt-backup river
reads, retained-map finalization, and the river postprocessing's local-only
assignment explicit. The OpenCiv2 implementation is the compatibility authority
for this specification; broader corrupt-state behavior and reconstruction stops
in the other project are not part of this contract.
