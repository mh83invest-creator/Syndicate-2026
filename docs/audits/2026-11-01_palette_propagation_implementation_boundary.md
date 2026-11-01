# Syndicate 2026 — Palette propagation evidence and implementation boundary

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only source audit. No engine changes.

## Verified code
- `engine/src/display.c:72-95`: AppScreenSetup passes GraphicsPalette to LbScreenSetup.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:290-325`: creation of indexed secondary drawing surface conditionally copies `lbPaletteColors` into the SDL2 surface palette via `SDL_SetPaletteColors` when `lbEngineBPP == 8 && set_palette`. SDL2 error criterion is negative return.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c:277-311`: analogous SDL1 helper calls `SDL_SetColors`; it tests return != 1.
- Both graphics backends include `bfpalette.h`, declare `extern SDL_Color lbPaletteColors[256]`, and have an explicit `LbPaletteSet(palette)` call during screen setup (SDL2 sscreen.c:560-568; SDL1 sscreen.c:451-459).
- The SDL2 custom scaler `sscreen.c:976-1004` passes `src->format->palette->colors` into the indexed-to-truecolour scaler. SDL1 analogue: `sscreen.c:805-830`. This confirms palette data are consumed from the source surface by the scaler; it does **not** establish how `LbPaletteSet` updates all internal or SDL surfaces.
- SDL2 rebind `sscreen.c:1014-1047` calls draw-surface creation with `set_palette=true` after application WScreen pointer changes.

## Source lookup result
Attempted direct GitHub fetch of `engine/bflibrary/src/x86-win-sdl2/spal.c` and `engine/bflibrary/src/x86-win-sdl/spal.c`; both 404. GitHub repository code searches for `LbPaletteSet` and `palette.c` returned no results. Those are **not proof that the implementation is absent**: code search is scoped to the default branch and may miss indexed files; exact file path has not been established.

## Conclusions / open questions
No source-backed conclusion about `LbPaletteSet` internal mutation order, rollback, colour-space conversion, or partial SDL palette update can yet be made. The existing normal screen setup late-failure path calls reset after the initialized flag is set, as documented in `2026-11-01_palette_set_late_failure_reset_order.md`; that is control-flow evidence, not successful failure-injection evidence.

NEXT: locate palette implementation via repository tree or full checkout (if accessible), then trace internal palette cache and SDL secondary/window palettes; run real SDL palette update and failure tests if available. Do not change graphics engine solely on speculation.

## Verification ledger
PASS: source reads of SDL1/SDL2 sscreen.c palette references and render consumption; unsuccessful candidate-file and code-search attempts recorded.
NOT TESTED: palette implementation, SDL API partial failure, Windows build, actual gameplay or screenshot output.
GitHub changed: this document and graphics handover index only. Protected v0.7, Mission Builder, main audit and other branches untouched.
