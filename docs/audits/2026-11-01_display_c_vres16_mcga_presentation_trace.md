# Syndicate 2026 — C display conversion and presentation boundary

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: source-only trace, not a visual test.
Source: `engine/src/display.c` on the graphics audit branch; original assembly `engine/src/syndre.sx` as documented in the preceding dirty-grid reports.

## Verified display.c findings
- Lines 149–232: `update_vscreen_whole_vres16` zeroes `640*480` bytes starting at `VScreen` and decodes four original planar streams from `WScreen + plane*400*80` into 8-bit indexed output. Each plane iteration processes 400 rows by 80 source bytes, expanding each input byte over eight output pixels. This yields 640×400 converted output pixels in a nominal 640×480 cleared target. Do not infer contents of the remaining display rows beyond zeroing without checking overlays and all writes.
- Lines 234–242: `blit_wscreen_vres16` copies `640*480` bytes from its buffer argument into `lbDisplay.WScreen`; `blit_wscreen_mcga` copies `320*200` bytes instead.
- Lines 244–260: `swap_wscreen` acquires a screen lock if necessary, selects the high-resolution blit of `VGABuffer` when `DrawFlags & DrwF_ScreenVres16`, otherwise blits original game `WScreen`; then calls `LbScreenUnlock` and `LbScreenSwap`, with lock restoration logic.
- Lines 72–88: `AppScreenSetup` maps original mode identifiers 18/19 to 640×480×8 and 320×200×8 and sets the *library* `lbDisplay.WScreen = VGABuffer + 640*480` before `LbScreenSetup`.
- Lines 127–136: `VGABuffer` is allocated at `640*480*2+16` bytes and later freed. It must not be confused with loader-managed original game `WScreen`.

## Unresolved alias and SDL boundary
The conversion writes to `VScreen`, whereas the high-resolution blit specifically copies from `VGABuffer`. These expressions alone do not establish whether `VScreen` aliases `VGABuffer`, how that pointer is set in each phase, or whether a separate staging copy occurs. The next source pass must trace `VScreen` assignment/aliasing, `LbScreenSetup`, `LbScreenSwap`, surface pitch, indexed-palette application, and SDL1/SDL2 backends. No bug is established from this distinction.

## Night lighting integration constraints
- Do not draw lights into a buffer whose content may subsequently be rebuilt or copied over without verifying ordering.
- Validate both high-resolution plane conversion and MCGA indexed blit, scrolling/dirty marker persistence, agent/effect layering, and palette colour changes before implementation.
- This graphics branch remains read-only audit/documentation. No original copyrighted art or gameplay modifications.

## Verification
PASS: GitHub `display.c` read and specific C loop/call inspection; byte-count arithmetic.
NOT RUN: build, Windows rendering, screenshots, runtime pointer instrumentation, palette validation, original asset parsing or performance tests.
NEXT: trace assignment/provenance of `VScreen` and its possible alias to `VGABuffer`, then inspect SDL screen-swap backend.
