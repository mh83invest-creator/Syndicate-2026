# Syndicate 2026 — Legacy Campaign Identity and Menu/Save Boundary
Date: 2026-11-01 (Australia/Brisbane)
Status: static audit, NO implementation, compilation or runtime tests
Related: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; docs/SHARED_DEVELOPMENT_HANDOVER.md; docs/audits/2026-11-01-assembly-save-status-and-legacy-provenance.md

## Exact source evidence
Examined full engine/src/syndre.sx (blob c35e22621a93d1e07c44d40de8867446ff7c64bf, 85,803 lines) with indexed matching and context, plus prior engine/src/game_data.c source.

IDENTITY-01: Assembly source has zero case-sensitive matches for 'American', 'Revolt', 'revolt' or 'campaign', though 'Rebel' occurs in strings. These limited string searches do NOT establish whether American Revolt is selected through data, a menu option, a separate executable/resource, or internal flags. No campaign identity metadata was located.

IDENTITY-02: The main-menu dispatcher has a separate Save/Load screen entry at lines 54630-54633, calling LoadSaveGame; menu save preview uses ac_load_save_text at lines 54574-54583. The game menu's map-selection function at 54593-54627 calls map_selection, then load_text(current_levno) for briefing after particular outcomes. These source hooks suggest campaign state must be fixed before either save-description path and persisted through game/menu transitions; they do not establish the selector itself.

IDENTITY-03: map_selection computes current_levno from selected territory data_5b071 around lines 32011-32022 and 32056-32066. Thus mission number is assigned by a territory-table lookup; this value is not demonstrated to be a stable campaign identifier. Never infer campaign ID from current_levno, map number or save slot.

IDENTITY-04: Existing C save_player() serializes description, players, country_states, weapons, cybmods, research and selected_team, but does not explicitly serialize a standalone campaign ID. This does NOT prove that no provenance could be inferred from state; proof requires examining the complete struct semantics and actual old saves. Legacy file provenance remains unresolved.

## Important distinctions
- The user has explicitly REQUIRED separate silos for Original Campaign, American Revolt, World Domination and playable Custom Campaign(s); this is a binding behavioural requirement.
- The exact storage layout, custom campaign granularity and compatibility policy for unidentifiable old saves are NOT selected.
- A campaign must be selected before save slots are enumerated; UI filtering and file API protection are both required. Continue must be included even if not mapped yet.
- Preserve old file bytes and gameplay; do not classify/migrate old saves on guesswork.
- Original save serialization expected size 28247 bytes is source-derived, not runtime verified.

## Open work
1. Determine how the original game distinguishes American Revolt, including alternate resources and state outside literal assembly strings.
2. Trace menu-option state/data tables and whether a separate American Revolt campaign is actually present in this source build or supplied data.
3. Determine whether any legacy save field carries trustworthy mode provenance, and verify against real original saves before approving migration.
4. Agree shared campaign ID contract with concurrent Mission Builder branch.
5. Create non-destructive tests for saving/loading/continuing same-number slots in each mode; verify error and file preservation.

## Verification and handover
Performed: full assembly blob inspection, symbol/text matches, indexed source segments and comparison to native serialization. Not performed: Windows compilation, execution, file inspections or gameplay testing. No engine/filesystem changes, no migration and no new version. Documentation commit only; known-good v0.7 package preserved.
