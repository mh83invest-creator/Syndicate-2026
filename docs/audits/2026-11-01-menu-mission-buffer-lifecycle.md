# Syndicate 2026 — Menu-to-Mission Buffer Lifecycle Audit
Date: 2026-11-01. Static source inspection, not runtime verified.
Related: docs/audits/2026-11-01-map-palette-descriptor-lifecycle.md

## Source inspection
- Assembly select() lines 54149–54163 calls ac_LbDataLoadAll(unk984_load_files), whose descriptor list has the star-prefixed map_buf allocation of 85152 bytes at lines 85040–85045.
- select() lines 54838–54839 calls ac_LbDataFreeAll(unk984_load_files) on its normal exit path. This establishes source-level pairing within select(), but does not establish state on every error/abnormal exit path.
- Mission loader load_map_level() begins at line 27845; its named mission_load_files resource list includes data/mapXX.dat targeting map_buf at lines 82625–82632. It calls ac_LbDataLoadAll(mission_load_files) at lines 27901–27903; init_level_data then consumes map_buf at 27719–27722.
- free_map_level at 27640–27656 calls ac_LbDataFreeAll(mission_load_files) and related resource cleanup. The top-level assembly flow calls free_map_level at line 285 after leaving gameplay.
- Calls to load_map_level at lines 28498 and 28536 occur on separate network/single-player paths; these callsites do not independently enforce a map-buffer capacity.
- Graphics resources select load_files_vres16 or load_files_mcga around 31239–31253. Their palette entries refer to GraphicsPalette. This is graphics-mode setup, not a complete map buffer lifetime proof.

## Interpretation and remaining gaps
The intended resource ownership is menu-specific map allocation/free and mission-specific map loading/free. The menu's 85152-byte declared allocation is not evidence of a fixed tactical-map allocation size. Full interprocedural proof is still needed for transitions, early exits, repeated mission starts and failure paths. No memory fault has been reproduced.

## Next checkpoint
Inspect the native ac_LbDataLoadAll/ac_LbDataFreeAll wrappers and their underlying code for nulling freed pointers, failed allocations, repeated calls and partial load cleanup; then design Windows regression cases for transitions. Preserve protected v0.7 and the Mission Builder save-silo contract.

## Verification
Source-reviewed exact assembly entry points and resource descriptors. No engine edits, compilation, runtime tests or real game assets inspected. Documentation-only audit.
