# Syndicate 2026 — Scaled true-colour last-pixel byte bounds and SDL1 dispatch

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: original SDL1/SDL2 source inspection and byte arithmetic, documentation only.

## Verified implementation
- SDL2: `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924-970`; SDL1: `engine/bflibrary/src/x86-win-sdl/sscreen.c:750-797`.
- The converted indexed pixel store is `*((long *)(dst_buf+dst_offset)) = ...`, with `dst_offset += dst_bpp` on each pixel. Each routine receives width/height and destination bytes per pixel, not actual SDL pitch.
- Let N = destination pixel count (width x height), B = destination bytes per pixel, L = sizeof(long). For a tightly packed surface, the final store begins at `(N-1)*B`, ends exclusively at `(N-1)*B+L`; nominal pixel storage ends at `N*B`. The overhang beyond logical pixel storage is `max(0,L-B)` bytes. On 32-bit Windows (L=4), B=2 implies 2 extra bytes and B=3 implies 1 extra byte. This is bounds arithmetic, not proof the SDL allocation has no padding or that a crash occurs.
- A store spanning more than the current pixel overwrites bytes occupied by following pixels; actual final row/pitch, whether allocated padding absorbs the last store, and byte order change observed effects. A long-pointer store to an unaligned location also deserves architecture/compiler verification.
- SDL2 `LbI_SDL_BlitScaled` (`sscreen.c:976-1006`) takes native `SDL_BlitSurface` for equal dimensions, native `SDL_BlitScaled` for non-indexed source when dimensions differ, or custom 8-bit indexed paths. The risky true-colour helper is reached on **unequal dimensions + one-byte source + destination BPP > 1**.
- SDL1 `LbI_SDL_BlitScaled` (`sscreen.c:802-831`) logs `unsupported source bit length` for non-indexed scaled sources but **does not return at that point**; the routine continues through the custom scaler selection. This differs from SDL2, and is a separate conditional compatibility risk, not an observed bug.

## Explicit limitations and test gate
No Windows executable format, actual SDL pitch/rows, window mode, build linkage, input assets or test outcomes were observed. No source change is authorised. A targeted validation can use standalone synthetic indexed pixels (no retail data) and guarded 16/24/32-bit destination buffers with unequal dimensions, checking exact last-pixel bounds and row pitch under the real compiled backend. Include SDL1 non-indexed-source case separately. Preserve game rendering and gameplay baselines until actual tests justify a change.

## Verification ledger
PASS: GitHub reads of SDL1/SDL2 helper and caller, symbolic pixel end-offset arithmetic.
NOT RUN: native compilation, ASan/guard pages, Windows executable, screenshots or gameplay.
Changes: report and graphics audit continuity index only.
