# MB-STRUCT-9 — Persuasion routine and last_person assignment correction
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT; no code or runtime testing
Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf

## Confirmed source evidence
- `can_i_persuad_you` begins at line 43472 and loads `last_person` into `%ebp` at line 43479.
- All six later assignments in this routine (43658, 43730, 43744, 43758, 43772 and 43781) write `%ebp` back into `last_person`. Within the inspected routine `%ebp` is retained as the saved boundary pointer. These stores alone do NOT demonstrate that the active range changes dynamically.
- The routine reads people and linked-person fields (+0x20), uses base-relative offset arithmetic against `level__things_base`, and divides by people stride 0x5c when assessing persuasion eligibility (43479–43515 and later branches). These calculations illustrate an entity-type-specific interpretation of links; they do NOT establish a general-purpose Mission Builder pointer schema.
- The previously verified initialization `set_structure_ends` stores initial `last_person`, `last_vehicle`, `last_object` based on reverse scans of fixed-capacity regions.

## Correction of earlier audit interpretation
MB-STRUCT-8 described additional stores to `last_person` as showing runtime changes. This was too strong. Their provenance is the persuasion routine, and they write back the saved entry value. Mutable-runtime-boundary behaviour remains UNPROVEN from these stores. Retain MB-STRUCT-8 for its verified locations, but use this correction when interpreting them. No general absence of other mutations is asserted without full dataflow/runtime tests.

## Mission Builder implication
- Validate every objective reference against static region, correct entity stride, sufficient field extent, active-state conditions and exact handler semantics.
- Treat a 16-bit +0x20 link as type-specific; verify target category and addressability before following. Never infer a valid person record solely from a base-relative integer.
- A future read-only inspector must not assume that persuasion function behaviour or gameplay-time `last_person` values establish asset file format.
- No authored mission writes or generic link traversal until a bounded resource parser and test fixtures are validated.

## Next focused audit
MB-STRUCT-10: determine how 16-bit references interact with the People and other entity regions, especially whether 16-bit offsets can represent all entities and which pointer encodings/record types appear in objective handlers.

## Verification
Static GitHub assembly review; no modified gameplay code, binaries, unit tests, retail-data tests or runtime verification.
