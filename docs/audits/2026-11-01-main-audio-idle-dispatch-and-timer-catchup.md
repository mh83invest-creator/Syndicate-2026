# Syndicate 2026 — Main Systems Audit: audio idle dispatch and timer catch-up

Date: 2026-11-01 (Australia/Brisbane)
Branch: main. Documentation-only. No implementation or runtime testing.

## Independently read GitHub sources
- `engine/src/game.c`: `game_handle_sdl_events` lines 97–103 and `game_update_full` lines 141–196.
- `engine/bflibrary/src/x86-win-sdl2/swindows.c` blob `e1a1114e3fcbf5e0231e2ff3067b0024c7bd84ea`, `LbWindowsControl` and `LbRegisterIdleHandler` lines 172–209.
- `engine/bfsoundlib/src/bfaudio.c`: `sound_update` and `sound_fake_timer_initialize` lines 109–119.
- `engine/bfsoundlib/src/ail.c`: `AIL2OAL_API_startup` calls `OPENAL_startup`, then `sound_fake_timer_initialize` around lines 101–137.
- `engine/bfsoundlib/src/aila.c` blob `ca2b9574d2f1b1fe2e2e2097b2bc9af6c6368e80`: `AIL_API_timer` lines 307–370.

## Confirmed call chain
1. `game_update_full()` calls `game_handle_sdl_events()` once before measuring the elapsed interval, plus once per actual sleep-loop iteration.
2. `game_handle_sdl_events()` calls `LbWindowsControl()`.
3. `LbWindowsControl()` drains at most 256 SDL events in one invocation, then iterates registered idle handlers until a null entry or the fixed array limit.
4. `AIL2OAL_API_startup()` invokes `sound_fake_timer_initialize()`, which registers `sound_update()` via `LbRegisterIdleHandler()`. It does not visibly check the registration result itself.
5. `sound_update()` calls `AIL_API_timer()` when `AILStartupAlreadyInitiated` is true.
6. `AIL_API_timer()` reads millisecond time, guards against locked or reentrant calls, and for running timers accumulates elapsed microseconds. After the initial call, elapsed **milliseconds are capped at 100** per invocation before conversion to microseconds. It loops while accumulator exceeds each timer period, queues triggers and dispatches callbacks until triggers clear or the timer ceases running. This can result in multiple callback invocations per service call; exact counts and costs depend on registered periods and timer status.

## Interpretation and limitations
- The current reviewed path is an **event-pump-serviced AIL timer**, not evidence of a native SDL audio callback. OpenAL is the native audio output backend.
- Irregular or late event servicing can lead to catch-up work, but neither meaningful explosion-triggered callback load nor measurable slowdown has been demonstrated.
- More event-pump iterations can service the timer more often within the same frame. This should not be misreported as a guaranteed one-timer-call-per-frame model.
- Capping a single elapsed contribution at 100 ms does **not** guarantee a callback count of one or prove perfect time accounting after long stalls. Examine timer callback registration and period values.
- The above is source evidence, not gameplay observation. The main audit must preserve sound effects, 16 FPS pacing, audio synchronization, and protected campaigns.

## Next audit
Inspect `AIL_register_timer` / start-timer consumers and callback bodies, prioritizing mixer/sequence processing; distinguish event-thread timer callbacks from OpenAL driver playback. For a future **disposable Windows** diagnostic build, measure per-pump handler count, per-timer accumulated delay/trigger count, callback duration, event duration and missed-tick distribution, alongside effect/collision/display timings. No instrumentation approved or implemented.

## Verification and change control
GitHub reads performed for all paths above. No source or binaries edited; no compilation, playback, profiling, or tests. Graphics Audit and Mission Builder unaffected; baseline and save semantics unchanged.
