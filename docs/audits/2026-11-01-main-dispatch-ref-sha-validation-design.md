# Syndicate 2026 — Main Systems Audit: manual dispatch ref and SHA validation design

Date: 2026-11-01 (Australia/Brisbane). Documentation-only on `main`.

## Source inspected
- `.github/workflows/windows-source-v0.7.yml`, Git blob `ab9c099923d859e85f996e0c11823cf82203f5d9`.
- `.github/workflows/restore-v0.7-source.yml`, blob `eaee0ac2a057daf86942483a4f08d85756626ecf`.
- `docs/audits/2026-11-01-main-optional-diagnostics-workflow-isolation-spec.md`, blob `8ab920883de134753e1b34dfa129dc185a542449`.

## Verified repository facts
- Existing v0.7 Windows workflow has `workflow_dispatch` with no input fields, `push` filters for `main`, `permissions: contents: read`, and `actions/checkout@v4` with `persist-credentials: false`. It does not explicitly set checkout `ref`, and it contains no custom verification of `github.ref` or `git rev-parse HEAD`.
- Existing restoration workflow also has `workflow_dispatch`, and `contents: write`, plus source replacement and push to `main`; it has no manually selected ref safety gate. Do not invoke for diagnostics.
- The proposed diagnostic workflow does **not** yet exist; no assumptions about actual runner behaviour have been validated by a run.

## Proposed fail-closed diagnostic job, unimplemented
1. Require a single fixed allowed diagnostics branch name chosen and reviewed before implementation (example in prose only: `diagnostics/main-performance`). Do not allow any branch selected through arbitrary dispatch inputs.
2. First job step, before checkout/build/dependency commands: compare `github.ref` exactly with `refs/heads/<approved diagnostic branch>`; fail if different. Additionally compare `github.ref_type` to `branch`. Evaluate this check only in the future diagnostic workflow; existing restoration workflow is not part of this plan.
3. Checkout **the event's immutable `github.sha`** (or another explicitly reviewed full-length commit SHA) via `actions/checkout@v4` and `persist-credentials: false`. Check the local `git rev-parse HEAD` equals the expected full SHA; fail if not. Do not treat a floating branch name as an immutable source pin.
4. A commit SHA check establishes which source was built; it **does not itself prove** the SHA is on the approved diagnostics branch or derived from v0.7. Verify branch/ref relationship separately (e.g. compare event ref to expected and inspect commit ancestry using correctly fetched history). Specify exact expected baseline SHA and ancestry policy during implementation, not by guessing now.
5. Set `permissions: contents: read`. Prohibit automatic triggers, git push, deployment and write credentials. Keep manifest/source SHA in artifacts; do not include retail game files or personal save data.
6. Fail closed before any dependency fetch or compile if the event ref or expected source hash is invalid. Test rejection on disposable builds before allowing actual performance instrumentation.

## Scope and limitations
This is a design review against checked-in workflow contents, **not a GitHub Actions execution test** and not external verification of all possible platform dispatch mechanics. No new workflow, branch, code, test run, Windows compilation, profiling or merges. Baseline v0.7 and separate Graphics / Mission Builder branches unchanged.

## Next checkpoint
Determine a verified clean baseline commit SHA and diagnostic branch relationship; prepare precise acceptance tests for rejected refs, hash mismatch and absence of source writes without executing them until approved.
