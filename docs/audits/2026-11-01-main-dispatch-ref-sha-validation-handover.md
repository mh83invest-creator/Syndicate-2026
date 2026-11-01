# MAIN handover — fail-closed diagnostic dispatch design

2026-11-01. MAIN audit, documentation-only.

Audit `docs/audits/2026-11-01-main-dispatch-ref-sha-validation-design.md` committed at `66733a308675e83c7a20795423d4e2b30d146d50`.

Verified existing Windows workflow manual trigger and read-only checkout; it does not currently gate dispatch with a ref check or verify local checkout SHA. Restore workflow is separately writable and must not be used for diagnostics. Proposed standalone diagnostic job would compare `github.ref` and `github.ref_type` against approved branch before checkout, checkout immutable `github.sha`, verify `git rev-parse HEAD`, require source lineage checking separately, and operate with read-only permissions/no automatic triggers. None of this is implemented or tested.

Next: obtain verified clean baseline SHA and define branch ancestry/rejection acceptance checks before any approved diagnostic workflow creation. Protected v0.7 remains unchanged; Graphics and Mission Builder untouched.
