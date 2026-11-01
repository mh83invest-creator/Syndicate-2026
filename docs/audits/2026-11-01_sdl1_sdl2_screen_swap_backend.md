# Syndicate 2026 — SDL1 and SDL2 screen-swap backend trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: static code audit, no implementation changes.

## Verified direct source
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:1050-1086` (`LbScreenSwap`): restores/checks SDL surface state with `LbIScreenSurfaceRestoreLost` and `LbIScreenDrawSurfaceCheck`; calls `LbMouseOnBeginSwap`; if `lbHasSecondSurface`, blits/scales `lbDrawSurface` to `lbScreenSurface` through `LbI_SDL_BlitScaled`; then invokes `SDL_UpdateWindowSurface(lbWindow)` on success; calls `LbMouseOnEndSwap` afterward.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c:875-909` (`LbScreenSwap`): checks the drawing surface, brackets the swap with mouse begin/end hooks, conditionally scales/blits via `LbI_SDL_BlitScaled` when `lbHasSecondSurface`, and presents with `SDL_Flip(lbScreenSurface)`.
- Both backends log failed blits and presentation errors. This source trace does not establish how often errors occur or that any observed visual defect exists.
- `engine/src/display.c:244-255` calls `LbScreenSwap` after copying high-resolution `VGABuffer` or MCGA `WScreen` into the library drawing buffer.
- Previous phase-alias trace establishes the exported assembly `VScreen` references `VGABuffer` with distinct phase offsets; no global equality is implied.

## Integration constraints
- Preserve cursor before/after hooks, SDL1 versus SDL2 backend semantics, `lbHasSecondSurface` conditional intermediate blit, indexed-colour surfaces and any scaling conversions. Avoid assuming SDL2 uses GPU textures: the observed path explicitly calls `SDL_UpdateWindowSurface`.
- New lighting must not assume writing after `LbScreenSwap` affects the already presented frame. Exact layering position, palette conversion, surface pitch and ownership must be traced and tested before an implementation proposal.
- Need check which backend is selected in relevant Windows build configurations; the presence of two implementations does not establish which specific executable is running.

## Verification
PASS: direct GitHub reads of both backend implementations and matching call sequence in `display.c`.
NOT RUN: build, Windows runtime, screenshot comparison, memory instrumentation, private retail asset decode, performance profiling.
FILES TO CHANGE: documentation report plus graphics continuity index only. No engine, gameplay, other audit branches, protected baseline or copyrighted original graphics modified.
NEXT: trace SDL `LbI_SDL_BlitScaled`, surface format/pitch and palette application, plus active Windows backend build selection.
