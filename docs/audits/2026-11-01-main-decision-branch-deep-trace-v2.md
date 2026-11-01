# Syndicate 2026 — AI, multiplayer and objective decisions: detailed source audit

2026-11-01. No source or build changes.

Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, direct assembly inspection.

## AI decisions
CPDoTurn (7604 onward) examines agent flags, objective records, equipment/ammunition, and produces player action packets. Observed action values include 0x06 and 0x18, followed by process_action. It uses random, objective dispatch jpt_15D74 and follow-on jpt_15E6C. Handlers call CPGetObjective, CPDoObjectiveStep, CPGetFirstObjectiveInList, CPCompletedObjectiveStep, CPSetupDoMissionObjective and CPSetupNewObjective (7696–7836). Combat paths (7840–7990) call CPTargetAlive, cp_i_can_see_and_shoot_person, CPShouldIShootMyTarget, CPShootPerson and CPGotoOldStateAndRelax. This establishes targeting, visibility, firing and fallback state transitions, not a full opcode-by-opcode truth table.

## Multiplayer
multi_play (28106–28213) records local level seed, person count and counter, calls NetworkPlayersCount, then ExchangeNetwork_Packet, and iterates process_action for Network__NumberOfSlots. A later status/display path accesses NetUserNcb, compares bytes at offsets 2 and 0x31, formats peer fields and calls set_all_changes. In the examined source, the ExchangeNetwork_Packet result is not immediately tested by a conditional branch before the action loop. This does not prove packet failure is ignored internally; inspect that function separately. No network runtime or disconnect tests.

## Mission outcomes
check_end_level (650–1130) uses objective-type table vtable_10550 and a second table vtable_10594. Several objective handlers set level__Objectives, and distinct conditions call level_failed (647,691,754,949). At the shared objective pass tail the engine calls level_complete. The handlers include agent state, tracked target pointers and getrdist distance comparisons. Consequently completion/failure is not one boolean criterion, and changing its logic risks mission-specific regressions.

## Strategic state and finance
AdjustStateHappiness (11759 onward) checks city ownership and modifies country_states; GetRevenueFromCity (11854 onward) checks owner and player slot and reads city economy fields. process_day performs research, happiness, revenue and strategic AI updates. No runtime comparison of financial formulas.

## Open evidence gaps
All objective dispatch targets and exact success conditions, all CPDoTurn handler cases and exact packet schema, ExchangeNetwork_Packet internal timeout/error handling, real multiplayer sync, save/reload and mission-specific outcomes remain unverified. Structural inspection must not be represented as comprehensive dynamic verification.

Protected v0.7, Graphics Audit and Mission Builder untouched.
