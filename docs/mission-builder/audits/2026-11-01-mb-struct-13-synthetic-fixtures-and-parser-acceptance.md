# MB-STRUCT-13 — Synthetic fixture strategy and read-only parser acceptance contract

Date: 2026-11-01 (Australia/Brisbane)  
Workstream: Mission Builder; branch `feature/mission-builder-audit`  
Status: SPECIFICATION / STATIC AUDIT ONLY — no parser, fixtures, automated tests or game changes implemented.

## Scope and protected baseline

This contract defines a *future* bounded, non-executing inspector for native mission objective records. It does not authorize a writer, decompressor replacement, campaign mode selector, new gameplay features or modifications to original mission resources. Preserve original gameplay and classic campaign resource handling, and save campaigns **between missions only**; no tactical checkpoint/save feature.

Evidence: prior MB-STRUCT-2 through MB-STRUCT-12 on this branch, especially `2026-11-01-mb-struct-12-sentinel-validation-matrix.md`.

## Source-backed provisional memory layout

- Assembly `level__Seed`: 0x80108; `level__Objectives`: 0x9be3a; `level__ComputerPlayersCount`: 0x9beaa.
- Derived objective table offset from Seed: 0x1bd32 = 113970 decimal.
- Eight records * 14 bytes = 112 bytes; minimum memory-image buffer extent to read the whole table from that origin = 114082 bytes.
- Per record: +0..3 state, +4..5 dispatch type, +6..7 context-dependent entity offset/parameter, +8..9 parameter, +10..11 parameter, +12..13 used parameter. Layout is a **provisional memory-image interpretation**. It is NOT a verified gameNN.dat file offset or proof that retail RNC output reaches this size.
- Native processor's fixed entity regions and 16-bit base-relative offsets were mapped previously; exact entity category/active semantics and retail input formats remain unverified.

## Fixture groups (to implement later, not yet created)

**F0 — Structural baseline:** Construct an explicitly synthetic, immutable memory-image buffer with the documented objective region present; seed eight 14-byte records with known raw byte patterns to confirm offsets, ordering and endian assumptions. This fixture must carry a synthetic-layout marker and must never be described as a valid retail mission.

**F1 — Bounds and truncation:** Cover empty, 1-byte, `offset-1`, `offset`, `minimum-1`, `minimum` and `minimum+1` input lengths. All below `minimum` must reject *before* any objective read; exactly `minimum` may allow structural parsing only, not gameplay validation.

**F2 — Record values:** Cover dispatch types 0, 1, 2, 3, 4, 5, 6–9, 10, 11, 12–13, 14, 15, 16, 17 and 0xffff. Preserve raw values, distinguish observed handlers from unsupported or semantically unknown cases, and avoid assigning friendly mission-goal names. A value above 16 must not index the dispatch table.

**F3 — Required primary references:** For types 1, 2, 3, 5, 14 and 15, test zero; first valid record start; last valid record start; one-byte misalignment; immediately before/after an entity region; an address within another category; and a 0xffff offset. Verify category-specific field extent and alignment; do not accept every address merely because it falls inside the 16-bit addressable span.

**F4 — Context-specific nested links:** For types 1 and 5, exercise zero `+0x20` termination, one and two valid links, invalid link targets, and repeated/cyclic values. For type 15, test `+0x1c` zero and nonzero separately, without treating that field as a universal link across entity categories. Fail or flag unresolved semantics as specified by a verified handler rule, not invented global rules.

**F5 — Cross-objective relationships:** Exercise type 16 with combinations of type 1 and type 5 and coordinates at +8/+10, including missing, malformed and unsupported companion records. Parsing must not execute `getrdist`, `level_complete`, `level_failed` or any engine routine.

**F6 — Integrity/non-mutation:** Hash input bytes before and after inspection; repeat validation, change record ordering, run multiple files in sequence and verify no persistent state contamination. Confirm all failures leave the source unchanged and return stable diagnostics.

**F7 — Input provenance:** Feed an apparently valid synthetic memory image, a raw/unrecognized file, a truncated RNC-like header, a malformed compressed container and a genuine retail-data fixture (when legally supplied and separately validated). The first stage accepts only explicitly identified, bounded decompressed memory images of a verified layout; compressed assets are not sent to the legacy unbounded loader.

## Proposed acceptance outcomes

- **REJECT:** Missing/unsupported input-format evidence, insufficient length, out-of-region or misaligned required references, invalid field extent, or unsafe nested-link traversal. Reject before dereference; no partial parsed-object publication.
- **STRUCTURALLY READABLE / SEMANTICS UNVERIFIED:** All objective bytes readable but type, entity classification, unused-slot behaviour or mission-winning interpretation cannot be confirmed. Report raw values and warnings; do not mark playable or editable.
- **STRUCTURALLY VALIDATED:** Byte bounds, record decoding and relevant references satisfy the verified layout and handler rules. This is **not** a claim that the mission can be played or completed.

Every diagnostic should carry record index (0–7), field offset, raw value, validation rule, provenance and severity. Unknowns must remain labelled unknown, not automatically rejected as corrupt retail content or accepted as safe authoring instructions.

## Future implementation and verification gates

1. Independently verify true decompressed payload layout, byte order, actual minimum length and RNC container bounds with disposable legal fixtures or vetted reference data.
2. Define versioned fixture manifest and test-oracle results before implementing a parser.
3. Implement a read-only, non-engine inspector that consumes a caller-validated immutable bounded byte slice; do not reuse the legacy unbounded RNC assembly loader for custom files.
4. Run automated negative cases, memory-safety checks and input-nonmutation tests; separately audit behaviour with actual retail resources.
5. Only after successful source, fixture and runtime verification, seek approval before developing any write-capable mission builder. Keep cross-branch handover communication with main development and graphics where resources overlap.

## Verification and handover

Performed: consolidated prior source-backed Mission Builder constraints into a test specification; checked the existing MB-STRUCT-12 matrix and branch handover. No source code or test fixture files created, no automated tests run, no build, no retail-asset loading and no Windows gameplay verification. Next: MB-STRUCT-14 should audit the source-backed mission-image/RNC boundary needed before fixtures can be treated as production-format tests, without implementing unapproved gameplay changes.
