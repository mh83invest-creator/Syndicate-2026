# Syndicate 2026 — Main Systems Audit: OpenAL shared pool accounting and initialization failures
Date: 2026-11-01 (Australia/Brisbane). Main branch. Documentation only.

## Evidence, direct GitHub reads
- `engine/bfsoundlib/src/drv_oal.c` blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`, lines 36–46, 85–95, 130–180.
- `engine/bfsoundlib/src/mssdig.c` blob `46654245790525135bab6aa7f9e029577237c52d`, lines 204–280 and 330–359.
- `engine/bfsoundlib/src/mssxmidi.c` blob `9ec9b43bffd031de08d80ab0ab05d192514348ef`, lines 420–448 and 645–675.
- Predecessor report: `docs/audits/2026-11-01-main-openal-music-initialization-shutdown.md`.

## Findings
1. `drv_oal.c` maintains one shared `sound_free_buffers` array with separate `sound_total_buffer_count` and `sound_free_buffer_count`. `SOUND_BUFFERS_PER_SRC=3` and the compiled array expects up to 64 digital plus three sequence sources, but this is a capacity assumption, not observed concurrency.
2. `OPENAL_create_buffers(n_sources)` computes `n_sources * 3`, asserts that adding it to the tracked total fits, calls `alGenBuffers`, then increases BOTH counters only when `check_al` reports success. It returns 0 on error. This function itself does not repair driver ownership decisions after its failed result.
3. Digital driver initialization (`mssdig.c` ~220) calls `OPENAL_create_buffers(AIL_preference[DIG_MIXER_CHANNELS])` without checking that return; then configuration/build-buffer/sample-structure failures can return after freeing their C/driver allocations but without a local matching `OPENAL_free_buffers` call.
4. Music initialization (`mssxmidi.c` ~656) similarly does not locally check `OPENAL_create_buffers(AIL_preference[MDI_SEQUENCES])`; its sequence-structure allocation failure returns without a corresponding local pool free.
5. Normal digital and music driver destructors attempt to call `OPENAL_free_buffers` using their current `n_samples` / `n_sequences` values. These counts may be truncated during failed OpenAL source generation; distinguish requested buffer count from current source count when reconciling pool state.
6. `OPENAL_free_buffers` computes the requested count, asserts it does not exceed `sound_total_buffer_count`, reduces the deletion count to `sound_free_buffer_count` if fewer are free, subtracts the effective deletion count from both counters, calls `alDeleteBuffers` and checks the error but returns 1 regardless. Its local counters therefore represent the effective deletion request rather than a validated count successfully deleted by OpenAL.
7. `OPENAL_shutdown` destroys current context and closes device, but the inspected routine does not reset the two static pool counters. Whether application startup/shutdown can repeat inside one process, and effects of a device reset on global state, remain unverified.

## Risks and interpretation
**Source-supported conditional risk:** Requested-versus-created source counts, unchecked pool creation and missing local cleanup on early exits can leave the shared counter/ownership model inconsistent. Actual exhaustion, leak, assertion, crash and explosion slowdown have not been reproduced. Claims about repeated startups must await lifecycle reachability tests.

## Diagnostic contract / next stage
Read startup/shutdown entrypoints and all `AIL_startup` ownership before assuming repeated restart. In a disposable build: instrument requested and effective buffer counts per driver, every alGen/alDelete result, outstanding queue IDs, pool total/free counters at each return and teardown, repeated startup/shutdown. Inject failure after buffer creation and after partial source creation. Preserve original audio, tactical cadence, protected v0.7 and campaign saves; coordinate graphics separately.

## Verification
Source inspection only. No source changes, compilation, runtime OpenAL fault injection, profiling, or gameplay tests. Parallel Graphics and Mission Builder branches untouched.
