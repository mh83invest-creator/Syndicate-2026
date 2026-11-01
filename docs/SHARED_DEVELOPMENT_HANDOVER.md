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
