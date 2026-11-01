# Syndicate 2026 — Main Systems Audit: display pipeline, scaling, and surface guards

Date: 2026-11-01 (Australia/Brisbane)
Branch: main; scope: documentation only
Direct source: `engine/src/display.c` (blob `9c6ba018f31d67b394c5973d05d6e51824171b68`); `engine/bflibrary/src/x86-win-sdl2/sscreen.c` (blob `ed6b6768ef6ebdb770ec9512ed315f83f5424852`); `engine/src/game.c` (blob `9f56221d527075fba2b1f2406fab60af28a4289a`).

## Observed pipeline

- `update_vscreen_whole_vres16` clears `VScreen` (`640*480` bytes), then iterates four source planes. Each plane processes 400 rows of 80 bytes, depositing eight indexed pixels per byte into `VScreen`. This is deterministic conversion work, not dynamically proportional to the number of explosions, although screen presentation remains relevant to each displayed frame.
- `swap_wscreen` conditionally copies `VGABuffer` (Vres16) or `WScreen` (MCGA) into `lbDisplay.WScreen`, unlocks and calls `LbScreenSwap`, then conditionally reacquires its original lock.
- In SDL2 `LbScreenSwap`, surface restoration and draw-surface consistency checks precede mouse swap handling. If `lbHasSecondSurface` is set, it calls `LbI_SDL_BlitScaled` to the screen surface; successful paths then call `SDL_UpdateWindowSurface`. If no second surface exists, the scaled blit is skipped. The return status of the restoration/check functions is not inspected locally; inspect their contracts before claiming a failure.
- `LbI_SDL_BlitScaled` takes a direct `SDL_BlitSurface` path when the dimensions match and a regular `SDL_BlitScaled` path when non-indexed source pixels are encountered. Otherwise it locks the required surfaces, dispatches to custom 8-bpp or true-colour scaling, unlocks and returns 0.
- **Conditional guard concern:** If `SDL_LockSurface(src)` or `SDL_LockSurface(dst)` fails in the custom branch, `LOGERR` is emitted but execution continues to use the surface pixels and later calls unlock. Actual surface-lock failure, downstream invalid memory access, and crash have **not** been reproduced. This is a candidate error-path robustness issue, not a demonstrated normal-operation defect.
- `game_update_full` occurs as a separate frame pacing stage; its wait/SDL events may affect observed timing. Do not conflate conversion time, lock contention, optional scaling, `SDL_UpdateWindowSurface` time and frame wait.

## Risk classification and experimental checks

1. **Performance attribution: unproven.** Time per-stage conversion, `swap_wscreen` copy, SDL swap checks, conditional scaling and window update with a monotonic diagnostic clock; compare idle versus stacked explosions under identical settings. Do not optimise or change output sizes/palette conversions before profiling.
2. **Failure handling: source-observed candidate.** Use an isolated diagnostic build and instrument lock outcomes, surface pixel formats, widths/heights, pitches, destination bytes-per-pixel and the presence of a second surface. Verify lock/unlock contract and failure handling without injecting failures in the protected build.
3. **Renderer correctness: separate graphics audit.** Changes affecting indexed palette conversion, output pixel layout, pitch, scaling, night lighting or sprite rendering require graphics-workstream coordination. This report does not supersede their ownership.
4. **Frame pacing: preserve.** Keep approved 16 FPS tactical simulation and 30 FPS experimental menu cadence; measure wait separately.
5. **Safety gates.** Preserve original explosion visuals, damage and gameplay, legacy modes, mouse/keyboard, campaign-isolated between-mission save requirement. No direct production change is recommended solely from static analysis.

## Verification

Performed: read back all three stated current GitHub C sources; confirmed branches, loops, SDL API call order and local error handling. No source change, compiler run, runtime SDL failure injection, real Windows profiling, render screenshots or gameplay verification. Assembly source not independently re-read in this checkpoint.

Next main audit: trace the SDL surface initialization and lock/recovery contracts and inspect the sound callback versus tactical update timing before specifying a diagnostic Windows build.
