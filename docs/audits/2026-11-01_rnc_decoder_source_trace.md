# Syndicate 2026 — RNC-1 Decoder Source Trace

Date: 2026-11-01 (Australia/Brisbane)
Status: focused source audit complete; actual palette decompression and runtime validation pending.
Source branch: feature/v0.9-visual-showcase; protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.

## Findings
- The repository includes its own RNC-1 unpacker at engine/bflibrary/src/general/rnc_1fm.c, with API in engine/bflibrary/include/rnc_1fm.h. There is no need to choose or introduce an external decoder just to understand existing unpacking.
- engine/bflibrary/src/general/gfile.c:LbFileLengthRnc interprets the RNC signature and declared decompressed length; LbFileLoadAt reads the file and calls UnpackM1, which delegates to rnc_unpack.
- rnc_1fm.h defines RNC_SIGNATURE 0x524E4301 and an 18-byte header, consistent with the five archived hpal resource headers already inspected. The five palette files each declare 768 decompressed bytes.
- rnc_1fm.c:rnc_unpack checks packed and unpacked CRCs and decodes Huffman/RNC data. The source offers explicit error statuses; decoding the actual assets through this function is not yet performed.
- rnc_1fm.c:UnpackM1 (lines 440–456) makes a temporary allocation using LbMemoryAlloc, then calls LbMemoryCopy **before** testing whether the allocation returned NULL. This is a conditional allocation-failure hazard in original source, not a demonstrated crash. The audit does not authorize a fix.
- UnpackM1 names its temporary copy size packedsize but initializes it from header offset +4 (declared unpacked size). Together with gfile.c's use of the declared unpacked length to request reading into an allocated buffer, that merits an isolated test. The actual compressed payload size is at header offset +8; we have not established whether copying beyond the shorter returned file data causes observable problems in normal gameplay.

## Design gate
The next step is a read-only offline test harness using the existing source RNC unpacker (or carefully validated equivalent), comparing returned decoded byte count, CRC success, and palette triplets for all five local private hpal files. Test only with the user's already uploaded archives; keep proprietary decoded output out of public GitHub. Do not introduce a decoder dependency into production game code.

## Verification
PASS: GitHub read of gfile.c, rnc_1fm.c, rnc_1fm.h, direct symbol and control-flow inspection.
PASS: Matching header fields to separately inspected private files.
NOT RUN: native compilation of unpacker, real Huffman decode, palette RGB inspection, renderer capture, Windows game startup or gameplay tests.
OPEN: allocation-failure handling, temporary-copy exact bounds, actual resource CRC/decode outcomes.

No engine modifications, workflow modifications, or new features. The protected main remains unchanged; rollback is documentation-only.
