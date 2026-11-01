# MB-STRUCT-4 — Remaining player-objective handlers and secondary dispatch
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC AUDIT ONLY. No engine edits, test execution or authored missions.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 491–528, 806–1103.

## Primary objective type handlers — source-verified behaviour
- Type 10 -> func_10966 (806–831): scans people records at data_883f0 through last_person, 0x5c stride. Checks byte +0x18 equals 1, flag bit 0x4 at +0x1c and bit 0x1 at +0x0b; marks record complete only if scan reaches last_person. The entity classification and intended gameplay wording remain unverified.
- Type 11 -> func_109bf (835–892): scans people records, filters +0x18==1 and bit 0x10 at +0x1d, resolves +0x20 linked entity relative to level__things_base, checks whether linked person is in current player's selected range, and counts qualifying entities without bit 0x1 at +0x0b. Marks objective complete if count is zero. Do not assume a specific rescue/kill objective label.
- Type 14 -> func_10a90 (896–907): resolves +6 word offset relative to level__things_base, marks complete when referenced +0x0b bit 0x1 is set. Similar predicate to type 2, but index does not establish semantic equivalence.
- Type 15 -> func_10ac8 (911–950): resolves entity via offset +6, follows its +0x1c word link, checks whether linked person is in current player's range, marks complete if so. If source entity +0x0b bit 0x1 set, invokes level_failed. Both completion and failure possibilities must be exposed to validation.
- Type 16 -> func_10b65 (954–1103): enumerates current-player people (0x5c stride), evaluates status flags and 2D distance via getrdist against words at objective +8/+10 (symbols data_9be42/data_9be44), using threshold 0x300. It then invokes a secondary objective-table scan. It is not a simple single-entity objective; raw interpretation as “go to marker” is NOT established.

## Secondary dispatch and objective relationships
- vtable_10594 (512–528) only routes type 1 -> func_10c99 and type 5 -> func_10cd9; other entries jump_10d15 (no additional secondary predicate in this table).
- func_10c99 checks a referenced entity's +0x0b bit 0x1 and its 2D distance against the current type-16 objective's coordinate words; if the entity is alive/unflagged and distance >0x300, it clears the accumulator via jump_10d13.
- func_10cd9 checks referenced entity distance to those same coordinates, also clearing accumulator when distance >0x300.
- Thus type 16's completion can depend on other entries in the eight-slot objective table. Independent per-row editing without cross-objective validation is unsafe.

## Known structural constraints and remaining unknowns
1. The player objective table is eight 14-byte records; type field offset +4, entity offset +6, and type-16 coordinate words +8/+10 are evidenced by source access. First dword offset 0 stores runtime completion state.
2. Entity pointers derive from 16-bit relative offsets into level__things_base. Safe entity type, alignment, allocation extent and required linkage must still be established.
3. Flags and entity-category semantics are not sufficiently decoded to label objective handlers for a public editor.
4. The exact mission-file serialization, RNC input sizes, unused slot representation and initialization are not yet proven.
5. Do not let users author arbitrary type values or entity offsets; build a read-only validator first, with linked-objective consistency checks and explicit rejection diagnostics.

## Next Mission Builder audit
Trace player-objective table initialization from mission data through load_map_level and the level__Objectives buffer; identify precise file offsets and mapping into mission data, and independently inventory entity type/length bounds. Do not edit save-menu mechanics, graphic effects, or gameplay.

## Verification
Reviewed original assembly by GitHub blob. No build, runtime test, synthetic fixture test or retail asset verification. Documentation-only; v0.7 and classic gameplay preserved.
