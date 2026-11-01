# MB-AUDIT-4.9 — Objective dispatch table decoding (2026-11-01)

Branch: feature/mission-builder-audit
Evidence: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; assembly lines 9541–9580, 9600–9705, 9740–9825.
Status: static source audit only.

## Findings
- CPSetupNewObjective indexes vtable_17278 with ActionType - 1, after rejecting out-of-range index >= 10. Entries for ActionType 1,2,5,6 reach common return jump_172f9; ActionType 3 reaches func_172c1 (clear agent offset 0x10, set offset 0x06 to 0xFF, clear 0x05); ActionType 4 reaches func_172da (calls CPSetupDoMissionObjective); ActionType 7 reaches func_172d2 (copies objective byte offset 0x09 to agent byte offset 0x06); ActionTypes 8,9,10 reach func_172e5 (adds objective 16-bit offset 0x09 to level__things_base and places resulting pointer in agent offset 0x10). The return-only entries do not establish invalidity or lack of prior setup.
- CPSetupDoMissionObjective reads a 14-byte entry from level__Objectives (index multiplication x14), with max index 0x10 for condition dispatch. Both vtable_17008 and vtable_1704c have 17 entries, indices 0–16; only index 5 diverts to CPDoIOwnWeapon helpers (func_17125 or func_17161). Other values take a generic path. The function advances the agent index up to eight entries (comparison to 8 around line 9665). Numeric dispatch index meanings remain unknown.
- vtable_17090 in this same function has 17 entries. Its indices 1,2,14,15 route to func_171c7; index 5 to func_171d7; remaining indices return. The first helper recomputes the 14-byte index and falls through to the second, which converts a 16-bit field at data_9be40 into a pointer within level__things_base at agent offset 0x10. Do not infer editor semantics from this table alone.
- CPDoIOwnWeapon (9709–9738) walks a linked chain of thing offsets to find a specific 16-bit weapon reference; the chain terminates when its link field at offset 0x1C is zero. Loop integrity and malformed reference handling remain unverified.
- No game-owned objective type names or file-backed mission grammar have been established. Table dispatch alone does not prove each action type is valid as an authorable mission objective.

## Design constraints
Do not merge 15-byte CPObjectives with 14-byte level__Objectives. Do not expose numeric types as named editable actions without reference and completion chain validation. Offset-to-pointer conversions require range/structure validation in any future authoring tool.

## Next audit
Trace CPCanAgentDoMission, the completion branches and objective resource serialization; map engine-visible objective semantics to authored data only after evidence. Retain original v0.7, fixed 16 FPS, campaigns and save isolation.

## Verification
Direct static inspection of assembly. No source modifications, compilation, Windows execution, gameplay or retail asset tests.
