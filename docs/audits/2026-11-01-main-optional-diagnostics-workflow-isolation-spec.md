# Syndicate 2026 — Main Systems Audit: optional diagnostic workflow isolation specification

Date: 2026-11-01 (Australia/Brisbane). Main Systems, documentation-only on default branch.

## Current source basis
Verified live GitHub file reads:
- `.github/workflows/windows-source-v0.7.yml`, blob `ab9c099923d859e85f996e0c11823cf82203f5d9`.
- `.github/workflows/restore-v0.7-source.yml`, blob `eaee0ac2a057daf86942483a4f08d85756626ecf`.
- `docs/audits/2026-11-01-main-diagnostic-branch-workflow-trigger-isolation.md`, blob `c22afa7249d6ae627e61add798534585ba2bb059`.

## Evidence, not assumptions
The Windows v0.7 build workflow runs via manual dispatch and on `main` push with explicit engine/release/workflow path filters. It grants `contents:read`, uses checkout with `persist-credentials:false`, a 32-bit MSYS2/MinGW source build and artifacts. The restoration workflow likewise permits manual dispatch, but grants `contents:write` and can commit a replacement engine to `main`. The prior branch audit reports `main` as `protected:false` in branch listing; detailed protection API was inaccessible. These are distinct concerns: a protected baseline convention is not proof of GitHub-enforced branch protection.

## Proposed optional diagnostic workflow, NOT created
1. **Separate artifact:** unique new workflow filename, separate from both existing workflow files. Do not modify, copy wholesale, or dispatch the restoration workflow. No changes to the v0.7 source baseline.
2. **Trigger:** `workflow_dispatch` only (no `push`, `pull_request`, `schedule` or `workflow_run` triggers). GitHub manual dispatch does not become branch-isolated merely by writing a `branches` key beneath `workflow_dispatch`; require an explicit job-level validation of `github.ref` and an exact allowed diagnostics branch/ref before checkout or build steps. Preserve the workflow's availability and ref semantics by validating them on a disposable test run.
3. **Repository permissions:** default `permissions: contents: read` only; checkout uses `persist-credentials: false`. No source writes, GitHub commits, release uploads, deployments, PR merging or secrets required for diagnostics.
4. **Source integrity:** check out the intended diagnostic ref explicitly, then compare `git rev-parse HEAD` with an expected, reviewed full 40-character commit SHA; reject a mismatch before dependency installation or build. Record the SHA, workflow ref, toolchain versions and flags in the diagnostics artifact. Do not assume that a user-supplied SHA and branch are mutually consistent without verifying ancestry.
5. **Toolchain:** reproduce the verified 32-bit `i686-w64-mingw32` pipeline and current SDL2/OpenAL Soft 1.24.3/WildMIDI 0.4.5 build configuration, including the established Windows import-library handling and build flags. Do not silently substitute an x64 build.
6. **Profiling boundary:** a diagnostic compile option, off by default, should initially measure existing C frame/input/audio/display boundaries with bounded in-memory records and deferred flushing. Any assembly probes require separately checked register/flags/stack preservation and proof that the `move_it` vehicle tail jump remains intact.
7. **Artifacts:** upload only disposable diagnostic executable and plain-text/CSV trace plus manifest; avoid bundling copyright-protected retail game data, credentials or private saves. Set explicit artifact retention and distinguish synthetic test data from actual runtime measurements.
8. **Acceptance gates:** confirm manual dispatch refuses main, Graphics Audit and Mission Builder refs; validate mismatch rejection; ensure no repo write token and no accidental push triggers; compare clean v0.7 baseline and diagnostic builds in identical quiet/combat/explosion scenarios; do not merge without user approval.

## Boundaries
This is a design specification only, not a workflow file or executable implementation. No diagnostic branch, GitHub Actions run, code change, Windows build, runtime test, profiling, merge or branch-protection setting change occurred. Existing Graphics and Mission Builder branches untouched.

## Next checkpoint
Audit potential workflow_dispatch ref validation and source SHA selection mechanisms against actual checked-in workflow context before any approved implementation; keep MAIN handover independent.
