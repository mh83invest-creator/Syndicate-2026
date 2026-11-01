# Syndicate 2026 — SDL1 focus palette restoration event chain

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only source audit, no executable changes.

## Verified SDL1 event path
- `engine/bflibrary/src/x86-win-sdl/swindows.c:98-119`: `WEvent(const SDL_Event *)` handles `SDL_ACTIVEEVENT`. If `ev->active.state & SDL_APPACTIVE`, the handler updates `lbAppActive`, logs activity, and calls `LbInputRestate`.
- In the same case at lines 110-113, when `lbAppActive && lbDisplay.Palette != NULL`, it invokes `LbIPaletteRestoreLost()` and **discards the return value**. The event handler returns `Lb_SUCCESS` regardless of whether the palette restoration succeeded.
- `swindows.c:131-151`: `LbI_ProcessEvent` routes `SDL_ACTIVEEVENT` and `SDL_SYSWMEVENT` to `WEvent`. This establishes the SDL1 event-to-restoration call chain in source.
- `engine/bflibrary/src/x86-win-sdl/spalette.c:111-127`: `LbIPaletteRestoreLost` attempts `SDL_SetColors` on the SDL screen surface if reported screen mode is <=8 BPP, logs failure and returns `Lb_FAIL` in that case. This result is not propagated by `WEvent`.
- `engine/bflibrary/src/general/gpalette.c:38-106`: timed and stepped palette fades ignore `LbPaletteGet` and `LbPaletteSet` returns; timed mode additionally ignores screen-swap return. The prior report describes possible zero divisor when `fade_to==0` but **reachability from actual callers has not been established**.

## Current limits
This proves SDL1 focus-recovery helper is connected to the event dispatch, but NOT that Windows build uses SDL1 rather than SDL2, a palette loss occurs, or a palette-restore failure was reproduced. Inspection of `engine/src/main.c` and `engine/src/game.c` did not yield direct `LbPaletteFade` calls; comprehensive calling-site enumeration was not completed. Do not claim the fade divisor is reachable with zero.

## Verification ledger
PASS: GitHub source inspection of SDL1 event handler and dispatcher, restore helper, selected game entry files.
NOT RUN: Windows gameplay, forced SDL focus/palette failure, screenshots, caller graph completeness, executable backend confirmation.
CHANGED: this report and graphics audit continuity index only; v0.7, Mission Builder and other branches untouched.
NEXT: search other modules for fade callers and evaluate zero divisor; identify active Windows SDL backend then test focus restoration under native failure injection.
