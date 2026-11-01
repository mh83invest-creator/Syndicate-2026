# MAIN workstream handover — behavioural deep audit batch

Date 2026-11-01. User direction: audit AI decisions, multiplayer coordination, research/economy progression and mission outcomes; preserve baseline and do not require repeated continuation requests.

Source-backed reports:
- docs/audits/2026-11-01-main-deep-behaviour-ai-network-economy-outcomes.md, commit 8ac9e961223da6404efbfbe725d68e16285c5559
- docs/audits/2026-11-01-main-ai-economy-functions-and-regression-matrix.md, commit 2f02dbf3008e7d0850ee3dec4f93c27b9aefff6e

Verified directly from full syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf: process_action dispatch uses per-player packets and a bounded jump table; multi_play exchanges packets before processing each network slot; AI process_computer_players is interval-gated and invokes CPDoTurn, which reads computer-agent objectives; process_day calls DoResearch, AdjustStateHappiness, GetRevenueFromCity and ComputerPlaysMap; GetRevenueFromCity and AdjustStateHappiness depend on current player ownership; check_end_level uses objective type jump dispatch, can call level_failed; level_finished mutates campaign fields, sound/display and may reset player data on loss. Test matrix defined for saves, campaigns, input, AI, multiplayer, pacing, assets and explosions.

Still unknown from static inspection: exact objective opcode semantics for all cases, full packet specification and network timing, every CPDoTurn decision path, exact research/finance formulas and gameplay result across all branches. No one should claim exhaustive behavioural certainty or runtime validation from these reports alone.

No engine edits, diagnostic branch/workflow, Windows builds, runtime tests, profiling or parallel branch changes. Protected v0.7 unchanged. Next permissible action only with explicit approval: isolated builds/tests; static audit may continue without changing game.
