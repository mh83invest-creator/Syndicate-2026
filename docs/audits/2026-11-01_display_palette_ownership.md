# Syndicate 2026 — Source Audit Stage: C Display/Palette Ownership Contract

Date: 2026-11-01 (Australia/Brisbane)
Status: COMPLETED **focused source-only inspection**; overall forensic audit remains IN PROGRESS.
Branch: `feature/v0.9-visual-showcase`
Protected baseline: `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`
Scope: inspect the actual tracked C display, palette, game-loop and mouse adapter code to tighten the renderer boundary for proposed visual work. No source edits or runtime claims. Prior evidence: `docs/SOURCE_AUDIT_ASSET_FREE_CONTINUATION_2026-11-01.md`, `docs/SOURCE_AUDIT_DECISION_REGISTER_2026-11-01.md`.

## Verified source facts

1. **VGA work allocation and screen setup.** `engine/src/display.c:127–138` allocates `640*480*2+16 = 614416` bytes using `LbMemoryAlloc`; the source does **not** check the returned pointer for NULL. `AppScreenSetup` assigns `lbDisplay.WScreen = VGABuffer + 640*480` *before* calling `LbScreenSetup`. Actual SDL surface ownership thereafter must not be inferred solely from this initial pointer assignment. No allocation failure was simulated.
2. **WScreen and VScreen are different conceptual targets.** `engine/src/display.h` describes mission `WScreen` as containing static/scrolling content, with sprites drawn on `VScreen`. `display.c:update_vscreen_whole_vres16` clears `640*480` bytes starting at `VScreen`, and reads four `400*80` byte planes from `WScreen`, emitting 640x400 4-bit-index values. The output buffer starts where the original assembly sets `VScreen`; this C file alone does not prove the runtime value or lifetime of the assembly pointer.
3. **Presentation path has mode-specific branches.** `display.c:swap_wscreen` snapshots lock state, locks if needed, copies either `640*480` from `VGABuffer` in Vres16 mode or `320*200` from `WScreen` in MCGA mode, unlocks, invokes `LbScreenSwap`, then attempts to relock if it was previously locked. The relock is a potentially unbounded busy loop on repeated failure; that is a conditional source risk, **not a demonstrated hang**.
4. **Sprite draw temporarily redirects shared display state.** `display.c:ApSpriteDrawMCGA` saves `lbDisplay.WScreen`, temporarily assigns `WScreen`, draws at half-resolution coordinates, and restores the saved pointer. Any new overlay must not assume the shared library display pointer always means the same backing image.
5. **Palette setter ownership and colour depth.** `engine/bflibrary/src/x86-win-sdl2/spalette.c:LbPaletteSet` copies 256 RGB triplets into `lbPalette`, masking components to 6 bits (`& 0x3F`) and producing SDL RGB values via `<<2`. It explicitly permits `srcColors == bufColors` (in-place copy), but only checks whether source and destination pointers are null; it does not validate actual input allocation length. It sets SDL palette colours on 8-bit output surfaces as applicable and points `lbDisplay.Palette` to `lbPalette`. This makes the library's `lbPalette` a concrete palette state destination; it **does not establish** that game assembly `GraphicsPalette`, `PalMem` and `WScreen` refer to the same bytes.
6. **Input offset is tied to mission presentation.** `engine/src/mouse.c:mouse_update_scaled_coords` subtracts 40 from mouse Y in Vres16 mode and caps one scaled moving-coordinate Y at 399. The alternative mode doubles coordinates. A scene offset change would require verified mouse mapping changes; do not adjust either path in an audit.
7. **SDL event pumping is coupled to the pacing bridge.** `engine/src/game.c:game_update_full` pumps SDL events and updates scaled mouse coordinates before frame waiting and again within delays. `game_update()` targets 16 FPS and `game_update_menu()` targets 30 FPS; both use a shared static `last_frame_ticks`. This is a timing interaction requiring regression tests if any frame boundary is moved; **no issue has been demonstrated**.
8. **Existing v0.9 source delta only changes a native window title.** `game.c` calls `LbSetTitle("Syndicate 2026")`; this does not constitute an in-game title screen or lighting implementation. It remains uncompiled/unplaytested.

## Implications for approved visuals

- A scene-only 8-bit colour manipulation must locate the **actual completed composited scene surface** and isolate mouse pointer, HUD, strategic screens, palette cycling, 16-colour indices and original dirty-region semantics. Do not treat `lbPalette`, `GraphicsPalette` or `VScreen` as interchangeable.
- A 4-bit planar input is insufficient to identify lamp/window/object boundaries from colour index alone. Headlight cones, window emission and occlusion require object/map data or an explicitly validated compositing solution.
- No new graphics implementation route is selected in this stage.

## Verification ledger

| Check | Result | Evidence |
|---|---|---|
| GitHub read of `display.c`, `display.h`, `spalette.c`, `mouse.c`, `game.c` | PASS | Connector returned exact files on v0.9 branch |
| Cross-check buffer sizes, pointer assignments, palette conversion and mode branches | PASS (static source) | Expressions and code paths in the five files |
| Native compile / automated tests / sanitizer / runtime pixel output | NOT RUN | No toolchain or executable invoked |
| Windows menu, scroll, focus and mission playtests | NOT RUN | Original playable Windows run not reproduced |
| Global source audit / proven lighting feasibility | NOT COMPLETE | Additional original assembly and retail-asset tests required |

## Remaining work and exact next step

1. Trace original assembly assignments/callers for `WScreen`, `VScreen`, `GraphicsPalette`, `PalMem` across mission, title and transitions (begin with `engine/src/syndre.sx` at address/line sites recorded in `docs/SOURCE_AUDIT_ASSET_FREE_CONTINUATION_2026-11-01.md`).
2. Trace Bullfrog SDL screen setup/reset/recreation code and the SDL screen/draw surface pointer lifetime, including failed locks and unusual destination pixel formats; distinguish static possibilities from actual tested crash conditions.
3. Map final scene/pointer/HUD layering with actual original game assets and Windows captures when available; this is a later runtime gate.
4. Preserve v0.7; make no engine change until a separately scoped and documented experiment is approved.

Rollback: documentation only on feature branch; `main` unchanged. This report is a completed **focused audit stage**, not completion of the full forensic audit.
