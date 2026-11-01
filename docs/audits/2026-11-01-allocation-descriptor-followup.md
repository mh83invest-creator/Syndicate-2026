# Syndicate 2026 — Allocation and resource descriptor follow-up

Date: 2026-11-01. Static source audit; no engine changes.

## Source evidence
- syndre.sx:14446–14456: LbMemoryAlloc adds 0x100 to requested byte count, updates allocation accounting, and delegates to ac_malloc. No overflow check is visible in this routine.
- syndre.sx:14728–14745: Generic data loader gets reported resource length and calls LbMemoryAlloc(length). Negative or malformed lengths are not visibly rejected at this callsite.
- syndre.sx:14759–14794: Generic loader returns 1 for matching read count; a mismatched read count calls UnpackM1, then returns 1 without checking its result. Runtime consequences unverified.
- syndre.sx:82620–82632: mission_load_files includes a map filename and the map_buf destination pointer.
- syndre.sx:85040–85045: Another data descriptor includes map_buf and an embedded length field, whose exact layout and semantics are not yet independently verified.
- syndre.sx:83634–83660: Palette resource descriptors reference GraphicsPalette. Both map_buf and GraphicsPalette declarations are four-byte pointer slots (lines 83895 and 83909), not inline arrays.
- syndre.sx:27719–27722: init_level_data passes the loaded map_buf pointer to init_map_data.
- syndre.sx:85702 onward: level__Seed begins contiguous level-state storage. Its two-byte initial field does not establish total destination capacity.

## Risks and outstanding evidence
Resource buffer capacities, exact descriptor layout, file decompression return contracts, and actual retail asset sizes need further tracing. The source includes unchecked arithmetic and failure paths, but no runtime corruption has been reproduced. Do not conflate these with proven crashes.

## Next checks
Confirm descriptor offsets and mode-specific allocation semantics; establish map/palette capacities; check missing-file, corrupted-length, short-read and decompression behavior in a controlled Windows test before proposing changes. Maintain original/American Revolt compatibility and the approved campaign-specific save isolation.

## Verification
Full assembly source reviewed at cited lines. No build, executable test, game data access, save migration, or source change. Protected v0.7 baseline and concurrent Mission Builder branch unchanged.
