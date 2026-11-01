# Syndicate 2026 — RNC Caller Inventory
Date: 2026-11-01 Australia/Brisbane
Status: complete **direct assembly caller inventory** for named routines in current syndre.sx blob; source audit only, no runtime verification.
Blob: c35e22621a93d1e07c44d40de8867446ff7c64bf (85,803 lines).
Previous: docs/audits/2026-11-01-mission-builder-briefing-safety-contract.md

## Direct calls to LbFileReadRNC — six
1. Line 1239: level-seed/game resource, destination EXPORT_SYMBOL(level__Seed) via generated game filename. Fails if open result is -1 (line 1241). Confirm destination capacity and file structure.
2. Line 1258: map resource, destination map_buf. Checks -1 open result at line 1260; does not establish buffer-size validation.
3. Line 27783: palette resource, destination GraphicsPalette, followed by screen-surface clear. No immediate return check observed.
4. Line 27879: level resource, destination EXPORT_SYMBOL(level__Seed), under graphics-mode branch. No immediate error check observed.
5. Line 27918: level resource, destination EXPORT_SYMBOL(level__Seed), alternate graphics-mode branch. No immediate error check observed.
6. Line 55279: briefing text, destination 4000-byte brief_text. No return/capacity validation and subsequent unchecked terminator index (previous audit).

## Direct calls to UnpackM1 — two
- Line 14774: generic data-loading path uses allocated buffer pointer from a load-file descriptor. Allocation-size handling around lines 14642 and 14728 must be examined further, including bad/negative lengths.
- Line 14941: LbFileReadRNC passes the same caller-provided buffer as both arguments to UnpackM1, without capacity.
No other exact direct call instructions to these two labels in this blob were found by full text matching; indirect calls, generated wrappers and C library callers are not excluded.

## Additional direct LbFileLengthRnc calls — four
- Lines 14642 and 14728: generic resource loaders.
- Line 14932: within LbFileReadRNC.
- Line 55283: briefing terminator indexing.

## Classification and priority
P0 briefing: confirmed bounded 4000-byte destination; negative or oversized index in load_text source; caller capacity absent.
P1 level seed/map/palette resources: potential capacity mismatch needs declared destination size and asset format evidence; DO NOT claim proven overflow.
P1 generic RNC loader: allocation and validation logic require review, especially on length failure.
IMPORTANT: Direct caller inventory does NOT mean all RNC-related entry points across C/native bflibrary have been mapped.

## Next checkpoints
A. Inspect generic loader allocation routines near 14642/14728 and any negative-size handling.
B. Identify map_buf and GraphicsPalette allocations and level__Seed destination layout; distinguish pointer vs scalar and static vs dynamic capacity.
C. Inspect complete UnpackM1 control flow for output bounds assumptions; do not implement patches during audit.
D. Define test fixtures and failure handling per resource class and validate on approved Windows i686 build with legal retail assets.
E. Coordinate Mission Builder briefing ingestion and separate campaign Save/Load/Continue namespaces.

## Tests/change log
Performed: GitHub full-blob indexed search for direct assembly calls and local context inspection. Not performed: compile, Windows run, memory instrumentation, synthetic fixtures, actual assets, save operations. Engine and Mission Builder branch untouched, documentation only. Preserve v0.7 known-good baseline.
