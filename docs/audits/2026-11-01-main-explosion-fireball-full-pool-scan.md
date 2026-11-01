# Syndicate 2026 — Main Systems Audit: Fireball Full-Pool Scan and Burst Workload

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch:** `main` — core gameplay/performance source audit.  
**Status:** Newly verified static assembly behaviour; **no gameplay performance measured and no code changed**.  
**Related pre-existing cross-branch analysis:** `docs/SOURCE_AUDIT_EFFECT_POOL_AND_PROFILING_2026-11-01.md` on `feature/v0.9-visual-showcase`. That report correctly identifies the 128-slot fireball half-pool and linear scans, but does not explicitly highlight that one `init_fireball` invocation **does not return upon filling a vacancy** and can fill *all* free slots in its range. This report narrows that behaviour directly from the same source; it does not modify the graphics branch.

## Evidence directly re-read

Complete `engine/src/syndre.sx` on `main` via GitHub blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines).

- `level__Effects:85736` at original address `0x95e70`; `data_96d70:85738` at `0x96d70`; `level__Commands:85740` at `0x97c70`.
- `init_fireball:19879–19920`, especially `19886–19891`, `19897–19915`.
- `move_on_mapwho:33712–33753`; does not explicitly reset the effect's active/type byte at `+0x18` in the inspected routine.
- `move_effects:19979–20007,21295–21317`.
- `func_1edb7:20172–20211` (loop/call at 20201, bound at 20203–20204).
- `func_1eecb:20264–20307` (loop/call at 20297, bound at 20299–20300).
- `vtable_1eaa8:19922–19975`; effect type indices map to the two cited handlers (type 11 and 14 in that zero-based table).
- `init_effect:27937–27975`, separate general first-available-slot allocator.
- `move_it:33330–33337`: worlds → people → weapons → effects → objects → vehicles (last transition is a tail jump).

## CORE-FIRE-01 — One call traverses the ENTIRE 128-record fireball range, even after successful allocation

`init_fireball` starts at `level__Effects` and checks against `data_96d70`. The in-loop test reads the marker byte `+0x18`; active entries branch straight to `jump_1ea98`, while an inactive entry has coordinates and state assigned, marker `+0x18` set to `3`, then `move_on_mapwho` called, subtype `+0x19` set to `0x29`. **The same `jump_1ea98` then advances by `0x1e` and loops while below `data_96d70`; there is no success `ret` or branch out after the first vacancy.** Both occupied and newly filled entries lead to the next slot.

Arithmetic: `(0x96d70 - 0x95e70) / 0x1e = 0xf00 / 30 = 128` slot inspections per invocation. If 128 inactive records are encountered and `move_on_mapwho` returns normally without external alteration of other slot markers, the function can initialise **all 128** in *one invocation*, including up to 128 `move_on_mapwho` calls. The freshly inspected `move_on_mapwho` implementation updates map links/coordinates/flags but does not itself clear `+0x18`.

This corrects any impression that `init_fireball` works like the **separate** `init_effect` routine, which returns immediately on the first free slot (`27943–27967`). The first uses a full half-pool sweep; the second uses a first-free allocator. Neither is a demonstrated bug; the multi-fireball behaviour may be intentional presentation/gameplay logic.

## CORE-FIRE-02 — Two handler loop sites can repeat full scans

The two directly observed call sites to `init_fireball` are at:
- `func_1edb7:20201`: `esi` increments and compares to `2` at `20203–20204`. The source loop is structured to call `init_fireball` **twice**, conditional on reaching that handler.
- `func_1eecb:20297`: `esi` increments and compares to `10` at `20299–20300`. The source loop is structured to call `init_fireball` **ten times**, conditional on reaching that handler.

Because each call scans 128 positions, these loops encode **256** and **1,280** candidate slot inspections respectively per *single handler execution*, even if there are no vacancies. These are source-derived instruction-loop ceilings in the inspected branch, **not measured counts of actual runtime fireballs, invocations or frame workloads**. Once the first call fills all vacant slots, the remaining calls may perform scans only; other in-frame activity and pool mutation must be considered before claiming exact spawn totals.

## CORE-FIRE-03 — Why the distinction matters for explosion slowdown

The prior forensic report identified 256 total 30-byte effect records, a 53-case effect dispatch, linear allocations and collision/animation work. This newly sharpened finding identifies an unusual fixed-work burst path: potential mass filling of the first half-pool PLUS repeated 128-entry scans from higher-level effect handlers. This is a plausible contributor to the user's original explosion-heavy combat slowdown.

**Still unverified:** frequency of effect types 11/14 with retail mission data, total active effect counts, collision/redraw/audio overhead, time spent in `init_fireball` vs `move_effects`, real 16 FPS frame-time misses, and whether speeding allocation without changing state is possible. No engine optimization, simulation skipping, effect-count limit, random-sequence change or animation removal is authorised by this audit.


## CORE-FIRE-04 — Adjacent effect handlers can fall through into burst loops
A subsequent exact line read adds an important reachability qualification. The dispatch table at `syndre.sx:19922–19938` maps zero-based type 10 to `func_1ed0d`, type 11 to `func_1edb7`, type 13 to `func_1ee2f`, and type 14 to `func_1eecb`. In the source, `func_1ed0d` at `20119–20168` **falls through** (no intervening unconditional jump/return) into `func_1edb7` at `20172` after its collision/status work. Likewise, `func_1ee2f` at `20215–20260` falls through into `func_1eecb` at `20264`. Thus the two-iteration and ten-iteration `init_fireball` loops can be reached by **more than the two direct jump-table indices 11 and 14**; types 10 and 13 also have source paths through them. Exact effect-type frequencies and conditional exits are not demonstrated. This refines handler reachability without implying all four types trigger every frame.

## Next performance checkpoint

1. Inspect effect handler 11/14 entry conditions and life-cycle/collision linkages in source to determine exactly which transitions reach the two loops. Do not infer dynamic frequencies from static dispatch-table entries.
2. Establish a **diagnostic-only** profiling contract: number of `init_fireball` invocations, scanned slots per invocation, newly occupied slots, `move_on_mapwho` calls, effect type counts, `move_effects` elapsed time and end-to-end render/present wall time. Use 32-bit ABI-preserving and timing-minimal probes; no source changes in this audit stage.
3. Compare noninstrumented and instrumented v0.7 baseline-equivalent Windows builds on identical legal retail data and scenario/camera conditions: idle, small firefight, one explosion, overlapping explosions. Preserve original FPS target/visuals/sound/collision/AI.
4. Escalate any proposed actual source modification through independent approval; coordinate render dependencies with Graphics Audit and mission resource assumptions with Mission Builder.

## Verification and change control

**Performed:** direct GitHub assembly blob read; slot and loop arithmetic; cross-check of `init_fireball`, `init_effect`, both direct call sites and `move_on_mapwho` side effects; examination of earlier feature-branch report for nonduplicative correction; documentation/report commit with separate read-back verification.

**Not performed:** compiler, CI, Windows launch, profiler, memory instrumentation, sound/visual tests, retail data inspection, gameplay regression, synthetic runtime harness, source change or branch merge.

**Baseline:** v0.7 protected. 16 FPS tactical simulation, mouse/keyboard-only controls, original campaigns and between-mission-only isolated campaign saving are unchanged; Mission Builder and Graphics Audit remain separate.
