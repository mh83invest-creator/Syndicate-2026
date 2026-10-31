# Syndicate 2026 — Private original block flag reconstruction

Date: 2026-11-01 (Brisbane)
Stage: limited offline source-to-resource cross-check PASS. Full runtime/whole-project audit remains incomplete.
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Source: original engine/src/syndre.sx:79740–79781, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
Private data: existing user-uploaded v0.6 archive, hblk01.dat. No game asset bytes committed.

## Method
Performed a read-only offline run of the independent Python RNC decoder in /mnt/data/rnc1_zero_audit.py with its *function source changed in memory only* to append 512 zero bytes for bitstream lookahead, as documented in the prior RNC reconciliation. The original archive's compressed CRC, expected decoded length and unpacked CRC must pass. The decoded hblk resource has 321,664 bytes.

Recreated the original assembly initializer's logic: 256 descriptor IDs, six 32-bit little-endian pointer offsets at positions 0,4,8,12,16,20 of each 24-byte descriptor, rebased relative to the decoded hblk start. For each component, tested 16 words at offsets 0,20,...,300; set a corresponding bit only when all 16 words were zero. Checked each 304-byte sample extent stays inside the decoded allocation.

## Observations
- 256 descriptors and 1,536 candidate component fields examined. **Zero component sample-extent failures**.
- 150 descriptor component pointers are exactly zero; the original algorithm still samples from the start of the allocation when this happens.
- Calculated flag patterns over all 256 descriptors: 149 with value 0, 89 with value 18 (binary 010010), 11 with value 16, and 7 with value 2.
- Set-bit counts by index 0..5: [0,96,0,0,100,0]. Thus only flag bits 1 and 4 are set in this *particular* hblk01.dat resource under this algorithm. This is a property of the inspected data and code; do not extrapolate to other resources or alternate level modes.
- None of these flag bits identifies a lamp, building window or emissive surface. The source predicate is 16 sampled zero words, not a complete empty-component test.

## Verification
PASS: Python read-only resource decompression with unpacked CRC, 256×6 pointer checks, exact 16-sample 20-byte step predicate, frequency count.
NOT RUN: original assembly compiled/executed, actual in-game flag array inspection, renderer/geometry visual comparison, Windows test or performance profile.
CAVEATS: independent RNC decode uses experimental zero lookahead padding; the sample reconstructs one source function, not all game-path control flow or pointer semantics.

## Follow-on
Relate flag values to source-documented producer branch decisions and assess the fixed 36-component pointer list's maximum reachable occupancy. Do not modify original engine or protected main. Retail data stays private.
