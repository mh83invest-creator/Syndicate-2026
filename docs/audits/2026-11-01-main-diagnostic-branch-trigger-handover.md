# MAIN handover — diagnostic branch and workflow isolation

Date: 2026-11-01. MAIN systems workstream; documentation-only.

New audit: `docs/audits/2026-11-01-main-diagnostic-branch-workflow-trigger-isolation.md`, commit `bdd3370083726eccb352652a57db88c1cd0009b3`.

### Verified GitHub evidence
- Windows workflow `.github/workflows/windows-source-v0.7.yml`: manual dispatch or `main` pushes touching `engine/**`, `docs/releases/v0.7.md`, or that workflow; `contents:read`.
- Restore workflow `.github/workflows/restore-v0.7-source.yml`: manual dispatch or `main` push touching its own file; `contents:write` and direct replacement/commit to `main`.
- Branch listing shows `main` with `protected:false`; detailed branch protection read was denied 403. This means policy-protected v0.7 cannot be assumed branch-rule protected.
- Separate `audit/graphics-assets-2026-11-01` and `feature/mission-builder-audit` workstream branches exist.

### Next checkpoint
Inspect whether CI workflow manual dispatch can be limited to an explicitly isolated branch; design a never-auto-restoring optional diagnostic workflow specification with read-only permissions and exact source SHA, without making any workflow or engine changes. Future code instrumentation requires approval. Preserve v0.7, 16 FPS gameplay, original explosions/audio and between-mission save isolation.

No diagnostic branch, source edits, workflow modifications, compilation, benchmarks or gameplay testing occurred.
