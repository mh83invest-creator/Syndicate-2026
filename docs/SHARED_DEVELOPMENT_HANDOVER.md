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
