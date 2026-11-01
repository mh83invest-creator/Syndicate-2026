# MB-STRUCT-18 — Alternate-campaign resource selection evidence boundary

Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC AUDIT ONLY
Workstream: Mission Builder; branch `feature/mission-builder-audit`

## Inspected evidence
- `engine/src/syndre.sx` native tactical loading invokes `load_map_level` with `level_fname`, a mutable `data/game01.dat` filename, and encodes a two-digit requested mission number (see MB-STRUCT-15–17).
- The checked `syndre.sx` text contains no explicit American Revolt / campaign-selection symbol or resource namespace identified by the search for `revolt`, `american` or `campaign`. This negative search is narrowly limited to the inspected assembly text, not evidence that American Revolt is absent from the game or other retail data / menus.
- `engine/src/game.c` on the Mission Builder audit branch includes level-data offset assertions and menu/tactical timing comments, but the inspected file did not reveal an alternate campaign resource router. These findings are not proof about external configuration, data packs or launcher selection.
- An earlier reference to `docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md` on this feature branch returned NOT_FOUND: do not assume the main branch's files and feature-branch files have automatically been merged. Cross-team requirements recorded in the main handover remain distinct from code integrated into this branch.

## Decisions preserved; implementation NOT established
- Original Campaign, American Revolt and each custom campaign must remain separate selectable modes.
- Campaign save namespaces are isolated; saves are made between missions only, never tactical/in-mission.
- Do not overwrite original `data/gameNN.dat`, claim native unused IDs, reuse save records or silently substitute custom data for other campaign paths.
- **No alternate-campaign routing mechanism is yet verified in this source audit.** A proposed future campaign-identity -> campaign-local mission ID -> resource manifest -> verified read-only payload abstraction is a design *candidate*, not an approved implementation or a claim about the present game.

## Before implementation
1. Inspect source / main-branch integration and the configuration or retail data responsible for American Revolt selection; establish exact resource root and campaign switching point.
2. Inventory available original and Revolt resource filenames using authorised inputs; confirm no collision.
3. Reconcile shared documentation across branches before selecting an implementation mechanism.
4. Preserve legacy loader and existing saves; require explicit approval and regression testing for any shared engine routing change.

## Next
MB-STRUCT-19: targeted source and repository inventory for campaign selection and asset-root configuration (including branch divergence), without modifying engine or unrelated audit streams.

## Verification
GitHub source search/review and branch-file existence check only; no builds, files generated for testing, asset inspection, retail data, or gameplay testing.
