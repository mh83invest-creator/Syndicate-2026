# Main Systems — map buffer and asset ownership

Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf. Static inspection 2026-11-01.

The mission_load_files descriptor contains a map file entry assigned to the global pointer map_buf (82620–82631). The same descriptor includes font, colour, hi-res block, sprites and animations. Tactical load_map_level calls LbDataLoadAll(mission_load_files) in one display-mode branch and its cleanup free_map_level invokes LbDataFreeAll(mission_load_files), plus the alternate descriptor and, when enabled, sound-bank cleanup (27640–27656, 27901–27930). This establishes descriptor-level ownership, but not the allocator implementation or whether all pointers are reset by LbDataFreeAll.

mission_briefing separately loads the selected map into map_buf using LbFileReadRNC and applies init_map_data. Tactical init_level_data also calls init_map_data. The briefing is invoked from the selection menu at line 54661; tactical loading is called from action processing at 28498/28536. Because map_buf is shared, pointer lifetime and the order of reads/relocations matter. Source paths do not yet prove whether one buffer is always overwritten between both relocations.

init_map_data rewrites exactly 0x3000 four-byte map offsets in place, relative to map_buf+12. It performs no per-offset validity check in the inspected routine. Do not claim that source-level absence of validation proves a reachable malformed-file vulnerability; loading and file content may impose other constraints.

Preserve descriptor ownership, 32-bit map offsets, map reload ordering, sound bank teardown, and briefing versus tactical map loading. No engine edits or runtime tests. Pending: inspect LbDataLoadAll/LbDataFreeAll/LbFileReadRNC implementations and memory allocation bounds; determine branch-specific resource freshness and errors.
