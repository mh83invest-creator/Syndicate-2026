# Map resource ownership handover

2026-11-01. Report: docs/audits/2026-11-01-main-map-buffer-resource-ownership.md, commit 0ce74551efe3aec11d91c6ba33a761bf853d8549.

Confirmed the mission resource descriptor owns an entry for map_buf; the tactical loader loads it and free_map_level frees the descriptor. Briefing also writes map_buf and both paths relocate map offsets. Allocation internals and whether both relocations can touch identical data are unverified. Source-only audit. No program changes, builds or playtesting. Preserve v0.7 and the other two audit branches.
