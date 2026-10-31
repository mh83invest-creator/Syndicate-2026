# Syndicate 2026 — Mandatory Build Handover Policy

**Effective from v0.8 and applies to every subsequent build, including experimental or failed builds.**

Every version must have a version-specific Markdown handover file checked into `docs/releases/` alongside the code and workflow that build it. A packaged ZIP or chat history is **not** a substitute for repository documentation.

## Minimum contents
1. **Identification:** version, release date, source repository and commit SHA, upstream baseline, branch, workflow name/run link, downloadable build artifact and status.
2. **Changes:** exactly what changed from the prior version, with source paths and a short technical explanation; state what was intentionally left unchanged.
3. **Build and dependencies:** toolchain, build commands/workflow, native Windows 32-bit runtime/DLL expectations, packaging and startup instructions. Do not commit proprietary Bullfrog retail game data.
4. **Tests and evidence:** source checks, Windows compilation/link and dependency checks, ZIP integrity, Windows gameplay tests separately; explicitly distinguish automated checks from user-confirmed playtests.
5. **Known issues and regressions:** unverified behaviours, failures and workarounds, with links to relevant GitHub runs.
6. **Decisions and rationale:** frame pacing versus mission simulation, camera fixes and other significant design constraints.
7. **Continuation guide:** stable fallback version, pending tasks, step-by-step starting point, exact branch/paths/workflow, priorities, and rollback procedure.
8. **Changelog:** cumulative summary or link to versioned changelog.

## Enforcement
- For any new build version, create/update `docs/releases/vX.Y.md` in the same commit or pull request as the source/workflow changes.
- Record a **pre-build** handover for work in progress. Update it after each build run and after user gameplay testing.
- Windows CI checks for the version handover before compiling and publishes a copy of the handover alongside the compiled executable.
- Never report an untested binary as gameplay-verified. Keep successful known-good test packages separate.
- Keep `main` as an independently buildable source baseline; create experiment branches if a change is risky.

## Mandatory continuity for ALL completed work — 1 November 2026

This extends (does not replace) the build/release requirements above. Every completed discrete **build, audit stage, source investigation, experiment, verification, bug isolation, test, implementation or material architecture decision** requires a durable GitHub Markdown handover **before being reported as completed**. A chat response or local download alone is not continuity.

### Where records live
- Builds/releases: keep `docs/releases/vX.Y[phase].md` as required above.
- Audit and investigation stages: `docs/audits/YYYY-MM-DD_<topic>.md` (new records); existing `docs/SOURCE_AUDIT_*.md` remain authoritative historical records and must not be discarded or silently reclassified.
- Maintain a chronological index at `docs/CONTINUITY_INDEX.md` linking each new handover and identifying status. Add to the index with each milestone; do not silently overwrite old entries.
- For existing in-progress long-form audit reports, append dated findings and link any standalone stage handover rather than duplicating contradictory claims.

### Required handover fields for every completed process
1. **Identity:** work item, date (Australia/Brisbane), version/stage, GitHub branch, starting and ending commit SHAs (or explicitly state unchanged), supporting file paths and upstream baseline.
2. **Instruction and boundaries:** user-approved request, in-scope items, expressly excluded items, protected baselines, and dependencies.
3. **Work actually performed:** precise file/function/line references, source findings or code deltas, decisions and evidence; label source-reading conclusions separately from inference and runtime proof.
4. **Verification ledger:** each check performed with method, result (PASS/FAIL/NOT RUN/BLOCKED), any workflow/run/artifact IDs, and separately recorded CI compilation, automated tests, native Windows runtime tests, and user gameplay confirmation. A successful commit/read-back is not a successful build or gameplay test.
5. **Known risks and outstanding work:** unresolved hypotheses, defects and possible regressions, with clear prerequisites for resolution. Do not call source-only review a complete end-to-end audit.
6. **Next action:** exact next stage, branch, files/symbols to inspect, command/workflow or artifact where applicable, prerequisites, acceptance/exit criteria, and rollback route for modifications.
7. **Repository verification:** confirm successful GitHub commit and fetch/read the committed document back, recording commit identity and link. If publishing fails, explicitly report handover NOT uploaded; do not claim the milestone is fully handed over.

### Completion and change rules
- A milestone is **documented** only after the handover has been uploaded to GitHub and read back. A milestone is **verified** only for the checks expressly recorded as having passed; all other checks remain unverified.
- Interim work may be logged as IN PROGRESS; its handover must not claim completion. Failed experiments require handovers too.
- Record completed work at the end of its discrete stage, not only after the whole project; update its entry whenever later evidence changes the assessment, preserving dated history.
- Do not modify, merge, rebuild, fix unrelated defects, copy proprietary assets or promote `main` merely to document a stage. Documentation writes stay on the active non-baseline branch unless explicitly approved otherwise.
- Each future conversation should start by reading `docs/CONTINUITY_INDEX.md`, this policy, the latest stage handover and the relevant release handover before resuming. Never rely on chat memory as the sole source.
