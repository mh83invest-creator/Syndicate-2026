# MB-STRUCT-8 — Entity link validation and dynamic boundary lifecycle
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: source audit only; no implementation or tests
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Findings
- `init_level_data` invokes `set_structure_ends` at line 27808, after early map/sprite and computer-player initialization (27712 onward). The function scans people, vehicle and object fixed regions backward, using +0x18 marker and respective 0x5c, 0x2a, 0x1e strides, then publishes `last_person`, `last_vehicle`, `last_object` (27660–27705).
- This is an initialization-time derivation, NOT an immutable lifetime limit: there are additional writes to `last_person` at lines 43658, 43730, 43744, 43758, 43772, 43781. Thus an inspector must distinguish static allocation bounds, initial active ranges and mutable runtime end pointers. No guarantee that a runtime snapshot equals original file contents.
- Objective type 10 and 11 handlers scan person entries against `last_person` (806–892). Other objective types dereference `level__things_base + objective field +6` and nested +0x20 links, or type-specific +0x1c values. These do not carry an explicit checked destination capacity in the inspected handler instructions (676–950).
- References to people must respect 92-byte stride and the valid people region; vehicle/object pointers must respect their respective strides. Since +0x1c may be status bits or a link depending on entity/type, a generic automatic link walker would be unsafe.
- Source-derived dynamic end values are pointer addresses, not validated counts of loaded records; their exact semantics under the alternate `byte_60B42` path and subsequent updates remain to be traced.

## Proposed parser gates (NOT IMPLEMENTED)
1. Parse a bounded immutable decompressed mission snapshot only; derive region boundaries from a verified versioned layout.
2. Resolve any 16-bit reference using widened arithmetic relative to things_base; require valid target region, record start alignment and enough bytes for the specific accessed field.
3. Validate each nested link according to the exact source handler's entity type; detect out-of-range, invalid category, self/cyclic references where prohibited, and inactive targets.
4. Treat runtime last_* pointers as advisory state until corresponding initialization/update conditions and inclusion rules are proven; never use them alone to sanction arbitrary reference reads.
5. Keep player objectives distinct from computer-player order records. Reject unsupported objective semantic editing and preserve original tactical rules.

## Remaining questions
Audit `last_person` mutation branches and exact object/vehicle reference encoding; verify how original decompressed asset bytes map to runtime entity arrays. Synthetic tests, asset validation and live Windows tests are not performed.

## Verification
Static examination only. No source or binary changes; no Windows build, asset fixture or gameplay test. Original missions and between-mission save-only rule preserved.
