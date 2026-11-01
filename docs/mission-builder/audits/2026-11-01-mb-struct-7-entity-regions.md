# MB-STRUCT-7 — Mission entity-region boundaries and reference validation
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: static source audit only; no implementation or runtime validation
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 85702–85742, 27660–27708, and objective handler lines 676–950.

## Verified static memory regions
- `level__things_base` 0x8810e, followed by `level__People` 0x88110.
- People: [0x88110,0x8dd10) = 0x5c00 bytes = 256 records * 0x5c (92 bytes).
- Vehicles: [0x8dd10,0x8e790) = 0xa80 bytes = 64 records * 0x2a (42 bytes).
- Objects: [0x8e790,0x91670) = 0x2ee0 bytes = 400 records * 0x1e (30 bytes).
- After these: `level__Weapons` 0x91670, `level__Effects` 0x95e70, `level__Commands` 0x97c70, `level__Worlds` 0x9bc70. They must not be treated as interchangeable People/Vehicle/Object records.
- All capacities derive from symbol addresses and fixed step sizes. They are **static allocation capacities**, not verified counts of active entities in a given mission.

## Dynamic boundaries
`set_structure_ends` scans backward in People/Vehicle/Object regions using the corresponding strides, examines byte +0x18, then assigns `last_person`, `last_vehicle`, `last_object` (lines 27660–27705). The routine also branches on `byte_60B42`. These pointers are dynamic usage boundaries, not necessarily array-end symbols. Their exact inclusive/exclusive meaning and invocation ordering must be validated before a parser relies on them.

## Reference-validation contract (proposed)
- Treat objective field +6 as a 16-bit offset only when a given handler actually uses it as a reference. Resolve as `level__things_base + offset`; do not assume every objective type uses the field this way.
- Check integer addition and bounds before reading entity bytes; require valid record alignment/stride and sufficient bytes for every handler-specific access (for example +0x0b, +0x1c or +0x20). Verify resolved record category and active-state rules; do not accept any address merely because it falls within the overall level memory allocation.
- Nested links at entity offsets +0x1c/+0x20 require their own checks; some may have category-specific meaning or represent flags rather than links. Validate handler semantics before treating them as universal pointers.
- Because offsets are 16-bit while these regions are large and base-relative, validate representable reference ranges; not every apparent entity can necessarily be directly addressed from the base by a 16-bit field. Avoid casting/truncation to make an otherwise unreachable entity fit.
- No write-capable Mission Builder until retail resource layout, record usage and on-disk packing are verified. Prefer a bounded read-only inspection prototype using synthetic immutable fixtures; never decompress untrusted custom data directly through legacy unbounded assembly paths.

## Next focused audit
MB-STRUCT-8: inspect objective entity reference sites and dynamic `set_structure_ends` invocation/limits, distinguish category-specific pointers and flags, establish per-type reference constraints.

## Verification
GitHub assembly source inspection and arithmetic only. No source edits, builds, Windows execution, retail assets or gameplay tests. Existing classic campaign and between-mission-only save mechanics unchanged.
