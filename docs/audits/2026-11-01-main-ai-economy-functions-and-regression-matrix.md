# Syndicate 2026 — detailed subsystem ownership and regression matrix

Date 2026-11-01, documentation only.

Source: engine/src/syndre.sx full Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf. Directly located functions and inspected their opening instructions: CPDoTurn ~7604, DoResearch ~51948, GetRevenueFromCity ~11854, AdjustStateHappiness ~11759, ComputerPlaysMap ~11896, level_failed ~451; previous report documents their callers.

## Detail
CPDoTurn receives an indexed computer-player record and inspects associated person state and level__CPObjectives, including an early status branch. Its full strategy is NOT established.
DoResearch reads research selection/progress state and current Network__Slot/player data. Research is advanced through process_day, not an independent wall-clock timer as far as this inspected call path shows.
GetRevenueFromCity checks current player ownership against data_5539e, checks country_states and accesses per-city data before returning revenue; process_day adds its result to player state.
AdjustStateHappiness is also ownership-dependent and runs during process_day.
ComputerPlaysMap scans next_mission_countries and uses HowManyStatesDoIOwn, demonstrating strategic rival-state planning.
level_failed conditionally sets mission-state bit 0x4 in data_60afc, modifies tactical status and triggers sample status; it is not the entire failure handling lifecycle. level_finished later interprets status bits and may reset player state.

## Dependency-preserving regression matrix (test design, not executed)
| Scenario | Cross-systems that need comparison |
| --- | --- |
| New campaign / mission selection | default player, resources, sound banks, selected team, map progression |
| Mission success | objective dispatch, country_states, agent transfer, economy and next mission |
| Abort / failure / retry | failure flags, audio/display restoration, asset free, campaign reset conditions |
| Save/load each slot | fixed binary ordering, credits, equipment, research, team and country ownership |
| Save file open failure | in-memory credits, save UI state, no partial overwrite |
| Tactical mouse/keyboard | ring buffering, scaled coordinates, panel selection, per-slot packets |
| AI turn and single-player | AI interval, CPDoTurn, player commands, random state |
| Multiplayer | network packet exchange, seed/person-count state, action ordering and disconnected peers |
| Calendar progression | research, city revenues, happiness, strategic map AI, game speed |
| Single/multiple explosions | effect allocation and MapWho chains, damage, audio, rendering and pacing |
| Display/menu switches | palettes, legacy buffers, SDL lock/unlock, menu 30 FPS vs tactical 16 FPS |
| Exit/restart | audio queue and buffers, input/screen reset, asset lifetime |

## Static-audit limitation
The entry points above are established but not every deep branch, network packet payload, objective type or data format has been exhaustively enumerated; this report must not be mistaken for runtime verification. No engine or workflow edits, builds, tests or profiling. Do not merge any modification until observed regression behaviour is compared with baseline.
