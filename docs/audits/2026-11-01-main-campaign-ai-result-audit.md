# Syndicate 2026: holistic audit — campaign, AI and mission results

Source: engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf, read directly. Supplemental source: engine/src/game_data.c. Date: 2026-11-01.

## Verified findings
- Save menu: assembly 30621–30646 guards save slots below ten and calls ac_load_player (30627) or ac_save_player (30643). Other lexical ac_load_save_text call sites occur near 30397 and 54577. These are not exhaustive linked-binary counts.
- Single-player: single_play at 28088–28102 loops through eight indexed calls to process_action. Multiplayer uses a different path beginning at 28106.
- Input/tactical clock: process_players_turn at 38397–38427 increments level__Timer, reads button state and calls process_panel_people.
- AI: process_computer_players at 7493–7525 increments its own processing timer and uses an interval/last-process condition before inspecting computer_players_count. Do not assume AI frequency is independent of tactical update cadence.
- Mission preparation: reset_mission_info at 25849–25903 resets multiple tactical globals, active music/sound flags, scanner pulse, game_speed and a 0x408-byte buffer.
- Mission result: level_finished at 54998–55075 changes display setup and audio state and conditionally updates country_states. Exact result decision branches are not fully mapped.
- Existing C save routines store players, countries, weapons, cyber-mods, research and selected team together in fixed binary order. Save can mutate credits before file-open success is known.

## Cross-system implications
The game is not a set of independent features. Mouse actions, tactical timer, per-slot player commands, AI scheduling, gameplay state, mission results, screen/audio resources and save-file layout share control flow. Changes to frame cadence or mission lifecycle may alter campaign outcomes and AI behaviour.

## Pending before modification
Trace the full process_action and mission result decision trees, AI strategy, multiplayer contract and save failure paths; obtain actual runtime tests. Preserve working v0.7, its 16 FPS tactical cadence, original audiovisual effects and per-campaign save separation.

Documentation-only: no source, workflow or parallel workstream edits; no build or playtest.
