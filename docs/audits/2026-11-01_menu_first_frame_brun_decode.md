# Syndicate 2026 — Original Menu First-Frame BRUN/PAL Decode Audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused offline decoding stage completed for six *first frames*; no complete animation or Windows runtime claim.
Branch: feature/v0.9-visual-showcase. Protected baseline main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: original assembly decoder at engine/src/syndre.sx:61848–62025 and 62118–62178; original user-supplied private v0.6 ZIP. Source/gameplay/CI unchanged. No commercial assets uploaded to GitHub.

## Method
Built a read-only, independent Python parser for the first frame record of each of six menu files. It verifies the original 0xAF12 file record, 0xF1FA frame, width/height, and inner subchunk lengths, then reads palette chunk type 4 and uses the signed-run logic from original decode_BRUN (type 15). The BRUN decoder skips one per-row packet-count byte, then processes positive signed run length as repeated pixels and negative run length as literal bytes until each 320-pixel row is filled, for 200 rows. The resulting first frame is a 64,000-byte indexed image; no image has been published or displayed as an original screenshot.

## Results
| File | Header frames | Decoded first-frame pixels | Distinct index values | BRUN payload bytes consumed / supplied |
|---|---:|---:|---:|---|
| mtitle.dat | 15 | 64,000 | 13 | 16,423 / 16,424 |
| mselect.dat | 23 | 64,000 | 61 | 31,280 / 31,280 |
| mbrief.dat | 15 | 64,000 | 56 | 31,321 / 31,322 |
| moption.dat | 11 | 64,000 | 57 | 30,925 / 30,926 |
| mmap.dat | 28 | 64,000 | 57 | 30,930 / 30,930 |
| mconfup.dat | 9 | 64,000 | 62 | 32,254 / 32,254 |

All six initial colour subchunks (type 4) contain a 1-packet, zero-skip, zero-count (=256) palette declaration followed by 768 channel bytes. Channel values observed are 0–63, consistent with the original 6-bit SDL palette conversion. The first-frame parser obtained the expected image size in all six samples. Some payloads have a single unconsumed byte, which might be padding or other format convention; this has NOT been resolved.

## Limitations
This is a bounded FIRST-FRAME decode, not verified fidelity to full original engine video output. SS2 interframe changes, frame timing, palette state during animation, UI overlays, mouse hitboxes, text, sound, focus and strategic transitions were not executed. First-frame header length discrepancies described in the preceding animation container audit remain. No external visual screenshot or original engine executable was tested.

## Feasibility implication
Original title/menu art is now mechanically parseable as 320x200 indexed image data in at least its first frame, enabling future before/after pixel-level comparison when a v0.9A visual design is approved. It does NOT justify arbitrarily replacing artwork without preserving subchunk conventions, palette changes, input mapping and user-approved scope.

## Next
Trace and validate decode_SS2 interframe updates, all declared frames, and original colour palette changes. Compare a full software-rendered frame sequence with real Windows screen captures before proposing UI implementation. Maintain protected v0.7 baseline and do not publish retail imagery/data to the public repository.
