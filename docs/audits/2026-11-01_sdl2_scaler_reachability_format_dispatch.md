# Syndicate 2026 — SDL2 scaler format dispatch and real-surface reachability

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics source audit only.

## Source verified
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:976-1006`: `LbI_SDL_BlitScaled` dispatches to `SDL_BlitSurface` if sizes match; to standard `SDL_BlitScaled` if sizes differ and source `BytesPerPixel != 1`; and otherwise to a custom scaler. Destination `BytesPerPixel==1` selects indexed-to-indexed; any other destination pixel byte count selects custom indexed-to-true-colour conversion.
- `sscreen.c:962-965`: indexed-to-true-colour scaler writes a `long` at `dst_buf+dst_offset`, then advances by `dst_bpp`; no row-pitch arguments. Prior standalone synthetic and source-derived tests document conditional write-width and padded-row risks.
- `sscreen.c:497-512`: window surface obtained with `SDL_GetWindowSurface`; `lbScreenSurface` and `lbDrawSurface` initially alias it. Secondary draw allocation may replace `lbDrawSurface` in `sscreen.c:520-535`; with application-managed WScreen configuration it is created unconditionally by the visible preprocessor structure.
- `sscreen.c:1050-1085`: frame swap passes separate draw/window surfaces through `LbI_SDL_BlitScaled` if `lbHasSecondSurface`, then calls `SDL_UpdateWindowSurface`.
- `sscreen.c:450-476`: source comments specifically acknowledge SDL may provide 32-bpp surfaces despite an 8-bit requested fullscreen display mode; `sscreen.c:523-530` also compares the actual surface BitsPerPixel rather than presuming requested format when library controls WScreen.

## Dispatch matrix
1. Same dimensions: `SDL_BlitSurface` (no custom scaled path).
2. Different dimensions and source >1 BPP: `SDL_BlitScaled`.
3. Different dimensions and source 1 BPP and dest 1 BPP: custom 8-bit-indexed scaler.
4. Different dimensions and source 1 BPP and dest >1 BPP: custom indexed-to-true-colour scaler, subject to prior width/pitch concerns.

## Evidence boundary
This establishes **conditional source reachability**, not the active runtime case for the shipped Windows executable. Requested graphics mode, actual window surface format, dimensions, pitch and executable backend/build remain unverified. SDL1 has its own implementation; this report focuses on SDL2. Neither a crash nor visible graphics corruption is claimed.

## Next checks
Identify actual Windows build SDL backend and log window/draw `w`, `h`, `format->BitsPerPixel`, `format->BytesPerPixel`, `pitch`, and path dispatch during mode changes using an isolated diagnostics build, without modifying the protected baseline.

## Verification
PASS: direct GitHub audit-branch reads of SDL2 dispatch, window surface acquisition, setup and swap call.
NOT RUN: native Windows executable, SDL surface instrumentation, screenshot or frame output tests.
CHANGED: report and graphics continuity index only; no engine or parallel branch edits.
