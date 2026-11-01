# Syndicate 2026 — Briefing Loader Bounds Static Audit
Date: 2026-11-01 Australia/Brisbane
Status: CONFIRMED SOURCE-LEVEL BOUNDS DEFECT; no crash reproduction, build, tests or engine modifications
Related: docs/audits/2026-11-01-language-briefing-map-id-verification.md, docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md, feature/mission-builder-audit

## Evidence
Read complete engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf using GitHub fetch_blob.
BRIEF-01: brief_text at line 85610 reserves exactly 0xfa0 (4000) bytes via .fill at line 85611.
BRIEF-02: load_text lines 55249–55289 generates a language-dependent briefing filename and calls LbFileReadRNC at line 55280, then LbFileLengthRnc at 55284, then writes a NUL at (brief_text + returned length) at 55286 without any range/error check between length retrieval and the write.
BRIEF-03: The assembly LbFileLengthRnc implementation initializes its result to -1 at line 14970 and retains that result on the failed-open branch at lines 14988–14989 and 15029–15041. Therefore a missing briefing can cause a write to brief_text[-1] in load_text, given the inspected branch. This is source-level out-of-bounds access, NOT a demonstrated runtime crash.
BRIEF-04: A returned length >=4000 would position the NUL at or beyond the declared 4000-byte buffer. No maximum-length check was found in load_text. Whether compressed-file paths cause additional overrun depends on the LbFileReadRNC implementation, source/destination capacity and actual data, which require separate auditing. Do not infer that 3999 bytes is automatically safe for all file formats.
BRIEF-05: LbFileReadRNC opens/reads/unpacks into caller-provided storage, and the inspected call passes no capacity parameter. The loader may also overflow during read/decompression before the NUL terminator write if supplied oversized or malformed input. Exact API contract and test reproduction outstanding.

## Scope and severity
Source-level memory safety issue in data loading relevant to optional custom missions. No claim of malicious exploitability or observed crash; severity prioritization requires runtime testing. Original legal retail assets have not been inspected. New authored missions increase importance of validating external briefing inputs.

## Safe next steps
1. Design bounded synthetic briefing fixtures: missing file, empty, negative file-length error, 3999/4000/4001-byte uncompressed content, oversized compressed data and malformed RNC header.
2. Determine exact RNC decompression API and buffer writes; specify an explicit size/error contract for any future fix. Do not change core assembly behavior until patch and regression scope are approved.
3. Run Windows i686 build and controlled game tests, verify no changes to original campaign or American Revolt, then handover versioned build changes per policy.
4. Cross-brief Mission Builder on mandatory external-file size validation and failure handling before custom missions are enabled.
5. Preserve the known-good v0.7 baseline and source until a reviewable change is ready.

## Verification and handover
Full assembly source reviewed at exact locations; code path and buffer allocation confirmed. No code modified; no compilation, fuzzing, gameplay, save-file operations or runtime test performed. Docs-only audit, rollback not needed.
