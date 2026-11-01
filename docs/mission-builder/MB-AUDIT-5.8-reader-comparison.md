# MB-AUDIT-5.8 — Comparison of mission resource readers
Date: 2026-11-01. Branch feature/mission-builder-audit. Static documentation audit.

Sources: engine/src/syndre.sx, engine/bflibrary/src/general/gfile.c, engine/bflibrary/src/general/rnc_1fm.c and engine/bflibrary/include/rnc_1fm.h.

## Confirmed source findings
- Legacy assembly LbFileReadRNC returns a handle-related value and does not compare its LbFileRead_ return count to the requested count before calling UnpackM1.
- C LbFileLoadAt returns decoded length or -1 on decompression error, but checks read_status == -1 rather than requiring read_status == filelength.
- C rnc_unpack parses unpacked and packed lengths, checks header bounds against a 1 GiB threshold, and includes packed and unpacked CRC checks and several Huffman-error returns.
- C UnpackM1 at rnc_1fm.c lines 440–455 is NOT established safe: it allocates based on the header field at offset 4 (unpacked size) capped by caller bufsize, copies that many bytes into allocated storage before testing the allocation result, then calls rnc_unpack.
- rnc_unpack derives its compressed input end from header offset 8 plus the 18-byte header. The copy length in UnpackM1 is not demonstrated to cover that entire input in every valid or malformed resource.
- C LbFileLengthRnc reads a header buffer and checks specifically for -1, not a short positive read, before interpreting the signature.
- These are static source risks, not reproduced crashes or confirmed vulnerability in an installed Windows game.

## Integration conclusion
Do not replace the legacy loader with the C loader on an assumption of safety. No resource writer, validator or custom game mode is approved from this comparison. Next: establish strict input-size, output-size and byte-count contracts with lawful original sample assets and isolated testing.

## Verification
Read repository source only; no code modification, assets, saves, compilation or gameplay tests. Protect original and American Revolt behaviour, v0.7, fixed 16 FPS and save isolation.