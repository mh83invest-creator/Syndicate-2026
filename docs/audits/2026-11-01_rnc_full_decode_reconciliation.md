# Syndicate 2026 — RNC Decoder Reconciliation and Complete Map/Palette Offline Verification

Date: 2026-11-01 (Australia/Brisbane)
Stage: focused private-resource decoding and verification COMPLETE; full game/renderer audit and Windows gameplay still INCOMPLETE.
Branch: feature/v0.9-visual-showcase. Protected baseline: main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40.
No game source, workflow, private data, production dependency, release or stable branch modified.
Evidence: existing private user-uploaded v0.6 Two Menus 30 FPS Test ZIP, temporary script /mnt/data/rnc1_zero_audit.py and existing docs/audits/2026-11-01_offline_rnc_decoder_trial.md; decoder reference: engine/bflibrary/src/general/rnc_1fm.c.

## Correction to earlier incomplete decoder trial
The prior independent Python decoder accepted 103/143 RNC1 files but rejected 40 with input/copy bounds or CRC errors. That was a limitation in the independent decoding harness, NOT evidence that retail resources were damaged.

For this stage, the existing decoder was run without changing its Huffman, output copying, 16-bit bitstream, or CRC implementation. The sole experimental modification was to append **512 zero-valued bytes to the temporary input stream** before decoding, while continuing to verify the CRC against the *original packed payload only* and checking decompressed byte count and header-declared unpacked CRC. This matches the source-level observation that the original file loader allocates declared decompressed length plus 512 bytes of scratch capacity; it does not prove the original implementation initializes its buffer padding to zero.

**Observed result: 143/143 compressed private files now decode with exact expected output length and unpacked CRC. Zero decode failures.** This substantially resolves the previous independent decoder false negatives for these resources. It does not establish memory safety of the original C decoder or prove any Windows engine build executed.

## New asset results
- Five original hpal01.dat through hpal05.dat files all return 768 decoded bytes; component range is 0–63 in each.
- Number of distinct RGB triples per palette: hpal01=19; hpal02=18; hpal03=19; hpal04=186; hpal05=186. This is data diversity, **not** a demonstration that palette IDs 4 or 5 are night scenes, nor evidence of emitters, lights or correct game colour output.
- All **59/59** map*.dat files now decode and pass the same static layout checks used in the previous 48-map stage: header triplet (128,96,12), 12,288 relative column entries, four-byte offset alignment, and each pointer target base-plus-12 within the corresponding decoded file. Zero map-structure failures.
- The earlier 48/59 map stage was correct *for its then-validated subset*; its 11 decoder-inconclusive maps have now been checked successfully. Previous stage reports are retained as dated historical observations and superseded by this report for current validation status.

## Verification ledger
PASS: 143/143 compressed resource decoded-length and unpacked CRC checks in private Python harness, using appended 512-byte zero padding.
PASS: five decoded palette lengths and component ranges, distinct RGB triplet counts.
PASS: 59 map headers and all their 12,288 column pointers checked for alignment and file bounds.
NOT RUN: original C decoder compilation and memory-sanitizer harness; native Windows build, SDL pixels, game launch, visual fidelity, focus, camera and effect slowdown benchmarks.
UNRESOLVED: why independent bitstream code needs lookahead beyond packed payload, native original decoder behaviour with uninitialized padding, object/block-to-world classification, actual 3D occlusion/light geometry and performance.

## Next stage
Review the original RNC bit-stream read functions against the temporary Python decoder and bound the lookahead issue; isolate original C decoder in test harness if feasible. Then inspect decoded hblk/col and map column data against the original assembly renderer for candidate object and occlusion classification. Existing game resources are sufficient for further offline audit; do not request missing historical v0.7 ZIP. Windows testing will ultimately be a new reconstructed baseline. All commercial game bytes stay private.
