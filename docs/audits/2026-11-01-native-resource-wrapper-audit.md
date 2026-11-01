# Native resource wrapper audit — 2026-11-01

Source reviewed: engine/conf/wrappers_game.conf, engine/bflibrary/src/general/gdata.c, engine/src/syndre.sx.

Wrapper configuration maps LbDataLoadAll and LbDataFreeAll to C. The C batch loader counts failures and returns the count. Eight direct assembly calls to ac_LbDataLoadAll were found. The callers inspected do not appear to test the returned count immediately; further path-sensitive analysis and runtime tests are outstanding. Native LbDataFree resets pointers; a resource read failure after allocation may lose the allocated pointer without freeing it.

This is an audit, not a verified runtime bug. No engine edits, compilation or gameplay tests. Preserve protected v0.7 baseline, save files and Mission Builder branch.
