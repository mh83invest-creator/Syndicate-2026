# Syndicate 2026 — SDL2 surface reset, mode-change and WScreen rebind lifetime

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: static graphics source audit, no source-code changes.

## Confirmed source findings
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:617-639`: `LbScreenReset` fails if not initialized, suspends the mouse, unlocks a locked screen, frees `lbDrawSurface` **only if `lbHasSecondSurface`**, then clears the secondary-surface flag, draw/window surface pointers and initialized state. The code explicitly does not free SDL-managed window surface.
- `sscreen.c:400-411`: during the mode setup path, any existing secondary draw surface is freed and the flag cleared; `lbDrawSurface` and initialized state reset before mode availability and window setup.
- `sscreen.c:497-512`: window surface is obtained from `SDL_GetWindowSurface`; secondary drawing surface may later be allocated/wrapped separately.
- `sscreen.c:727-745`: `LbIScreenSurfaceRestoreLost` obtains a potentially replaced window surface (such as after a window operation) using `SDL_GetWindowSurface`; reports failure if none.
- `sscreen.c:1014-1047`: with app-managed WScreen, `LbIScreenDrawSurfaceCheck` compares existing surface pixels against application WScreen and, when different, calls `LbIScreenDrawSurfaceCreate(true)`. On create failure it restores `lbDrawSurface=oldDrawSurf` and returns `Lb_FAIL`; on success it frees old draw surface. The creation function (`sscreen.c:290-325`) assigns to global `lbDrawSurface` before all validation and sets `lbHasSecondSurface` before palette initialization; its failure paths can therefore interact with global flag/handle state.
- Previous audit verified both `LbScreenSwap` and `LbScreenSwapClear` currently ignore return values of restoration/rebind calls, so any failure-state consistency is relevant.

## Risk assessment
Separate draw-surface freeing and SDL-managed window-surface ownership are intentional. The rebind failure path restores the old pointer but does not visibly restore all modified globals (notably `lbHasSecondSurface`); depending on where creation fails, the state could be inconsistent. This is a potential failure-path concern, **not a reproduced error** or proof of leaked/freed resources during gameplay. Observe real SDL errors and generated Windows configuration before proposing corrective code.

## Verification ledger
PASS: direct GitHub reads of SDL2 reset, mode setup, draw creation, restore and rebind.
NOT RUN: Windows build, fault-injection, SDL resource lifetime instrumentation, focus/fullscreen gameplay, screenshots.
CHANGED: report and graphics continuity index only on graphics audit branch. No engine, protected baseline or sibling branch edits.
NEXT: inspect `LbIScreenDrawSurfaceCreate` failure cleanup with exact global handle and secondary-surface flag values, then identify controlled failure tests.
