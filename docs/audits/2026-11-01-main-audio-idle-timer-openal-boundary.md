# Syndicate 2026 — Main Systems Audit: audio idle timer and OpenAL boundary

Date: 2026-11-01 (Australia/Brisbane). Main branch. Documentation-only.

## Sources inspected on GitHub
- `engine/src/Makefile.am` — links `libbfsound.a`.
- `engine/src/sound.c` — direct C sound setup, priority and sample submission.
- `engine/bfsoundlib/Makefile.am` — sound library composition.
- `engine/bfsoundlib/src/bfaudio.c` — blob `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`.
- `engine/bfsoundlib/src/drv_oal.c` — blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`.
- `engine/bfsoundlib/src/ssampply.c` — blob `6fa62ba0b0ba83ad50b7c8b692deddeeff9a8ccc`.

## Source-confirmed findings
1. Default game audio options in `sound.c`: `MaxSamples = 10`, with sound and music enabled in `AbleFlags` and sound/music bank auto-load separately disabled. This is a configuration upper-bound parameter, **not** observed concurrent playback or a measured workload.
2. `sound_bank_setup` checks `smptable != NULL` and `GetSoundAble()` prior to preparing sample entries. Existing partial-load hazards in other sample consumers remain relevant.
3. `BFPlaySample` and `BFSonundUnkn1` execute game-side sample selection/submission using `PlaySampleFromAddress` on non-`WITH_AIL2` builds. `BFSonundUnkn1` walks up to 255 sample-status IDs and selects a candidate; this is a *source-level candidate cost* until call frequency is traced.
4. `bfaudio.c` defines `sound_update()` calling `AIL_API_timer()` only when `AILStartupAlreadyInitiated` is true. `sound_fake_timer_initialize()` registers `sound_update` with `LbRegisterIdleHandler`. Registering the handler does not by itself establish the thread, actual call frequency, or exact tactical-loop timing; those require reading the idle-handler dispatcher and its callers.
5. The native backend includes `drv_oal.c` with OpenAL device/context setup, sources, buffers and `alSourcePlay` APIs. This rules out presuming that the native audio path is a single SDL audio callback based solely on display SDL2 usage. Detailed selected-backend conditions and mixer execution threading require further verification.
6. `engine/src/game.c` uses SDL for event handling and frame pacing separately. Do not conflate OpenAL playback with SDL screen presentation or derive performance impact without profiling.

## Profiling/testing contract
On an agreed disposable Windows diagnostic build, sample per-tick `BFSonundUnkn1` calls/time, statuses visited, sample submissions, live voices, `AIL_API_timer` calls/time, OpenAL queue/submit paths, and frame-time distribution. Compare idle, gunfight, single and stacked explosions at same 16 FPS semantics, with sound enabled/disabled and identical mission input. Quantify instrumentation overhead. Keep audio and renderer phase metrics separate. Include missing/partial sound-bank failure scenarios **only** in disposable tests.

## Status and change control
Direct GitHub source inspection: yes. Audio callback thread/dispatcher end-to-end: **not established**. Actual concurrency, explosion-triggered cost, crash or slowdown: **not measured**. No source modifications, builds, playback experiments or gameplay tests. Preserve protected v0.7, original campaigns, 16 FPS gameplay, original effects and sounds, mouse/keyboard, and between-mission-only campaign save isolation. Mission Builder and Graphics Audit branches untouched.

Next: locate the actual `LbRegisterIdleHandler` execution path and `AIL_API_timer` implementation; then inspect source/voice reclamation under bursts. Do not turn hypotheses into measured findings.
