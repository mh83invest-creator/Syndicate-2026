# Syndicate 2026 — Resource loader offset audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused static audit completed; full audit incomplete.
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40
Evidence: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Confirmed source observations
- Lines 83626–83650 define the high-resolution load descriptor. It includes a WScreen allocation of 256048 bytes and a separately specified high-resolution palette file for GraphicsPalette.
- Lines 83651–83674 define the low-resolution load descriptor and its own palette. Its precise buffer allocation was not decoded in this stage.
- Lines 31137–31142 and 31238–31253 dispatch to ac_LbDataLoadAll using the selected descriptor.
- Lines 31325–31330 invoke ac_LbDataFreeAll using the high-resolution descriptor; other cleanup cases require investigation.
- Mission assignments (27866–27880): UScreen = WScreen + 128008; BScreen = WScreen + 128024; VScreen = VGABuffer + 25600.
- Title/select assignments (54253–54261): BScreen = WScreen + 64000; PalMem = WScreen + 132096; VScreen = VGABuffer.
- Post-level assignments (55008–55023): BScreen = WScreen + 128000; PalMem = WScreen + 196096.

All the listed WScreen-derived starting offsets are less than 256048, but this does not prove subsequent writes fit, or that all phases retain the same allocated buffer. VGABuffer is allocated separately in display.c. The game changes scratch-pointer relationships between phases.

## Verification
PASS: Direct read of original assembly blob and arithmetic evaluation.
NOT RUN: build, automated tests, Windows gameplay, allocation-failure tests, real game data and graphics captures.
UNRESOLVED: exact MCGA allocation size, alternate cleanup paths, scratch-buffer write extents, loader failure handling.

## Next audit
Inspect library implementations of LbDataLoadAll and LbDataFreeAll and trace resource ownership/cleanup semantics. Continue without modifying engine source. Only this documentation may be committed to the feature branch.
