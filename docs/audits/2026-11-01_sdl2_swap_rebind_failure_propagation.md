# Syndicate 2026 — SDL2 swap and surface-rebinding failure propagation

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: documentation-only graphics source audit.

## Verified source facts
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:1014-1047`: when `LB_WSCREEN_CONTROL` is not defined, `LbIScreenDrawSurfaceCheck` captures existing draw surface, conditionally locks it, compares its pixel buffer with application `WScreen`, and recreates draw surface if different. If recreation fails it restores the old handle and returns `Lb_FAIL`.
- `sscreen.c:1025-1032`: SDL draw-surface lock failure is logged, but execution continues to pixel comparison and conditional unlock; there is no immediate failure return.
- `sscreen.c:1050-1085`: `LbScreenSwap` calls `LbIScreenSurfaceRestoreLost()` and `LbIScreenDrawSurfaceCheck()` at lines 1057-1058 and ignores both returns before mouse hooks, scaling blit and `SDL_UpdateWindowSurface`.
- `sscreen.c:1088-1124`: `LbScreenSwapClear` likewise ignores both calls' return values at lines 1095-1096, then executes scaling/window update; it performs `SDL_FillRect(lbDrawSurface, NULL, colour)` at line 1123 without checking the fill result, including following a prior reported presentation failure.
- Both swap paths *do* check scaled-blit and update-window return values and return a failure if one is negative. The risk is confined to the unchecked earlier restoration/rebinding steps and final fill, not a blanket absence of all error checking.

## Interpretation
These paths could conceal restoration or rebinding errors after window recreation or application buffer changes, but no actual failure, null dereference, stale-pointer access, or visible fault has been reproduced. A valid fix requires fault-injection tests and tracking of SDL surface lifetime on the actual Windows build. Do not alter behaviour, macros, screen buffers or timings as part of this audit.

## Verification ledger
PASS: direct GitHub source read through SDL2 swap/rebind complete relevant blocks.
NOT DONE: native Windows build, SDL error fault injection, focus/resize testing, actual game runtime or screenshots.
CHANGED: report and graphics continuity index only. Protected baseline, Mission Builder and core audit untouched.
NEXT: inspect reset/free lifecycle for the old and refreshed window/draw surfaces, followed by controlled native failure-path tests if build environment is available.
