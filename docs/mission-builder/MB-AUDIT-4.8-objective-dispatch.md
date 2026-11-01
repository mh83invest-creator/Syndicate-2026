# MB-AUDIT-4.8 — CP objective dispatch boundary (2026-11-01)

Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 9364–9485, 9600–9665, 9773–9825, 85788–85803.
Mode: documentation-only static source audit.

## Verified observations
- level__CPObjectives at line 85798 allocates 0x780 = 1,920 bytes, matching 128 records of 15 bytes each. This is an existing static capacity, not a verified editor authoring limit.
- CPGetObjective at 9364–9485 scans 15-byte records, reads flags at offset 0x06 and a field at 0x02, and calls CPCanAgentDoMission. Its fallback creates a descriptor with Flags=0 at offset 0x06, ActionType=1 at offset 0x07, Action=3 at offset 0x08 and zeros at offsets 0x09, 0x0B, 0x0D. Meaning of action 3 is unverified.
- CPSetupNewObjective at 9773–9825 dispatches by ActionType byte minus one through a 10-index jump table. Observed helpers reset agent state, copy offset 0x09 to agent field, invoke CPSetupDoMissionObjective or interpret offset 0x09 as an offset into level__things_base. The numeric range does not prove all values valid for user-generated missions.
- CPSetupDoMissionObjective at 9600–9665 instead indexes level__Objectives using a 14-byte stride and dispatches through vtable_17008 or vtable_1704c for values <= 0x10. It contains CPDoIOwnWeapon checks. This is a distinct objective representation; do not conflate its 14-byte records with 15-byte CPObjectives.

## Implications
- Do not hard-code a user-visible objective type list from raw numeric ranges.
- Validate separate structural representations and target offset semantics before editing or serializing objectives.
- Do not generate custom mission files or patch game logic based on this partial mapping.

## Next steps
1. Decode vtable_17278 case mappings for ActionType 1–10.
2. Decode vtable_17008 and vtable_1704c, with level__Objectives field layout.
3. Read CPCanAgentDoMission and objective caller/completion chains.
4. Validate actual file-backed objective records using legally provided game data before writing an editor schema.

## Verification
Direct static assembly inspection only. No game code edits, builds, gameplay tests, save or asset changes. Keep v0.7, fixed 16 FPS, classic modes and campaign save isolation intact.