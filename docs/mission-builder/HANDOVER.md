# Mission Builder / World Domination — Development Handover
Status: AUDIT ONLY — no game code changed
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Repository: mh83invest-creator/Syndicate-2026
Related shared coordination document: docs/SHARED_DEVELOPMENT_HANDOVER.md (main branch, commit 3fb2df07faa54cf7b10cbb5de1570f88cd5c25fb)

## Project intent and agreed decisions
- Mission Builder must integrate into the Syndicate 2026 native engine, not a separate incompatible game.
- Selection screen concept: Original Campaign, American Revolt, World Domination, Custom Campaign.
- Keep original campaigns intact; Mission Builder accessed separately.
- Support complete Bullfrog-style flow: selection > briefing > agent/equipment selection > tactical mission > results > campaign effects.
- Allow pre-authored random missions (especially revolts) and, when technically validated, variable parameters and eventual procedural generation.
- World Domination is a selectable campaign with overarching goals, territorial consequences, rebellion and dynamic missions.
- Do not implement unapproved mechanics or edit existing original campaign missions. Classic-campaign revolt randomisation is a separate optional change requiring approval.

## Current evidence
- GitHub README.md on main says engine/ contains native source derived from SyndicatFX 0.0.6.1022 and reflects Windows playtests through v0.7.
- README says mission loop at original 16 FPS, company configuration 30 FPS.
- Original game assets must be supplied separately; not in repository.
- A repository-level mission code search returned no results; this does NOT prove functionality absent.
- Individual engine source files have not yet been inspected. All feasibility and integration details below are INVESTIGATION TARGETS, not established capabilities.

## Technical audit checklist
1. Inventory engine code, documentation and build configuration. Record actual paths/functions before making conclusions.
2. Trace title/menu and Original Campaign / American Revolt selection, plus data loading.
3. Trace mission identity / briefing text / map selection / placement / inventory and agent allocation.
4. Trace objectives, triggers, win/loss, post-mission results, money and territorial state.
5. Trace taxation thresholds, uprising events, revolt mission dispatch.
6. Trace saves and persistence, version compatibility and original campaign data.
7. Trace spawning/AI/navigation, map collision, supported asset types and maximum entities.
8. Compare any relevant FreeSynd structures for architectural insight, without assuming file-format compatibility.
9. Define extension points: CampaignRegistry, MissionDefinition/Loader, MissionResult, Event/ScenarioSelector, SaveSchema as conceptual names only. Do not claim these interfaces exist.
10. Record risks: hardcoded mission IDs, binary format assumptions, licensing/data dependencies, compatibility, pathfinding and save migration.
11. Draft an integration contract with exact source references and provide mainline audit a cross-check opportunity.
12. Plan minimal end-to-end proof of concept (custom mission on existing map with briefing and completion) and integration tests before editor or strategic simulation.

## Isolation and collaboration rules
- Main is the protected production/development baseline. No changes to mainline gameplay from this branch without review and approval.
- Read latest docs/SHARED_DEVELOPMENT_HANDOVER.md from main at the start of each work session, plus latest mainline commit and relevant engine audit handovers.
- The shared coordination document lives on main. This branch was created before it and MUST be updated from main by a controlled merge/rebase before integration; do not assume it is present on this branch.
- Maintain this handover with dated evidence, file paths, exact modifications, tests, limitations and next steps.
- Use pull requests for eventual implementation; check for concurrent mainline changes to the same files.
- Preserve keyboard/mouse, timing, camera, combat visuals, original campaign and American Revolt behaviours.

## Implementation / test report
- Source code modified: none.
- Mission Builder implementation: none.
- Tests: no compilation or gameplay testing.
- Documentation: this handover created.
- Outstanding: source inventory and feasibility assessment, integration design, proof-of-concept test.
- Handover version: MB-AUDIT-0.1.

## Audit 0.2 — Source architecture discovery (2026-11-01)
Read from main repository source:
- `engine/src/game.c`: `game_initialise()` asserts fixed offsets for `level__CPObjectives` relative to `level__Seed` (lines ~69–70). `game_update()` targets 16 FPS, and `game_update_menu()` targets 30 FPS. This confirms existing memory layout constraints, not an editable mission loader.
- `engine/src/wrcountry.h`: packed `CountryState` contains `Happiness`, `OwningPlayer`, `Revenue`, `TaxPercent`, `Population`; `country_states[50]`. `wrcountry.c` contains no corresponding active gameplay logic in current source snapshot. Territory mutation code must be located elsewhere.
- `engine/src/game_data.c`: `load_player(int slot)`, `save_player(char *,int)` operate on `save/%02d.gam`, using fixed binary arrays `players`, `country_states`, `weapons`, `cybmods`, `research`, `selected_team`. Hard `sizeof` assertions include players=8376 bytes, countries=500 bytes. `save_player` also changes credits under a threshold condition. Do not change original save format without explicit compatibility plan.
- `engine/src/Makefile.am`: compiles `syndre.sx`, wrapper objects generated from `conf/wrappers_*.conf`, with native C files. Treat mission/campaign extension as native+assembly integration until further proved.
- `engine/src/game_bstype.h`: `ThingIdx` and `MapCoord` types available for later review.
- `engine/src/player.c` and `wrcountry.c` contain scaffolding, not yet substantive gameplay implementations in this snapshot.

Engineering consequences:
1. First prove how original assembly-linked mission selection/objectives work. Do not assume simple C hooks exist.
2. Prefer separate versioned World Domination state or demonstrably compatible save extension; no changes to existing `.gam` layout until proven safe.
3. Mission builder should initially emit a separate data model; actual in-engine execution is unverified.
4. Preserve original territory state and mission update timing.
5. Existing selectable mode design remains approved concept, not proven implementable yet.

Next inspection: `engine/conf/wrappers_game.conf`, `engine/src/game_data.h`, `engine/src/player.h`, generated symbol relationships, source data inventory and `syndre.sx` treatment.
Tests: static inspection only, no build/runtime test.
Code modified: none.
Handover version: MB-AUDIT-0.2.

## Audit 0.3 — Objective schema and execution boundary (2026-11-01)
Additional inspected repository files:
- `engine/src/game_data.h`: packed `CPObjective` (fields `Child`, `Parent`, `field_4`, `field_6`, `ActionType`, `Action`, `TargetOffs`, `field_B`, `field_D`); declared `level__CPObjectives[128]` and `level__Seed`. Semantics of objective action values are NOT yet known.
- `engine/src/main.c`: declares `current_levno`, `level__MapNumber`; initialises both to 1; calls `syndicate()` after resource initialization. `syndicate()` is declared as assembly import. This is a critical runtime boundary requiring assembly/data examination.
- `engine/src/Makefile.am`: includes `syndre.sx` as source and generated wrapper objects. `engine/conf/wrappers_game.conf` includes `game_update`, `game_update_menu`, `load_player`, `save_player`, etc. Wrapper presence does not imply editable mission functions.
- `engine/src/guitext.c`: loads `language/<language>/guitext.dat` into GUI strings. This is menu text, NOT established mission briefing text support.
- `engine/src/player.h`: `PlayerInfo players[8]`, selected agent team `selected_team[4]`; direct use in binary save routines reinforces ABI constraints.

Implications:
- An existing objective array provides a concrete target for identifying mission semantics, but its encoding and loading path must be verified.
- `current_levno` and `level__MapNumber` must be traced into mission selection and any expansion switches.
- Assembly imported `syndicate()` is the principal unknown. Do not promise adding selectable campaigns until the integration seam is demonstrated.
- Need inspect stored form and provenance of `syndre.sx`: connector returned empty text, so cannot conclude it is a genuinely empty file or accessible textual assembly.
- Need inspect actual original game mission resources separately if legally provided; none committed.

Next audit: inspect workflow, assembly asset provenance, data directories and available symbols; produce risk-graded implementation strategy; no coding before validated integration seam.
Tests: static code/document reading only. No compilation or runtime tests. Existing behaviour unchanged.
Handover version: MB-AUDIT-0.3.

## Approved requirement — siloed saves by campaign (2026-11-01)
User explicitly requires Original Campaign, American Revolt and each added mode (including World Domination and Custom Campaigns) to have separate save collections. A campaign's save/load interface must ONLY display/access its own saves; saving in one campaign must not overwrite another campaign's identically numbered slot.

Cross-development contract to establish before implementation:
- Campaign identity selection and propagation into save/load/continue (and deletion if offered).
- Storage namespace and validation against incorrect campaign loads.
- Preserve compatibility with existing legacy `save/%02d.gam` fixed-format saves; identify what old slots represent before proposing migration.
- Determine treatment of multiple user-made Custom Campaigns (individual campaign IDs versus one shared Custom Campaign namespace) during design; no silent assumption.
- Avoid changing original binary payload layout without a tested migration or versioned extension.
- Test independent slot numbering, save-list filtering, wrong-campaign rejection, legacy compatibility and uninterrupted original campaign behaviour.
- Co-ordinate this requirement with mainline handover at `docs/SHARED_DEVELOPMENT_HANDOVER.md` (main). Both workstreams must review save-related changes.
Status: documented and approved requirement, NOT implemented or tested.
Handover version: MB-AUDIT-0.4 (documentation-only).
