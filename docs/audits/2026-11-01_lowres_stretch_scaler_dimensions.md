# Syndicate 2026 — Low-resolution stretch dimensions and SDL2 scaler selection

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: source-backed graphics audit only; no game code changes.

## Verified source
- `engine/src/display.c:72-95`: `AppScreenSetup` maps game modes 19 and 18 to 320x200x8 and 640x480x8, respectively, and calls `LbScreenSetup(mode, mdinfo->Width, mdinfo->Height, GraphicsPalette)` with those underlying drawing dimensions.
- `engine/src/display.c:119-125`: `display_set_lowres_stretch(true)` calls `LbScreenSetMinScreenSurfaceDimension(400)`; false calls the same setter with 1.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:87,238-240`: minimum screen-surface dimension defaults to 1 and setter updates it.
- `sscreen.c:328-343`: SDL2 `LbIGetScreenModeDimensions` starts with `mdinfo->Width,Height`, uses `min(width,height)`, and if that minimum is below `lbMinScreenSurfaceDimension`, rescales both dimensions by factor `lbMinScreenSurfaceDimension/minD` using integer division.
- Thus if the low-res stretch setting is enabled, logical 320x200 produces requested window dimensions **640x400**, while drawing dimensions passed from `AppScreenSetup` remain **320x200**. Logical 640x480 has minimum dimension 480, so no stretch is requested at the 400 threshold.
- `sscreen.c:520-535` can create a second drawing surface when dimensions differ, and `sscreen.c:976-1006` routes different-size 8-bit indexed sources through custom indexed scaling; the true-colour destination case selects `LbI_SDL_BlitScaled_totcbpp` with its previously flagged fixed-`long` store and no SDL pitch parameters.

## Specific conditional path
`display_set_lowres_stretch(true)` + 320x200x8 logical source -> 640x400 requested SDL window surface. If window surface actually has >1 BytesPerPixel, custom true-colour scaler is selected upon swap with secondary surface. If window surface is indexed 8-bit, custom indexed scaler is selected instead. The SDL window surface actual format, pitch and dimensions must be measured to establish real runtime branch.

## Outstanding questions
- Whether the verified Windows v0.7 executable actually enables low-res stretch through its entry-point/configuration and uses that mode during gameplay is **not established by this report**.
- Actual SDL surface type/format/pitch and scaler dispatch during Windows gameplay **not measured**.
- This is a deterministic mathematical consequence of verified sizing source, not a Windows reproduction of corruption.

## Verification ledger
PASS: direct graphics-branch source reads of app mode mapping, stretch toggle, SDL2 sizing, draw surface construction and scaler dispatch.
NOT RUN: native Windows game, low-res stretch screen capture, SDL runtime format instrumentation, failure reproduction.
CHANGED: this report and graphics continuity index only; game code, protected baseline and other branches untouched.
NEXT: locate calls to `display_set_lowres_stretch` and record actual toggle defaults; then runtime SDL surface dimensions, BPP and pitch.
