# MB-STRUCT-3 — Player objective dispatch and safe authoring boundary
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC SOURCE AUDIT ONLY — no implementation, compile or gameplay test
Baseline: v0.7 unchanged. Scope: Mission Builder player mission objectives only.

## Evidence examined
- engine/src/game_data.h defines packed CPObjective; this is a **different** system from player mission win/fail objectives (see MB-STRUCT-2).
- engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 490–510 and 659–803, 1100–1130.
- check_end_level reads a 16-bit player-objective type at data_9be3e, indexed by 14-byte stride, and dispatches entries through vtable_10550 only when type <= 16.
- Verified dispatch cases: 0 -> func_10781; 1 -> func_10790; 2 -> func_10855; 3 -> func_10889; 4 -> jump_10d3d; 5 -> func_108b8; 6–9 -> jump_10d3d; 10 -> func_10966; 11 -> func_109bf; 12–13 -> jump_10d3d; 14 -> func_10a90; 15 -> func_10ac8; 16 -> func_10b65.
- Case 0 directly marks the current objective complete (state 1).
- Cases 1, 2, 3, 5 use a 16-bit offset field (data_9be40) relative to level__things_base. Cases 2 and 3 test bit 0x1 at referenced entity offset 0x0b; case 2 marks completed when set, while case 3 calls level_failed when set. Case 1 also can call level_failed; case 5 traverses linked entity fields at 0x20. These are code observations, NOT verified friendly names such as assassination/protection/escort.
- The dispatch contains intentionally shared no-op target jump_10d3d. Do not assume these cases signify supported authorable objectives or harmless empty slots; completion state of all eight records is still tested at the end.

## Consequences for authoring
1. An editor must **not** equate a numeric type index with a documented objective design concept until all handlers and data references are decoded.
2. Validate each entity reference against the loaded mission's actual entity table and valid layout; a 16-bit offset alone does not prove safe referent or alignment.
3. Confirm the exact representation of unused objective records, initial completion state, and activation logic before offering fewer than eight editor entries.
4. Keep player mission objectives separate from CPObjective computer-player orders; no cross-population.
5. Make the first prototype a read-only inspector that reports raw type, referenced offset, completion state and validation warnings; no write or resource repackaging.
6. Preserve original retail missions, original victory/failure criteria, and between-mission-only campaign saves. No tactical save/checkpoint feature.

## Next focused audit
Trace handlers for types 10, 11, 14, 15, 16 and the secondary dispatch vtable_10594. Map exact per-record offsets/field widths and check validation possibilities against level__things_base bounds. Only then propose a human-friendly, approved editable schema; real retail assets are still absent.

## Tests and handover
GitHub source inspection only. No code edited, tests run, mission serialized, executable built or game played. This note is not evidence of working Mission Builder functionality. No rollback needed for documentation-only changes.
