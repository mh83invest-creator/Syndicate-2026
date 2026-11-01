# Syndicate 2026 — SDL2 surface setup and rebind failure-recovery matrix

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics source control-flow inspection, documentation only.

## Direct source evidence
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:400-411` releases prior secondary draw surface, clears `lbHasSecondSurface`, clears `lbDrawSurface`, and sets `lbScreenInitialised=false` at entry to a fresh mode setup.
- `sscreen.c:290-325` allocates or wraps a draw surface; on allocation failure it assigns `lbDrawSurface=NULL` and returns failure before setting `lbHasSecondSurface`. On successful allocation it sets `lbHasSecondSurface=true`. If later optional indexed-palette setup fails, it frees the draw surface, sets `lbDrawSurface=NULL`, and returns without clearing the secondary-surface flag.
- `sscreen.c:520-539`: mode setup calls `LbIScreenDrawSurfaceCreate(palette == NULL)`; on failure it calls `LbScreenReset()` and returns `Lb_FAIL`.
- `sscreen.c:617-639`: `LbScreenReset()` returns failure immediately if `lbScreenInitialised` is false. Therefore the mode-setup failure path can call reset while the screen is still marked uninitialised, meaning that reset's cleanup block is not reached. For *palette initialization failure*, the flag can therefore remain true while `lbDrawSurface` is NULL. Do not infer that palette failure is always reachable: mode setup passes `set_palette=(palette==NULL)`, so both this argument and `lbEngineBPP==8` are required.
- `sscreen.c:1014-1047`: app-managed `WScreen` rebinding compares buffer pointers and replaces the draw surface when changed. On creation failure the caller restores `lbDrawSurface=oldDrawSurf` but does not explicitly restore any changed `lbHasSecondSurface` flag; potential state depends on starting values and failure stage. The old surface is freed only after a successful replacement.

## Four-case static failure matrix
| Scenario | Expected control-flow result from source | Still requires test |
|---|---|---|
| Initial allocation failure in clean mode setup | Create returns failure with null draw pointer; reset invoked while uninitialised and returns failure; initial flag had been cleared | Actual SDL allocator failure and subsequent retry |
| Palette setup failure in clean mode setup (8-bit and set_palette) | Newly created surface freed, pointer null, secondary flag remains true; reset may short-circuit because screen not initialized | Can actual palette API fail; later retry/reset consistency |
| Allocation failure during app WScreen rebind | Create overwrites draw pointer with NULL; rebind restores old pointer and returns failure | Flag consistency and subsequent frame swap |
| Palette failure during app WScreen rebind | New surface freed, pointer NULL, rebind restores old pointer; flag remains true | Old surface still valid, retry, palette/swap and cleanup |

## Conclusions
These are **source-verified state transitions**, not observed runtime failures, and do not prove that the error paths are reachable in the current Windows executable. No game behaviour was changed. Native fault injection should capture `lbScreenInitialised`, `lbHasSecondSurface`, draw and window surface pointers before and after failure, then confirm clean reinitialization and safe swaps.

## Verification
PASS: direct source reading of setup, creation, reset and rebind code; conditional control-flow derivation.
NOT DONE: actual failure injection, compilation, Windows SDL/runtime, gameplay/screenshot checks.
FILES CHANGED: this report and graphics audit continuity index only; protected baseline, Mission Builder and other audit branches unchanged.
