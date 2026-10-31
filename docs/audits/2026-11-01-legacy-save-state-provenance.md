# Syndicate 2026 — Legacy Save State Provenance Audit
Date: 2026-11-01 (Australia/Brisbane)
Status: SOURCE-ONLY INVESTIGATION; no changes to engine, tests, or format
Predecessor: docs/audits/2026-11-01-legacy-campaign-identity-followup.md
Cross-workstream contract: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md

## Source inspected
engine/src/game_data.c; engine/src/player.h; engine/src/player.c; engine/src/wrcountry.h; engine/src/research.h. Compared against previous full assembly review.

## Specific findings
PROV-01: The save_player serialization is ordered: 20-byte description; PlayerInfo players[8]; CountryState country_states[50]; weapons; cybmods; Research research; 4-byte selected_team. There is no separately named campaign identifier or version/signature field written by this C function. Some existing unnamed fields might carry other state; no provenance can be assigned without further proof.

PROV-02: PlayerInfo is packed and includes credits, name/company fields, flags, arrays of cyborgs and numerous unidentified field_* members; none is explicitly documented as Original versus American Revolt. A particular value, name or cyborg inventory must NEVER be used to classify a legacy file absent validated semantics.

PROV-03: CountryState is a packed 10-byte record: Happiness, OwningPlayer, Revenue, TaxPercent, field_5, Population; 50 entries are stored. These describe strategic state but offer no directly documented campaign ID. Mission numbers, territory ownership and completed stages cannot be trusted as identifying a campaign across different playthroughs without real-game verification.

PROV-04: Research contains CurrentKind, WMType and several unknown fields. The name WMType is not evidence for an American Revolt or World Domination selector; avoid speculative attribution.

PROV-05: Native game_data.c uses filename save/%02d.gam and takes slot only. The separate assembly save descriptions and load/save UI also use slot indices without campaign ID. Existing binary structure cannot safely establish campaign ownership from source names alone.

## Implication for approved isolation
The user requires distinct Original, American Revolt, World Domination and custom playable campaign save silos. New campaign identity should be explicit and enforced at both file access and the user-visible menu, but this audit does not select the on-disk layout or new APIs. Existing ambiguous legacy saves remain in place and untouched; do not silently move, delete, overwrite or guess their origin.

## Unresolved
- Determine how the engine actually launches and differentiates American Revolt, including separate executable or resources, selection state and mission datasets.
- Compare legally held real legacy saves to determine any trustworthy provenance data; no samples were supplied or inspected.
- Agree exact handling of legacy ambiguous saves and custom campaign granularity before implementation.
- Trace all save/load/Continue paths and error reporting from assembly to underlying filesystem for a test plan.

## Verification / handover
Performed repository source reads and fixed-layout field inventory. Not performed Windows compile, binary ABI measurement, tests, real saves, game execution or campaign identification. No code changes and no rollback needed for this documentation-only commit. Preserve v0.7 protected baseline and Mission Builder branch.
