# Syndicate 2026 — Synthetic AddressSanitizer validation of scaler write width

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: standalone, synthetic C harness in a temporary directory. **NOT a native Syndicate build or SDL execution.**

## Source-derived test target
In `engine/bflibrary/src/x86-win-sdl2/sscreen.c:962-965` and `engine/bflibrary/src/x86-win-sdl/sscreen.c:788-790`, the scaler stores a `long`-sized value at `dst_buf+dst_offset` and advances `dst_offset` by `dst_bpp`. On Windows i686, `long` is four bytes. Prior audit: `2026-11-01_scaled_truecolour_last_pixel_bounds.md`.

## Harness and method
Host used for this test: Linux x86_64 with Clang, not Windows. To specifically model the Windows four-byte store, a `uint32_t` value was copied using `memcpy` with size `sizeof(uint32_t)` at each destination index. This models the **byte extent and stride** of the source expression, but does not exercise its casts, alignment semantics, actual SDL surfaces or pixel-colour calculations.

Test code (synthetic only):
```c
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char**argv){
  int bpp=argc>1?atoi(argv[1]):2, count=5;
  size_t size=(size_t)count*bpp;
  unsigned char *dst=malloc(size);
  memset(dst,0,size);
  for(int i=0;i<count;i++){
    uint32_t pixel=0x12345678;
    memcpy(dst+i*bpp,&pixel,sizeof(pixel));
  }
  printf("done bpp=%d\n",bpp);
  free(dst);
}
```
Built using `clang -g -O0 -fsanitize=address -fno-omit-frame-pointer` and invoked separately with destination BPP 2, 3 and 4, using no copyrighted assets.

## Actual observations
- **BPP 2:** AddressSanitizer reported `heap-buffer-overflow`, `WRITE of size 4`.
- **BPP 3:** AddressSanitizer reported `heap-buffer-overflow`, `WRITE of size 4`.
- **BPP 4:** program printed `done bpp=4`, without AddressSanitizer fault.
- The original command piped output into `grep -m2`, so failing runs showed pipeline status 141 (SIGPIPE) rather than a reliable unpiped process exit code. Do not report 141 as the native sanitizer exit status.

## Conclusion
The **write-width/stride pattern** is demonstrably out of bounds for tightly packed 2- and 3-byte-per-pixel buffers. This is stronger than symbolic arithmetic alone, but **does not establish a reproduced defect in Syndicate 2026**: source scaler was not compiled or called, SDL dimensions/pitch/allocation padding were not tested, Windows compiler/configuration was not used, and exact backend activation is not confirmed.

## Next gate
Use an isolated harness that compiles the real scaler function (or an exact extracted copy with traceable provenance), run with ASan/guard regions and variable source/destination pitch, then inspect a real Windows 32-bit SDL build, 640x480/320x200 modes and surface formats. Do not modify baseline gameplay until validated and approved.

## Verification ledger
PASS: compiled and ran synthetic Clang/ASan harness, observed two overflow diagnostics and one non-failing 32-bit-pixel control.
NOT RUN: actual source scaler test, SDL, 32-bit Windows compilation, Windows gameplay, screenshots, original art.
CHANGED IN GITHUB: this report and graphics continuity index only; no executable/test harness source file committed.
