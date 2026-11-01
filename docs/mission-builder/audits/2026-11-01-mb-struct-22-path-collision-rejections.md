# MB-STRUCT-22 — Cross-platform path collision and package rejection contract

Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC SOURCE AUDIT AND PROPOSED TEST CONTRACT ONLY.

## Verified source
- `engine/src/game.c` lines 114–138: `game_transform_path_full` matches `save/` case-insensitively, routes these paths under `GetDirectoryUser()`, passes apparent absolute paths through unchanged, and routes other relative paths under `GetDirectoryHdd()`. This is **not** a package path sandbox. Comment "abort on absolute paths" does not match the shown pass-through branch: the function itself does not abort.
- `engine/src/dos.c` lines 400–419: `dos_path_to_native` rejects a string whose length reaches `FILENAME_MAX`, translates backslashes to native separator and performs `lookup_path`; it does not establish a custom package confinement policy.
- `engine/util/install` lines 134–169: source file lookup uses `find -ipath` (case-insensitive), copies directly to target path, and does not have a dedicated per-pack collision/symlink/traversal validator. Existing base-installer target handling and destructive cleanup risks documented in MB-STRUCT-21 remain.
- A future custom-pack loader cannot rely on these legacy convenience routines as a secure package importer. The precise cross-platform symlink/canonicalisation semantics of `lookup_path` remain unverified from this inspection.

## Proposed custom package collision-rejection cases (NOT EXECUTED)
| Case | Input / situation | Expected disposition |
|---|---|---|
| P01 | Canonical relative path within a new, isolated custom campaign root | Accept for staging only after full manifest validation |
| P02 | `../`, `./../`, mixed slash/backslash parent traversal | Reject before writing |
| P03 | Absolute POSIX, Windows drive path, or UNC/network path | Reject |
| P04 | `save/` or case variants of it as package asset path | Reject reserved save namespace |
| P05 | `data/game01.dat` or any legacy `gameNN.dat` destination | Reject classic mission collision |
| P06 | Duplicate paths differing only by case or slash normalization | Reject as ambiguous |
| P07 | Symlink file or directory escaping staging root | Reject after canonical containment check |
| P08 | Existing path owned by another campaign or unknown source | Reject, never overwrite |
| P09 | Empty, trailing-dot/space, reserved device names, control/NUL characters | Reject or require platform-specific validated encoding |
| P10 | Path exceeding safe platform/container length or file-count/size policy | Reject |
| P11 | Manifest references missing files, bad hashes, repeated entries | Reject atomically, no publication |
| P12 | Interrupted staging/publish, permission error or full disk | Preserve existing packs and saves; clean up only new, proven-owned staging artifacts |
| P13 | Case-insensitive filesystem collision that would not collide on Linux | Reject for portable package safety |
| P14 | Cross-campaign identity mismatch between manifest and selected destination | Reject |

## Implementation gates
- Confirm allowed namespace, versioned manifest, exact path policy, ownership metadata and update/uninstall workflow through later explicit development approval; none are implemented here.
- Use a dedicated package confinement validation layer, not `game_transform_path_full` or the original installer as a permission boundary.
- For testing, create disposable temporary directories and canary files, verify before/after hashes, and check no mutation of original `data/`, legacy `save/`, American Revolt assets or other custom campaign roots.
- Protect separate selectable Original, American Revolt, World Domination and each Custom Campaign, with separate between-mission saves only.

## Next
MB-STRUCT-23: inspect `lookup_path` and path normalization routines plus relevant path-test coverage in repository, and narrow the portability acceptance gates.

## Verification
Reviewed repository source only. These are proposed negative tests, not run results. No source changes, package implementation, installer execution, builds, retail file or gameplay tests.
