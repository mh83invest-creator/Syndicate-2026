# Syndicate 2026 graphics audit — original assembly sprite descriptors

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Stage: original assembly source trace complete for sprite setup and selected animation drawing call sites. Private asset record validation and Windows test not performed.
Source: engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf (85,803 lines).

## Resolved access issue
Earlier ranged fetch_file requests for the very large assembly file returned empty content. GitHub fetch_blob using the exact repository blob SHA returned the full original assembly text. This stage directly inspected it and restores source inspection access. Prior reports should not be read as proving that the assembly was unavailable.

## Verified source evidence
- syndre.sx:82641-82648 identifies distinct resources data/hspr-0.dat (h_sprites_data) and data/hspr-0.tab (h_sprites / h_sprites_end).
- syndre.sx:27723-27730: init_level_data passes h_sprites, h_sprites_end and h_sprites_data to ac_LbSpriteSetup.
- engine/bflibrary/src/general/gsprite.c:27-45: LbSpriteSetup traverses an entry range and adjusts TbSprite.Data offsets into pointers relative to a data allocation.
- syndre.sx:76222-76246: DrawASpriteHires16_A receives sprite descriptor at argument +0x10, loads a 32-bit data pointer at descriptor +0, and two one-byte dimensions from +4 and +5, then draws to WScreen using DrawSpriteLores16.
- syndre.sx:76250-76274: DrawASpriteLores16_B uses the same descriptor field accesses but draws to UScreen.
- syndre.sx:17733-17752, 19069-19087: animation element begins with a two-byte offset into h_sprites, then has positioning values +2 and +4, a word at +6 passed as drawing argument, and a word at +8 used to step to subsequent animation elements.
- syndre.sx:19058-19065: animation element indexing multiplies an index by 10 (5x then doubled). This corroborates a 10-byte element structure, distinct from the sprite descriptor.
- syndre.sx:82650-82662: hfra-0.ani, hele-0.ani, hsta-0.ani are loaded into separate frame, element and start pointers.

## Derived constraints, not asset verification
- A packed six-byte sprite descriptor is strongly supported by direct draw field accesses (+0 pointer, +4/+5 dimensions) and LbSpriteSetup use.
- Prior original resource audit reported hspr-0.tab decompressed length = 10,812 bytes; 10,812 / 6 = 1,802. This is a mathematically consistent candidate descriptor count, NOT yet a verified count of valid sprite frames. Sentinel/unused entries, pointer bounds, actual dimensions and 32-bit layout must be checked against the decoded bytes.
- Animation elements use a distinct ten-byte record stride in inspected draw loops. A sprite reference is a byte offset, not yet proved to be a numeric sprite ID. Do not divide or convert blindly without checking alignment and bounds.
- These evidence paths explain why global colour remapping or screen overlays must be tested separately across actual buffer destinations.

## Next validation
1. Privately decode hspr-0.tab using the established CRC-checked original asset decoder. Parse candidate six-byte entries as little-endian offset32, width8, height8. Check pointer starts against decompressed hspr-0.dat length 355,356, dimensions, sentinel entries, and alignment.
2. Privately decode hele-0.ani and inspect 10-byte candidate element fields; verify that referenced two-byte sprite offsets align to six and remain below the table size, without assuming all animation records have identical semantics.
3. Trace frame/start animation fields from syndre.sx, then identify sprite pixel encoding in DrawSpriteLores16 and compare with private offline render samples.
4. Use native Windows game screens before declaring visual equivalence; record performance and draw order before any lighting implementation.

## Verification ledger
PASS: original GitHub assembly blob fetched and relevant lines inspected; earlier C sprite setup code corroborated; exact arithmetic for inferred table capacity checked.
NOT RUN: fresh original asset decode, descriptor byte parsing, visual sprite reconstruction, C/assembly compile, Windows tests, performance tests.
FILES CHANGED: this report and graphics continuity index only. No gameplay or retail files, workflow, protected main, Mission Builder or core audit branch modified.
