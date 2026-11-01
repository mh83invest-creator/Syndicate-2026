# Syndicate 2026 — Indexed scaler activation and destination-format test gate

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-source audit only; no gameplay/rendering implementation changed.

## Verified SDL2 dispatch and setup
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:976-1006`: `LbI_SDL_BlitScaled` first checks identical source/destination dimensions; these take `SDL_BlitSurface`. If dimensions differ and source `BytesPerPixel != 1`, native `SDL_BlitScaled` is used. When dimensions differ and the source is one byte per pixel, it chooses custom indexed-to-indexed for destination BPP=1 or custom indexed-to-true-colour `LbI_SDL_BlitScaled_totcbpp` for all other destination BPP.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924-970`: custom true-colour path writes through a `long *` but advances by `dst_bpp`; the original report `2026-11-01_sdl_indexed_truecolour_writes_and_pitch_risk.md` documents potential overlapping writes for target BPP 2/3 on four-byte-long systems and width-based addressing without pitch.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:497-534`: SDL2 obtains the actual window surface through `SDL_GetWindowSurface` and can create a separate draw surface based on format/dimension conditions. On the no-`LB_WSCREEN_CONTROL` path, the extra condition for creating a secondary surface is not present, so the create block executes.
- `engine/src/display.c:79-88`: application maps mode identifiers 19 and 18 to 320x200x8 and 640x480x8, respectively, and sets library `WScreen` before `LbScreenSetup`. Requested engine format is 8-bit; whether actual SDL window surface is 8/16/24/32-bit in a specific Windows run is not established.

## Risk boundary and test matrix
Potential 4-byte store overlap is **conditional**, not an observed bug. To reach it in SDL2, the final surface presentation needs unequal draw/screen dimensions, a one-byte-per-pixel source, and a destination BPP of 2 or 3 on a four-byte-long target. BPP=4 does not have the same store-width-versus-stride mismatch, though the custom scaling pitch assumptions still require checking. Equal dimensions bypass the custom scaler entirely.
Recommended controlled checks: (1) equal 8-bit dimensions, (2) scaled 8-bit-to-8-bit, (3) scaled 8-bit-to-16-bit, (4) scaled 8-bit-to-24-bit, (5) scaled 8-bit-to-32-bit; collect source/destination `pitch`, formats, dimensions and memory-safety instrumentation. No modifications authorised yet.

## Outstanding
Exact Windows CI workflow and generated compile flags remain unverified with accessible tools. Source-level macro prefix explanation has already been resolved and should not be reopened. Confirm actual linked SDL backend and window surface format from a native build before prioritising a fix.

## Verification
PASS: direct GitHub reads of SDL2 dispatcher, conversion function, surface setup and application mode setup.
NOT RUN: compile, test harness, Windows display tests, memory instrumentation, screenshots, retail asset tests.
CHANGED: this documentation file and graphics continuity index only. Game v0.7 baseline and core/Mission Builder branches untouched.
