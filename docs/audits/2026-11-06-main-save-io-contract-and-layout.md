# Syndicate 2026 — Main Systems Audit: Save I/O Contract and Serialized Layout

**Date:** 2026-11-06 (Australia/Brisbane)  
**Branch:** main, documentation only  
**Predecessor:** `docs/audits/2026-11-06-main-save-write-integrity-static-audit.md`  
**Status:** New static evidence; Windows backend and runtime untested.

## Directly inspected GitHub source

- `engine/src/game_data.c` (blob `23a7159fc016e58872d30b61b46ba62b6efabac7`): all C save functions.
- `engine/src/game_data.h` (blob `52bc05db907d8d571465e6a90fb2eb9330d4d7f6`): `GAME_DESC_TEXT_MAX_LEN=20`.
- `engine/src/player.h`: `selected_team[4]` and eight player records.
- `engine/src/research.h`: packed `Research` layout.
- `engine/bflibrary/include/bffile.h` (blob `445b831f62cdd0b21b0ee7a0822cf376a3e6d537`): documented read/write/flush contracts.
- `engine/bflibrary/src/general/gfile.c`: `LbFileCopy` verifies full-length I/O; `LbFileSaveAt` returns the count written without verifying expected length.
- `engine/bflibrary/include/bftypes.h`: generic platform-dependent C typedefs.
- `engine/src/game.c`: file path resolver distinction.

## CORE-IO-01 — The API supports actual partial-transfer detection

The documented `LbFileRead(handle, buffer, len)` return is number of **bytes actually read**, or -1 on error. `LbFileWrite(handle, buffer, len)` returns number of bytes written. Success of a particular operation requires **`result == requested_len`**, not merely `result != -1`. `LbFileFlush` exists and documents 1 on success, 0 on error. `LbFileLengthHandle` is available for checking opened-file size.

Crucially, the repository itself demonstrates this comparison in `LbFileCopy` in `general/gfile.c`, checking each read and write against the block size. Therefore the existing save routines' ignored lengths are a tangible missing check in caller code, not an API limitation. Actual behaviour of the underlying Windows backend for partial writes, close failures and durable flush is still not confirmed.

## CORE-IO-02 — Derived save record layout

`save_player` emits fields sequentially (and `load_player` reads the same fields in the same order):

| Serialized field | Bytes | Exclusive end offset |
| --- | ---: | ---: |
| Description (`GAME_DESC_TEXT_MAX_LEN`) | 20 | 20 |
| `players` (asserted `sizeof(players)`) | 8,376 | 8,396 |
| `country_states` (asserted `sizeof(country_states)`) | 500 | 8,896 |
| `weapons` (asserted equipment block) | 10,020 | 18,916 |
| `cybmods` (asserted parts block) | 8,838 | 27,754 |
| `research` (packed layout, 32-bit int) | 489 | 28,243 |
| `selected_team[4]` (ubyte) | 4 | **28,247** |

`Research`: three one-byte fields + one four-byte `int` + 10 × (24 two-byte words) + final two-byte word = 3+4+480+2 = 489 bytes when the target uses a 4-byte C `int` and `#pragma pack(1)` is honoured.

**28,247 bytes is a source-derived expected size for the existing 32-bit-compatible packed layout, NOT an experimentally measured Windows save file size.** The code asserts the four large buffer sizes on builds with active assertions, but no save-size assertion was observed. The type `int` requires confirmation under the exact Windows target compiler; no compiler run occurred. Preserving the legacy format remains mandatory. Future validation should calculate `sizeof(research)` and `sizeof(selected_team)` at runtime or compile time, not hard-code the derived total without verification.

## CORE-IO-03 — Detectable integrity gaps and appropriate acceptance tests

- A file may open but contain fewer than the expected number of bytes; `load_player` does not check size or individual read returns.
- A destination may open but accept less data than requested; `save_player` returns success without checking returned byte counts or close status.
- The 20-byte description is the only portion inspected by `load_save_text`; display of a slot is not whole-file validation.
- The deduction of >30,000-credit balances occurs before the file-open attempt, as documented in the predecessor audit; changing this operation demands review of original economics first.
- The file API exposes `LbFileFlush`, but no explicit flush or close-result check occurs inside `save_player`; this is an observed omission, not proof of disk-loss incidents.

Future *approved* tests should compare known legacy files against derived size, test each truncation boundary, observe I/O byte counts on Windows, inject open and short-write failures, and verify no user saves are altered. Use disposable fixtures and keep gameplay economics and campaign identity unchanged.

## Compatibility and workstream boundaries

The source still registers `SyndFXFileNameTransform` for general `LbFileOpen` calls, resolving relative paths from HDD/data root; `game_transform_path_full` independently redirects `save/` to user root. Both paths remain relevant to Mission Builder's approved campaign save isolation. No migration or namespace changes are authorised here.

## Verification and limitations

**Performed:** cross-checked read/write API headers, an independent library caller's use of return counts, save-function order, packed research definition, selected-team declaration and byte arithmetic.

**Not performed:** inspection of final Windows SDL file backend, compilation, CI, real file-size comparison, failure injection, gameplay, instrumentation or frame profiling. Some likely backend paths were attempted without a matching file; do not claim that those backends have been inspected.

**No engine changes.** Protected v0.7, 16 FPS tactical simulation, original campaigns, other branches, and existing saves remain untouched.

**Next:** trace active platform implementation of `LbFileRead`/`LbFileWrite` and whether an opened existing save is replaced/truncated immediately under `Lb_FILE_MODE_NEW`; separately continue explosion hot-path profiling preparation without modifying simulation order.
