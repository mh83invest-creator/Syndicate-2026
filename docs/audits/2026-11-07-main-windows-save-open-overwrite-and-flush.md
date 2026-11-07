# Syndicate 2026 — Main Systems Audit: Windows save open, overwrite, and flush semantics

Date: 2026-11-07 (Australia/Brisbane)
Branch: main; documentation-only investigation
Status: source-level audit completed for both SDL file backends. Actual Windows runtime, installed data layout, and retail save compatibility remain untested.

## Evidence

Direct repository reads:
- `engine/bflibrary/Makefile.am`: selects `src/x86-win-sdl2/sfile.c` when `USE_SDL2`, otherwise `src/x86-win-sdl/sfile.c`.
- `engine/bflibrary/src/x86-win-sdl2/sfile.c` and `engine/bflibrary/src/x86-win-sdl/sfile.c`, each Git blob `af534f3f1b912150360095862b062f8a8f57ff43`: same implementations of `LbFileOpen`, `LbFileRead`, `LbFileWrite`, `LbFileClose`, `LbFileFlush`, `LbFileLengthHandle`, `LbFileRename`.
- `engine/bflibrary/include/bffile.h`: `Lb_FILE_MODE_NEW=0`, other mode constants; file return-value contracts; `LB_FILENAME_TRANSFORM=1`.
- `engine/src/game_data.c` blob `23a7159fc016e58872d30b61b46ba62b6efabac7`: `save_player` opens `save/%02d.gam` using `Lb_FILE_MODE_NEW`, writes the legacy fields without checking return counts or close status, returns true after successful open; `load_player` and `load_save_text` similarly lack full-length checks.
- `engine/src/game.c`: alternate user-save path transform.
- Prior reports `2026-11-06-main-save-write-integrity-static-audit.md` and `2026-11-06-main-save-io-contract-and-layout.md`.

## CORE-WINFILE-01 — Existing files are opened WITHOUT truncation

Both Windows/SDL backends implement `Lb_FILE_MODE_NEW` using `_sopen(fname, O_RDWR|O_CREAT|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)` under the Windows branch. Neither `O_TRUNC` nor an equivalent explicit `ftruncate` is in this enabled path. A commented-out, disabled block includes `_O_TRUNC`, but is not executable. The POSIX alternative also uses `open(fname, O_RDWR|O_CREAT, ...)` with no `O_TRUNC`.

If the file already exists, opening for a new save begins at offset zero and does not remove its existing contents by design. Writing fewer bytes leaves stale trailing bytes; writing the ordinary full payload overwrites the beginning but is not protected by truncation/rollback. The exact 28,247-byte expected legacy payload is a source-derived calculation from the predecessor report, not yet measured from a Windows save file.

Important nuance: lack of truncation can incidentally preserve a previous tail on short writes; it does **not** make saving safe. The overwritten prefix is already altered, and any old suffix may belong to the previous save. A failed save therefore has a realistic torn-state risk without needing to assume a fully erased file.

## CORE-WINFILE-02 — Backend returns byte counts; save caller ignores them

Both backends call `read(fhandle, buffer, len)` and `write(fhandle, buffer, len)` and return their results. `bffile.h` documents number of bytes transferred, with read error represented by -1. The save caller makes seven consecutive writes and does not compare any returned length with requested bytes, then closes and returns success (if open succeeded). A short write, error, or later close failure can therefore escape the caller's success reporting.

This is static failure-path reachability, not an observed real-user corrupted save. The wrapper offers `LbFileLengthHandle` and `LbFileClose` with error codes that a future approved implementation could inspect.

## CORE-WINFILE-03 — Flush wrapper has a Windows descriptor/handle mismatch concern

Under `WIN32`, `LbFileFlush` invokes `FlushFileBuffers((HANDLE)fhandle)`, while `LbFileOpen` returned a CRT low-level descriptor from `_sopen`. Passing a CRT descriptor cast directly to a Win32 HANDLE is not equivalent to obtaining the underlying OS handle through a descriptor-to-handle conversion. Furthermore, the wrapper treats GetLastError() == 6 (invalid handle) as success after a failed call. This is a source-visible *potential inability to guarantee flushing*, subject to the actual Windows compiler/runtime API and handle representation.

The current `save_player` does not call `LbFileFlush`, so this concern is **not** itself a demonstrated failure of an active flush in that save path. It is a separate blocker to assuming a naive 'add flush' fix would be reliable.

## CORE-WINFILE-04 — Rename exists but no transactional save mechanism is used

Both backends implement `LbFileRename` by applying filename transforms and calling `rename`. No temporary-file-then-commit/rename sequence is visible in `save_player`; it writes directly to the canonical slot file. Do not infer atomicity merely from `LbFileRename` existing in the library. Atomic replacement behaviour on Windows, including existing destination semantics, has not been validated.

## CORE-WINFILE-05 — Save-root discrepancy is preserved

With `LB_FILENAME_TRANSFORM=1`, both backends transform names before opening. `SyndFXFileNameTransform` applies `GetDirectoryHdd()` to relative `save/NN.gam` paths. By contrast, `game_transform_path_full` separately chooses `GetDirectoryUser()` for paths beginning `save/`. Actual installed save location and path implementation must be tested before any migration or campaign namespace change; this is coordinated with the Mission Builder audit.

## Diagnostic and acceptance sequence (not executed)

1. Determine exact Windows build flags/CRT and the file backend in the published Windows artefact.
2. Work only in a disposable installation and save directory; back up user/legacy saves first.
3. Create known-size benign test files; open with `Lb_FILE_MODE_NEW`, write fewer bytes, close, measure unchanged tail; separately inject partial writes and open failure to confirm caller success reporting.
4. Validate 28,247-byte assumption against the exact target compiler and genuine known-good saves, including struct packing and unmodified original-mode compatibility.
5. Trace the proper Windows file-handle conversion and flush/close error semantics; test only after a separately approved diagnostic patch or harness.
6. Design any future atomic-save strategy with explicit compatibility, path, campaign namespace and backup requirements; do not silently change game economics or existing save semantics.
7. Regression tests must include two original campaigns, legacy saves, new campaign save isolation, same slot numbers per campaign, error handling, Continue/menu routing, and unaffected gameplay.

## Verification and handover

**Done:** inspected both selected SDL backend implementations and their Makefile selection; cross-referenced the native save consumer, file API, prior source-derived save-layout calculation, and shared-path handover.

**Not done:** changed engine source, compiled code, run CI, performed Windows filesystem tests, run a game, profiled explosions, tested save repair, inspected actual original data or modified any user's saves.

**Protected:** v0.7; 16 FPS tactics; original campaigns, assets and gameplay; separate Graphics and Mission Builder branches.

**Next:** verify Windows build configuration and dependencies for this exact file implementation; then resume the explosion performance source audit and prepare a diagnostic-only timing contract before any code change.
