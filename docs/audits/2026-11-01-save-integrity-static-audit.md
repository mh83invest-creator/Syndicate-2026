# Syndicate 2026 — Save Integrity Static Audit
Date: 2026-11-01 (Australia/Brisbane)
Status: SOURCE AUDIT ONLY — no fixes, build, or gameplay verification
Repository: mh83invest-creator/Syndicate-2026
Source examined: main branch, engine/src/game_data.c (blob 23a7159fc016e58872d30b61b46ba62b6efabac7), engine/src/game.c (blob 9f56221d527075fba2b1f2406fab60af28a4289a), engine/src/dos.c (blob 46c024cabee74aec717b1fe9f2612be2753ca7f8).
Coordination: docs/SHARED_DEVELOPMENT_HANDOVER.md and docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; separate feature/mission-builder-audit workstream.

## Confirmed source findings
SAV-01 — State mutation before opening save file.
In save_player(char *p_desc, int slot), the current credit balance of players[Network__Slot] is reduced by 10% when credits > 30000, before the LbFileOpen(fname, Lb_FILE_MODE_NEW) call. If opening the file fails, save_player returns false, but the deduction already happened in the in-memory player state. This is an observable logical execution sequence; whether a given Windows session encounters it remains untested. DO NOT remove or change the apparent original credit-saving penalty as a side effect of the audit.

SAV-02 — Save success does not validate writes.
save_player() invokes LbFileWrite for description, players, country_states, weapons, cybmods, research and selected_team; it does not inspect return values, then closes the handle and returns true. Short or failed writes could therefore produce incomplete files while the call reports success. File-close and persistence guarantees also remain unverified. No actual save corruption was reproduced.

SAV-03 — Load does not validate binary reads.
load_player() opens a file then calls LbFileRead on the same fixed-layout fields without checking byte counts or a version/format signature, and returns void. A truncated or unrelated .gam file may partially overwrite live globals. This is a conditional risk; not a reproduced crash.

SAV-04 — Save descriptions are read separately.
load_save_text() reads GAME_DESC_TEXT_MAX_LEN bytes into p_desc without checking the number of bytes read, then falls back to the empty-slot string when p_desc[0] == '\0' and explicitly terminates at the final description byte. This cannot be treated as authoritative confirmation of a valid or correctly classified save payload. The code does not expose a campaign identity argument.

SAV-05 — Two IO surfaces; do not assume one interception point.
Native save functions construct save/%02d.gam and use LbFile* functions. game_transform_path_full() handles paths beginning with save/ by resolving them relative to GetDirectoryUser(); dos_low_level_open() calls game_transform_path() and then dos_open(). Other assembly/low-level callers and file enumeration are not yet fully traced. A namespace design must cover ALL save listing/open/read/write/continue paths, not only save_player().

SAV-06 — No campaign-level separation in inspected C APIs.
load_save_text(char*,int), load_player(int), and save_player(char*,int) use slot numbers without a campaign ID. This confirms an integration gap for the approved Original Campaign / American Revolt / World Domination / Custom Campaign isolation requirement. It does NOT prove where campaign selection should be inserted in untraced assembly.

## Scope restrictions
- This audit does not approve a path layout, new .gam format, save migration, UI redesign, or gameplay rule change.
- Never touch, reclassify, overwrite or migrate existing .gam files based on guesses about their campaign origin.
- Existing main/v0.7 build, 16 FPS mission simulation, camera controls and prior playtests remain protected.
- Mission Builder owns proposed campaign identity/interface design; mainline audit owns legacy compatibility and verification, with joint review before source changes.
- The custom campaign per-campaign versus pooled namespace granularity has not been decided.

## Verification and tests
Performed: read back current main branch source content via connected GitHub, inspected exact save/load operations and path transformation, and compared those calls against the existing cross-workstream contract.
Not performed: full assembly caller trace, compilation, test suite, instrumentation, retail-data validation, Windows playtest, save-failure injection, integration tests, file-system durability test, or profiling. No engine or workflow files modified.

## Next controlled audit tasks
1. Trace every calling site for load_save_text, load_player, save_player and DOS file APIs, especially assembly save/load screens, slot enumeration, Continue and campaign-selection paths.
2. Document expected size of each serialized field with exact types and packing, and compare against actual save file sizes using legally obtained retail data when available.
3. Design isolated, synthetic tests for nonexistent/readonly save directory, short write, truncated file, wrong campaign, same-number slots, Continue and legacy-preservation cases. Tests are proposals, not completed.
4. Classify whether pre-open credit deduction is intended original gameplay and decide separately whether failure should restore state; do not silently change the penalty.
5. Coordinate with Mission Builder contract before implementation; handover every later build via docs/HANDOVER_POLICY.md.

## Handover
This note is self-contained and read-only with respect to gameplay. It does not constitute a verified release. Retain the known-good v0.7 package as rollback baseline; no rollback action is required for this docs-only commit.
