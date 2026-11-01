# Mission Builder — Post-consolidation integration path cross-check
Date: 2026-11-01. Branch: feature/mission-builder-audit. Documentation only.

## Evidence
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
1. Menu func_37b29 calls map_selection (54593–54597), then obtains current_levno and calls load_text for brief_text (54620–54626).
2. Menu func_37bbf calls mission_briefing (54655–54663). mission_briefing independently reads numbered game data into level__Seed (1230–1245), then uses level__MapNumber to read numbered map data into map_buf (1246–1274). Return -1 is checked for these calls, but decompression success is not comprehensively checked.
3. Tactical process_action calls load_map_level with current_levno and level_fname (28531–28543), then init_players_people. load_map_level independently loads game and map data and calls init_level_data (27845–27933). No immediate load-result test is visible in tactical caller.
4. load_text (55249–55289) generates language-based numbered mission-text resource names, calls LbFileReadRNC at 55280, then calls LbFileLengthRnc at 55284 and writes a terminator at brief_text[length] at 55286. No negative-length check is visible before indexing. This is a conditional source-level risk, not a reproduced failure.

## Architectural consequence
Resource consistency must encompass world map mission ID, mission text, briefing game and map reads, and tactical game/map reads. A resource resolver must be campaign-qualified before any of these accesses. A tactical-only substitution would leave inconsistent or mixed campaigns possible. Independent validated read-only import is required before accepting arbitrary user-provided mission resources. Original and American Revolt behaviours remain protected.

## Test cases specified, not run
I01 valid original mission follows matching text, briefing and tactical ID; I02 missing text rejects safely; I03 truncated mission RNC rejected before use; I04 mismatched map reference rejected; I05 custom mode cannot silently fall back to classic files; I06 classic mode does not move existing files; I07 localized mission text follows active mode and bounded buffer; I08 missing map fails safely; I09 mission results retain campaign identity; I10 disposable Windows regression for original modes.

## Outstanding
No legal retail fixture tested; no independent bounded importer, campaign selector, resource namespace, saved-game silo, implementation, or gameplay test performed. Source study cannot substitute for those gates.

## Verification
Direct assembly and consolidated audit inspection. No source code, assets, saves, builds or Windows runtime changed. Protected v0.7 untouched.
