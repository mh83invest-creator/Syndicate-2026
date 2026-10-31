# Syndicate 2026 — Assembly Save Status and Legacy Provenance Audit
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE REVIEW ONLY; no gameplay modifications, builds, or runtime tests
Repository: mh83invest-creator/Syndicate-2026
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf (85,803 lines), fetched in full by GitHub fetch_blob. Inspected alongside engine/src/game_data.c and docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md.

## Verified assembly findings
ASM-SAVE-01: LoadSaveGame at line 30354 loops ten slots and calls ac_load_save_text at line 30397. Save screen uses global savegame_slot, checks slot < 10, and invokes ac_load_player at line 30627 and ac_save_player at line 30643.
ASM-SAVE-02: Immediately following ac_save_player, assembly sets a flag to 1 at lines 30644-30646 without testing the function result. save_player() returns TbBool, but this path does not branch on that result. Therefore failure reporting is a potential false-success/transition risk, not yet demonstrated in gameplay.
ASM-SAVE-03: ac_load_player is called at line 30627 and the UI likewise sets a completion flag at lines 30628-30630. The native load_player() returns void and silently returns on failed open; assembly cannot currently discriminate missing/corrupt/incompatible files through this native return value.
ASM-SAVE-04: The main menu has a separate ten-slot description loop using ac_load_save_text at lines 54574-54583. A campaign-aware implementation must apply the same active namespace to this preview and the LoadSaveGame screen; hiding cross-campaign saves only in one view is insufficient.
ASM-SAVE-05: The present assembly file has no literal case-insensitive matches for "American", "Revolt" or "campaign" (plain-string search), so the legacy campaign identifier remains unresolved. Absence of those English literals is NOT proof that the engine has no alternate campaign state. Inspect menu data, map tables, resources, binary save fields and gameplay paths; do not infer from filenames or slot numbers.
ASM-SAVE-06: The historic aCSyndSave symbol is referenced by old ASM_save/load functions, but verified active ac_ wrappers use native C. Do not assume the old string is currently used for mode selection or save location.

## Required compatibility investigation
- Determine whether original and American Revolt gameplay are differentiated through executable state, alternate data files, mission sequence, save payload, or other persistent metadata.
- Preserve every existing slot in place until reliable provenance is established. If ambiguous, document ambiguity and seek an explicit migration/compatibility decision; NEVER guess ownership.
- New modes (World Domination and user-built campaigns) must have separate Save/Load/Continue lists and file namespaces; same slot numbers cannot collide.
- Save/load failures must be surfaced correctly once a separately approved implementation begins; preserving original save penalties and game semantics requires explicit review.
- Mission Builder remains a separate workstream and must adopt the cross-workstream contract; no new storage layout or API is approved by this audit.

## Verification
Performed full-blob retrieval, line-indexed inspection of save handlers, and literal search for candidate campaign strings. No binary execution, automated tests, Windows build or save-file content inspection. No source or workflow modifications.

## Handover
Read alongside docs/audits/2026-11-01-campaign-save-silo-enumeration.md, docs/SHARED_DEVELOPMENT_HANDOVER.md, docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md and the Mission Builder branch handover. Current known-good main/v0.7 game source is unchanged; documentation commit only, no rollback necessary.
Next: trace mode-selection and legacy save identity via assembly menu state and resources; prepare scoped failure and save-isolation test design before proposing code.
