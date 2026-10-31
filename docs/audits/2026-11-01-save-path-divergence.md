# Syndicate 2026 — Save Path Divergence Audit
Date: 2026-11-01 Australia/Brisbane
Status: static source analysis, no engine changes, no runtime validation
Previous handovers: docs/audits/2026-11-01-save-integrity-static-audit.md; docs/audits/2026-11-01-save-call-boundary-followup.md
Cross-team: docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md and docs/SHARED_DEVELOPMENT_HANDOVER.md

## Inspected files and exact code paths
- engine/src/game_data.c: setup_file_names(), SyndFXFileNameTransform(), GetDirectoryHdd(), GetDirectoryUser(), load_save_text(), load_player(), save_player()
- engine/src/game.c: game_transform_path_full(), game_transform_path()
- engine/src/dos.c: dos_low_level_open(), dos_open(), dos_path_to_native()
- engine/src/oswindws.c: sys_get_data_path(), sys_get_user_path()
- engine/src/osunix.c: sys_get_data_path(), sys_get_user_path()
- engine/conf/wrappers_game.conf: native wrappers

## Findings
PATH-01 — C save calls construct relative `save/%02d.gam` names and use LbFileOpen. setup_file_names() installs `SyndFXFileNameTransform` as `lbFileNameTransform`. That transform prefixes *relative* names with `GetDirectoryHdd()`, not `GetDirectoryUser()`, when invoked. Therefore the apparent C LbFile route is `<data-root>/save/%02d.gam`.

PATH-02 — The separate `game_transform_path_full()` treats a path starting with `save/` as special, prefixing `GetDirectoryUser()` and stripping `save/`, producing an apparent `<user-root>/%02d.gam` path. This is not the same as the apparent C path above. `dos_low_level_open()` calls `game_transform_path()` prior to `dos_open()`. NOTE: Confirm whether any low-level wrapper already passes an altered path and exactly which assembly callers use this route.

PATH-03 — `GetDirectoryUser()` creates `save` and `screenshots` using relative names via LbDirectoryMake, while the source contains commented-out full-path construction. That may create directories in a different place from the apparent user-root save redirect; actual LbDirectoryMake transformation and working directory require verification. Do not claim a proven user-visible defect.

PATH-04 — Windows user path is conditionally based on a valid SyndicatFX InstallPath registry key AND APPDATA. If either condition fails, sys_get_user_path() returns false and GetDirectoryUser() falls back to ".". The data root itself comes from HKCU/HKLM registry when available and falls back to ".". The actual path depends on installation/working directory and is unverified.

PATH-05 — On Unix, sys_get_user_path uses XDG_CONFIG_HOME or HOME/.config/syndicatfx while sys_get_data_path uses GAME_DATA_PATH, reinforcing that save destinations can differ across IO routes/platforms.

PATH-06 — The current source provides no explicit campaign namespace at either of the inspected path selection boundaries. Interception design must account for both original campaign and American Revolt legacy save lookup, and every future mode, without assuming that path remapping alone enforces save-screen filtering or Continue safety.

## Highest-priority verification questions
1. Locate the Bullfrog LbFileOpen and LbDirectoryMake implementations and verify whether/how lbFileNameTransform is used.
2. Trace assembly save-screen and Continue callsites (syndre.sx); GitHub connector line-range reads returned empty, so these are NOT traced.
3. Establish actual paths and existing save locations with instrumented Windows testing and legally owned game resources when available.
4. Verify whether legacy Original Campaign and American Revolt saves share names, how campaign origin is represented, and whether it can be identified reliably without migration.
5. Test save/load from working directories with and without expected registry keys and APPDATA. No such tests were run.

## Guardrails
No source fixes, path migration, namespace format, save deletion, gameplay adjustments or builds authorised by this audit. Preserve proven v0.7 playtests and legacy save files. Coordinate interface decisions with Mission Builder before coding.

## Verification
Full text of named smaller C files and wrappers was inspected through connected GitHub. Derived routing is conditional on the actual file API implementation and caller use. Windows build, tests, runtime path traces and assembly caller analysis not performed. This document is a documentation handover only; rollback is unnecessary.
