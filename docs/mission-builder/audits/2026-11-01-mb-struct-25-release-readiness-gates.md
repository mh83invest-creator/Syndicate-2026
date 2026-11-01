# MB-STRUCT-25 — Mission Builder implementation/release readiness gate register
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder only. STATUS: AUDIT AND PLANNING; NO RELEASE OR IMPLEMENTATION.

## Evidence consolidated
MB-STRUCT-2–12 trace the native eight-by-14-byte player objective table, source handler dispatch, memory offsets, entity regions and reference rules. MB-STRUCT-13 defines proposed synthetic fixture acceptance. MB-STRUCT-14–17 establish legacy RNC caveats, `data/gameNN.dat` resource naming, two-digit number constraints and city-to-mission mapping. MB-STRUCT-18–20 record the unverified American Revolt resource switch, divergent branch documentation and installer-listed native files. MB-STRUCT-21–24 establish that legacy installation/path compatibility helpers are not a safe custom-pack importer; proposed package rejection and rollback requirements remain unimplemented.

## Readiness gates — all OPEN unless marked
| Gate | Required proof/deliverable | Status |
|---|---|---|
| R01 Native layout | Verify compressed and decompressed mission sizes, record bounds, file version and entity reference semantics using authorised fixtures | BLOCKED: no verified retail resource data |
| R02 Safe decode | Bounded decompression independent of legacy unbounded RNC path; corrupt/short input rejection | NOT IMPLEMENTED |
| R03 Read-only inspector | Immutable bounded parser for 8 objective records, type-specific reference validation and diagnostics | NOT IMPLEMENTED |
| R04 Synthetic test suite | F0–F7 fixtures, bad offset/sentinel/cross-reference cases, input non-mutation and results | NOT IMPLEMENTED |
| R05 Semantic coverage | Verify objective state/unused slots and handler category/flag meaning against actual missions | BLOCKED on source/asset validation |
| R06 Campaign identity | Locate/verify Original and American Revolt selection and resource roots; preserve original binaries | UNVERIFIED |
| R07 Custom campaign routing | Distinct campaign-local mission identity, separate resource namespaces and Original/Revolt/World Domination/custom mode selection | NOT IMPLEMENTED; interface design subject to approval |
| R08 Save isolation | Independent namespace for each custom campaign and each existing mode, with legacy compatibility and between-mission-only saving | NOT IMPLEMENTED; main branch coordination required |
| R09 Package format | Explicit identity/version, manifest/hash/path policy and non-clobbering unpack | NOT SPECIFIED/IMPLEMENTED |
| R10 Safe publish/rollback | Contained staging, platform-safe publication, owner-scoped cleanup/upgrade/uninstall, no loss of saves | NOT IMPLEMENTED |
| R11 Integration/review | Synchronised cross-branch contract, source review, Windows build, actual retail asset validation and gameplay regression | NOT PERFORMED |
| R12 Handover | Source-backed sequential audit notes and cross-workstream handover | COMPLETED FOR MB-STRUCT-2–25 only; not a build-completion gate |

## Priority and safe implementation sequence (proposal only)
1. Reconcile campaign identity / resource and save boundary with main development (R06/R08); keep per-custom-campaign save isolation as approved.
2. Obtain authorised disposable game resource fixtures and verify actual RNC image metadata, objective and entity layouts (R01/R05).
3. Specify independent bounded decoder/parser and fixture oracle; implement and test **read-only** tools (R02–R04), preserving baseline.
4. Confirm resource pack manifest, namespace and transaction design with stakeholder review before any installer/editor writer (R07/R09/R10).
5. After passing source/runtime gates, separately approve a write-capable editor, campaign chooser and deployment. No unapproved code changes in audit stage.

## Invariants
No original `data/gameNN.dat` overwrite. No conversion/migration/deletion of existing saves. No in-mission saves; Escape's exact effect requires runtime verification. Original / American Revolt / World Domination and each Custom Campaign are separate selectable save silos. Do not alter graphics branch, engine baseline or other audits.

## Verification
Read prior GitHub MB audit artifacts and source-backed conclusions. No code, new fixture files, builds, Windows execution, game data or automated tests. This is a gate registry, **not** a claim of release readiness.

## Next
MB-STRUCT-26: perform narrow source-backed verification of the highest-priority unresolved native mission layout details and prepare an actionable fixture request/acceptance form, without substituting synthetic data for real evidence.
