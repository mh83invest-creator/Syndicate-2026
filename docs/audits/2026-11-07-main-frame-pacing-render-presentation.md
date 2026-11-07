# Syndicate 2026 — Main Systems Audit: Frame Pacing and Rendering Presentation Boundary

Date: 2026-11-07 (Australia/Brisbane)
Branch: main, documentation only
Status: source-backed static audit, no runtime timings.

## Directly inspected source
- `engine/src/game.c`, SHA `9f56221d527075fba2b1f2406fab60af28a4289a`: `game_update_full`, `game_update`, `game_update_menu`.
- `engine/src/display.c`, SHA `9c6ba018f31d67b394c5973d05d6e51824171b68`: `swap_wscreen`, `blit_wscreen_vres16`, `blit_wscreen_mcga`, `update_vscreen_whole_vres16`.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c`, SHA `ed6b6768ef6ebdb770ec9512ed315f83f5424852`: `LbScreenSwap` and `SDL_UpdateWindowSurface`.
- `engine/src/timer.c`, SHA `73b7edfbf741c48cdf6c78d1e2b6d8239ed4012c`: SDL tick-derived clocks.
- `engine/src/main.c`: native entry and assembly `syndicate()` handoff.
- Existing main audit `docs/audits/2026-11-01-main-fireball-same-tick-cascade.md` (source-verified effect-pool ordering and potential same-pass work). No new assembly conclusion is claimed here; assembly line-range fetch not repeated for this report.

## CORE-PACE-01 — The two pacing targets are separate at the public call sites, but share static history

`game_update()` invokes `game_update_full(true,16)`; `game_update_menu()` invokes `game_update_full(true,30)`. Inside `game_update_full`, `frame_duration = 1000 / max_fps` with integer division, giving **62 milliseconds** for 16 FPS and **33 milliseconds** for 30 FPS, rather than exact fractions (62.5 ms and 33.333 ms).

The function holds **one static `last_frame_ticks` value across both call sites**. If the menu and tactical loops alternate calls, the timing baseline crosses the transition; there is no observed reset on mode switch. This implies a possible single-transition pacing discrepancy, not proof of ongoing gameplay FPS drift or any explosion slowdown.

## CORE-PACE-02 — Sleep is conditional and measures only a limited timing interval

The pacing routine unlocks display, pumps SDL/window events and scaled mouse updates, then reads `start_ticks = SDL_GetTicks()`. It computes `last_frame_duration = start_ticks - last_frame_ticks + 2`. If below target, it delays in a loop; each delay temporarily locks/unlocks the display around additional event processing. At end it sets `last_frame_ticks = SDL_GetTicks()` and re-locks display.

The timestamps do **not** directly instrument duration of `move_effects`, fireball scans, collision, sprite drawing, or `LbScreenSwap`; they yield a timing interval containing work between pacing invocations. Thus simply logging this function's sleep calculation cannot identify the overloaded subsystem. If the interval is above target, no compensating sleep is used; the code does not itself restore lost simulation time.

The chosen `SDL_GetTicks()` source has millisecond granularity, so a future diagnostic build should use a high-resolution monotonic clock for individual work sections. No measurement exists yet.

## CORE-PRESENT-01 — Presentation has independent work from tactical effect processing

`display.c:swap_wscreen` obtains the lock state; if needed it locks; depending on `DrawFlags & DrwF_ScreenVres16`, it copies a 640×480 buffer or 320×200 buffer to `lbDisplay.WScreen`; it unlocks, calls `LbScreenSwap`, and restores an earlier lock with a retry loop when applicable.

In the selected SDL2 backend `sscreen.c:LbScreenSwap` includes lost-surface restoration/check, mouse swap hooks, conditional draw-to-screen surface blit, and `SDL_UpdateWindowSurface(lbWindow)`. This distinguishes software framebuffer copy, conditional scaling/blit and window presentation from effect allocation/collision. The actual profile cost depends on screen mode, surface configuration, driver and machine; none have been measured.

Note also `display.c:update_vscreen_whole_vres16` clears a 640×480 buffer and uses four nested bit-plane conversion passes (each 400 × 80 iterations) when invoked. Source inspection verifies its algorithm, **not** its per-frame invocation frequency. Do not attribute explosion slowdown to this conversion without verifying active call sites and timing.

## CORE-PROFILE-01 — Stage-separated diagnostic contract (future, not implemented)

A later separately approved Windows diagnostic build should use high-resolution monotonic timestamps and small count-only probes, preserving function order and game state:
1. Complete tactical step time around original move stages, with effect substep timers (`init_fireball` call counts/128-slot scans, newly allocated fireballs, same-pass type 41 processing and collision/movement).
2. CPU time spent in software scene composition and any bitmap/bitplane conversion, separately from `swap_wscreen` framebuffer copy.
3. Time in `LbScreenSwap`, including conditional SDL surface blit vs `SDL_UpdateWindowSurface`, and present frequency.
4. Event-processing and intentional `SDL_Delay` time inside `game_update_full`, recorded separately from actual work time.
5. Record frame intervals and over-budget tick counts under identical no-explosion, light-firefight, one-explosion, overlapping-explosion scenarios; compare instrumented vs uninstrumented Windows builds to estimate probe overhead.

Important limitations: Do not conflate 16 FPS simulation cadence with graphics refresh or use `SDL_GetTicks()` alone to assign microsecond-scale costs. Do not change effect scheduling, effects counts, collision, animations, random calls, visual appearance or timing as part of diagnostics. Confirm exact assembly hooks and Windows ABI before any proposed instrumentation.

## Verification and handover
Done: read current GitHub C source directly, traced frame pacing and selected SDL2 display presentation, cross-checked prior cascade audit.
Not done: assembly blob reinspection, runtime path-frequency proof, profiler runs, Windows compilation, CI, gameplay tests, benchmark numbers, source/asset modifications, branch merges.
Protected: mainline v0.7 behaviour and Windows compatibility, tactical 16 FPS pacing, mouse and keyboard, original campaigns, parallel Graphics and Mission Builder branches.
Next: trace active call sites for `update_vscreen_whole_vres16`, `swap_wscreen`, and tactical tick-to-render ordering; refine diagnostic sampling without deploying or changing gameplay.
