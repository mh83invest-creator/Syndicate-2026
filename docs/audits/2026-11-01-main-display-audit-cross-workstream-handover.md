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
