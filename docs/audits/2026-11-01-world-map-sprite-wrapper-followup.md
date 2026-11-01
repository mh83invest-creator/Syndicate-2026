# Syndicate 2026 — Sprite Wrapper and World-Map Consumer Follow-up

Date: 2026-11-01. Static audit only.

## Verified source
- engine/conf/wrappers_game.conf maps LbSpriteSetup to native C (`C i ppp`). Native engine/bflibrary/src/general/gsprite.c iterates through sprite entries without validating their backing allocation sizes.
- The assembly calls `ApSpriteSetup_ForceHeight` at lines 31261 and 31546; this is distinct from `ac_LbSpriteSetup`, and its implementation and safeguards remain unverified.
- `display_cities` at assembly line 32235 calls `_draw_map_area` at 32268.
- `_draw_map_area` at line 32804 directly reads `world_map` at 32819 and uses data tables (`data_5b062`, `map_countries`) to compute drawing locations. No immediate null check on `world_map` is visible in the inspected entry path. Whether missing resources could leave this pointer null, and whether bounds are otherwise guaranteed, remain unverified.
- The world-map resource load at 31724 is not followed by a check of the native batch-loader failure count before `display_cities` at 31727.

## Outcome and handover
This confirms native sprite setup dispatch and identifies an additional unguarded-looking world-map consumer. No runtime crash or invalid address access has been reproduced. Next audit should trace world_map allocation, data source, error paths and bounds, and locate ApSpriteSetup_ForceHeight implementation before designing changes. Preserve v0.7 baseline, existing saves, and the concurrent Mission Builder campaign-isolation contract.

Verification: repository source inspection only; no compilation, runtime test, asset inspection or code modification.
