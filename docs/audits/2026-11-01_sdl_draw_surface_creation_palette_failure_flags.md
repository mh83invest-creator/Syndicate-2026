# Syndicate 2026 — SDL draw-surface creation failure and secondary-surface flag consistency

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics source audit only; no engine changes.

## Directly verified source
- SDL2 `engine/bflibrary/src/x86-win-sdl2/sscreen.c:290-325` creates secondary surface by wrapping `lbDisplay.WScreen` via `SDL_CreateRGBSurfaceFrom` where applicable or allocating with `SDL_CreateRGBSurface`. Creation failure at lines 309-312 returns `Lb_FAIL`; successful creation sets `lbHasSecondSurface=true` at line 313. For 8-bit draw surfaces with `set_palette`, `SDL_SetPaletteColors` failure at lines 317-323 frees the new surface, assigns `lbDrawSurface=NULL`, and returns `Lb_FAIL` without resetting `lbHasSecondSurface`.
- SDL1 `engine/bflibrary/src/x86-win-sdl/sscreen.c:277-311` has the same ordering and cleanup pattern. SDL1 uses `SDL_SetColors` and the analogous failure path at lines 304-309 frees draw surface, nulls pointer and returns while leaving the secondary-surface flag set.
- Hence in either implementation **a palette failure after successful surface allocation leaves the flag true while the global draw-surface handle is null**. This is a source-level state inconsistency, not a confirmed real-game failure.
- Allocation failure returns before changing the secondary-surface flag; its effect depends on prior caller state. Mode setup has a separate reset path; WScreen-rebind caller (`SDL2 sscreen.c:1014-1047`) restores the old pointer on create failure but does not explicitly restore the flag. Do not infer all failure cases produce the same state.
- The screen-swap code checks `lbHasSecondSurface` before accessing draw surface, so an inconsistent state could matter if execution continues; actual error-path execution and subsequent reset behaviour remain untested.

## Verification and next tests
PASS: direct GitHub source reads of SDL1 and SDL2 create/palette failure branches and earlier documented SDL2 rebind/swap logic.
NOT RUN: forced SDL palette failure, allocation failure, native Windows/SDL integration, surface recovery validation, gameplay or screenshots.
Proposed isolated fault-injection matrix: (1) allocate failure from clean state, (2) palette failure from clean state, (3) palette failure during WScreen rebind with old draw surface, (4) rebind retry/reset, checking handle, flag and locked/initialized state after every call.
No code changes requested or made. Protected baseline and other branches untouched.
