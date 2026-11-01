# Main Systems audit — resource manager implementation and buffer freshness

2026-11-01. Source-only, no engine changes or playtesting.

Sources: engine/bflibrary/src/general/gdata.c (blob 497816319c0e438076e90b7166e8677f41657138), engine/bflibrary/src/general/gfile.c (blob 5184353e6ccf2f7604edbe043f2c52ee6543d00c), and engine/src/syndre.sx (blob c35e22621a93d1e07c44d40de8867446ff7c64bf).

## Verified resource ownership and allocation
- LbDataFree resets SEnd to null when present and frees and nulls a descriptor's Start allocation through LbMemoryFree.
- LbDataFreeAll iterates the descriptor list until Start is null.
- LbDataLoad calls LbDataFree on its descriptor before loading; chooses LbMemoryAllocLow when Flags bit 0 is set, else LbMemoryAlloc.
- LbDataLoadAll first calls LbDataFreeAll on the complete list, then attempts each descriptor. Thus **reloading mission_load_files explicitly frees and replaces its map_buf allocation**, when that descriptor is present in the selected branch and the allocation succeeds. There is no reliance on allocator picking a different address.
- The load-all return is an error count, not a boolean success; it logs allocation and load failures, but continues to later entries. The inspected tactical assembly call at syndre.sx:27903 does not check that return. Failed map loading could leave map_buf null; whether this leads to a reachable crash depends on resources and other checks.
- LbDataLoad checks LbFileLengthRnc, allocates slength + 512 bytes, calls LbFileLoadAt and expects exactly slength on success. A failed load path sets Start and SEnd null and SLength zero; the code does not visibly free that just-allocated pointer in the failure branch, suggesting a potential allocation leak. Do not claim this is observed at runtime.
- LbFileLengthRnc opens the file and reads its RNC header, returning the declared uncompressed length if signature matches, otherwise ordinary file length; -1 on certain read/open failures. LbFileLoadAt attempts a read and invokes UnpackM1, returning decompressed length or -1. These functions explain compressed-size interpretation but do not alone establish map payload format.

## Clarification of earlier uncertainty
There are two init_map_data call sites: mission_briefing after a direct map read, and init_level_data after tactical loading. In the mode-2 tactical load branch, LbDataLoadAll(mission_load_files) frees and replaces map_buf before init_level_data; thus prior briefing relocation is not reused *on this successful branch*. In mode 4, the code loads unkn1_empty_load_files instead; source-level evidence does not yet establish replacement of map_buf in that branch, so a mode-independent conclusion would be wrong. Full alternate resource descriptors and selection transition must be traced.

## Preserved contracts / next static evidence
Maintain descriptor terminators, 32-bit map offsets, reallocation timing, explicit map and RNC failure pathways, and mode-specific ownership. Remaining: inspect alternate descriptor for map loading, LbFileReadRNC implementation, LbMemoryAlloc failure behaviour and level file-size guards. No other audits or branches edited.
