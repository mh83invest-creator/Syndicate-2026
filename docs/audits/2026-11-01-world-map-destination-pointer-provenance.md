# Syndicate 2026 — World-map destination pointer provenance

Date: 2026-11-01 (Australia/Brisbane)
Audit status: static assembly inspection; no implementation, compilation or runtime tests.
Repository: mh83invest-creator/Syndicate-2026, main.
Prior audit: docs/audits/2026-11-01-world-map-display-buffer-followup.md.
Assembly inspected: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Exact drawing call-site provenance
Four literal call sites to _draw_map_area were inspected:
- syndre.sx line 31779: third argument pushed from BScreen at line 31769 (map_selection).
- line 32268: third argument pushed from BScreen at line 32258 (map-click / update branch).
- line 32748: third argument pushed from EXPORT_SYMBOL(WScreen) at line 32738.
- line 32792: third argument pushed from EXPORT_SYMBOL(WScreen) at line 32783.
The _draw_map_area routine receives its destination pointer from stack argument at line 32839, adds computed territory coordinates and uses it for the row-major writes (32839–32863). Thus there are distinct observed destination pointer pathways and the buffer for a given call should not be guessed from the routine name.

## BScreen provenance across modes
Three assembly assignments to BScreen were inspected:
- line 54253–54255: BScreen = WScreen + 0xFA00 (64000 bytes), in a menu-screen setup path. That branch also explicitly selects DrawFlags=1 and AppScreenSetup(0x13) in some conditions at 54245–54251; exact condition coverage of map_selection remains a separate trace.
- line 55008–55012: BScreen = WScreen + 0x1F400 (128000 bytes), in level_finished.
- line 27870–27879: in a DrawFlags==2 route, BScreen = WScreen + 0x1F418 (128024 bytes), while VScreen = VGABuffer + 0x6400. This is a distinct setup path and should not be conflated with the map menu.
BScreen is a mutable assembly pointer, not a fixed alias for the native C lbDisplay.WScreen screen surface. The inspected assignments do not establish which pointer value is live at each draw call.

## Boundary implications
Prior audit established 320-byte destination stride, 64 columns × 44 rows, and possible nominal horizontal row overhang for territory indices 14, 44, 45. Because the draw target is caller-provided and BScreen can point inside WScreen with variable offset, earlier conditional bounds reasoning using a standalone 64000-byte allocation is insufficient. Each map/menu draw path must be tied to an actual live allocation, destination offset, screen mode and pointer lifetime.
The source currently does not prove out-of-allocation writes or reproduce visible corruption. No change to original drawing is approved.

## Next actions
1. Trace the allocation and live size of exported WScreen, including initialization and teardown; distinguish native C display pointers from assembly BScreen/UScreen.
2. Establish whether both BScreen-using map call sites execute after the 0xFA00 assignment and before any alternative reassignment.
3. For each _draw_map_area caller calculate maximum last-address offset using its actual base pointer and coordinate conventions, then cross-check allocation.
4. Run legally owned retail asset / Windows 32-bit visual and memory checks before any engine patch. Retain 16 FPS mission loop and v0.7 fallback.
5. Keep Mission Builder branch and campaign save namespace contract synchronized; this audit changes no interfaces.

## Verification and handover
Performed: read live GitHub assembly blob and decoded exact stack argument origins, three BScreen assignments and _draw_map_area destination-register usage. This is a reproducible static-source finding. Not performed: compilation, runtime mode validation, buffer-size test, launch, screen capture, asset scan or save modification. No engine files changed; docs-only handover.
