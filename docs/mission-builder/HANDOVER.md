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
