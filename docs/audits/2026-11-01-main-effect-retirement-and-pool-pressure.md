# Syndicate 2026 — Main Systems Audit: Effect Retirement, Reuse and Pool Pressure

Date: 2026-11-01 (Australia/Brisbane)  
Workstream: **MAIN/core systems** on `main`; source audit only.  
Evidence identity: complete original `engine/src/syndre.sx` Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines).  
Predecessors: `docs/audits/2026-11-01-main-explosion-fireball-full-pool-scan.md`, `docs/audits/2026-11-01-main-fireball-same-tick-cascade.md` and `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md`.  
Status: **source-confirmed conditional mechanics; no code change, executable test or measurement**.

## Findings

### MAIN-LIFE-01 — Removal unlinks an effect and makes its fixed slot reusable

`remove_model` at `syndre.sx:33341–33349` first calls `move_off_mapwho` and then writes zero to the record's active/type marker at offset `+0x18`. `move_off_mapwho` at `33669–33708` checks the linked flag (bit `0x4` at offset `+0x0a`), updates the preceding next-link or `level__MapWho` cell head and the following record's backlink, and clears that flag.

Both `init_effect` (`27937–27975`) and `init_fireball` (`19879–19920`) use `+0x18 == 0` as the available-slot test. Thus the observed **normal removal path returns a slot to the reusable fixed pool and unlinks its spatial entry**; it is not a per-explosion malloc/free cycle. The function does not zero the entire 30-byte record; loaders/allocators overwrite the fields they explicitly initialise. No stale-state bug is claimed without a failing case.

### MAIN-LIFE-02 — Two allocator policies contend for the same first-half records

Fixed pool source boundaries are `level__Effects=0x95e70`, `data_96d70=0x96d70`, `level__Commands=0x97c70`, with 30-byte entries: **128 first-half records, 256 overall**.

- `init_effect` scans **all 256** slots in increasing order, returns immediately after preparing the first free record, and returns zero when none exists (`27943–27975`). This includes the first 128 slots.
- `init_fireball` scans **only first 128**, does **not** stop on the first available record, and initialises **every vacant slot encountered** (`19889–19915`). It does not return an allocation pointer.
- Both call `move_on_mapwho` to link newly activated entries; the fireball subtype is `0x29` (41), while the general allocator's subtype is normally chosen by its caller.

Consequently **ordinary effects can already occupy fireball-eligible slots**, and a fireball burst can use previously available slots which would otherwise be considered by subsequent general-effect allocations. Neither allocator has a separate independent first-half reservation enforced in this code. Conversely a full first-half pool does **not** imply all 256 effect slots are full; the second half may remain available for `init_effect`.

A full first-half fireball scan still performs its 128 occupied-slot checks each invocation. The ten-call effect branch therefore still encodes **1,280 checks** under saturation, even when it creates no additional fireballs. These are source-derived bounds, **not observed frequency, per-frame allocations or an explanation for every slowdown**.

### MAIN-LIFE-03 — Newly created fireball's subtype 41 progresses to 42, then removes on an animation condition

The 53-entry effect type table `vtable_1eaa8` maps zero-based subtype `41` (`0x29`) to `func_1f48a` at `20746–20753`, which initialises a local counter `+0x14` to zero, increments the subtype to **42**, and falls through into `func_1f4a0` (`20757–20799`). The following handler runs wind, sprite collision and spatial movement, increments the counter, and checks whether it equals **2**. If not, it advances the ordinary effect iteration. On the counter-equals-2 branch it calls `animate_model` and runs `remove_model` **only if the animation result is nonzero**, then resets the counter.

`animate_model` at `33353–33372` advances the stored frame using the `frames` table and returns a flag derived from the current frame entry. Therefore **two visits are not necessarily the fireball's full lifetime**. Actual retirement depends on animation data and result; a still-active subtype-42 record can consume more simulation/collision/spatial-update work on later passes. The real frame data and live lifetime distribution were not inspected or measured. There is **no proof of an indefinitely stuck effect or leak** from this alone.

The table also maps subtype 43 to `func_1fb07` (removal), but the above 41→42 path **does not itself advance to 43**. Do not invent a guaranteed 41→42→43 progression.

### MAIN-LIFE-04 — Ordered iteration explains same-pass and later-pass work

`move_effects:19979–20007,21308–21317` checks each active slot once per ordinary forward pool sweep. If a handler spawns a fireball into a **later** slot (`j > current index`) in the first half, its subtype-41 handler can run in the **same simulation pass**; if the slot precedes the current index, it is not revisited by that ordinary sweep and will be considered in a later pass. This is a conditional state-ordering rule, not a measured effect or unbounded recursion.

As `remove_model` unlinks and clears its marker, the next eligible allocation scan can reuse the slot. Time spent in a crowded spatial list and effect subtype 42 can continue while the record remains active, but the **pool size and per-`move_effects` top-level slot count remain fixed**.


### MAIN-LIFE-05 — Fireball frame progression depends on separately loaded mission animation data
The missing animation-data provenance is now identified directly in the same assembly blob: `mission_load_files:82620–82663` includes `data/hfra-0.ani` into `frames`/`frames_end`, `data/hele-0.ani` into `elements_ani`/`elements_ani_end`, and `data/hsta-0.ani` into `starts_ani` (without a separately declared end slot in that descriptor). `load_map_level:27901–27912` calls `ac_LbDataLoadAll(mission_load_files)` without checking its returned number of failed entries, before continuing to mission setup. At `move_effects:21295–21306`, the animation selection path dereferences `starts_ani` at an offset based on the chosen animation ID (for subtype 41, `func_1f48a:20746–20753` supplies `0x187`, decimal 391), without a local loaded-buffer/length check. `animate_model:33353–33372` uses `frames` as an array of **8-byte** records, obtains the next-frame index from field `+6`, then reads field `+5` of that next record for its completion flag, also without a local pointer or `frames_end` validation.

These are **cross-resource integration contracts**, not proof that ordinary legal retail data are missing, that index 391 is invalid, or that a crash occurs. Existing graphics/asset audits own asset-specific inspection; the core audit owns the load-failure propagation and safe runtime decision. No animation files or images have been read in this stage. A later copy-only test should cover unavailable `hfra-0.ani`, unavailable `hsta-0.ani`, short positive reads, out-of-range next-frame indices, and observable behaviour through the original mission-load return path. Protect original explosion animation and 16 FPS processing.

## Why this matters for the explosion-performance investigation

Prior source reports identified repeated 128-slot scans, conditional same-pass spawning, collision linked-list traversal and drawing. This checkpoint adds a lifecycle distinction:
1. **Allocation pressure** from shared first-half entries (and general effects competing for those entries).
2. **Post-burst persistence** from subtype-42 effects continuing collision/wind/movement until their animation condition retires them.
3. **Normal recovery/reuse** via `remove_model → move_off_mapwho → active marker 0`; therefore no evidence of an intrinsic permanent accumulation in the verified removal path.

The first two are plausible causes of work persisting beyond the moment of explosion, but **which dominates is unknown**. Avoid conflating a full fixed pool with a memory leak; they are different mechanisms.

## Next diagnostic contract (not implemented)

A *separately approved* 32-bit Windows diagnostic copy should measure per simulation pass: total and half-pool active/free counts; per-subtype histogram 10/11/13/14/41/42; `init_effect` scan distance and failed allocations; `init_fireball` invocations/slots filled; creation index versus current iterator; subtype-42 age measured in ticks and removal reason; number/time of `move_on_mapwho` and `move_off_mapwho`; linked nodes traversed by `sprite_colide`; simulation versus conversion/blit/present wall times. Compare quiet, ordinary fight, single explosion, overlapping explosion, and *post-explosion decay* windows at identical camera/mission state, with uninstrumented control. Never modify random-call order, effects, collision damage, simulation order, animation frame tables or 16 FPS pacing without separate approval.

## Verification

**Performed:** direct source read of fixed array bounds, both allocation policies, spatial map insertion/removal, `remove_model`, `animate_model`, type 41/42 dispatch and effect-iteration order. No code or asset edit.

**Not performed:** binary linkage inspection, 32-bit compilation/CI, retail asset analysis, actual animation frame inspection, gameplay, profiler, memory instrumentation, saved-game tests or Windows runtime validation.

**Next main/core source checkpoint:** inspect effect animation frame tables and the source paths that set subtype 41/42, and verify whether the C/assembly build can instrument timing safely; then obtain real data before optimization. Preserve protected v0.7, original and American Revolt, user-requested original explosions, mouse/keyboard, strategic save isolation only between missions, and parallel Graphics Audit and Mission Builder ownership.
