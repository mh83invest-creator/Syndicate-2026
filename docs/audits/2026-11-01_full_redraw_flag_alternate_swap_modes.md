# Syndicate 2026 — Full-Redraw Flag and Alternate-Mode Screen-Swap Audit

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Status: focused assembly and C source review complete; original resource data / Windows runtime not exercised.
Protected gameplay baseline: `main` v0.7; **documentation only**.

## Evidence and exact semantics

Source: `engine/src/syndre.sx` original assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` and `engine/src/main.c`, `engine/src/display.c`, `engine/src/display.h` on this graphics audit branch.

### The map-change grid and full-redraw flag
- `syndre.sx:85204-85205` reserves `0x190 = 400` bytes for `data_5db2c`, corresponding to `25 × 16` indexed map cells.
- `set_all_changes` (`17774-17799`) loops over all 25 × 16 cells, sets `data_5db2c[(column << 4) + row]` to `1`, then sets `data_60b4f=1`. Thus this byte acts as a **pending full-screen/map update signal** in this original render flow, rather than a graphics-mode flag.
- `set_block_changes` (`17803-17890`) can write `3` to cells in a clipped rectangle without the same `data_60b4f` assignment shown in that routine. The change-byte values should not be conflated with final pixel colours or full sprite flags.
- `set_all_changes` is called from full/repositioned map redraw and scroll paths (`19344, 19520, 19559, 19675, 19787, 19864`) and another drawing path (`28207`). These are actual source call sites, not an exhaustive call graph.

### Swap dispatch and same-iteration copy-back
- `swap_screen_vres16` (`17894-17974`) performs cursor/block-change preparation. If `data_60b4f != 0`, it sets the flag to zero and calls `LbScreenSwapVres16` (`17941-17946`); otherwise, non-DOS builds directly call `ac_update_vscreen_whole_vres16` then `ac_swap_wscreen` (`17947-17952`).
- `LbScreenSwapVres16` (`76138-76166`) tests `DrawFlags & 2` and in that case calls `ac_update_vscreen_whole_vres16`; if bit 2 is absent and bit 4 set, calls `nullsub_6` (`76150-76153`). `nullsub_6` is **only `ret`** at `76216-76218`. Non-DOS then calls `ac_swap_wscreen` (`76157-76160`).
- `copy_back` (`18910-19032`) tests the **same** `data_60b4f`. When nonzero it takes a set of `LbCopyScreenBox` calls (`18916-18999`); otherwise it scans the 25×16 map-change grid, copying nonzero-marked cells through `copy_screen_cube` (`19000-19026`). The inspected `copy_back` function does not itself clear `data_60b4f`.
- Crucially, the main loop (`syndre.sx:241-270`) calls `swap_screen_vres16` **before** calling `scroll_map` (twice), then `copy_back`. A scroll/redraw can therefore set `data_60b4f=1` **after this iteration's swap**. `copy_back` can observe the newly set flag in the same iteration; the *following iteration's* swap can consume it. It would be incorrect to assume the `copy_back` nonzero branch is unreachable simply because swap clears the flag.
- This is a source-order interpretation. There is no runtime trace proving every mode follows every branch, nor evidence justifying simplification of dirty-map flags in a new renderer.

### DrawFlags, alternate mode and actual presentation
- `engine/src/display.h:30-34` identifies `DrwF_ScreenMCGA=0x1`, `DrwF_ScreenVres16=0x2`, and a deliberately **unknown-named** `DrwF_Unkn04=0x4`.
- `engine/src/main.c:266-273` sets the default `DrawFlags=DrwF_ScreenVres16`; its command option `-H` chooses `0x2` and `-L` chooses `0x4` (`main.c:210-216`). Mode 0x4 should not be silently renamed as equivalent to 0x1.
- `engine/src/main.c:311-319` selects `load_files_vres16` for bit 0x2 or `load_files_mcga` for bit 0x4. This confirms a distinct low-resolution resource load path but does not establish complete runtime feature parity.
- `engine/src/display.c:244-261` `swap_wscreen` copies 640×480 bytes from `VGABuffer` into `lbDisplay.WScreen` if bit 0x2 is set; otherwise copies 320×200 bytes from exported game `WScreen`; then calls `LbScreenSwap()`. These are different backing buffers and copy sizes. Do not confuse `lbDisplay.WScreen` with exported game `WScreen`.
- Other display phases can explicitly save/replace/restore `DrawFlags`, e.g., `syndre.sx:54239-54251` temporarily sets `0x1` for a menu phase and `54849-54850` restores `OldDrawFlags`; `55019-55023` also stores and changes DrawFlags during another transition. That makes a single global copy of mode state unsuitable for an overlay implementation.

## Graphics audit consequence
Rendering optimisations and any future lighting overlay must preserve both the full-update signal and 25×16 dirty-cell handling, because the scroll/copy-back and swap stages operate on different sides of the frame loop. Must support actual game buffer ownership, per-mode screen resolution, pointer lifecycle, cursor placement and palette restoration. **This investigation authorises no engine changes.**

## Verification ledger
- **PASS:** full original assembly scanned for all textual occurrences of `data_60b4f` (5 including declaration), `set_all_changes` call sites, exact cell writes, swap dispatch, `nullsub_6`, `copy_back`, DrawFlags writes, and original main-loop order. `main.c`, `display.c`, `display.h` read directly from GitHub. Grid size 0x190=400 computed and cross-checked with 25×16 loop limits.
- **NOT RUN:** compiled Windows build, gameplay, actual incremental/full redraw capture, pixel/palette inspection, timing/performance profiling, low-resolution path testing, private original asset decoding.
- **CHANGES:** only this report and its graphics continuity index on `audit/graphics-assets-2026-11-01`; protected baseline, parallel core and Mission Builder branches, CI and assets unchanged.

## Next focused trace
Inspect where the dirty-cell values are cleared/consumed inside `copy_screen_cube` and related block redraw functions; establish the correct zero/nonzero lifecycle and what may be invalidated by future local lighting in the map area. No edits until source and runtime gates are satisfied.
