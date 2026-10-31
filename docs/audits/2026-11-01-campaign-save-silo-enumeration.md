# Syndicate 2026 — Campaign Save Silos and Enumeration Audit
Date: 2026-11-01 (Australia/Brisbane)
Status: APPROVED BEHAVIOURAL REQUIREMENT; static audit only. No implementation or runtime verification.
Repository: mh83invest-creator/Syndicate-2026, main
Related: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; docs/SHARED_DEVELOPMENT_HANDOVER.md; docs/audits/2026-11-01-bullfrog-file-library-followup.md; separate feature/mission-builder-audit docs/mission-builder/HANDOVER.md.

## Explicit requirement reaffirmed by user
Save data MUST be siloed between (1) Original Campaign, (2) American Revolt, (3) our planned World Domination mode and (4) Custom Campaigns / Mission Builder output as relevant to playable campaigns.
- Save/Load/Continue and slot previews display or access only data belonging to the active campaign.
- Identical slot numbers across namespaces may coexist without collision.
- An attempt to load a different campaign's save must fail at the data boundary, not just be hidden in UI.
- Legacy existing saves must not be deleted, silently moved, reclassified or overwritten.
- Preserve original game modes and binary format during investigation.
- Custom campaigns individually versus collectively separated is not yet specified by the user. Do not choose one without clarification during implementation design.
- Mission Builder as an editor does not automatically imply a separately playable save namespace; save isolation applies to playable campaigns/modes created by the editor, with exact granularity subject to specification.

## Source-level enumeration evidence (new this audit)
Inspected engine/bflibrary/src/x86-win-sdl2/sffind.c, blob b17cc9abc02a0bd1ed79529d47d35009bd10ac4c.
ENUM-01: LbFileFindFirst(filespec, ffind, attributes) applies lbFileNameTransform when enabled before invoking _findfirst; therefore directory enumeration follows the installed filename transformation, same as LbFileOpen under the inspected SDL2 backend.
ENUM-02: LbFileFindNext continues the enumeration through the reserved handle, without a campaign filter in this API. Filtering would have to be designed above the generic file-library layer or via a validated campaign-specific filespec resolution mechanism.
ENUM-03: The generic file-listing API has no campaign argument. There is no evidence that changing LbFileFindFirst alone could enforce campaign separation across the existing save UI paths, which also enumerate by calling load_save_text for slot numbers.

## Assembly evidence imported from parallel workstream — not independently re-traced here
The currently updated docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md documents that Mission Builder accessed the assembly GitHub blob and identified LoadSaveGame, a 10-slot save description loop via ac_load_save_text, ac_load_player/ac_save_player, plus another main-menu save-description pass. These exact assembly references belong to MB-AUDIT-0.8 and require cross-workstream test coverage. Previously noted assembly access blocker is superseded by that workstream; do not state it is unresolved globally.

## Integration gates (no architecture selected)
1. Pin actual assembly references and current save list/Continue state transitions to one evidence table.
2. Establish existing Original Campaign vs American Revolt save provenance; ambiguous legacy files must be preserved and not guessed into a silo.
3. Agree campaign identity lifetime, representation and all user-visible and low-level access-control points with Mission Builder.
4. Design non-destructive namespace layout and explicit legacy compatibility plan, subject to separate approval.
5. Validate same-slot separation, wrong-mode rejection, main-menu previews, load/save screens, continue/resume, failures and file-preservation in tests before merge.

## Test status, scope and rollback
Static reading of sffind.c and coordination contract; no implementation, build, Windows playtest, retail assets or actual save migration. v0.7 baseline unchanged. No rollback necessary for docs-only update. Continue auditing without altering engine.
