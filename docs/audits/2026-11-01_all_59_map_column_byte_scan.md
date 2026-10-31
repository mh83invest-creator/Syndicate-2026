# Syndicate 2026 — All 59 Original Map Column Byte Scan

Date: 2026-11-01 (Australia/Brisbane). Focused private-resource verification COMPLETE; overall forensic audit incomplete.
Branch: feature/v0.9-visual-showcase. Protected main v0.7: a22b0b9580a9710b4e693e88854351835ef1ee40.
Source link: original syndre.sx map pointer rebase at lines 25694–25716 and block lookup reading map-column +10 and +11 at 71638–71739. Private input: existing user-uploaded v0.6 resource ZIP. Commercial bytes were not published.

## Test method
Decompressed all 59 original map*.dat members with the previously documented independent RNC1 decoder and its in-memory 512-byte zero-lookahead padding. Accepted results only if the unpacked CRC and declared length passed. For every map, interpreted its 12-byte header followed by 12,288 little-endian 32-bit relative offsets. Converted each to decoded-file position `12 + offset`, then checked whether **all 12 bytes** of the associated column record would fit in the decoded resource. This is a stronger bounds test than checking the start offset alone.

Collected the union of all byte values in these 12-byte column records across all 59 maps. This scans candidate bytes only; **it does not establish that every byte is a descriptor ID used in all draw paths**.

## Observed results
- 59 original maps decoded with correct expected lengths and unpacked CRCs.
- 59 x 12,288 = **724,992 column offsets**, each with all 12 referenced bytes inside the decoded file. **Zero 12-byte extent failures**.
- Across those columns, **255 of 256 possible unsigned byte values** occur. The missing value is decimal 32; the highest observed is 255. Values greater than four occur at 250 distinct values, but their semantic role depends on source-position and renderer branch.
- A single map, map01.dat, contains 122 distinct values when scanning all 12 bytes of every column. Other maps vary, and no claim of an 8-bit lighting/material flag is justified merely from this count.
- Source lines 71638–71739 specifically use column byte +10 or +11 to select h_blocks descriptors after a >4 check. A future follow-up should count the **position-specific** index values, not treat the union of all byte positions as actual render-selected block IDs.

## Verification ledger
PASS: private ZIP read, all 59 RNC1 decompressions with declared length/unpacked CRC checks, 724,992 12-byte record bounds checks, 256-value union analysis.
NOT RUN: original C decoder harness; game-map rebasing in native executable; actual Windows rendering, camera/focus, explosion profiling or lighting test.
OPEN: per-byte-position descriptor-ID distributions, valid block table end, referenced component extents under actual drawing branches, object geometry and real-world light-source classification.

## Next
Match column byte offsets used by the renderer with the sampled h_blocks descriptor fields and verify the actual reachable IDs. Maintain source-only audits, preserve main and keep proprietary assets offline.
