# MAIN handover — holistic engine audit phase

2026-11-01. Main Systems Audit. Scope explicitly prioritises **understanding the whole game before modifying it**.

Report: `docs/audits/2026-11-01-main-holistic-startup-mission-frame-lifecycle.md`, commit `3646b2f91ac492a21260d58cb47a7666e778a598`.

Verified from `main.c`, `game.c`, `display.c`, `sound.c`, Makefile, and actual `syndre.sx` Git blob: C startup calls resources/UI/input/audio initialization then assembly `syndicate`; assembly controls selection → mission preparation → conditional tactical simulation → draw/event/pacing → mission cleanup and progression → selection/quit. Tactical `move_it` and screen work precede C `game_update` pacing. Mission exit has alternative paths (free level and return to selection, or transfer people then `level_finished`). These branches need comprehensive persistence/lifecycle review.

**Next:** trace exact assembly entry/exit, data ownership and error paths for selection, `reset_mission_info`, `initialise_player`, `free_map_level`, `transfer_people_into_player` and `level_finished` before moving to input, rendering, audio, campaign saves, research/economy and performance relationships. Keep a holistic subsystem/dependency map and list unknowns explicitly.

No engine, CI, baseline, Graphics or Mission Builder changes; no runtime tests or builds. Prior diagnostics-only proposals remain proposals, not approval to modify the game.
