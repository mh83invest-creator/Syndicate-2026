# Syndicate 2026 — Main Systems Audit: Save Read/Write Integrity

Date: 2026-11-06 (Australia/Brisbane)
Branch: main (documentation only)
Scope: Native save functions and file-path boundary; original gameplay and Mission Builder integrations remain untouched.
Status: Source-verified conditional risks. No executable test or demonstrated corrupted save.

## Direct sources inspected
- `engine/src/game_data.c` (Git blob `23a7159fc016e58872d30b61b46ba62b6efabac7`): `load_save_text`, `load_player`, `save_player`, `SyndFXFileNameTransform`, `setup_file_names`.
- `engine/src/game.c` (Git blob `9f56221d527075fba2b1f2406fab60af28a4289a`): `game_transform_path_full`, `game_update_full` and `game_update`.
- `docs/SHARED_DEVELOPMENT_HANDOVER.md` (main): campaign save isolation remains a documented requirement, not implemented.
- Source review only, not a file I/O experiment.

## CORE-SAVE-01 — Save call mutates the live player before file creation succeeds
`save_player(char *p_desc, int slot)` reads `players[Network__Slot].Credits`; if over 30000, it subtracts `10 * creds / 100` **before** `LbFileOpen(fname, Lb_FILE_MODE_NEW)`. If the open fails, function returns `false`, but the credit deduction has already happened. This is an observable code ordering fact, with actual runtime occurrence untested. Preserve established game economic rules; any fix requires investigation of the original rationale for the deduction and the proper commit boundary.

## CORE-SAVE-02 — Individual writes are unchecked
`save_player` writes description, players, countries, equipment, mods, research and selected_team using successive `LbFileWrite` calls. It does not inspect any return value or final saved length, then closes and returns `true` if the initial open succeeded. Therefore it cannot report a short/failed intermediate write. Whether a specific backend can short-write under realistic conditions is untested. A prospective safe implementation needs a known expected serialized length and backend-specific write-result semantics, plus a non-destructive test.

## CORE-SAVE-03 — Player loading can partially overwrite live state
`load_player` checks only whether opening succeeded. It then calls `LbFileRead` for the header and all live state structures without checking read lengths, before returning `void`. A truncated file may leave a partially updated mixture of prior state and newly loaded bytes; exact behaviour is backend-dependent and not tested. There is no observed up-front file-size validation or staging buffer/atomic commit at this C boundary. Do not change serialization format or structure sizes without compatibility review.

## CORE-SAVE-04 — Description loading is weakly validated
`load_save_text` starts with an empty string, reads up to `GAME_DESC_TEXT_MAX_LEN` without inspecting result and sets the last byte to NUL; if the first byte remains NUL it uses the localized empty-slot string. This protects the output terminator but does not independently validate the full save payload. A description can therefore potentially appear despite a truncated payload, subject to actual backend and slot data.

## CORE-SAVE-05 — Two filename resolution paths still matter
`setup_file_names` assigns `lbFileNameTransform = SyndFXFileNameTransform`, which prefixes the HDD/data root to relative names. The separate `game_transform_path_full` routes `save/` paths to the user root. These are not equivalent. This corroborates the Mission Builder audit's existing resolver discrepancy; do not introduce isolated save namespaces until both entry points and legacy location behaviours are covered.

## Acceptance gates for later, separately approved implementation
1. Verify `LbFileRead` and `LbFileWrite` result conventions on the actual Windows backend and current compile flags.
2. Establish a byte-accurate legacy save file layout and expected file length, including header and variable structure sizes.
3. Test simulated file-open failure and short writes; confirm no state change/false success.
4. Test truncation at each serialized segment with disposable copies, verifying live state is not partly committed on failure.
5. Validate legacy save access and active-campaign save filtering in coordination with the Mission Builder save integration contract. Never overwrite user saves in testing.
6. Preserve any intended credit deduction or campaign rules pending explicit product decisions, rather than silently removing it.

## Verification
Read and analysed the live GitHub source files and previously committed handover. This document is new static audit evidence only.
No engine code edited, no compilation, no CI, no Windows or gameplay testing, no save files accessed, no Graphics or Mission Builder branch edits.
Next main audit: inspect actual low-level I/O return semantics and save-block size; independently continue effect cost and timing investigation. 
