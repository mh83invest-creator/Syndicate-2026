# Syndicate 2026 — Main Systems Audit: OpenAL digital voice and buffer lifecycle

Date: 2026-11-01 (Australia/Brisbane). Scope: main systems audit; documentation only.

## Sources directly inspected
`engine/bfsoundlib/src/drv_oal.c` Git blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`; `engine/bfsoundlib/src/mssdig.c` blob `46654245790525135bab6aa7f9e029577237c52d`; `engine/bfsoundlib/src/ssampply.c` blob `6fa62ba0b0ba83ad50b7c8b692deddeeff9a8ccc`. Previous callback audit: `docs/audits/2026-11-01-main-audio-timer-callback-workload.md`.

## Source-confirmed facts
1. `OPENAL_create_sources_for_samples` (drv_oal.c 183–202) iterates `digdrv->n_samples`, creates one OpenAL source per sample slot, records source and zero buffer-use count. On `alGenSources` error it sets `digdrv->n_samples = i` and stops; function returns whether any sources exist. `mssdig.c` calls this before registering the `SS_serve` timer. Its return is not checked locally in the inspected call.
2. `OPENAL_free_sources_for_samples` (204–219) deletes each recorded OpenAL source during driver cleanup and writes the same source value back to its system-data slot. Do not assert that it clears stale handles.
3. `OPENAL_unqueue_source_buffers` (259–283) queries `AL_BUFFERS_PROCESSED`, unqueues up to that count and invokes a supplied reclamation callback on successful unqueue. Failed unqueue attempts continue without calling the reclamation callback. No leak reproduced.
4. `OPENAL_unqueue_finished_dig_samples` (670–694) scans `digdrv->n_samples`, skips `SNDSMP_FREE`, and calls `unqueue_dig_sample_buffers` for other slots. This happens in the non-DOS `SS_serve` path.
5. `OPENAL_update_dig_sample` (696–700) calls `queue_dig_sample_buffers`. That helper (396 onwards) returns when `buffers_used >= SOUND_BUFFERS_PER_SRC`, and its loop is bounded by remaining sample data and `SOUND_BUFFERS_PER_SRC`; each iteration obtains a free buffer and calls `alBufferData`, `alSourceQueueBuffers`, with further source-state/volume adjustments. This is source-backed per-active-sample work, not a measured bottleneck.
6. `SS_serve` (mssdig.c 393 onwards) uses a reentry guard, unqueues finished non-DOS buffers, scans allocated sample slots for playing samples and calls `OPENAL_update_dig_sample`. Work is thus shaped by allocated slots, playing samples, processed buffers and remaining data. Source call counts alone cannot be treated as runtime measurements.

## Risk classification
- **Performance candidate:** repeated timer servicing after a slow frame may cause queued and reclaimed buffer work; its real wall-time contribution and concurrent sample count remain unknown.
- **Failure/cleanup candidates:** source creation may truncate `n_samples`; errors during buffer queue/unqueue need isolated testing. The observed local call does not check the creation return. No Windows failure or resource leak established.
- **Preservation:** maintain original audible effects, sound priorities, gameplay timing, explosions, campaigns and save rules. Do not change source allocations or callback cadence from static evidence alone.

## Next
Inspect `pop_free_buffer`, `push_free_buffer`, `unqueue_dig_sample_buffers`, `queue_dig_sample_buffers` error exits, and sample handle acquisition/recycling. Establish ownership and failure invariants; profile separately in an agreed disposable Windows diagnostic build.

## Verification
Direct current GitHub C-source inspection only. No source edits, compilation, live playback, CPU profiling, synthetic OpenAL failure tests, gameplay regression or parallel-branch changes.
