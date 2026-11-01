# Main Systems — level loading and state ownership

Date: 2026-11-01. Source-only audit. Working engine unchanged.

Source: directly inspected complete engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf at lines 27660–27725, 27845–27933, 28217–28312, and prior mission lifecycle/dispatch reports.

## Verified findings
- load_map_level (27845–27933) calls clear_savegame before assigning current_levno, formats a level file name, and uses distinct branches for DrawFlags values 2 and 4. These branches load compressed level data via LbFileReadRNC; the first configures VGA screen buffer pointers and mission_load_files, loads sound_bank_files0 if sound enabled and calls init_hires_blocks. The second loads unkn1_empty_load_files. Both reach init_level_data. Hence map loading depends on display mode, compressed input, asset-bank lifetime and level memory.
- init_level_data (27712 onward) starts by calling init_map_data(map_buf), then operates on sprite data and other level structures. This ties map file loading to object and sprite initialization.
- set_structure_ends (27660–27708) finds active limits of person, vehicle and object arrays by backward scans, assigning last_person, last_vehicle and last_object. Its record strides differ by type; structure changes must preserve these layouts.
- initialise_player (28217–28270) initializes single-player slot defaults. In multiplayer it checks NETBIOS/setup return codes, prints login failure/abort and can call exit; it sets player/network state, starts network and exchanges player records. Single-player explicitly clears multiplayer mode.
- set_network_player (28274–28312) copies a 0x417-byte player record into a slot, resets associated packet/network fields, records current_levno and player assignment. This directly connects player struct layout, campaign state and multiplayer packet initialization.

## Cross-audit implications
The Graphics Audit must preserve legacy mode-specific buffer choices and compressed resource loading. Mission Builder must preserve file loading, level__Seed, map-object initialization and objective/actor array layouts. Main Systems must preserve savegame reset timing, per-slot player record ownership and mission asset teardown. These are source-derived interfaces, not instructions to change the other branches.

## Important uncertainties
The semantics of clear_savegame, the full compressed level format, complete init_level_data pipeline, the entire object/mission data schema, resource error handling and runtime differences across display modes remain unverified. Calling this comprehensive behavioural coverage would be incorrect.

No gameplay testing, profiling, compilation, engine edits, workflow changes or other workstream edits.
