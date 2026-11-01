# Syndicate 2026 — WScreen allocation semantics and loader failure propagation
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT, no engine changes or executable tests.
Repository: mh83invest-creator/Syndicate-2026; main.
Follows: docs/audits/2026-11-01-wscreen-descriptor-alias-audit.md.

## Source files reviewed
- engine/bflibrary/include/bfdata.h: packed TbLoadFiles struct (44 bytes, 32-bit ABI), FName[28] offset 0, Start offset 28, SEnd offset 32, SLength offset 36, Flags offset 40.
- engine/bflibrary/src/general/gdata.c: LbDataLoad, LbDataLoadAll and LbDataFreeAll.
- engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf: descriptors at 83626–83656; assembly calls at 31138–31142, 31239–31253 and 31325–31330.

## Findings
1. LbDataLoad() first calls LbDataFree(load_file) and then chooses allocator: LbMemoryAllocLow if Flags bit 0 is set, otherwise LbMemoryAlloc.
2. If modified resource filename starts with '*', the loader allocates exactly load_file->SLength; no file read or 512-byte cushion applies. In syndre.sx, both load_files_vres16 and load_files_mcga begin with a '*' byte and direct their Start to WScreen.
3. vres16 descriptor supplies 256048 as SLength; MCGA descriptor encodes 0x0003E810 = 256016 as SLength in bytes. Subject to correct 32-bit struct interpretation and successful allocation, these are the requested WScreen backing allocations, not merely unrelated constants.
4. Normal file-backed resources use LbFileLengthRnc() and allocate slength + 512, followed by LbFileLoadAt(). An allocation failure returns -100; invalid length or mismatched read returns -101. On failed LbFileLoadAt, code resets the Start/SEnd and SLength but does not visibly free the freshly allocated block before losing the pointer: potential conditional memory leak, not demonstrated at runtime.
5. LbDataLoadAll counts failed entries and returns that count. Inspected assembly startup branch at 31251–31254 calls ac_LbDataLoadAll without an immediate check of EAX return count. Hence failed allocation may leave a NULL WScreen while subsequent code assumes valid pointers. No crash is reproduced.
6. The assembly shutdown path calls ac_LbDataFreeAll with load_files_vres16 at 31328–31330. Whether MCGA and high-res variants are always exclusively initialized and safely freed in each exit path remains to be established; do not infer a verified leak or double-free.
7. Previous pointer-provenance audit identifies BScreen offsets of 64000, 128000, and 128024 into WScreen, and an assembly temporary WScreen alias. These fit only with appropriate successful allocations and lifetimes. The existence of 256016/256048-byte backing requests does NOT itself prove all map draw addresses are valid.

## Implications and remaining gates
- Establish precise mode/flag entry conditions and loader success before map_selection. Confirm BScreen active offset for each call path.
- Compute worst-case _draw_map_area destination byte offset using all 50 built-in coordinates with the exact active WScreen/BScreen pointer and allocation; differentiate in-bounds row wrapping from physical out-of-bounds access.
- Inspect error handling around LbDataLoadAll for startup/mode changes and BScreen initialization. Treat any fix as a separate reviewed change.
- Verify failed-file allocation cleanup with a dedicated synthetic/fault-injection test, without changing baseline behaviour.
- Windows 32-bit build, real asset inspection, visual gameplay and memory tooling are still unperformed.

## Cross-workstream contract
Original Campaign, American Revolt, Mission Builder / World Domination, legacy saves and 16 FPS simulation remain unchanged. New custom resources must validate actual capacity and loader success before use. Campaign save namespace implementation is a separate blocked design gate.

## Handover
Performed: GitHub source reads and field-offset/byte-size reasoning. Not performed: code patch, compilation, runtime test, gameplay test, file modification outside this audit note. Preserve v0.7 fallback.
