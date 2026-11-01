# Syndicate 2026 — Eight native batch-loader callers: consolidated handover

Date: 2026-11-01. Static source review only.

## Sources
engine/conf/wrappers_game.conf maps LbDataLoadAll and LbDataFreeAll to native C. engine/bflibrary/src/general/gdata.c implements batch loading, continuing on individual failures and returning an error count. engine/src/syndre.sx, indexed full assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Eight direct assembly callsites to ac_LbDataLoadAll
1. Line 27902: mission_load_files, continues into sound/graphics/map setup without inspecting returned error count.
2. Line 27908: sound_bank_files0, continues to ac_sound_bank_setup.
3. Line 27926: unkn1_empty_load_files, continues to init_level_data.
4. Line 31251: graphics-mode load_files_vres16 or load_files_mcga, continues to sprite setup.
5. Line 31723: mmapblk_load_files, continues to display_cities.
6. Line 54162: menu unk984_load_files, continues to sound and sprite setup.
7. Line 54171: menu sound_bank_files1, continues to ac_sound_bank_setup.
8. Line 55040: menu sound_bank_files1 reloading, continues to ac_sound_bank_setup.

No inspected direct caller immediately branches on or checks the returned error count. This finding concerns direct assembly callsites only, not every possible indirect or C caller.

## Native cleanup cross-check
LbDataFree() frees stored allocations and resets Start/SEnd pointers. LbDataLoadAll() frees the list, attempts each item and reports number of failed items. In LbDataLoad(), a file-read failure after allocation resets Start without an immediately visible free; potential unreleased allocation requiring runtime confirmation. Negative/nonpositive resource lengths are rejected in C, so do not attribute legacy assembly allocation semantics to active wrapped C routines.

## Consequence for Mission Builder
Resource validation and all-or-nothing mission start must be defined before custom input can use these loaders. This is a proposed safeguard, not implemented functionality. Keep campaign ID, mission ID, map ID and language distinct; preserve separate Original, American Revolt, World Domination and playable custom campaign save access.

## Next audit and verification gates
- Trace specific consumers for null dereferences and whether they independently handle partial loads.
- Define failure-state and cleanup regression cases and test on Windows i686 with legal game assets.
- Check active resource wrapper behavior under short reads/corrupt files.
- Preserve game v0.7 baseline; no engine modifications during audit.

## Handover status
Performed exact source-call review and native wrapper cross-check. Not performed compile, runtime tests, memory instrumentation or asset tests. Documentation only; no version bump.
