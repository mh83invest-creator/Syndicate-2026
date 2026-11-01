# Syndicate 2026 — Main Systems Audit: assembly probe boundaries and evidence limits
Date: 2026-11-01 (Australia/Brisbane). Documentation-only on main.

## Source reads and limitation
Attempted four direct GitHub ranged reads of `engine/src/syndre.sx`: lines 220–280, 33320–33350, 19975–20015, and 20740–20785. All four returned **empty content**. Consequently this checkpoint **cannot** claim fresh per-instruction register/flag liveness verification of those routines.
Read-back verified predecessor: `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md` (blob `c31e31b716c4b95e22c2d93c6f14df1ec9dbee86`), which records an earlier source-based analysis of those assembly routines. Also freshly read `engine/conf/wrappers_game.conf` (blob `1be08603b34cf2361780de36ecfa677e442e9ce4`).

## Findings supportable with provenance
**From earlier source-backed audit, not freshly re-read here:** `move_it` is recorded as chaining `move_worlds`, `move_people`, `move_weapons`, `move_effects`, `move_objects`, ending in a vehicle-stage tail jump around `syndre.sx:33330–33337`. Prior audit also records `move_effects` scanning 256 effect slots, `init_fireball` scanning 128 slots, and a collision function traversing a conditional 3×3 MapWho neighborhood. Do not reinterpret lexical call sites as measured dynamic calls.

**Freshly inspected wrapper configuration:** `wrappers_game.conf` declares wrappers for `game_update`, `game_update_menu`, `swap_wscreen`, `update_vscreen_whole_vres16`, `LbScreenSwap` and `LbScreenWaitVbi` as C convention targets. This supports coarse boundary timing opportunities; it does not establish ABI safety of probes inserted into arbitrary internal assembly branches.

## Recommendation for a disposable diagnostic build (not implemented)
1. First instrument established C-level frame/event/display/audio boundaries, preserving original 16 FPS tactical and 30 FPS menu requests; measure intentional waits independently.
2. Before instrumenting internal assembly collision or explosion routines, obtain readable exact source or actual compiler-produced disassembly and review all branch paths, live register values, flags and stack discipline. Particularly protect the `move_it` tail jump, same-pass effect dispatch and simulation RNG.
3. Compare identical scenarios with and without probes, quantify probe overhead, and preserve original explosion visuals/damage, sounds, campaign state and save behaviour.
4. Coordinate display-specific results with Graphics Audit, but do not alter that workstream or Mission Builder.

## Verification
Completed: four failed ranged-read attempts documented, predecessor report re-read, wrappers configuration re-read. Not completed: direct new inspection of exact assembly instructions, generated Windows wrapper output, build, gameplay test or profiling. No engine source or build configuration changes.
