# Syndicate 2026 — Shared Development Handover
Updated: 2026-11-01 (Australia/Brisbane)

## Purpose
Single cross-workstream coordination entry point for the existing Syndicate 2026 engine audit and the Mission Builder / World Domination audit. Read this before new development. This file describes agreed scope and links, not completed implementation.

## Protected baseline
- Repository: mh83invest-creator/Syndicate-2026
- Main branch: main
- README identifies the source baseline as SyndicatFX 0.0.6.1022, with Windows playtesting through v0.7.
- Source under engine/. The README says mission gameplay remains at 16 FPS; some company configuration pacing changed to 30 FPS.
- Preserve all validated current functionality, original campaign, American Revolt, gameplay speed, mouse/keyboard support, assets and existing Windows builds.
- No proprietary original game data is stored in this repository.
- Existing mainline engine audit continues independently; do not replace its findings or claim it is complete.

## Agreed mode selection
- Keep Original Campaign and American Revolt available and unchanged in behaviour.
- Investigate adding selectable World Domination and Custom Campaign modes using the same tactical engine, not a separately implemented game.
- Expose Mission Builder separately in the main menu (design preference agreed by user).
- Mission Builder scope: complete mission definitions, briefing, equipment/agents flow, tactical execution, results, integration into campaign.
- Random mission framework: pre-authored pools for territory revolts first; procedural mission parameters only after validation.
- World Domination: optional persistent strategic campaign with overall objective, territory states, rebellions, dynamic missions and consequences.
- Existing original campaigns must remain operational and must not be silently modified; any optional random revolt behaviour in classic campaign needs separate explicit approval.

## Workstreams
1. Existing engine audit / modernisation: main and its established development practices.
2. Mission Builder integration audit: feature/mission-builder-audit. See `docs/mission-builder/HANDOVER.md` on that branch for investigation log, decisions, and outstanding questions.

## Collaboration protocol
- Mission Builder changes happen on its own branch. Do not alter mainline engine code during audit.
- Mainline developers should read this shared file plus the mission branch handover before changing campaign selection, mission loading, objective logic, map data, save/load, AI/spawns or briefings.
- Mission Builder developers should check mainline commits and engine audit handovers before proposing interfaces.
- Integration contract must be documented (formats, functions, ownership, state transitions, test coverage) before code changes that touch shared components.
- Merge via reviewable pull request after cross-workstream check; never auto-merge or overwrite main.
- Update handover with date, branch, commit, exact files, verified behaviour, untested items, integration dependencies and next steps after each development change.

## Immediate next steps
- Mission audit: enumerate and inspect actual engine files controlling mode selection, briefings, objectives, mission loading, revolt triggers, maps/spawns, mission results and saves.
- Produce evidence-backed feasibility matrix with file/function references; note unknowns.
- Define first end-to-end proof of concept using an original-map custom mission, without disrupting the baselines.
- Coordinate any proposed shared engine changes with mainline audit before implementation.

## Current verification
- GitHub repository README was read on 2026-11-01.
- No mission loader, save game, AI or menu source files have yet been inspected for this workstream.
- No implementation, build or runtime tests of Mission Builder have occurred.

## Approved cross-development requirement — campaign-isolated saves (2026-11-01)
- Save files must be siloed by campaign/mode. Original Campaign, American Revolt, World Domination and any added Custom Campaign must each have their own save namespace.
- From within any campaign, Save/Load screens must show and load ONLY saves belonging to that active campaign. No cross-campaign access, selection or accidental overwrite.
- Campaign identity must be consistently propagated through campaign selection, save slot enumeration, load, save, delete (if available), and continue/resume pathways.
- Preserve access to existing Original Campaign and American Revolt save files. Examine legacy `save/%02d.gam` behaviour and define a migration/compatibility approach before changing paths or file format. Do not silently migrate, delete, replace or reclassify existing saves.
- Exact on-disk directory structure, metadata and migration rules are implementation decisions pending source audit; the isolation behaviour itself is an approved requirement.
- Cross-workstream owners: the mainline audit owns compatibility with current game and original save paths; Mission Builder audit owns campaign identity integration and new mode isolation design. Document the interface and test together.
- Required tests: separate saves per campaign, identical slot numbers without collision, Save/Load filtering, wrong-campaign rejection, resume/continue isolation, legacy save access, unaffected original gameplay and backward compatibility.
- Status: REQUIREMENT DOCUMENTED ONLY; not implemented or runtime-tested.

## Cross-workstream finding — save and mission integration (2026-11-01)
Source inspection now completed for `engine/src/game_data.c`, `game_data.h`, `game.c`, `main.c`, `wrcountry.h`, `player.h`, `engine/src/Makefile.am`, `conf/wrappers_game.conf` and the assembly file's initial content. Earlier note that no save source was inspected is superseded.
- Save slots currently use `save/%02d.gam` in `load_save_text()`, `load_player()`, `save_player()`. No campaign argument exists in these native C functions.
- `game_transform_path_full()` intercepts `save/` paths for the user directory. Isolated namespaces must account for both this path resolution and direct `save/%02d.gam` references.
- Original gameplay is called via assembly-imported `syndicate()`. The assembly source is substantive (its contents were confirmed via GitHub contents API), and its selection/loading logic remains to be traced.
- Immediate cross-team rule: Do not independently change save path semantics, save slot selection, campaign mode, or `load_player/save_player` interfaces until a common campaign identity contract is agreed. Preserve legacy data.
- Requirements remain: only own-campaign saves visible/loadable; wrong-campaign paths cannot load; old saves remain accessible; protect fixed payload structures.
- No implementation or runtime testing. Detail in Mission Builder handover on `feature/mission-builder-audit`.

## Cross-workstream integration finding — result flags (2026-11-01)
Mission Builder source audit MB-AUDIT-1.2 established `level_complete` sets bit 0x2 and `level_failed` sets bit 0x4 of assembly game state `data_60afc`. A later post-mission routine at assembly lines ~55045–55079 checks completion and changes territory ownership/happiness/tax state. **Do not change these flags or the post-mission state transition in the mainline engine without reviewing Mission Builder requirements and tests.** Existing campaign rules must remain unchanged. Full source references: Mission Builder handover on `feature/mission-builder-audit`. Documentation only; untested in gameplay.

## Cross-development warning — campaign win condition (2026-11-01; Audit 1.3)
Assembly `level_finished` at `engine/src/syndre.sx` ~54998–55130 applies mission completion to country state and counts territories held by the active player. A count of 50 triggers legacy campaign victory, ending with terminal flag 0x10. This is a protected original-mode outcome. Do not modify its flag handling, territorial count, or win branch without verifying Original Campaign and American Revolt compatibility and reviewing Mission Builder / World Domination mode requirements. Full assembly evidence in Mission Builder handover MB-AUDIT-1.3. Static inspection only, no gameplay test.

## Cross-workstream warning — city eligibility and loss transitions (2026-11-01; Audit 1.4)
Assembly `can_i_play_this_city` at `engine/src/syndre.sx` around 32871–32965 checks ownership, a special country-state 0xFF case, special mission ID 1 and adjacency; map connection drawing calls the same eligibility function. Campaign selection or random-mission routing must not break either tactical eligibility or its map visualisation. Assembly `level_finished` failure route 55140–55192 may trigger a total-loss ending and player reset; new campaigns must not inadvertently execute classic ending/reset rules. Full evidence in Mission Builder handover MB-AUDIT-1.4. Static inspection only.

## Cross-development note — possible replay/rebellion marker (2026-11-01, MB-AUDIT-1.5)
`engine/src/syndre.sx` `can_i_play_this_city` has a branch allowing active-player-owned countries with `country_states == 0xFF` to be selected; `level_finished` changes this marker to 0xAA on successful completion. These are source-verified candidate paths for an existing replay/rebellion state; the cause of 0xFF has NOT been traced and must not be assumed to be tax rebellion without verification. Preserve these paths and classic `data_5b071` mission mapping during engine changes. No code changes or gameplay test. See MB-AUDIT-1.5 on Mission Builder branch.

## MB-AUDIT-1.6 cross-team correction (2026-11-01)
Assembly `AdjustStateHappiness` (syndre.sx lines 11835–11844) writes 0xFF as the upper clamp of happiness (>255). This does NOT establish that 0xFF means a tax rebellion. Previous hypothetical interpretation is explicitly unverified. Preserve the legacy selection and state-transition logic until the true event trigger is traced and tested. Mission Builder audit commit 42f1902. Documentation only.

## Audit 1.7 — territory field mapping (2026-11-01)
Mission Builder inspection reconciled the inline assembly country-state offsets to packed `CountryState` in `engine/src/wrcountry.h`: Happiness offset 0, OwningPlayer 2, Revenue 3, TaxPercent 4, Population 6; record size 10 bytes. Assembly `adjust_tax` changes TaxPercent; `AdjustStateHappiness` reads Revenue, requiring further investigation of the update chain. The true rebellion trigger and American Revolt selector remain unconfirmed. Do not change tax, happiness, campaign routing or mission IDs based on an assumed relationship. Detailed audit MB-AUDIT-1.7 on Mission Builder branch, commit 3cfb022. Documentation-only; no runtime tests.

## MB-AUDIT-1.8 cross-team note (2026-11-01)
Strategic tick calls AdjustStateHappiness before GetRevenueFromCity. Revenue routine calculates using previous stored rate, then copies newly selected tax into stored rate, so preserve original ordering. ComputerPlaysMap changes neighbouring-country ownership directly as rival expansion, not a confirmed rebellion trigger. Full source notes Mission Builder Audit 1.8. Static inspection only.

## Cross-workstream — mission files are number-based (MB-AUDIT-1.9, 2026-11-01)
Assembly mission loader builds `data/gameNN.dat` filenames using `current_levno`; briefing loader similarly uses language-specific `data/miss...dat`, and map resources use `level__MapNumber`. No campaign-identity dimension is visible in these inspected paths; do not globally reassign those paths or assume higher mission IDs work without isolated validation. `engine/src/wrcountry.h` declares 50 packed 10-byte territory records. American Revolt routing remains unconfirmed. No engine code change or runtime tests; detailed source audit on Mission Builder branch.

## Cross-workstream update: CLI campaign switch absent (MB-AUDIT-2.0, 2026-11-01)
C entry engine/src/main.c process_options currently has no campaign/American Revolt CLI selection switch. The observed assembly mission asset path also has no campaign identity component. This does not rule out expansion resources elsewhere. Any future selector and mode-specific resource resolution must be separately designed and tested without changing Original Campaign defaults. Mission Builder handover 2.0 contains details. Static inspection only.

## MB-AUDIT-2.1 — Asset and save directory isolation risk (2026-11-01)
Current `game_data.c` uses `GetDirectoryHdd`/`SyndFXFileNameTransform` for relative game resources and uses fixed `save/%02d.gam` names for all C save listing/read/write paths. `dos.c` has additional path translation entry points. Campaign-specific resource and save routing must inspect these independently and preserve existing `save/NN.gam` files; do not change the global transform casually. Detailed source evidence in Mission Builder handover Audit 2.1. No build or gameplay testing.

## Cross-workstream path findings MB-AUDIT-2.2 (2026-11-01)
`game_transform_path_full` in `engine/src/game.c` explicitly routes `save/` paths to the user directory, other relative paths to the game data directory, and leaves absolute paths alone. Windows data install path comes from `Software\\SyndicatFX\\CurrentVersion` / `InstallPath`; Windows user path is `%APPDATA%\\SyndicatFX`. Unix-like builds use `GAME_DATA_PATH` for data, XDG/HOME for user saves. Distinct DOS path functions exist and must be reconciled with bffile before implementing strict per-campaign save isolation. Preserve legacy saves and all platform path behaviour. More detail in Mission Builder Audit 2.2; static source audit only.

## Audit 2.3 resource note
Original Syndicate Plus assets are required and not fully bundled. Unix uses configured GAME_DATA_PATH; Windows reads SyndicatFX InstallPath. Expansion assets remain unverified. See Mission Builder handover Audit 2.3. No runtime changes.

## Critical file-path discrepancy (MB-AUDIT-2.4, 2026-11-01)
The actual bflibrary LbFileOpen backend passes relative paths through registered SyndFXFileNameTransform, which prefixes the game-data directory even for `save/NN.gam`. A separate `game_transform_path_full` special-cases saves to GetDirectoryUser(), but is not that registered transform. Thus save locations depend on the call path. Prior assumption that all save paths resolve to the user directory is incorrect. Preserve legacy files, inspect LB_FILENAME_TRANSFORM build settings and test both paths before implementing campaign-specific saves. No engine edits. See MB-AUDIT-2.4.

## MB-AUDIT-2.5 — active filename transform verified in source (2026-11-01)
LB_FILENAME_TRANSFORM is defined as 1 in bflibrary/include/bffile.h. Both inspected SDL file backends call lbFileNameTransform for LbFileOpen, and game_initialise registers SyndFXFileNameTransform before resources load. Thus source C save/NN.gam through LbFileOpen resolves against the data root. DOS low-level save routing uses a different user-root path resolver. Test the actual Windows executable with harmless isolated test slots before introducing campaign save siloing; do not migrate or overwrite legacy saves. Static source verification only. See Audit 2.5.

## MB-AUDIT-2.6 (2026-11-01)
Save isolation acceptance gates documented in docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md (commit 30331d4). Both file resolvers and legacy saves must be protected. No implementation or test results yet.

## MB-AUDIT-2.7 — Candidate save namespace boundary (2026-11-01)
The observed active assembly save menus route all slot descriptions, load and save through native C load_save_text/load_player/save_player wrappers. These three form a potential scoped namespace boundary. Any implementation must establish campaign identity before both save menus and Continue, and preserve legacy files. Avoid global asset filename remapping. See save integration contract commit ae0c2fd. This is static analysis, not an implemented patch.

## MB-AUDIT-2.8 — Menu load and save error caveat (2026-11-01)
The inspected assembly main menu enters `LoadSaveGame`, and its slot list/read/write converge on native C wrappers. Assembly does not visibly check the C save result; C load_player returns void and silently returns if opening fails. Preserve original behaviour while designing explicit campaign-specific file-validation and error handling; do not represent the separate Continue path as verified. See save contract Audit 2.8. Documentation only.

## MB-AUDIT-2.9 — Main-menu load path (2026-11-01)
Assembly main-menu calls LoadSaveGame; that screen uses native C load/save wrappers. No separate Continue loader has been established in the reviewed assembly. Future Continue, if implemented, must use campaign namespace checks and never silently access a different mode's slots. Source inspection only.

## MB-AUDIT-3.0 — Build readiness gate (2026-11-01)
Save integration assessment: not ready for a production code patch. Source seam known; legacy save location, original/expansion assets, mode identity and Windows execution still need verification. Updated CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md at 59c8a7f. No engine code or build affected.

## MB-AUDIT-3.1 — Installer inventory (2026-11-01)
engine/util/install enumerates imported gameNN.dat, mapNN.dat, missNN.dat and other required assets from original Syndicate Plus data. This is a filename manifest, NOT proof of American Revolt mode or runtime availability. Save/screenshot directory setup is commented as game-managed. Do not modify resource routing before examining actual retail-data inventory in a disposable installation. See Mission Builder Audit 3.1. Documentation only.

## MB-AUDIT-3.2 (2026-11-01)
Installer validates source directory and filenames, not American Revolt compatibility. Requires empty target. See campaign save integration contract commit b119686. No code modification or runtime testing.

## MB-AUDIT-3.3 — Mission identifiers are not mode selectors (2026-11-01)
World-map selection yields a numeric current_levno; loader reads corresponding data/gameNN.dat and uses embedded level__MapNumber for map assets; mission text uses numbered data/miss...dat. No verified campaign identity is passed in this path. Do not infer American Revolt solely from filename ranges. Resource routing and save isolation are distinct requirements. See Mission Builder Audit 3.3; static source inspection only.
