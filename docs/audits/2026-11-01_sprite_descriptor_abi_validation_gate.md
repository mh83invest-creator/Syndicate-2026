# Syndicate 2026 — Sprite Descriptor ABI and Private Asset Validation Gate
Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused header/assembly ABI review completed; original data-byte validation outstanding.

## Newly verified evidence
- Direct GitHub read of engine/bflibrary/include/bfsprite.h establishes a packed TbSprite definition: one TbSpriteData pointer followed by SWidth and SHeight; without SPRITE_FORMAT_V2 the dimensions are unsigned bytes, whereas V2 uses two ushort fields.
- Original assembly engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf at lines 76222-76239 and 76250-76267 reads the sprite pointer at +0 and byte dimensions at +4/+5. This is consistent with the non-V2 six-byte packed layout on the i386 engine path.
- Assembly lines 27723-27730 pass h_sprites, h_sprites_end, h_sprites_data to ac_LbSpriteSetup. Assembly lines 82641-82648 show the separate DAT data buffer and TAB descriptor resource.
- The previously verified offline hspr-0.tab size is 10,812 bytes: a six-byte stride would give 1,802 entries. This arithmetic is consistent with the source ABI, not a verified count of valid sprites or evidence that every six-byte chunk represents an active sprite.
- Original assembly references to h_sprites_end occur in the resource loader and setup, but the inspected draw sites compare selected pointers against h_sprites start, not necessarily against the end. Independent bounds checks are needed before relying on external sprite IDs.
- The separate animation element indexing in selected original draw paths uses a ten-byte stride and references the sprite table through a two-byte offset. It is not the same structure as TbSprite.

## Validation gate and limitation
No original retail v0.6 ZIP or decoded hspr-0.tab bytes are mounted in this execution environment. Conversation/Project file search and Library search did not locate a usable retail-data archive. Do not invent per-entry byte distributions, offset ranges, valid sprite count, dimensions, pixel frames, or animation-reference statistics.
A subsequent offline private test must: (1) decompress both TAB/DAT with header length and unpacked CRC checks; (2) parse TAB as 6-byte little-endian offset32, width8, height8; (3) distinguish unused entries and test all nonempty offsets against decompressed DAT size; (4) inspect animation sprite offsets for six-byte alignment and index bounds; (5) verify the compile-time SPRITE_FORMAT_V2 selection in the actual build before interpreting as six-byte records; (6) compare a sample of decoded images against original Windows renderer screenshots. Do not upload commercial files publicly.

## Test ledger
PASS: original full assembly blob and bfsprite.h read directly; C sprite setup behaviour cross-referenced with earlier graphics audit; record-size arithmetic checked.
NOT RUN: private resource byte decoding, packed data validation, native compilation, Windows gameplay, screenshot or speed benchmark.
Changes: this report and graphics continuity index only. No source, resource, CI, protected baseline, mission-builder, or core-audit modifications.
