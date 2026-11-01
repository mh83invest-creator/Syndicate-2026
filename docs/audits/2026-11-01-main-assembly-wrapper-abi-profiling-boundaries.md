# Syndicate 2026 — Main Systems Audit: assembly wrapper ABI and profiling boundaries

Date: 2026-11-01 (Australia/Brisbane). Documentation only; default main branch.

## Direct GitHub reads
- `engine/conf/wrappers_libc.conf` blob `5542cd86e020f6e01bc0a992923f3ae2bb7b0d52`.
- `engine/conf/wrappers_game.conf` blob `1be08603b34cf2361780de36ecfa677e442e9ce4`.
- `engine/util/mkwrappers` blob `fa3eba2ea22878d1387c1ec4f1bec5ba5305b571`.
- `engine/src/Makefile.am` blob `dfaad56c157bd9042bec49e681b619794b0fcba3`.
- `engine/src/game.h` blob `fc86093ba9a84f651b1b46fd96e4596f47655422`.
- Previous profiling specification: `docs/audits/2026-11-01-main-profiling-entrypoints-and-timer-selection.md`.

## Confirmed ABI and code generation
1. Wrapper configuration explicitly defines W (Watcom: integer/pointer arguments in eax, edx, ebx, ecx), C (cdecl stack arguments), and V (variadic) conventions, plus return/argument type markers. This is not a universal permission to call C functions directly from any assembly location.
2. `mkwrappers` creates `ac_`-prefixed wrappers, generates push operations for stack arguments versus Watcom registers, preserves ecx and edx around C calls in its helper routines, and manages stack adjustment / prologue. It also supports output symbol mapping and optional debug generation.
3. `wrappers_game.conf` explicitly exposes `game_update`, `game_update_menu`, `game_handle_sdl_events`, `swap_wscreen`, `update_vscreen_whole_vres16`, `LbScreenSwap` and relevant audio interfaces as C-convention wrapper targets. Their signatures are listed there; this supports treating these existing calls as approved assembly-to-C bridge boundaries.
4. `engine/src/Makefile.am` builds C sources and `syndre.sx` alongside generated `wrappers_game.o`, `wrappers_dos.o`, `wrappers_libc.o`, and `wrappers_util.o`. The generated assembly passes through a C preprocessor, `ASFILTER`, and assembler.
5. The actual generated wrapper assembly and final Windows compiler flags were **not** built or inspected in this checkpoint. Correct stack alignment, flags, register liveness, reentrant callback effects and cycle overhead at any new insertion point remain unverified.
6. The direct `wrappers_util.conf` fetch returned empty content. No absence conclusion about any compiled wrapper is drawn from that alone.

## Diagnostic guidance, not implemented
Use existing C timing boundaries first; record monotonic high-resolution per-stage timestamps with deferred/bounded output. For assembly-only effect/collision stage timing, verify actual disassembly and control flow before proposing probes: avoid disrupting flag-dependent jumps, register/stack values or the `move_it` vehicle tail jump. If a new wrapper is needed, first document its arguments, return convention, caller-saved vs callee-saved registers and generated assembly, then compare a diagnostic build with an uninstrumented baseline. Protect tactical 16 FPS, explosions, graphics, audio and saves.

## Outstanding validation
Exact assembly label boundaries, a generated wrapper listing, Win32 toolchain-specific ABI validation and gameplay profiling are outstanding. No code, configuration or build modifications were made. Graphics Audit and Mission Builder untouched.
