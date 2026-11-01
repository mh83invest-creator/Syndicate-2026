# Syndicate 2026 — Map and palette descriptor lifecycle
Date: 2026-11-01. Static assembly review, documentation only.

## Findings
1. Generic descriptor loader LbDataLoad (syndre.sx 14475 onward) uses the descriptor flag byte 0x2a to select preallocated memory entries. It reads the size at descriptor offset 0x24 and allocates it using LbMemoryAlloc, then zeroes that declared region (14581–14607).
2. The main menu's unk984_load_files list has a star-prefixed map_buf entry (85040–85045). Descriptor offset 0x24 contains A0 4C 01 00, little-endian 0x14CA0 = 85,152 bytes. That establishes the *declared* menu-map allocation size, not a verified runtime capacity or tactical-map bound.
3. The menu calls ac_LbDataLoadAll(unk984_load_files) at 54156–54163 and ac_LbDataFreeAll for the same list at 54838–54839. Thus the menu map buffer is explicitly allocated and freed in the menu lifecycle.
4. mission_load_files references map_buf in its named data/mapXX.dat entry (82620–82632); it is loaded through ac_LbDataLoadAll at 27901–27903 and freed via ac_LbDataFreeAll at 27642–27644. No explicit size is specified for the named map item; its buffer size is derived from LbFileLengthRnc in the generic loader branch. Runtime source file lengths and safe decompression capacity remain unverified.
5. Graphics modes select different descriptor arrays: load_files_vres16 or load_files_mcga through ac_LbDataLoadAll around 31137–31142 and 31239–31253. These lists contain different palette paths (hpal01.dat and lpal01.dat) each targeting GraphicsPalette (83634–83660). This establishes mode-specific resource selection but not a verified palette byte count.
6. The screen allocation of 256048 bytes in load_files_vres16 is a WScreen allocation, not the palette capacity. Other unnamed palette allocations require direct source or runtime examination.

## Outstanding work
Trace call ordering and retained pointers across menu-to-mission transitions; identify loader descriptors' actual runtime sizes, especially GraphicsPalette. Validate legitimate game resources before any bounds fix. Maintain independent save modes and the Mission Builder briefing safety gate.

## Verification
Source-level exact-line review and descriptor arithmetic only. Not run: Windows builds, game launch, allocation instrumentation or retail asset inspection. No source or save files modified; protected v0.7 baseline unchanged.
