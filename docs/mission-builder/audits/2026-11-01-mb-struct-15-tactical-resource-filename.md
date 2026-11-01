# MB-STRUCT-15 — Tactical mission resource filename traced

Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: STATIC SOURCE AUDIT ONLY.

Source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Source-confirmed resource chain
- `level_fname` is a mutable string initialized to `data/game01.dat` (assembly lines 80475–80477).
- The mission setup routine `func_25a6f` passes `$level_fname` to `load_map_level` in both multiplayer (28493–28498) and single-player (28531–28536) branches. The mission-number parameter is sourced respectively from `data_5e553` and `current_levno`; these should not be assumed identical without scenario verification.
- `load_map_level` (27845–27867) writes the decimal tens and units into filename offsets +9 and +10. In the initial `data/game01.dat` string those positions are the two digits in `01`. Hence the native tactical resource pattern is **`data/gameNN.dat`**, within the observed two-digit formatting path; mission index range and invalid index behaviour remain unverified.
- It calls `clear_savegame` and uses `LbFileReadRNC` with `level__Seed` (27873–27880, 27917–27919). RNC output shape, bounds and failure handling remain subject to MB-STRUCT-14.
- There is also a distinct format literal `aGameDat = "data/game%02d.dat"` at 80380–80382 used by a different flow around 1232–1244. Its coexistence corroborates the name pattern but does not establish equivalence of the associated caller semantics.
- `load_text` (55249–55280) uses `data/miss%02.2d.dat` and `data/miss%01d%02.2d.dat` literals (82522–82525). These are text-resource reads, not the reviewed tactical level image load.
- `mission_load_files` (82620 onward) includes separate map, font, collision and block resources, e.g. `data/map` with two formatted digits. Do not treat the map resource as the gameNN objective image.

## Correction/coordination
MB-STRUCT-14 properly withdrew an unsupported assertion until caller evidence existed. This audit **confirms** the `data/gameNN.dat` tactical mission resource pattern from actual caller, initialized buffer and digit-write sites. Do not remove MB-STRUCT-14's decompression safety caveats: identifying a filename does not validate format, decompressed byte capacity or safe writable offsets.

## Next Mission Builder audit
MB-STRUCT-16: inspect mission-number bounds and the distinction between `data_5e553` and `current_levno`, and document how Mission Builder candidate custom resources could be selected without changing original campaigns. No new selector or engine edit is authorized by this audit.

## Verification
Inspected assembly source and cross-checked literal and caller references. No source-code modification, binary compilation, asset unpacking, retail-data testing or gameplay execution.
