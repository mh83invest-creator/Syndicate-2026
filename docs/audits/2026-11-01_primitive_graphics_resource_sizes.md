# Syndicate 2026 — Original Primitive Graphics Resource Decode Inventory

Date: 2026-11-01 (Brisbane)
Status: focused offline decoding and length/CRC checks COMPLETE; field semantics and render equivalence remain open.
Branch: feature/v0.9-visual-showcase. Protected main v0.7 commit a22b0b9580a9710b4e693e88854351835ef1ee40.
Input: original private game assets in user-uploaded v0.6 archive. Decode method: the independent Python RNC1 decoder with bounded, 512-byte zero-padded lookahead described and verified in docs/audits/2026-11-01_rnc_full_decode_reconciliation.md. No proprietary file contents published, only lengths and safe high-level metadata.

## Verified file size comparisons
| Original resource | Compressed file bytes | Decoded bytes |
|---|---:|---:|
| hblk01.dat | 117484 | 321664 |
| col01.dat | 127 | 256 |
| hspr-0.dat | 121817 | 355356 |
| hspr-0.tab | 4641 | 10812 |
| hele-0.ani | 38453 | 104860 |
| hfra-0.ani | 17583 | 71584 |
| hsta-0.ani | 1937 | 3940 |
| hpal04.dat | 577 | 768 |
| hpal05.dat | 576 | 768 |

All listed samples decoded to their RNC1-declared length and passed the original unpacked CRC stored in each resource header; they are part of the earlier 143/143 overall decode check.

## Findings and limits
- hblk01 and hspr-0 contain materially more decoded bytes than the original compressed files, so any offset arithmetic/field validation must operate on *decompressed* memory. Parsing raw ZIP bytes as block or sprite records would be wrong.
- The decoded col01.dat resource is 256 bytes, making it an obvious candidate lookup table for renderer inspection, but **its meaning and indexing have not been independently proven** by this inventory.
- hspr-0.tab decodes to 10,812 bytes; it may describe sprite record locations, but this stage does not establish its record stride, indexing rules or off-by-one/end-of-table handling. That requires alignment with the original assembly sprite drawing code.
- All five original high-resolution palettes now decode to 768 bytes with valid unpacked CRC; see the preceding reconciliation report. Their palette component variety alone is not a night-lighting implementation plan.
- Neither the existence of decoded blocks nor map pointers proves which objects are road signs, windows, building faces or artificial light emitters. Object semantics, isometric occlusion and camera projection still need explicit tracing.

## Verification
PASS: read-only unpacked length and CRC for the nine files; recorded exact compressed/decoded byte lengths.
NOT RUN: original C decoder executable, Win32 build and game playback, pixel-level rendering, object identification, lighting effects and explosion benchmarking.
NEXT: Trace hblk01 and hspr-0.tab consumers in syndre.sx and their actual record dimensions/indices; cross-check map column references before classifying luminous objects. Protect game source and retail assets, maintain GitHub handovers.
