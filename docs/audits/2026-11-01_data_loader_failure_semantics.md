# Syndicate 2026 — Focused audit: Resource loader failure and ownership semantics

Date: 2026-11-01 (Australia/Brisbane)
Status: focused source review COMPLETED; overall forensic audit remains INCOMPLETE.
Branch: feature/v0.9-visual-showcase.
Protected baseline: main v0.7, a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: engine/bflibrary/src/general/gdata.c and engine/bflibrary/include/bfdata.h on this branch. Parent stage: docs/audits/2026-11-01_resource_loader_offsets.md.

## Findings, directly grounded in source
- bfdata.h defines a packed TbLoadFiles entry, documented size 44 bytes for the 32-bit original layout, consisting of 28-byte name, Start/SEnd pointer slots, SLength, Flags and Spare. Runtime struct size was not compiled or checked in this stage.
- gdata.c LbDataFree clears the optional SEnd pointer slot and frees the allocation referenced by Start when nonnull; it then nulls Start. LbDataFreeAll walks a descriptor array until the Start *slot address* is null, not until the allocation value stored in that slot is null.
- LbDataLoad first calls LbDataFree. For a resource whose modified filename begins with an asterisk, it allocates SLength bytes using LbMemoryAllocLow if Flags bit 0 is set or LbMemoryAlloc otherwise. Other resources use LbFileLengthRnc, allocate reported length plus 512, and load through LbFileLoadAt. It sets SEnd to Start plus SLength when successful.
- LbDataLoadAll clears the previous descriptor allocations, then visits every entry, counting failures rather than stopping at the first failure. Its return value is the number of entries that failed; it does not guarantee a fully populated resource set on nonzero return.
- The file-load mismatch path in LbDataLoad assigns Start=0 and SEnd=0 without calling LbMemoryFree on the allocation before clearing the pointer. This is a conditional lost-allocation/leak path in the inspected source, not a Windows runtime finding. Repair is outside audit scope.
- The original high-resolution descriptor points to WScreen as a loader-managed allocation, so treating its address as persistent across LbDataLoadAll/LbDataFreeAll calls would be unsafe. Other application-owned buffers (including VGABuffer) have a distinct lifetime.

## Consequences
A future renderer or asset workflow must not assume all resources loaded successfully, retain references across data reload/free, or substitute pointers into the loader-owned WScreen memory without explicit ownership and lifecycle tests. Investigation does not authorise changing the loader or gameplay behaviour.

## Verification ledger
PASS: GitHub read of gdata.c and bfdata.h; direct inspection of loader/free/return paths.
NOT RUN: native compile, sizeof assertion, memory sanitizer, allocation failure injection, actual game asset loading, Windows gameplay.
OPEN: original assembly callers' handling of nonzero LbDataLoadAll result; alternate MCGA descriptor free path; loader struct ABI under actual build options.

## Next
Trace assembly response to loader return codes and remaining release/cleanup branches. Review exact original descriptor representation against packed TbLoadFiles, using the original assembly blob, and separate any future proposed fixes from this read-only audit. No engine, workflow or protected main changes. Rollback: documentation only on development branch.
