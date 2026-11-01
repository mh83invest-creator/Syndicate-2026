# Syndicate 2026 — MAIN audit: Windows wrapper toolchain and profiling gates

Date: 2026-11-01, Australia/Brisbane. Source inspection only; no engine edits.

## Direct evidence
- `engine/configure.ac` blob `3deb91f7da5abb9b44eb20351f4b1807f9445fd1`, lines 115–153, 212–245.
- `engine/src/Makefile.am` blob `dfaad56c157bd9042bec49e681b619794b0fcba3`, lines 6–80.
- `engine/util/mkwrappers` blob `fa3eba2ea22878d1387c1ec4f1bec5ba5305b571`, lines 327–363.
- `engine/conf/wrappers_game.conf` blob `1be08603b34cf2361780de36ecfa677e442e9ce4`.
- Earlier source-backed assembly exit audit `docs/audits/2026-11-01-main-effect-and-collision-exit-boundaries.md`.

## Findings
1. Configure starts with `stack_alignment=4`. The `*-*-mingw*` host case selects Windows and disables POSIX; the Darwin case instead sets `stack_alignment=16`. Windows does not, in this inspected block, override the initial 4-byte value.
2. `MKWFLAGS` on Windows receives `-r "$(top_srcdir)/conf/fnmap_windows.conf"`. Mac receives the assembler filter; non-Darwin sets `ASFILTER=""`. Symbol prefixing depends on independent `need_underscore` selection rather than a blanket Windows rule.
3. On Windows, `configure.ac` strips `-g` from `CCASFLAGS` due to excessive symbols in `syndre.sx`. Do not assume engine assembly debug symbols are available for direct profiler attribution.
4. `engine/src/Makefile.am` runs the Python `mkwrappers` generator for `wrappers_*.sx`, then processes .sx through preprocessor, optional filter and assembler. Link inputs include generated wrappers and `libbullfrog`/`libbfsound`.
5. `mkwrappers` exposes stack alignment as `-s` / `--stack-align`, with generator-specific validation. Its generated wrapper bridge implements its own argument/register/stack handling. That alone does not certify safety for an arbitrary assembly-internal probe.
6. Existing wrapper declarations cover tactical/menu game updates and screen presentation but do not by themselves certify an ABI-safe wrapper for `move_effects` or `sprite_colide`.
   
## Diagnostic design gates (unimplemented)
Use bounded C-level SDL/event/audio/presentation timing first. For internal assembly measurements, inspect actual emitted Win32 wrapper object/disassembly and all entry/exit control flow with flag/register liveness checks; retain `move_it` tail jump and original simulation ordering. Generate builds only on disposable diagnostic branch/worktree with explicit approval. Run repeatable quiet/explosion scenarios and compare with an untouched release baseline to quantify probe overhead. Keep v0.7 protected, tactical 16 FPS, audio, explosion damage/visuals and between-mission saves intact.

## Test status
Verified source configuration and recipe only. Did not run `configure`, generator, assembler, Windows compiler, game, tests or profiling. Graphics Audit and Mission Builder untouched.
