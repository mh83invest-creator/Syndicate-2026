# Syndicate 2026 — American Revolt Historical Save Isolation Evidence
Date: 2026-11-01 (Australia/Brisbane)
Status: historical documentation + source cross-check, no implementation, no gameplay tests
Relevant: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md, docs/SHARED_DEVELOPMENT_HANDOVER.md, docs/audits/2026-11-01-legacy-save-state-provenance.md

## Important external evidence
- PCGamingWiki Syndicate Plus data locations lists DOS SAVE and SAVE2; specifically identifies American Revolt saves in SAVE2. https://www.pcgamingwiki.com/wiki/Syndicate_Plus
- Original Syndicate Plus user manual describes selecting the version between original Syndicate and American Revolt on launch. https://www.mocagh.org/origin/origincompilation.pdf (Syndicate Plus booklet); alternative manual mirror https://oldies.ndlp.info/retroarch/roms/pc/SYNDICAT/DOCS/SYNDREF.PDF
- Independent historical player report describes original SAVE folder and American Revolt SAVE2 folder: https://www.old-games.ru/forum/threads/syndicate.24444/
- Community reverse-engineering notes describe differences in American Revolt saves: https://syndicate.lubiki.pl/synd/rework/synd_rework_savegame.php. Treat this as a lead needing verification, not a proven layout or migration formula.

## Cross-check against current repository
HIST-01: Historical original Syndicate Plus appears to silo Original Campaign in SAVE and American Revolt in SAVE2. This substantially strengthens the product requirement, but it does not demonstrate that current SyndicatFX native C uses SAVE2.
HIST-02: Inspected current engine/src/game_data.c builds save/%02d.gam in load_save_text, load_player and save_player without any mode parameter; its inspected code does not select save2. Repository support for American Revolt selection and resource switching remains to be traced and gameplay tested.
HIST-03: Syndicate Plus manual documents pregame version selection. The previously inspected assembly current-game menu and native C main startup have not yet yielded an explicit equivalent selector. It might belong to original setup/launcher executable or separate data not represented in this source. Do not claim one exists.
HIST-04: Since original uses distinct directories, the approved new save-silo design should explicitly consider compatibility with both SAVE and SAVE2, but no target path layout is approved. Do NOT automatically redirect, convert or reclassify existing data based only on these historical descriptions.
HIST-05: A community report says American Revolt binary saves differ somewhat. Verify against legitimately held samples before asserting interchangeability or extending the original 28247-byte source-derived expected format to both campaigns.

## Required next engineering gates
1. Inventory startup scripts, resource paths, and original-game package selection mechanism; inspect actual user-held Syndicate Plus install.
2. Determine whether the current build can launch American Revolt and which files it reads/writes for saves; no test performed.
3. Establish reliable legacy SAVE and SAVE2 recognition without modifying original files.
4. Design separate new-mode namespaces for World Domination and playable Custom Campaign(s); preserve original/expansion operation as-is until cross-branch agreement.
5. Test ten slots per campaign, load/save/continue isolation, identical slot numbers and legacy preservation, along with failure paths, on Windows 32-bit.

## Handover and verification
External manual/wiki/community references inspected and matched against native save source and prior assembly audit. Source evidence is static and historical evidence is not a current build test. No engine code changed, no Windows build, no save read/write, no mission run. v0.7 baseline protected; docs-only audit does not require rollback.
