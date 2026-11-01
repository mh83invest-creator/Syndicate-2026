# Syndicate 2026 — World-map territory index caller audit

Date: 2026-11-01. Static review only.

## Four direct assembly calls to _draw_map_area
- syndre.sx line 31779: next_mission_countries entry is explicitly rejected when >=0x32 (50) at 31751–31753. This establishes an upper bound at this callsite.
- line 32268: display_cities passes loop variable bl, which starts at 0 and increments until it reaches 0x32 at 32238–32282. This supplies indices 0–49.
- line 32748: a territory index is read from data_5b069 and decremented before drawing; can_i_play_this_city is called, and drawing is conditional on its result. There is no explicit local 0–49 range check immediately before drawing. In particular a zero table value could become -1; whether such data exists, or can_i_play_this_city rejects it, requires separate tracing.
- line 32792: loop variable bl is incremented until 0x32 at 32760–32797, supplying 0–49.

## Resource context
_draw_map_area uses 2816 bytes per territory (64 columns x 44 rows), giving a calculated 140800 bytes for 50 blocks if the data is flat and indices are valid. This is not a measured retail file size. The resource is data/mmapblk.dat and current direct callers do not establish actual buffer size.

## Next checkpoints
Trace can_i_play_this_city and data_5b069 possible values, then validate source and destination spans with owned game assets and Windows runtime tests. Retain campaign ID isolation and Mission Builder safe-resource requirements.

## Verification
Indexed assembly inspected. No builds, runtime tests, source edits, asset or save manipulation. Protected v0.7 and Mission Builder branch untouched.
