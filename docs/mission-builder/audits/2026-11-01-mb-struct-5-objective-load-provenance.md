# MB-STRUCT-5 — Native player objective table load provenance
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC SOURCE AUDIT ONLY; no binary writer or runtime tests
Source: engine/src/syndre.sx, GitHub blob c35e22621a93d1e07c44d40de8867446ff7c64bf; engine/src/game_data.h

## Verified source chain
1. `load_map_level` (assembly lines 27845–27933) first calls `clear_savegame`, sets `current_levno`, formats the numeric `data/gameNN.dat` filename and calls `LbFileReadRNC` with destination `level__Seed` in the inspected display branches (27873–27880; 27917–27920). It then reads `level__MapNumber` and calls `init_level_data`.
2. `clear_savegame` (27627–27636) zeroes memory from `level__Seed` to `level__End` using `ac_LbMemorySet`. Thus stale objective state should be cleared before each load in this path; this is only a source-level observation, not a malformed-file test.
3. `init_level_data` (27712–27745) starts with map and sprite setup and invokes `init_computer_players` for non-multiplayer missions. This is not evidence that it converts player objectives from a separate data format.
4. Assembly static labels place `level__MapNumber` at 0x9be30, `level__Objectives` at 0x9be3a and `level__ComputerPlayersCount` at 0x9beaa (85760–85783). Difference 0x70 = 112 bytes = 8 objective records x 14 bytes. Objective record layout accessed by the handlers: runtime state dword +0, type word +4, entity-offset word +6, additional words +8/+10, and remaining bytes +12/+13 not yet decoded.
5. First objective record's labels `data_9be3e` / `data_9be40` / `data_9be42` / `data_9be44` refer to fields at +4/+6/+8/+10. Loop stride is 0x0e; these symbols are not independent arrays.
6. The source passes `level__Seed` as the RNC destination and contains no separate, explicit objective-table copy in the inspected loader. This supports a candidate in-memory layout for validating decompressed mission data, NOT a verified on-disk editable offset or proof of exact payload length/packing. The RNC loader's length and bounds remain an independent safety gate.

## Safety/authoring implications
- The objective records are structurally part of the broader level-data memory region, not proven standalone data files.
- Do not write directly at a presumed offset into a compressed gameNN.dat file. File header, decompressed length, encoding, version, endianness and retail-data content require independent verification.
- An objective inspector should accept a *bounded, validated decompressed buffer* and refuse to parse when it is too short to cover the eight-record region. The exact offset relative to `level__Seed` must be calculated from verified symbol offsets and reconciled against real asset data, not simply assumed from an address displayed by the disassembler.
- Validate all referenced people/entity offsets against the actual loaded array bounds before dereferencing or presenting semantic labels.
- The first prototype is read-only; no direct use of the unbounded legacy RNC loader for untrusted builder input.
- Keep CPObjective AI records distinct, original missions unchanged, and all campaign saves between missions only.

## Remaining gates
1. Determine `level__Seed` address, `level__End` and objective table relative offset by source-symbol reconciliation, then validate capacity and decompress output boundaries.
2. Verify exact original gameNN.dat payload layout with legally supplied retail assets or controlled fixtures; no retail data is committed.
3. Trace the remaining bytes in objective record (+12/+13), unused-entry rules, and all entity reference validations.
4. Add read-only synthetic parsing tests before any edit/save/export feature; test failed/truncated RNC loads and rejection without partial parsing.
5. Cross-workstream review before editing mission loader, level-data layout, or palette/resource dependencies.

## Verification
Source inspection only, via connected GitHub blob and existing headers. No code edits, execution, CI build, retail asset tests or gameplay test. Baseline v0.7 and original mission mechanics preserved.
