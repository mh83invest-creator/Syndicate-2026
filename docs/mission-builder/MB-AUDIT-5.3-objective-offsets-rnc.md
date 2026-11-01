# Mission Builder Audit 5.3 — Level layout and RNC loading
Date: 2026-11-01. Documentation-only review.

Source: engine/src/syndre.sx at blob c35e22621a93d1e07c44d40de8867446ff7c64bf and engine/bflibrary/src/general/gfile.c.

## Source observations
- level__Seed address 0x80108.
- level__Objectives address 0x9BE3A, relative offset 0x1BD32, length 0x70 = 112 bytes (8 x 14).
- level__CPObjectives address 0x9BEB2, relative offset 0x1BDAA, length 0x780 = 1920 bytes (128 x 15).
- level__End address 0x9C632, total level memory region 0x1C52A = 116010 bytes.
- These refer to decompressed memory positions, not confirmed compressed file positions.
- Assembly LbFileReadRNC (lines 14916–14960) opens a file, obtains length, reads, runs UnpackM1, prints on decompression failure, and returns a handle-related value. It does not visibly validate a short positive read against expected length.
- load_map_level calls this loader at approximately 27880 and 27919 and does not immediately check its return value in those branches.
- Separate C function LbFileLoadAt in gfile.c uses a different return contract, so it cannot be assumed to protect these assembly calls.

## Unresolved
Confirm output length, RNC format and reader error semantics, resource references and actual retail mission data before writing any mission file. No edit format validated.

## Verification
Static code reading only. No engine modifications, builds, runtime tests, resource changes or save migration. Maintain protected v0.7, 16 FPS and original campaigns.