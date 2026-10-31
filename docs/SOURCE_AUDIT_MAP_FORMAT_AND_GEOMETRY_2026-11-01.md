# Syndicate 2026 — Original Map Lookup Table and Block Geometry Audit

**Date:** 2026-11-01 (Australia/Brisbane)
**Status:** Static source analysis with one synthetic offset-arithmetic cross-check; **no game asset or executable was accessed**.
**Game engine:** SyndicatFX retained, protected baseline v0.7 `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Sources:** `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (line numbers 1-based); `CeRiAl/freesynd/src/map.cpp` and `src/mapmanager.cpp` on FreeSynd master as **external, unmodified reference**. No FreeSynd source added to project.

## 1. Map file has an identifiable offset-table structure

The mission load table at `syndre.sx:82620–82640` loads original `data/mapNN.dat` into `map_buf`, `data/col01.dat` into `h_col`, and `data/hblk01.dat` into `h_blocks`. `init_level_data` at `27719–27722` passes `map_buf` to `init_map_data`.

**Exact assembly at `syndre.sx:25694–25716`:**
- Receives the decoded `map_buf` pointer and advances by **12 bytes**.
- Iterates `0x3000 = 12,288` entries, each **four bytes** apart.
- Reads each 32-bit value and **replaces it in place** with `base_after_header + relative_value`.
- Stores the resulting table base in `data_55358`.

This establishes an in-place **48 KiB (49,152-byte) 32-bit column pointer table** immediately after a 12-byte header in the loaded original map data. In-place rebasing makes subsequent code use native addresses, not offsets.

### Independent C++ reference comparison

FreeSynd `src/map.cpp:47–70` reads **three 32-bit little-endian values at byte offsets 0, 4 and 8** as X/Y/Z map dimensions, reads a column lookup table starting at byte **12**, and resolves a tile at level z as `mapData + 12 + lookup[idx] + z`. FreeSynd's lookup length is `max_x * max_y`, unlike the **fixed 12,288 iterations** in the original assembly path.

The two implementations therefore **agree on the 12-byte header and relative column-offset convention** for their inspected code paths. They differ in how many columns are iterated (fixed vs header-derived), which requires actual retail map files to reconcile. This is a considerably stronger source cross-check than a superficial class-name comparison, but does **not prove every map file is safe/compatible**.

The fixed 12,288 entries and `draw_horizontal`'s 128-column stride (below) are *consistent* with a **128×96** map-column grid. **Do not state the X/Y/Z header is actually 128×96×something until genuine `mapNN.dat` bytes are inspected.** FreeSynd's dimensions are read from data; the assembly's fixed iteration count is an implementation assumption.

## 2. Actual isometric/block draw dependencies

`syndre.sx:71385–71425`, `draw_horizontal`, derives the column index by shifting one tile coordinate right by one bit and the other by one bit, then multiplying the latter by **128 (`shl $0x7`)**. It multiplies the resulting index by **four** to access the rebased column table `data_55358`. Odd/even coordinate bits select one of **four jump-table drawing paths** (`jpt_488B7` at lines 71426–71430). The hi-resolution branch **multiplies one drawing argument by `0x500 = 1,280`** (line 71421). This resembles a row/stride-derived offset, but **its exact semantic role has not been resolved** from the adjacent helper routines; it must not be asserted to be the framebuffer's universal row pitch.

The block draw helpers and their branches (`syndre.sx:71513–71544`, `71628–71675`) inspect block data and associated `h_blocks` records, and gather multiple referenced block parts before drawing. Later actor composition passes through `draw_mapwho` and `draw_element` (original lines 18029+ and 19109+).

**Implication for actual night lighting:** Road/surface, building faces and window geometry are not exposed as a ready-made light-source list in this layer. Geometry arises from the original column offsets, block identifiers, tile elevation/layering, sprite actors and redraw rules. To show windows and neon while preserving occlusion, a lighting system would need to understand and classify these objects or use curated scene/mission metadata. Merely drawing light circles in the SDL surface may appear in front of buildings incorrectly.

## 3. Synthetic equivalence test actually run

Constructed an **independent synthetic uncompressed map byte buffer**:
- 12-byte header with illustrative 128×96×1 dimensions;
- 12,288 little-endian 32-bit relative offsets occupying 49,152 bytes;
- one synthetic tile byte per column following the table.

For every table index, compared:
- FreeSynd-style `buf[12 + lookup[i]]`, and
- the assembly rebasing rule `rebased_ptr[i] = (base + 12) + lookup[i]`, then retrieving `buf[rebased_ptr[i]]`.

**Result:** **12,288/12,288 synthetic column-byte comparisons matched.** This proves the **arithmetic equivalence of those two algorithms for the constructed buffer**, not that a real retail map loads correctly, nor that tile/render interpretation is identical. Neither the original assembly nor FreeSynd C++ was compiled or executed in this test.

## 4. Outstanding map-specific evidence gates

- Privately inspect a legally owned original `mapNN.dat` after the game's RNC decompression: verify dimensions, table length, offsets, bounds, Z-layers, and whether all original maps satisfy 12,288 entries.
- Determine how `h_col` and `h_blocks` encode tile classes, building faces, signs and possible luminous surfaces, and how those map to world-space coordinates. Their contents cannot be inferred from filenames alone.
- Identify exact map/object-to-screen coordinate transformation and occlusion, including `draw_mapwho`, `__cover_sprite` and dirty/scroll repainting.
- Verify map tile and actor layering against real daytime mission screenshots before adding emissive surfaces/headlights.
- Benchmark block drawing separately from effect simulation and final SDL scaling. No hotspot identified from source counts alone.

## 5. Scope and verification

**Completed:** Static cross-check of real original and external reference map offset conventions, exact 12,288-entry rebase loop, 128-column indexing and branching, synthetic lookup arithmetic test.

**Not performed:** Real map resource parsing, actual window geometry proof, compilation, combat/menu/camera gameplay, lighting implementation or tests. No repository engine source, FreeSynd code or workflows changed.

**Decision:** Continue with existing SyndicatFX; preserve camera, AI, 16 FPS mission cadence, save format and original assets. This source evidence narrows the next map/lighting feasibility questions but is **not** an approved implementation plan.
