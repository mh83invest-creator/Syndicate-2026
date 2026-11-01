# Syndicate 2026 — Main Systems Audit: diagnostic branch and workflow trigger isolation

Date: 2026-11-01 (Australia/Brisbane). Workstream MAIN. Documentation only.

## Directly verified GitHub evidence
- `.github/workflows/windows-source-v0.7.yml` blob `ab9c099923d859e85f996e0c11823cf82203f5d9`.
- `.github/workflows/restore-v0.7-source.yml` blob `eaee0ac2a057daf86942483a4f08d85756626ecf`.
- GitHub branch listing retrieved from `/repos/mh83invest-creator/Syndicate-2026/branches` on 2026-11-01; includes `main`, `audit/graphics-assets-2026-11-01`, `feature/mission-builder-audit`, and other legacy/recovery/feature branches. Branch list reports `protected:false` for `main` and the listed branches.
- Attempt to read `/branches/main/protection` returned 403 Resource not accessible by integration. Do not interpret inaccessible detailed protection configuration as inspected evidence.

## Trigger / write-permission facts
1. `windows-source-v0.7.yml` has `workflow_dispatch`, plus `push` restricted to branch `main` and path patterns `engine/**`, `docs/releases/v0.7.md`, and `.github/workflows/windows-source-v0.7.yml`. It grants `contents:read` and uses checkout with `persist-credentials:false`.
2. `restore-v0.7-source.yml` has `workflow_dispatch` plus `push` restricted to `main` and its own workflow filename. It grants `contents:write`, imports a pinned external engine and later runs a source replacement commit/push to `main`. It is a recovery mechanism, not a safe profiling-build starting point.
3. Pushing audit files only under `docs/audits/**` to `main` is outside both workflows' specified automatic push path patterns. This statement concerns these two workflows only; it does not rule out other repository events, manually dispatched workflows, or changes to triggers.
4. Branch `main` is returned as `protected:false` by the GitHub branch-list API. This matters: the project rule to protect v0.7 is an operational baseline policy, **not shown to be enforced by a branch protection rule**.
5. The listing confirms active separate Graphics and Mission Builder workstreams. No branch was created, merged or modified in this checkpoint.

## Recommended future diagnostic isolation (proposal, not implemented)
- Create an explicitly named, disposable diagnostics branch from a fixed, verified source commit, only after approval.
- Do not edit `main`, the existing recovery workflow, `docs/releases/v0.7.md`, or parallel branches.
- If adding an optional diagnostic workflow, give it a unique filename, restrict its triggers to manual dispatch and the diagnostic branch, and grant read-only permissions unless a specific operation requires more.
- Avoid treating the restoration workflow as a diagnostic workflow, because it can rewrite the repository.
- Retain existing 32-bit MinGW/SDL2/OpenAL/WildMIDI build settings and preserve original 16 FPS tactical timing, explosion logic and visuals, audio, controls, and isolated between-mission saves.
- Before profiling, verify the disposable branch's exact source SHA, test workflow triggers, compare with an untouched baseline, and keep probe output bounded and out of the main simulation hot path.

## Test/status boundary
Completed: read actual two workflows, enumerate branches, attempt protected-branch details. Not completed: branch protection API details (403), diagnostic branch creation, workflow edits, Windows compilation, game execution, profiling, benchmarks or source fixes. No engine or parallel workstream changes.
