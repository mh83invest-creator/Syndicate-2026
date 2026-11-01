# Syndicate 2026 — High-Resolution Palette Blackout and Restore Trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Status: original assembly and SDL source trace completed; native visual timing and gameplay not tested.
Scope: resolve the previously unexplained `LbScreenSurfaceClear(WScreen,0)` immediately followed by `ac_LbPaletteSet(WScreen)`.

## Resolved code path

1. `engine/src/syndre.sx:75307-75340` contains **the actual `LbScreenSurfaceClear` implementation**. With `DrawFlags & 0x2` set, it calls `ac_LbMemorySet(dst, fill, 0x1f400)`, so the requested zero fill covers **128,000 bytes**. When bit 0x2 is not set, the inspected branches for other DrawFlags return without a memory-set call. The name alone is not enough to presume all display modes clear memory.
2. `engine/bflibrary/src/general/gmemut.c:33-36` implements `LbMemorySet` through C `memset(dst,c,size)`. Under the high-resolution branch, successful invocation makes `WScreen[0..127999]` zero.
3. `syndre.sx:27779-27793` first loads the selected palette resource into separately allocated `GraphicsPalette` via `LbFileReadRNC`, calls `LbScreenSurfaceClear(WScreen,0)`, then immediately passes **`WScreen`** to `ac_LbPaletteSet`. There are no intervening writes to `WScreen` in that source sequence.
4. `engine/bflibrary/include/bfpalette.h:31-32` specifies 256 entries at 3 bytes each: **768 bytes**. Both SDL palette implementations (`engine/bflibrary/src/x86-win-sdl/spalette.c:50+` and `x86-win-sdl2/spalette.c:50+`) read 3 source channels per entry, mask each to six bits, and convert to displayed colour channels. The first 768 bytes of a successfully cleared `WScreen` are all zero, so the setter receives an **all-black palette** in this branch.
5. `syndre.sx:199-248`: `syndicate` initialises `ebx=0` when entering its active-mission loop; after `swap_screen_vres16`, the loop calls `ac_LbPaletteSet(GraphicsPalette)` whenever `ebx==2`. `syndre.sx:274-279` increments `ebx` once at the end of each loop up to three. **If the normal active loop runs three uninterrupted iterations**, the restore call is reached on its third iteration; this is not a measured real-time duration or proof of a particular visible fade effect.

## Conclusions and limits

The previous report's “WScreen argument discrepancy” is better understood: in the high-resolution path, the original assembly itself zeroes an ordinary graphics work buffer and immediately reuses its first 768 bytes as a temporary black palette, with a later mission-loop restore from `GraphicsPalette`. This is **consistent with a startup blackout/reveal design**, not evidence requiring a bug fix.

The exact player-visible effect, screen-swap order, and whether any extra palette operations intervene outside these paths have not been tested. In paths **without** DrawFlags bit 0x2, `LbScreenSurfaceClear` does not perform the zero-fill, so do not generalise the black-palette conclusion to MCGA/other modes. Do not change this behaviour or use it as a substitute for the requested localised night lighting.

`WScreen` here is the original exported and loader-managed buffer, **not** `lbDisplay.WScreen` (the SDL display buffer field). See `docs/audits/2026-11-01_wscreen_allocation_palette_phase_bounds.md`.

## Verification ledger

PASS: exact assembly routine and caller reads, `LbMemorySet` C implementation, both SDL `LbPaletteSet` implementations, palette-size constants, and mission-loop counter/restore path. Static calculations: `0x1f400 = 128000`, `3 * 256 = 768`; 768 is within the cleared 128000-byte prefix and the 256048-byte high-res allocation.

NOT RUN: actual Windows execution, native compilation, live memory logging, screenshots, observed blackout duration, sound/gameplay regression, performance tests or private palette-byte decoding.

FILES MODIFIED: this report plus referenced graphics handover documents, all on graphics audit branch only. No code, CI, protected main, core feature branch, Mission Builder or commercial assets changed.

## Next audit gate

Trace palette behaviour on mode changes and non-high-resolution paths, and determine precisely when the original mission intro draws/screen-swaps relative to palette restore. No renderer hook or lighting implementation authorised by this audit alone.
