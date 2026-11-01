# Syndicate 2026 — Language Briefings and Map Identity Verification
Date: 2026-11-01 Australia/Brisbane
Status: static assembly and installer audit only; no code changes or runtime tests
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; engine/util/install; engine/src/main.c
Related: docs/audits/2026-11-01-resource-packaging-mission-number-ranges.md; docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; docs/SHARED_DEVELOPMENT_HANDOVER.md

## Verified
LANG-01: Assembly load_text at lines 55249–55289 branches on EXPORT_SYMBOL(language): zero selects data/miss%02.2d.dat; nonzero selects data/miss%01d%02.2d.dat with the language value as first formatted parameter. Literal strings at 82522–82525. Thus installer miss01–50 and miss101–150, 201–250, 301–350, 401–450 are organised as source-language variants for mission indices 01–50, not five independent campaigns. This is a source-derived format explanation; no retail assets opened.
LANG-02: The integer format indicates two-digit mission ID in the mission text filename. Assigning indices >99, or treating 101–450 as independent IDs, is not validated by source and could collide with language-variant paths.
MAP-01: load_map_level at lines 27845–27903 updates current_levno from a mission argument, while level__MapNumber is used separately at 27881–27903 to patch two ASCII digits in the map filename before ac_LbDataLoadAll. The tactical map index is not equivalent to mission ID, nor to a campaign namespace.
MAP-02: Neither language nor map/mission number provides an approved Original/American Revolt/World Domination/Custom campaign identity. Existing campaign save silos are required independently.

## Additional source issue to investigate — not a demonstrated bug
At lines 55280–55286, load_text calls LbFileReadRNC and LbFileLengthRnc, then writes a trailing NUL using the reported length as an index without an obvious local negative-value check. A missing/unreadable briefing could theoretically yield an invalid index. Verify the called library return contract and buffer sizing before changing anything; do not assert runtime failure.

## Next audit gates
1. Inspect LbFileReadRNC and LbFileLengthRnc return semantics and the brief_text storage capacity before classifying the possible bounds issue.
2. Trace resource and campaign selection separately; determine actual American Revolt support with legal assets and Windows execution.
3. Align Mission Builder design with explicit mission ID, map ID, language variant and campaign ID; do not infer one from another.
4. Continue enforcing distinct save namespaces for Original, American Revolt, World Domination and playable Custom Campaigns without touching existing saves.
5. Produce Windows build and gameplay verification only after approved implementation.

## Verification/handover
Full assembly blob exact-line inspection; installer reviewed. Documentation only. No runtime, binary tests, asset inspection, migration, code changes, or version bump. Protect v0.7 baseline.
