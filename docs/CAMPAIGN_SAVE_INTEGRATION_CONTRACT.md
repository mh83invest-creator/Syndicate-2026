# Campaign Identity & Save Isolation — Cross-Workstream Integration Contract (Draft)
Date: 2026-11-01
Status: DESIGN / AUDIT ONLY — NO CODE IMPLEMENTED
Related: docs/SHARED_DEVELOPMENT_HANDOVER.md; feature/mission-builder-audit docs/mission-builder/HANDOVER.md

## Approved behavioural requirements
1. Original Campaign and American Revolt retain their original gameplay and save compatibility.
2. World Domination and Custom Campaigns are separate selectable modes.
3. A campaign's Save, Load and Continue operations must only expose and access its own saves.
4. Equal slot numbers in different campaign namespaces must never overwrite one another.
5. Existing saves must not be deleted, moved, overwritten, misidentified or silently migrated.

## Verified current integration surface
- `engine/src/game_data.c`: `load_save_text(char*,int)`, `load_player(int)`, `save_player(char*,int)` build `save/%02d.gam`; calls to `LbFileOpen`, `LbFileRead`, `LbFileWrite`.
- `engine/src/game.c`: `game_transform_path_full()` treats `save/` as special, routing under `GetDirectoryUser()`.
- `engine/src/game_data.h`: `CPObjective`, packed fixed-size array `level__CPObjectives[128]`.
- `engine/src/main.c`: enters assembly `syndicate()`; campaign selection currently untraced.
- `engine/conf/wrappers_game.conf`: wrappers include save/load functions.
- `engine/src/syndre.sx`: tracked 1,848,334-byte assembly source; large enough that connector returned empty for line-range reads and raw endpoint rejected oversized response. Its contents are not fully audited; must not state selection symbols or hooks as known.

## Proposed contract, subject to source validation
- Define a stable campaign ID established at campaign selection BEFORE a save listing or load call.
- Route all save enumeration, read, write and Continue logic through a campaign-qualified namespace. Reject invalid/wrong-mode identifiers at the data boundary, not merely the UI.
- Hold original binary `.gam` payload layout fixed while analysing legacy files.
- New persistent world-mode state must be versioned and separately identifiable from old `.gam` files. Actual storage format/path not selected.
- Storage options to evaluate (NOT yet approved): per-campaign directories, namespaced filenames, or indexed metadata with namespace-aware loaders.
- Legacy classification: a saved file's campaign origin must be grounded in actual recorded metadata or verified engine behaviour. If ambiguous, preserve it untouched and obtain a migration decision rather than guessing.
- Custom Campaign isolation granularity (per custom campaign or shared) still requires explicit decision.

## Required implementation gates
G0: Map exact campaign selector assembly and all save/load entry points; inventory preexisting save-screen branches.
G1: Specify campaign ID representation, namespace, error behavior and compatibility.
G2: Implement on isolated branch with no classic campaign gameplay changes.
G3: Automated tests: same-slot separation, listing isolation, cross-mode rejection, Continue isolation, corrupt/unknown save handling, file-preservation and safe path handling.
G4: Windows 32-bit build and original retail-data gameplay tests for both classic modes + new modes.
G5: Cross-workstream review and pull request; include versioned docs/releases handover as required by docs/HANDOVER_POLICY.md.

## Open risks
- Assembly screens may bypass C wrapper save APIs; audit all references before selecting interception layer.
- Existing save buffers use fixed binary offsets; ABI and compatibility require special care.
- File-path remapping may not affect all direct file access or screen slot enumeration.
- World mode may require persistent strategic data beyond native save format.

## Verification
Static inspection of repository files only. No changes to engine, no build or gameplay tests. 

## Addendum — second file access route (2026-11-01; MB-AUDIT-0.7)
`engine/src/dos.c:dos_low_level_open()` calls `game_transform_path()` before `dos_open()`, while native `game_data.c` uses the `LbFile*` API for save slots. These are two relevant IO pathways; exact assembly callers are untraced.
Implementation MUST account for every save list/read/write entry point (including DOS wrappers), not assume `game_data.c` catches everything. No implementation or build test. The detailed source review is recorded on the Mission Builder branch in `docs/mission-builder/HANDOVER.md`.
