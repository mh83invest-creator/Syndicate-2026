# Syndicate 2026 — Static Rendering, UI, ABI and Test-Gate Audit

**Date:** 2026-11-01 (Brisbane)
**Branch inspected:** `feature/v0.9-visual-showcase`
**Protected baseline:** `main` known-good v0.7, `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Scope:** read-only source investigation. No engine migration. FreeSynd is reference-only. No gameplay source, workflow or retail assets added or changed.
**Status:** Static rendering/input/ABI/test-gate investigation complete **for the named inspected routines**. Whole-source semantic audit, runtime tests, and lighting viability remain incomplete without additional work/evidence.
**Parent audit:** `docs/SOURCE_FORENSIC_AUDIT_2026-10-31.md`.

## Executive assessment

The source confirms **a two-stage legacy rendering arrangement**, not a simple modern 256-colour 2D canvas:
1. In `engine/src/display.h:30-52`, `DrwF_ScreenVres16` is a distinct display flag. The source comments explicitly describe a simulated VGA buffer for Vres16 and state that `WScreen` holds static/scrolling mission elements while sprites are drawn on `VScreen`.
2. `engine/src/main.c:270-273` defaults `DrawFlags` to `DrwF_ScreenVres16`. `main.c:311-319` selects different resource tables for Vres16 versus the alternate display mode.
3. `engine/src/display.c:149-232`, `update_vscreen_whole_vres16`, expands **four separate packed bit planes** into palette-indexed pixel data; the routine loops over 400 rows and 80 bytes per row, yielding 640 horizontal pixel positions per row. It clears 640x480 output storage. This establishes a **640x400 logical, 16-index source path**; it is not the same thing as a native 32-bit RGBA scene.
4. `engine/src/display.c:234-257` blits one of the engine's buffers into the Bullfrog screen and calls `LbScreenSwap`.
5. `engine/bflibrary/src/x86-win-sdl2/sscreen.c:1050-1085` handles the mouse swap hook, SDL draw/screen surface blit and `SDL_UpdateWindowSurface`. `spalette.c:50-99` writes palette colours to the SDL screen and draw surfaces.

**Consequence:** A global darkened palette may also darken agents, HUD, cursors, and intended light sources. Selective glow requires scene segmentation, an overlay stage or indexed-region treatment that has not been demonstrated in the original code. Do not call global palette recolouring equivalent to streetlights/headlights/neon.

## Observed assembly rendering and menu graph

Source: `engine/src/syndre.sx`, Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Lines below are 1-based and calculated from the unmodified, 85,803-line blob. Call lists are **static direct call-site evidence**, not full runtime execution tracing.

| Function / assembly lines | Calls and state established | Safety impact |
| --- | --- | --- |
| `mission_briefing` : 1207–2140 | Calls `draw_brief_tact_map`, `DisplayScreenTitle`, `DrawMenuGadget`, `DrawScreenGadget`, `draw_logo`, `ac_swap_wscreen`, `ac_game_update_menu` and resource/text/audio routines | Briefing layout, buttons, draw-and-input updates share the same loop |
| `draw_brief_tact_map` : 2141–2514 | Calls `rectangle` and an unnamed helper; uses tactical map data passed in | Pure styling cannot be assumed without understanding its inputs |
| `draw_config_menu` : 10925–11247 | Calls `draw_config_menu_box`, `new_show_text`, `LbSpriteDrawMCGA`, `draw_logo`, `DrawScreenGadget`; references scaled mouse coordinates | Company menu style and interactive hit boxes are coupled |
| `draw_config_menu_box` : 11270–11324 | Performs screen copy, swap and mouse-place/remove calls | Palette/background changes and cursor must be coordinated |
| `map_selection` : 31709–32156 | Loads and frees resources, draws menu/map/logo, swaps frames, calls `ac_game_update_menu` | Strategic map rendering and strategic simulation updates share the same loop |
| `DisplayScreenTitle` : 55935–55972 | Calls `GetLineWidth`, `new_show_text` | Text metrics and display are code-driven |
| `DrawMenuGadget` : 56074–56119 | Reads `lbDisplay__MMouseX_640`/`lbDisplay__MMouseY_400` and calls `DrawScreenGadget` | The visible control appearance and mouse-over behaviour are linked |
| `draw_mapwho` : 18029–18715 | References world actors, players, viewport, sprites; calls `DrawASpriteHires16_A`, `set_block_changes`, `__cover_sprite` | Occlusion, redraw markers, and viewport bounds must remain coherent |
| `draw_element` : 19109–19171 | Calls `DrawASpriteHires16_A` with `h_sprites` | Tile/object sprite draw hook, not a confirmed lighting system |
| `full_redraw` : 19285–19350 | Calls `draw_horizontal`, `set_all_changes`, reads viewport/mission map bounds | Scrolling affects redraw work |
| `draw_panel` : 39036–39215 | Draws panels, tactical map, bars, text and changes the mouse pointer | UI must not be dimmed or overdrawn unpredictably |
| `load_map_level` : 27845–27935 | Reads compressed original game data; loads `mission_load_files`; initializes sprites, audio bank, and level state | Level-specific atmosphere initialization requires careful lifecycle mapping |
| `mission_load_files` : 82620–82673 | References original map, `map_buf`, block/colour data, sprite data and frames/animation resources | Tile and animation resources are loaded together |
| `DoPalette` : 61958–62031 | Refers to `PalMem`, calls `ac_LbPaletteSet` | Palette is already game-owned; arbitrary writes can interfere with animation |
| `scroll_map` : 19352–19457 | Converts mouse/keyboard directions into `scroll` calls, with non-DOS edge normalization | Do not change input mapping or direction dispatch |
| `scroll` : 19459–19871 | Reads `player_view_map_x/y`, bounds and calls redraw/change routines | Tied to mission rendering; historical v0.8 regression area |

### Explicit camera edge rules to preserve

The existing non-DOS `scroll_map` code at approximately 19357–19378 maps **x <= 8** to 0; **x >= 630** to 638; **y <= 8** to 0; and **y >= 390** to 398, before keyboard overrides and direction dispatch. The code uses separate x/y registers and direction branches. This is source evidence of the existing boundary translation, **not** approval to rewrite it. The failed v0.8 camera-speed experiment is excluded.

### Timing and ABI bridge

- `engine/src/game.c:142–193`: `game_update_full` processes SDL events and throttles via `SDL_GetTicks`/`SDL_Delay`, unlocking and relocking screen around update processing. **16 and 30 are maximum pacing inputs, not proof of perfectly constant FPS**.
- `engine/src/game.c:205–207`: mission `game_update_full(true,16)`.
- `engine/src/game.c:224–226`: strategic/menu `game_update_full(true,30)`.
- Assembly direct menu-call sites at original lines 2065, 10533, 31936.
- `engine/conf/wrappers_game.conf:236–239` declares the `game_update_menu`, `game_update`, and `swap_wscreen` bridge signatures.
- `engine/src/Makefile.am:44,73–79` builds generated wrapper assembly via `util/mkwrappers` and compiles it with the native assembler.
- **Risk:** changing C declarations without matching wrapper ABI or changing screen lock/event pumping risks runtime faults, not merely compilation errors.

### First-pass v0.9 feasibility classification (no implementation approval)

| Proposed capability | Source-backed judgement | What is still needed |
| --- | --- | --- |
| Native OS window title | Implemented in current v0.9A branch, **not runtime verified** | Windows launch / fullscreen visibility check |
| Existing menu text/colour/selected control appearance | Routines located; **plausible with local changes but input-coupled** | Map exact resource palette and hitboxes; visual screenshot checks |
| Strategic screen layout or fonts | Routines and text metrics located; **medium to high regression risk** | Coordinate correspondence, language, menu action regression |
| Mission colour grading / palette tint | Palette manipulation available; **global tint insufficient for design brief** | 16-index source palette state, source visibility and UI isolation |
| Sprite-specific tint | Library `gspr_map.c` remapping functions exist; integration into mission renderer **unverified** | Sprite consumers, transparency/occlusion and colour-index ownership |
| Per-object streetlights, headlights, neon and windows | **NOT PROVEN** by present source, cannot guarantee feasible as isolated edit | Full composite stage, masking, object positions, lights/scene geometry, original assets |
| Day/dusk/night mission presets | **NOT PROVEN** and not present as a tested feature | Mission selection + transient settings without save format changes |
| Rendering replacement / wholesale game reimplementation | **Out of v0.9 scope** | Explicit separate approval only |

### Test-source inventory and unexecuted CI tests

The library declares **seven** `check_PROGRAMS` tests in `engine/bflibrary/Makefile.am:166–174`: math, memory, polygon, line, trigonometry, sprite draw and triangle. `Makefile.am:291` specifies `TESTS = $(check_PROGRAMS)`, so the intended Automake test action is `make check`. The sprite test in `engine/bflibrary/tests/bflib_test_sprdrw.c` calls `LbSpriteDrawRemap`, uses test sprites and various drawing modes.

The repository's `.github/workflows/windows-source-v0.7.yml` performs a Windows i686 source compilation (`make -C release V=1`), checks DLL imports, packages and uploads artifacts. **It does not invoke `make check`, and no evidence of test execution was found in the workflow.** This is a test-execution gap, not a discovered code defect. A passing build cannot prove sprite remapping, menu hit testing, camera boundary behaviour or lighting readiness.

These test executables have not been built/run in this audit. Running sprite tests might require graphical test setup and fixture PNGs; do not assume completely headless execution or test coverage of the native Syndicate missions.

### Asset-independent checks achievable next

1. On a **separate test-only branch once authorised**, add CI steps to compile and run `make check`, capture stdout/logs/screenshots where practical, and record platform/environment constraints. Preserve current working v0.7 workflow unless deliberately versioning CI.
2. Static scan for other colour tables and unused palette redraw paths to distinguish background tile processing from overlay candidates.
3. Generate a map of direct callers for `draw_element`, `DrawASpriteHires16_A`, `DoPalette`, `draw_panel`, `full_redraw`, and `ac_swap_wscreen`; verify data ownership and draw order.
4. Review exact screen clipping, cursor timing and resolution scaling. Tests can be designed from known constants even before proprietary assets are available.
5. Draft test matrix for 640x400 mission and 320x200 strategic mode; high/low display modes; four edges and corners; mouse focus; diagonal scrolling; audio; map scrolling during explosion-heavy scenes.
6. No source code changes until feasibility is reviewed against the complete static evidence and, later, actual asset/runtime evidence.

### Tests and decision gate

**Passed in this source-only investigation:** GitHub file/tree/blob retrieval; lexical symbol and call-site checks; C source/assembly-wrapper configuration cross-check; identification of 16-index planar unpacking and SDL2 window-surface presentation code; CI vs Automake test declarations.

**Not performed:** C/assembly compile or link; execution of Automake checks; Windows smoke/regression tests; asset format checks against actual retail bytes; visual screenshot/palette inspection; performance profiling. Therefore **the audit cannot yet certify implementation feasibility, compatibility, or correctness**.

**Current decision:** Keep the SyndicatFX engine. Protect v0.7 source, menu cadence, camera, audio, save layout. The FreeSynd reference ZIP stays outside the repository. This file documents static evidence only; further asset-free inspection is possible, and asset-specific/Windows runtime checks remain a later user dependency.
