# Syndicate 2026 — Source audit: Mode selection and resource teardown reachability

Date: 2026-11-01 (Australia/Brisbane)
Status: completed focused static-source inspection; project-wide audit incomplete.
Repository: mh83invest-creator/Syndicate-2026
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40
Starting source: original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
Ending gameplay source: unchanged. No gameplay, workflow or retail-asset modifications.

## Source-confirmed findings
1. At assembly lines 31003–31010, startup initializes DrawFlags to 0x2, the high-resolution path. This identifies the normal initial selection, not the complete set of reachable configurations.
2. Lines 31138–31145 allow setting DrawFlags to 0x2; lines 31187–31189 can set it to 0x4, selecting an alternative mode in that option-processing path.
3. Lines 31238–31253 choose load_files_vres16 when bit 0x2 is set, otherwise load_files_mcga when bit 0x4 is set, and call ac_LbDataLoadAll. The immediate subsequent instructions proceed without an explicit error-count check.
4. Lines 31308–31314 show an early branch around the call to syndicate, but that path still reaches the common cleanup label at 31326–31330. This does not establish what every mode and all possible exits do.
5. The inspected common cleanup at 31327–31330 unconditionally passes load_files_vres16 to ac_LbDataFreeAll. For a run that loaded only the MCGA descriptor, this is a potential descriptor mismatch. The original assembly control flow, pointer aliasing and alternate cleanup pathways are not comprehensively proven; this is a conditional source concern, not a reproduced leak or double free.
6. Earlier stages established that the resource loader frees allocations by their descriptor pointer slots, and the two descriptors have distinct WScreen lengths (256048 high-res; 256016 MCGA). Shared pointer slots could affect what is actually freed, so do not assert leak severity without an isolated mode test.

## Verification ledger
PASS: Direct GitHub blob read of original assembly, source line checks and correlation with gdata.c resource loader.
NOT RUN: Windows compile, low-resolution game launch, allocator instrumentation, full call graph, automated tests, actual assets, gameplay regression.
UNRESOLVED: exact mode reachability in current Windows wrapper, terminal cleanup behaviour for every branch, failures during partial loads, effects of shared descriptor slot reuse.

## Next audit
Audit original resource-loader descriptor pointer-slot aliasing and actual common-shutdown paths; then develop a test-only harness specification for loading/freeing controlled synthetic descriptors, without introducing any test code into protected main. Keep source-level risks separate from runtime-confirmed defects. Preserve mission timing, camera, audio, original assets and save data.

No source changes. Rollback: this feature-branch documentation can be disregarded without affecting the protected build.
