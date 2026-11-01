# MB-AUDIT-6.1 — Objective field mutation inventory
Date: 2026-11-01. Scope: static audit only.

Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified mutations
- init_computer_players lines 7052–7078 clears CP objective flags bit 0x10 for 128 records at record offset 0x06.
- Objective processing at 9260–9289 sets flags bit 0x10 along a CP objective linked chain. It also resets several agent-specific state fields.
- CPGetObjective fallback at 9457–9467 writes CP objective record zero: Flags=0, ActionType=1, Action=3, and clears fields at offsets 0x09, 0x0B and 0x0D. This is a runtime-generated fallback, not confirmed author-facing semantics.
- CPGetObjective assignment at 9469–9480 increments and writes the byte at CP objective offset 0x04 when assigning a selected objective to an agent.
- CPDoTurn at 7762–7783 can switch an agent to another linked objective and dispatch its setup logic.
- Tactical level__Objectives status dwords are updated during check_end_level (e.g. 672, 740, 801, 1100).
- The inspected code does not prove that all fields are persisted unchanged in original mission files or that loaded objective records are safe to reserialize.

## Next evidence
Trace the complete CPCompletedObjectiveStep chain and identify additional CP flags/target updates; distinguish runtime-owned fields from necessary authored defaults using lawful retail mission samples. Do not implement an editor writer from this inventory alone.

## Verification
Source inspection only; no changes to code, assets, saves, builds or gameplay. Preserve protected baseline, fixed 16 FPS and original campaigns.