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
