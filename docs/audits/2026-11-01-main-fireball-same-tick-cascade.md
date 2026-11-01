# Syndicate 2026 — Main Systems Audit: Fireball Same-Tick Dispatch Cascade

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch:** `main`, core engine/performance audit.  
**Predecessor:** `docs/audits/2026-11-01-main-explosion-fireball-full-pool-scan.md` (initial commit `b84da3076df2405256188f958fc006ee3c6307b4`; follow-up `8b8c0d4baafc7e71c68ff5c7193b591572af175a`).  
**Status:** new source-only temporal/ordering deduction. No executable profile or code change.

## Sources examined directly

Original full assembly `engine/src/syndre.sx`, SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf` on `main`. Confirmed exact code:
- `init_fireball:19879–19920`: traverses all 128 half-pool entries, may initialize every inactive entry, stores byte `+0x18 = 3` (active) and `+0x19 = 0x29` (dispatch type 41), calls `move_on_mapwho`, then moves to next entry without returning early.
- `move_effects:19979–20007`: starts at `level__Effects`, tests marker `+0x18`, obtains type `+0x19`, dispatches through 53-entry table.
- `vtable_1eaa8:19922–19975`: zero-based entry **41** (`0x29`) is `func_1f48a`.
- `func_1f48a:20746–20753`: updates effect fields, increments dispatch type; source then **falls through** into `func_1f4a0` at 20757.
- `func_1f4a0:20757–20799`: invokes `affect_by_wind`, `sprite_colide`, `move_mapwho`; an animation/removal path is conditional on state.
- `move_effects:21308–21317`: advances the current effect pointer by **30 bytes** and iterates until `level__Commands`.
- Two `init_fireball` callers within the effects handler region: `func_1edb7` (two calls per reached loop) and `func_1eecb` (ten calls per reached loop). Preceding types 10/13 may also fall into these respective handlers.

## CORE-CASCADE-01 — A newly allocated effect can be dispatched in the *same* move_effects pass

`move_effects` processes the entire 256-entry pool **in increasing record-address order**. Its active marker and effect subtype are read as the loop reaches each record. `init_fireball`, called while processing one record, can initialize previously inactive records anywhere in the first 128 positions.

**Source-derived conditional:** If a record at index `i` invokes `init_fireball` and the function initializes a vacant slot `j > i` (with `j < 128`), then under the inspected normal loop control that slot will be visited **later in the same pass**, see the new active marker, and dispatch subtype `0x29`. This is not deferred automatically to the next 16 FPS simulation tick. Slots `j <= i` have already been scanned this pass, so their ordinary first processing will wait until a later pass unless another path handles them. If the invoking record is index `i >= 128`, all first-half fireball slots precede it, so this particular sequential traversal cannot process them again during the same pass.

This is a **conditional ordering property of the assembly**, not a claim that all 128 slots are spawned or visited on each real gameplay tick. The source gives no measured occupancy distribution, effect handler frequency or runtime timeline.

## CORE-CASCADE-02 — Newly born type 41 has a nontrivial same-pass handler

Subtype `0x29` is decimal 41, which the 53-entry dispatch table maps to `func_1f48a` (not directly to `func_1f4a0`). That entry performs state initialization and then falls into `func_1f4a0`: wind effect, sprite collision, map-object movement, and conditional model animation/removal. Thus same-pass fireball dispatch can include **additional collision/movement work beyond the already costly 128-slot scan and map linking**. Handler execution and collision outcomes remain conditional.

This is a credible mechanism for an explosion burst to increase CPU work **inside the very same `move_effects` call**, but cannot independently identify the cause of observed frame slowdown without profiling.

## CORE-CASCADE-03 — Performance instrumentation and no-change constraints

A future separately approved, diagnostic-only Windows build should log per game tick (not alter):
1. Current effect-record index, effect subtype and active count when entering/leaving `move_effects`.
2. `init_fireball` call count, per-call vacant slots filled, and number of those with index higher than the spawning handler's current slot.
3. Number of subtype 41 entries dispatched in the same pass they were created versus on a later pass.
4. Time spent in fireball scans, `move_on_mapwho`, subtype-41 `sprite_colide`/`affect_by_wind`/`move_mapwho`, full effects step, other gameplay steps and rendering/presentation.
5. 16 FPS frame-time distribution with no explosions, ordinary fighting, single burst and overlapping explosions, comparing an uninstrumented control build to avoid probe overhead misleading measurements.

Instrumentation must preserve original effect counts, random calls, collision/damage, visuals, sound, order of all six movement stages and 16 FPS tactical pacing. Do not add a delayed-effect queue or change traversal as an unapproved “optimization”; a new scheduling strategy would visibly alter gameplay/effect timing.

## Verification and handover

**Performed:** direct original 85,803-line assembly read, zero-based table-index cross-check, manual code-flow reasoning about ordered slot traversal and creation, and explicit identification of the subtype 41 handler/calls. No engine edits.

**Not performed:** compilation, CI, retail data inspection, gameplay execution, real frame timing or instrumented tests, code change, saved game modification, or other workstream branch change.

**Next main audit priority:** capture evidence under real combat to distinguish allocator scans, same-pass effect collision work, graphics conversions, audio and window presentation before selecting a safe optimization path. Protected v0.7, original campaigns, mouse/keyboard, between-mission-only campaign-isolated saves and untouched Graphics/Mission Builder branches remain mandatory.
