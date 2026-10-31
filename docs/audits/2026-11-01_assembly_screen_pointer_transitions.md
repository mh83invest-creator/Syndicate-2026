# Syndicate 2026 — Focused Audit: Original Assembly Screen Pointer Transitions

Date: 2026-11-01 (Australia/Brisbane)
Status: COMPLETED focused static-source audit; overall forensic audit INCOMPLETE
Branch: `feature/v0.9-visual-showcase`
Protected baseline: `main` v0.7, `a22b0b9580a9710b4e693e88854351835ef1ee40`
Evidence: GitHub assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, 85,803 source lines, inspected directly by blob SHA (normal text-file fetch was empty). Related C evidence: `engine/src/display.c`, `engine/src/display.h`, `engine/bflibrary/src/x86-win-sdl2/sscreen.c`, earlier audit reports.

## Completed source findings
1. **Distinct exported pointer slots:** `syndre.sx:83901–83910` declares consecutive distinct 4-byte storage locations `WScreen` (original address 0x55368), `VScreen` (0x5536c) and `GraphicsPalette` (0x55378), with intervening `BScreen` and `UScreen`; `PalMem` is elsewhere. Distinct slots do NOT establish that their pointed-to memory cannot overlap.
2. **Mission VScreen assignment is explicit:** `syndre.sx:27866–27880` tests `DrawFlags` for 0x2 and, on that path, calculates `VGABuffer + 0x6400` (25,600 bytes), writing the result to `VScreen`. Separately calculates `UScreen = WScreen + 0x1f408` and `BScreen = WScreen + 0x1f418`. This is source-level evidence that these are alternate subregions of a larger game-controlled work buffer during the mission path.
3. **Title/select phase uses another VScreen value:** `syndre.sx:54253–54261` calculates `BScreen = WScreen + 0xfa00`, another scratch pointer `data_60ab0 = WScreen + 0x1f400`, `PalMem = WScreen + 0x20400`, then assigns `VScreen = VGABuffer` (without the mission's 0x6400 offset). Therefore `VScreen` is **phase-dependent**; treating it as a permanently offset scene pointer would corrupt menu/title operation.
4. **Post-level phase updates scratch/palette area:** `syndre.sx:55008–55023` assigns `BScreen = WScreen + 0x1f400`, `data_60ab0 = WScreen + 0x2ee00`, and `PalMem = WScreen + 0x2fe00`, then calls `ac_AppScreenSetup`. These offsets differ from title-select calculations; the actual allocation and bounds must be tracked per phase.
5. **Temporary WScreen substitution confirmed in original assembly:** `syndre.sx:56471–56473` saves current `WScreen` in EBP, assigns `WScreen = BScreen` for a drawing path, and `56498–56506` restores `WScreen` conditionally after `LbSpriteDrawMCGA`. This establishes a real temporary exported-pointer reassignment, consistent with the library's draw-surface recreation check; not every control-flow exit or failure case was audited.
6. **Original cursor is layered within mission swap:** `syndre.sx:17894–17974` runs cursor-related `set_block_changes`/ `PlaceMouse_A` before calling `ac_update_vscreen_whole_vres16` and `ac_swap_wscreen` in the non-DOS path; it then calls `RemoveMouse_A`. The SDL swap independently uses Bullfrog mouse begin/end hooks. A proposed lighting overlay must not assume there is only one cursor composition mechanism or that the scene is cursor-free at swap time.
7. **Original palette data and API argument are not trivially identical:** `syndre.sx:27780–27793` reads a palette resource into the buffer pointed to by `GraphicsPalette`, then clears and passes `WScreen` to `ac_LbPaletteSet`; `syndre.sx:244–249` separately passes `GraphicsPalette` into the palette setter during the mission loop. This can be a deliberate transition sequence; no error conclusion or repair is justified without game data and runtime observation.

## Static-reference occurrence inspection
A read-only scan of the **pre-data region** of the 85,803-line assembly blob found 23 textual uses of `EXPORT_SYMBOL(VScreen)`, four uses of `EXPORT_SYMBOL(PalMem)`, two uses of `EXPORT_SYMBOL(GraphicsPalette)`, and 48 uses of `EXPORT_SYMBOL(WScreen)`. These are **lexical counts of exact expression spelling only**, not a complete def-use analysis: data tables, alternate symbol spelling, pointer-indirect writes and macros may add accesses.

## Graphics feasibility decision
**No renderer hook selected.** The original pointer values change across mission, menu and level-finished phases. A post-process or localized lighting implementation must prove its backing buffer, stage, format/pitch, pointer lifetime, cursor/HUD separation and redraw order across these transitions. Palette-only dimming cannot meet the requested source-localized lighting specification.

## Verification ledger
| Item | Status |
|---|---|
| Original assembly blob retrieval with SHA and line count | PASS |
| Source read: pointer slots, mission/title/level-finished assignments | PASS |
| Source read: temporary pointer replacement and cursor swap | PASS |
| Full transitive alias/def-use proof for 85,803-line assembly | NOT DONE |
| Native Win32 build, automated tests, renderer capture, Windows gameplay | NOT RUN |
| Original asset/palette validity and lighting feasibility at runtime | NOT VERIFIED |

## Continuation and rollback
Next inspect the bounds and pointer lifetime of the original WScreen allocation relative to `BScreen`, `UScreen` and `PalMem` across the resource loader; check the original menu/title composition and possible clipping boundaries. After that, prepare an *isolated test-only* SDL draw-surface/format/pitch harness plan. The runtime/asset gate remains required. Never insert a graphic hook on the basis of these findings alone. No source, CI, gameplay or runtime files changed; only this audit report/index are committed on the feature branch. `main` stays intact.
