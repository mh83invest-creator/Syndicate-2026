# Syndicate 2026 — Private Asset RNC1 Packed Integrity Audit

Date: 2026-11-01 (Australia/Brisbane)
Stage: complete packed-header and packed-CRC validation of the supplied v0.6 ZIP, NOT decoded asset validation.
GitHub branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40
Private input (not uploaded to GitHub): user-supplied Syndicate_2026_v0.6_Two_Menus_30FPS_Test(1).zip.

## Work performed and method
Used Python standard-library zipfile to read the 437 entries beneath data/. Identified RNC1 members by the exact four-byte magic 52 4e 43 01. Parsed big-endian 32-bit decompressed and packed lengths at header offsets 4 and 8, and packed CRC at offsets 14–15. Verified each RNC1 member's total byte count equals 18 + declared packed size; computed a CRC16 using initial value zero and reflected polynomial 0xA001 over the bytes from offset 18 onward. This is a standalone synthetic implementation of the packed-data checksum, not execution of the game's RNC unpacker.

## Results
- 437 total data files; 143 RNC1-compressed files; 294 entries without RNC1 signature.
- All 143 compressed entries have the declared packed payload length matching their ZIP member byte length.
- All 143 packed payload CRC16 values matched the corresponding RNC header packed-CRC field. Zero mismatches.
- The 143 compressed files have combined header-declared decompressed length of 12,345,697 bytes and combined packed payload size of 1,819,620 bytes.
- The five high-resolution palettes each declare an unpacked length of 768 bytes; all five packed CRCs matched. All are privately retained resource files; none of their original data bytes or game imagery are included here.
- CRC confirmation validates the archived *compressed stream* against its header checksums. It does **not** show that Huffman unpacking succeeds, that the decoded CRC matches, or that the engine can render/map these files.

## Verification status
PASS: ZIP member inventory, 143 compressed header-length checks, 143 compressed CRC16 checks, five palette checks.
NOT RUN: engine decoder, decompressed CRC, palette RGB values, geometry parsing, Windows gameplay or asset compatibility with the historical v0.7 executable.
UNRESOLVED: decoded data contents and compatibility with current graphical paths.

## Next action and safeguards
Execute verified RNC1 decoder in a read-only offline harness; compare unpacked lengths and header-declared unpacked CRCs before assessing 256-entry palette RGB values and level mapping. Do not commit proprietary retail files or extracted graphics to public GitHub. No source code, game assets, workflows or main baseline were changed.
