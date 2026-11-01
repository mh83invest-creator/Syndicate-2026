# Main systems display audit — cross-workstream handover

Date: 2026-11-01

Report: `docs/audits/2026-11-01-main-display-pipeline-scaling-and-surface-guards.md` (commit `9becca40edc8146867d500f92393f487420deb88`).

Main engine audit verified the C display conversion, buffer copy, optional software scaling and SDL window update paths. It identified a conditional surface-lock failure guard concern, not a reproduced crash. No performance cause was established without profiling. Graphics Audit retains palette, visuals and scaling ownership; main audit owns engine timing and stability. Mission Builder is unaffected. Do not alter the 16 FPS tactical baseline, original effects, campaigns or between-mission save rules.

Next checks: SDL surface lock and restoration contracts, audio/render timing, disposable Windows instrumentation. No source changes or runtime tests.

The central `docs/SHARED_DEVELOPMENT_HANDOVER.md` update was attempted but blocked by a tool safety check; it remains unchanged in this step. This addendum is the confirmed GitHub handover for the audit, not a claim of central-file update.

## SDL recovery follow-on — 2026-11-01
The main audit traced the `LbScreenSwap` return contract and identified unchecked returns from window-surface restoration and draw-surface consistency checks. `LbScreenLock` has a direct failure return, while non-WSCREEN_CONTROL consistency checking can continue after SDL lock failure. Static source-only risks; no reproduced crash or performance diagnosis. Report: `docs/audits/2026-11-01-main-sdl-surface-recovery-return-contracts.md` (commit `9bdc249566bf1c2d166b3e26af3eceb7e1150e00`). No code, save or parallel-branch changes. Central shared handover remains unchanged by this follow-on.

## Audio scheduling checkpoint — 2026-11-01
Main audit inspected `engine/src/sound.c`, `engine/bfsoundlib/src/bfaudio.c`, `drv_oal.c`, `ssampply.c`, and build library manifest. Audio idle timer `sound_update()` calls `AIL_API_timer()` when initialized and can be registered through `LbRegisterIdleHandler`; native sound library includes OpenAL output. Thread/dispatcher call reachability and runtime performance not yet verified. Report: `docs/audits/2026-11-01-main-audio-idle-timer-openal-boundary.md`, commit `5f3f0d432053a87cd086b31c1d7233dbdee06259`. No production code changes or tests; graphics/mission branches untouched.

## Audio idle dispatcher and AIL callback catch-up — 2026-11-01
Main audit traced `game_update_full` → `game_handle_sdl_events` → `LbWindowsControl` → registered `sound_update` → `AIL_API_timer`. SDL events (up to 256 per pump) precede idle-handler calls. `AIL_API_timer` services running timers with elapsed-time accounting capped at 100 ms per call and may dispatch several callbacks when periods accumulate. This is source-confirmed scheduling, not measured explosion slowdown. Full report: `docs/audits/2026-11-01-main-audio-idle-dispatch-and-timer-catchup.md`, commit `7669f778dc4b6ee645a2fd847197f0b6ff28a276`. No production edits or gameplay tests. Next: timer callback registrations and bodies.

## Main Systems — audio timer callback workload (2026-11-01)
Direct source reads found digital `SS_serve` registered with initial `DIG_SERVICE_RATE=200` and music `XMI_serve` registered with initial `MDI_SERVICE_RATE=120`; these values are initial preferences, not measured invocation rates. Non-DOS `SS_serve` visits playing samples and calls OpenAL digital updates; `XMI_serve` visits active music sequences and updates OpenAL music buffers. Catch-up processing remains a source-level candidate, not a measured explosion slowdown. Audit `docs/audits/2026-11-01-main-audio-timer-callback-workload.md` committed as `2156616c604694a735d150b2f3213378599252a6`. Main branch documentation-only. Next: OpenAL queue/reclamation and per-callback cost. No code, graphics or Mission Builder changes; runtime tests outstanding.

## Main Systems — OpenAL voice/queue lifecycle (2026-11-01)
Audit inspected `drv_oal.c`, `mssdig.c`, and `ssampply.c`. Source creation is bounded by sample slots and can reduce `n_samples` on OpenAL creation error; callback unqueues processed buffers and updates playing samples through per-source bounded queue work. Error-handling and resource-lifecycle questions remain conditional, not reproduced. Report `docs/audits/2026-11-01-main-openal-digital-voice-queue-lifecycle.md`, audit commit `65f006410d0b1511d52418575072970f20f834ed`. Next: free-buffer ownership, queue error exits, sample allocation/reclamation. No baseline or workstream code changed; no Windows testing.

## Main Systems — OpenAL buffer ownership/error exits (2026-11-01)
Main source inspection of `engine/bfsoundlib/src/drv_oal.c` confirmed a conditional free-pool ownership hazard: after successful `alSourceQueueBuffers`, subsequent OpenAL operation failures may enter an error label that returns the still-submitted buffer ID to the free pool. No failure was reproduced. Free pool assertions and partial free-buffer destruction are additional test gates. See `docs/audits/2026-11-01-main-openal-buffer-ownership-error-exits.md`, commit `783c8f87949bbd647682a19b80fc278be4717a26`. Documentation-only; protected v0.7 baseline, Graphics and Mission Builder unchanged. Next: sequence buffer ownership and init/free ordering; Windows fault-injection outstanding.
