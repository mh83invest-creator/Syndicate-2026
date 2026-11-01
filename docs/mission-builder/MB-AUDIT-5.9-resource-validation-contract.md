# MB-AUDIT-5.9 — Proposed mission resource validation contract
Date: 2026-11-01. Branch: feature/mission-builder-audit. Status: specification only, no implementation.

## Source references
- engine/bflibrary/include/rnc_1fm.h: RNC_SIGNATURE 0x524E4301; RNC_HEADER_LEN 18; error codes and decoder declarations.
- engine/bflibrary/src/general/gfile.c: LbFileLengthRnc and LbFileLoadAt.
- engine/bflibrary/src/general/rnc_1fm.c: rnc_unpack, UnpackM1; CRC and decoding paths.
- engine/src/game_data.h: packed CPObjective and 128-record declaration.
- engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf: legacy read/decode path, level memory layout.

## Proposed acceptance gates (NOT implemented)
G1. Determine the actual file length, and require at least 18 readable bytes before parsing RNC fields.
G2. Verify RNC Method 1 signature and validate header fields, including integer overflow in header length plus packed payload length.
G3. Require the compressed payload and header to fit the actual bytes available; reject truncation and unexpected trailing data according to a separately approved compatibility rule.
G4. Establish the destination memory capacity and require the decoded length to fit; confirm whether an exact length is required using lawful retail samples. The observed level memory region is 116010 bytes, but this is not proven to be each file's required decoded length.
G5. Require full read counts for every requested input block, with distinct errors for EOF, IO failure and decompression failure.
G6. Check decoded length, packed and unpacked CRC, and structural validation of tactical objective entries, CP objective entries, and entity links before exposing data to gameplay.
G7. Perform loading and verification in controlled scratch storage; do not modify or overwrite retail assets or existing saves.
G8. Test original mission compatibility, malformed headers, truncated input, lengths near capacity, invalid references, each supported campaign and known-good replay. No runtime test has been performed.

## Existing versus proposed
The C decoder contains header and CRC checks and returns error codes. Neither C LbFileLoadAt nor legacy assembly LbFileReadRNC has been verified as a safe end-to-end loader for arbitrary custom data. This contract is not approval to replace current game loading or change timing, save routing, mission logic, or campaign selection.

## Next audit
Trace true file-to-memory extent and any loading-time alterations to objective fields. Use legally provided original mission resources for format validation before implementation.

## Verification
Source inspection only. No game code, graphics, save or asset changes; no build or Windows gameplay test. Preserve v0.7, fixed 16 FPS, original and American Revolt campaigns, and siloed-save requirements.
