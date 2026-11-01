# Syndicate 2026 — Indexed-to-true-colour scaling: destination write width and surface pitch

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only static audit; documentation, no implementation changes.

## Verified source observations
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924-970` custom `LbI_SDL_BlitScaled_totcbpp` takes source image width/height and destination width/height/BPP, but **no explicit surface pitch**.
- At SDL2 lines 962–965, it computes colour components and stores through `*((long *)(dst_buf+dst_offset))`, then increments `dst_offset += dst_bpp`. The SDL1 equivalent contains the same store/stride pattern at `engine/bflibrary/src/x86-win-sdl/sscreen.c:788-791`.
- On a target where C `long` is four bytes (including common 32-bit Windows build targets), the dereference performs a four-byte write despite `dst_bpp` possibly being 2 or 3. Source-level consequence: writes overlap subsequent pixel positions and, at the final pixel, may extend past the logical last pixel. Actual buffer bounds/row padding and visible impact require runtime or controlled test confirmation.
- Source/destination row movement in the custom scaler uses `src_w`, `dst_bpp`, and loop counters rather than passing SDL `src->pitch` or `dst->pitch`. Prior reports identify the same width-based assumption for the 8-bit-to-8-bit scaler.
- SDL2 caller `LbI_SDL_BlitScaled` at 976–1006 uses SDL native `SDL_BlitSurface` when source/destination dimensions match, native `SDL_BlitScaled` for non-8-bit input, otherwise one of the custom indexed scaler paths; thus these risks are conditional on scaling indexed source pixels to the corresponding destination format.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:344-366` recognises indexed, RGB555, RGB565, RGB24 and RGB888 formats; available format support does not establish that all destination formats are observed at runtime.

## Scope and recommendations
This is a concrete static **potential memory overwrite/pixel corruption** concern but **not** a confirmed Windows runtime defect. No source modification is authorised by this audit. Before proposing an isolated patch, trace the actual Windows backend selection, screen-surface formats and their pitch; make a targeted test matrix for scaled indexed output to 8/16/24/32-bit surfaces and verify no overlap/out-of-bounds with instrumentation.

## Build evidence status
The graphics audit's GitHub connector is still unable to enumerate workflow directory contents, and public GitHub API/web access was not available for the repository. Actual Windows CI workflow and generated compiler definitions remain unverified. The prior source-level `WSCREEN_CONTROL`/ `LB_WSCREEN_CONTROL` prefix mapping is resolved by `AX_PREFIX_CONFIG_H`; do not revive that as a suspected source typo.

## Verification
PASS: direct GitHub reads of both SDL1/SDL2 indexed-to-true-colour functions, SDL2 caller dispatch and pixel-format switch.
NOT RUN: Windows compilation, test harness, address sanitizer, memory instrumentation, screenshot comparison, runtime performance, private game assets.
CHANGED: this audit report and graphics continuity index only. Protected v0.7/game code and sibling branches untouched.
