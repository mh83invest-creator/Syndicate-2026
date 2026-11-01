# Syndicate 2026 — Resource Descriptor Allocation Rules
Date: 2026-11-01 Australia/Brisbane
Status: static source audit only; code and save files unchanged.

## Source inspected
engine/src/syndre.sx full indexed assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Confirmed observations
DESC-01: LbDataLoad at 14475 onward distinguishes descriptors beginning with the '*' byte (0x2a) from filename-backed entries. For star-prefixed entries, lines 14581–14607 allocate and zero the predeclared size at descriptor offset 0x24, storing the resulting pointer through offset 0x1c.
DESC-02: For ordinary named resources, lines 14728–14745 obtain a size from LbFileLengthRnc and call LbMemoryAlloc(size) without an immediately visible negative-length guard. The loader's different memory-allocation modes require additional tracing.
DESC-03: On reads, lines 14759–14794 branch according to actual versus requested count; mismatched counts lead to UnpackM1 and a subsequent return of 1 without inspecting the decompressor's status. Runtime consequences unverified.
DESC-04: Graphics/display resource descriptor at lines 83626–83636 declares an explicit WScreen allocation of 256048 bytes, with a source comment explaining two 320x400 surfaces and safety bytes; this is NOT the capacity of GraphicsPalette, which appears as a separate subsequent resource.
DESC-05: A different graphics resource descriptor at lines 83651–83660 references an MCGA screen allocation and lpal01.dat palette. No explicit inline palette capacity is established at its pointer declaration.
DESC-06: The descriptor block at lines 85040–85045 contains a star-prefixed map_buf entry and a little-endian length field with bytes A0 4C 01 (0x014CA0 = 85152 decimal) at an apparent size offset. The exact descriptor structure is consistent with the LbDataLoad 0x24 size offset but must be checked at its actual descriptor start and loader call before asserting a runtime allocation of 85152 bytes.
DESC-07: mission_load_files at 82620–82632 supplies map_buf as a destination for the map filename. It is not yet proven that each mode's map_buf has the same capacity or that a map resource fits.

## Scope and follow-up
Trace descriptor start/stride and the two map use cases; verify allocation order and the resource's size and compression. Trace GraphicsPalette startup allocation through both display-mode descriptor lists. Inspect malformed-size and short-read paths with controlled tests before any source changes. Existing briefing bounds risk remains separately documented.

## Verification
Indexed source inspection only. Not run: Windows build, game launch, allocation tests, actual asset inspection or saves. Documentation only; protect v0.7 and independent Mission Builder branch.
