# MB-STRUCT-6 — Objective record offset reconciliation and read-only validation contract
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder schema audit only
Status: Static source evidence; not an implemented parser or tested game feature
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Symbol arithmetic
- level__Seed 0x80108 (line 85702); level__Objectives 0x9be3a (85770); level__ComputerPlayersCount 0x9beaa (85782); level__CPObjectives 0x9beb2 (85795); level__End after 0x780 CPObjective bytes at 0x9c632 (85800).
- Difference Objectives-Seed = 0x1bd32 = 113970 bytes.
- Eight player objective entries, stride 0x0e = 14, occupy [0x9be3a, 0x9beaa), 112 bytes. Consequently a bounded decompressed buffer interpreted from the same base would require at least 113970+112=114082 bytes to inspect the entire objective table. This is a **candidate memory-layout-relative requirement**, not yet evidence of the correct RNC output length or file format.
- Level region Seed-End = 0x1c52a = 116010 bytes. This is the clear_savegame memory clear length in inspected code, **not** proof that retail RNC output always fills all of it.

## Field mapping refined
Within one 14-byte player objective record:
+0..3 runtime state (dword); +4..5 objective type; +6..7 linked entity offset or type-specific parameter; +8..9 first coordinate/parameter; +10..11 second coordinate/parameter; +12..13 third parameter word.
Evidence for +12: assembly data_9be46 at 85780 is read as a word by func_16831 (8816) and func_16c83 (9217) together with +8/+10 values, comparing against another entity's +8 field. Therefore +12 is NOT an unused padding field; exact semantics are context-dependent. Do not call it height or altitude without broader source/retail confirmation.

## Minimum read-only inspector contract — proposal, not implemented
1. Accept only a previously validated, bounded decompressed mission payload with explicit byte length and provenance; never pass untrusted inputs to legacy unbounded RNC routines.
2. Verify expected layout/version and minimum buffer length before reading; reject any truncated data. Distinguish level-memory-relative offsets from offsets in compressed gameNN.dat.
3. Parse eight 14-byte entries into immutable records, with raw unsigned fields and separate interpreted annotations. Record type range and explain unknown types without executing them.
4. Check coordinate/parameter access according to handler type and scan all reference-using handlers; do not reinterpret raw +6 entity offset as a validated pointer.
5. Verify referenced entity address belongs to the correct loaded region, is aligned to the appropriate record type and has sufficient bytes before displaying or dereferencing links; reject invalid nested links and cross-record relationships.
6. Flag unresolved unused-slot behaviour and type-16 dependencies. No write/edit/export facility until original resource fixtures and all safety gates are passed.
7. Never modify retail files, original mission rules, AI CPObjective table or campaign-save semantics. Between-mission-only saving remains approved.

## Verification and next gate
Static GitHub assembly inspection and arithmetic only. No executable, unit tests, Windows build, RNC decompression, retail assets, gameplay or actual inspector implementation. Next MB-STRUCT-7: trace origin of entity table bounds and check all object types and link rules needed for robust read-only validation.
