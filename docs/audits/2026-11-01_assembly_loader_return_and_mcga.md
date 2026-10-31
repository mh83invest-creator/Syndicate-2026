# Syndicate 2026 — Original assembly descriptor and return-value follow-up

Date: 2026-11-01 (Brisbane)
Status: focused static-source stage completed; broader audit incomplete.
Branch: feature/v0.9-visual-showcase. Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf; engine/bflibrary/include/bfdata.h; engine/bflibrary/src/general/gdata.c.
No gameplay source, workflow, resources or other feature changed.

## Direct findings
- syndre.sx lines 83651–83657 declare low-resolution WScreen descriptor with its four-byte length field encoded as little-endian bytes 10 e8 03 00. The implied SLength is 0x0003e810 = 256016 bytes. This uses the 44-byte packed TbLoadFiles layout established in bfdata.h. High-resolution WScreen descriptor at lines 83626–83632 declares 256048 bytes.
- syndre.sx lines 31238–31253 select high-resolution or MCGA descriptor based on DrawFlags and call ac_LbDataLoadAll. The immediate continuation adjusts the stack and proceeds toward pointer sprite setup. No check against the returned failure count is visible in this local call-site sequence; a full control-flow proof is not asserted.
- syndre.sx lines 31325–31330 call ac_LbDataFreeAll with load_files_vres16. No matching low-resolution descriptor cleanup appears in that immediate path; whether MCGA mode can take that exit path and what previous teardown occurs is unresolved.
- gdata.c LbDataLoadAll returns a count of resource-load failures and continues processing later entries, so a caller not checking it may encounter incomplete assets. This is a source-level risk, not reproduced gameplay failure.
- Resource loader allocation ownership, WScreen offsets and the phase-dependent VScreen assignments are documented in the preceding audit handovers.

## Verification
PASS: GitHub retrieval of original assembly, packed descriptor arithmetic, local call-site inspection.
NOT RUN: native compile, low-resolution launch, memory leak/failure injection, actual game assets, alternate-mode cleanup tests.
OPEN: complete MCGA mode end-to-end reachability and teardown correctness; all return-value uses beyond these inspected sites; possible resource aliasing during mode switches.

## Continuation
Trace mode-setting routes to the shutdown block and branch-specific release logic. Keep source-only risks separate from confirmed runtime issues. Proposed repairs remain out of scope until authorised. No changes to protected main. Rollback: documentation-only.
