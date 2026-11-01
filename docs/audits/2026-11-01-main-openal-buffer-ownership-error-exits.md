# Syndicate 2026 — Main Systems Audit: OpenAL buffer ownership and error exits
Date: 2026-11-01 (Australia/Brisbane). Workstream: MAIN on main. Documentation-only.

## Verified source
Direct GitHub read `engine/bfsoundlib/src/drv_oal.c` blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`, lines 36–46, 85–95, 138–180, 259–283, 396–518, 625–648; compare prior voice lifecycle report.

## Findings and level of certainty

1. The static free pool contains `SOUND_MAX_BUFFERS = (64+3)*3` entries; `SOUND_BUFFERS_PER_SRC = 3`. The 64+3 is a source capacity expectation, not measured active sound count.
2. `pop_free_buffer` asserts a positive free count, then decrements it; `push_free_buffer` asserts the pool has capacity, then appends. Assertion-based constraints deserve release-build fault tests; there is no confirmed exhaustion event.
3. `queue_dig_sample_buffers` acquires a buffer, calls `alBufferData`, then `alSourceQueueBuffers` and increments local `buffers_used`. Several subsequent OpenAL calls (`alGetSourcei`, `alSourcef`, `alSource3f`, and potentially `alSourcePlay`) can jump to the shared `err:` label.
4. **Conditional ownership risk:** the `err:` label pushes `buf` back to the free pool if nonzero, without distinguishing failure **before** versus **after** successful queuing. The post-queue path does not reset `buf` to zero. On a later-call error, that buffer may be considered free even though it has been submitted to the source. This is source-evidenced control flow; actual OpenAL state and a failure reproduction remain unverified.
5. The music sequence queue similarly has a shared `err:` that pushes nonzero `buf` (around lines 625–648). Inspect its precise ownership transitions independently; do not claim identical reproduction without tracing.
6. `OPENAL_unqueue_source_buffers` invokes reclamation callback only after successful unqueue; on errors it continues without callback. `unqueue_dig_sample_buffers` reduces `buffers_used` by the number successfully removed. Resource reclamation policy is visible, but runtime leaks or double returns have not been demonstrated.
7. `OPENAL_free_buffers` bounds deletion to currently free buffers when fewer than expected are present and adjusts counters. The source can leave non-free buffers undeleted during this call; determine cleanup ordering and OpenAL device destruction semantics before labeling this an actual leak.

## Diagnostic requirements
On a disposable diagnostic Windows build only: track unique OpenAL buffer IDs with states FREE, ACQUIRED, QUEUED(source), PROCESSED, RETURNED; log each OpenAL operation and injected failure point before and after source submission; assert no ID appears both free and queued; test source cleanup and repeated combat transitions. Time normal queue/unqueue work separately from tactical effects and SDL presentation. Do not change voice counts, effects, damage, timing, or allocator without separately approved implementation and regression testing.

## Status
Source analysis completed; no source change, Windows compilation, OpenAL fault injection, runtime playback, benchmark, or gameplay testing. Graphics Audit and Mission Builder unchanged. Protected v0.7 baseline preserved.
Next: inspect full music queue transitions and free-pool creation/destruction sequencing; distinguish genuine resource-retention defects from intended device-owned cleanup.
