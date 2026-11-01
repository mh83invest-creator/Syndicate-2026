# Palette ABI: SDL1/SDL2 implementation versus original WScreen call

Date: 2026-11-01 (Australia/Brisbane). Branch: audit/graphics-assets-2026-11-01.
Scope: source-only audit of palette set argument semantics; no game changes.

## Source observations
- engine/bflibrary/src/x86-win-sdl2/spalette.c:50 onwards, LbPaletteSet(const ubyte *palette), treats its input pointer as RGB palette bytes; loops over PALETTE_8b_COLORS, masks three source channels with 0x3F and converts them to SDL_Color channels by shifting left two bits. It also copies the six-bit channel values into lbPalette.
- engine/bflibrary/src/x86-win-sdl/spalette.c:50 onwards implements the same basic incoming RGB-triplet interpretation and conversion using SDL1 palette APIs.
- Original engine/src/syndre.sx:27779-27794 first loads palette file data into GraphicsPalette, clears WScreen, then calls ac_LbPaletteSet with WScreen. Meanwhile syndre.sx:244-249 passes GraphicsPalette when restoring a palette from a main-loop branch, and lines 54973-54980 pass m_select_pal in menu context.
- Therefore, the palette setter's documented SDL implementations do not interpret WScreen as a special framebuffer argument. It remains unresolved why the original mission loader passes WScreen, and whether WScreen aliases/contains prepared palette bytes in this phase.

## Consequence
Do not change the original call or relabel it a confirmed bug on source inspection alone. Track WScreen allocation/aliasing and the load function call context, then test original Windows palette colours in mission load and transition phases before any modification. Proposed night-lighting changes must preserve palette fades and the 6-bit-to-display conversion.

## Verification
PASS: direct reads of SDL1 and SDL2 palette setter implementations and original assembly call sites.
NOT RUN: palette pointer alias test, native compilation, Windows runtime, screenshots, private palette-byte decoding.
Documentation only, protected main and parallel workstreams unchanged.

## Later source resolution — 2026-11-01
Read `docs/audits/2026-11-01_palette_blackout_restore_source_trace.md`: original assembly implements `LbScreenSurfaceClear` at `syndre.sx:75307-75340`. When DrawFlags bit 0x2 is set, it zeroes 128,000 bytes of `WScreen`, including the 768-byte prefix read by `LbPaletteSet`, explaining the all-black palette argument in the inspected high-resolution path. A later active mission loop restores `GraphicsPalette` when the counter is two. No source repair or generalisation to other graphics modes is justified. Runtime behaviour has not been tested.
