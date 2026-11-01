# MB-STRUCT-14 — Mission resource/RNC loader boundary

Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder source audit; **no implementation**
Source: `engine/src/syndre.sx` (assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`).

## Observed native read path
- `load_map_level` (27845–27933) calls `clear_savegame` then, on the relevant `DrawFlags` branches, passes the caller-provided filename and `level__Seed` destination to `LbFileReadRNC` (27873–27880, 27915–27919); it then continues to `init_level_data` (27930). The inspected branches do **not** immediately validate a return code or unpacked byte count prior to interpreting level memory.
- `clear_savegame` (27627–27636) clears the level-memory region from Seed to End; zeroed memory is not proof of a valid loaded mission.
- `LbFileReadRNC` (14916–14960) opens a path, obtains a length from `LbFileLengthRnc`, invokes `LbFileRead_` with that length and the caller-supplied destination, then calls `UnpackM1` with destination/source the same pointer. Its inspected signature has no explicit destination-capacity parameter, and the wrapper logs a negative unpack result but returns the file handle value rather than propagating the decompression error.
- `LbFileLengthRnc` (14964–15041) reads up to 0x14 header bytes and compares an RNC magic/version sequence. When recognized it extracts a 32-bit big-endian size from header bytes 4–7; when unrecognized it falls back to the physical file length. The observed code does not prove bounds checks for short/corrupt headers, valid decompressed length, or destination capacity.
- Separate save/load path near 1232–1245 formats `aGameDat` and checks the wrapper result against -1. **Do not conflate this savegame filename with the tactical mission file passed into `load_map_level`.** Previous handover shorthand `gameNN.dat` for the mission load was unverified and must not be treated as established. The precise caller-provided mission filename/path and archive mapping require tracing.

## Risk statements — conditional, not reproduced
1. A length reported by a corrupt or malicious file may be inconsistent with allocated destination capacity. The wrapper's source does not expose a destination capacity or checked upper bound; no demonstrated memory violation has been reproduced.
2. A negative return from `UnpackM1` is logged inside this wrapper without an explicit negative error propagation via its return. This does not prove corruption in legitimate retail files.
3. The `DrawFlags` paths differ; other loader paths may exist. Exact native file layout, expected header, decompressed length, in-place unpack semantics and asset provenance remain unverified.

## Safe inspector contract
- **Do not reuse `LbFileReadRNC` or `UnpackM1` directly** on untrusted Mission Builder input as a substitute for a validated decompression boundary.
- Keep two explicit stages: (1) independently bounded file/container inspection with verified RNC header, packed and expanded sizes and format; (2) immutable, length-checked decompressed memory-image inspection according to MB-STRUCT-13.
- Reject unknown formats, bad magic/version, short files, length arithmetic overflow, over-capacity output, inconsistent unpack results and incomplete objective table data before any entity or objective dereference.
- Never assume the candidate `0x1bd32` objective offset is an offset into the **compressed** file. Verify the native filename and actual image offsets with controlled fixtures first.
- Preserve original tactical engine, campaign selection, campaign save isolation, and **between-mission-only** saving. No mission writer, custom mission installer or engine loader edits in this audit.

## Next audit
MB-STRUCT-15: trace the native `load_map_level` call sites and the source of the filename string; distinguish campaign-resource, map-resource and savegame naming paths. Confirm exact resource path only when source-backed.

## Verification
Source-only inspection. No instrumented test, actual RNC decompression, retail files, engine modifications, compilation, Windows or gameplay tests.
