# MB-STRUCT-19 — Campaign configuration and branch divergence inventory

Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder integration audit; no code or asset modification.

## Inspected branches and authoritative evidence
- GitHub recursive tree: `main` 473 entries; `feature/mission-builder-audit` 448 entries at inspection. These are repository-tree counts, **not** measured code/feature differences.
- `main` contains `docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md`, `docs/audits/2026-11-01-american-revolt-launch-resource-audit.md`, `docs/audits/2026-11-01-american-revolt-historical-save2.md` and `docs/audits/2026-11-01-legacy-campaign-identity-followup.md`. These were not returned in the Mission Builder branch's matching tree paths; therefore do not assume main documentation was merged into it.
- The main branch's American Revolt launch audit states that SyndicatFX's documented startup is `syndicatfx.exe`, no explicit American Revolt switch was verified, and `engine/util/install` copies resources from existing game media. Source game media is external, so repo-tree absence is not proof of missing original expansion content.
- The main campaign-save contract confirms currently inspected C save endpoints `load_save_text`, `load_player`, `save_player` and two relevant file-path routing mechanisms. It also documents a separate main-menu save listing. A campaign identity needs to be established **before** listing, reading or writing slots, but this is a proposed integration rule, not implemented behaviour.
- The main legacy campaign identity audit reports no explicit American Revolt selection symbol found in assembly literal search. It also confirms `current_levno` derives from city mapping and is not a stable campaign identifier.
- The main contract still marks custom campaign save-isolation granularity as undecided; the user's approved Mission Builder direction is **each custom campaign has its own save silo**. Treat this discrepancy as a documentation-alignment item; do not silently replace that user-approved requirement with a shared custom namespace.

## Conclusions and integration boundary
1. No verified native American Revolt resource selector, data-root switch or separate executable-mode argument has been established by these inspected sources.
2. Main's evidence supports the need for campaign-qualified Save/Load/list/Continue boundaries; it does **not** prove existing mode routing is functional.
3. Custom campaigns require their own distinct campaign identity and resource mapping, separate from Original, American Revolt and World Domination. No new mission number range is demonstrably safe.
4. Cross-workstream coordination should link/read main's contract, and reconcile the custom-campaign silo wording. Do not copy or merge unrelated main-audit files into the feature branch as a side effect of this Mission Builder audit.
5. Before implementation, inspect actual resource packaging and mode selection using legally supplied original game files and a verified executable; only then select a resource-routing architecture.

## Next audit
MB-STRUCT-20: targeted examination of `engine/util/install`, `engine/src/main.c` and resource-root transformations for implications of an isolated custom mission namespace. Report source facts and uncertainties only.

## Verification
GitHub tree comparison and main-branch documentation review. No source code changes, tests, native builds, retail files or runtime verification. Baseline and classic campaign saves untouched.
