# Syndicate 2026 — RNC short-read contract and file backend audit

Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE INSPECTION ONLY. No engine patch, build, gameplay test or save changes.
Predecessor: docs/audits/2026-11-01-rnc-malformed-input-temporary-buffer.md.

## Examined source
- engine/bflibrary/src/general/gfile.c: LbFileLengthRnc (lines 31-56), LbFileLoadAt (58-91)
- engine/bflibrary/src/x86-win-sdl/sfile.c: LbFileRead (211-217)
- engine/bflibrary/src/x86-win-sdl2/sfile.c: LbFileRead (211-217)
- engine/bflibrary/src/general/rnc_1fm.c: UnpackM1 (440-455), rnc_unpack (280-437)
- engine/bflibrary/src/general/gdata.c: LbDataLoad and LbDataLoadAll

## Verified findings
RD-01: Both inspected SDL file backends implement LbFileRead by calling read(fhandle, buffer, len) and returning that result. A nonnegative short count is distinguishable from the -1 error sentinel.

RD-02: LbFileLengthRnc allocates a local buffer of RNC_HEADER_LEN+1 and requests RNC_HEADER_LEN=18 bytes. It rejects only -1, not a nonnegative count less than 18. Header bytes may therefore remain uninitialised on a short positive/zero read before signature/length inspection. This is a static source risk, not a reproduced failure.

RD-03: LbFileLoadAt obtains 'filelength' through LbFileLengthRnc, opens the file again, calls LbFileRead(..., filelength), and rejects only -1. It does not require returned bytes == filelength before passing the buffer to UnpackM1.

RD-04: For files with the RNC signature, LbFileLengthRnc returns the *declared unpacked length* at header offset 4, not the physical packed-file length (which may differ). Consequently, LbFileLoadAt's single read-size request does not represent an independently established physical source size. A safe future redesign must track physical compressed bytes, declared decoded bytes, and allocated destination extent independently.

RD-05: UnpackM1 uses header-declared lengths and rnc_unpack builds header-derived input/output end pointers without explicit verified physical source length. Its temporary allocation NULL check follows LbMemoryCopy. These observations were recorded in the predecessor audit; no runtime error proven.

RD-06: LbDataLoad allocates LbFileLengthRnc(fname)+512 for a file resource and calls LbFileLoadAt; the map_selection assembly ignores the batch loader's returned failure count before rendering. A missing or corrupt world-map resource therefore remains an unverified but relevant error-path risk.

## Caller inventory scope and limitations
The common path LbDataLoadAll -> LbDataLoad -> LbFileLoadAt -> UnpackM1 -> rnc_unpack is verified in the examined C implementations. Other direct callers and other file backends have NOT been exhaustively enumerated. This document is not a repository-wide audit completion claim.

## Required next steps
1. Enumerate direct LbFileLoadAt and UnpackM1 callers and any other file backends; capture compatibility requirements.
2. Propose isolated nonproduction tests for 0..17 byte header reads, valid 18 byte RNC header with truncated payload, inconsistent unpacked and packed lengths, short positive read, CRC mismatch and allocation failure. Test only copies/synthetic resources.
3. Establish binary-compatible source/destination length contracts before proposing a shared loader patch; coordinate with Mission Builder.
4. Retain original v0.7, original and American Revolt campaigns, fixed 16 FPS mission simulation, and campaign-save separation design unchanged.

## Verification and rollback
Verified: static source read of both SDL backend read implementations and loader size/error checks. Not performed: compilation, executable launch, Windows file operations, retail-data test, fuzzing or instrumentation. Documentation-only commit; no rollback of executable required.
