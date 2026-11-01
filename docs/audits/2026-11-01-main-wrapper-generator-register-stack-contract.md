# Syndicate 2026 — Main Systems Audit: generated wrapper register and stack contract

Date: 2026-11-01 (Australia/Brisbane). Default main branch; documentation only.

## Source directly inspected
- `engine/util/mkwrappers` blob `fa3eba2ea22878d1387c1ec4f1bec5ba5305b571`, generation helpers ~114–267.
- `engine/conf/wrappers_game.conf` blob `1be08603b34cf2361780de36ecfa677e442e9ce4`, game/display callback declarations ~237–243.
- `engine/src/Makefile.am` blob `dfaad56c157bd9042bec49e681b619794b0fcba3`, wrapper build recipe and linked objects.
- Prior report `docs/audits/2026-11-01-main-assembly-wrapper-abi-profiling-boundaries.md`.

## Verified generator behaviour
1. `generate_push_arguments` uses stack offsets from EBP for C, V and S conventions, and register operands for up to four Watcom arguments (eax, edx, ebx, ecx) in its alternate branch. Additional Watcom arguments are read from the stack.
2. `generate_store_registers` emits pushes for ECX and EDX; `generate_restore_registers` pops EDX and ECX. The generator's own explanatory comments discuss the Watcom and i386 System V register preservation differences. Do not generalise this to every register/flag at arbitrary inline assembly sites.
3. `generate_call` emits a native call then stack adjustment for pushed arguments. `generate_prologue` / `generate_epilogue` create and leave an EBP frame. `generate_ret` distinguishes stdcall cleanup and Watcom with stacked arguments.
4. `generate_wrapper` selects code by calling convention: Watcom, C/stdcall, and variadic wrappers. Optional stack alignment and debug output are generated conditionally; the actual compiler arguments and generated Windows wrapper output have not been verified here.
5. `wrappers_game.conf` registers game update, menu update, SDL event handling, `swap_wscreen` and `update_vscreen_whole_vres16` as C wrapper targets, giving established assembly-to-C entry points for coarse, boundary-level diagnostics.
6. `Makefile.am` builds `syndre.sx` and generated `wrappers_*.sx` via preprocessor, assembly filter and assembler, linking the resulting objects into the executable.

## Safe measurement implication
Prefer instrumenting C-level stage boundaries and event/audio/display callbacks first. Direct insertion into assembly effects/collision code cannot be assumed ABI-neutral: CPU flags, live registers, stack layout and tail-jump execution paths need exact disassembly verification. Preserve original v0.7 tactical 16 FPS, explosion damage and visuals, audio and save semantics.

## Test/checkpoint status
Source-only inspection completed. No generated wrapper binary, Windows disassembly, build, profile, gameplay test, branch modification or engine source edit. Exact generated wrappers and assembly callsite live-register analysis remain pending.
