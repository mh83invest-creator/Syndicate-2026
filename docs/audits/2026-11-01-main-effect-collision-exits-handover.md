# MAIN handover — verified effect and collision exit control flow

Date: 2026-11-01. Main workstream; protected v0.7 unmodified.

Audit: `docs/audits/2026-11-01-main-effect-and-collision-exit-boundaries.md`, commit `a505b2b371112a749c770b61d819adc14ec25021`.

Full Git blob read of `engine/src/syndre.sx` (`c35e22621a93d1e07c44d40de8867446ff7c64bf`) confirms: `move_effects` saves four registers and 0x1c local stack bytes, uses an indirect effect jump table, and exits at `jump_1fb44` by restoring those values; shared `jump_1fb35` advances 0x1e-byte records; handler `func_1ebe5` has a shared stack-cleanup jump; `sprite_colide` saves four registers and 0x70 local stack bytes, returning at `jump_1378a` after copying ECX to EAX and restoring its frame. `move_it` still ends with the tail `jmp move_vehicles`.

Next: verify candidate wrapper/trampoline signatures and Windows compiler/assembler ABI on a disposable diagnostic build before attempting assembly probes. Prefer C boundary timing first. No source modifications, Windows tests, measured FPS or graphics/mission-branch changes.

This small checkpoint extends the earlier `docs/audits/2026-11-01-main-assembly-recovery-handover.md` without rewriting the growing cross-workstream handover.
