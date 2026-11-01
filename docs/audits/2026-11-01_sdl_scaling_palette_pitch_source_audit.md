# Syndicate 2026 — SDL scaling, indexed palette and surface-pitch source audit

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only source audit. No engine change.

## Sources directly inspected
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` lines 878-1010; source scaling dispatch, 8-bit conversion path.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c` lines 800-832; corresponding SDL1 scaling dispatch.
- `engine/bflibrary/src/x86-win-sdl2/spalette.c` lines 50-103; indexed palette conversion and distribution.
- Previously verified: `engine/src/display.c` and SDL1/SDL2 `LbScreenSwap` traces.

## Findings
- SDL2 `LbI_SDL_BlitScaled` first uses `SDL_BlitSurface` for equal source/destination dimensions, and delegates non-8-bit source scaling to `SDL_BlitScaled`. For 8-bit sources it locks surfaces where required, picks a destination bytes-per-pixel value, and calls the custom indexed-to-indexed or indexed-to-true-colour conversion path.
- SDL1 counterpart uses a custom conversion/scaling path for 8-bit sources and logs unsupported non-8-bit source formats. It also uses `SDL_BlitSurface` at equal dimensions.
- SDL2's custom 8-bit-to-8-bit scaler `LbI_SDL_BlitScaled_to8bpp` accepts raw pixel buffers and source/destination widths and heights, not explicit source/destination `pitch` values; source-row and destination offsets are width-based. This is a potential surface-layout assumption requiring confirmation against actual SDL allocations; it is NOT evidence of a reproduced visual or memory fault.
- SDL2 `LbPaletteSet` masks three input components per indexed colour with `0x3f`, shifts each channel left two bits into `SDL_Color`, updates `lbPalette`, and sets colours on the screen surface when it is indexed (<= 8 bpp). When there is a second drawing surface with engine BPP <= 8 it updates that indexed palette as well.
- These findings apply to the inspected code paths; the precise Windows build backend/flags and real surface pitch/format have not been confirmed by building or running it.

## Constraints before graphics implementation
- Trace SDL setup surface creation, `pitch` and `BytesPerPixel` for draw/screen surfaces; determine whether width-based scaling is valid in all supported modes.
- Trace indexed-to-true-colour conversion and screen palette update timing for fades/night lighting.
- Compare actual Windows SDL build config and rendered output at both 640x480 and 320x200.
- Keep v0.7 protected gameplay baseline, 16 FPS tactical simulation, original/American Revolt campaign isolation, and mission builder unaffected. No original commercial graphic assets in public GitHub.

## Verification ledger
PASS: direct GitHub source inspection of SDL1/SDL2 scaling dispatch and SDL2 palette setter.
NOT RUN: compilation, Windows runtime, pitch measurement, screenshot equivalence, performance profiling, private retail-asset validation.
CHANGED: this report and graphics continuity index only.
NEXT: source trace SDL surface creation and pixel/pitch ownership, then active build backend configuration.
