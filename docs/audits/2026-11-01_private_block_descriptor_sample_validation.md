# Syndicate 2026 — Private High-Resolution Block Descriptor Sample Validation

Date: 2026-11-01 (Australia/Brisbane)
Status: focused PRIVATE asset sample test completed; wider block semantics and gameplay testing incomplete.
Branch: feature/v0.9-visual-showcase. Protected main v0.7: a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: user-supplied private v0.6 ZIP; existing private Python RNC1 decoder; original syndre.sx:66748–66763, 71638–71669. No commercial bytes included in this GitHub report.

## Work performed
A separate read-only Python invocation imported the existing experimental RNC decoder and changed only the **in-memory input stream padding** to append 512 zero bytes before decompression (without altering original CRC fields). The hblk01.dat output passed the decoder's unpacked CRC and length validation. Examined the first 256 entries at 24-byte stride and the 32-bit little-endian fields at offsets +4 and +12 against the decoded buffer length of 321,664 bytes. The existing decoder file and original engine were not modified.

## Results
- hblk01.dat decoded exactly 321,664 bytes.
- The first 256 candidate 24-byte descriptor records have **zero out-of-range positive offsets** at either examined field, relative to the entire decoded hblk buffer.
- Field +4: 241 values nonzero and below 321,664, and 15 zeros. Sample: entry 5 -> 6,784; entry 6 -> 8,384.
- Field +12: 223 values nonzero and below 321,664, and 33 zeros. Sample: entry 5 -> 7,424; entry 12 -> 16,384.
- Decoded col01.dat has 256 bytes and 17 distinct byte values. No meaning is assigned to those values by this test.
- Assembly source independently confirms that certain original draw paths use a 24-byte descriptor stride, plus fields at +4 or +12 and relative-address rebasing.

**Limitations:** This tests only the first 256 candidate descriptor entries and whether candidate pointer *starts* lie within the whole hblk allocation. It does not establish how many entries are valid, their full field layout, how much data is accessed at each target, whether any individual asset entry is a window or streetlight, or which map-column indexes actually refer to them. In particular, `321664 / 24` should NOT be taken as the block count: the file also includes data pointed to by descriptors.

## Verification
PASS: local read-only decompression to expected size/unpacked CRC; 256 records times two fields checked; all 464 nonzero starts in file bounds; col01 length and distinct-value count.
NOT RUN: Windows game/asset loader, original native decoder, sprite rendering, occlusion, object classifications, lighting tests.
NEXT: derive a credible descriptor-table termination bound from original assembly and private data, then match actual map-column values and sprite/block geometry in isolated tests. No production code changes; main stays protected.
