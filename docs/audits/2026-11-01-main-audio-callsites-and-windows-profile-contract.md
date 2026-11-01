# Syndicate 2026 — Main Systems Audit: audio call-site checkpoint and Windows profiling contract

Date: 2026-11-01 (Australia/Brisbane). Workstream: main systems, main branch. Documentation only.

## Current evidence
Live reads of `engine/src/main.c` (blob `e9621c6594173569e78366553ddc2de820e407f8`), `engine/src/game.c` (`9f56221d527075fba2b1f2406fab60af28a4289a`), `engine/src/sound.c` (`d2f5946d11e11cb123846ee5e54ee4065bf1f07d`), `engine/bfsoundlib/src/bfaudio.c` (`56dd05654f6bb9b653fb81f0a1a3dab5088b6258`), and `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md` (blob `c31e31b716c4b95e22c2d93c6f14df1ec9dbee86`).

## Call-site conclusion / limits
- In the inspected C entrypoint, `main.c` calls `init_audio()` once at line 336, before `syndicate()`, then `game_quit()` at line 364. The old inline cleanup is disabled by `#if 0`.
- `game_quit()` calls `host_reset()`, `LbBaseReset()`, then `exit(0)`. Non-DOS `host_reset()` calls `FreeAudio()`.
- `sound.c` defines `init_audio` and invokes `InitAudio`, and the bfaudio layer invokes `FreeMusic()` followed by `FreeSound()`.
- The four explicitly read C files do **not** establish a normal between-mission in-process audio restart. This checkpoint has **not** exhaustively searched every assembly-generated/wrapper call site. GitHub code-search requests for `init_audio`, `FreeAudio`, and `AIL_shutdown` returned zero hits, so they cannot be interpreted as proof of repository-wide absence.
- Previous audits describe conditional stale OpenAL pool counters and non-deduplicated idle handler registration during hypothetical in-process restart. Keep them classified as failure/restart risks, not measured causes of explosion slowdown.

## Nonproduction Windows profiling specification — not implemented
**Invariant:** preserve protected v0.7 code and behaviour, tactical 16 FPS, original explosion creation/damage/visuals, original sounds, mouse/keyboard and between-mission-only isolated campaign saves. No release code changes or merge without review.

1. Record high-resolution monotonic timestamps and per-tick IDs for tactical update, between-tick frame wait, VBI wait, and renderer stages. Keep real processing time, intentional waits and SDL event time separate.
2. In the tactical stage, time `move_worlds`, `move_people`, `move_weapons`, `move_effects`, `move_objects`, `move_vehicles` separately (preserve assembler tail-jump/control flow). Record effects occupancy, fireball allocation call count and slot scans, same-pass dispatch, MapWho insertions, collision invocations and linked-list visits.
3. Record converted pixels and time for four-plane conversion, display buffer copy, optional SDL scaling and `SDL_UpdateWindowSurface`, plus surface-lock waits. Share findings with Graphics Audit without changing ownership of rendering.
4. Capture SDL pump count and event count, `LbWindowsControl` duration, audio `sound_update` and `AIL_API_timer` invocations, elapsed timer accumulation, digital `SS_serve` and music `XMI_serve` callback counts and wall time.
5. Track actual active sample/sequence counts and OpenAL queue/unqueue/update duration, but use bounded logging and quantify probe overhead. Keep error-path injection and buffer-ID ownership testing as a **separate** test suite, not part of ordinary benchmarking.
6. Compare a fixed replayable quiet scene, typical combat, single explosion and sustained stacked explosions with identical mission state and settings. Collect frame-time distributions (median, 95th, 99th percentile, worst and count over the nominal tick budget) and counts per tick; correlate with dynamic effect/collision/audio work. Preserve presentation fidelity.
7. Run a no-instrumentation comparison build and an instrumented build against identical workloads; avoid altering RNG, sound priorities, simulation updates, AI or timing with probes. Do not conclude that a subsystem is the bottleneck until distributions show its measured contribution.
8. On candidate improvements, require before/after playable Windows builds and visual/audio/regression checks; leave Mission Builder and Graphics branches independent. Coordinate cross-branch interface expectations through shared handover only.

## Verification
Performed: current GitHub C and prior source-backed audit read; specified diagnostic contract. Not performed: profiling probes, assembly-wide caller census, compilation, Windows runs, measurements, performance optimization or branch changes.

Next: verify instrumentation entrypoints and low-overhead output format in a dedicated disposable worktree/branch if authorized. Main protected baseline must remain untouched.
