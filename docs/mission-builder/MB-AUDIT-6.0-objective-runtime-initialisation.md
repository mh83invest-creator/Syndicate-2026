# MB-AUDIT-6.0 — Mission objective source and runtime mutation
Date: 2026-11-01; branch feature/mission-builder-audit. Static source-only review.

## Source evidence
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- clear_savegame zeros the region from level__Seed to level__End (27627–27636).
- load_map_level reads gameNN.dat through LbFileReadRNC into level__Seed and then invokes init_level_data (27845–27930). Exact original decoded resource length remains unverified.
- level__Objectives begins at 0x9BE3A relative to level__Seed 0x80108, giving offset 0x1BD32; 8 x 14 = 112 bytes. The tactical evaluator changes status fields at runtime (e.g. lines 672, 740, 801, 1100).
- level__CPObjectives starts at 0x9BEB2, offset 0x1BDAA, with 128 x 15 byte records.
- init_level_data calls init_computer_players only for the non-multiplayer path (27738–27745).
- init_computer_players (7052–7088) clears bit 0x10 of the flags byte at offset 6 in each of 128 computer-player objective records; it also copies loaded computer-player count, team size, process interval and level-init configuration into runtime variables.
- This demonstrates loaded objective records and runtime updates are distinct concerns. It does not establish the exact on-disk byte coverage or original resource grammar.

## Constraints and next checks
Do not serialise a live modified level image back into an authored mission without separating source data from runtime changes. Verify actual decoded resource size, loaded record initial state and all post-load mutations with lawful original sample data before implementing a writer. Keep campaign and save separation protected.

## Verification
Assembly line inspection only. No game source, assets, saves, builds or gameplay tests changed or performed. Original and American Revolt, fixed 16 FPS and protected v0.7 unchanged.