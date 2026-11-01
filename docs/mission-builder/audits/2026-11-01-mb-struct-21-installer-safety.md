# MB-STRUCT-21 — Installer missing-file, overwrite and rollback behaviour

Date: 2026-11-01 (Australia/Brisbane)
Status: Mission Builder STATIC AUDIT / DESIGN CONSTRAINTS ONLY.
Source: engine/util/install on feature/mission-builder-audit, lines 62–101, 134–180, 183–269, 274–294, 332–360.

## Source-verified behaviour
- `check_target` rejects an existing nonempty TARGET at lines 89–95, then creates TARGET and sets `OWN_TARGET=yes` at 98–99. This is an installation precondition, not an approved custom pack transaction model.
- `install_file` / `install_file_to` (134–169) search SOURCE ignoring case; missing source file yields a diagnostic and a nonzero return. Files are installed directly with `install -v -T -m 0644`; there is no per-file collision or content-hash verification inside these functions.
- Native `copy_data` lists tactical `gameNN.dat` files, maps and mission text and returns upon the first failed mandatory file copy. This does not prove original files are all present in every supplied retail edition.
- `extract_packages` uses `unzip -o` into TARGET, then `mv -vn` into selected destination directories (274–292). This mixes overwrite-capable extraction with no-clobber moves; it is not an atomic rollback strategy.
- `error` (342–349) calls `rm -rf "$TARGET"` if `OWN_TARGET=yes`, and `check_target` sets this flag for an accepted target (even one that existed but was empty). This is destructive cleanup of the designated target after failure. It is **not** a safe custom campaign installer for existing game folders.
- `create_run_script` writes a launch script for the base game, not a campaign-specific launcher.

## Mission Builder distribution contract — proposed, NOT implemented
1. Never run this legacy installer against an existing player's game directory for custom campaign deployment.
2. Custom campaign packages must have explicit identity and version, an independent resource namespace, a manifest of files and hashes, and a destination path that does not overlap classic game files. The exact package schema/path is **not yet approved**.
3. Reject package paths containing traversal, absolute paths, cross-campaign namespace overlaps, reserved classic resource names, duplicate case-insensitive paths, and conflicting existing files.
4. Verify all files and dependencies before making any changes; stage into a new isolated destination, and require atomic/rollback-safe publication without deleting unrelated files. Preserve pre-existing target content on all failures.
5. Uninstall/update behaviour must affect only files proven to belong to that custom campaign; original game and other campaigns/saves are protected.
6. No installer or resource routing implementation until original and American Revolt resources and campaign selector boundaries are verified. Each custom campaign retains its own save silo, with saving only between missions.

## Verification and limitations
Performed source read of installer and source-path behaviour only. No installer execution, disposable-install test, filesystem mutations, generated archive, native build, gameplay or retail asset inspection. A source-observed destructive cleanup path is not a claim of a reproduced deletion. No engine changes.
Next: MB-STRUCT-22 — inspect cross-platform file/path handling and draft concrete collision rejection test cases, keeping documentation-only scope.
