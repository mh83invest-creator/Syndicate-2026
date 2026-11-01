# MB-STRUCT-12 — Objective sentinel audit and read-only validation matrix
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC SOURCE AUDIT ONLY — no code, builds or executable tests
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; check_end_level dispatch lines 490–528 and 659–1103.

## Sentinel inspection
Reviewed literal tests and zero/reference behaviour across primary and secondary player objective handlers. No explicit comparison against 0xffff or 0xfffe was found in the inspected objective handler/dispatch ranges. Occurrences of `and $0xffff` in types 1 and 5 are unsigned-width masks, NOT sentinel checks. **This does not prove 0xffff is invalid or unused globally**; do not translate it to null, a valid entity or an editor value without retail fixture and full-consumer evidence.

## Per-type reader matrix (observed; semantic names NOT approved)
| Type | Primary objective +6 | Additional dependency | Recommended inspector disposition |
|---|---|---|---|
| 0 | Not dereferenced by primary completion handler | State set to complete | Read-only known handler; unused-slot meaning unknown |
| 1 | Required base-relative entity reference; unguarded | Optional +0x20 link chain, 0 ends traversal; failure condition | Validate entity category, chain and status |
| 2 | Required unguarded entity reference | +0x0b bit 1 state | Validate reference and state |
| 3 | Required unguarded entity reference | +0x0b bit 1 triggers failure | Validate reference and state |
| 4 | Not dereferenced by primary dispatch | Shared fallback branch | Unknown semantics; do not author |
| 5 | Required unguarded entity reference | Optional +0x20 link chain, 0 ends traversal; secondary type-16 checks | Validate chain, status and cross-objective references |
| 6–9 | Not dereferenced by primary dispatch | Shared fallback branch | Unknown semantics; do not author |
| 10 | No +6 primary read in reviewed handler | Iterates person records | Validate people region/capacity and state; naming unknown |
| 11 | No +6 primary read in reviewed handler | Person scan and linked +0x20 entity | Validate linked references; naming unknown |
| 12–13 | Not dereferenced by primary dispatch | Shared fallback branch | Unknown semantics; do not author |
| 14 | Required unguarded +6 entity reference | +0x0b bit 1 completion | Validate reference/state |
| 15 | Required unguarded +6 entity reference | Contextual +0x1c word, completion/failed conditions | Validate source category, nested link and state |
| 16 | Coordinates/parameters at +8/+10 | People scan and second dispatch for types 1,5 | Validate coordinates, dependency set and valid entity refs in other entries |

## Rules before a synthetic read-only parser
1. Parse at most eight 14-byte player objective records from a validated decompressed, bounded mission snapshot with explicit format/version and length. Reject truncated input before any record reads.
2. Treat type index 0–16 as dispatch indices, not documented user-friendly mission goals; report unknown semantics without executing code.
3. Do not use generic zero/null or 0xffff/sentinel rewriting. Type 1/5 +0x20 chain zero has observed stop meaning; the primary +6 reference is not optional merely because it is a 16-bit word.
4. Validate every entity reference for exact source type, region, record alignment and accessed-field extent; nested links independently validated.
5. Detect unsupported reference combinations and inter-objective dependencies before offering an editor. Maintain original campaign state, mission end conditions and between-mission-only saves.
6. Preserve separate AI CPObjective[128] records; never map them into eight player objective entries.

## Remaining gaps
Unverified on-disk RNC mission representation and decompressed length, special sentinel conventions outside inspected handlers, objective record initialisation and unused entries, entity category/flag names, safe writer output, and retail mission fixtures.

## Verification
Read-only assembly inspection and matrix documentation. No test harness, game code patch, CI build, retail asset or gameplay test. Next MB-STRUCT-13: choose a documented fixture strategy and explicit parser acceptance cases without constructing an unverified production writer.
