# Syndicate 2026 — WScreen Allocation, Palette Separation and Phase Bounds

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: documentation-only cross-check of original assembly plus current port source. Native compilation and Windows runtime tests were not performed.

## Critical disambiguation

There are **two distinct screen pointer variables**:
- **`WScreen`**: exported from `engine/src/syndre.sx:83901-83902`. Its allocation is managed by `LbDataLoadAll` using the first `TbLoadFiles` entry in the chosen original load table.
- **`lbDisplay.WScreen`**: a separate Bullfrog library display structure field set in `engine/src/display.c:72-88` to `VGABuffer + 640*480`. `display_create_vga_buffer` allocates `640*480*2 + 16 = 614,416` bytes in `display.c:127-131`. This is **not proof that the original exported `WScreen` points there**; the exported game pointer is separately loader-managed. `ApSpriteDrawMCGA` temporarily sets `lbDisplay.WScreen = WScreen` before calling `LbSpriteDraw`, then restores the display pointer (`display.c:263-275`).

Do not conflate these two fields in implementation notes, size estimates or palette diagnostics.

## Loader ownership and allocation

- `engine/src/main.c:298-319` calls `display_create_vga_buffer` and, depending on `DrawFlags`, loads either `load_files_vres16` or `load_files_mcga`. Neither branch being selected is outside this trace.
- `syndre.sx:83626-83637`: high-resolution `TbLoadFiles` first entry has an asterisk resource name, points its Start slot to exported `WScreen` and specifies **256,048 bytes**. The next entry names `data/hpal01.dat` and stores its allocation pointer in `GraphicsPalette`, a separate exported pointer slot (`syndre.sx:83909-83910`).
- `syndre.sx:83651-83661`: the MCGA first entry points to `WScreen`; its little-endian SLength bytes `10 E8 03 00` specify **256,016 bytes**. The separate initial palette resource is `data/lpal01.dat` with destination `GraphicsPalette`.
- `engine/bflibrary/include/bfdata.h:29-42` defines the packed load descriptor's Start, SEnd and SLength layout; `engine/bflibrary/src/general/gdata.c:65-103` allocates SLength bytes for asterisk entries, while normal filename entries get independently allocated buffers. `LbDataLoadAll` frees and reloads descriptor entries (`gdata.c:105-130`). Therefore a graphics hook cannot assume the exported `WScreen` address is stable across resource reloads.
- In `syndre.sx:27779-27794`, mission palette data is read into `GraphicsPalette`; a call to `LbScreenSurfaceClear` receives `WScreen` and zero, and the following `ac_LbPaletteSet` receives **`WScreen`**, not `GraphicsPalette`. SDL1/SDL2 `LbPaletteSet` implementations read palette RGB triplets from their argument. This establishes a source-level role mismatch, **not** a proven runtime defect. The actual clear-function implementation, byte extent and visual transition were not verified.

## Pointer-offset budget (source starts only)

The original assembly's documented phase-dependent addresses are relative to exported `WScreen`. Offsets below are tested arithmetically against the two nominal loader capacities, **not** validated ranges of drawable surfaces.

| Source phase / pointer | Relative byte offset | Bytes remaining, Vres16 (256,048) | Bytes remaining, MCGA (256,016) |
|---|---:|---:|---:|
| Title BScreen | 64,000 (0xFA00) | 192,048 | 192,016 |
| Title scratch | 128,000 (0x1F400) | 128,048 | 128,016 |
| Title PalMem | 132,096 (0x20400) | 123,952 | 123,920 |
| Mission UScreen | 128,008 (0x1F408) | 128,040 | 128,008 |
| Mission BScreen | 128,024 (0x1F418) | 128,024 | 127,992 |
| Post-level scratch | 192,000 (0x2EE00) | 64,048 | 64,016 |
| Post-level PalMem | 196,096 (0x2FE00) | 59,952 | 59,920 |

All these **start positions** fall inside either nominal allocation. That does **not** prove any subsequent read/write extent is in bounds. The same phase offsets need not be active in both drawing modes. See existing `docs/audits/2026-11-01_assembly_screen_pointer_transitions.md`, `docs/audits/2026-11-01_wscreen_build_flag_ownership.md` and `docs/audits/2026-11-01_data_loader_failure_semantics.md` for preceding source evidence.

## Consequences and next gate

Do not fix or reorder the mission palette calls or write a night-lighting hook on these findings alone. The next validation needs (a) actual `LbScreenSurfaceClear` semantics and affected byte extent, (b) source tracing of phase-specific palette restore timing, (c) live `WScreen`/`GraphicsPalette`/`lbDisplay.WScreen` pointer logging in an isolated Windows test, (d) visual evidence for initial black/normal palette and menu transitions, and (e) actual surface pitch and owner lifecycle. No original retail graphics files should be committed.

## Verification ledger

**PASS:** original 85,803-line assembly blob inspection, game/display/main C reads, packed loader and allocation source trace, explicit byte arithmetic and comparison with prior audits.

**NOT RUN:** game compilation, actual loader allocations, WScreen clear-function test, Windows screenshot comparison, gameplay/performance tests, original palette RGB resource parsing.

**Changed:** this document plus graphics audit continuity index only; no engine code, commercial assets, protected main or parallel audit branches changed.

## Subsequent clearing-behaviour resolution — 2026-11-01
The previously open `LbScreenSurfaceClear` source question is resolved by `docs/audits/2026-11-01_palette_blackout_restore_source_trace.md`. Original assembly at `syndre.sx:75307-75340` zeros 0x1f400 bytes (128,000) only when DrawFlags bit 0x2 is set, making the subsequent `LbPaletteSet(WScreen)` input an all-zero palette prefix in that mode. `GraphicsPalette` is separately restored on the third active mission-loop iteration under the traced counter sequence. Non-high-resolution branches have no such zero-fill in this routine; no Windows runtime validation exists.
