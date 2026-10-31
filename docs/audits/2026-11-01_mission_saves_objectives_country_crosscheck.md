# Syndicate 2026 — Mission Objective, Save Slot and Country-State Source Cross-check

Date: 2026-11-01 (Australia/Brisbane). Read-only original assembly audit.
Branch feature/v0.9-visual-showcase. Protected baseline v0.7 source, main changed independently.
Mission Builder audit counterpart: feature/mission-builder-audit; see docs/mission-builder/HANDOVER.md, MB-AUDIT-0.3.
Assembly original blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Objective storage
At syndre.sx:9379–9399 an objective entry is located via a 15-byte stride, and at 9459–9467 the routine sets objective fields at byte offsets +6,+7,+8,+9,+11,+13. These are direct source observations; specific meaning of each action code is not proven. The 128-entry CPObjectives array and packed structure are separately documented by the Mission Builder source audit. A custom mission must use a verified loader and objective setup path, not merely allocate an array.

## Original campaign save dispatch
At syndre.sx:30597–30610, menu action changes savegame_slot and marks data_54c4a selection data. At 30621–30630 the load action calls ac_load_player(slot) only for slot values under 10. At 30634–30646 the save action calls ac_save_player(pointer, slot) only for slot values under 10. This is **original menu slot dispatch**. Mission Builder's C audit of game_data.c found save/%02d.gam files and fixed binary arrays. The approved requirement is separate save namespaces for Original Campaign, American Revolt, World Domination, Custom Campaign; no file paths, selectors or namespace routing have been implemented. Do not implement a shared 0–9 slot pool for all modes without isolating the storage path and menu actions.

## Country data and candidate revolt/revenue seam
At syndre.sx:11797–11844 there are actual 10-byte-stride country_states accesses, including adjustments and clamps; at 11853–11890 GetRevenueFromCity references country state and compares a country field against 0xa0 before calculating revenue. This is stronger than assuming wrcountry.c scaffolding owns all strategic behaviour. It does NOT establish the tax-to-revolt trigger, random revolt selection or campaign transition. Country policy code must be traced before a dynamic rebellion engine is proposed.

## Integration warning and gates
Do not conflate graphical DrawFlags with campaign mode. Save selection, mission objective array and country state are original assembly/C shared ABI boundaries. Existing fixed-size binary save data is protected. Branches may diverge as other developers make concurrent documentation changes; review main and the Mission Builder branch before editing shared files.

PASS: exact original assembly line review of objective stride/fields, menu load/save dispatch and country revenue/state references; read the Mission Builder handover.
NOT RUN: build, actual save/load, mission success/failure, random revolts, save silo implementation, Windows gameplay or tests.
NEXT: trace exact objective loader and briefing dispatch plus revolt call chain; prepare a joint interface proposal, but do not alter gameplay. No source/asset changes and no proprietary game assets committed.