# Syndicate 2026 — Cross-backend short-read and decompression caller scope
Date: 2026-11-01 (Australia/Brisbane)
Scope: static audit; protected baseline unchanged.
Prior handover: docs/audits/2026-11-01-rnc-short-read-backend-contract.md.

## Verified evidence
1. engine/bflibrary/src/x86-win-sdl/sfile.c:211-217 returns the integer result of read(fhandle, buffer, len), including nonnegative short counts.
2. engine/bflibrary/src/x86-win-sdl2/sfile.c:211-217 does the same.
3. engine/bflibrary/src/x86-dos-watcom/sfile.c:127-133 also returns the integer result of read(handle, buffer, len). Thus all three identified backend implementations preserve short-read information.
4. engine/bflibrary/Makefile.am:123-157 selects the SDL2 implementation of sfile.c when USE_SDL2 is defined; otherwise uses x86-win-sdl/sfile.c. The DOS/Watcom source exists separately; do not infer it is part of this Automake selection.
5. engine/bflibrary/src/general/gfile.c: LbFileLengthRnc checks only -1 for an 18-byte header read, and LbFileLoadAt checks only -1 rather than a full requested length for the resource read. These are shared wrapper-level gaps independent of the three reviewed backend read implementations.
6. The verified C call chain is LbDataLoadAll -> LbDataLoad -> LbFileLoadAt -> UnpackM1 -> rnc_unpack for file-backed resources. Direct calls to LbFileLoadAt or UnpackM1 elsewhere have not been exhaustively enumerated.
7. Repository code search requests for UnpackM1, LbFileLoadAt and LbFileReadRNC returned no indexed results; absence of indexed matches is NOT proof of absence of callers. Explicitly preserve this limitation.

## Assessment
The short-read acceptance gap is present in the common loader wrapper, not newly introduced by SDL1 vs SDL2 selection. Source audit alone does not prove runtime exploitation, crashes or malformed retail resources. A shared-loader change would have compatibility implications across all resources and must not be made as part of an audit-only pass.

## Next gates
- Complete repository-wide direct caller enumeration using a complete source checkout or reliable code indexing, including assembly entry points and alternate file loaders.
- Define and test physical compressed input length, RNC header size and declared unpacked output length separately; include forced allocation failure and short reads in isolated harnesses.
- Confirm Windows 32-bit build and retail resource loading on a copy, keeping untouched v0.7 as rollback.
- Coordinate resource requirements with Mission Builder; campaign saves and original gameplay pacing remain protected.

## Work actually performed
Reviewed live GitHub source for all three sfile.c backends, Automake branch selection, and the prior gfile.c/gdata.c/rnc_1fm.c call chain; checked the results of GitHub code-index searches. This handover is documentation-only. No engine edits, CI runs, executable compilation, memory or gameplay tests, save writes or Mission Builder changes.
