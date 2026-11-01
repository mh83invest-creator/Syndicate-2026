# Syndicate 2026 — RNC Decompression Capacity Static Audit
Date: 2026-11-01 Australia/Brisbane
Status: SOURCE AUDIT ONLY; NO ENGINE CHANGE, BUILD, RUNTIME TEST, OR EXPLOIT REPRODUCTION
Related: docs/audits/2026-11-01-briefing-loader-bounds-static-audit.md; Mission Builder integration contract

## Evidence
Inspected engine/src/syndre.sx in full, GitHub blob SHA c35e22621a93d1e07c44d40de8867446ff7c64bf.
RNC-01: brief_text reserves 0xfa0=4000 bytes at lines 85610–85611.
RNC-02: load_text at lines 55277–55286 calls LbFileReadRNC(filename, brief_text), then gets LbFileLengthRnc(filename), then writes a terminating zero at brief_text+returned_length with no range/error guard. Existing out-of-bounds findings remain.
RNC-03: LbFileReadRNC at lines 14916–14960 receives filename and destination pointer but no destination capacity. It opens a file, asks LbFileLengthRnc for a length, calls LbFileRead_ into destination, then calls UnpackM1 with two copies of the destination pointer. It does not check whether the returned length fits the destination, read count, or decompressed output size. It returns a file handle-like value rather than a verified output byte count.
RNC-04: UnpackM1 at lines 62225 onward checks an RNC header and reads unpack_len and pack_len fields, then performs unpacking. The observed signature does not include destination capacity. The caller's nominal 4000 bytes cannot be enforced at that interface. Untrusted or corrupt RNC lengths therefore pose a potential buffer-overrun risk; the exact write path and runtime outcome require further analysis and tests.
RNC-05: LbFileLengthRnc at lines 14964 onward returns -1 on failed open and derives size from an RNC header or file length. Its value is not independently checked by load_text before indexing brief_text. Different paths may have different compressed/uncompressed size handling; do not claim this is a completed security exploit or reproduced overflow.

## Guardrails and requirements
- Separate the existing raw resource loader from future custom content ingestion; require validated bounded input and explicit failure state before author-created resources are enabled.
- Preserve original v0.7 working engine, 16 FPS mission simulation, camera and classic save files.
- Do not patch UnpackM1, alter RNC formats, redefine custom mission data or change error handling without an approved reviewable implementation plan.

## Proposed tests (not performed)
- Missing and empty resource, malformed RNC header, oversized declared unpack length, truncated compressed stream, invalid file-length result, and boundary sizes at 3999, 4000 and 4001 bytes.
- Instrument/write sentinels around synthetic destination buffer, check every return status and output count; run under suitably supported memory diagnostics.
- Regression on original and American Revolt assets, and later safe Mission Builder fixtures.

## Verification/handover
Full GitHub assembly blob examined with exact symbol/line references. No Windows build, gameplay, test harness, real asset inspection, RNC decompression execution or engine code edits. Docs-only handover; no version change or rollback required.
