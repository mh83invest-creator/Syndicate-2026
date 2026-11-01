# Syndicate 2026 — briefing vs tactical map loading contract

Date: 2026-11-01. MAIN source audit. No engine or CI modifications, no playtesting.

## Direct evidence
Read complete `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`; inspected `mission_briefing` (1207–1298), `init_map_data` (25694–25716), `init_level_data` (27712–27841), `load_map_level` (27845–27933), `process_action` (28474–28550), plus direct call-site searches.

## Confirmed findings
1. **Distinct callers of map relocation:** `mission_briefing` reads `game.dat` for the selected level and `data/map%02d.dat` for `level__MapNumber` into global `map_buf`. After checking both compressed-read returns against `-1`, it calls `init_map_data` at line 1273. The second call, at line 27721, is from `init_level_data`, reached by tactical `load_map_level` after it loads mission resources. This distinguishes *briefing-time* data interpretation from *tactical-level* initialization. Whether the buffer is overwritten/fresh between calls depends on the complete selection-to-tactical path; **do not claim they always relocate the same bytes twice**.
2. `init_map_data` adds 12 to the base pointer and rewrites 0x3000 four-byte offsets as pointers into that base in-place. This is source-supported 32-bit relocation behaviour, not a fully decoded file format.
3. `mission_briefing` has explicit `-1` compressed-file failure branches (lines 1242–1245 and 1261–1269), returning code 2. In the separately inspected `load_map_level` body, compressed reads at 27880 and 27919 are not immediately checked for `-1` before continuing. Different error policies exist; do not conflate them.
4. `load_map_level` starts by clearing `level__Seed` through `level__End`, sets `current_levno`, chooses display-mode-specific resource paths, then always invokes `init_level_data`. The latter sets sprites, initializes computer players only for single-player, adjusts vehicles, loads a level-dependent palette (multiplayer uses a separate palette), resets mouse buttons and sound-sample status, and establishes active entity bounds.
5. `process_action` directly calls `load_map_level` at lines 28498/28536, followed by `init_players_people`; the multi-slot branch initializes each network slot. This is a whole-level and agent initialization operation triggered through an action handler.
6. Direct lexical assembly searches found `mission_briefing` called at 54661 and `load_map_level` called at 28498/28536. These counts are **not** proof of no indirect or other-language callers.

## Cross-audit contract
Mission Builder must preserve compressed briefing/map records, selected level and map-number semantics, the 12-byte header and 0x3000 relocation entries, and the distinction between on-disk offsets and live pointers. Graphics must preserve mode-specific resource buffers and per-level palette selection. Main Systems must preserve briefing error handling versus tactical-load handling, player/AI initialization and persistent-versus-level-memory boundaries.

## Not established
Whether each loading path uses a fresh allocation, full binary schema, validation and size limits, exact surrounding UI transitions, indirect callers, malformed-file resilience or runtime outcomes. No compile, tests, profiling or gameplay execution.
