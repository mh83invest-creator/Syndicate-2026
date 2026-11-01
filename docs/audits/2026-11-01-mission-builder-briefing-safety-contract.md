# Syndicate 2026 — Mission Builder Briefing Resource Safety Contract (Proposed)
Date: 2026-11-01 (Australia/Brisbane)
Status: DESIGN REQUIREMENTS DERIVED FROM STATIC AUDIT; NOT AN APPROVED IMPLEMENTATION OR TESTED FIX
Baseline: known-good v0.7; original engine, game data and save files remain untouched
Dependencies: docs/audits/2026-11-01-briefing-loader-bounds-static-audit.md; docs/audits/2026-11-01-rnc-decompression-capacity-audit.md; docs/audits/2026-11-01-language-briefing-map-id-verification.md; docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; Mission Builder workstream on feature/mission-builder-audit.

## Evidence
- Assembly brief_text allocation: exactly 0xfa0 (4000) bytes, syndre.sx lines 85610-85611.
- load_text chooses filename from language and mission ID, invokes LbFileReadRNC, then LbFileLengthRnc and writes NUL at returned index without a size/negative check (lines 55249-55289).
- Assembly LbFileReadRNC calls lower-level file read and UnpackM1 using destination pointer without capacity parameter (lines 14916-14960).
- Assembly UnpackM1 parses packed and unpacked header lengths; caller does not enforce a destination size. A resource may overrun before the final terminator write.
- Original use includes user-owned retail data, absent from repository; no real resource sizes yet validated.

## Proposed boundary contract — subject to design approval
BC-01: Custom content is not passed directly from Mission Builder to the legacy unbounded assembly loading path. A dedicated validation and bounded-reading layer must be specified before executing user-created missions.
BC-02: Validate resource existence and readable regular-file status, lengths, RNC signature/header and compressed/uncompressed size declarations before allocating or writing into a destination buffer. Reject negative sizes, inconsistent lengths, short reads, decompression failures and oversized declared output. Check arithmetic overflow.
BC-03: For the legacy 4000-byte briefing buffer, reserve one byte for final NUL, allowing at most 3999 actual output bytes AFTER successful validation and decompression. This is a proposed bound, not proof that all legacy RNC parsing is safe at 3999 bytes.
BC-04: Any loader API must accept explicit destination capacity and provide status plus actual output length. A failure must not publish partially written briefing text or advance to a mission based on invalid data; exact UI error wording and transitions remain implementation design.
BC-05: Keep campaign ID, mission ID, map ID and language separate. Briefing paths use language-aware patterns, while tactical maps use level__MapNumber. Do not infer campaign namespace from resource naming.
BC-06: Original and American Revolt assets must remain readable, with no modification or repackaging of user-owned data. Compatibility to be demonstrated by tests before changing legacy path behaviour.
BC-07: The save-silo requirement remains independent: Original Campaign, American Revolt, World Domination and playable custom campaigns have isolated Save/Load/Continue. This contract does not decide custom-campaign granularity or migrate old saves.

## Proposed gates and verification cases — NONE EXECUTED
G0: Enumerate all callers of LbFileReadRNC and UnpackM1, distinguishing arbitrary destinations and expected capacities.
G1: Specify where bounded validation occurs, data format supported, and how it prevents a direct call into the unbounded legacy path.
G2: Synthetic tests: missing/empty file, 3999/4000/4001 output bytes, malformed RNC header, mismatched packed length, decompression failure, read error/short read, integer wrap and repeat loading after failure.
G3: Buffer sentinel or sanitizer checks: no out-of-range writes and no partial publishing; verify both raw and compressed input.
G4: Run 32-bit Windows build and real-asset tests on classic modes, then a separate approved Mission Builder fixture.
G5: Document approved implementation design and code changes under docs/HANDOVER_POLICY.md. No code changes under this audit-only record.

## Handover
Performed: cross-checked the three cited GitHub audit notes against relevant source semantics and authored a bounded loading specification for review. No code, test harness, build, runtime experiment or game data modifications. This document does not assert a working fix. Rollback not needed for documentation-only commit.
