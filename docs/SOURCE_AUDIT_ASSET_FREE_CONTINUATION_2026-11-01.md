# Syndicate 2026 — Asset-independent Forensic Audit: Frame Lifecycle, Palettes, Mouse/Focus and Regression Gates

**Date:** 2026-11-01 (Australia/Brisbane)
**Repository/branch:** `mh83invest-creator/Syndicate-2026` / `feature/v0.9-visual-showcase`
**Protected known-good:** `main` source rooted at commit `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Status:** Read-only forensic inspection; **asset-independent architecture conclusions are source-based, not runtime-tested**. Full 386-file semantic audit or working lighting proof is not claimed.
**Companion documents:** `docs/SOURCE_FORENSIC_AUDIT_2026-10-31.md`, `docs/SOURCE_AUDIT_RENDERING_AND_TEST_GATES_2026-11-01.md`, and `docs/ASM_ADDRESS_LABEL_INDEX_2026-11-01.md`.
**Source evidence:** Current branch file paths + exact 1-based source line numbers; original assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Other source file locations confirmed by GitHub direct reads.
**Policy:** No code, workflow, save-data, retail resource or dependency modifications during audit. Keep FreeSynd outside project as reference only.

## 1. Critical new observation: real mission frame order

The **actual original game loop** is visible in `engine/src/syndre.sx:198–303`, specifically inside `syndicate`:

| Assembly lines | Actual call / action |
| --- | --- |
| 203–210 | `set_default_player`, `select`, `reset_mission_info`, `initialise_player` |
| 220–230 | `process_players_turn`, network/single-player branches, `process_computer_players`, `move_it` |
| 231–240 | `correct_buttons`, audio/status, `check_for_danger`, `process_day`, `check_end_level` |
| 241–243 | `ac_LbScreenWaitVbi`, `swap_screen_vres16` |
| 244–249 | Conditional palette update every selected cycle using `GraphicsPalette` |
| **251–266** | **Two sequential `scroll_map` calls** with scaled mouse X/Y inputs |
| 267–270 | `copy_back`, `draw_mapwho`, `draw_panel`, `click_map` |
| 271–273 | Non-DOS `ac_game_update` (16 FPS target C bridge) |
| 274–279 | Loop counter/state test and repeat |
| 280–297 | Mission exit/music shutdown, level free, agent transfer and result |

**Implication:** Frame presentation occurs *before* the subsequent camera/redraw/entity/HUD calls in this loop. A lighting layer placed blindly inside the SDL presentation function would need to correctly distinguish prior completed mission content from other screens. A layer placed after sprite drawing has to handle cursor/HUD, future clearing, palette and possible frame-order latency. Neither overlay placement is approved without further observation.

**Camera:** The twice-per-loop `scroll_map` invocations are baseline behaviour, not a bug confirmed by this audit. They must not be arbitrarily consolidated for "cleaner code" or to adjust speed. The failed v0.8 experiment is **not** imported.

## 2. The 4-plane mission image is NOT the final 8-bit screen

- `engine/src/display.h:30–54`: `DrwF_ScreenVres16` and explicit `WScreen` versus `VScreen` responsibilities. Mission `WScreen` holds static/scrolling elements; sprites draw on `VScreen`.
- `engine/src/main.c:270–273`: game defaults to Vres16 mode; command-line options can select alternate display modes (`main.c:211–218`).
- `engine/src/display.c:127–131`: allocated VGA work buffer, 640*480*2+16 bytes for source/output roles.
- `engine/src/display.c:149–231`: `update_vscreen_whole_vres16` expands **four bit planes** over a 640×400 logical area (400 rows × 80 bytes/plane/row) into an output buffer of palette indices. This 4-bit source can represent 16 indexed colours; do **not** infer unrestricted independent 24-bit pixel colours.
- `engine/src/syndre.sx:17894–17974`: `swap_screen_vres16` handles cursor placement, redraw-change markers, and calls `ac_update_vscreen_whole_vres16` then `ac_swap_wscreen` on the non-DOS path.
- `engine/src/display.c:234–260`: `swap_wscreen` copies output to the Bullfrog screen and invokes `LbScreenSwap`, preserving/restoring the lock state.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:976–1006,1050–1085`: software-surface scaling/blit and `SDL_UpdateWindowSurface`; `LbMouseOnBeginSwap`/`LbMouseOnEndSwap` wrap presentation.
- `engine/bflibrary/src/x86-win-sdl2/spalette.c:50–99`: screen and draw palette mapped through `SDL_SetPaletteColors`.

**Lighting consequence:** A renderer-neutral post-process *might* be possible by operating on a final composited surface, but scene-specific bright windows, vehicle lamps, occlusion and UI masking require scene metadata and correct ordering. No such implementation or performance result exists. A palette-only edit changes indexed colours globally and is **not** equivalent to localized light sources. A real 8-bit remap API does not mean the 4-plane mission renderer can directly use that API.

### Explicit per-level palette selection already exists

In `engine/src/syndre.sx:27768–27793`, `init_level_data` reads `current_levno`, indexes `level_palettes` (table at 82677), formats a `data/hpal%02d.dat` file name, reads that palette asset and calls the palette-setting pathway. Multiplayer takes a separate `data/hpal01.dat` route. **This proves per-level palette *selection*, NOT support for per-light/per-pixel illumination.** The numeric table repeats palette numbers; it is not one unique night palette per mission.

`engine/src/syndre.sx:82620–82673` also explicitly references the mission load table: `hfnt01.dat`, map, `col01.dat`, `hblk01.dat`, `hspr-0.dat/.tab`, `hfra-0.ani`, `hele-0.ani`, `hsta-0.ani`, plus sound bank. Actual file bytes and format compatibility remain untested.

**Risk:** `init_level_data` uses multiple buffer pointers around palette loading and surface clearing. The full alias relationship between `GraphicsPalette`, `WScreen`, `PalMem` and SDL backing surfaces must be resolved before editing palette writes. Do not assume those pointers are interchangeable from a high-level name.

## 3. Original UI / title screen is not the operating-system window title

- `engine/src/syndre.sx:54149+`, `select`: loads menu sprite/resource tables and sets up menu input, display mode and title data; `select` is called from the main game loop.
- `engine/src/syndre.sx:54274–54294`: references original `data/mtitle.dat` filename, calls `AniPlay`, then `ac_swap_wscreen` and `ac_game_update`. This is the legacy title animation/display path, distinct from the OS window title in C.
- `engine/src/syndre.sx:10925–11247`: `draw_config_menu` reads mouse position and calls sprite/text/gadget drawing.
- `engine/src/syndre.sx:56074–56119`: `DrawMenuGadget` reads `lbDisplay__MMouseX_640`/`MMouseY_400`.
- `engine/src/syndre.sx:31709–32156`: strategic map drawing resource / update loop; `mission_briefing` at 1207–2140 has a comparable interleaved draw/input/update loop.
- `engine/src/main.c:87–103` defines a DOS-era external `run_intro` command helper; startup calls it at line 268. This source call exists even though the previously agreed v0.7 user-observed intro is disabled. **Do not assert the external intro actually plays**, and do not restore it or alter the path without a separate explicit request. The original title animation inside `select` is a distinct matter.

**Regression gate:** Changing text sizes, sprite positions or menu framing without changing the *matching* logical interaction rectangles will break hover/click behaviour. Preserve menu navigation, selected button state, keyboard input, briefing progression, mission selection and language variations.

## 4. Mouse and window focus are a three-layer problem

1. `engine/bflibrary/src/x86-win-sdl2/swindows.c:171–191` pumps up to **256** SDL events per call and processes idle handlers; `swindows.c:104–113` handles focus gained/lost, updates `lbAppActive`, resets input state and refreshes the window surface path.
2. `engine/bflibrary/src/x86-win-sdl2/smouse.cpp:260–318` maps native mouse coordinates and screen scaling. It has an auto-reset/warp path, but **`engine/src/main.c:326` sets `lbMouseAutoReset = false`**, so it would be incorrect to blame the auto-reset branch for the observed v0.8 focus bug without runtime evidence.
3. `engine/src/game.c:95–102` calls the SDL event pump and subsequently `mouse_update_scaled_coords`. `engine/src/mouse.c:31–45` uses a 40-pixel vertical offset for Vres16 but doubles coordinates in the alternate mode, then clamps logical mouse Y.
4. `engine/src/syndre.sx:19352–19457` translates logical input into edge scroll directions; in the non-DOS path it normalizes x <= 8 / x >= 630 and y <= 8 / y >= 390 to extreme direction constants.
5. `engine/src/syndre.sx:19459–19871` then changes viewport state and triggers redraw/change tracking; `full_redraw` calls `draw_horizontal` and `set_all_changes`.

**Result:** The historical desktop focus switch and top-edge veer may involve any of SDL native input, surface/window mode, coordinate scaling, game camera dispatch, and timing. **No root cause established.** Preserve v0.7 and do not resurrect v0.8 code.

## 5. Additional gameplay/save compatibility boundary

`engine/src/game_data.c:141–190` saves fixed ordered binary structures: players, country state, weapons, cyborg modifications, research, selected team and a description. The code includes explicit size assertions for several structures (`game_data.c:146–149`). Notably `save_player` adjusts credits by 10% when selected-player credits exceed 30,000 (`game_data.c:173–175`), **before** writing the save. This is source-confirmed gameplay behaviour, whether intended or not. Audit does not authorise changing it.

**Consequences:** Never insert atmosphere/night metadata directly into these structs or rearrange save calls without format migration, backwards compatibility tests and explicit design approval. Do not "fix" save-credit behaviour incidentally.

## 6. Baseline preservation verified via Git blob comparisons

Compared exact Git blob SHAs on `main` and `feature/v0.9-visual-showcase`:

| Tracked file | Main vs audit branch |
| --- | --- |
| `engine/src/syndre.sx` | IDENTICAL |
| `engine/src/display.c` | IDENTICAL |
| `engine/src/mouse.c` | IDENTICAL |
| `engine/src/game_data.c` | IDENTICAL |
| `engine/bflibrary/src/x86-win-sdl2/sscreen.c` | IDENTICAL |
| `engine/bflibrary/src/x86-win-sdl2/spalette.c` | IDENTICAL |
| `.github/workflows/windows-source-v0.7.yml` | IDENTICAL |
| `engine/src/game.c` | **One line differs**: line 85 `LbSetTitle(PACKAGE_NAME);` on `main` versus `LbSetTitle("Syndicate 2026");` on branch. This remains **uncompiled/unverified**. |

Thus the previously failed v0.8 speed experiment is not present in the compared assembly and camera path; this is stronger source evidence than the handover alone. This spot comparison is **not** a full tree-level equality proof.

## 7. Audit indexing and automated-test execution gap

A separate generated **lexical** source index `docs/ASM_ADDRESS_LABEL_INDEX_2026-11-01.md` covers **1,617 address-annotated original assembly labels** (515 code-like, 1,077 data-like, 25 unclear by heuristic). Includes original line/offset and a count of nearby direct `call` text. **Labels are NOT equivalent to individual functions; local labels, tables and vtables can divide functions.** The index is a navigation aid only, not complete control-flow reconstruction.

`engine/bflibrary/Makefile.am:166–174` declares seven test executables and line 291 sets `TESTS = $(check_PROGRAMS)`. The sprite test at `engine/bflibrary/tests/bflib_test_sprdrw.c:331–503` generates sprite inputs and calls the remap routines. Existing v0.7 Windows workflow only builds/packaged the game, checks native MinGW DLL linkage, uploads artifacts, **without invoking `make check`**. No tests were executed in this audit. The Linux/macOS upstream workflows are not evidence of our Windows v0.9 branch having passed.

**Next test-only action if authorised later:** A separate non-regression CI variant can run `make check` with its reference fixtures, save logs and inspect failures. It must not overwrite the known-good workflow or claim real gameplay verification.

## 8. Feasibility and *not-yet-tested* implementation routes

| v0.9 goal | Feasibility status | Risk / evidence still required |
| --- | --- | --- |
| Native title string | Code-level change exists; no Windows build | Low code scope, unknown visible effect in fullscreen |
| In-game title presentation | Original `AniPlay` / title asset route located | Needs asset inspection, visual screenshot, interaction tests |
| Menu framing/selection/text | Sprite/text/gadget routines located | Medium–high: hard-coded mouse rectangles and multiple languages |
| Atmosphere using per-level palette assets | Existing `level_palettes` / `hpalXX.dat` selection confirmed | **Possible exploration**, not approved; 16-colour legibility and HUD/sprites affected |
| Localized headlight/streetlight/neon and lit windows | No demonstrated current system | **Unproven**, likely substantial render-layer / masking work; needs original scene composition and tests |
| Post-composite scene-only processing | Technically an architectural *candidate*, not implementation | Must isolate HUD/menus/cursor and preserve screen lock/event/pacing |
| Higher resolution or replacing 16-colour mission renderer | Not in current approved scope | High risk; separate design and baseline approval |
| Explosion-heavy performance fixes | Deferred to separate milestone | Requires profiling to separate game simulation from render/palette/blit costs |

**No lighting model, overlays, asset modifications, or extra features have been implemented.** Keep SyndicatFX as foundation, FreeSynd as read-only reference, 16 FPS mission and 30 FPS strategic menu cadence, original audio, input and save layout intact.

## 9. Remaining dependencies and acceptance criteria

**Possible without retail files:** further tracing of C/assembly symbols and data lifetimes, build/ABI/header audit, exact palette buffer ownership investigation, test-fixture/code inspection, third-party dependency and licence reviews, and design of minimal non-regression test plans. They have not all been completed.

**Requires game assets and/or actual Windows playback:** verify map/sprite/palette bytes; visually demonstrate title/menu styling; prove selective lighting and emissive source placement; check map occlusion, UI brightness and mission accessibility; measure FPS/frametime in explosion-heavy scenes; verify focus, corner scrolling and audio through a real playable build. Assets must be private and legally obtained; no original retail game data belongs in public GitHub.

**Audit completion gate:** before resuming implementation, a reviewer should be able to map each planned effect to an exact data source, C/assembly draw/palette hook, buffer and ordering expectation, build/test regression plan, and a working Windows test run. Currently **this gate is NOT PASSED**.

## Handover

1. Read this report, the original forensic audit, v0.9 roadmap/handover and the assembly index.
2. Do not overwrite the v0.7 baseline or bring in v0.8 camera changes. No new code on `feature/v0.9-visual-showcase` until explicitly authorised.
3. Continue the missing buffer alias and original asset-format investigations. Use FreeSynd only for independent documentation of original data semantics.
4. When retail assets become available later, reproduce the known-good v0.7 Windows build and capture baseline mission/menu rendering and mouse/scroll behaviour before testing effects.
5. Record each verified discovery with exact path, line/symbol and evidence. Separate source reads, compilation, automated tests and actual gameplay.

## 10. Additional buffer and palette allocation evidence (continuation, same audit)

Original assembly `engine/src/syndre.sx:83626–83676` defines separate initial resource-load tables:

- `load_files_vres16` allocates a `WScreen` work buffer (declared size `256048`), loads `data/hpal01.dat` through the separately declared `GraphicsPalette` pointer, and loads `hpointer.tab`, `hpointer.dat`, `hreq.dat`.
- `load_files_mcga` also has a `WScreen` allocation, loads `data/lpal01.dat` through `GraphicsPalette`, and separate low-resolution pointer and other assets. This establishes **distinct high/low mode palette and pointer resources**, not just scaling of the same title art.
- `engine/src/syndre.sx:83901–83910` declares **separate pointer slots** for `WScreen`, `VScreen`, `GraphicsPalette`. `PalMem` is another separate pointer slot at line 85440. Their runtime values and intended contents are not interchangeable merely because they are pointer slots.
- `engine/src/syndre.sx:27768–27793` reads per-level `hpal` data into `GraphicsPalette`, yet the next `ac_LbPaletteSet` receives `WScreen` after a clear operation. The main mission loop conditionally calls `ac_LbPaletteSet(GraphicsPalette)` later at lines 244–249. This **may** serve a screen/palette transition (e.g. black then restored), but no runtime proof was made. **Do not "fix" this apparent mismatch without establishing exact buffers and startup appearance.**
- `engine/src/syndre.sx:54253–54261` shows a different title/menu phase assigning `PalMem` from a calculated offset of `WScreen`. This demonstrates that allocation lifetime/aliasing may vary by screen phase. A generic palette pointer hook is not safe without checking current mode and allocation.

**Cost of the existing converter (static count, not benchmark):** `engine/src/display.c:149–231` iterates 400 rows × 80 bytes for each of 4 bit planes, performing 8 indexed byte operations for each input byte. That is **128,000 source bytes and about 1,024,000 pixel-index update operations per full unpack call**, before copy/scaling/presentation. Actual CPU time, rate and bottleneck impact are **not measured**; heavy explosions may be dominated by other logic/drawing. A lookup-table or SIMD proposal would require exact output equivalence tests and separate approval.

### Review implication

This direct pointer/resource evidence strengthens the recommendation to **avoid palette injection at the general SDL screen-swap function** as a first v0.9B change. Source-level candidates can be investigated, but the first safe decision point is understanding actual data aliases and exactly which pixels belong to a mission versus menu/HUD/cursor at each stage. No original asset samples or Windows render traces are available yet.
