# MB-STRUCT-17 — City-to-mission mapping and resource collision limits
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder static audit only.
Source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified
- The city selection paths at lines 32010–32021 and 32055–32066 calculate index `city*19` (city*5*4 minus city), load a **byte** at `data_5b071 + city*19`, zero-extend it and assign `current_levno`. The map entry is a mission number, not the city index itself.
- `data_5b071` is within the statically initialized world-map/country data beginning around `map_countries` (84528 onward). The first byte at `data_5b071` is 0x11 (17 decimal), the next record begins at +19 with 0x27 (39 decimal), and a later byte scan checks for a mapped mission number 1 at 32900. This establishes examples and confirms reused table structure; it does **not** comprehensively enumerate all valid map rows or reserved resource IDs.
- The level loader uses the selected value to construct `data/gameNN.dat` by writing two digits (MB-STRUCT-15/16). A mapped index is therefore an actual legacy resource dependency.
- An observed mission ID absent from the inspected static bytes cannot safely be considered unoccupied: separate American Revolt resources, dynamic country state, alternate maps and runtime loading need explicit investigation.

## Mission Builder collision policy
1. Do NOT assign custom missions to apparently unused native IDs or overwrite existing `data/gameNN.dat` assets.
2. Keep Original, American Revolt, and each custom campaign separately selectable with isolated resource IDs and save namespaces; campaign saves remain between missions only.
3. A future custom campaign selector must resolve campaign identity and a campaign-local mission identifier to a distinct validated resource path, rather than passing unbounded indices through legacy two-digit formatter.
4. No reservation of specific ID ranges is approved. The audit does not prove which 01–99 values are occupied/free.
5. Before designing routing, trace full city-record count/indices, American Revolt map and data access, and any external campaign tables.

## Next
MB-STRUCT-18 — examine alternate-campaign resource selection, map table extents and where the existing mode is chosen; remain within Mission Builder integration boundaries.

## Verification
Source inspection and selected record/stride arithmetic only. No full map enumeration, retail asset inventory, binaries, build, source edits or gameplay tests.
