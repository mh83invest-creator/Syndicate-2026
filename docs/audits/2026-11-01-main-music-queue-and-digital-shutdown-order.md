# Syndicate 2026 — Main Systems Audit: Music queue ownership and digital shutdown ordering

Date: 2026-11-01 (Australia/Brisbane). main branch. Documentation only.

## Direct source evidence
GitHub `engine/bfsoundlib/src/drv_oal.c` ~138–180, 396–486, 531–668 (blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`); `engine/bfsoundlib/src/mssdig.c` ~275–359; `engine/bfsoundlib/src/mssxmidi.c` ~712–748. Prior report `docs/audits/2026-11-01-main-openal-buffer-ownership-error-exits.md`.

## Confirmed
1. Music queue `queue_mdi_sequence_buffers` calls `pop_free_buffer`, `alBufferData`, and `alSourceQueueBuffers`. **After successful queue status**, it sets `buf = 0` before updating local `buffers_used` and handling gain/playback state. Its shared error handler returns a nonzero buffer to the free pool. Therefore the music path avoids *the particular later-error return of a successfully queued nonzero buffer* observed as a conditional risk in the digital queue.
2. The analogous digital `queue_dig_sample_buffers` increments its local `buffers_used` after queuing, but does not clear `buf` before subsequent calls that may fail. Its error label returns nonzero `buf`; compare the prior ownership report. This difference is source-confirmed, but an actual OpenAL failure/double-ownership case remains untested.
3. Music queueing is capped by `SOUND_BUFFERS_PER_SRC` and conditionally compiled `LBS_ENABLE_WILDMIDI`; in the disabled branch the queue's generation loop breaks. Do not generalise runtime sequence work without verifying the build configuration and active music backend.
4. Digital driver setup in `mssdig.c` initializes sample records, calls `OPENAL_create_sources_for_samples` before timer registration, and **does not locally check** the source-creation result. An early timer-registration failure frees driver allocations through its own path; verify resource ownership of any already created OpenAL sources in that failure case before making a leak claim.
5. Normal digital teardown calls `AIL_release_timer_handle`, releases non-free sample handles, deletes OpenAL sample sources, and calls `OPENAL_free_buffers(digdrv->n_samples)` before freeing sample driver memory. The free-buffer function may delete fewer than requested if fewer pooled buffers are free; it logs the difference in debug mode. This is a source-observed lifecycle caveat, not a demonstrated leak.
6. Music timer initialization allocates and registers `XMI_serve`, sets its timer user and service frequency, then starts the timer. Complete music driver teardown, partial failure recovery and pool release invariants remain separate checkpoints.

## Acceptance/test gates
In a disposable Windows diagnostic build, inject OpenAL failures before and after queue submission for digital and music paths; track buffer IDs with free/queued/processed states; confirm that no ID can be simultaneously free and queued, and that shutdown leaves expected pool/device state. Include repeated mission/menu transitions and audio enabled/disabled, preserving the original sound and 16 FPS simulation.

## Status
Completed source review only. No code edits, compilation, playback, failure injection, profiling, gameplay tests, branch merging or source fixes. Protected v0.7, Graphics Audit and Mission Builder untouched.

Next: verify source creation/buffer pool initialization and full music driver destruction, including failure rollback and `LBS_ENABLE_WILDMIDI` configuration.
