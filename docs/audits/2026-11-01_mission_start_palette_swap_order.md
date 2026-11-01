# Syndicate 2026 — Mission Startup Palette and Swap Ordering

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Status: focused original assembly / SDL presentation source audit. Runtime visual behaviour and timing not tested.

## Direct source trace

1. Mission setup in `engine/src/syndre.sx:27747-27758` calls `ac_AppScreenSetup(0x12)`, then clears the display (non-DOS uses `ac_LbScreenClear(0)`). Single-player palette filename is chosen via `level_palettes[current_levno]`; multiplayer chooses `aDataHpal01Dat` (`27759-27779`).
2. `syndre.sx:27780-27793` loads palette data into `GraphicsPalette`, calls `LbScreenSurfaceClear(WScreen,0)` and installs `ac_LbPaletteSet(WScreen)`. As already established in `docs/audits/2026-11-01_palette_blackout_restore_source_trace.md`, the original `LbScreenSurfaceClear` at `75307-75340` zeroes 0x1f400 = 128,000 bytes only if `DrawFlags & 2` is set. Thus the following palette call receives an all-zero 768-byte palette prefix in that mode, assuming successful calls.
3. `syndre.sx:199-215` initialises a mission-start register counter `ebx=0` before the active-game loop. At `216-243`, each iteration processes the game state, calls `ac_LbScreenWaitVbi`, `nullsub_1` and `swap_screen_vres16`.
4. `syndre.sx:244-249` restores `GraphicsPalette` **after** `swap_screen_vres16` in the iteration for which `ebx==2`. `syndre.sx:274-279` increments this counter after subsequent scroll, `copy_back`, `draw_mapwho`, `draw_panel` and `click_map` work, until it reaches three. Hence, if three active-loop iterations proceed normally, **the first three swap calls precede the normal palette restoration**. The third iteration restores the palette immediately after its swap call. It does not call a gradual fade routine here. A colour update may have a visible effect before a subsequent swap depending on SDL surface/display behaviour: this report does not assert a count of visually black frames or real-time timing.
5. `syndre.sx:17894-17974` handles block-change/cursor preparation and calls one of two swap routes:
   - If `data_60b4f` is zero, non-DOS builds call `ac_update_vscreen_whole_vres16` then `ac_swap_wscreen` (`17941-17952`).
   - If nonzero, it clears that flag and calls `LbScreenSwapVres16` (`17941-17946`). `syndre.sx:76138-76166` defines that routine: with `DrawFlags & 2`, it performs the conversion update and ultimately calls `ac_swap_wscreen` in non-DOS builds. With bit 4 it calls `nullsub_6`; the exact implications for alternate display modes require deeper tracing.
   - In either branch this swap wrapper is invoked **before** the `ebx==2` palette-restoration check in the active-game loop.
6. Actual C implementation: `engine/src/display.c:244-261` `swap_wscreen()` locks, chooses between `blit_wscreen_vres16(VGABuffer)` (copy 640x480 bytes) and `blit_wscreen_mcga(WScreen)` (copy 320x200 bytes), invokes `LbScreenSwap`, and restores lock state. `display.c:234-242` performs these copies into `lbDisplay.WScreen`. This game `WScreen` is a separately loader-allocated pointer, as documented in `docs/audits/2026-11-01_wscreen_allocation_palette_phase_bounds.md`.

## Interpretations / constraints

- The high-resolution path is consistent with *intentional palette blackout followed by a discrete restoration* after initial mission loop work. No evidence from this sequence alone of an interpolated fade, a broken palette, or need for repair.
- Other graphics modes do not run the same zeroing code inside `LbScreenSurfaceClear`. Do not presume MCGA has this exact blackout behaviour, nor claim MCGA always passes through the identical high-res conversion path.
- The palette update is after the third swap call, but a palette change on an existing indexed SDL surface may itself affect visible colours. Three swap calls does **not** prove three visually black frames.
- Screen swap also involves cursor placement/restoration; any planned night-lighting hook must preserve composition order, HUD and original palette restoration instead of treating initial black palette as a lighting system.
- `data_60b4f` is written in another assembly path (`syndre.sx:17797`) and read by `copy_back` (`18916`). This stage traced its effect on swap dispatch, not all conditions that change the flag.

## Verification ledger

**PASS:** Original source blob reviewed at cited lines; SDL/port display C source inspected; source ordering and counter transitions traced; prior graphics reports cross-checked.

**NOT RUN:** Native build, gameplay, original graphics resource decode, screenshots, SDL palette-update timing tests, pixel/pitch checking, crash or performance testing.

**FILES CHANGED:** This graphics audit report and its graphics continuity index only; no engine, workflow, game assets, main, Mission Builder or other development branches.

## Next

Trace the graphics-mode transitions that use DrawFlags bit 4 / `data_60b4f` and test palette application and screen composition in an isolated Windows build before authorising renderer alterations.
