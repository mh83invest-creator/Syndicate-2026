# Syndicate 2026 graphics audit — dirty-cube copy source trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: source-only continuation of 25×16 dirty grid / screen-copy analysis. No implementation changes.

## Source reviewed
Original assembly `engine/src/syndre.sx`, blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines).

### Verified facts
- `copy_back` (`syndre.sx:18910-19032`) checks `data_60b4f`. In its no-full-redraw branch, it scans 25 rows × 16 columns, checks the corresponding byte in `data_5db2c` for nonzero and calls `copy_screen_cube(x,y)` for marked cells (19000-19026).
- `copy_screen_cube` (75476-75660) executes the byte-copy body only when `DrawFlags & 0x2` is set. Its other tested modes fall through to an immediate return; do not assume equivalent copying in MCGA or CLI `-L` mode.
- In the active Vres16 branch the source pixel-address calculation uses `BScreen`, wrapped destination tile coordinates, a `0x500` byte row stride and four separate plane offsets. The destination is exported original `WScreen`, not automatically `lbDisplay.WScreen`. The fixed assignment pattern copies 16 rows of four-byte groups in each of four planes, for 256 byte assignments per invocation (64 per plane). This is arithmetic over inspected move instructions, not a runtime throughput measurement.
- `copy_screen_cube` does NOT itself write `data_5db2c`, nor does the inspected `copy_back` loop clear the tested marker following the call. Accordingly, a nonzero dirty marker is not proven single-use by these routines. A broader lifecycle trace is still necessary.
- In original DOS-specific swapping code (`syndre.sx:17953-17968`) `func_4c797` receives rows of dirty markers; this code is under `#else` for DOS. `func_4c797` inspects positive signed marker bytes when copying to `VScreen` (75664-75888), and does not visibly reset those markers. The non-DOS path instead calls `ac_update_vscreen_whole_vres16` and `ac_swap_wscreen`. Do not claim DOS dirty-screen swapping is directly active in the current SDL/Windows build.

## Audit interpretation and constraints
- The grid is an input to conditional background restoration/copying, not evidence that new lighting effects may safely use it without further review.
- A dirty value of 0x03 written by `set_block_changes` should not be interpreted as a visual colour, alpha channel or bitfield until all readers/writers are traced.
- Before any night lighting overlay, identify **all** writers/clearers of `data_5db2c`, determine where the actual SDL frame is assembled and presented, and test stationary light, scrolling, agents and explosions against redraw persistence.
- Preserve the 16 FPS tactical loop, existing mouse/keyboard play, v0.7 baseline, campaign save isolation and between-mission-only saves. Coordinate proposals with core audit and Mission Builder. No authorised engine patch in this audit.

## Verification
PASS: direct GitHub original assembly blob read and line-specific control flow review; counted four screen planes × 16 row copies × four bytes.
NOT RUN: compile, Windows runtime test, graphics equivalence screenshots, performance profiling, private original-asset decoding.
CHANGES: this documentation report and the graphics continuity index only; no copyrighted retail asset committed.
