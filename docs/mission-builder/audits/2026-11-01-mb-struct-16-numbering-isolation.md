# MB-STRUCT-16 — Mission numbering and resource isolation boundaries
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder static audit; no code modifications.

## Verified source (engine/src/syndre.sx)
- `load_map_level` obtains a 16-bit level number, stores it as `current_levno`, divides it by 10 and overwrites only positions +9 and +10 of `level_fname` (27845–27867), initialized `data/game01.dat` (80475–80476).
- The two-character filename encoding is not proven to safely represent arbitrary values: although the arithmetic uses a 16-bit number, values >=100 can produce a non-digit tens character rather than a valid `NN`. Source does not show a local 1–99 bounds check in `load_map_level`; this is a conditional limitation, not demonstrated end-user behavior.
- Single-player call at 28531–28536 passes `current_levno`; multiplayer path at 28493–28498 passes the word at `data_5e553`.
- `data_5e553` is assigned from `current_levno` in a player-slot initialization path (28305–28309). This establishes a source-backed copying relationship, not a guarantee every multiplayer slot always matches in every scenario.
- At least one campaign city-selection route sets `current_levno` from a mapping array `data_5b071` (32010–32021, 32055–32066); the map ID and mission number must not be assumed equivalent without inspecting the campaign map.
- The observed native tactical file pattern is `data/gameNN.dat` (MB-STRUCT-15). No evidence that blindly placing new files in that namespace is safe or that existing campaign numbering is free.

## Mission Builder constraints
- Do not allocate arbitrary mission numbers by extending the native two-digit filename arithmetic; no assumption that 100+, negative numbers, or colliding 01–99 indices work.
- Existing campaigns (Original, American Revolt) and each future Custom Campaign must retain independent resource identifiers and between-mission-only saves, as already approved.
- Isolated custom resource namespace, campaign selection and routing are future design requirements, **not** implemented mechanisms. Any integration needs separate mapping validation and approval, not overwriting classic `data/gameNN.dat` or repurposing established indices.
- Check campaign-to-mission map and loader call paths before selecting a technical implementation. Do not alter UI, engine mechanics, graphics or campaign saves in this audit.

## Next source audit
MB-STRUCT-17: trace `data_5b071` and mission-selection mapping / original campaign resource index limits, to assess collision risks for a separately selected custom campaign.

## Verification
Direct GitHub assembly source inspection only; no builds, synthetic fixtures, asset unpacking, runtime test or engine/source edits.
