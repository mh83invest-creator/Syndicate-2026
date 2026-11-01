# MB-AUDIT-5.1 — Tactical objective evaluator dispatch (2026-11-01)

Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 491–527 and 530–1136.
Status: documentation-only static inspection.

## Evaluator routing

check_end_level uses 8 entries of 14 bytes in level__Objectives and dispatches on a 16-bit code at entry offset 4 (data_9be3e), accepting numeric codes 0..16. vtable_10550 at lines 491–508 maps:
0 -> func_10781 (sets status dword to 1);
1 -> func_10790 (entity status/linked-person checks, with a level_failed call);
2 -> func_10855 (referenced entity flag 0x1 at offset 0x0B sets status 1);
3 -> func_10889 (same referenced flag can invoke level_failed);
4 -> generic advance;
5 -> func_108b8 (linked entity/player agent check);
6,7,8,9 -> generic advance;
10 -> func_10966 (scans person records and can mark complete);
11 -> func_109bf (scans person state and linked player-agent relationships);
12,13 -> generic advance;
14 -> func_10a90 (entity status check and status update);
15 -> func_10ac8 (linked player agent check plus direct failure path);
16 -> func_10b65 (player-agent coordinates and proximity/distance logic; additional checks use vtable_10594).

vtable_10594 (lines 510–527), used within type 16, handles subcondition indices 1 and 5 with distance routines; others fall through. Numeric codes are not validated player-facing mission names. The documented actions summarize the observed branches, not complete data semantics.

At lines 1112–1130, eight status dwords (14-byte stride, total 112 bytes) are compared to 1. If all equal 1, level_complete is invoked; the source also has a mission-cheat shortcut. Source inspection alone does not establish how unused entries are initialised in loaded retail mission files. Special handlers may call level_failed independently, and the outer function handles prior terminal flags.

## Required cautions
- Do not label type codes Assassinate/Escort/etc until object roles and retail data are verified.
- A writer must preserve and validate all eight entries, including unused entries; the proper sentinel/initial state is unverified.
- Entity offsets, linked chains, ownership, status flags, and spatial units require independent validation.
- The 14-byte tactical objective entries must not be conflated with 15-byte CPObjectives.
- No implementation or gameplay test is authorised by this static mapping.

## Next
Trace level objective initialisation and the file serialization relationship for all eight entries. Then correlate code meanings with legally supplied sample missions and runtime outcomes.

## Verification
Read assembly branch table and handlers in source. No source modifications, builds, Windows runtime tests, assets, save changes or Mission Builder implementation. Protected original/American Revolt campaigns, fixed 16 FPS, v0.7 and independent save requirement unchanged.
