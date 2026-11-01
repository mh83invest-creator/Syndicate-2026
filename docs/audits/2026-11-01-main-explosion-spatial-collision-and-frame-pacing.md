# Syndicate 2026 — Main Systems Audit: Explosion Spatial Collision and Frame Pacing

Date: 2026-11-01 (Australia/Brisbane)  
Workstream: MAIN/core systems on `main` — documentation-only.  
Source baseline: `engine/src/syndre.sx` Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines).  
Related: `docs/audits/2026-11-01-main-explosion-fireball-full-pool-scan.md`, `docs/audits/2026-11-01-main-fireball-same-tick-cascade.md`, plus earlier `docs/SOURCE_AUDIT_EFFECT_POOL_AND_PROFILING_2026-11-01.md` on `feature/v0.9-visual-showcase`.

**Scope:** audit the coupling between fireball creation, spatial collision checks, screen work and the existing 16 FPS pacing contract. No engine source change, profiler run or Windows execution.

## Findings confirmed by direct current `main` source reads

### MAIN-PERF-01 — Creation inserts the new effects into the spatial MapWho lists

The original `init_fireball` function (`syndre.sx:19879–19920`) visits each of the **128 records** between `level__Effects` and `data_96d70`, stride 30 bytes. For an inactive record, it sets active/type marker `+0x18=3`, then calls `move_on_mapwho` at line 19908 and writes dispatch subtype `+0x19=0x29` (41). The source does *not* return on its first successful allocation. By contrast, `init_effect:27937–27975` returns immediately after the first free record.

`move_on_mapwho:33712–33753` inserts an unlinked effect into the appropriate `level__MapWho` linked list using the entry and next/previous fields and sets a linked flag. This is not simply cosmetic particle allocation: newly allocated fireballs can alter spatial-list density before the next collision query. The helper itself has a condition that skips re-insertion if a record is already linked, but `init_fireball` explicitly zeros its field at `+0x0a` before this call (`19903`).

### MAIN-PERF-02 — Collision reads a local 3×3 group of MapWho cells, with linked-list traversal

`sprite_colide:3579–3637` finds a starting cell through `level__MapWho`, reads its list head and follows `(%ebx)` as a linked next-record offset. `sprite_colide:4639–4665` increments a per-cell traversal count and stops that individual list at `0x400` (1,024) links; after each list it moves to the next cell, with an inner and outer **3-cell** bound. Therefore the observed implementation can examine a **3×3 neighbourhood** of linked records, subject to its earlier coordinate/extent exits and other branches.

**Important qualification:** `vtable_127b8:3524–3530` dispatches candidate record type marker `3` to `jump_13734`, i.e. the *skip-to-next-list-item* path. Fireballs created by `init_fireball` use marker 3: they can contribute to linked-list traversal work but are **not processed as collision targets on this particular candidate-type branch**. Do not claim the routine performs full object-collision resolution against every fireball. The 1,024-link guard and 3×3 structure are **static source loop boundaries**, not measured entity counts or collision events, and 3×3 × 1,024 is NOT an observed workload.

### MAIN-PERF-03 — In-frame effect work can compound before drawing or sleeping

`move_it:33330–33337` sequences worlds → people → weapons → effects → objects → vehicles (vehicle is a tail jump). `move_effects:19979–20007,21308–21317` scans the entire 256-slot effect pool in ascending address order. The established `init_fireball` burst paths at `20201,20297` can visit 128 slots each multiple times; a newly created *later-index* first-half fireball can be processed in that same `move_effects` pass, dispatching subtype 41 via `vtable_1eaa8[41]` to `func_1f48a:20746–20753` and its fallthrough `func_1f4a0:20757–20799`. That path includes `affect_by_wind`, one `sprite_colide` call and `move_mapwho`. Later-index conditional dispatch plus MapWho insertions can produce additional *within-pass* collision-list traversals; it is not proof of actual measured slowdown or recursive/unbounded execution.

Inside `move_effects` there are **eight textual direct `call sprite_colide` locations** (20113, 20149, 20245, 20331, 20547, 20672, 20771, 21071), **four** `call sight_colide` locations (20417, 20555, 20706, 20843), and **two** `call bullet_colide` locations (20441, 20858). These are **callsite counts** across alternative handler paths, *not* the number executed in one tick. Effects 10→11 and 13→14 share fallthrough paths into fireball loops as documented in the predecessor.

### MAIN-PERF-04 — The 16 FPS pacing call comes after simulation and rendering preparation

The tactical `syndicate` loop in `syndre.sx:199–303` performs `move_it` at 230, other gameplay/logic through 240, `ac_LbScreenWaitVbi` 241, `swap_screen_vres16` 243, two `scroll_map` calls 257 and 265, `copy_back` 267, `draw_mapwho` 268, `draw_panel` 269 and `click_map` 270, before `ac_game_update` at 272 on the non-DOS path. In `swap_screen_vres16` the usual non-DOS branch calls `ac_update_vscreen_whole_vres16` then `ac_swap_wscreen` at `17950–17951` (a separate flag-controlled branch exists, so do not claim every pass necessarily uses these exact calls).

`engine/src/game.c:141–196` implements `game_update_full`; `game_update:204–208` calls it with `wait=true, max_fps=16`. The local integer `frame_duration = 1000 / max_fps` gives **62 integer milliseconds at 16**, not an exact 62.5 ms target each invocation. The function reads SDL ticks after unlocking the screen and handling SDL events; if the elapsed duration from the **previous stored tick** is short enough, it enters a delay/event/lock cycle. When the measured interval is already above the threshold, the sleep path is skipped. The elapsed expression includes **+2 ms** adjustment in `game.c:159`. `game_update_menu:223–227` passes `max_fps=30`, giving integer 33 ms, and both use the **same static `last_frame_ticks`** in `game_update_full`. Thus switching from a menu cadence to a mission cadence has a shared-timestamp transition; normal play implications require runtime observation.

**Do not confuse scheduler delay, collision CPU time and renderer/present CPU time.** An over-budget frame can skip sleep without speeding the fixed gameplay logic. `game_update_full` is not by itself proof of a precisely achieved 16.0 visible FPS rate, because SDL timer granularity, OS scheduling, `LbScreenWaitVbi` and work done elsewhere all influence wall-clock presentation. Do NOT propose reducing tick work or eliminating original fireball effects without separate approval.

### MAIN-PERF-05 — Pixel conversion and output are additional bounded stages, not known bottlenecks

`engine/src/display.c:149–231` converts **400 rows × 80 source bytes per row × 4 planes** into indexed pixels; `display.c:244–261` copies the composed buffer and calls `LbScreenSwap` when `DrawFlags` selects Vres16. `engine/bflibrary/src/x86-win-sdl2/sscreen.c:1050–1085` restores/checks SDL surfaces, may perform a software scaled blit and invokes `SDL_UpdateWindowSurface`. The custom scaling function is at `sscreen.c:976–1006`; existing graphics/source audits describe conditional output format/row pitch concerns. Those concerns do not establish that software scaling **causes** the explosion slowdown. The converter and output stages must be timed independently from effect simulation and collision.

## Proposed nonproduction profiling contract — no instrumentation implemented

For each actual tactical tick, capture distinct, monotonic wall-clock elapsed durations and counters (with probe overhead quantified):
1. `move_worlds`, `move_people`, `move_weapons`, `move_effects`, `move_objects`, `move_vehicles` (preserve ABI including the **tail jump** to vehicle update), plus other main tactical functions.
2. `move_effects` occupancy/type histogram at entry and exit; indices of all newly created fireballs, same-pass subtype-41 dispatch count and per-pool-position allocation/scan count.
3. For `init_fireball`, calls and 128 visits per call, number of newly linked records, `move_on_mapwho` calls and time; for `init_effect`, visited-slot counts and failed allocations; avoid mutating random generator state.
4. `sprite_colide` call frequency, MapWho cells visited, linked records traversed, type-3 skips, and elapsed time; `sight_colide` and `bullet_colide` separately. Do not count assembly lexical calls as dynamic invocations.
5. `swap_screen_vres16`, 4-plane conversion, buffer copy, scaled SDL blit and `SDL_UpdateWindowSurface` separately; wall clock around `LbScreenWaitVbi`; measured work vs delay/event-loop time in `game_update_full`.
6. Overall presentation timestamps and frame-time distribution (median, P95, P99, missed-tick count) compared across **identical legal mission assets, camera location, entities, sound settings and input sequence**: idle; traffic; single gunfight; one explosion; overlapping explosions. Repeat with/without instrumentation to quantify probe overhead and compare v0.7 vs an isolated diagnostic build without modifying either baseline.

A diagnostic implementation would require a separate agreed scope and a disposable **32-bit Windows** build. Source-only audit cannot produce benchmark numbers. A 62 ms integer scheduler frame duration is a code fact; not a demonstrated computational allowance, guaranteed frame interval or measured FPS.

## Verification and change control

**Performed:** direct live GitHub complete assembly blob inspection; targeted linked-map and collision dispatch line reads; cross-check of current `game.c`, `display.c` and SDL output source; precise static callsite count, address/loop arithmetic and test boundary design. No third-party code incorporated.

**Not performed:** compilation, runtime/gameplay, benchmarks, 16 FPS verification, collision injection, screenshots, actual graphics/retail assets, memory tools, CI, source/binary edits or mission/save changes.

**Next source-only checkpoint:** deeper examination of the tactical time budget, sound scheduling and per-effect type lifetimes where independently relevant; after real Windows access, measure before approving any optimization.

Protected v0.7, original and American Revolt gameplay, 16 FPS tactical pacing, original explosion appearance and damage, mouse/keyboard input, between-mission-only campaign-specific save isolation and the two parallel workstreams remain untouched.
