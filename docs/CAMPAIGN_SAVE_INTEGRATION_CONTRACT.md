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

## Addendum — verified save UI assembly callers (2026-11-01)
Full `engine/src/syndre.sx` accessed with GitHub blob-by-SHA. Save UI `LoadSaveGame` (~line 30354) loops over 10 slots via `ac_load_save_text` (~30397), and calls `ac_load_player` (~30627) / `ac_save_player` (~30643). The main menu also enumerates save descriptions through `ac_load_save_text` (~54575–54582). These are two distinct UI-related enumeration points and need shared campaign identity before they run.

Legacy assembly `ASM_load_player` / `ASM_save_player` and original `synd/save` string remain present but active wrappers use `ac_` native C functions; do not conflate historical embedded save path with current active storage. Static evidence only; execution must still be tested. Full assembly audit blocker resolved via GitHub `fetch_blob`. See Mission Builder handover MB-AUDIT-0.8.

## MB-AUDIT-2.6 — Save integration regression gates
The active C save functions use LbFileOpen with the registered filename transform, resolving save/NN.gam relative to the data root in inspected code. The separate DOS-level transform routes save/ paths to the user root. Preserve both behaviours until runtime paths are verified. Before implementation, inventory legacy saves in a disposable copy and record hashes. Test same-slot isolation across all campaigns, save listing and Continue isolation, switch-back behaviour, persistence after restart, failed writes, corrupt saves and no change to legacy save bytes. Campaign identity must be available before every save enumeration and access. New namespace paths and legacy classification require explicit review; custom campaign isolation granularity remains undecided. No implementation, compilation or gameplay testing performed.

## MB-AUDIT-2.7 — Candidate integration boundary (2026-11-01)
Confirmed assembly call sites: LoadSaveGame lists 10 save slots through ac_load_save_text (syndre.sx lines 30390–30400), then calls ac_load_player or ac_save_player after slot checks (30621–30646). A separate main-menu view also enumerates all 10 slots via ac_load_save_text (54566–54582). The engine wrapper config engine/conf/wrappers_game.conf lines 245–247 exposes all three C functions. Since all observed active save listing, read and write actions converge on C functions in engine/src/game_data.c, a campaign-aware save service at that boundary is a candidate for an isolated prototype, conditional on complete caller audit and preserving existing raw .gam layout. The campaign ID must be set BEFORE the initial menu enumeration, and also be consistent with Continue and mode changes; wrapping only save_player would leak listings and allow accidental load. Do not implement a new global lbFileNameTransform override: it also affects game assets. Existence of a C seam does not prove selector or runtime campaign mode handling. No patch implemented or tested.

## MB-AUDIT-2.8 — Main-menu load and error propagation (2026-11-01)
Source `engine/src/syndre.sx` blob c35e22621a93d1e07c44d40de8867446ff7c64bf: main-menu `func_37b93` calls `LoadSaveGame` and uses its result (~54630–54633). Another main-menu branch enumerates 10 slot descriptions with `ac_load_save_text` (~54566–54583). The LoadSaveGame assembly interface selects slot then calls `ac_load_player` (~30621–30631) or `ac_save_player` (~30634–30646). Complete literal call-name search in this blob found only one `ac_load_player`, one `ac_save_player`, and two `ac_load_save_text` call sites. The assembly call sites appear to set menu continuation state without testing a read success or native save return value. In C, `load_player` returns void and silently returns on failed open, whereas `save_player` returns boolean, which the assembly appears not to consume. Regression gate: guard the common save boundary against mismatched campaign IDs and missing/corrupt data, with explicit failure visibility; preserve classic behaviour pending approved failure-handling design. `Continue` as a distinct feature has NOT been proven; the observed main-menu load flow is the verified path, and other load mechanisms outside the inspected assembly remain possible. Static inspection only; no changes or tests.

## MB-AUDIT-2.9 — Continue-path evidence (2026-11-01)
Full assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf has exactly one active `call LoadSaveGame` at line 54631, in main-menu function `func_37b93`. That screen calls `ac_load_player` at line 30627 and `ac_save_player` at 30643. Literal search finds only these calls to native C save/load routines. The main menu also lists all ten slots via `ac_load_save_text` at 54575–54582. No separately named Continue loader is proven in this assembly snapshot. This does NOT prove no Continue UI is available elsewhere: no runtime test or external launcher audit was performed. Do not invent a new Continue path; any future Continue must enforce the same active campaign identity and namespace checks as the verified load path. No source modifications, builds or runtime tests.

## Audit 3.0 — Implementation readiness decision (2026-11-01)
**Decision: NOT READY to modify production save routing or claim American Revolt compatibility.** This is a design gate, not a failed compile. Source-backed: the existing native save UI converges on C `load_save_text`, `load_player`, `save_player`; existing 10-slot `.gam` storage has no verified campaign discriminator. `LB_FILENAME_TRANSFORM=1` is enabled in bflibrary, with C saves resolved through the registered HDD-root transform, while DOS-level resolution has a separate save path mechanism. The current startup option parser has no campaign selector, and a separate Continue loader is not verified. The source port requires retail data not fully in the repository.
**Ready for an isolated prototype only after the following inputs and checks:** (1) identify Windows installation data/user directories and preserve a hashed backup of legacy saves; (2) confirm supported retail-data inventory and whether American Revolt files are actually available; (3) define stable campaign identity and when the selection UI sets it before both listing paths; (4) settle custom campaign isolation granularity with the user before implementing that subfeature; (5) document a backward-compatible namespace/file strategy and explicit invalid/missing-file behaviour; (6) make a separate experiment branch and add tests for equal-slot isolation, Save/Load, mode switching, restarting and no mutation of legacy saves; (7) compile/test 32-bit Windows with original assets and retain mandatory release handover for any v0.8+ build. Original Campaign remains unchanged until regression tests prove compatibility. A global file transform override is not an approved implementation. No code modifications or build/runtime tests have been performed during Audits 2.0–3.0.

## MB-AUDIT-3.1 — Installer manifest and resource inventory gate (2026-11-01)
Read-only inspection of engine/util/install (lines 183-269) establishes an explicit installer manifest: game mission files game01-50 and game90-99.dat; maps selected from map01-94.dat; briefing/text files miss01-50.dat and miss101-150,201-250,301-350,401-450.dat; additional graphics, UI, audio, and resource files. These numbered groups are observed filenames only; they do not prove an American Revolt selector, expansion identity, or installed asset contents. Installer creates data and sound directories and explicitly leaves save/screenshots folder creation to the game (commented install_dir lines 267-269). The installer imports source files from supplied retail data; an actual file-by-file inventory on a disposable installation remains essential before code integration. Compare installer-required files to present files; report missing files and hashes, without relocating or altering any existing save or retail asset. This is static manifest analysis, not runtime verification; existing Audit 3.0 readiness gate remains unresolved.

## MB-AUDIT-3.2 — Installer checks (2026-11-01)
engine/util/install checks whether SOURCE contains a syndicat directory, then searches for each listed filename without case sensitivity. Missing filenames cause failure, but retail asset contents and campaign edition are not authenticated. The TARGET must be empty. MD5 checks apply to separately downloaded graphics/audio packages, not imported original retail mission files. The generated run script selects language only. This installer cannot establish American Revolt support. Any installed-resource comparison must use a disposable copy; protect existing files. No installer execution, game code changes or runtime verification.

## MB-AUDIT-3.3 — Mission number is not campaign identity (2026-11-01)
Assembly `syndre.sx` source inspection: world-map selection reads a one-byte mission identifier from a map_countries record at `data_5b071` and writes `current_levno` (around lines 32010–32022). `load_map_level` (27845–27880) writes that level number's decimal digits into the `data/game01.dat` filename seed, then calls `LbFileReadRNC`. It subsequently obtains `level__MapNumber` from the loaded mission data and uses separate map asset filenames (27881–27903). Mission briefing `load_text` (55249–55285) builds numbered `data/miss...dat` paths based on language and requested text number. These routines use numeric mission/map/text IDs and contain no demonstrated original-versus-American-Revolt identity. The installer enumerates game01-50 and game90-99 assets, but numeric ranges are not established as campaign indicators. Do not assume which retail edition any given file represents or allow two campaign roots to silently resolve the same filename. Required gate: inspect actual separate retail asset sets and their file metadata/hashes, define a campaign-aware resource root independent from save namespace, and test original mission/map/text linkage before implementation. No game code or installed assets modified; no gameplay tests.

## MB-AUDIT-3.4 — Mission/map/briefing dependency contract (2026-11-01)
Static source trace: `process_action` calls `load_map_level(level_fname,current_levno)` around syndre.sx 28531–28537. `load_map_level` edits decimal digits within `data/game01.dat`, reads RNC mission data into `level__Seed` (27845–27880), then obtains `level__MapNumber`; a zero value is replaced with 1 (27881–27885, similarly 27917–27924 for a second rendering path). It substitutes the two decimal map digits into a separate map resource filename and calls `ac_LbDataLoadAll(mission_load_files)` in the high-resolution branch (27887–27903). `load_text` separately formats `data/miss%02d.dat` for English or a language-indexed miss file for other languages, then calls LbFileReadRNC and LbFileLengthRnc (55249–55285). Therefore mission number, embedded map number and language-dependent briefing filenames are distinct resource dependencies. Future campaign-aware resource resolution must select a consistent resource set before the mission read, preserve embedded map-number semantics including the zero-to-one fallback, and apply the same campaign context to dependent map/text files without disturbing shared UI/audio resources. This is a compatibility requirement rather than an approved global path rewrite. No installed data inspected and no code/build/gameplay tests.
