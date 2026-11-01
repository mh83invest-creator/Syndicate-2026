# Main Systems audit handover — actual bflibrary implementation

2026-11-01. Report docs/audits/2026-11-01-main-resource-loader-implementation-audit.md, commit e084833d8655a48cdd64b07613f82713b5a0442b.

Direct inspection of bflibrary gdata.c/gfile.c establishes freeing and nulling descriptor pointers, allocating/replacing each mission resource, error counts and RNC header/UnpackM1 handling. Critical nuance: mode-2 tactical reload calls LbDataLoadAll(mission_load_files), thus replaces map_buf on successful loads; mode-4 uses another list and map freshness is not yet proven. Tactical assembly does not inspect aggregate load errors at call site. Potential failed-load leak is not runtime proven. No engine modifications, builds, profiling or playtesting; preserve working v0.7, Graphics and Mission Builder audits.
