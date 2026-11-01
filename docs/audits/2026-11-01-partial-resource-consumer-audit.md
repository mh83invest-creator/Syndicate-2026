# Syndicate 2026 — Partial Resource Load Consumer Audit

Date: 2026-11-01. Source-only handover; no code changes.

Inspected engine/src/syndre.sx and native C wrapper mapping.

The eight direct assembly ac_LbDataLoadAll calls have no immediate check of the returned error count. Specific downstream consumers are:

- Graphics setup, lines 31252–31263: loads mode resources and passes pointer_sprites, pointer_sprites_end and pointer_data to ApSpriteSetup_ForceHeight. No resource error-count check between these calls.
- Menu setup, lines 54162–54203: loads menu assets, optionally sound, then performs inlined sprite-table processing using m_sprites and m_sprites_end; loop logic exists, but is not a demonstrated check of the load error count.
- Mission setup, lines 27903–27930: loads mission assets, then sound and additional data, then calls init_level_data. No immediate load error-count handling.
- World-map setup, lines 31724–31727: loads mmapblk resources, then calls display_cities without a load error-count check.
- Sound paths at 27909–27911, 54172–54174, 55040–55042 continue to ac_sound_bank_setup after loads without checking the batch result.

These findings are source-level control-flow observations. They do not demonstrate that any particular downstream routine dereferences invalid data in practice. Next: inspect consumers' internal null guards and control-flow, then formulate targeted missing-asset regression fixtures. Do not patch source without a reviewed implementation plan.

Verification: assembly source inspected. No Windows compile, runtime diagnostics, game execution, asset tests or save operations. Preserve protected v0.7 baseline and separate Mission Builder branch; ensure campaign save isolation requirements remain unchanged.
