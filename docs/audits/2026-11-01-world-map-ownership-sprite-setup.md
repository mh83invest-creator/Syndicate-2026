# Syndicate 2026 — World-map ownership and graphics sprite setup

Date: 2026-11-01. Static source audit only.

## Evidence
- engine/src/syndre.sx, full indexed assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- mmapblk_load_files, lines 83675–83683, maps data/mmapblk.dat into the world_map pointer.
- world_map is a four-byte pointer slot at lines 85220–85221, not the actual data array.
- World map is loaded through ac_LbDataLoadAll at line 31724; the next consumer display_cities is called at line 31727 without checking the batch loader failure count.
- _draw_map_area at line 32819 reads world_map; check_for_click_on_cityadjust_tax reads world_map at 32164 and uses it to read pixels at 32213–32214. No local null guard was identified in these entry paths.
- ApSpriteSetup_ForceHeight is explicitly implemented in assembly at lines 31546–31580 (previous handover incorrectly said its definition had not been located). It skips processing when start >= end but otherwise traverses six-byte sprite entries and adjusts pointer values according to DrawFlags without validating backing allocation sizes.
- This assembly function is separate from the native C LbSpriteSetup, whose wrapper mapping was confirmed earlier.

## Significance
World-map loader failures can plausibly affect drawing or hit testing, but no runtime crash has been reproduced. Sprite-table range comparison is a limited safeguard and not a substitute for resource validation. Real asset lengths and the resource cleanup path remain unverified.

## Next checkpoints
Trace world-map lifetime and frees, distinguish menu/world-map ownership, check drawing address bounds and historical resources, and design controlled Windows tests. Preserve v0.7 engine and Mission Builder branch. No source modifications, no save changes and no gameplay testing.
