# MB-STRUCT-24 — Non-destructive custom campaign deployment and rollback contract

Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: DOCUMENTATION / SOURCE AUDIT ONLY; no installer, parser or deployment implementation.

## Source evidence
- The original `engine/util/install` rejects nonempty TARGET directories then sets `OWN_TARGET=yes` after `install -d` (89–100); its `error` handler removes TARGET with `rm -rf` whenever that flag is set (342–349). This ownership flag does not establish ownership of every target file or a safe rollback ledger.
- Its `extract_packages` runs `unzip -o`, uses `mv -vn` for selected nested files, and removes temporary subdirectories (274–292). These operations do not constitute an atomic release/rollback transaction.
- `copy_data` requires a fixed native resource list (MB-STRUCT-20) and the legacy path resolver is not a confinement validator (MB-STRUCT-22/23).
- No dedicated custom campaign package owner registry, immutable content manifest or transactional publish/rollback implementation was established in this audited path. This does not prove no such external utility could exist.

## Proposed acceptance contract (NOT IMPLEMENTED)
1. **Namespace:** Each custom campaign has a stable ID and independent resource root, save silo and manifest. Do not use the native `data/gameNN.dat` namespace or original campaign paths. Exact directory layout and ID encoding remain design decisions.
2. **Preflight:** Validate package format, manifest version, declared campaign identity, byte lengths, cryptographic hashes, allowed relative paths, canonical containment, path collisions (including case-insensitive), symlinks, destination ownership, dependencies and bounded storage before mutating anything.
3. **Stage:** Extract only to a newly created disposable directory specifically owned by the transaction; never stage inside an original game or existing campaign data root without confinement checks.
4. **Publish:** Only expose the new campaign when the complete staged content passes validation. A final rename/commit must be constrained by platform/filesystem atomicity; no assumption that cross-volume rename is atomic. Preserve an existing installed version until the new version is validated.
5. **Failure:** On any error, only remove newly created transaction-owned staging artifacts. Restore/retain the prior visible version and hashes; do not remove original game files, another custom campaign, unknown files or any save data.
6. **Upgrade/uninstall:** Require verified owner manifest and exact selected campaign identity. Refuse ambiguous ownership or externally modified files rather than deleting or overwriting them. Leave saved games intact unless the user independently authorizes their deletion.
7. **Recovery:** A crash/interrupted operation must not expose an incomplete campaign; stale staging should be quarantined or cleaned only with provable transaction ownership.
8. **Diagnostics:** Report campaign ID, phase, failed path/rule and whether publication occurred. Never report success merely because some files were copied.

## Future test matrix, not executed
- Successful new installation with complete manifest; byte-for-byte source preservation.
- Missing file, hash mismatch, oversized file, corrupt metadata, casefold duplicate, path traversal, absolute destination and symlink escape.
- Existing campaign name/path collision; existing owned and unknown files; wrong campaign identity; stale staging.
- Disk full and permissions failures during preflight/stage/publish; interruption before/after publication.
- Retry after failure, upgrade, uninstall, rollback, failed cross-volume publication; ensure original game and other campaign hashes unchanged.
- Ensure save list/load/continue remain campaign-isolated and saving is only between missions, with no changes to original gameplay.

## Source vs proposal
The checks above are design gates, **not** existing game features and not independently approved installer architecture. Implementation requires review with the core development workstream and runtime testing on supported systems, including Windows, before distribution.

## Next
MB-STRUCT-25 — prepare a scoped Mission Builder release-readiness checklist integrating source-backed resource/validator/packaging gates, and identify unresolved blockers without altering the baseline.

## Verification
Original installer source inspected via GitHub and previous MB-STRUCT notes cross-referenced. No file deletion, installer execution, code edits, synthetic tests, build, original-data tests or gameplay tests.
