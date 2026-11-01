# Syndicate 2026 — Source-derived scaler AddressSanitizer test (Linux LP64)

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: isolated graphics test and audit documentation only. No game source changes.

## Test provenance and method
- Original reference: `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924-971`, `LbI_SDL_BlitScaled_totcbpp`.
- Transcribed the routine's calculations, iteration, palette lookup and **unaltered** `*((long *)(dst_buf+dst_offset))` pixel store into a temporary standalone C harness at `/mnt/data/synd_scaler_audit/test.c`, adding only minimal synthetic `ubyte` / `SDL_Color` declarations and a `main` test driver. The original function was **not compiled directly as part of the engine**, so this is source-derived, not an integrated SDL test.
- Synthetic indexed image: 2x2 pixels, palette initialized for four indices; scaled to a 3x3 destination; tightly allocated `3*3*bytes_per_pixel` destination buffer; destination BPP cases 2, 3 and 4.
- Host compiler: clang 17.0.0, Linux x86_64. Compile flags: `-O0 -g -fsanitize=address -fno-omit-frame-pointer`. Run with `ASAN_OPTIONS=detect_leaks=0`.

## Observed results
| Destination bytes/pixel | Exit | AddressSanitizer |
|---|---:|---|
| 2 | 1 | `heap-buffer-overflow`, **WRITE of size 8** |
| 3 | 1 | `heap-buffer-overflow`, **WRITE of size 8** |
| 4 | 1 | `heap-buffer-overflow`, **WRITE of size 8** |

The host follows an LP64 ABI with 8-byte `long`, unlike typical 32-bit Windows with 4-byte `long`. That is why the 4-byte destination case also fails in this run. The preceding **synthetic 4-byte store model** tested Windows-sized writes and found BPP=2 and 3 overflow, BPP=4 passed; that earlier result does NOT establish a native 32-bit Windows source test.

An attempt to compile the source-derived harness with `clang -m32 -O0 -g -fsanitize=address` **failed before build** because `bits/libc-header-start.h` was unavailable from this environment's 32-bit development headers. No 32-bit binary or Windows executable was produced.

## Interpretation
Direct source-derived execution confirms the scaler's `long`-sized store can overrun tightly packed output for the host ABI. The C function was called without SDL allocation, row padding, window modes, generated config flags or actual application control flow. Therefore **not a reproduced Syndicate Windows runtime crash**, and cannot justify a gameplay patch without build and integration tests.

## Next verification
Use a configured 32-bit Windows/MinGW or 32-bit sanitizer-capable environment; compile and test the exact scaler with real SDL surfaces/pitch for scaled indexed 8-to-16/24/32-bit cases, and inspect active Windows build workflow, surface formats and scaling reachability. Preserve all gameplay baselines and other branches.

## Verification ledger
PASS: isolated Clang build and three actual ASan executions on LP64, verified 8-byte writes and exit 1, documented missing 32-bit headers.
NOT RUN: actual engine/SDL integrated build, 32-bit native execution, Windows gameplay or screenshots, original copyrighted assets.
GitHub changed: report plus graphics continuity index only; temporary C harness exists only in container and is not committed.
