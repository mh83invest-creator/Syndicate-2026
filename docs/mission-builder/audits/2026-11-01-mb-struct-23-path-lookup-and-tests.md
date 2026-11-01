# MB-STRUCT-23 — Native path lookup and existing test coverage

Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: static source audit; no tests executed or code changed.

## Verified native path handling
- `engine/src/dos.c:lookup_path` (291–398) performs component-by-component case-insensitive directory lookup using `find_file_in_directory` (277–288). It accepts absolute paths (306–329) and does not enforce a custom-campaign directory root.
- If a component does not exist, the routine may lowercase remaining portions and return success (323–339, 367–385), making success mean a converted path was produced, **not** that a safe or existing destination was verified.
- The empty string path returns true at 298–299 without visibly initialising output in this function. Callers must not equate successful return with a usable path; runtime consequences are untested.
- Existing code uses `snprintf`, `strncat` and a `FILENAME_MAX` initial-length check, but these observations are not proof against traversal, symlinks, case folding collisions, truncation or destination escape.
- `dos_path_to_native` (400–419) replaces backslashes and invokes `lookup_path`; it is a compatibility path resolver, not an importer sandbox.
- A repository tree inventory shows `engine/bflibrary/tests/` with math, graphics and memory test sources; **no dedicated custom campaign importer/path confinement test** is established from the inspected tree. This is a bounded inventory finding, not a claim that no path tests exist anywhere.

## Additional proposed negative cases — unexecuted
- Lookup success with nonexistent segment must not authorise publication.
- Empty path, a canonicalised `..` chain, and absolute path must reject at the custom package boundary.
- Case-insensitive collision across source files; mixed-case resolved destination differing from manifest; symlink-redirected destination; existing file not owned by campaign all reject.
- Any apparent truncation of a resolved path must reject, not silently refer to another file.
- Identical normalized path supplied twice or with slash variants must reject.
- Preserve original resources and per-campaign saves across failed staging, restart and retry. Run disposable before/after-hash tests only after implementation approval.

## Design gate
A separate, explicit package path validator must accept only a canonical relative path under a uniquely owned custom-campaign staging root. It must verify post-resolution confinement, absence of symlink escape, normalized collisions, platform-reserved filenames and file ownership before any write. Do not transplant the legacy `lookup_path` return semantics into the package verifier.

## Next
MB-STRUCT-24: inspect package/deployment directory APIs and installer ownership semantics sufficiently to write a bounded, non-destructive release/rollback specification; no implementation or new functionality.

## Verification
GitHub source/path tree inspection only. No fixture creation, automated tests, install, Windows build, retail file use or gameplay testing.
