# MB-AUDIT-5.0 — Objective eligibility and mission terminal paths (2026-11-01)

Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; lines 600–666, 669–1136, 9364–9539 and 46148–46162.
Status: static documentation audit only.

## Confirmed
- CPGetObjective invokes CPCanAgentDoMission (9412–9417) while selecting a CP objective.
- CPCanAgentDoMission (9498–9539) checks an agent-objective assignment byte at objective offset 0x05 against the agent byte at 0x07, allowing wildcard 0xFF. For ActionType 3, it rejects when 16-bit objective offset 0x09 matches the agent byte. When objective flags at 0x06 contain 0x08, it calls person_has_weapon with literal weapon ID 0x0C and rejects absent the weapon. Without these rejection conditions, it returns 1. Exact gameplay semantics and weapon name remain unverified.
- The tactical objective evaluator (approximately 655–1136) uses entries at 14-byte stride, iterating eight entries and checking completion flags. At 1112–1130 it tests each objective's first dword against 1 and calls level_complete when all are marked complete (with a separate mission cheat shortcut). This is *distinct* from 15-byte CPObjectives.
- A person/agent state condition invokes level_failed around 647. Additional event-handler cases call level_failed at 46152 and level_complete at 46161, proving terminal transition paths exist outside the eight-entry evaluator.
- These source paths substantiate objective-driven victory, but do not yet establish a safe external mission editor schema or full validity of objective type codes.

## Risks and next evidence
- Preserve the terminal flags and original campaign semantics; do not change engine behavior based on these findings.
- Decode evaluator jump table vtable_10550 and its per-type handlers, separating objective completion from other terminal events.
- Trace load_map_level and level objective bytes to file offsets in actual legally supplied assets before creating a writer.
- Verify both normal and error transitions at runtime in a disposable Windows install before integration.

## Verification
Read complete existing assembly blob and checked named call sites and local branches. No game code changes, no compilation, no gameplay execution, no changes to assets or saves. Protect v0.7, 16 FPS, original campaigns, and independent campaign save requirements.
