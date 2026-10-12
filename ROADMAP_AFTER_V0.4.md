# Syndicate 2026 — Roadmap after v0.4 camera validation

## Working baseline
- v0.4 experimental camera edge normalization: user confirmed scrolling correctly in every direction, sound and music work.
- Keep v0.3 as fallback. Preserve v0.4 engine and confirmed native Windows audio linkage.
- Never bundle commercial game data into the public GitHub repository.

## Revised priority order (12 October 2026)
1. **Camera edge scrolling** — fixed in v0.4; maintain regression checks.
2. **Smooth rendering and menu responsiveness** — investigate menu frame pacing and raise visual refresh without speeding mission simulation, animation/game clock, or input.
3. **Camera scroll speed option** — adjustable slider, default matching v0.4, persistent across sessions, independent of simulation rate.
4. **Graphics improvements** — progressive, verified changes after stable rendering.
5. **Intro cinematic restoration** — deprioritised by request; intro currently not playing.
6. **Heavy explosion/combat slowdown** — deprioritised by request, retain as tracked performance issue.

## Menu FPS source finding
SyndicatFX release 0.0.6.1022 `src/game.c` contains `game_update_full(bool wait)` with `max_fps = 16`; `game_update()` calls it with true. This is a shared loop throttle, **not** evidence that simply changing 16 to 60 is safe. Establish which call sites represent menus versus missions before modifying engine timing; do not conflate visual presentation with the simulation tick.

## Next implementation gate
- Trace menu, mission, render and event-processing update call graph.
- Add isolated *menu-only* timing experiment when context can be identified reliably.
- Require native 32-bit Windows build and non-MSYS audio DLL checks on any test package.
- Verify navigation, music, controls, menu animations, mission clock and scrolling do not run faster.
- Only afterward attempt simulation/render decoupling for 60 FPS mission presentation.
