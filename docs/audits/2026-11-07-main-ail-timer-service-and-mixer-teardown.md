# Syndicate 2026 — Main Systems Audit: AIL Timer Catch-Up and Digital Mixer Teardown

Date: 2026-11-07 (Australia/Brisbane)
Branch: main. Documentation-only continuation of `2026-11-07-main-audio-slot-boundary-initialisation.md`.
Status: current-source tracing; **no Windows runtime profile** or engine changes.

## Source files inspected
- `engine/bfsoundlib/src/aila.c` (Git blob `ca2b9574d2f1b1fe2e2e2097b2bc9af6c6368e80`): `AIL_API_timer` lines 307–366, timer registration and release at 278–305.
- `engine/bfsoundlib/src/ail.c` (Git blob `bc54f7ff1abe19ca90de441f861ecbf0647a7600`): AIL/OpenAL startup, idle audio registration and shutdown.
- `engine/bfsoundlib/src/bfaudio.c` (Git blob `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`): `EnsureAILStartup`, `sound_update`, `sound_fake_timer_initialize`.
- `engine/bfsoundlib/src/mssdig.c` (Git blob `46654245790525135bab6aa7f9e029577237c52d`): driver mixer resources, `SS_serve`, installation timer and teardown.
- `engine/bfsoundlib/src/drv_oal.c` (Git blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`): OpenAL source and buffer work.
- `engine/bfsoundlib/src/aildebug.c`: debug wrappers for `AIL_allocate_sample_handle` and `AIL_release_sample_handle`.
- `engine/bfsoundlib/src/init_snd.c` (Git blob `9a21b14297fe6ee886cdd71ad11cfbb69d147f8a`): ten requested sample handles and driver uninstall.
- `engine/bfsoundlib/src/sndtimer.c` (Git blob `168bb3b598d6834384723d58dc036197d76d9de0`): Bullfrog-specific timer slot helper.

## CORE-AIL-01 — Idle callback routes to AIL timer after successful startup

`AIL2OAL_API_startup` in `ail.c` runs OpenAL startup and invokes `sound_fake_timer_initialize`, which calls `LbRegisterIdleHandler(sound_update)` in `bfaudio.c`. The registered `sound_update` calls `AIL_API_timer` when `AILStartupAlreadyInitiated` is true. `EnsureAILStartup` sets that flag following `AIL_startup` on its path. The exact frequency of the Bullfrog idle handler and whether the registration succeeds in the published Windows executable remain unmeasured.

This connects the audio timer machinery to the application's idle processing **at source level**; exact call site and frequency of the idle-handler dispatcher should be verified before attributing per-frame time.

## CORE-AIL-02 — Driver requests 200 Hz timer servicing

`InitSound` in `init_snd.c` sets `DIG_SERVICE_RATE=200`. In `mssdig.c` sound driver setup registers `SS_serve`, sets that timer to `AIL_preference[DIG_SERVICE_RATE]`, then starts it. Nominal timer period is 1/200 s = 5 milliseconds. Actual callback execution cadence may differ with idle-handler frequency and OS scheduling.

On the active OpenAL branch, `SS_serve` unqueues completed OpenAL buffers and visits physical digital sample structures to update active samples; the work is **not** identical to the game's single-winner 256-entry `BFSonundUnkn1` queue.

## CORE-AIL-03 — Delayed updates permit callback catch-up bursts

`AIL_API_timer` reads `AIL_ms_count`, rejects calls while locked/reentering, then computes elapsed time since last update. On subsequent updates it caps `diff` to 100 ms before converting to microseconds. For each running timer it adds the elapsed value to `timer_cb_elapsed_times[i]`, repeatedly subtracts the configured period while incrementing `timer_trigger[i]`, then invokes the callback repeatedly until its trigger count is exhausted or timer stops.

**Static implication:** with 5 ms service interval and a 100 ms credited gap, up to 20 nominal service periods could be newly credited in one dispatcher call (plus any accumulator remainder). This is *not* an observed 20 callbacks per tactical frame; other registered timer callbacks, accumulated residuals, and scheduler conditions must be measured. Unlike the tactical 16 FPS frame target, the audio callback operates on a separate time scale.

A frame delayed by expensive explosion effects could plausibly lead to audio-service catch-up work *after* the delay, creating a feedback cost. There is no measurement of whether that occurs; avoid assigning audio culpability before profiling both stages separately.

## CORE-AIL-04 — Driver teardown releases physical SNDSAMPLE structures

`DoFreeSound` calls `AIL_uninstall_DIG_driver` on `SoundDriver` after conditional `StopAllSamples`. `AIL2OAL_API_uninstall_DIG_driver` calls `AIL_uninstall_driver`, which executes the installed driver's destructor. The destructor in `mssdig.c` loops over the driver's physical `SNDSAMPLE` structures and calls `AIL_release_sample_handle` where status is not `SNDSMP_FREE`, then releases OpenAL sample sources/buffers and driver allocations. Thus the lower-level driver teardown provides a **separate physical resource release path** from game-level `StopAllSamples`, which may be skipped when muted. Do not conclude that every mute-dependent game cleanup omission causes leaked hardware channels.

The 10 entries in `sample_id[32]` are stored pointers to physical driver sample handles; the physical driver sets `n_samples=AIL_preference[DIG_MIXER_CHANNELS]`, which is requested as 15 in the default game initialization. This does not establish that all 15 were allocated successfully, or that all are game-available, or that the exact Windows runtime matches assumptions.

## CORE-AIL-05 — Bullfrog timer helper has reversed-looking free-slot test

`SetupBullfrogSoundTimer` in `sndtimer.c` loops up to five `sound_timer` entries and executes `if (stinf->used) break;` before assigning `stinf->handle=AIL_register_timer(cb)` and setting `used=1`. It therefore chooses an already-used entry when it finds one; if all five entries are zeroed (as `InitAllBullfrogSoundTimers` would normally leave them) it reaches `st>=5` and returns 0. This is **source-visible anomalous allocation logic** and may prevent use or overwrite a live entry if this helper is called. Actual call sites, compiled configuration and runtime use **not yet established**. `ReleaseBullfrogSoundTimer(st)` indexes `sound_timer[st-1]` without checking the input index locally; only `MusicInstalled`/`SoundInstalled` and per-entry used state are tested. This is a distinct error-path review subject, not evidence of explosion slowdown.

## Remaining verification gates
1. Locate actual `AIL2OAL_API_allocate_sample_handle` and release function bodies and confirm explicit failure return codes, reuse semantics and handling of null handles.
2. Trace `LbRegisterIdleHandler` invocation cadence, registered-handler lifetime and whether AIL startup/shutdown unregisters or guards callback after shutdown.
3. Check source call sites for `SetupBullfrogSoundTimer` and `ReleaseBullfrogSoundTimer`, including possible assembly wrappers; do not assert active runtime impact until traced.
4. In an approved disposable Windows diagnostic build, separately time `SS_serve`, `AIL_API_timer`, queued game sample arbitration, sprite/effect collision work, and window presentation. Compare with unchanged v0.7, preserving 16 FPS simulation and audio behaviour.
5. Verify 10 game pointers, 15 requested physical channels and real driver resource counts independently; preference requests are not observations.

## What was and was not verified

**Performed:** GitHub source reads, start-to-timer registration path, high-level and physical driver teardown cross-check, explicit callback arithmetic, anomaly identification in Bullfrog timer helper.

**Not performed:** actual Windows compilation, symbol/ABI verification, game or audio playback, instrumented profile, fault injection, original data testing, branch merges or source edits.

Preserve protected v0.7, mouse/keyboard, original and American Revolt campaigns, original explosion counts/damage/graphics/sounds, user saves, 16 FPS tactics, and independent Graphics/Mission Builder branches.
