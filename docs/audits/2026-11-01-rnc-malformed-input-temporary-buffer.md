# Syndicate 2026 — RNC malformed-input and temporary-buffer static audit
Date: 2026-11-01 (Australia/Brisbane)
Status: static source audit ONLY; no code patch, compilation, retail asset access or runtime tests.
Repository: mh83invest-creator/Syndicate-2026, main.
Continues docs/audits/2026-11-01-world-map-rnc-loader-size-contract.md.

## Source
- engine/bflibrary/src/general/rnc_1fm.c, specifically UnpackM1 lines 440–455 and rnc_unpack lines 280–437
- engine/bflibrary/src/general/gfile.c, LbFileLengthRnc and LbFileLoadAt
- engine/bflibrary/src/general/gdata.c, LbDataLoad and LbDataLoadAll
- engine/bflibrary/include/rnc_1fm.h (RNC_HEADER_LEN=18)

## Source-backed findings
RNC-01. rnc_unpack parses ret_len=blong(input+4) (declared unpacked size) and inp_len=blong(input+8) (declared packed payload size), rejects either greater than 1<<30, checks packed CRC, decodes blocks with multiple guarded error returns and finally verifies output size and CRC. These guards do not themselves validate that the caller supplied an allocation large enough for the claimed source and destination ranges.

RNC-02. UnpackM1 at lines 440–455 first reads the four-byte signature and returns zero for non-RNC input. For RNC input it assigns packedsize=blong(buffer+4), i.e. the same field identified by rnc_unpack as *unpacked* length, caps this at bufsize, and allocates that many bytes for a copy of the input. It does NOT use the packed payload length at header +8 plus the 18-byte header to determine actual bytes available for the copy. This mismatch is observable from source and requires controlled runtime testing to assess consequences.

RNC-03. At UnpackM1 line 451, LbMemoryCopy(packed,buffer,packedsize) executes BEFORE the line-452 check 'if (packed == NULL) return -1'. A failed allocation could therefore reach a copy with NULL destination. This is an ordering defect visible directly in the code, NOT a reproduced crash.

RNC-04. LbFileLoadAt computes filelength from LbFileLengthRnc: for RNC files that function returns the *unpacked* length declared in the header, not physical packed-file length. LbFileLoadAt then calls LbFileRead(handle,buffer,filelength) and UnpackM1(buffer,filelength). This requires validation of packed-file/header availability; the loader does not visibly require the read to have returned exactly filelength (only -1 triggers an error) before decompressing.

RNC-05. rnc_unpack constructs inputend = input + 18 + inp_len from the header and outputend = output + ret_len without explicit caller-buffer extent arguments. It checks many per-chunk decode ranges and CRCs, but the header-derived endpoint can exceed the actual provided packed data if the file is truncated or malformed. Do not claim memory safety solely from the internal chunk checks.

RNC-06. LbDataLoad(file-backed) allocates expected decompressed length+512, expects the LbFileLoadAt return length to match, and reports failures. map_selection immediately renders after LbDataLoadAll without inspecting failure count. The decompressor concerns therefore matter to world-map resource handling and potential new Mission Builder assets. No retail mmapblk.dat sample was examined.

## Risk classification
These are conditional unsafe-input / allocation-failure concerns and a clear NULL-check ordering defect, not tested exploits or established end-user crashes. The original v0.7 gameplay must not be changed by this documentation-only audit. Treat any bounds or allocation fix as a separately reviewed source patch with 32-bit Windows compilation and actual retail-data regressions.

## Next engineering gates
1. Build isolated synthetic RNC samples in a dedicated test harness, including tiny/truncated header, inconsistent header lengths, zero-length, compressed payload shorter/longer than unpacked length, CRC failures and forced allocation failure. Do NOT test against live installed assets or original saves.
2. Confirm raw byte count from the file backend, actual physical packed size and the destination capacity before modifying a decompressor interface.
3. Before changing shared loader code, check resource consumers beyond map_selection and coordinate with Mission Builder branch.
4. Preserve protected v0.7 executable, mainline build pipeline, 16 FPS tactical simulation, original campaigns and save isolation requirements.

## Verification
Performed: exact GitHub source read of decompression routines and cross-reference to existing C loader chain. Not performed: Windows compile, fuzzing, runtime fault injection, gameplay or engine edits. No changes to saves or Mission Builder.
