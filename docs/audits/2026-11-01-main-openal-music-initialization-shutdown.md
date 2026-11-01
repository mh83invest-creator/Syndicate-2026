# Syndicate 2026 — Main Systems Audit: music OpenAL initialization and shutdown
Date: 2026-11-01 (Australia/Brisbane). Workstream MAIN / main branch. Documentation only.

## Verified current GitHub sources
- `engine/bfsoundlib/src/mssxmidi.c` blob `9ec9b43bffd031de08d80ab0ab05d192514348ef`: `XMI_destroy_MDI_driver` ~420–448; initialization ~645–747.
- `engine/bfsoundlib/src/drv_oal.c` blob `f47aeab66d9b628a6dbb40b44134e93653c8c76a`: source create ~221–242; pool create/free ~138–180; shutdown ~130–136.
- `engine/bfsoundlib/src/ail.c` blob `bc54f7ff1abe19ca90de441f861ecbf0647a7600`: `AIL2OAL_API_shutdown` ~137–158.
- Prior audit `docs/audits/2026-11-01-main-music-queue-and-digital-shutdown-order.md`.

## Findings
1. Normal music driver destruction ends sequences, unlinks the driver head, releases its service timer, frees OpenAL sequence sources and pooled buffers, conditionally shuts down WildMidi, and releases sequence/driver allocations. This is the **observed order**, not proof of complete cleanup under every failure scenario.
2. Global AIL shutdown first uninstalls registered drivers, then releases all timers, shuts down AILA and calls `OPENAL_shutdown`. The final routine destroys the current OpenAL context and closes the device. Verify device/context failure handling and repeated startup/shutdown separately.
3. During music driver initialization, `OPENAL_create_buffers(AIL_preference[MDI_SEQUENCES])` is called **without a local success check**, before the sequence allocation. If `AIL_MEM_alloc_lock` for sequence structures fails, its visible error branch shuts down the device driver and frees the driver memory, **without a local corresponding OpenAL free-buffer call**. Whether the buffers survive until later global shutdown is not tested. This is a conditional partial-initialization ownership concern.
4. `OPENAL_create_sources_for_sequences(mdidrv)` also has **no locally checked return**. Its implementation reduces `mdidrv->n_sequences` to the count already created if `alGenSources` fails, and returns whether at least one source exists. On a zero-source outcome, subsequent code is still visible in the initialization function; exact downstream failure reachability requires testing.
5. The prior digital driver audit similarly noted unchecked source creation but has distinct initialization and teardown paths. Avoid claiming identical outcomes without following each rollback branch.
6. The previous comparison found music queue resets its temporary `buf` variable to zero after successful `alSourceQueueBuffers`, unlike digital queue. This is a **specific** ownership distinction, not a guarantee of all-path cleanup.

## Next verification
Map exact buffer pool creation ownership across digital/music drivers and each early-return branch; inspect timer registration failure after successful source creation, global AIL teardown state resets and options governing WildMidi. Use disposable Windows fault-injection tests only after approval. Verify pool count, allocated source ID, active buffers and teardown error returns. Preserve original 16 FPS tactical simulation, audio, explosion visuals/damage and protected v0.7. Mission Builder and Graphics Audit remain independent.

## Status
Source review only. No modifications of game source, branches, assets or save paths. No compilation, runtime testing, crash reproduction, OpenAL failure injection or profiling.
