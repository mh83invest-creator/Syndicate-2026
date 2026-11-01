# Graphics audit — original mission palette loading path

Date: 2026-11-01 (Brisbane)
Branch: audit/graphics-assets-2026-11-01
Source: engine/src/syndre.sx, Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified source observations
- syndre.sx:82677-82684 defines level_palettes, with ten consecutive groups of palette identifiers 1,2,3,4,5, followed by two zero bytes (50 assigned entries plus two trailing zeros). It is a level-indexed identifier table, not actual RGB palette data.
- syndre.sx:27768-27778 indexes level_palettes using current_levno and formats the resource filename using aDataHpal02dDat. A separate earlier branch reaches the same file-load step, so the level table is not claimed as the only possible filename path.
- syndre.sx:27779-27785 passes GraphicsPalette to LbFileReadRNC as the destination buffer for the selected palette file.
- syndre.sx:27786-27794 clears WScreen, then calls ac_LbPaletteSet with WScreen as its argument in this call sequence. That unusual argument should be traced against the actual function implementation; do not assert that WScreen itself contains the loaded 768-byte RGB palette solely from this call site.
- Previous source inspection established player_colour_for_map as a ten-byte indexed-colour lookup. Its bytes cannot be converted to RGB without the correct active palette.

## Consequences
Palette IDs are reused across multiple missions. Replacing the shared hpal resource to create mission-specific darkness would affect every mission referencing that palette unless the routing changes. This report does not authorise a lighting change.
The source route establishes where palette data is loaded, but does not establish whether and when palette updates occur after load (effects, fades or flashes). Do not infer screenshots or night lighting from static palette IDs.

## Verification
PASS: direct original assembly blob inspection at cited ranges and static-table arithmetic.
NOT RUN: private original hpal resource decoding/RGB sampling, ac_LbPaletteSet implementation trace, Windows gameplay, compilation, light/effect benchmarks.
Changes: documentation only in graphics audit branch. Other branches, gameplay and assets untouched.
Next: locate ac_LbPaletteSet implementation and runtime palette effects; map indexed colours against private original palette bytes when available.
