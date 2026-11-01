# Mission Builder — Objective target reference validation (2026-11-01)

Status: verified read-only fixture tests; no editor writer or game changes.
Branch: feature/mission-builder-audit.

## Input and source
Private supplied prototype ZIP with 60 gameNN.dat resources. No asset data committed.
engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf: entity-pool boundaries at 85710–85736; check_end_level target dereferences near 678–754. level__things_base is at 0x8810e.

## Relative entity pool boundaries and strides
- People: start 2; next region at 23554; record size 92; 256 candidate records.
- Vehicles: start 23554; next region at 26242; record size 42; 64 candidate records.
- Weapons: start 38242; next region at 56674; record size 36; 512 candidate records.
- These relative offsets are relative to level__things_base, not level__Seed; they are not raw compressed-file offsets.
- The type-to-pool associations below are grounded by observed archive targets and source-level objective evaluation references, not by user-facing type descriptions.

## Tests executed
All 60 user-supplied missions decoded with bounded read-only RNC routine, then parsed by mission_inspector. Applied pool interval and stride alignment to every nonzero tactical target for these types:
- Types 1, 2, 3: People; 25 + 22 + 5 = 52 checked.
- Type 5: Weapons; 4 checked.
- Types 14, 15: Vehicles; 8 + 3 = 11 checked.
- Total: 67/67 fit source-established pool bounds and record alignment, zero failures.
- Test file: tools/mission_builder/test_mission_references.py; local matching script executed on supplied ZIP and passed.

## Interpretation limit
An aligned reference does not establish entity activation, valid class/state, campaign meaning, objective achievability or complete cross-reference correctness. Other objective types, CP linked chains, and zero target semantics are not validated by this test. Do not enable write-capable editing from this check alone.

## Verification and preservation
Static source boundary inspection and read-only local fixture tests only. GitHub test source committed; post-upload execution of the GitHub copy not performed. No game code, source assets, graphics, saves or protected v0.7 modified. No Windows gameplay test.
