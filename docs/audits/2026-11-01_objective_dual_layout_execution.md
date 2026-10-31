# Syndicate 2026 — Original objective execution dispatch audit

Date: 2026-11-01 (Australia/Brisbane). Status: focused static audit, no gameplay implementation.
Branch: feature/v0.9-visual-showcase. Protected original v0.7 source baseline unchanged.
Original source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
Concurrent Mission Builder: feature/mission-builder-audit; coordination docs/audits/2026-11-01_mission_builder_concurrent_contract.md.

## Inspected source paths
- syndre.sx:9031 onward, CPCompletedObjectiveStep tests the byte at CP objective offset +7, branching on type 1 and the action byte at +8. The action comparison distinguishes several cases, including 3, 9, 11-14, 20 and 25. This is evidence that execution is value-specific; it is NOT a validated complete enumeration of mission objectives.
- syndre.sx:9600 onward, CPSetupDoMissionObjective resets byte +6 of the provided structure, indexes a separate level__Objectives array with a 14-byte stride (index*14), inspects per-objective words and dispatches through vtable_17008. This exposes a second objective structure distinct from CPObjectives' established 15-byte entries. A future editor cannot collapse the two representations.
- syndre.sx:9742-9756, CPGetFirstObjectiveInList traverses linked CPObjectives using the word at offset +2 and multiplying by 15. No cycle or out-of-range index check is visible in that short routine; malformed custom objective chains require separate validation before they reach native code.
- syndre.sx:9773 onward, CPSetupNewObjective reads the type byte at +7, subtracts one, bounds the table index to 0..9, and dispatches through vtable_17278. A type outside the range takes the default branch, rather than invoking arbitrary code.
- syndre.sx:7755-7784, CPDoTurn selects CPSetupDoMissionObjective on type value 4, otherwise CPSetupNewObjective. Type values and behaviors are original engine conventions and must not be repurposed without a compatibility test.

## Constraints for Mission Builder
Two separate objective record layouts appear in the execution path: 15-byte CPObjectives and an apparent 14-byte level__Objectives stride. This requires source-to-asset verification, exact bounds and a translation contract. Custom editing must not write raw arbitrary link indices or unsupported type/action values directly into original global structures. Original campaigns and mode-specific saves remain protected.

## Verification ledger
PASS: source blob inspected at cited lines, jump-table range and index arithmetic confirmed; no binary or data changes.
NOT RUN: decoded level objective table verification, original runtime, AI/action completion, new missions, Windows compilation, save restore, stress tests.
NEXT: trace level__Objectives allocation and loading, exact packed C type, map/missions relationship, and dispatch return conditions; share a read-only audit link with concurrent Mission Builder branch before code integration.
Rollback: documentation only; no source changes.