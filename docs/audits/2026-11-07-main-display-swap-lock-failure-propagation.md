# Syndicate 2026 — Main Systems Audit: Display Swap and Lock Failure Propagation

Date: 2026-11-07 (Australia/Brisbane)
Workstream: main/core systems; documentation only
Status: new source-level failure-path audit. No reproduced crash, hang, slowdown, or driver failure.

## Scope and prior work
The original tactical tick-to-render ordering was already established in `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md` (commit `ff102830c4d4b0c90d22decfdbb7bc77a971c129`). Do not call that ordering a new discovery. The 7 November frame-pacing report also covers routine performance stage boundaries. This report examines the independently unresolved **error propagation and lock recovery** behaviour.

Direct source reads on `main`:
- `engine/src/display.c` blob `9c6ba018f31d67b394c5973d05d6e51824171b68`, `display_lock`, `display_unlock`, `swap_wscreen`.
- `engine/src/game.c` blob `9f56221d527075fba2b1f2406fab60af28a4289a`, `game_update_full`.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` blob `ed6b6768ef6ebdb770ec9512ed315f83f5424852`, `LbIScreenSurfaceRestoreLost` (738–746), `LbScreenLock` (654–679), `LbScreenUnlock` (681–696), `LbI_SDL_BlitScaled` (976–1007), `LbIScreenDrawSurfaceCheck` (1014–1048), `LbScreenSwap` (1050–1086).

## CORE-DISPLAY-01 — Surface restore failure is ignored by LbScreenSwap

`LbIScreenSurfaceRestoreLost` assigns `lbScreenSurface = SDL_GetWindowSurface(lbWindow)`; when NULL it logs and returns `Lb_FAIL`. `LbScreenSwap` calls this function **without checking the result** and continues through draw-surface checks, mouse cursor work and possibly the conditional `LbI_SDL_BlitScaled` and `SDL_UpdateWindowSurface`. A second surface active at that point would make the unresolved screen-surface pointer particularly relevant. This creates a conditional invalid-surface path, not a demonstrated Windows crash.

## CORE-DISPLAY-02 — Draw-surface check/recreation failure is also discarded

Without `LB_WSCREEN_CONTROL`, `LbIScreenDrawSurfaceCheck` compares its current surface pixels against `lbDisplay.WScreen`. On mismatch it attempts `LbIScreenDrawSurfaceCreate(true)`; if the creation fails it restores `lbDrawSurface = oldDrawSurf` and returns `Lb_FAIL`. `LbScreenSwap` ignores that return value as well. This may allow subsequent presentation to use a surface whose expected app-buffer association has not been restored. Whether the build defines `LB_WSCREEN_CONTROL`, and which SDL surface configurations appear at runtime, must be confirmed.

## CORE-DISPLAY-03 — Custom SDL scaling can continue after unsuccessful surface locks

`LbI_SDL_BlitScaled` has early routes for 1:1 dimensions (SDL_BlitSurface) and non-8bpp sources (SDL_BlitScaled). The custom 8bpp scaling path attempts `SDL_LockSurface(src)` and `SDL_LockSurface(dst)` when SDL_MUSTLOCK is true. For either negative return, it **logs but does not return**. It then passes pixel pointers to custom conversion and unconditionally calls `SDL_UnlockSurface` for any surface satisfying SDL_MUSTLOCK, before returning `0`. Hence its caller's `blresult < 0` check will not detect those custom-lock failures. This is a conditional failure handling issue, not proof that those failures occur during ordinary play.

## CORE-DISPLAY-04 — Caller discards presentation result and has an unbounded lock-retry path

`display.c:swap_wscreen` saves `was_locked=LbScreenIsLocked()`, locks if previously unlocked, copies one framebuffer variant, calls `LbScreenUnlock()`, then `LbScreenSwap()` **without inspecting return status**. If it was locked earlier, it executes `while (LbScreenLock() != Lb_SUCCESS) ;` with no limit, delay, event pump, or explicit failure return. A persistent failure of `LbScreenLock` can therefore lead to an unbounded busy retry in that branch. The branch depends on the incoming lock state; do not claim every screen swap can hang. In addition, `display_lock` and `display_unlock` themselves return void and discard library results, including those called from the 16 FPS pacing path.

## Risk separation
- This is **reliability/recovery** risk during errors such as unavailable window surfaces or failed locks, not a measured explanation for explosion-associated frame slowness.
- Do not equate the callback's return being ignored with certain null dereference; exact branch, null-safety of helpers, video mode and surface state require runtime evidence.
- No direct engine modifications are authorised by this report. A future fix could alter error behaviour, input handling, recovery timing and resource ownership, requiring independent review.

## Future non-destructive verification gates (not executed)
1. Identify exact Windows build defines including `USE_SDL2`, `LB_WSCREEN_CONTROL`, current surface mode and output pixel formats.
2. In a disposable diagnostic build, independently simulate SDL_GetWindowSurface failure, draw-surface recreation failure, source/destination lock errors and repeated LbScreenLock failures. Preserve real user installation and original saves.
3. Confirm that normal success path, fullscreen/windowed toggles and resizing match v0.7 behaviour; record return propagation, lock counts, surface ownership and any hangs.
4. Measure explosion performance independently using previously agreed high-resolution stage profiling; do not treat these failure modes as performance benchmark evidence.
5. Evaluate separately approved bounded recovery/failure propagation changes with graphics workstream before changing screen internals.

## Verification and limits
Performed: direct reads of source functions and exact control-flow comparison; existing 1 November performance report checked to avoid duplicated findings.
Not performed: Windows compilation, CI, runtime error injection, gameplay, graphics screenshots, profiling, asset tests, any source edits, branch merges.
Protected: v0.7 baseline, tactical 16 FPS, mouse/keyboard, original campaigns and existing saves; Graphics and Mission Builder branches untouched.
Next main checkpoint: verify conditional feature flags and call-site-level error propagation as source allows; continue sound/event and explosion runtime-cost audit before proposing fixes.
