# Syndicate 2026 — Objective Indexing and AI Dispatch Source Audit
Date: 2026-11-01 (Australia/Brisbane). Engine audit branch feature/v0.9-visual-showcase. Main remains protected.
Mission Builder counterpart: feature/mission-builder-audit, docs/mission-builder/HANDOVER.md; read-only coordination.
Original source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Confirmed original assembly evidence
- Lines 7051-7070: init_computer_players iterates objective entries with 15-byte additions and clears selected flags via AND 0xef.
- Lines 7604-7626: CPDoTurn derives CPObjectives pointer with index multiplied by 15; the objective structure is not an unconstrained runtime container.
- Lines 7755-7784: CPDoTurn checks completion with CPCompletedObjectiveStep. It reads objective field +7, compares to 4, then invokes CPSetupDoMissionObjective for that value, otherwise CPSetupNewObjective. This is evidence of a **specific objective action dispatch**, not proof of a complete custom-mission API.
- Lines 9741-9756: CPGetFirstObjectiveInList follows the 16-bit link at entry offset +2, converting each link by multiplication by 15, until that link is zero. An editor must preserve correct index/link encoding and terminate chains; this pointer-like list is not a generic C pointer list.
- Lines 9379-9399 and 9459-9467: more objectives are manipulated using 15-byte strides, and fields +6,+7,+8,+9,+11,+13 are written. Their meanings require careful correlation with game_data.h.

## Mission Builder integration implications
Treat the existing objective block, indexed 15-byte record links, setup dispatch and action values as ABI constraints. Do not invent new ActionType values or modify original campaign objective links without proof. First custom-mission prototype should establish how an existing objective set is selected and how win/loss is detected, then extend via separate definitions and safe translation. Campaign-specific saves must stay siloed; this report modifies no save paths or mission data.

## Verification ledger
PASS: original assembly GitHub blob read at all cited addresses, static index and branch trace; no files of proprietary game data published.
NOT RUN: Windows compilation, mission objectives at runtime, AI turn handling, save restore, new mission, mission editor, regression or performance tests.
OPEN: exact action values/CPObjective field semantics, objective loading from mission resources, chain integrity bounds, mission-end result dispatch, ownership of custom mission state.

NEXT: trace CPDoTurn and objective loader callers, mission-end state transitions, and briefings; coordinate any planned shared source changes with Mission Builder branch. Rollback: documentation only.