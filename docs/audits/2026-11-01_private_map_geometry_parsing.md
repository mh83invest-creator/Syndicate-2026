# Syndicate 2026 — Private Map Geometry Parsing (Subset Verified)

Date: 2026-11-01 (Australia/Brisbane)
Status: completed a focused offline map-structure validation for 48 successfully decoded original maps; 11 remain undecoded in independent test harness.
Branch: feature/v0.9-visual-showcase; protected baseline main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Private input: existing user-uploaded Syndicate_2026_v0.6_Two_Menus_30FPS_Test(1).zip, never published to GitHub.

## Method
Enumerated original data/map*.dat members in ZIP, passed RNC1 resources through an independent temporary Python decoder accepting results only when decoded length and *unpacked* CRC16 matched. Parsed three little-endian 32-bit map-header fields at offsets 0,4,8; inspected 12,288 little-endian 32-bit relative column offsets starting at byte 12. Verified every offset is 4-byte aligned and each absolute position (12 + relative offset) is inside the decoded file. No proprietary map bytes are reproduced here.

## Results
- 59 map*.dat members examined.
- 48 maps decoded with matching unpacked CRC and passed all header and 12,288-column-pointer structural checks.
- All 48 accepted maps had header fields (128,96,12). The first two multiply to 12,288, confirming the earlier original-assembly fixed pointer count for these samples. Do not infer what the third field signifies without further format evidence.
- Every decoded pointer in those 48 maps was 4-byte aligned and the base-plus-offset target was inside the corresponding decoded map file.
- The 11 other map files failed the **independent trial decoder** (input exhausted, copy bounds or CRC mismatch); their original archive RNC1 compressed CRCs had already passed. They are not certified corrupt.
- Examples of successfully decoded maps: map01.dat decoded size 52,632 bytes, 289 distinct column pointer offsets; map02.dat decoded size 64,476, 1,276 distinct offsets; map09.dat decoded size 49,188, 2 offsets; map15.dat decoded size 49,176, 1 offset. This illustrates real variation in underlying map data without exposing the original map content.

## Consequences for lighting
The map format has an established column-pointer table that can be parsed for verified maps. It does not yet reveal which column/block IDs represent illuminated building windows, streetlights, roads or neon. The existence of pointer offsets is NOT a lighting object model or a complete occlusion solution. Do not implement per-block or per-object glow until the referenced block records, projection and original renderer lookup have been inspected.

## Verification ledger
PASS: 48 decoded map lengths and unpacked CRCs, header fields, all 12,288 pointers per successful map checked for alignment and file bounds.
NOT VERIFIED: remaining 11 maps, light-source classification, block record semantics, camera projection from map position, mission render output.
NOT RUN: original native C decoder, Windows build, actual gameplay, spotlight/neon visuals, performance profiling.

## Next process
Validate the independent decoder against the game C implementation or fix its divergences, then decode all 59 maps and follow offset targets into block/height data and original assembly map drawing. Keep assets private, protect source baseline and document any failures separately. No engine, workflow or public game-data changes.
