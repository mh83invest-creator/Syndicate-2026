# Syndicate 2026 — Main systems audit: SDL surface recovery return contracts
Date: 2026-11-01. Main branch, documentation-only.

## Directly inspected sources
`engine/bflibrary/src/x86-win-sdl2/sscreen.c` Git blob `ed6b6768ef6ebdb770ec9512ed315f83f5424852`, particularly lines 654–696, 738–746, 1015–1047, 1050–1085; `engine/src/game.c`, blob `9f56221d527075fba2b1f2406fab60af28a4289a`.

## Findings
1. `LbScreenLock` (654–679) checks `lbScreenInitialised`, returns `Lb_FAIL` if SDL reports `SDL_LockSurface` failure, and under `LB_WSCREEN_CONTROL` clears window/screen pointers on lock failure. `LbScreenUnlock` (681–696) checks initialisation and unlocks a lock-required surface; its own return is not proof that the preceding lock succeeded.
2. `LbIScreenSurfaceRestoreLost` (738–746) obtains `SDL_GetWindowSurface(lbWindow)`, returns `Lb_FAIL` on null and `Lb_SUCCESS` otherwise.
3. `LbIScreenDrawSurfaceCheck` (1015–1047) returns `Lb_OK` unconditionally when `LB_WSCREEN_CONTROL` is defined; without that macro it compares draw surface pixels against `lbDisplay.WScreen`, and may recreate the draw surface. It returns `Lb_FAIL` when creation fails. On the non-control branch, its `SDL_LockSurface` failure logs an error without immediately returning and an unlock may follow.
4. `LbScreenSwap` (1050–1085) calls `LbIScreenSurfaceRestoreLost()` and `LbIScreenDrawSurfaceCheck()` but discards **both** results; it then calls `LbMouseOnBeginSwap`, sets `ret = Lb_SUCCESS`, conditionally invokes `LbI_SDL_BlitScaled`, and updates the window surface. The status of the earlier recovery checks is not propagated locally. This presents a **conditional** failure-chain candidate, not a reproduced crash.
5. The custom scaler's surface-lock failure continuation was previously documented in `2026-11-01-main-display-pipeline-scaling-and-surface-guards.md`. Surface restore, check, lock, scaling, SDL presentation, event processing, and intentional pacing wait must be separately instrumented for attribution. No measured performance bottleneck is established.

## Boundaries and verification
Direct live source inspection on GitHub: completed. Build, injected SDL failure, gameplay test, profiler, frame measurements, source edit: NOT performed. No observed Windows crash. Original v0.7 and tactical 16 FPS are protected. Graphics team owns actual rendering/palette changes; Mission Builder remains untouched. Preserve original explosion effects and damage. Do not implement speculative runtime patches under a documentation-only audit.

## Next
Read the audio callback/SDL_mixer (or alternate audio backend) source to distinguish asynchronous sound mixing from per-tick game effects before designing a diagnostic Windows test. Verify build macros and surface locking modes. Run controlled, disposable failure injections only after agreeing changes.
