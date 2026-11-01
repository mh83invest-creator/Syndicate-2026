# MB-STRUCT-27 — RNC header and loader failure-path contract
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT, NO CODE CHANGE

Source: engine/src/syndre.sx GitHub blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Exact source observations
- `LbFileLengthRnc` (14964–15041) opens a file and requests 0x14 (20) header bytes. It constructs a four-byte signature `RNC\x01` (14971–14983), calls `ac_strncmp` for four bytes (14996–15004), and in the matched branch assembles a 32-bit big-endian length from the first four bytes after that signature (15005–15017).
- The value is treated by the loader as a byte-count argument for `LbFileRead_`. It is *not yet validated* whether this value represents packed bytes, expanded bytes, or another format field from genuine assets; no authentic file was tested. Established RNC method-1 convention suggests an unpacked length, but this source-only audit does not establish a safe physical read length.
- On signature mismatch, `LbFileLengthRnc` calls `ac_dos_filelength` and returns the physical file length instead of rejecting the input (15019–15024).
- On an open failure its default length is -1, while it tests file handle `<=0` (14970, 14985–14989, 15029–15036). No observed short-header read-count check precedes the signature/length interpretation.
- `LbFileReadRNC` (14916–14960) opens again, calls `LbFileLengthRnc`, reads that returned length into caller-supplied destination and then calls `UnpackM1` in-place with two identical pointers (14932–14943).
- If `UnpackM1` returns negative the wrapper prints an error, but its final return register contains stored `%esi` file handle, not the negative decoder status (14944–14955). The caller-visible return is therefore not an adequate decode-success contract. The inspected wrapper does not pass a destination-buffer capacity.
- No known malicious file crash, over-read or corruption was reproduced; source-defined risks should not be presented as measured exploits or existing gameplay defects.

## Bounded read-only header validator acceptance specification (NOT IMPLEMENTED)
H01: Require an explicitly supported file/container format; reject unrecognised and malformed signatures (no physical-file-length fallback).
H02: Require complete header with exact length and supported RNC method/version; short/truncated inputs reject before reading fields.
H03: Extract advertised packed and unpacked lengths using independently verified field offsets and endian rules; do not take `LbFileLengthRnc`'s name or return value as a proof of file size semantics.
H04: Check compressed input length against actual file size and reasonable header/trailer policy; reject arithmetic overflows and trailing-data ambiguity until specified.
H05: Bound unpacked bytes to an explicitly allocated, capped buffer before any decode; reject zero, unsupported or excessive lengths according to verified format contract.
H06: Run only an independently bounded, audited decoder; require exact decoded byte count and explicit success, check integrity/checksum fields if format requires.
H07: Treat non-RNC original fixtures as a separate explicit format candidate only after native-file provenance verification; never silently interpret garbage as valid.
H08: Inspect objective/entity memory only after format and exact output layout are independently established (MB-STRUCT-6, 13, 26).
H09: On all failures, leave original files unchanged, return deterministic diagnostics and do not invoke the native mission loader.
H10: Verify against at least two authorised authentic `gameNN.dat` files and synthetic negative cases before claiming format compatibility.

## Open blockers
Full authentic RNC header layout and the source of packed vs expanded lengths; checked decoder implementation; actual compressed and decoded mission data; Windows/platform tests. Note that MB-STRUCT-15 resolved the filename to `data/gameNN.dat`, but not its physical layout.

## Next
MB-STRUCT-28: inspect the native `UnpackM1` call chain / header consumption for concrete packed-vs-unpacked length interpretation; do not implement or run a decoder.

## Verification
Read source assembly only, created documentation and updated handovers; no original files, tests, build, runtime or engine changes.
