# Syndicate 2026 — Main Systems Audit: audio restart and counter persistence
Date: 2026-11-01 (Australia/Brisbane). Workstream: Main Systems. Documentation only.

## Source read in this checkpoint
- `engine/bfsoundlib/src/bfaudio.c` blob `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`: `EnsureAILStartup`, `FreeAudio`.
- `engine/bfsoundlib/src/init_snd.c`: `DoFreeSound` ~260–280, `InitSound` service preferences ~390–400, `FreeSound` ~455–464.
- `engine/bfsoundlib/src/init_mus.c`: `DoFreeMusic` ~140–175, `InitMusic` and `FreeMusic` ~366–430.
- `engine/bfsoundlib/src/ail.c` blob `bc54f7ff1abe19ca90de441f861ecbf0647a7600`: `AIL2OAL_API_startup` and `AIL2OAL_API_shutdown` ~97–157.
- `engine/bfsoundlib/src/drv_oal.c` blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`: `OPENAL_startup`, `OPENAL_shutdown`, shared pool counters.
- `engine/src/game.c`: `game_quit` and `host_reset` ~108–112, ~210–222.

## Source-supported findings
1. `EnsureAILStartup` checks `AILStartupAlreadyInitiated`, invokes `AIL_startup` only when false, and sets the flag true. `sound_update` only enters `AIL_API_timer` if this flag is true.
2. `DoFreeSound` uninstalls the digital driver then may set `AILStartupAlreadyInitiated=false` and call `AIL_shutdown` if `!MusicInstalled` or `(!SoundInstalled && !MusicAble)`. `DoFreeMusic` uninstalls the music driver and calls AIL shutdown if `!SoundInstalled`. Both contain separate state-dependent cleanup logic.
3. `FreeAudio` invokes `FreeMusic` then `FreeSound` (after optional CD/stream cleanup). Each wrapper may return early if its `*Installed` flag is false. This means every partial-install/failure state must be checked before claiming all paths shut down exactly once.
4. `AIL2OAL_API_startup` initializes defaults and timers, calls `OPENAL_startup`, registers the sound idle handler and returns the OpenAL startup result. The immediate source does not check `sound_fake_timer_initialize` success before returning.
5. `AIL2OAL_API_shutdown` uninstalls registered drivers, releases timers, shuts down AILA and then OpenAL. `OPENAL_shutdown` destroys current context and closes the device without locally resetting `sound_total_buffer_count` or `sound_free_buffer_count`, which are statically initialized in `drv_oal.c`.
6. A repeated *in-process* startup after partial cleanup could thus reuse nonzero pool counts. This is a conditional state-risk hypothesis, not a reproduced crash or demonstrated application workflow.
7. Game `game_quit` calls `host_reset` and then `exit(0)`. This observed quit path ends the process, so it does not by itself prove an in-process audio restart across missions. Reachability of other restart paths remains unverified.
8. Important numerical clarification: `sound.c` provides default `MaxSamples=10`, but `init_snd.c` subsequently uses `DIG_MIXER_CHANNELS = MaxNumberOfSamples + 5`. Do not confuse default configured sample count with actual digital driver voice slots or active voices.

## Scope, risk and next stage
Static issue: preservation of shared pool invariants across failed initialization/partial shutdown; possible duplicate idle handler registration if an in-process restart occurs without clearing the handler list. No runtime manifestation established. Verify registration/reset lifetime and caller reachability before calling it a bug.

Next: inspect exact init/free caller paths and idle handler reset/registration consistency, then specify disposable Windows restart/failure test matrix. Audit must remain separate from the Graphics and Mission Builder workstreams, protect v0.7, 16 FPS tactical timing, explosion effects and audio, campaign isolation and between-mission saves.

## Verification
Completed: direct current GitHub source reads and cross-file control-flow tracing. Not completed: Windows build, executable test, profile, injected failures, proof of in-process reinit, or source changes. No production code modified.
