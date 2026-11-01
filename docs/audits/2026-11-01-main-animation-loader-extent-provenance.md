# Syndicate 2026 — Main Systems Audit: Animation Loader Extent Provenance

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch:** `main`. **Static source audit and test contract only; no C or assembly modifications.**  
**Predecessor:** `docs/audits/2026-11-01-main-animation-consumer-index-and-element-chain.md`, commit `915c920f7d0242835cb46d589a12b77f29a2365a`.

## Source inspected live

- `engine/bflibrary/include/bfdata.h:29–42,53–60` — packed `TbLoadFiles` layout and `LbDataLoadAll` return contract.
- `engine/bflibrary/src/general/gdata.c:31–130` — free, load, batch-load and failure branch state.
- `engine/bflibrary/src/general/gfile.c:31–90` — length/read/decompression pipeline.
- `engine/src/syndre.sx` GitHub blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, lines `82620–82663`, `83877–83886`, `27901–27930`, `33353–33372`, `19035–19169`, `18717–18906`. Complete original assembly blob 85,803 lines inspected directly this turn.
- Prior `docs/audits/2026-11-01-main-animation-bounds-and-profiling-feasibility.md`.

## MAIN-EXT-01 — Descriptor SLength exists even without a separate exported End pointer

`TbLoadFiles` is `#pragma pack(1)`, 44 bytes on the protected 32-bit layout: `FName[28]` offset 0, `Start` pointer slot offset 28, `SEnd` *optional* pointer slot offset 32, `SLength` (unsigned long) offset 36, `Flags` offset 40, `Spare` offset 42 (`bfdata.h:29–42`). The mission assembly descriptor list `mission_load_files` uses `data/hfra-0.ani` with `frames` and `frames_end`; `data/hele-0.ani` with `elements_ani` and `elements_ani_end`; and `data/hsta-0.ani` with `starts_ani` but **no separate `SEnd` slot** (`syndre.sx:82650–82663`).

**Important refinement:** `starts_ani` having no exported end-pointer variable does **not** mean no length information is available. The descriptor itself has `SLength`. A hypothetical bounded read-only validator could consult its entry's `SLength` *after* the successful load has been established. That validator has not been written.

## MAIN-EXT-02 — SLength is only trustworthy with an explicit successful-load condition

`LbDataLoad` (`gdata.c:65–103`) frees the existing pointer, asks `LbFileLengthRnc` for a decoded length, assigns that to `SLength`, and rejects lengths <= 0. The declared field `SLength` is unsigned long, while local `slength` is signed long, so a negative file-length result may be stored in the descriptor before returning `-101` (depending on the native 32-bit representation). It allocates `slength + 512` bytes, but the **valid decoded payload extent** is intended to be `slength`, not that extra 512 bytes of allocation. A load result mismatch zeros `Start`, `SEnd` (if configured) and `SLength`, returning `-101`; the loss of its allocated pointer without immediate free is documented in prior core loader audit.

A missing-file branch can therefore leave a `NULL` Start with an invalid/stale/unsigned-converted `SLength`; the file-load mismatch branch leaves a `NULL` Start and zero SLength. **Do not use SLength alone as a validity flag**. A safe contract would first require a successful per-entry load (not merely `SLength > 0`), valid pointer and a checked decoded byte length. `LbDataLoadAll` returns the **number of failed entries**, not an array of per-file success flags; it continues after one entry fails. Source inspection identifies these contracts, not tested malformed-input symptoms.

## MAIN-EXT-03 — File length and read/decompression sources need separate validation

`LbFileLengthRnc` (`gfile.c:31–55`) requests the RNC header length but rejects only a read returning -1; it does not insist on the exact header-byte count before testing for an RNC signature and declared length. `LbFileLoadAt` (`58–90`) requests `filelength` bytes and rejects only read status -1, **not all shorter positive reads**, then passes the supplied buffer to `UnpackM1`. These are previously identified partial-read/input-length hazards. Here they matter because the descriptor's apparent decoded length is not automatically proof the uncompressed buffer represents the required animation records. Decompression return length is checked by `LbDataLoad` for equality with expected `SLength`, but that alone does not validate the application's frame indices/links, or the physical compressed-byte read count.

**No real RNC, `.ani`, `.tab` or commercial resource file was opened in this audit. No crash, buffer overrun or memory leak has been reproduced.**

## MAIN-EXT-04 — Minimum extents must be derived per descriptor and per consumer

An illustrative *read-only* validator should enforce:
- `starts_ani`: on successful `hsta-0.ani` load, use its descriptor's decoded `SLength` with **16-bit record** indexing. Source-observed selectors include fireball 391 → 784-byte minimum, selected frame fixed byte offset `0x4b0` → 1,202-byte minimum, and `number_people` four-member branch at fixed `0xf3e + 2×member index`, ending at byte 3,910 when index 3 is reached. These do not certify a universal upper bound because many other selectors are computed dynamically. Do not assume legitimate asset size.
- `frames`: on successful `hfra-0.ani` load, derive count from *payload bytes / 8* only after checking file-format consistency; validate the current frame index, its `+6` next-frame index, then its next frame's `+5` completion flag and `+4` optional sound trigger. `frames_end` can provide a consistency cross-check but its mere presence is not a validation of index values.
- `elements_ani`: on successful `hele-0.ani` load, validate *payload bytes / 10* and each record's 16-bit next-link at `+8`, not only comparison against the base pointer. Check full record extent, sprite reference extent and a bounded traversal policy for synthetic invalid/cyclic links.

Use overflow-safe tests (`index <= (length - record_size) / stride` only after `length >= record_size`), **not unchecked pointer addition** for malformed selectors. Avoid classifying the original content format, sentinel or valid count before privately inspecting legal assets. These are acceptance tests for a future separately approved diagnostic validator, not engine implementation changes.

## Test states and verification levels

| Input state | Source-derivable state | Future observation |
| --- | --- | --- |
| All 3 ANI files valid | Pointer and `SLength` should reflect loaded decoded bytes | Confirm expected actual extents, linkage and dynamic selectors |
| hsta-0.ani absent | `starts_ani` can be NULL and `SLength` may contain unsigned-transformed negative result | Confirm failed-entry count, controlled abort vs later lookup |
| hfra-0.ani truncated | `frames`/end state depends on exact short read and decode result | Confirm exact rejection, no consumption of uninitialised bytes |
| hele-0.ani missing with other two present | Partial batch may retain successful other buffers | Confirm loader return count and whether draw-element chain is entered |
| SLength zero or nonmultiple of record size | Consumer indices may not fit full 8- or 10-byte records | Confirm explicit validation before reading |
| start selector valid but next frame / next element invalid | Initial index fit does not imply chain is safe | Confirm chained-index bounds checks |
| frame +4 requests sound with failed sound TAB | Audio and animation resource success flags are independent | Confirm status setter safely handles cross-resource partial failure |

**Performed:** read all cited C/header/assembly sources via GitHub, checked descriptor offsets and loader error branches, derived payload-vs-allocation distinction and test expectations, committed this report with subsequent read-back verification.

**Not performed:** code patch, synthetic fixtures, actual RNC decoding, retail animation assets, compilation, 32-bit Windows run, profiling, gameplay, audio tests, binary instrumentation, source/branch merge or saved-game changes.

**Next main audit gate:** finish caller-level evidence for load failure exits and maintain a formal input-contract register for the next permitted diagnostic test run. Coordinate shared animation and audio loader requirements with Graphics Audit and Mission Builder **by handover only**, without modifying their branch content. Protected v0.7, original campaigns, explosion visuals/sound, fixed 16 FPS gameplay and between-mission-only siloed saves remain unchanged.
