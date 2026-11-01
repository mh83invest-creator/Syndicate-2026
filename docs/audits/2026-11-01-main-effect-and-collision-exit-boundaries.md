# Syndicate 2026 — Main Systems Audit: effect-loop and collision exit boundaries

Date: 2026-11-01 (Brisbane). Main workstream; documentation only.

## Direct source basis
Fetched full Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` for `engine/src/syndre.sx` and freshly inspected lines 19979–20045, 21285–21317, 33330–33337, 3579–3605, and 4655–4671.

## Findings
- `move_it:33330–33337`: successive calls to worlds, people, weapons, effects and objects, followed by a **tail `jmp move_vehicles`**. No local `ret`; an exit probe must not assume the vehicle stage is a conventional call returning inside `move_it`.
- `move_effects:19979–20007` saves EBX, ESI, EDI, EBP, reserves 0x1c bytes and dispatches active effect subtypes with an indirect `jmp *%cs:vtable_1eaa8(,%eax,4)`. Inactive records and multiple handler paths return to shared loop labels.
- `func_1ebe5:20011–20015` pushes EBX, calls `animate_model`, then jumps to shared `jump_1fb0d`, which pops the argument indirectly via `add $0x4,%esp`. Probes in this area must preserve the expected stack delta.
- `jump_1fb10:21295–21307` conditionally updates animation state, flowing into `jump_1fb35`; `jump_1fb35:21307–21310` advances EBX by 0x1e and iterates while below `level__Commands`. `jump_1fb44:21311–21317` restores 0x1c stack bytes and pops EBP, EDI, ESI and EBX before `ret`. Function-level entry/exit measurement is less intrusive than arbitrary handler-label calls; exact instrumentation safety still requires build-specific checks.
- `sprite_colide:3579–3585` separately saves EBX, ESI, EDI, EBP and reserves 0x70 stack bytes. `jump_1378a:4664–4671` copies ECX to EAX, releases 0x70 bytes, pops saved registers and returns. This is a freshly verified shared return boundary, not a count of dynamic collision calls.
- No inference is made that the reported 3×3 neighbourhood bound or lexical collision calls are observed workload measurements.

## Proposed diagnostics, not implemented
Prefer instrumentation at established C frame/present/event/audio boundaries before any direct assembly code. For exact effect and collision whole-call timing, consider a disposable wrapper/trampoline only after Win32 symbol and ABI verification, preserving flags, return EAX, all live registers and stack. Avoid disturbing simulation ordering, RNG, effect counts, MapWho state and the move_vehicles tail jump. Quantify probe overhead separately from observed execution times.

## Verification limits
Static Git source reviewed. No generated binaries, compilation, instrumentation, Windows gameplay testing, performance samples or modifications to protected v0.7. Graphics Audit and Mission Builder untouched.
