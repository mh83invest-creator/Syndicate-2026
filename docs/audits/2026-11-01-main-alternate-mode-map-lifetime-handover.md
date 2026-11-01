# Main Systems handover — alternate mode map loading

Date: 2026-11-01. Report: docs/audits/2026-11-01-main-alternate-mode-map-lifetime.md, commit 1c40eee73ecc66b0332f8ba9983db46757d972e8.

Inspected exact assembly descriptor. unkn1_empty_load_files is an all-zero terminated resource list; it does not replace map_buf. The DrawFlags==4 tactical path still calls init_level_data and therefore init_map_data, requiring a previously established map_buf. Mode-2 successful loading explicitly refreshes map_buf via mission_load_files. Do not assert a proven double relocation until mode-4 reachability and briefing transition are traced. No code changes or playtesting. v0.7 and parallel Graphics/Mission Builder untouched.
