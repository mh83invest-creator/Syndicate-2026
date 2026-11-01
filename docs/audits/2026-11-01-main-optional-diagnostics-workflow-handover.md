# MAIN handover — optional diagnostic workflow specification

2026-11-01. Main systems audit, documentation-only.

New report: `docs/audits/2026-11-01-main-optional-diagnostics-workflow-isolation-spec.md`. Audit commit `90b715f179b54236a8fa4dd75322a2967e9794db`.

Verified original Windows source-build workflow has read-only token, guarded main push triggers and 32-bit dependencies; restoration workflow is separately manually dispatchable and can rewrite main using write permission. The proposed optional *new* diagnostic workflow is manual-only with explicit job-level exact branch/ref and full source-SHA validation, read-only permissions, checkout without persisted credentials, existing MinGW/OpenAL/WildMIDI build identity, diagnostics disabled by default and artifact-only output (no game retail data or secrets). Do not assume `workflow_dispatch` supports branch restrictions by simply adding push-style branch filters; validate actual selected ref in the job before running build steps.

No workflow was created or run, no source/profiler code written, no branch/merge/protection changes or gameplay testing. Protected working v0.7, Graphics Audit and Mission Builder remain unchanged.

Next: validate dispatch ref/check-out and SHA-check design against verified repo workflows, then await explicit approval for any isolated build/instrumentation.
