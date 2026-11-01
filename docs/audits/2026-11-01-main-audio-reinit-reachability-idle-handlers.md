# Syndicate 2026 — Main Systems Audit: audio reinitialization reachability and idle-handler lifetime

Date: 2026-11-01 (Australia/Brisbane). Branch: main. Documentation only.

## Current GitHub source examined
- `engine/src/main.c` blob `e9621c6594173569e78366553ddc2de820e407f8`, lines 316–370.
- `engine/src/game.c` blob `9f56221d527075fba2b1f2406fab60af28a4289a`, `game_quit` lines 106–111, `host_reset` lines 210–220.
- `engine/bflibrary/src/x86-win-sdl2/swindows.c` blob `e1a1114e3fcbf5e0231e2ff3067b0024c7bd84ea`, initialization lines 49–64, registration lines 194–206, reset lines 208–217.
- `engine/bfsoundlib/src/bfaudio.c` blob `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`, idle-registration around 111–117 and startup around 93–105.
- Prior report `docs/audits/2026-11-01-main-audio-restart-lifecycle-and-counter-persistence.md`.

## Verified findings
1. In the directly inspected `main.c` execution path, `init_audio()` precedes entering `syndicate()`. The function later calls `game_quit()`. This path does not show audio being reinitialized at each mission transition.
2. `game_quit()` calls `host_reset()`, then `LbBaseReset()`, then `exit(0)`. The non-DOS `host_reset()` calls `FreeAudio()`; process exit follows. This normal quit sequence is not evidence of an in-process audio restart.
3. `LbBaseInitialise()` clears `lbIdleHandlers`, and `LbBaseReset()` clears that array again.
4. `LbRegisterIdleHandler(cb)` appends to the first null slot, or fails when full. There is no local deduplication check for the same callback pointer. The registered sound callback is `sound_update()`, installed by `sound_fake_timer_initialize()` during AIL startup.
5. Observed `FreeAudio()` / AIL shutdown paths do not independently remove `sound_update()` from the SDL idle table in these examined functions. During ordinary `game_quit()`, `LbBaseReset()` clears it before process exit.
6. An **in-process** audio shutdown and reinitialization without `LbBaseReset()` could accumulate duplicate sound handlers; this is conditional. No reachable normal gameplay path or runtime symptom establishing that event has been found in this limited call-path audit.
7. Similarly, shared OpenAL counters not reset on `OPENAL_shutdown()` are primarily a partial-failure/repeated-start concern until a reachable same-process restart or failing startup is demonstrated. Do not label it a proven mission-transition fault.

## Next validation
Trace the remaining `init_audio` and `FreeAudio` callers (including generated assembly/wrappers if necessary), and any menu-to-game transition calling `LbBaseInitialise` without exiting. Plan a disposable in-process restart/fault-injection harness if permitted. For explosion slowdown, focus separately on measured live effect collision/render/audio callback times.

## Verification
Direct GitHub read of named files. No game source modified, build run, runtime or Windows tests, performance measurement, branch merges, or changes to protected v0.7. Mission Builder and Graphics Audit unaffected.
