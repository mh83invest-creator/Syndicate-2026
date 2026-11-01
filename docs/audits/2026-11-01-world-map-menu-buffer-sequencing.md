# Syndicate 2026 — World-map menu buffer sequencing follow-up
Date: 2026-11-01 (Australia/Brisbane)
Status: static source inspection only; no engine changes or runtime tests.
Repository: mh83invest-creator/Syndicate-2026, main.
Predecessor: docs/audits/2026-11-01-wscreen-loader-allocation-semantics.md.
Assembly source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Newly verified control and copying paths
1. In menu initialization at assembly lines 54239–54255, conditional values 2 or 3 of data_55590 select DrawFlags=1 and call ac_AppScreenSetup(0x13), but BScreen = WScreen + 0xFA00 (64000) occurs after that conditional and is thus unconditional for the inspected execution path through jump_37709. This does not prove all possible callers of map_selection first passed this initialization.
2. The same setup positions data_60ab0 at WScreen+128000 and PalMem at WScreen+132096 (54253–54261). They are pointer assignments, not independent allocations. A successful 256016/256048-byte synthetic WScreen allocation would accommodate those base offsets, subject to actual buffer use and lifetime.
3. map_selection at 31719–31727 calls ac_LbDataLoadAll(mmapblk_load_files), discards its return count and immediately calls display_cities. During its loop it calls LbCopyScreenFromBackMCGA at 31731. A missing world-map file may leave world_map invalid before drawing: this is a static failure-path concern, not a reproduced crash.
4. LbCopyScreenFromBackMCGA at 54861–54886 copies from BScreen to WScreen via a fixed loop. Its loop starts at 0x3E7F, subtracts three per iteration and copies three four-byte words; total transferred is (0x3E7F / 3) * 12 = 63996 bytes (0x3E7F = 15999, divisible by 3). The final four bytes of a nominal 64000-byte frame are not copied by this routine, subject to interpreting its loop exactly; whether intentionally reserved or visually consequential is untested. Do not silently change the copy count.
5. Prior audits found two map-draw call sites using BScreen (31779, 32268) and two using WScreen (32748, 32792). The known WScreen+64000 backing offset in the inspected menu setup makes those two distinct 64000-byte image regions. Thus map drawing into the BScreen image can stay within a 256016-byte allocation even where a rectangle crosses the *logical* 320-pixel row boundary. Pointer provenance still needs full calling-path confirmation for every draw.
6. Source assembly also contains alternate BScreen assignments at 55012 (WScreen+128000) and 27879 (WScreen+128024). Their temporal relationship to menu map drawing remains unproven; the above explanation applies conditionally to the menu setup path, not globally.

## Reproducibility and remaining gates
- Trace the specific caller of map_selection to prove whether menu setup 54253–54255 always precedes it, and whether any BScreen reassignment occurs in between.
- Establish how WScreen is populated in each graphics-mode branch, and verify allocation success/error handling.
- Independently calculate the *last written address* for each _draw_map_area call using all 50 territory coordinates, the destination pointer and 320-byte row stride, distinguishing scanline wrap from allocation overrun.
- Check whether the back-buffer copy intentionally transfers 63996 rather than 64000 bytes; confirm against runtime screen comparisons before changing any pixel copy.
- Retail assets, Windows 32-bit compilation, playable map tests and memory instrumentation are outstanding.

## Protected contract and verification
No source files, game binaries, saves or Mission Builder code modified. Campaign isolation contract remains design-only. Current v0.7 baseline retained. This audit is based on static assembly instruction tracing and arithmetic, not a gameplay or memory-safety result.
