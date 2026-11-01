# Syndicate 2026 — Main Systems Audit: recovered assembly and direct control-flow verification
Date: 2026-11-01 (Australia/Brisbane). Documentation-only main branch.

## Source retrieval breakthrough
GitHub contents API ranged reads of `engine/src/syndre.sx` returned empty content in both UTF-8 and base64 mode. Fetching the **same verified Git blob** `c35e22621a93d1e07c44d40de8867446ff7c64bf` using `fetch_blob` returned 1,848,334 characters. The source was split into lines and the exact instructions below were freshly inspected. This supersedes the previous source-read blocker in `docs/audits/2026-11-01-main-assembly-probe-boundaries-evidence-limits.md` but does not retroactively establish runtime verification.

## Direct source findings
- Tactical loop `syndre.sx:220–278`: `call move_it` at 230, `call ac_LbScreenWaitVbi` at 241, `call swap_screen_vres16` at 243, two `scroll_map` calls at 257/265, `copy_back` at 267, `draw_mapwho` 268, `draw_panel` 269, `click_map` 270, and non-DOS `call ac_game_update` at 272. Thus the pacing call appears after the observed simulation/draw pipeline; timing and waits need separate accounts.
- `move_it:33330–33337` contains five `call` instructions (`move_worlds`, `move_people`, `move_weapons`, `move_effects`, `move_objects`) followed by `jmp move_vehicles`. The final operation is a tail jump. Placing entry/exit probes by assuming a normal `call move_vehicles; ret` would change the control-flow contract.
- `move_effects:19979–20007` saves `ebx`, `esi`, `edi`, `ebp`, reserves 0x1c bytes on the stack, scans from `level__Effects` and dispatches effect subtypes with `jmp *%cs:vtable_1eaa8(,%eax,4)`. Handler return to the scanning continuation is controlled by internal labels, not a C callback.
- `func_1f48a:20746–20753` falls through into `func_1f4a0:20757`; that handler invokes `affect_by_wind`, pushes eight arguments and calls `sprite_colide` at 20771, then adjusts `esp` by 0x20 at 20774, pushes arguments and calls `move_mapwho` at 20782. Preserve stack and live flags/register state around this sequence.
- `engine/conf/wrappers_game.conf` (blob `1be08603b34cf2361780de36ecfa677e442e9ce4`) confirms existing C wrapper targets `game_update`, `game_update_menu`, `LbScreenWaitVbi`, `swap_wscreen`, `LbScreenSwap`. The generator ABI audit remains applicable.

## Profiling implication (no code implemented)
Begin with C-level frame/event/sound/display timing; for internal effects/collision probes, separately review function entry/exit, jump table labels, conditional branches, flags and register liveness. The presence of `push ebx/esi/edi/ebp` at `move_effects` entry does NOT mean arbitrary inline calls preserve all necessary state. The `move_it` vehicle tail jump must remain intact. Collect baseline vs probe overhead; preserve 16 FPS tactical, 30 FPS menu, original explosion visuals and damage, audio, campaign save semantics and v0.7.

## Validation
Completed: direct blob-based source reading of specified line ranges and comparison with previous audit. Not performed: disassembly, Windows build, runtime instrumentation, measurements, regression tests or any game-source edits. Graphics Audit and Mission Builder untouched.
