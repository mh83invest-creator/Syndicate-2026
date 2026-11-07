# Syndicate 2026 — Main Systems Audit: Audio Slot Boundary, Allocation and Teardown

Date: 2026-11-07 (Australia/Brisbane)
Branch: main — documentation only
Status: source-level investigation; no compiled/runtime verification.

## Scope

This closes the specific unresolved question in `docs/audits/2026-11-07-main-mixer-slot-reuse-and-muted-cleanup.md`: where `end_sample_id` is initialized, what default is configured and how the mixer slot range relates to the static array. It also traces local allocation and teardown guards. Earlier priority-queue, sound-bank and mute audits are predecessors; do not report them again as discoveries.

Current GitHub source inspected:
- `engine/src/main.c`: calls `init_audio()` during startup before invoking assembly `syndicate()`.
- `engine/src/sound.c`, blob `d2f5946d11e11cb123846ee5e54ee4065bf1f07d`: `audio_options_set_default` assigns `audOpts.MaxSamples=10`; `init_audio` invokes `InitAudio(&audOpts)` for non-DOS.
- `engine/bfsoundlib/src/bfaudio.c`, blob `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`: `InitAudio` copies `audOpts->MaxSamples` to global `MaxNumberOfSamples`, then invokes `InitSound`, `InitMusic`, and timer initialization. `FreeAudio` invokes `FreeMusic` and `FreeSound`.
- `engine/bfsoundlib/src/init_snd.c`, blob `9a21b14297fe6ee886cdd71ad11cfbb69d147f8a`: `InitSound`, `InitAllSamples`, `DoFreeSound`, `FreeSound`.
- `engine/bfsoundlib/src/variable.c`, blob `f3e3cbf5e6fc2667a0b662261a674b463562b400`: global `sample_id[32]` and `end_sample_id`.
- `engine/bfsoundlib/src/ssampply.c`, blob `6fa62ba0b0ba83ad50b7c8b692deddeeff9a8ccc`: active-range scans, done-slot selection and `StopAllSamples`.

## CORE-SLOT-01 — Confirmed default effective scan range is 10 requested entries

The game's default `audOpts.MaxSamples` is 10. Native audio initialization assigns `MaxNumberOfSamples = audOpts->MaxSamples`. `InitAllSamples` explicitly executes `end_sample_id = &sample_id[MaxNumberOfSamples - 1]` and iterates inclusively from `sample_id[0]` to that pointer, calling `AIL_allocate_sample_handle(SoundDriver)` for each record. On this normal default path it requests ten handles (indices 0–9). `sample_id` has **32 static elements**, but the default scanning range is **not** all 32.

`InitSound` also sets `DIG_MIXER_CHANNELS` preference to `MaxNumberOfSamples + 5` (15 with default input). This is a requested mixer preference, **not proof of 15 concurrently usable game sample handles**. No real mixer/driver success measurement was performed.

## CORE-SLOT-02 — No local bounds validation in InitAllSamples

`InitAllSamples` directly computes `MaxNumberOfSamples - 1` to form an array address and loops to that address without an observed local zero/min/max bound check. Passing a value above 32 can create out-of-range access; zero leads to an invalid pre-array pointer expression and cannot be treated as a safe empty range. The actual default is ten; do not claim these invalid configurations are reachable from shipped UI or normal startup. Any future runtime override/configuration must validate `1 <= MaxNumberOfSamples <= 32` before pointer calculation.

## CORE-SLOT-03 — Individual handle allocation results unchecked

Within `InitAllSamples`, each result from `AIL_allocate_sample_handle` is assigned to `smpinfo->SampleHandle` and only `FadeState`/`UserFlag` are initialized in the shown loop. There is no per-handle null/error check or rollback in this function. `InitSound` subsequently assigns `SoundInstalled=1` after `InitAllSamples`. A partial allocation failure could therefore leave a configured range containing unusable handles if the underlying library reports such failure; that specific runtime condition is **not verified**. Inspect the active AIL implementation's failure return and teardown contract before prescribing a fix.

## CORE-SLOT-04 — Teardown includes a SoundActive-dependent stop gate

`FreeAudio` invokes `FreeSound`. `FreeSound` returns early when `SoundInstalled` is false, otherwise invokes `DoFreeSound`. `DoFreeSound` calls `StopAllSamples` before uninstalling `SoundDriver` and disabling `SoundAble`/`SoundActive`. As already established in the previous audit, `StopAllSamples` itself returns early if `SoundActive` is false. Hence stop/reset of tracked sample metadata and the separate callback queue is **conditional on active state** even during sound teardown. The driver uninstall remains present on the following path, so this is not proof that OS audio resources leak or continue playing after exit.

## CORE-SLOT-05 — Timer servicing is a separate potential work path

`InitAudio` calls `InitAllBullfrogSoundTimers` after `InitSound` and `InitMusic`. The `bfaudio.c` module also has `sound_update` which calls `AIL_API_timer` when `AILStartupAlreadyInitiated` and a `sound_fake_timer_initialize` registering it with `LbRegisterIdleHandler`. Direct demonstration that this registration is active in the target Windows build and its per-tick cost requires further call-site/build tracing; **do not** call it an observed explosion bottleneck.

## Verification, compatibility and next audit

Performed: resolved the formerly missing `end_sample_id` assignment; traced main startup to options, sound initialization, mixer-channel preference, slot creation, and teardown; identified conditional absence of local capacity and handle-allocation checks.

Not performed: compilation, Windows runtime, audio driver allocation failure injection, live sample count verification, measured AIL callback workload, full assembly reinspection, engine edits, asset changes, gameplay tests or other branch modifications.

Next source-only gate: inspect the active AIL sample-handle allocator and how `end_sample_id`/allocated handles are released or reset across FreeAudio and reinitialization; verify when/where idle-handler timer registration occurs.

Protected: v0.7, tactical 16 FPS, original audio behaviour, explosions and campaigns, saves, mouse/keyboard and independent Mission Builder / Graphics audit branches.
