# Syndicate 2026 — Main Systems Audit: Frame Wait Timing Semantics

Date: 2026-11-01 (Australia/Brisbane)
Workstream: Main Systems Audit; documentation only.
Evidence: direct live read of `engine/src/game.c` lines 141–227 on main; previous report `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md`.
Scope: C scheduler arithmetic and what it does **not** establish about explosion slowdown.

## Verified source observations

1. `game_update()` calls `game_update_full(true, 16)`; `game_update_menu()` calls `game_update_full(true, 30)`. This preserves separate nominal gameplay/menu cadence. Integer `1000 / max_fps` produces 62 ms and 33 ms thresholds, respectively.
2. `last_frame_ticks` is function-static, shared by both call paths; it is set after the wait logic from `SDL_GetTicks()`. There is no separate per-mode timestamp or mode-transition reset in this function.
3. `display_unlock()` and `game_handle_sdl_events()` occur before `start_ticks = SDL_GetTicks()`. The elapsed comparison is `start_ticks - last_frame_ticks + 2`, so event handling preceding that sample contributes to the measured elapsed interval; it does not by itself prove any recurring defect.
4. The wait branch executes only if `wait && last_frame_ticks != 0` and adjusted elapsed is under `frame_duration`. If work already runs over the nominal interval the code skips waiting, rather than making simulation run faster or catching up by extra ticks.
5. `min_sleep_time = 25`, `max_sleep_time = 50` (integer milliseconds). In the positive-wait branch, `f` is computed from remaining time, and the calculated rounded `base_sleep_time` is used as a cap for each `SDL_Delay`. It is a delay chunk, **not** a guaranteed 25–50 ms sleep, nor an FPS measurement.
6. Each delay iteration relocks the display, handles SDL events and unlocks again; the measured `SDL_GetTicks` delta is subtracted from remaining wait time. SDL timer resolution, event cost, system scheduling and display locks can affect realised cadence. Once the wait ends, `last_frame_ticks` is updated and the display is relocked.
7. There are no timings or counters in this function that attribute over-budget frames to fireballs, collisions, audio, conversion or the SDL window present path. It cannot establish which causes the reported explosion-heavy slowdown.

## Diagnostic controls before any engine change

- In a disposable diagnostic **32-bit Windows** build, record per-tick monotonic elapsed times at main loop entry, after gameplay simulation, at conversion/presentation boundaries, entering `game_update_full`, before delay, and on return. Record events duration, requested wait, actual waited time, and total tick-to-tick interval.
- Log count of `SDL_Delay` calls and over-budget intervals, with no gameplay-state or random-number mutation.
- Include 16-FPS tactical versus 30-FPS menu transitions; identify initial tick and timestamp sharing. Do not infer a transition bug from static source alone.
- Compare identical tactical scenes (idle, gunfight, single and stacked explosions) and the same executable/settings; compare instrumented and uninstrumented overhead.
- Cross-reference previous effect-pool, MapWho and collision findings, but do not label them a measured bottleneck without timings.

## Change control and verification

Direct current GitHub `engine/src/game.c` read performed; findings above are based on that source. The previous assembly report was also read. Targeted assembly excerpts returned empty on this connector path in the current session; this stage does **not** claim fresh direct assembly verification. No source modifications, Windows build, runtime benchmark, graphics work, Mission Builder work, or game-save changes.

Preserve protected v0.7 gameplay behaviour, tactical 16 FPS, original explosion effects and damage, mouse/keyboard, legacy campaigns and between-mission-only saves. This report documents investigation, not implementation.

Next: obtain independently readable assembly excerpts or build an isolated timing probe only after coordinated change approval; trace audio/rendering timing separately without assuming source-level bottlenecks.
