# Syndicate 2026 — holistic engine audit: startup, mission state and frame lifecycle

Date: 2026-11-01 (Brisbane). MAIN audit, source inspection only.

## Verified sources
- `engine/src/main.c` blob `e9621c6594173569e78366553ddc2de820e407f8`.
- `engine/src/game.c` blob `9f56221d527075fba2b1f2406fab60af28a4289a`.
- `engine/src/display.c` blob `9c6ba018f31d67b394c5973d05d6e51824171b68`.
- `engine/src/sound.c` blob `d2f5946d11e11cb123846ee5e54ee4065bf1f07d`.
- `engine/src/Makefile.am` blob `dfaad56c157bd9042bec49e681b619794b0fcba3`.
- Full `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, freshly inspected lines 198–303.

## Confirmed structure
1. `main.c:266+` initializes options, game support, display buffers, screen mode, assets, UI strings, mouse and keyboard, then calls `init_audio()`, `init_buffered_keys()`, and assembly `syndicate()`. Following the game loop it clears data/buffers and calls `game_quit()`. Some paths and optional code are compile-time conditional.
2. Assembly `syndicate:199–303` first sets default player. Its outer loop invokes `select`, then `reset_mission_info` and `initialise_player` when entering a mission, then conditionally enters tactical loop.
3. Tactical loop `syndre.sx:215–279` processes player turn and single/multiplayer/computer turns, calls `move_it`, handles buttons/day/end-state, waits VBI, swaps screen, handles palette/scroll/copy/map sprites/panel/click, then calls `ac_game_update` for non-DOS event and pacing support. Explicitly preserve order, including the two scroll_map calls and conditional branches, until their semantics are fully understood.
4. Exiting mission loop at `syndre.sx:280–299` can stop music, free map level, branch directly back to selection, or transfer people and call `level_finished` before returning to selection. Thus mission state transitions have multiple paths, not merely a single post-level cleanup.
5. `game.c:141–196` uses shared `last_frame_ticks` and `SDL_GetTicks` to pace updates. It unlocks display, handles SDL events, optionally sleeps in chunks while pumping events, then relocks display. This timing function is not the entire tactical frame; simulation/display calls occur before `ac_game_update`.
6. `Makefile.am` confirms hybrid C and assembly executable with generated Watcom/native wrappers and shared libraries. The whole-engine map must account for this bridge rather than treat the program as conventional C alone.

## Provisional dependency map (not yet fully audited)
Startup/configuration → resource loading → player/menu selection → mission initialization → tactical per-frame input/network/AI → simulation (worlds/people/weapons/effects/objects/vehicles) → temporal state and end checks → display/map/panel/input processing → SDL event/pacing → mission termination/transfer/progression → selection or quit. Audio spans startup, tactical, mission transitions and shutdown. Persistence and campaign data must be traced separately before claiming complete knowledge.

## Open questions and next investigation
- Trace `select`, `reset_mission_info`, `initialise_player`, `free_map_level`, `transfer_people_into_player`, `level_finished`: ownership of campaign vs temporary mission state and failure/escape cases.
- Trace input paths to `process_players_turn`, `click_map`, `scroll_map`, and SDL events; distinguish keyboard/mouse ownership across C and assembly.
- Trace rendering buffer ownership, palette updates, VBI, C screen swap, SDL surface presentation; coordinate only conclusions with separate Graphics Audit.
- Trace sound start/stop, servicing/callback schedule, sound-bank ownership and shutdown.
- Trace save/load and research/economy persistence across mission selection and completion; avoid regression to campaign-specific saves.

## Evidence and verification limits
Direct static source inspection and GitHub file blob identities only. No engine modification, workflow change, build, profiling, actual gameplay test, or unverified performance claim. Graphics Audit and Mission Builder untouched.
