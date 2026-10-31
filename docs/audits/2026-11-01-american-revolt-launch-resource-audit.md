# Syndicate 2026 — American Revolt Launch and Resource Audit
Date: 2026-11-01 (Australia/Brisbane)
Status: repository static review only; no implementation, builds or gameplay testing.
Previous: docs/audits/2026-11-01-american-revolt-historical-save2.md
Cross-workstream: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md and feature/mission-builder-audit.

## Inspected source
- engine/README.md (upstream distribution and Windows startup notes)
- engine/util/install (source-game media and resource copying)
- engine/src/main.c (previously inspected startup flow)
- engine/src/game.c and engine/src/dos.c (previously inspected path handling)
- engine/src/syndre.sx complete blob from prior indexed audit
- main README.md (protected v0.7 baseline)

## Findings
LAUNCH-01: engine/README.md explicitly describes SyndicatFX as an alternative executable for the 1993 DOS game, requiring original data from Syndicate Plus CD/digital distribution. Thus the original data format and bundled asset coverage must be treated as external dependencies.
LAUNCH-02: engine/util/install accepts a source tree under a 'syndicat' folder, copies game resource files into the target data folder, and leaves 'save' and 'screenshots' directory creation to the game. No SAVE2-specific handling or separate American Revolt save import was identified in inspected installer logic.
LAUNCH-03: The documented Windows startup is syndicatfx.exe with language options. No American Revolt selector/switch is established by these documented launch commands. This negative finding applies only to inspected source/documentation, not all possible runtime modes.
LAUNCH-04: engine/src/main.c initializes and calls syndicate(); no explicit Original-vs-American Revolt argument or path is passed there. Relevant mission text/map data are resolved from standard data filename patterns in assembly. Resource-specific mode selection elsewhere remains unverified.
LAUNCH-05: Historical Syndicate Plus reports Original saves under SAVE and American Revolt under SAVE2. Current inspected C save functions exclusively use save/%02d.gam, so historical SAVE2 compatibility is not demonstrated in the current build.
LAUNCH-06: New user-approved campaign silos (Original, American Revolt, World Domination and playable Custom Campaigns) cannot be considered implemented merely by creating directories. Mode selection, correct campaign resource loading, save visibility and Continue behaviour require a validated identity and routing contract.

## Outstanding / next audit
1. Audit all install script file-copy rules and resource packaging for American Revolt data, distinguishing lack of explicit mode flag from lack of assets.
2. Inspect Windows build packaging and actual launch configuration for any separate original/expansion binary or custom command.
3. Verify with legitimately owned Syndicate Plus data how American Revolt missions are selected and how SAVE2 files are named/formatted.
4. Align source and Mission Builder branch around confirmed campaign identity, save routing, legacy preservation, error handling and test fixtures.
5. Keep original game rules, mission speed, camera and v0.7 baselines protected; no speculative edits.

## Verification
GitHub source and documentation read, source paths cross-checked. Not performed: Windows compile, application launch, retail-data inspection, existing save read, user playtest, or save namespace implementation. This is a docs-only handover, no version bump or rollback required.
