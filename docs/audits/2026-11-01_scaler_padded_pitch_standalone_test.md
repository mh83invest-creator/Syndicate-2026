# Syndicate 2026 — Source-derived scaler test with padded destination pitch

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: read-only graphics audit plus isolated synthetic C test. No engine source changes.

## Source context
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924-970` and SDL1 equivalent `engine/bflibrary/src/x86-win-sdl/sscreen.c:750-797` contain a custom indexed-to-true-colour scaler.
- The routine accepts width, height and bytes per destination pixel, but no destination row pitch; offsets increment by `dst_bpp` across the complete image. The original store is through `long *`, which is eight bytes on the prior Linux LP64 host and commonly four on Win32.

## Executed standalone test
- Local test file: `/mnt/data/synd_scaler_audit/pitch32.c` in the session working container. Built by Clang with `-g -O0 -fsanitize=address -fno-omit-frame-pointer`; test binary at `/mnt/data/synd_scaler_audit/pitch32`.
- Function control flow and offset arithmetic copied from the previous isolated source-derived scaler harness. **For this test alone**, replaced original `long *` assignment with a 4-byte `uint32_t` value and `memcpy(..., 4)` so the test isolates *pitch handling* without the unrelated eight-byte Linux LP64 store-width effect. This is an explicitly modified model, NOT execution of the exact original function.
- Synthetic source: 2x2 indexed pixel array; output: 3x3, four bytes per pixel, declared row pitch 16 bytes (12 bytes of pixel data + 4 padding bytes), 48-byte initialized allocation with padding marker `0xCC`.
- Running the binary produced: `row_padding_modified=8 expected_first_row_pad_start=12 sizeof_storage=48`, exit code **3**. The nonzero exit is intentional for detected padding alteration, **not an AddressSanitizer overflow**.
- Conclusion: source's destination offset model is not pitch-aware. With padded output rows, some pixel writes fall into what should be padding, and their intended row locations are shifted. The test did not report an out-of-bounds write: the 48-byte backing allocation was large enough for the compact 36-byte sequence.

## Boundaries
- This is a synthetic C reproduction, not an SDL surface or Windows executable test. It demonstrates a row-layout mismatch only if an affected destination format actually has pitch differing from width * bytes-per-pixel.
- No supported runtime evidence yet establishes Windows SDL backend, surface pitch, actual display mode or scaled indexed-to-true-colour branch reachability. Do not patch game code or claim visible gameplay corruption solely from this test.
- Prior report `2026-11-01_source_derived_scaler_asan_lp64.md` covers original source-derived 8-byte `long` store on Linux LP64; this test isolates the independent pitch issue.

## Verification ledger
PASS: compiled synthetic modified scaler, ran under AddressSanitizer, captured row-padding modification count and exit status.
NOT RUN: real SDL surface/pitch test, native 32-bit Windows build, original exact scaler on Win32, graphical screenshot, gameplay.
CHANGED IN GITHUB: this report and graphics handover index only. Local standalone test file not committed.
NEXT: verify exact Windows SDL destination format and pitch; assess whether scaled 8-bit draw surface reaches custom scaler; test source function against actual SDL surfaces before fixes.
