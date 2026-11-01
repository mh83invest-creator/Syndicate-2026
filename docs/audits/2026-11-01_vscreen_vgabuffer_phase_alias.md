# Syndicate 2026 — VScreen / VGABuffer phase-specific alias trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Type: static original assembly and current C source audit only.

## Source references
- Original `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines), direct exact-symbol scan of `VScreen`.
- `engine/src/display.c:149-260` on the graphics branch, previously verified.

## Verified assignments
1. `syndre.sx:27868-27879`: when the inspected `DrawFlags` comparison selects the high-resolution mission branch, original code loads `VGABuffer`, adds `0x6400` (=25,600 bytes), and assigns that result to `VScreen`. It separately sets `UScreen=WScreen+0x1f408` and `BScreen=WScreen+0x1f418`.
2. `syndre.sx:54253-54261`: an independent display/title-related initialisation path loads `VGABuffer` into `ebp` and assigns its base directly to `VScreen`, while other scratch buffers are derived from original `WScreen`.
3. An exact occurrence scan of the original assembly `VScreen` symbol found these two direct stores (lines 27875 and 54260); remaining references are reads and its export declaration. This is NOT proof that no linked C code or indirect alias changes the pointer.
4. `display.c:149-232` converts the original planar `WScreen` to `VScreen`; `display.c:244-255` selects `VGABuffer` as the source for the 640x480 high-resolution blit to `lbDisplay.WScreen` and then calls `LbScreenSwap`.
5. `display.c:127-130` reserves `640*480*2+16` bytes for `VGABuffer`. An initial `VScreen` pointer at +25,600 is within that allocation; this proves only the pointer offset is in the allocation, not that every subsequent draw/copy is safe.

## Interpretation
`VScreen` is sourced from `VGABuffer` in both directly observed assignment paths, but with **phase-dependent starting offsets** (0 and +0x6400). Therefore the prior open alias question is partially resolved; do not claim `VScreen == VGABuffer` globally. Further trace is required to establish active buffer layout, window/panel offsets, other pointer writers, display phases and final SDL surface update. The byte-array contents, screen alignment and visible behaviour are not established by pointer arithmetic alone.

## Lighting integration gate
No night-lighting patch is authorised. An eventual implementation must verify the exact presentation origin, clipping, scroll sequence, palette/indexed colour application, sprite and effect layers and MCGA alternative. Do not change original play mechanics or sibling audit branches; never commit proprietary retail assets.

## Verification ledger
PASS: fetched original complete assembly blob; exact VScreen symbol occurrence scan and line-specific assignments; linked C display reading; 0x6400 arithmetic.
NOT RUN: native compilation, pointer instrumentation, SDL backend execution, screenshots, retail archive decoding or performance tests.
NEXT: trace the C `LbScreenSwap` implementation and SDL1/SDL2 surface/palette copy path; verify effect of VScreen +0x6400 in live Windows session before modifications.
