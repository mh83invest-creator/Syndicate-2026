# WScreen aliasing and palette-call source trace
Date: 2026-11-01 (Australia/Brisbane). Branch: audit/graphics-assets-2026-11-01.
Status: original assembly source audit; runtime pointer values and visual effects untested.

## New source evidence
- syndre.sx:27780-27794: mission palette file is read into GraphicsPalette; WScreen is then cleared via LbScreenSurfaceClear and passed directly to ac_LbPaletteSet. The code does not load palette bytes into WScreen in the inspected sequence.
- syndre.sx:27868-27880: when DrawFlags indicates a particular mode, the source derives UScreen = WScreen + 0x1f408 and BScreen = WScreen + 0x1f418, while VScreen derives from VGABuffer + 0x6400. These offsets demonstrate buffer relationships in that branch, not an RGB palette alias.
- syndre.sx:56469-56506: a rendering routine temporarily substitutes BScreen for WScreen, calls LbSpriteDrawMCGA and restores the original WScreen. WScreen is demonstrably a mutable rendering-target pointer in this path.
- Previous direct inspection of SDL1/SDL2 spalette.c establishes that LbPaletteSet interprets its input as 256 RGB triples, with six-bit source channels.
- Therefore the WScreen argument differs in type/role from GraphicsPalette. Nothing inspected proves a WScreen/GraphicsPalette alias, and no real runtime pointer values were checked.

## Interpretation
This elevates the discrepancy from a mere naming oddity to a definite source-level role mismatch: a rendering buffer pointer is passed to a routine expecting palette triples. It is NOT yet a verified runtime error; the buffer may contain meaningful contents at call time in modes not traced, or original-compatible behaviour may be intentional. The apparent zeroing immediately before palette-setting makes an all-black initial display a plausible hypothesis, not a measured result. Do not patch or 'fix' this without Windows reproduction and before/after screen evidence.

## Verification ledger
PASS: full assembly blob read, mission-load and screen-pointer arithmetic, temporary rendering pointer substitution, SDL palette setter implementation comparison.
NOT RUN: live pointer tracing, compiled game, Windows screenshots, original asset palette decode or actual visual testing.
Changes: documentation only; protected baseline, main, other workstreams and retail assets untouched.
Next: track exact WScreen allocation and mission-load phase transitions; test palette RGB values and screen appearance on native Windows.
