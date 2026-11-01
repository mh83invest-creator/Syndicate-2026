# Syndicate 2026 — Main Systems Audit: profiling entrypoints and time-source selection

Date: 2026-11-01 (Australia/Brisbane). Workstream: MAIN. Documentation-only.

## Current source inspected
- `engine/src/game.c` blob `9f56221d527075fba2b1f2406fab60af28a4289a`, frame update lines 142–226.
- `engine/src/display.c` blob `9c6ba018f31d67b394c5973d05d6e51824171b68`, `swap_wscreen` around 241–261.
- `engine/bflibrary/src/x86-win-sdl2/stime.c` blob `7961c77a486b7528e7a5c562078c54c8619bbed1`, `LbTimerClock` around 60–78.
- `engine/src/Makefile.am` blob `dfaad56c157bd9042bec49e681b619794b0fcba3`, C source list and assembly wrapper/translation build rules.
- Prior `docs/audits/2026-11-01-main-audio-callsites-and-windows-profile-contract.md`.

## Confirmed instrumentation boundaries
1. `game_update_full` handles frame wait and SDL event pumping, with `SDL_GetTicks` to track millisecond timing and `SDL_Delay` during wait. Diagnostic accounting must separate wait, event pump and actual update/present work. Tactical `game_update` requests 16 FPS and menu `game_update_menu` requests 30 FPS.
2. `LbTimerClock` is platform-conditioned: non-`__unix__` code derives milliseconds from `clock()`; Unix branch uses `clock_gettime(CLOCK_MONOTONIC)`. Do not treat non-Unix `LbTimerClock` as a proved high-resolution monotonic wall-time profiler for a Windows run.
3. `swap_wscreen` checks existing screen lock state; copies either Vres16 or MCGA screen data under lock, unlocks, calls `LbScreenSwap`, and if previously locked loops until `LbScreenLock` succeeds. Measurements should distinguish copy, lock, screen swap/present and any lock reacquisition. Separate rendering work from intentional waits.
4. `Makefile.am` links engine C with `syndre.sx` and generated `wrappers_*.sx` objects through the assembler filter and compiler pipeline. Do not insert naive C profiling calls into assembly control flow (especially tail jumps) or alter calling conventions without verifying preservation requirements.

## Suggested *disposable* Windows diagnostics (design only, NOT added)
- Prefer a monotonic high-resolution measurement clock such as SDL performance counter and frequency if available in the target SDL version. Verify available symbols and header/build linkage before implementation; do not substitute scheduler tick timestamps without checking resolution.
- Use compact fixed-size per-tick records and counters, with bounded ring buffering and deferred output; never write a line per effect, audio callback or collision directly in a time-critical callback.
- Capture explicit boundaries: frame wait, SDL pump, AIL timer callback durations, fixed simulation stages, MapWho/collision record scans, Vres16 conversion, backbuffer copy, scaled blit, SDL presentation and lock reacquisition.
- Protect RNG state, assembly register/stack semantics, explosion allocations/damage and sound. Produce baseline vs diagnostic side-by-side workloads; distinguish cost of probes from measured workload. Record frame budget overruns and quantiles, not invented FPS improvements.
- Keep error-injection tests (OpenAL ownership and restart) separate from normal performance tracing. Coordinate visual measurements with Graphics Audit; keep Mission Builder isolated.

## Boundaries and outstanding work
This checkpoint inspected source files, not the full assembly instrumentation ABI or actual Windows compiler environment. No diagnostic source, build, tests, Windows execution, performance data, fixes or production modifications. Protected v0.7 remains unchanged.

Next: inspect wrapper ABI conventions and exact C↔assembly entrypoints needed for noninvasive measurement; then seek authorization before any disposable instrumentation branch/code.
