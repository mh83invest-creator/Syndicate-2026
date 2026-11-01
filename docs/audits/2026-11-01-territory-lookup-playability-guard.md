# Syndicate 2026 — Territory lookup zero guard and playability audit
Date: 2026-11-01. Source-only, no engine changes.

## Verified
- In syndre.sx lines 32682–32695 the fourth _draw_map_area caller reads an entry from data_5b069 and explicitly skips an entry when it is zero.
- The same path subtracts one from a nonzero value and consults can_i_play_this_city before drawing (32720–32748). Its local branch does not explicitly enforce that the table entry is <=50.
- can_i_play_this_city (32871 onward) has a cheat_worldmap bypass returning true (32877–32880); in its other branches it indexes country state/territory tables based on the passed byte and tests ownership/playability, without a general range validation at entry.
- Earlier handover's hypothetical zero-to-minus-one risk is corrected: this specific caller skips zero before decrement. The outstanding issue is whether entries greater than 50 can occur and whether index validation is otherwise guaranteed.
- Other three _draw_map_area callsites have source-level 0–49 bounds as previously documented.

## Outstanding
Independently enumerate every value in the full table, account for record stride and the precise column mapping, and test source/destination extents with legal assets on Windows. No out-of-bounds access or crash has been reproduced.

## Handover
Reviewed assembly lines 32682–32748 and 32871–32972; no build, runtime test, gameplay, save modification or code patch. v0.7 and Mission Builder branch protected.
