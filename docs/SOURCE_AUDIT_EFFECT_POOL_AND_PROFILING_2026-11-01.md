# Syndicate 2026 — Original Effects / Explosion Pipeline Forensic Audit

**Date:** 2026-11-01 (Brisbane)
**Status:** Static source dissection only. Not a runtime profile, gameplay change, or implementation approval.
**Baseline:** `main`, v0.7 at `a22b0b9580a9710b4e693e88854351835ef1ee40`. Audit branch: `feature/v0.9-visual-showcase`.
**Source:** Original assembly `engine/src/syndre.sx`, Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, 85,803 lines. All locations below are 1-based positions in that exact file.
**Purpose:** Trace asset-independent mechanisms underlying original effect-heavy combat and draw pressure, **without changing timing, visuals, or simulation**.

## Critical correction to earlier audit: five calls PLUS one tail jump

`syndre.sx:33330–33337` defines `move_it`:

```asm
move_it:
    call move_worlds
    call move_people
    call move_weapons
    call move_effects
    call move_objects
    jmp  move_vehicles
```

Earlier documentation listed only five calls; that is incomplete as a **simulation dispatch graph**. **Vehicle processing is invoked via a tail jump**, not an ordinary `call`. The target `move_vehicles` exists at **line 57242** (the label is indented, which also explains why narrow column-zero label scans missed it). Effective transition sequence: worlds → people → weapons → effects → objects → vehicles (tail jump). This is confirmed assembly control flow, although subsequent indirect branches in each subsystem remain untraced. Preserve this ordering.

**General audit methodology correction:** A scan counting only `call` instructions misses tail jumps and indirect jump-table dispatch. Prior counts such as “77 direct effect calls within `move_effects`” are **textual occurrences aggregated across numerous handlers**, not a certified set of calls executed by the top-level entry routine or per-frame. Do not present such counts as benchmark evidence or an exact semantic call graph.

## Actual fixed effect pool

| Fact | Source evidence | Calculation |
| --- | --- | --- |
| Effect pool base | `level__Effects` at original address `0x95e70`, assembly line 85736 | — |
| Midpoint | `data_96d70` at `0x96d70`, line 85738 | (0x96d70-0x95e70=0xF00) bytes |
| Pool end | `level__Commands` at `0x97c70`, line 85740 | (0x97c70-0x95e70=0x1E00) bytes |
| Record stride | `add $0x1e,%ebx` in effect allocator/iterator at lines 19913, 21308, 27969 | (0x1e=30) bytes |
| General pool capacity | 7,680 bytes / 30 | **256 effect records** |
| Fireball allocator range | `init_fireball` compares against `data_96d70` at 19886–19915 | **First 128 records** |
| General effect allocator | `init_effect` compares against `level__Commands` at 27939–27975 | **All 256 records** |

The pool-size arithmetic is exact for those source addresses and stride. It does not establish that 256 effects are always visible or that each record represents an explosion.

**Allocation behaviour:** `init_effect` scans from the *beginning* of the pool for an inactive entry (type at offset `0x18`); on success it initializes coordinates/type, calls `move_on_mapwho` and returns the record pointer. If all records are occupied it returns zero. `init_fireball` scans only the first 128 entries, initializes type `0x29`, but returns no pointer. These are *linear searches*; their frequency and cost under load are not measured.

## Effect-update dispatch: 53 cases with different cost shapes

`syndre.sx:19922–19975` contains **53 explicit `.long` entries** in `vtable_1eaa8`. `move_effects` reads each active record's type byte at offset `0x19`, checks it against `0x34` (52), and performs an indexed indirect jump at **lines 19999–20007**.

The table contains 28 distinct handler/jump destinations (a static count, not distinct gameplay effect types). Repeated entries are common: `jump_1fb10` 14 times, `func_1fb07` 7 times, and `func_1ebe5` 6 times. Different effect types therefore follow materially different work paths. An “average effect cost” derived from simple count of instructions would be misleading.

At **21295–21317**, shared post-dispatch code advances the record pointer by `0x1e` and loops until `level__Commands`, meaning all pool slots can be visited on each `move_effects` call, even if most entries are inactive.

Some source-confirmed work paths:

- Lines **20090–20115**: animation and `sprite_colide` calls, with state changes.
- Lines **20119–20163** and **20215–20259**: nested area loops that repeatedly call `sprite_colide` and trigger sound status changes (effect-type-dependent).
- Lines **20172–20210**: random location generation and two `init_fireball` attempts in an effect handler, followed by animation/state progression.
- Lines **21180–21285**: several bounded loops (including up to 16 and up to 32 iterations) with random coordinates and `init_effect` attempts. Conditional branches mean these counts are **source loop ceilings, not observed spawned effect counts**.
- Lines **21288–21317**: effect removal and shared animation/advance logic.
- `move_weapons` and action handlers also invoke effect initialization; see `move_weapons` near line 58876 and `process_action` near line 28450.

**Risk hypothesis (not measured):** More concurrent effects can increase active case dispatch, repeated collision checks, sound triggers, and linear allocator scans. Effect bursts can cause both gameplay-simulation cost and subsequent sprite redraw pressure. It is therefore unsafe to blame all explosion slowdown on SDL swapping without measurement, or to treat a graphics-only shortcut as equivalent to preserving original gameplay/effects.

## Profiling boundaries for a later approved diagnostic-only build

These boundaries follow actual call sites; suggested probes are **not implemented**.

| Proposed measured stage | Actual source hook | Metric, without changing game speed |
| --- | --- | --- |
| Whole mission frame / update pacing | `syndicate` at lines 198–303 and `game_update_full` in `engine/src/game.c:142–193` | Wall time, call frequency, time spent sleeping vs processing |
| World/agent/weapon/effect/object/vehicle progression | `move_it` at 33330–33337 | Stage times for six steps, including tail-jump vehicle handling; sampling/instrumentation must preserve ABI |
| Effects iteration | `move_effects` at 19979 and exit 21311–21317 | Occupied slots, distribution of type values, handler count, elapsed time |
| Effect allocation | `init_effect` 27937–27975, `init_fireball` 19879–19920 | Search length, failed allocations, allocation frequency (general vs first-half) |
| Collision and fireball spawn | handler call sites around 20113, 20149, 20201, 20245 and 21180–21285 | Count and elapsed time, distinguishing source ceiling from actual calls |
| Sprite draw / dirty area restore | `draw_mapwho` 18029–18715, `copy_back` 18910–19034, `set_block_changes` 17803–17890 | Draw area/dirty region, visible sprites, CPU time |
| Planar expansion | `engine/src/display.c:149–231` | Conversion CPU time and byte-equivalent output |
| SDL scaling, surface copy and present | `sscreen.c:976–1006,1050–1085` | Blit vs update times, fullscreen/windowed differences |
| Audio effects | `engine/src/sound.c`, `bfsoundlib` OpenAL layer | Play requests, active sources, processing overhead |

**Timing policy:** Maintain existing 16 FPS mission pacing and three menu 30 FPS calls. Instrumentation must not add extra ticks, drop simulation work, shorten animations, suppress original sound or mutate random states. Measure CPU elapsed time and emitted frames separately. Profiling itself may perturb timings, so a noninstrumented control run is required.

**Test scenarios when the user supplies local game assets:** idle map → normal pedestrian traffic → moving vehicles → small firefight → overlapping explosions/fireballs (same mission/camera position if practicable). Compare frame-time distributions and per-stage counters, not just an anecdotal impression of “smoother.”

## Regression gates and source-only status

**Source checks performed:** verified six-step dispatch including tail jump, `move_vehicles` target label, pool addresses and stride, both allocator search ranges, 53-entry effect dispatch table, handler loop/collision/effect-creation code. Calculations are source-byte/address-derived; no commercial map data needed for these facts.

**Not performed:** compile, sample-frequency profiling, measuring actual CPU hot spots or FPS, high-explosion Windows playtest, simulated asset data, pixel-equivalence tests, gameplay regressions. No source implementation changed.

**Do not optimize yet.** The investigation establishes testable hypotheses and exact instrumentation boundaries. Changes require a controlled branch plus real baseline playtesting, especially since excessive effect work may be part of original gameplay rather than pure presentation cost.

**Engine decision:** Keep SyndicatFX, not FreeSynd; preserve v0.7, original camera, simulation cadence, effect visuals, audio, saves, and the approved v0.9 roadmap. No controller feature.
