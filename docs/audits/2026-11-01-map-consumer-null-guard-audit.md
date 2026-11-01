# Syndicate 2026 — Mission map consumer failure-path audit

Date: 2026-11-01. Static inspection only.

## Evidence
- engine/conf/wrappers_game.conf maps LbDataLoadAll to native C; native engine/bflibrary/src/general/gdata.c counts failures and can leave a failed item's Start pointer NULL.
- syndre.sx lines 27900–27903 call ac_LbDataLoadAll(mission_load_files) without checking its returned failure count, then call init_level_data at 27930.
- init_level_data at lines 27719–27722 passes map_buf to init_map_data without a visible null check.
- init_map_data at lines 25694–25716 advances 12 bytes into its incoming pointer, then loops 0x3000 (12,288) times reading a table entry and updating it as a pointer. No local null or buffer-length check is visible.
- init_level_data then passes other loaded sprite pointers to ac_LbSpriteSetup at lines 27723–27737, also without checking the batch loader's failure count.
- display_cities at lines 32235 onward uses rendering state after world-map resource loading; null checks in its deeper drawing routines are not yet established.
- ApSpriteSetup_ForceHeight is not defined under that exact label in the inspected assembly blob; implementation may be elsewhere. Do not assert its own null-handling behavior.

## Assessment
The map load -> init_level_data -> init_map_data sequence contains no intervening return-code or pointer guard in the inspected assembly. A missing map can therefore plausibly lead to invalid memory access; this was NOT reproduced at runtime. Table length is a required input-format constraint, not proof that all real maps contain or lack sufficient data.

## Next
Trace native map file loading and map buffer allocation using legal resource fixtures; inspect sprite setup's C implementation and downstream world-map rendering. Plan controlled Windows i686 missing-resource tests before any implementation. Preserve v0.7 and concurrent Mission Builder work, including campaign-isolated saves.

## Verification
Exact assembly source review only. No executable or gameplay test, no code patch, and no save manipulation.
