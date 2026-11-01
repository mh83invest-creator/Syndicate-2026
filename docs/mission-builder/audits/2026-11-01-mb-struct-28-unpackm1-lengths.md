# MB-STRUCT-28 — UnpackM1 packed/unpacked length and error contract
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder source audit only. No decoder implementation.
Source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Confirmed by decoder itself
- `UnpackM1` at source lines 62225–62356 starts by checking the two 16-bit words at input: 0x4e52, then 0x0143 (62236–62242), corresponding to byte signature `RNC\x01` on little-endian execution.
- It immediately calls `blong` twice: first stored as `unpackm1__unpack_len`, second as `unpackm1__pack_len` (62243–62246). `blong` (62360–62366) converts 32-bit big-endian byte order. Therefore the header after the four-byte signature contains **unpacked length at offsets +4..+7** and **packed length at +8..+11**. This RESOLVES MB-STRUCT-27's uncertainty over those field semantics based on stronger decoder-source evidence, but still needs genuine fixture confirmation for format compatibility.
- `UnpackM1` takes source and destination pointers, and the legacy wrapper invokes them pointing to the same area (14940–14943). In-place overlap handling copies packed data backwards in some address-overlap situations (62247–62284). `unpack_len` and `pack_len` govern relocation; unchecked arithmetic here must not be transplanted into a new validator.
- Native output uses Huffman table helpers, bit input, `rep movsb` for raw literals and backreferences (62285–62341). The inspected function accepts no explicit input or output buffer capacities, so the source does not demonstrate robust bounds protection against malformed inputs.
- On its explicit signature mismatch branch, `UnpackM1` sets `eax=0` and the carry flag (62345–62349); on successful loop completion it returns `unpack_len` and clears carry (62342–62344). The wrapper `LbFileReadRNC` checks only whether signed `eax<0` before printing decompression error (14944–14950), so this explicit signature failure is not necessarily detected by that signed-value check. Caller receives file handle, not decoder status (14954–14955).
- `LbFileLengthRnc` returns the header's **unpacked** length when the signature matches, and `LbFileReadRNC` passes that value into `LbFileRead_` (14932–14939). This is source-observed legacy behaviour, not a rule to copy into a safe compressed-file reader: actual physical file length and packed length need separate validation.

## Bounded header-only validator gates (revised; NOT IMPLEMENTED)
1. Require at least the four-byte signature and 12-byte base length fields before reads; when interpreting the native decoder's later header fields (block count etc.) require full verified header length.
2. Match `RNC\x01`, decode big-endian 32-bit unpacked length at +4, packed length at +8. Require nonzero supported values and platform-independent bounded conversions.
3. Validate physical file extent against packed payload, verified header length, and any allowed trailer policy. Do not read `unpack_len` bytes from compressed input merely because the legacy wrapper does.
4. Bound output by independent maximum and allocation size before decode; reject arithmetic overflow and impossible overlap.
5. Require explicit decoder error status, consumed packed bytes and produced bytes exactly matching declared lengths. Avoid the legacy wrapper's file-handle return as a success condition.
6. Confirm header checksum/CRC and block fields independently before declaring actual RNC method-1 compatibility; source-only offsets beyond +11 and checksum semantics are still to be audited.
7. For all malformed containers, reject without executing native `UnpackM1` or modifying any original resource.
8. Validate with authorised real `gameNN.dat` samples before parser/write claims.

## Next
MB-STRUCT-29: inspect RNC post-length header fields, checksum and block-count handling, and whether a bounded reader can derive all necessary input constraints.

## Verification
Static complete assembly fragment inspected. No game files, decoder runs, synthetic tests, builds, gameplay or source modifications.
