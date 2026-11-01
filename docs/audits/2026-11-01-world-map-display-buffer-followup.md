# Syndicate 2026 — World-map row-spill and display-buffer follow-up

Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT ONLY; no gameplay, build or engine changes.
Branch: main; baseline reviewed at 3e98ba9f7f4a052e1ea2ab3374a01644c7b5529e.
Cross-workstream: docs/SHARED_DEVELOPMENT_HANDOVER.md and feature/mission-builder-audit.

## Scope and evidence
Continues docs/audits/2026-11-01-world-map-destination-boundaries.md and docs/audits/2026-11-01-world-map-territory-layout.md.
New source inspected: engine/src/display.c and engine/src/display.h at main, using the GitHub file API.

## Source-verified display observations
1. display_create_vga_buffer() allocates `640*480*2 + 16` bytes (614416 bytes) into VGABuffer. AppScreenSetup() assigns `lbDisplay.WScreen = VGABuffer + 640*480` before LbScreenSetup(); these are distinct from the global `WScreen` and `VScreen` pointers named in display.h. Do not infer the assembly world-map destination is lbDisplay.WScreen from the similar names alone.
2. AppScreenSetup maps mode 19 to 320x200x8 and mode 18 to 640x480x8. blit_wscreen_mcga() copies 320*200 (64000) bytes from WScreen to lbDisplay.WScreen; blit_wscreen_vres16() copies 640*480 (307200) bytes from VGABuffer. swap_wscreen chooses by DrawFlags. The mode and buffer used during the relevant map draw must still be established.
3. Prior audit derives an effective map-draw destination stride of 320 bytes and a 64x44 rectangular write. Max logical right edge column 334 was reported for indices 14, 44 and 45, exceeding the 320-column nominal row width. A contiguous 320-byte-stride linear buffer can convert that into writes to the following row, not necessarily outside allocation.
4. At the prior reported maximum bottom edge of row 184, an additional 15-column horizontal spill would land within nominal row 185 and well inside a 200-row contiguous 320x200 buffer, IF the target is a full 64000-byte buffer from its beginning. That conditional arithmetic does not establish real pointer bounds, clipping correctness or image correctness. It does narrow the suspected issue from proven memory overrun to possible scanline wrapping / visual corruption pending actual destination pointer provenance.

## Audit judgement
- No memory-safety failure or visual defect has been reproduced.
- Do not apply a clipping patch or change map coordinates based on the above arithmetic. Original visuals and current v0.7 must remain unchanged until source-pointer tracing and a Windows before/after test.
- For Mission Builder/custom map support, avoid assuming padding, row clipping or resource capacity merely because 320*200 nominal screen size is available. Map data and destination dimensions require separate bounds contracts.

## Remaining gates
1. Trace actual _draw_map_area destination register to WScreen, VScreen, VGABuffer or other allocation, including screen offsets and current DrawFlags/mode.
2. Confirm pointer lifetime and per-mode pitch in the map_selection path and LbScreenSetup; distinguish staging from display surfaces.
3. Verify highest right/bottom extents against the actual destination allocation and image placement.
4. With legally owned retail assets, perform Windows native build plus visual checks on territories 14, 44 and 45, and memory instrumentation if available.
5. Keep custom-mode save namespaces and all existing classic save files unchanged; coordinate with Mission Builder before modifying shared rendering interfaces.

## Verification / handover
Performed: inspected two additional source files; reconciled buffer allocation, copy lengths and mode mapping with previously documented world-map rectangle arithmetic. No source modification, build, test run, game launch, retail-asset inspection or runtime pointer observation. Rollback: no executable changes; v0.7 remains the protected playable baseline.
