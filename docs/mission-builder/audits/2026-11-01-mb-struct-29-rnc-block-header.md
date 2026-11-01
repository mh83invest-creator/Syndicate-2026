# MB-STRUCT-29 — RNC method-1 post-length fields and block counter
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT ONLY; no decoder, file-processing or engine changes.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Source-verified header consumption
- `UnpackM1` checks `RNC\x01` (+0..+3), and calls `blong` twice for big-endian unpacked and packed sizes (+4..+7 and +8..+11); see MB-STRUCT-28.
- After `blong`, `%esi` points to byte +12. `mov 0x5(%esi),%bl` (62247) therefore reads **header byte +17** into `unpackm1__blocks` (62248). `add $0x6,%esi` (62249) advances input to **+18**, the beginning of the compressed bitstream by this implementation.
- Decoder does not visibly interpret bytes **+12..+15** as CRC/checksum fields in this routine; their actual semantic meaning and integrity requirements are NOT independently source-verified here. Byte **+16** is read at 62258 in in-place overlap relocation arithmetic; it is not established as a capacity guarantee or validation check.
- The `packed length + 0x12` calculation at 62250 uses **18-byte header overhead** for overlap/relocation decisions. That confirms the decoder's presumed header-to-data boundary; it is not proof the input file has sufficient physical bytes.
- Three Huffman tables are generated per block (62291–62297), then a 16-bit `unpackm1__counts` is loaded from the bitstream (62298–62300). Raw and reference runs are processed, then `decb unpackm1__blocks` controls the outer loop (62337–62341).
- Because the byte-sized block count decrements and tests for zero **after processing a block**, initial value 0 may wrap to 255 and drive additional iterations; the routine shows no explicit initial-zero reject. This is a *conditional malformed-input risk*, not a reproduced crash.
- `UnpackM1` takes no input/output capacities and provides no verified CRC comparison in the inspected routine; it cannot be treated as a safe untrusted-file validator.

## Proposed bounded validation rules — NOT IMPLEMENTED
C01: Require full verified minimum 18-byte method-1 header before any field access.
C02: Decode signature, unpacked and packed sizes independently and verify physical extent (at least 18 plus packed bytes, with any trailer convention separately specified).
C03: Treat +12..+15 as uninterpreted checksum candidates until corroborated by format documentation or genuine fixture; do not claim the legacy decoder enforces them.
C04: Interpret byte +16 only after its role has been established; do not repurpose legacy overlap arithmetic as a safety check.
C05: Read block count at +17 and reject zero before decompression, subject to validation against authentic format examples.
C06: Bound per-block Huffman-table construction, 16-bit run counts, literal/copy operations, input consumption and output production by explicit capacities.
C07: Reject inconsistent physical packed lengths, decoded byte counts, exhausted input, malformed tables or invalid copy-back references.
C08: Require independent checksum verification if the format mandates it; unresolved until documented.
C09: Preserve source resources and do not call native `UnpackM1` with arbitrary imported bytes.
C10: Confirm all offset assumptions with authorised real `gameNN.dat` samples before offering parse compatibility.

## Next audit
MB-STRUCT-30: inspect `make_huftable`, `input_bits` and `input_value` capacity and termination contracts, separating observed code from inferred format requirements.

## Verification
Indexed GitHub assembly fragment review only. No fixtures, test suite, builds, authentic asset inspection, gameplay or modified engine/source.
