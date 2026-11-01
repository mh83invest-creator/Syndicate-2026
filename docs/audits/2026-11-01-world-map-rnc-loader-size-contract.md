# Syndicate 2026 — World-map asset-size and RNC loader contract
Date: 2026-11-01 (Australia/Brisbane)
Status: static source audit only, no code/build/gameplay test.
Source: engine/bflibrary/src/general/gdata.c, gfile.c; engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
Previous: docs/audits/2026-11-01-world-map-complete-destination-bounds.md.

## Source findings
- map_selection (syndre.sx 31719–31727) loads mmapblk_load_files and immediately renders without testing the returned error count. The descriptor at 83675–83683 targets world_map with data/mmapblk.dat.
- LbDataLoad (gdata.c) obtains file-backed length using LbFileLengthRnc, allocates reported length + 512 bytes, then requires LbFileLoadAt return to equal the reported length. The returned length represents decompressed size if the RNC header is recognized; otherwise physical file size.
- LbFileLengthRnc (gfile.c) opens the file, inspects an RNC header, and reads the declared unpacked byte length when the signature matches. Uncompressed files fall back to LbFileLengthHandle.
- LbFileLoadAt (gfile.c) reopens and reads using that reported length, then calls UnpackM1; on a nonnegative unpack result, returns decompressed length (or original filelength when unpack reports 0). A negative decompression result is treated as failure.
- Source-derived map drawing minimum is 50 × 64 × 44 = 140800 bytes (for contiguous 0..49 territory block indexing), not a verified decompressed mmapblk.dat size. The loader's +512 allocation cushion does not replace validation of 140800 bytes at the consumer.
- LbDataLoadAll counts and returns failed resource loads; map_selection does not immediately check that result. Missing/corrupted data can leave the pointer unusable before rendering; this is conditional source-level risk, not a reproduced fault.
- gdata.c failed LbFileLoadAt branch resets resource pointer to zero without a visible LbMemoryFree of the just-allocated block: possible conditional leak; requires focused fault testing before a fix.

## Audit decision
All 50 built-in destination rectangles have source-derived upper bounds within a valid 64000-byte target frame (maximum 59214), but SOURCE world_map length, successful allocation and runtime pointer integrity remain unverified. Destination geometry and source resource length are separate requirements. Do not implement generalized clipping or assume map data safe.

## Next gates
1. Inspect UnpackM1 and RNC header size behavior for malformed/truncated inputs and read-count validation.
2. Verify decompressed mmapblk.dat length with legitimately obtained game data; check >=140800 bytes before any custom resource support.
3. Windows 32-bit runtime tests with intact/truncated/missing assets and memory diagnostics; do not run destructive tests on live save or retail files.
4. Coordinate Mission Builder resource validation contract, protect v0.7, Original Campaign, American Revolt and save namespaces.

## Verification
Performed source-level inspection and checked static arithmetic. Not performed Windows builds, game launches, retail resource inspection, fault injection or code modification.
