# MB-AUDIT-5.2 — Objective load and initialisation order (2026-11-01)

Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; lines 27627–27636, 27712–27745, 27845–27933, 7052–7085 and 85770–85800.
Scope: static, documentation-only.

## Verified loading order
1. load_map_level invokes clear_savegame at line 27851; clear_savegame zeroes [level__Seed, level__End) using ac_LbMemorySet at 27627–27636. This region includes both tactical and CP objective structures.
2. load_map_level formats mission ID into its supplied level filename and invokes LbFileReadRNC with level__Seed as destination at 27873–27880 when DrawFlags is 2, with an alternate load at 27915–27919 when DrawFlags is 4.
3. For DrawFlags 2, it obtains level__MapNumber from loaded data, normalises zero to one, formats map resource name, and loads mission_load_files. For DrawFlags 4, it also normalises zero map number before loading a different resource table. Thus mission and map selection happen before init_level_data.
4. Both paths converge on init_level_data at 27930. This function calls init_computer_players at 27744 only when is_multiplayer_game is zero.
5. init_computer_players loops over 128 CP objective records in groups of four, 15-byte stride, and ANDs the flags at record offset 6 with 0xEF (clears bit 0x10). It then copies computer player count and team-size settings to runtime state.
6. level__Objectives at line 85770 occupies 112 bytes, up to the next symbol at 85782 (8 x 14 bytes); level__CPObjectives at line 85798 occupies 0x780 bytes (128 x 15). Both are inside the level storage cleared by clear_savegame.

## Interpretation boundaries
- The source supports that mission-file data is loaded into level memory containing the objective arrays, but actual on-disk layout, compressed block length, validation, and objective values have NOT been verified against retail assets.
- There is no demonstrated authoring schema or safe write path. Do not assume every in-memory field is directly editable on disk without checking RNC decoding and mission-level offsets.
- The code shown contains no explicit return-value check immediately following LbFileReadRNC in these loading branches; error handling elsewhere or runtime responses remain unverified.
- Multiplayer skips the specific CP objective initialisation call. Do not infer there is no other multiplayer processing.
- The call to init_computer_players clears a runtime flags bit, not necessarily all state or all objective completion bits. Avoid asserting full reset.

## Next audit
Calculate exact offsets from level__Seed to the two arrays and identify RNC reader size/return contracts, source representation, and any independent objective reinitialisation or mutation after loading. Actual content validation needs lawful retail sample assets and disposable Windows runtime tests.

## Verification
Inspected assembly source directly, no engine/game changes, no compilation, no runtime/gameplay or save tests. Preserve original and American Revolt modes, protected v0.7, fixed 16 FPS and campaign save isolation.
