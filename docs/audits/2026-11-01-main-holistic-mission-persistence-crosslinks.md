# Syndicate 2026 — holistic architecture supplement: mission transitions, asset ownership, persistence call sites

Date: 2026-11-01. Main Systems static audit.

## Source basis
Direct full Git blob `engine/src/syndre.sx` at `c35e22621a93d1e07c44d40de8867446ff7c64bf` and `engine/src/game_data.c` at `23a7159fc016e58872d30b61b46ba62b6efabac7`.

## Newly verified exact relationships
- Assembly call-site scan identifies exactly one lexical `call ac_load_player` at assembly line ~30627 and one `call ac_save_player` at ~30643; `ac_load_save_text` occurs at ~30397 and ~54577. These are **lexical direct call counts in one assembly file**, not an exhaustive linked-binary call graph (indirect calls, other code and wrappers are not excluded).
- `reset_mission_info:25849–25903` clears multiple tactical state globals, resets `SoundActive` and `MusicActive`, `ScannerPulse`, sets `game_speed=7`, resets arrays and clears a 0x408-byte region using `ac_LbMemorySet`. This is a **multi-subsystem reset**: changing it affects input/game flow, sound and timing, not just mission bookkeeping.
- `select:54149+` loads `unk984_load_files`, conditionally loads `sound_bank_files1` and initializes the sound bank before selection UI. Thus menu selection is linked to asset and sound-bank lifetime.
- `free_map_level:27640–27656` frees mission assets and another load-file set, and when sound is enabled calls `StopAllSounds` and frees `sound_bank_files0`. This is an asset+audio shutdown boundary, not just a level-memory clear.
- `level_finished:54998+` sets screen buffer pointers and render flags, invokes `ac_AppScreenSetup`, initializes sound/music states, potentially reloads `sound_bank_files1`, and branches based on mission state/country conditions. Mission completion thus integrates campaign progression, screen mode, UI and audio.
- `set_structure_ends:27660+` computes limits for active people, vehicle and object arrays using record-size-specific backward scans; the effective simulation bounds are updated at level preparation. Avoid changing structure layouts or actor/effect traversal without considering these sentinel pointers.
- The C save routine records players/countries/weapons/mods/research/team together in fixed binary layout and can reduce credits before checking file open, as previously reported. See `game_data.c:141–191`.

## Whole-engine consequence
Mission transitions are **not** cleanly separated from presentation, asset ownership, audio state, economy or simulation array limits. Proposed fixes to either mission transitions or renderer/audio lifetime require a shared regression test. The safest next engineering step after documenting these contracts would be an isolated diagnostic build and real playtesting, only when authorized.

## Unresolved
Full objective outcome semantics and every multi-branch menu path, indirect function invocation, binary compatibility checks, networking contract, data file formats, and runtime memory validity are not proven by this scan. No modification or runtime testing.
