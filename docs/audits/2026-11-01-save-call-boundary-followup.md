# Syndicate 2026 — Save Call Boundary Follow-up
Date: 2026-11-01 (Australia/Brisbane)
Status: static source review only; no engine changes
Prerequisite: docs/audits/2026-11-01-save-integrity-static-audit.md
Coordination: docs/SHARED_DEVELOPMENT_HANDOVER.md and docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md

## Inspected source
- engine/conf/wrappers_game.conf
- engine/src/main.c
- engine/src/game_data.h
- Previously checked engine/src/game_data.c, game.c, dos.c

## Findings
CALL-01: Assembly/native boundary is established in wrappers_game.conf. The wrapper declarations explicitly expose `load_save_text C v pi`, `load_player C v i`, and `save_player C i pi` to the translated assembly layer. These correspond to C signatures with no campaign identifier. Their presence supports assembly calling these functions but does not identify individual assembly callsites.

CALL-02: Additional file operations `LbFileOpen`, `LbFileRead`, `LbFileWrite`, `LbFileClose`, `LbFileExists` and `LbFileDelete` are also exposed as wrappers. A campaign save isolation design cannot rely on interception of only three game_data.c functions. Whether each is invoked by save-screen assembly remains unverified.

CALL-03: main.c enters `syndicate()` after input, graphics, language and audio initialization. No separate C campaign-selector entrypoint is shown in this startup sequence. Selection and Continue logic cannot be declared mapped without tracing assembly.

CALL-04: GAME_DESC_TEXT_MAX_LEN is 20 and game_data.h declares CPObjective as a packed structure, plus 128 objectives in a global array. This is not a campaign identity structure, and does not authorize modifying the mission-objective layout for save namespaces.

CALL-05: Path handling surfaces include the native LbFile* API and dos_low_level_open->game_transform_path->dos_open route. The exact final path transformation mechanism for LbFile* under installed file-name callback must be verified before proposing a unified routing hook.

## Compatibility controls
- Keep existing Original Campaign, American Revolt, camera, mission pacing and 32-bit ABI unchanged.
- No alterations to the 28247-byte source-derived legacy save layout until runtime-sized validation.
- Do not select campaign ID storage or silently classify existing saves.
- No new campaign modes, UI, random missions, or save routing implemented in this audit.

## Verification
The named GitHub files were read in full using the connected repository. Findings CALL-01 through CALL-05 are bounded to inspected source; assembly caller trace is NOT completed.
No local source checkout, compilation, Windows testing, retail-game-asset verification or runtime save tests performed.

## Next audit tasks
1. Retrieve or locally inspect syndre.sx with line-oriented search to locate callers for load_save_text, load_player, save_player and raw LbFile* operations, plus menu and Continue dispatch.
2. Confirm actual import symbol naming and calling conventions from the generated wrapper outputs.
3. Trace mode identity from menu entry through save slot screens, mission start and resume.
4. Review filesystem mapping and enumeration functions to assess how isolated save listing can be enforced without disturbing legacy data.
5. Prepare a test-only harness for save failures and truncated-file input without altering gameplay code, then run the Windows build and eventual owner playtest.

## Handover
Documentation-only audit commit; no new build/version or gameplay claim. Read this together with the previous save integrity audit and Mission Builder handover. Keep stable v0.7 rollback package untouched.
