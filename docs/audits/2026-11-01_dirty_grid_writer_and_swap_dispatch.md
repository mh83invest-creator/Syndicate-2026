# Syndicate 2026 — Dirty-grid writers and non-DOS swap dispatch

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Audit type: original assembly static source trace only.
Source: `engine/src/syndre.sx`, verified blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`.

## Exact grid write/read inventory in this assembly file
An exact-text scan for `data_5db2c` in the 85,803-line source returned five occurrences:
- 17791: `set_all_changes` writes 1 to indexed byte.
- 17878: `set_block_changes` writes 3 to indexed byte.
- 17954: DOS-only `swap_screen_vres16` supplies the grid base to `func_4c797`.
- 19012: `copy_back` tests an indexed byte for zero/nonzero.
- 85204: array storage declaration, `.fill 0x190` (400 bytes).
No explicit reference resetting the array was found in this file. This does NOT prove the grid cannot be cleared indirectly through aliases, memory fills or other linked sources.

## Coordinate / full-redraw control
- `set_all_changes`, lines 17774-17799, writes 1 to all 400 dirty bytes using loops of 25 and 16 and also sets `data_60b4f=1`.
- `set_block_changes`, lines 17803-17890, clips the affected bounds and writes marker value 3. It does not itself set the full-redraw flag in the inspected body.
- `copy_back`, lines 18910-19032, checks the full-redraw flag; if clear, visits nonzero dirty entries and calls `copy_screen_cube`. It does not visibly zero dirty entries.
- `swap_screen_vres16`, lines 17940-17968, tests `data_60b4f`; when set, clears that FLAG before `LbScreenSwapVres16`. Clearing the flag is **not the same thing as clearing grid entries**.

## Current non-DOS boundary
- In the no-full-redraw branch, the code under `#if !defined(DOS)` calls `ac_update_vscreen_whole_vres16` and `ac_swap_wscreen` (17947-17952). The DOS `#else` is the path that passes grid rows to `func_4c797`.
- `LbScreenSwapVres16` (76138-76166) calls `ac_update_vscreen_whole_vres16` only when `DrawFlags & 2`; for non-DOS modes it proceeds to `ac_swap_wscreen`. Its mode 4 branch only calls `nullsub_6`.
- These calls establish **dispatch/order**, not how SDL converts buffers to pixels or presents them. The linked C implementations must be inspected next.

## Conclusions and limits
Verified: direct writes set marker 1 or 3; nonzero checks are used by `copy_back`; original assembly does not show a direct indexed clear of the dirty byte array; full-redraw flag has a separate clear path; DOS dirty-row presentation differs from the non-DOS whole-screen calls.
Not established: complete dirty-grid lifetime across linked code, memory aliases, effective SDL screen presentation, visible redraw persistence, night-lighting safety, any functional defect.
Implementation permission: none. No retail graphics copied to GitHub.
Tests: static exact-occurrence scan, assembly control-flow inspection; NO compilation, Windows gameplay, screenshots, performance profiling or private archive validation.
Next: inspect `ac_update_vscreen_whole_vres16` and `ac_swap_wscreen` implementations in the linked C/library files, including buffer ownership and pixel format.
