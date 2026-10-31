# Syndicate 2026 — Planar Block Component Read Extent

Date: 2026-11-01 (Australia/Brisbane)
Status: focused static original-assembly audit COMPLETE; runtime and asset-wide bounds not tested.
Branch: feature/v0.9-visual-showcase. Protected main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Source: original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf. This stage adds evidence to docs/audits/2026-11-01_block_composition_fixed_pool.md; no code or game data changed.

## Source evidence
- syndre.sx:71587–71623 declares data_48ad0 with 36 dword slots. Drawing branches write the component pointers into that array; their maximum reachable count has not been proved.
- syndre.sx:66362–66595 has a bitplane component consumer using the pointer at data_48ad0[ECX], repeated AND/OR mask operations and four plane destinations separated by 0x7d00 bytes. The source region includes a **read at component offset +0x13c (316 bytes)**, implying at least **320 bytes** must be accessible at every component start on that specific path. This is stronger than merely proving that component start addresses lie inside hblk01.dat.
- The scanned block-mask consumer includes 94 explicitly written mov operations of the form mov 0xHEX(%ebx),%eax within assembly lines 66363–66740; this is a lexical count of that particular instruction spelling, NOT a count of logical pixels or the total number of reads. The highest offset identified by this inspection is 0x13c, so a four-byte load at that address ends at relative byte 319.
- The previous audit checked two descriptor fields across 256 candidate entries for pointer-start range only. That test is insufficient to prove 320-byte record access safety; starts within the final 319 bytes of hblk01.dat would be out of bounds for this consumer, if those starts were reached via this path.

## Consequences
The fixed 36-entry collection is a distinct possible bound from **320-byte component content**; neither may be conflated with the 24-byte h_blocks descriptor stride. The h_blocks record stores pointers to potentially larger visual fragments, while the consumer performs planar bitwise composition. Scene-local lighting must respect this draw order and preserve the original four-plane result prior to palette mapping.

## Verification ledger
PASS: Direct original GitHub assembly read; identification of array declaration, producer/consumer, +0x13c read and required 320-byte extent.
NOT RUN: native compile; per-map/per-block reachability; actual private hblk pointer plus 320 bounds across all valid descriptors; Windows rendering or gameplay; night-lighting or explosion profiling.
OPEN: which paths invoke this 320-byte consumer, exact layout of all component classes, maximum list count, geometry and luminous object identities.

## Next audit
Privately check **pointer + 320 <= decoded hblk01.dat length** for actual referenced components, accounting for pointer-field zero/nonzero and meaningful descriptor indexes. Trace all consumer paths and list bounds before any code changes. Preserve baseline; public repository contains only this report and its index entry.
