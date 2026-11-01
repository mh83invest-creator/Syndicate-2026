# Mission Builder — Entity records and CP link chains (2026-11-01)

Status: read-only local validation against 60 user-supplied mission resources. Branch: feature/mission-builder-audit.

## Method
Source boundaries from engine/src/syndre.sx: level__things_base at 0x8810E (offset 0x8006 from level__Seed), People beginning at relative +2 with 92-byte stride, Vehicles at +23554 with 42-byte stride, Weapons at +38242 with 36-byte stride. The referenced record byte at offset +0x18 is compared against observed source-coded entity type (People=1, Vehicles=2, Weapons=4); this is a *type consistency check*, not a check of entity activity.

All 60 mission files were decoded read-only, and 128 CP objectives plus 8 tactical objectives parsed from each. Nonzero CP child and parent fields were treated as candidate record indices; 0 is sentinel. Followed child chains, detected repeated indices as cycles, recorded mismatched reverse links without correcting them. Tactical objective types 1,2,3,5,14,15 were checked for valid pool alignment and type byte.

## Observations (executed locally)
- 60 missions / 7680 CP records.
- 123 nonzero child edges, 120 nonzero parent edges; no out-of-range indices.
- Zero forward child-chain cycles found among the 60 samples.
- Two valid-source one-way child edges: game01.dat slot 4 -> slot 1 (slot 1 parent=0); game19.dat same case. These are not automatically errors; original resource compatibility requires permitting or explaining them.
- All 67 selected nonzero tactical references remain pool-aligned. No nonzero unexpected entity-type bytes were found.
- Exactly one target references an entity with a zero type byte: game94.dat tactical objective slot 0, type 2, offset 14538. Whether this is an intentional inactive/spawned-later target is unknown.
- A local Python test with assertion gates for these aggregate counts passed. Corresponding GitHub regression script: tools/mission_builder/test_entity_chains.py (source-matched implementation, GitHub copy not independently executed after commit).

## Consequences
Don't enforce reciprocal CP links or a nonzero entity type for every objective as an editor rule before the engine's handling is verified. Only out-of-range CP indices, known invalid offsets and actual decoded-size/CRC failures are demonstrated candidates for hard rejection. Record anomalies as diagnostic warnings with filename and slot, not automatic repairs. No campaign/mission writer is authorised.

## Limitations
A clean pool-alignment and type-byte result does NOT verify entity active state, objective achievability, target-class validity across all action handlers, CP list termination under runtime changes, map passability or Windows gameplay. The test covers the supplied 60 files only. Commercial resource bytes are not committed.

## Preservation
Documentation and test code only; no engine, original assets, graphics, saves, or protected v0.7 baseline changed.
