# Syndicate 2026 — Main Systems Audit: digital and music timer callback workload

Date: 2026-11-01 (Australia/Brisbane). Branch: main. Documentation-only.

## Source evidence
Direct live reads: `engine/bfsoundlib/src/mssdig.c` blob `46654245790525135bab6aa7f9e029577237c52d`; `engine/bfsoundlib/src/mssxmidi.c` blob `9ec9b43bffd031de08d80ab0ab05d192514348ef`; `engine/bfsoundlib/src/ail.c` blob `bc54f7ff1abe19ca90de441f861ecbf0647a7600`; prior `docs/audits/2026-11-01-main-audio-idle-dispatch-and-timer-catchup.md`.

## Confirmed callback registration
1. `mssdig.c` registers `SS_serve` via `AIL_register_timer`, associates `digdrv` as user data, configures frequency from `AIL_preference[DIG_SERVICE_RATE]`, then starts that timer (approximately lines 293–313).
2. `mssxmidi.c` registers `XMI_serve` for music sequencing, sets `AIL_preference[MDI_SERVICE_RATE]`, and starts the timer (approximately lines 725–746).
3. `ail.c` startup defaults set digital service rate to 200 and music service rate to 120. These are **initial preferences**, not measured runtime execution frequencies or proof that both services are active in a given scene.

## Callback body evidence
- `SS_serve` (`mssdig.c` ~393 onwards) has a re-entry guard and returns early when driver playback is inactive. On non-DOS paths it unqueues finished OpenAL digital sample buffers, scans `digdrv->n_samples` and invokes `OPENAL_update_dig_sample` for each sample with status `SNDSMP_PLAYING`. The DOS path uses software mixing instead. Thus non-DOS digital service is not simply a single audio callback or a per-frame no-op.
- `XMI_serve` (`mssxmidi.c` ~1330 onwards) returns if disabled or reentrant. The non-DOS path unqueues finished OpenAL music buffers, scans active sequences and invokes `OPENAL_update_mdi_sequence`. Other sequence bookkeeping follows.
- Previous evidence established that `AIL_API_timer` dispatches timer callbacks via the registered SDL event-pump idle handler and can catch up multiple elapsed periods in one call. The combination gives a plausible *source-level* burst-work path; measured contribution to explosion slowdown is UNKNOWN.

## Caveats
- OpenAL output and driver-internal scheduling must not be confused with caller-side event-pump timer servicing.
- Whether and how often both drivers are initialized, how many voices/sequences are playing, how much work each OpenAL update performs, and the magnitude of any frame delay remain unmeasured.
- Neither the exact aggregate callback count per frame nor audible defects are established by static source.

## Next verification
Trace `OPENAL_update_dig_sample`, queue/unqueue behavior, allocation limits and any bounded vs unbounded loops. Specify separate monotonic diagnostic counters for each timer callback invocation, active sample count, queue and OpenAL update latency, 16 FPS tactical frame duration, fireball/collision work, SDL present and intentional wait. Preserve all sound effects, original explosion visuals and damage, v0.7, classic campaigns, mouse/keyboard and between-mission-only save requirements.

## Work performed / not performed
Performed: read the current GitHub timer registration and callback source. Not performed: source modification, build, Windows gameplay, OpenAL instrumentation, benchmark, sound playback or branch merge. Mission Builder and Graphics Audit untouched.
