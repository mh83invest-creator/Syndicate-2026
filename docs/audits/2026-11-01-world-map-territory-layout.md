# Syndicate 2026 — World-map territory resource layout

Date: 2026-11-01. Static assembly audit only.

## Confirmed source findings
- In engine/src/syndre.sx, _draw_map_area at lines 32804–32867 converts the selected unsigned territory index into a source offset. The arithmetic produces 11 * 256 = 2816 bytes per territory (0xB00).
- Its nested loop reads 0x40 (64) bytes across and 0x2c (44) rows, for 2816 bytes per territory. The source pointer advances contiguously, without per-row source padding.
- A 50-territory resource therefore requires at least 50 * 2816 = 140800 bytes (0x22600) for these blocks, assuming valid indices 0–49 and no leading header. This is a calculated minimum for the drawing routine, not a verified retail file size or guarantee of no header.
- The tables map_countries and data_5b062 at 84528 onward supply display coordinates; they are not a resource-size or campaign identifier.
- World-map loading uses data/mmapblk.dat via mmapblk_load_files (83675 onward), and callers do not inspect the native batch-load error count before display_cities.
- The drawing routine does not locally compare the source offset plus 2816 against allocated resource capacity.

## Unverified
Actual compressed/uncompressed mmapblk.dat size, validity of every territory index passed, table lengths, drawing destination bounds, and runtime behavior when the asset is missing or truncated.

## Next steps
Verify table-index range and callers; trace asset length through the native RNC loader; use legitimate retail data for read-only length checks, then Windows fault-path tests. Preserve v0.7, campaign save isolation and separate Mission Builder branch.

## Verification
Inspected source and independently checked index/loop arithmetic. No compilation, executable testing, game assets, save operations or engine edits.
