# Syndicate 2026 — Main Systems Audit: Startup Display Resource Loads and Pointer Sprite Guards

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch/workstream:** MAIN/core systems, `main`; documentation only.  
**Evidence:** GitHub `main` source `engine/src/main.c`, `engine/src/display.h`, `engine/bflibrary/src/general/gdata.c` and complete original `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines).  
**Predecessor:** `docs/audits/2026-11-01-main-mission-load-failure-to-initialisation.md` (commit `625bb61b6ae6327dc72cde2439972b202c49f8b4`).  
**Method/status:** verified *source* paths, no Windows execution, no code changes.

## MAIN-START-01 — Default graphics mode is the mission loader's DrawFlags=2 branch

`engine/src/display.h:30–35` defines `DrwF_ScreenVres16 = 0x2` and `DrwF_Unkn04 = 0x4`. `engine/src/main.c:266–274` sets `DrawFlags = DrwF_ScreenVres16` by default. `process_options` supports `-H` -> Vres16 and `-L` -> Unkn04 (`main.c:210–216`). Startup `main.c:311–319` loads `load_files_vres16` or `load_files_mcga` according to these values.

This confirms the prior mission-report's `DrawFlags == 2` branch is the **default configured** branch, rather than an arbitrary hypothetical mode, and `DrawFlags == 4` is a separately selectable alternate path. This does **not** prove both paths are exercised in current native Windows CI or reach identical mission pointer states. Flag precedence in start-up conditionals must be preserved.

## MAIN-START-02 — Initial batch success is not checked before pointer sprite setup/use

`main.c:311–320` calls `LbDataLoadAll(load_files_vres16)` or `LbDataLoadAll(load_files_mcga)` without checking the returned number of failed entries, then invokes `ApSpriteSetup_ForceHeight(pointer_sprites, pointer_sprites_end, pointer_data)`. Subsequently `main.c:327–328` uses `pointer_sprites[1]` for `mouse_sprite` and `LbMouseSetup`, again without a pointer-table readiness or length check.

`syndre.sx:83626–83674` declares the actual startup lists: Vres16 uses `data/hpal01.dat`, `data/hpointer.tab` (into `pointer_sprites` with `pointer_sprites_end`), `data/hpointer.dat` (into `pointer_data`), and `data/hreq.dat`; alternate MCGA loads `data/lpal01.dat`, `data/lpointer.tab`, `data/lpointer.dat`, and `data/lreq.dat`. Each list also has an internally allocated `WScreen` resource (filename `*` with declared allocation size). The pointer sprite slots are shared between the modes; loading one list does not prove the other is active.

`LbDataLoadAll` (`gdata.c:105–130`) frees the old list resources, attempts each file and returns a failed-entry count; a single missing pointer TAB need not abort all loading. The unchecked result creates a **conditional** path to using absent/undersized pointer-sprite data.

## MAIN-START-03 — Original pointer-sprite setup can skip an empty table, but caller still indexes it

Direct original assembly routine `GLOBAL_FUNC(ApSpriteSetup_ForceHeight)` at `syndre.sx:31546–31580` compares the supplied `start` and `end` pointers and loops over **6-byte sprite records**, changing the stored sprite data pointer and sprite height byte for selected draw modes. It returns immediately when the initial `start >= end` comparison is satisfied. It does not verify ownership of the end pointer, size/alignment, non-null pixel buffer or source sprite-table contents inside the loop. **Therefore do not claim the setup always dereferences a NULL table**; the particular pointer comparison can skip iteration for an empty range. Nevertheless the following `main.c` source uses `pointer_sprites[1]` without establishing the presence of two complete sprite entries. A table with fewer than two 6-byte records would be insufficient to address `[1]`, irrespective of the setup function's return.

This is a specific first-startup validation gap with directly observed source callsites; it is **not** a verified retail file defect, a reproducible exception or proof that modern Windows builds fail to launch.

## MAIN-START-04 — Startup and mission failure gates are distinct

The successful startup display pointer table is required *before* `main.c:348` enters `syndicate()`. The mission `DrawFlags=2` branch later loads another list (`mission_load_files`) and may reach `init_hires_blocks`, `init_map_data`, sprite setup, animation frame lookups and sound calls without testing mission batch results; see predecessor report. The startup gate is distinct from those mission gate(s): a test with missing `hpointer.tab` may fail earlier than a test with missing `map??.dat` or `hblk01.dat`. Avoid attributing early startup errors to the explosion/frame system.

## Diagnostic acceptance matrix — not yet run

| Scenario (disposable legal-data copy or synthetic input only) | Observe |
| --- | --- |
| Default Vres16 baseline with all pointer assets | Exact failed count, pointer table size/range, setup result, first mouse sprite use, enters menu |
| Missing `hpointer.tab`, valid `hpointer.dat` | failed-entry count and first safe error boundary vs invalid use of `[1]` |
| `hpointer.tab` contains fewer than two 6-byte records | setup iteration, following indexed mouse sprite access, fail-safe behaviour |
| Missing `hpointer.dat` with TAB present | pointer relocation behaviour and any later pixel buffer use |
| Alternate `-L`/MCGA equivalent files | independent pointer ownership, setup branch and subsequent mission mode |
| Successful startup plus missing mission `hblk01.dat` or `map??.dat` | distinguish later mission init failure from first startup failure |
| Missing initial WScreen allocation or palette | loader failure, display init/teardown and safety; do not infer current resilience |

**Performed:** exact source inspection and static mode/record-width reasoning, creation of report and planned read-back verification.  
**Not performed:** Windows build/launch, asset sizes, corrupted-resource reproduction, display/audio testing, memory instrumentation, compiler diagnostics, source edits, commits to other branches or saved-game tests.

**Next MAIN checkpoint:** complete source-only startup loader and direct RNC read error-boundary review, then separately approve synthetic fault injection in an isolated 32-bit Windows copy. Protect v0.7, original campaigns/expansions, mouse/keyboard, tactical 16 FPS, explosion visuals and sound and only between-mission campaign-isolated saves. Graphics Audit and Mission Builder remain separate; coordinate any shared loader contract by handover.
