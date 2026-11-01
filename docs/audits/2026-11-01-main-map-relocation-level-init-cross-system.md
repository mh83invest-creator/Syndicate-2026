# Syndicate 2026 — Map relocation and level initialization audit

Date: 2026-11-01. Static source audit, no engine changes.

Source: full engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf. Verified locations: clear_savegame 27627–27638, set_structure_ends 27660–27708, init_level_data 27712–27813, load_map_level 27845–27933, init_map_data 25694–25716.

## Newly confirmed source contracts

1. clear_savegame explicitly clears the memory range from level__Seed through level__End via ac_LbMemorySet. It is invoked at the beginning of load_map_level *before* changing current_levno or loading map data. This is tactical level-state memory, not proof that user campaign save slots are deleted; name alone is misleading. Never change this order without tracing every reference to the shared memory range.
2. init_map_data receives map_buf, skips a 12-byte header, then loops through exactly 0x3000 (12,288) entries of four bytes each. For each entry it reads an offset, adds the base pointer (map_buf+12), then stores the resulting pointer back in that entry. This is an **in-place offset-to-pointer relocation** operation. Running it twice on the same buffer, or changing pointer size/endianness, would change behaviour and may corrupt the map; these are source-inferred hazards, not reproduced bugs.
3. load_map_level uses distinct branches depending on DrawFlags, with compressed file loading, mission resources, audio bank setup, high-resolution block initialization or an alternate load-file set, and converges on init_level_data.
4. init_level_data calls init_map_data, sets up two sprite sets, initializes computer players in single-player only, adjusts vehicles, invokes AppScreenSetup with mode 0x12, clears the surface, chooses a multiplayer palette name or level-specific palette, loads palette data, updates mouse sprite and button state, recalculates active entity boundaries via set_structure_ends, and resets sound sample statuses. Mission initialization is coupled to simulation, UI, graphics and audio.
5. set_structure_ends scans fixed-size records and assigns last_person, last_vehicle and last_object bounds. The relocations and entity-bound computations constrain any future mission-data loader/editor.

## Interfaces to other audits

Mission Builder: preserve the map header, 0x3000 offset table, relocation-once semantics, level layout and actor/object bounds. Those are *candidate binary-format contracts* requiring source-format confirmation before any editor writes files.
Graphics: preserve screen mode/palette selection, sprites, mouse-coordinate and buffer lifetimes in level transitions.
Main Systems: preserve tactical state clearing versus persistent campaign saves, single-player AI setup, multiplayer exceptions, resource loading and audio status reset.

## Not proved
The target data represented by all 12,288 relocated entries, validation of offsets before relocation, full map file size, portability to 64-bit pointers, complete mission binary schema, and runtime handling of malformed input remain unknown. No claim of runtime correctness, build or playtesting.

No program, workflow or parallel Graphics/Mission Builder workstream modified.
