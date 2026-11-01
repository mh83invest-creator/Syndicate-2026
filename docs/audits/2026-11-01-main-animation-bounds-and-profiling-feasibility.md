# Syndicate 2026 — Main Systems Audit: Animation Bounds and Profiling Feasibility

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch/workstream:** `main` — Main/Core Systems Audit. **Documentation-only.**  
**Predecessor:** `docs/audits/2026-11-01-main-effect-retirement-and-pool-pressure.md`, latest follow-up commit `fd353f1f89d659507597003b50ac03e5342645a6`.  
**Method:** direct live GitHub source reads, *not* execution, benchmark or asset inspection.

## Evidence read for this checkpoint

- Complete `engine/src/syndre.sx` from Git blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, **85,803 lines**.
- `engine/src/Makefile.am:1–40,44–47,73–78` — assembled original `.sx`, generated wrapper `.sx` and preprocessing/assembler path.
- `engine/util/mkwrappers:76–112,213–270` — wrapper generation, register saving, calling convention and debug-print option.
- `engine/src/game.c:141–196,204–208,223–227` — 16 FPS tactical / 30 FPS menu pacing through shared C function and millisecond SDL ticks.
- `engine/bflibrary/src/general/gdata.c:65–130` — native batch-resource-load partial failure and unchecked result contract.
- `.github/workflows/windows-source-v0.7.yml:13–18,37–59,103–104,122–169` — existing Windows i686/MSYS2 native-build path; not executed now.

## MAIN-ANI-01 — Animation data sources and boundary symbols (verified)

The mission descriptor at `syndre.sx:82620–82663` contains these *distinct* resources:
- `data/hfra-0.ani` → `frames` **and** `frames_end`, using pointers declared at `83877–83880`.
- `data/hele-0.ani` → `elements_ani` **and** `elements_ani_end`, declared at `83881–83884`.
- `data/hsta-0.ani` → `starts_ani`, declared at `83885–83886`; no adjacent end pointer is listed for this file descriptor.

This is the *native loading descriptor's* information, not proof of on-disk file format, actual decoded byte lengths or valid index counts. The original assembly has two lexical `frames_end` uses (definition and descriptor), and two `elements_ani_end` uses; the **specific** `animate_model` and effect-table consumers inspected below do not use them to validate access. Full cross-module end-pointer usage is not certified from these lexical counts.

`load_map_level:27901–27930` calls `ac_LbDataLoadAll(mission_load_files)` and continues regardless of the numeric failure count at that callsite. A native loading error does not establish that the animation pointer is valid, or that gameplay reaches a consumer, without further state testing.

## MAIN-ANI-02 — Fireball animation-start lookup uses 16-bit index 391 (verified)

The first fireball frame handler `func_1f48a:20746–20753` places literal animation selector `0x187` (decimal **391**) in the local value subsequently read by `move_effects:21295–21306`. The shared post-dispatch path indexes `starts_ani` as `(%eax,%edx,2)` and writes the **16-bit** result into effect frame index field `+0x10`.

For that lookup alone, the table requires at least **(391 + 1) × 2 = 784 bytes** of addressable entries starting at the `starts_ani` pointer. This is a *conditional minimum for this specific selector*, **not** a measured size or blanket required length for all game animation resources. The source shows no local pointer/length check before that dereference. Other `starts_ani` consumers exist (including fixed-offset uses), so the eventual validation must derive the **maximum reachable index across all consumers**, not assume 784 bytes is universally sufficient.

## MAIN-ANI-03 — Frame-animation record chaining and retirement depend on loaded bytes (verified)

`animate_model:33353–33372` takes the current effect's 16-bit frame index, computes `frames + 8 × index`, reads the **16-bit** next-frame index at offset `+6`, then indexes `frames + 8 × next_index` and reads the completion flag at offset `+5`, masking bit 0. It uses the returned flag to drive animation-dependent removal in the effect subtype 42 path (`func_1f4a0:20783–20799`). Neither `frames` nor `frames_end` is locally validated in this routine.

Consequently a meaningful validator must check: a non-null pointer; the **actual loaded byte extent**; the selected first record (`8 × index + 8 <= loaded_length`); the *next* record (`8 × next_index + 8 <= loaded_length`); and allowed frame record/flag semantics using the real resource format. These are proposed validation conditions, not an implemented parser, proof that legal assets violate them, or a safe way to alter the original animation progression.

`move_effects` itself also has a separate `starts_ani` lookup after subtype handlers (`21295–21306`) with no local end check. Prior audits cover absent/partially loaded mission resource failure propagation.

## MAIN-PROF-01 — Build has an assembly/C boundary, but no ready-made granular timing probes

`engine/src/Makefile.am:6–40` includes original `syndre.sx` and C sources in the same executable. Lines `44,73–78` generate `wrappers_game.sx` and other wrapper assembly from `engine/conf/*.conf` via `util/mkwrappers`, then preprocess/assemble `.sx` files. `mkwrappers:213–270` reveals convention-sensitive register saving and call adapters. **This establishes an ABI-sensitive integration point, not an already working profiler.**

Several hot routines — `move_effects`, `init_fireball`, `init_effect`, `sprite_colide`, `move_mapwho`, `animate_model` — are directly called within the original assembly. They are **not automatically exposed as individually timed C wrappers** just because other engine functions use wrappers. The six-step `move_it:33330–33337` ends with a **tail jump** to `move_vehicles`; naive C wrappers or an extra `ret` risk changing registers, stack or update scheduling. Any future instrumentation of internal assembly requires an explicitly scoped disposable diagnostic build, verified calling conventions and binary disassembly/test evidence.

The wrapper generator has a debug/log output option (`mkwrappers`), but logging every wrapper call cannot be assumed representative of uninstrumented performance and does not directly time the internal assembly functions. Do not use it as sole evidence that explosions have a particular hotspot.

## MAIN-PROF-02 — Scheduler timing is a separate quantity from work time

`game_update_full` (`game.c:141–196`) uses `SDL_GetTicks()` and `SDL_Delay()`, a shared static `last_frame_ticks`, and computes `frame_duration = 1000 / max_fps`. Tactical `game_update()` requests `max_fps=16` (integer **62 ms** target); the menu wrapper requests 30 FPS (integer **33 ms**). The conditional elapsed expression adds 2 ms and skips deliberate sleeping if already late. These code facts are **not measurements of CPU work**. In the tactical assembly loop, game simulation and presentation/drawing paths precede `ac_game_update`, which makes timing the scheduler alone insufficient to isolate collision/animation/render costs.

Do not equate an observed 62 ms tick setting with an actual stable 16 FPS delivery. A future diagnostic must separately measure CPU stage timings, delay/event processing, frame presentation, input latency and observed frame intervals; use a monotonic high-resolution timer verified in the chosen Windows runtime. Validate probe overhead against an identical noninstrumented control. Merely adding logs or changing `game_update` is not an accepted optimisation.

## Minimum evidence gates before proposing a change

1. **Asset extent gate** (private/legal data copy only): load actual `hfra-0.ani`, `hele-0.ani`, `hsta-0.ani`, verify decoded sizes and field formats, check all reachable `starts_ani` lookups and the two-step frame-chain bounds. Never commit commercial assets.
2. **Negative-resource gate** (synthetic disposable resources): missing file, short positive read, corrupted RNC, zero-length frame table, out-of-range start selector and next-frame index; record precise loader failure and whether execution reaches effect/render consumer. Do not claim a crash without reproduction.
3. **Profiler gate**: separately approved 32-bit Windows diagnostic branch/copy; confirm symbol mapping and wrapper assembly ABI, save/restore all relevant registers, preserve tail-jump flow and 16 FPS; measure six movement stages, effect allocation, animation, spatial list work, collision and audio independently from 4-plane expansion, SDL scale/copy/present and sleep.
4. **Scenario gate**: compare quiet scene → walking/traffic → firefight → single explosion → overlapping explosions → post-explosion decay, matched mission/camera/input/asset state, with uninstrumented baseline, per-stage elapsed distributions and probe-overhead checks.
5. **Regression gate**: only after real measurements, consider separately approved optimisation preserving explosions, damage, sound, RNG consumption, original mission cadence, original/American Revolt resources, mouse/keyboard and between-mission-only siloed campaign saves.

## Handover and verification

**Completed now:** GitHub source inspection of the original 85,803-line assembly blob, file/animation pointer provenance, precise index and record-width arithmetic, build/wrapper generation mechanism, CI 32-bit Windows pathway and scheduler limitations. This source-only handover contains executable test cases as *future work*, not claimed results.

**Not performed:** C/assembly edit, wrapper modification, compiler/build/CI run, binary symbol or ABI runtime verification, profiler, asset read, actual animation screenshot, gameplay, malformed-resource injection, memory analysis, save test, branch merge or changes to graphics/Mission Builder.

**Next MAIN checkpoint:** source-level review of mission animation table consumers and index production across other effect/object paths, then external/disposable Windows validation when approved resources and environment exist. Share the animation resource contract with the Graphics Audit and Mission Builder handovers; **do not** write into their branches or implement a shared loader change without approval.

Protected v0.7 remains unchanged. The three development workstreams remain separate.
